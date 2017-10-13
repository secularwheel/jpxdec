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

#ifndef _JP2_DEC_H_
#define _JP2_DEC_H_

//--------------------------------------------------------------------------------
//			JP2 Box defines
//--------------------------------------------------------------------------------
#define	JP2_BOX_JP			0x6A502020	// JPEG 2000 Signature box ('jP\040\040')
#define JP2_BOX_FTYP		0x66747970	// File Type box ('ftyp')
#define	JP2_BOX_JP2H		0x6A703268	// JP2 Header box ('jp2h')
#define	JP2_BOX_IHDR		0x69686472	// Image Header box ('ihdr')
#define	JP2_BOX_BPCC		0x62706363	// Bits Per Component box ('bpcc')
#define	JP2_BOX_COLR		0x636F6C72	// Color Specification box ('colr')
#define	JP2_BOX_PCLR		0x70636C72	// Palette box ('pclr')
#define	JP2_BOX_CMAP		0x636D6170	// Component Mapping box ('cmap')
#define	JP2_BOX_CDEF		0x63646566	// Channel Definition box ('cdef')
#define	JP2_BOX_RES			0x72657320	// Resolution box ('res\040')
#define	JP2_BOX_RESC		0x72657363	// Capture Resolution box ('resc')
#define	JP2_BOX_RESD		0x72657364	// Default Display Resolution box ('resd')
#define	JP2_BOX_JP2C		0x6A703263	// Contiguous Code Stream box ('jp2c')
#define	JP2_BOX_JP2I		0x6A703269	// Intellectual Property box ('jp2i')
#define	JP2_BOX_XML			0x786D6C20	// XML box ('xml\040')
#define	JP2_BOX_UUID		0x75756964	// UUID box ('uuid')
#define	JP2_BOX_UINF		0x75696E66	// UUID Info box ('uinf')
#define	JP2_BOX_ULST		0x75637374	// UUID List box ('ulst')
#define	JP2_BOX_URL			0x75726C20	// URL box ('url\040')

typedef struct jp2_dec_struct FAR * jp2_decp;

// JP2 box header
typedef struct jp2_box_header_struct {
	jpx_uint32				type;
	jpx_uint32				len;
} jp2_box_header;
typedef jp2_box_header FAR * jp2_box_headerp;

// File Type box
typedef struct jp2_box_ftyp_struct {
	jpx_uint32				br;		// Brand.
	jpx_uint32				minv;	// Minor version.
	jpx_uint32p				cls;	// Compatibility list.
	jpx_uint32				clnum;	// Number of Compatibilities.
} jp2_box_ftyp;
typedef jp2_box_ftyp FAR * jp2_box_ftypp;

jp2_box_ftypp			jp2_box_ftyp_create(jp2_decp jp2dec);
void					jp2_box_ftyp_destroy(jp2_decp jp2dec, jp2_box_ftypp ftyp);

// Image Header box
typedef struct jp2_box_ihdr_struct {
	jpx_uint32				height;	// Image area height.
	jpx_uint32				width;	// Image area width.
	jpx_uint16				nc;		// Number of components.
	jpx_byte				bpc;	// Bits per component.
	jpx_byte				c;		// Compression type.
	jpx_byte				unkc;	// Colourspace Unknown.
	jpx_byte				ipr;	// Intellectual Property.
} jp2_box_ihdr;
typedef jp2_box_ihdr FAR * jp2_box_ihdrp;

jp2_box_ihdrp			jp2_box_ihdr_create(jp2_decp jp2dec);
void					jp2_box_ihdr_destroy(jp2_decp jp2dec, jp2_box_ihdrp ihdr);

// Bits Per Component box
typedef struct jp2_box_bpcc_struct {
	jpx_bytep				bpcs;	// Bits per component.
} jp2_box_bpcc;
typedef jp2_box_bpcc FAR * jp2_box_bpccp;

jp2_box_bpccp			jp2_box_bpcc_create(jp2_decp jp2dec);
void					jp2_box_bpcc_destroy(jp2_decp jp2dec, jp2_box_bpccp bpcc);

