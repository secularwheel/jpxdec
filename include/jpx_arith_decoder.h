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

#ifndef _JPX_ARITH_DECODER_H_
#define _JPX_ARITH_DECODER_H_
							
typedef struct jpx_arith_ctx_struct {
	jpx_byte				mps	: 1;
	jpx_byte				i	: 7;
} jpx_arith_ctx;
typedef jpx_arith_ctx FAR * jpx_arith_ctxp;

void						jpx_arith_reset_contexts(jpx_arith_ctxp contexts);

typedef struct jpx_arith_decoder_struct {
	jpx_bitstreamp			bitstream;
	jpx_byte				b;
	jpx_uint32				c;
	jpx_int32				a;
	jpx_int32				ct;
	jpx_int32				state;
} jpx_arith_decoder;
typedef jpx_arith_decoder FAR * jpx_arith_decoderp;

jpx_arith_decoderp			jpx_arith_decoder_create(jpx_memmgrp memmgr, jpx_bitstreamp bitstream);
void						jpx_arith_decoder_destroy(jpx_memmgrp memmgr, jpx_arith_decoderp arithdecoder);
jpx_int32					jpx_arith_decoder_decode(jpx_arith_decoderp arithdecoder, jpx_arith_ctxp arithctx, jpx_int32 cx);

#endif // _JPX_ARITH_DECODER_H_
