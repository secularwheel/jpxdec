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
#include "../include/jpx_bitstuff.h"

jpx_bitstuffp				jpx_bitstuff_create(jpx_memmgrp memmgr, jpx_bitstreamp bitstream)
{
	jpx_bitstuffp bitstuff = (jpx_bitstuffp)memmgr->alloc_func(memmgr, sizeof(jpx_bitstuff));
	jpx_memset(bitstuff, 0, sizeof(jpx_bitstuff));
	bitstuff->bitstream = bitstream;
	return bitstuff;
}
void						jpx_bitstuff_destroy(jpx_memmgrp memmgr, jpx_bitstuffp bitstuff)
{
	memmgr->free_func(memmgr, bitstuff);
}
jpx_bool					jpx_bitstuff_readbit(jpx_bitstuffp bitstuff)
{
	jpx_byte c;
	jpx_bool val;

	if (bitstuff->bitlen == 0) {
		if (!jpx_bitstream_readbyte(bitstuff->bitstream, &c)) {
			c = 0x7f;
		}
		if (bitstuff->skipbit) {
			bitstuff->bitbuf = c&0x7f;
			bitstuff->bitlen = 7;
		} else {
			bitstuff->bitbuf = c;
			bitstuff->bitlen = 8;
		}
		bitstuff->skipbit = c==0xff;
	}
	val = (bitstuff->bitbuf>>(bitstuff->bitlen-1))&0x01;
	bitstuff->bitlen--;
	return val;
}
jpx_uint32					jpx_bitstuff_readbits(jpx_bitstuffp bitstuff, jpx_uint32 n)
{
	jpx_byte c;
	jpx_uint32 val;

	while (bitstuff->bitlen < n) {
		if (!jpx_bitstream_readbyte(bitstuff->bitstream, &c)) {
			c = 0x7f;
		}
		if (bitstuff->skipbit) {
			bitstuff->bitbuf = (bitstuff->bitbuf<<7)|(c&0x7f);
			bitstuff->bitlen += 7;
		} else {
			bitstuff->bitbuf = (bitstuff->bitbuf<<8)|c;
			bitstuff->bitlen += 8;
		}
		bitstuff->skipbit = c==0xff;
	}
	val = (bitstuff->bitbuf>>(bitstuff->bitlen-n))&((1<<n)-1);
	bitstuff->bitlen -= n;
	return val;
}
void						jpx_bitstuff_alignbyte(jpx_bitstuffp bitstuff)
{
	if ((bitstuff->bitlen==0) && bitstuff->skipbit) {
		jpx_byte c;

		if (!jpx_bitstream_readbyte(bitstuff->bitstream, &c)) {
			c = 0x7f;
		}
		bitstuff->bitbuf = (bitstuff->bitbuf<<7)|(c&0x7f);
		bitstuff->bitlen += 7;
		bitstuff->skipbit = c==0xff;
	}
}
jpx_bool					jpx_bitstuff_checkpredict(jpx_bitstuffp bitstuff)
{
	if ((bitstuff->bitlen==0) && bitstuff->skipbit) {
		jpx_byte c;

		if (!jpx_bitstream_readbyte(bitstuff->bitstream, &c) || (c!=0x2a)) {
			return jpx_false;
		}
		bitstuff->bitlen = 8;
		bitstuff->bitbuf = c;
	} else {
		if ((bitstuff->bitbuf&~(0xff<<bitstuff->bitlen)) != ((jpx_uint32)0x55>>(8-bitstuff->bitlen))) {
			return jpx_false;
		}
	}
	return jpx_true;
}
