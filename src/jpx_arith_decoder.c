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
#include "../include/jpx_arith_decoder.h"
							
typedef struct jpx_arith_qe_struct {
	jpx_uint16				qe;
	jpx_byte				nmps	: 6;
	jpx_byte				nlps	: 6;
	jpx_byte				swt		: 4;
} jpx_arith_qe;
typedef jpx_arith_qe FAR * jpx_arith_qep;

const jpx_arith_qe g_qetable[] = {
  { 0x5601,  1,  1, 1 },
  { 0x3401,  2,  6, 0 },
  { 0x1801,  3,  9, 0 },
  { 0x0AC1,  4, 12, 0 },
  { 0x0521,  5, 29, 0 },
  { 0x0221, 38, 33, 0 },
  { 0x5601,  7,  6, 1 },
  { 0x5401,  8, 14, 0 },
  { 0x4801,  9, 14, 0 },
  { 0x3801, 10, 14, 0 },
  { 0x3001, 11, 17, 0 },
  { 0x2401, 12, 18, 0 },
  { 0x1C01, 13, 20, 0 },
  { 0x1601, 29, 21, 0 },
  { 0x5601, 15, 14, 1 },
  { 0x5401, 16, 14, 0 },
  { 0x5101, 17, 15, 0 },
  { 0x4801, 18, 16, 0 },
  { 0x3801, 19, 17, 0 },
  { 0x3401, 20, 18, 0 },
  { 0x3001, 21, 19, 0 },
  { 0x2801, 22, 19, 0 },
  { 0x2401, 23, 20, 0 },
  { 0x2201, 24, 21, 0 },
  { 0x1C01, 25, 22, 0 },
  { 0x1801, 26, 23, 0 },
  { 0x1601, 27, 24, 0 },
  { 0x1401, 28, 25, 0 },
  { 0x1201, 29, 26, 0 },
  { 0x1101, 30, 27, 0 },
  { 0x0AC1, 31, 28, 0 },
  { 0x09C1, 32, 29, 0 },
  { 0x08A1, 33, 30, 0 },
  { 0x0521, 34, 31, 0 },
  { 0x0441, 35, 32, 0 },
  { 0x02A1, 36, 33, 0 },
  { 0x0221, 37, 34, 0 },
  { 0x0141, 38, 35, 0 },
  { 0x0111, 39, 36, 0 },
  { 0x0085, 40, 37, 0 },
  { 0x0049, 41, 38, 0 },
  { 0x0025, 42, 39, 0 },
  { 0x0015, 43, 40, 0 },
  { 0x0009, 44, 41, 0 },
  { 0x0005, 45, 42, 0 },
  { 0x0001, 45, 43, 0 },
  { 0x5601, 46, 46, 0 }
};

void						jpx_arith_decoder_initdec(jpx_arith_decoderp arithdecoder);
void						jpx_arith_decoder_bytein(jpx_arith_decoderp arithdecoder);
void						jpx_arith_decoder_renormd(jpx_arith_decoderp arithdecoder);

