//--------------------------------------------------------------------------------
// Copyright (c) 2012, Secular Wheel
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//	- Redistributions of source code must retain the above copyright notice, 
//	  this list of conditions and the following disclaimer.
//	- Redistributions in binary form must reproduce the above copyright notice, this
//	  list of conditions and the following disclaimer in the documentation and/or other 
//	  materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//--------------------------------------------------------------------------------

#include "../include/jpx_dec.h"
#include <stdio.h>

jpx_uint32				jpx_filestream_getsize(jpx_streamp stream);
jpx_uint32				jpx_filestream_getpos(jpx_streamp stream);
jpx_bool				jpx_filestream_seekpos(jpx_streamp stream, jpx_uint32 pos);
jpx_bool				jpx_filestream_readat(jpx_streamp stream, jpx_uint32 pos, jpx_voidp buf, jpx_uint32 len);
jpx_bool				jpx_filestream_read(jpx_streamp stream, jpx_voidp buf, jpx_uint32 len);
void					jpx_filestream_destroy(jpx_streamp stream);

typedef struct jpx_filestream_struct {
	jpx_stream				pub;
	jpx_memmgrp				memmgr;
	FILE*					file;
} jpx_filestream;
typedef jpx_filestream FAR * jpx_filestreamp;
						
static jpx_uint32		jpx_filestream_getsize(jpx_streamp stream)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	jpx_uint32 pos = ftell(filestream->file), size;
	fseek(filestream->file, 0, SEEK_END);
	size = ftell(filestream->file);
	fseek(filestream->file, pos, SEEK_SET);
	return size;
}
static jpx_uint32		jpx_filestream_getpos(jpx_streamp stream)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	return (jpx_uint32)ftell(filestream->file);
}
static jpx_bool			jpx_filestream_seekpos(jpx_streamp stream, jpx_uint32 pos)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	return fseek(filestream->file, pos, SEEK_SET)==0;
}
static jpx_bool			jpx_filestream_readat(jpx_streamp stream, jpx_uint32 pos, jpx_voidp buf, jpx_uint32 len)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	jpx_uint32 savepos = jpx_filestream_getpos(stream);
	jpx_bool ret = jpx_false;

	if (!jpx_filestream_seekpos(stream, pos)) return jpx_false;
	ret = jpx_filestream_read(stream, buf, len);
	
	// Restore pos after read
	jpx_filestream_seekpos(stream, savepos);
	return ret;
}
static jpx_bool			jpx_filestream_read(jpx_streamp stream, jpx_voidp buf, jpx_uint32 len)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	return fread(buf, 1, len, filestream->file)==(jpx_uint32)len;
}
static void				jpx_filestream_destroy(jpx_streamp stream)
{
	jpx_filestreamp filestream = (jpx_filestreamp)stream;
	if (filestream->file) fclose(filestream->file);
	filestream->memmgr->free_func(filestream->memmgr, stream);
}

jpx_streamp				jpx_create_filestream(jpx_memmgrp memmgr, jpx_const_charp filename)
{
	FILE *file = NULL;
	jpx_filestreamp filestream = NULL;

	if (filename == NULL || filename[0] == '\0') return NULL;
	file = fopen(filename, "rb");
	if (file == NULL) return NULL;
	
	filestream = (jpx_filestreamp)memmgr->alloc_func(memmgr, sizeof(jpx_filestream));
	filestream->pub.getsize_func = jpx_filestream_getsize;
	filestream->pub.getpos_func = jpx_filestream_getpos;
	filestream->pub.seekpos_func = jpx_filestream_seekpos;
	filestream->pub.read_func = jpx_filestream_read;
	filestream->pub.destroy_func = jpx_filestream_destroy;
	filestream->memmgr = memmgr;
	filestream->file = file;
	return (jpx_streamp)filestream;
}



jpx_uint32				jpx_memstream_getsize(jpx_streamp stream);
jpx_uint32				jpx_memstream_getpos(jpx_streamp stream);
jpx_bool				jpx_memstream_seekpos(jpx_streamp stream, jpx_uint32 pos);
jpx_bool				jpx_memstream_readat(jpx_streamp stream, jpx_uint32 pos, jpx_voidp buf, jpx_uint32 len);
jpx_bool				jpx_memstream_read(jpx_streamp stream, jpx_voidp buf, jpx_uint32 len);
void					jpx_memstream_destroy(jpx_streamp stream);

typedef struct jpx_memstream_struct {
	jpx_stream				pub;
	jpx_memmgrp				memmgr;
	jpx_bytep				buf;
	jpx_uint32				size;
	jpx_uint32				pos;
} jpx_memstream;
typedef jpx_memstream FAR * jpx_memstreamp;
						
static jpx_uint32		jpx_memstream_getsize(jpx_streamp stream)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	return memstream->size;
}
static jpx_uint32		jpx_memstream_getpos(jpx_streamp stream)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	return memstream->pos;
}
static jpx_bool			jpx_memstream_seekpos(jpx_streamp stream, jpx_uint32 pos)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	memstream->pos = pos;
	return jpx_true;
}
static jpx_bool			jpx_memstream_readat(jpx_streamp stream, jpx_uint32 pos, jpx_voidp buf, jpx_uint32 len)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	if (pos+len > memstream->size) return jpx_false;
	jpx_memcpy(buf, memstream->buf+pos, len);
	return jpx_true;
}
static jpx_bool			jpx_memstream_read(jpx_streamp stream, jpx_voidp buf, jpx_uint32 len)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	if (memstream->pos+len > memstream->size) return jpx_false;
	jpx_memcpy(buf, memstream->buf+memstream->pos, len);
	memstream->pos += len;
	return jpx_true;
}
static void				jpx_memstream_destroy(jpx_streamp stream)
{
	jpx_memstreamp memstream = (jpx_memstreamp)stream;
	memstream->memmgr->free_func(memstream->memmgr, stream);
}

jpx_streamp				jpx_create_memstream(jpx_memmgrp memmgr, jpx_bytep buf, jpx_uint32 size)
{	
	jpx_memstreamp memstream = (jpx_memstreamp)memmgr->alloc_func(memmgr, sizeof(jpx_memstream));
	memstream->pub.getsize_func = jpx_memstream_getsize;
	memstream->pub.getpos_func = jpx_memstream_getpos;
	memstream->pub.seekpos_func = jpx_memstream_seekpos;
	memstream->pub.read_func = jpx_memstream_read;
	memstream->pub.destroy_func = jpx_memstream_destroy;
	memstream->memmgr = memmgr;
	memstream->buf = buf;
	memstream->size = size;
	memstream->pos = 0;
	return (jpx_streamp)memstream;
}
