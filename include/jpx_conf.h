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

#ifndef _JPX_CONF_H_
#define _JPX_CONF_H_

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define jpx_memcpy			memcpy
#define jpx_memcmp			memcmp
#define jpx_memset			memset
#define jpx_strcpy			strcpy
#define jpx_strcmp			strcmp
#define jpx_strlen			strlen
#define jpx_assert			assert

#ifndef FAR
#define FAR
#endif

/* typedefs for types */
typedef unsigned char		jpx_byte;
typedef unsigned short		jpx_uint16;
typedef short				jpx_int16;
typedef unsigned int		jpx_uint32;
typedef int					jpx_int32;
typedef unsigned __int64	jpx_uint64;
typedef __int64				jpx_int64;
typedef int					jpx_bool;

/* Add typedefs for pointers */
typedef const char		FAR * jpx_const_charp;
typedef char            FAR * jpx_charp;
typedef void			FAR * jpx_voidp;
typedef jpx_byte        FAR * jpx_bytep;
typedef const jpx_byte	FAR	* jpx_const_bytep;
typedef jpx_uint16		FAR * jpx_uint16p;
typedef jpx_int16		FAR * jpx_int16p;
typedef jpx_uint32		FAR * jpx_uint32p;
typedef jpx_int32		FAR * jpx_int32p;
typedef jpx_uint64		FAR * jpx_uint64p;
typedef jpx_int64		FAR * jpx_int64p;
typedef jpx_bool		FAR * jpx_boolp;

/* Pointers to pointers; i.e. arrays */
typedef const char		FAR * FAR * jpx_const_charpp;
typedef char            FAR * FAR * jpx_charpp;
typedef void			FAR * FAR * jpx_voidpp;
typedef jpx_byte        FAR * FAR * jpx_bytepp;
typedef jpx_uint16		FAR * FAR * jpx_uint16pp;
typedef jpx_int16		FAR * FAR * jpx_int16pp;
typedef jpx_uint32		FAR * FAR * jpx_uint32pp;
typedef jpx_int32		FAR * FAR * jpx_int32pp;
typedef jpx_uint64		FAR * FAR * jpx_uint64pp;
typedef jpx_int64		FAR * FAR * jpx_int64pp;
typedef jpx_bool		FAR * FAR * jpx_boolpp;

#define jpx_false					0
#define jpx_true					1

#define JPX_SUCCESS					0	
#define JPX_FAILED					-1
#define JPX_ERROR_TOO_SHORT			-2
#define JPX_ERROR_FETAL				-3

#define JPC_CAPABILITY_STANDARD		0
#define JPC_CAPABILITY_PROFILE0		1
#define JPC_CAPABILITY_PROFILE1		2

#define JPX_BUFFER_SIZE				512
#define JPX_MAX_COMPONENT			32
#define JPX_MAX_CHANNEL				32

//--------------------------------------------------------------------------------
//			JPX Colourspace defines
//--------------------------------------------------------------------------------
typedef enum jpx_colorspace_enum {
	JPX_CS_UNKNOWN		= -1,	// Undefined
	JPX_CS_BILEVEL		= 0,	// Bi-level
	JPX_CS_YCBCR1		= 1,	// YCbCr(1)
	JPX_CS_YCBCR2		= 3,	// YCbCr(2)
	JPX_CS_YCBCR3		= 4,	// YCbCr(3)
	JPX_CS_PHOTOYCC		= 9,	// PhotoYCC
	JPX_CS_CMY			= 11,	// CMY
	JPX_CS_CMYK			= 12,	// CMYK
	JPX_CS_YCCK			= 13,	// YCCK
	JPX_CS_CIELAB		= 14,	// CIELab
	JPX_CS_BILEVEL2		= 15,	// Bi-level(2)
	JPX_CS_SRGB			= 16,	// sRGB
	JPX_CS_GREYSCALE	= 17,	// greyscale
	JPX_CS_SYCC			= 18,	// sYCC
	JPX_CS_CIEJAB		= 19,	// CIEJab
	JPX_CS_ESRGB		= 20,	// e-sRGB
	JPX_CS_ROMMRGB		= 21,	// ROMM-RGB
	JPX_CS_YPBPR1125_60	= 22,	// YPbPr(1125/60)
	JPX_CS_YPBPR1250_50	= 23,	// YPbPr(1250/50)
	JPX_CS_ESYCC		= 24	// e-sYCC
} jpx_colorspace;

// coefficient
typedef union jpx_coeff_union {
	jpx_int16p				buf16;
	jpx_int32p				buf32;
} jpx_coeff_line;
typedef jpx_coeff_line FAR * jpx_coeff_linep;

#define _JPX_ENABLE_LOG_
#ifdef _JPX_ENABLE_LOG_
#include <stdio.h>
#define JPX_LOG printf
#endif

#ifdef _JPX_ENABLE_LOG_
//#define _JPX_ENABLE_PARSE_LOG_
//#define _JPX_ENABLE_ARITH_LOG_
//#define _JPX_ENABLE_DEQUANT_LOG_
//#define _JPX_ENABLE_SYNTHESIS_LOG_
//#define _JPX_ENABLE_SYNTHESIS_HORZ_LOG_
//#define _JPX_ENABLE_SYNTHESIS_VERT_LOG_
#endif

#endif // _JPX_CONF_H_