jpx_arith_decoderp			jpx_arith_decoder_create(jpx_memmgrp memmgr, jpx_bitstreamp bitstream)
{
	jpx_arith_decoderp arithdecoder = (jpx_arith_decoderp)memmgr->alloc_func(memmgr, sizeof(jpx_arith_decoder));
	jpx_memset(arithdecoder, 0, sizeof(jpx_arith_decoder));
	arithdecoder->bitstream = bitstream;

	jpx_arith_decoder_initdec(arithdecoder);
	return arithdecoder;
}
void						jpx_arith_decoder_destroy(jpx_memmgrp memmgr, jpx_arith_decoderp arithdecoder)
{
	memmgr->free_func(memmgr, arithdecoder);
}
void						jpx_arith_decoder_initdec(jpx_arith_decoderp arithdecoder)
{
	if (!jpx_bitstream_readbyte(arithdecoder->bitstream, &arithdecoder->b)) {
		arithdecoder->state = -1;
		arithdecoder->b = 0xff;
	}
	arithdecoder->c = arithdecoder->b << 16;

	jpx_arith_decoder_bytein(arithdecoder);

	arithdecoder->c = arithdecoder->c << 7;
	arithdecoder->ct = arithdecoder->ct - 7;
	arithdecoder->a = 0x8000;
}
void						jpx_arith_decoder_bytein(jpx_arith_decoderp arithdecoder)
{
	jpx_byte b1;

	if (arithdecoder->state == -1) {
		arithdecoder->c = arithdecoder->c + 0xff00;
		arithdecoder->ct = 8;
	} else {
		if (!jpx_bitstream_readbyte(arithdecoder->bitstream, &b1)) {
			arithdecoder->state = -1;
			arithdecoder->c = arithdecoder->c + 0xff00;
			arithdecoder->ct = 8;
		} else {
			if (arithdecoder->b == 0xff) {
				if (b1 > 0x8f) {
					arithdecoder->c = arithdecoder->c + 0xff00;
					arithdecoder->ct = 8;
				} else {
					arithdecoder->c = arithdecoder->c + (b1<<9);
					arithdecoder->ct = 7;
				}
			} else {
				arithdecoder->c = arithdecoder->c + (b1<<8);
				arithdecoder->ct = 8;
			}
			arithdecoder->b = b1;
		}
	}
}
void						jpx_arith_decoder_renormd(jpx_arith_decoderp arithdecoder)
{
	do {
		if (arithdecoder->ct == 0) {
			jpx_arith_decoder_bytein(arithdecoder);
		}
		arithdecoder->a <<= 1;
		arithdecoder->c <<= 1;
		arithdecoder->ct --;
	} while ((arithdecoder->a&0x8000) == 0);
}
jpx_int32					jpx_arith_decoder_decode(jpx_arith_decoderp arithdecoder, jpx_arith_ctxp arithctx, jpx_int32 cx)
{
	jpx_int32 value;
	const jpx_arith_qep qe = (const jpx_arith_qep)&g_qetable[arithctx->i];

	arithdecoder->a = arithdecoder->a - qe->qe;
	if ((arithdecoder->c>>16) < qe->qe) {
		if (arithdecoder->a < qe->qe) {
			arithdecoder->a = qe->qe;
			value = arithctx->mps;
			arithctx->i = qe->nmps;
		} else {
			arithdecoder->a = qe->qe;
			value = 1 - arithctx->mps;
			if (qe->swt == 1) {
				arithctx->mps = 1 - arithctx->mps;
			}
			arithctx->i = qe->nlps;
		}

		jpx_arith_decoder_renormd(arithdecoder);
	} else {
		arithdecoder->c = arithdecoder->c - (qe->qe<<16);
		if ((arithdecoder->a&0x8000) == 0) {
			if (arithdecoder->a < qe->qe) {
				value = 1 - arithctx->mps;
				if (qe->swt == 1) {
					arithctx->mps = 1 - arithctx->mps;
				}
				arithctx->i = qe->nlps;
			} else {
				value = arithctx->mps;
				arithctx->i = qe->nmps;
			}

			jpx_arith_decoder_renormd(arithdecoder);
		} else {
			value = arithctx->mps;
		}
	}

#ifdef _JPX_ENABLE_ARITH_LOG_
	JPX_LOG("%d\n", cx);

	{
		static int count = 0;
		if (++count == 127492) {
			count = count;
		}
	}
#endif

	return value;
}


void						jpx_arith_reset_contexts(jpx_arith_ctxp contexts)
{
	jpx_int32 i;
	
	for (i = 0; i < JPC_MAX_CONTEXTS; i++) {
		contexts[i].i = 0;
		contexts[i].mps = 0;
	}
	contexts[0].i = 4;
	contexts[17].i = 3;
	contexts[18].i = 46;
}
