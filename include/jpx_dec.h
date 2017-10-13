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

#ifndef _JPX_DEC_H_
#define _JPX_DEC_H_

#include "jpx_conf.h"

//--------------------------------------------------------------------------------
//			Memory interface
//--------------------------------------------------------------------------------
typedef struct jpx_memmgr_struct FAR * jpx_memmgrp;
typedef jpx_voidp		(*jpx_memmgr_alloc_funcp)(jpx_memmgrp memmgr, jpx_uint32 size);
typedef jpx_voidp		(*jpx_memmgr_realloc_funcp)(jpx_memmgrp memmgr, jpx_voidp buf, jpx_uint32 size);
typedef void			(*jpx_memmgr_free_funcp)(jpx_memmgrp memmgr, jpx_voidp buf);
typedef void			(*jpx_memmgr_destroyp)(jpx_memmgrp memmgr);

typedef struct jpx_memmgr_struct {
	jpx_memmgr_alloc_funcp		alloc_func;
	jpx_memmgr_realloc_funcp	realloc_func;
	jpx_memmgr_free_funcp		free_func;
	jpx_memmgr_destroyp			destroy_func;
} jpx_memmgr;

//--------------------------------------------------------------------------------
//			Stream interface
//--------------------------------------------------------------------------------
typedef struct jpx_stream_struct FAR * jpx_streamp;
typedef jpx_uint32		(*jpx_stream_getsize_funcp)(jpx_streamp stream);
typedef jpx_uint32		(*jpx_stream_getpos_funcp)(jpx_streamp stream);
typedef jpx_bool		(*jpx_stream_seekpos_funcp)(jpx_streamp stream, jpx_uint32 pos);
typedef jpx_bool		(*jpx_stream_read_funcp)(jpx_streamp stream, jpx_voidp buf, jpx_uint32 len);
typedef void			(*jpx_stream_destroyp)(jpx_streamp);

typedef struct jpx_stream_struct {
	jpx_stream_getsize_funcp	getsize_func;
	jpx_stream_getpos_funcp		getpos_func;
	jpx_stream_seekpos_funcp	seekpos_func;
	jpx_stream_read_funcp		read_func;
	jpx_stream_destroyp			destroy_func;
} jpx_stream;

//--------------------------------------------------------------------------------
//			Error handler
//--------------------------------------------------------------------------------
typedef void			(*jpx_error_funcp)(jpx_const_charp format, ...);
typedef void			(*jpx_warn_funcp)(jpx_const_charp format, ...);

#include "jpx_memmgr.h"
#include "jpx_stream.h"
#include "jpx_bitstream.h"
#include "jpx_bitstuff.h"
#include "jpx_tagtree.h"
#include "jpx_arith_decoder.h"
#include "jpx_fixpoint.h"
#include "jpx_colorspace.h"
#include "jpc_dec.h"
#include "jp2_dec.h"

//--------------------------------------------------------------------------------
//			Format detection
//--------------------------------------------------------------------------------
jpx_bool				jpx_is_jp2(jpx_streamp stream);
jpx_bool				jpx_is_jpc(jpx_streamp stream);

#endif // _JPX_DEC_H_