// Colour Specification box
typedef struct jp2_box_colr_struct {
	jpx_byte				meth;	// Specification method.
	jpx_byte				prec;	// Precedence. 
	jpx_byte				approx;	// Colourspace approximation.
	jpx_uint32				enumcs;	// Enumerated colourspace.
	jpx_bytep				profile;	// ICC profile.
	jpx_uint32				profilelen;	// Length of ICC profile.
} jp2_box_colr;
typedef jp2_box_colr FAR * jp2_box_colrp;

jp2_box_colrp			jp2_box_colr_create(jp2_decp jp2dec);
void					jp2_box_colr_destroy(jp2_decp jp2dec, jp2_box_colrp colr);

// Palette box
typedef struct jp2_box_pclr_struct {
	jpx_uint16				ne;		// Number of entries in the table.
	jpx_byte				npc;	// Number of palette columns specified in the Palette box.
	jpx_bytep				bs;		// The bit depth of palette columns.
	jpx_uint32p				cs;		// The values for entries and palette columns.
} jp2_box_pclr;
typedef jp2_box_pclr FAR * jp2_box_pclrp;

jp2_box_pclrp			jp2_box_pclr_create(jp2_decp jp2dec);
void					jp2_box_pclr_destroy(jp2_decp jp2dec, jp2_box_pclrp pclr);

// Component Mapping box
typedef struct jp2_box_cmap_struct {
	jpx_uint32				channelnum;	// Number of channels.
	jpx_uint16p				cmps;		// Component indexes for channels.
	jpx_bytep				mtyps;		// How channels generated from the actual components in the file.
	jpx_bytep				pcols;		// Component index from the palette to map the actual component.
} jp2_box_cmap;
typedef jp2_box_cmap FAR * jp2_box_cmapp;

jp2_box_cmapp			jp2_box_cmap_create(jp2_decp jp2dec);
void					jp2_box_cmap_destroy(jp2_decp jp2dec, jp2_box_cmapp cmap);

// Channel Definition box
typedef struct jp2_box_cdef_struct {
	jpx_uint16				n;			// Number of channel descriptions.
	jpx_uint16p				cns;		// Channel index.
	jpx_uint16p				typs;		// Channel type.
	jpx_uint16p				asocs;		// Channel association.
} jp2_box_cdef;
typedef jp2_box_cdef FAR * jp2_box_cdefp;

jp2_box_cdefp			jp2_box_cdef_create(jp2_decp jp2dec);
void					jp2_box_cdef_destroy(jp2_decp jp2dec, jp2_box_cdefp cdef);

// Capture Resolution box
typedef struct jp2_box_resc_struct {
	jpx_uint16				vrcn;		// Vertical Capture grid resolution numerator.
	jpx_uint16				vrcd;		// Vertical Capture grid resolution denominator.
	jpx_uint16				hrcn;		// Horizontal Capture grid resolution numerator.
	jpx_uint16				hrcd;		// Horizontal Capture grid resolution denominator.
	jpx_byte				vrce;		// Vertical Capture grid resolution exponent.
	jpx_byte				hrce;		// Horizontal Capture grid resolution exponent.
} jp2_box_resc;
typedef jp2_box_resc FAR * jp2_box_rescp;

jp2_box_rescp			jp2_box_resc_create(jp2_decp jp2dec);
void					jp2_box_resc_destroy(jp2_decp jp2dec, jp2_box_rescp resc);

// Default Display Resolution box
typedef struct jp2_box_resd_struct {
	jpx_uint16				vrdn;		// Vertical Display grid resolution numerator.
	jpx_uint16				vrdd;		// Vertical Display grid resolution denominator.
	jpx_uint16				hrdn;		// Horizontal Display grid resolution numerator.
	jpx_uint16				hrdd;		// Horizontal Display grid resolution denominator.
	jpx_byte				vrde;		// Vertical Display grid resolution exponent.
	jpx_byte				hrde;		// Horizontal Display grid resolution exponent.
} jp2_box_resd;
typedef jp2_box_resd FAR * jp2_box_resdp;

jp2_box_resdp			jp2_box_resd_create(jp2_decp jp2dec);
void					jp2_box_resd_destroy(jp2_decp jp2dec, jp2_box_resdp resd);

