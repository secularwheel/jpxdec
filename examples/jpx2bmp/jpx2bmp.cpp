#include <windows.h>
#include <stdio.h>
extern "C" {
	#include "../../include/jpx_dec.h"
}

jpx_bool writeBmp(char *filename, jpx_uint32 width, jpx_uint32 height, jpx_uint32 linebytes, jpx_bytep imagebuf)
{
	FILE *fp;
	BITMAPFILEHEADER hdr;
	BITMAPINFOHEADER bih;

	fp = fopen(filename, "wb");
	if (fp == 0) {
		return jpx_false;
	}

	bih.biBitCount = 24;
	bih.biWidth = width;
	bih.biHeight = height;
	bih.biSize = 40;
	bih.biCompression = BI_RGB;
	bih.biPlanes = 1;
	bih.biSizeImage = height*linebytes;

	hdr.bfType = 0x4d42;
	hdr.bfSize = 14 + bih.biSizeImage + sizeof(BITMAPINFOHEADER);
	hdr.bfReserved1 = hdr.bfReserved2 = 0;
	hdr.bfOffBits = 14 + sizeof(BITMAPINFOHEADER);

	fwrite(&hdr, sizeof(hdr), 1, fp);
	fwrite(&bih, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(imagebuf, bih.biSizeImage, 1, fp);
	fclose(fp);

	return jpx_true;
}

int main(int argc, char* argv[])
{
	if (argc < 3) {
		printf("Usage: jpc2bmp.exe src.jp2 dst.bmp\n");
		return 0;
	}

	char *jpcfile = argv[1];
	char *bmpfile = argv[2];
	if (jpcfile==NULL || jpcfile[0]=='\0' || bmpfile==NULL || bmpfile[0]=='\0') {
		printf("Usage: jpc2bmp.exe src.jp2 dst.bmp\n");
		return 0;
	}

	jpx_memmgrp memmgr = jpx_create_default_memmgr();
	jpx_streamp stream = jpx_create_filestream(memmgr, jpcfile);
	if (stream==NULL) {
		printf("The source file couldn't be opened.");
		memmgr->destroy_func(memmgr);
		return -1;
	}

	if (jpx_is_jpc(stream)) {
		jpc_decp jpcdec = jpc_dec_create(memmgr, stream);
		if (!jpc_dec_parse(jpcdec)) {
			printf("Parsing JPEG2000 Codestream error");
			jpc_dec_destroy(jpcdec);
			stream->destroy_func(stream);
			memmgr->destroy_func(memmgr);
			return -1;
		}
		
		jpx_uint16 componentnum = jpc_dec_get_componentnum(jpcdec);
		jpx_uint32 width, height;
		jpc_dec_get_imagesize(jpcdec, &width, &height);
		
		jpx_uint32 linebytes = (width*3+3)/4*4;
		jpx_bytep imagebuf = (jpx_bytep)malloc(height*linebytes);
		jpx_int32 offsets[3] = { 2, 1, 0 };
		
		jpc_dec_init(jpcdec, componentnum, offsets, 3);
		for (jpx_uint32 y=0; y<height; y++) {
			jpc_dec_scanline(jpcdec, imagebuf+y*linebytes);
		}
		jpc_dec_finalize(jpcdec);
		
		if (componentnum == 1) {
			for (jpx_uint32 j = 0; j < height; j++) {
				jpx_bytep line = imagebuf + j*linebytes;
				jpx_bytep p = line;
				for (jpx_uint32 i = 0; i < width; i++) {
					*p = *(p + 2);
					*(p + 1) = *p;
					p += 3;
				}
			}
		}
		
		jpx_bytep linebuf = (jpx_bytep)malloc(linebytes);
		for (jpx_uint32 j = 0; j < height/2; j++) {
			memcpy(linebuf, imagebuf+j*linebytes, linebytes);
			memcpy(imagebuf+j*linebytes, imagebuf+(height-j-1)*linebytes, linebytes);
			memcpy(imagebuf+(height-j-1)*linebytes, linebuf, linebytes);
		}
		
		if (!writeBmp(bmpfile, width, height, linebytes, imagebuf)) {
			printf("Save to bmp failed.");
			free(linebuf);
			free(imagebuf);
			jpc_dec_destroy(jpcdec);
			stream->destroy_func(stream);
			memmgr->destroy_func(memmgr);
			return -1;
		}
		
		free(linebuf);
		free(imagebuf);
		jpc_dec_destroy(jpcdec);
	} else if (jpx_is_jp2(stream)) {
		jp2_decp jp2dec = jp2_dec_create(memmgr, stream);
		if (!jp2_dec_parse(jp2dec)) {
			printf("Parsing JP2 file error");
			jp2_dec_destroy(jp2dec);
			stream->destroy_func(stream);
			memmgr->destroy_func(memmgr);
			return -1;
		}
		
		jpx_uint32 width, height;
		jp2_dec_get_imagesize(jp2dec, &width, &height);

		// set output to rgb
		jp2_dec_set_convert2rgb(jp2dec, jpx_true);
		
		jpx_uint32 linebytes = (width*3+3)/4*4;
		jpx_bytep imagebuf = (jpx_bytep)malloc(height*linebytes);
		jpx_int32 offsets[3] = { 2, 1, 0 };

		jp2_dec_init(jp2dec, 3, offsets, 3);
		for (jpx_uint32 y=0; y<height; y++) {
			jp2_dec_scanline(jp2dec, imagebuf+y*linebytes);
		}
		jp2_dec_finalize(jp2dec);

		jpx_bytep linebuf = (jpx_bytep)malloc(linebytes);
		for (jpx_uint32 j = 0; j < height/2; j++) {
			memcpy(linebuf, imagebuf+j*linebytes, linebytes);
			memcpy(imagebuf+j*linebytes, imagebuf+(height-j-1)*linebytes, linebytes);
			memcpy(imagebuf+(height-j-1)*linebytes, linebuf, linebytes);
		}
		
		if (!writeBmp(bmpfile, width, height, linebytes, imagebuf)) {
			printf("Save to bmp failed.");
			free(linebuf);
			free(imagebuf);
			jp2_dec_destroy(jp2dec);
			stream->destroy_func(stream);
			memmgr->destroy_func(memmgr);
			return -1;
		}
		
		free(linebuf);
		free(imagebuf);
		jp2_dec_destroy(jp2dec);
	} else {
		printf("It's not a jp2/j2c/jpc file.");
	}

	stream->destroy_func(stream);
	memmgr->destroy_func(memmgr);
	return 0;
}

