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

#ifndef _JPX_BITSTUFF_H_
#define _JPX_BITSTUFF_H_
									
typedef struct jpx_bitstuff_stuct {
	jpx_bitstreamp			bitstream;
	jpx_uint32				bitbuf;
	jpx_uint32				bitlen;
	jpx_bool				skipbit;
} jpx_bitstuff;
typedef jpx_bitstuff FAR * jpx_bitstuffp;

jpx_bitstuffp				jpx_bitstuff_create(jpx_memmgrp memmgr, jpx_bitstreamp bitstream);
void						jpx_bitstuff_destroy(jpx_memmgrp memmgr, jpx_bitstuffp bitstuff);
jpx_bool					jpx_bitstuff_readbit(jpx_bitstuffp bitstuff);
jpx_uint32					jpx_bitstuff_readbits(jpx_bitstuffp bitstuff, jpx_uint32 n);
void						jpx_bitstuff_alignbyte(jpx_bitstuffp bitstuff);
jpx_bool					jpx_bitstuff_checkpredict(jpx_bitstuffp bitstuff);

#endif // _JPX_BITSTUFF_H_