// Resolution box (superbox)
typedef struct jp2_box_res_struct {
	jp2_box_rescp			resc;
	jp2_box_resdp			resd;
} jp2_box_res;
typedef jp2_box_res FAR * jp2_box_resp;

jp2_box_resp			jp2_box_res_create(jp2_decp jp2dec);
void					jp2_box_res_destroy(jp2_decp jp2dec, jp2_box_resp res);

// JP2 Header box (superbox)
typedef struct jp2_box_jp2h_struct {
	jp2_box_ihdrp			ihdr;
	jp2_box_bpccp			bpcc;
	jp2_box_colrp			colr;
	jp2_box_pclrp			pclr;
	jp2_box_cmapp			cmap;
	jp2_box_cdefp			cdef;
	jp2_box_resp			res;
} jp2_box_jp2h;
typedef jp2_box_jp2h FAR * jp2_box_jp2hp;

jp2_box_jp2hp			jp2_box_jp2h_create(jp2_decp jp2dec);
void					jp2_box_jp2h_destroy(jp2_decp jp2dec, jp2_box_jp2hp jp2h);


typedef struct jp2_channel_struct {
	jpx_uint16				comp;
	jpx_uint16				cn;
	jpx_uint16				type;
	jpx_uint16				asoc;
} jp2_channel;
typedef jp2_channel FAR * jp2_channelp;

typedef struct jp2_channel_list_struct {
	jpx_uint16				channel_num;
	jp2_channelp			channels;
	jpx_uint32				type;	
} jp2_channel_list;
typedef jp2_channel_list FAR * jp2_channel_listp;

jp2_channel_listp		jp2_channel_list_create(jp2_decp jp2dec);
void					jp2_channel_list_destroy(jp2_decp jp2dec, jp2_channel_listp channellist);

//--------------------------------------------------------------------------------
//			JP2 decoder
//--------------------------------------------------------------------------------
typedef struct jp2_dec_struct {
	jpx_memmgrp				memmgr;
	jpx_bitstreamp			bitstream;
	jpx_error_funcp			error_func;
	jpx_warn_funcp			warn_func;

	jp2_box_ftypp			ftyp;
	jp2_box_jp2hp			jp2h;

	jpc_decp				jpcdec;
	jp2_channel_listp		channel_list;
	jpx_bool				alpha;
	jpx_colorspace			colorspace;

	jpx_bool				output_convert2rgb;
	jpx_uint32				output_width;
	jpx_uint32				output_height;
	jpx_uint16				output_channum;
	jpx_int32				output_offsets[JPX_MAX_CHANNEL];
	jpx_uint32				output_pixgap;
	jpx_bytep				chan_lines[JPX_MAX_CHANNEL];
	jpx_bytep				chan_raw_lines[JPX_MAX_CHANNEL];
	jpx_bytep				full_comp_raw_line;
} jp2_dec;

jp2_decp				jp2_dec_create(jpx_memmgrp memmgr, jpx_streamp stream);
void					jp2_dec_destroy(jp2_decp jp2dec);
void					jp2_dec_set_error_func(jp2_decp jp2dec, jpx_error_funcp error_func);
void					jp2_dec_set_warn_func(jp2_decp jp2dec, jpx_warn_funcp warn_func);
jpx_bool				jp2_dec_parse(jp2_decp jp2dec);
void					jp2_dec_get_imagesize(jp2_decp jp2dec, jpx_uint32p width, jpx_uint32p height);
jpx_bool				jp2_dec_has_alpha(jp2_decp jp2dec);
jpx_int32				jp2_dec_get_colorspace(jp2_decp jp2dec);
void					jp2_dec_set_convert2rgb(jp2_decp jp2dec, jpx_bool convert2rgb);
void					jp2_dec_init(jp2_decp jp2dec, jpx_uint16 channum, jpx_int32p offsets, jpx_int32 pixgap);
jpx_bool				jp2_dec_scanline(jp2_decp jp2dec, jpx_bytep line);
void					jp2_dec_finalize(jp2_decp jp2dec);

#endif // _JP2_DEC_H_
