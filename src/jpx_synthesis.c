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
#include "../include/jpx_codeblock.h"

#define JPX_ALPHA			-103949	// -1.586134342059924
#define JPX_BETA			-3472	// -0.052980118572961
#define JPX_GAMMA			57862	// 0.882911075530934
#define JPX_DELTA			29066	// 0.443506852043971
#define JPX_KAPPA			80621	// 1.230174104914001

void						jpx_dwt_interleave(jpx_coeff_line line, jpx_coeff_line low, jpx_coeff_line high, jpx_uint32 width, jpx_uint32 u0);
void						jpx_dwt_rev_horz(jpx_coeff_line edgel, jpx_coeff_line edger, jpx_uint32 i0, jpx_uint32 i1);
void						jpx_dwt_inrev_horz(jpx_coeff_line edgel, jpx_coeff_line edger, jpx_uint32 i0, jpx_uint32 i1);
jpx_bool					jpc_dwt_rev_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_coeff_line line);
jpx_bool					jpc_dwt_inrev_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_coeff_line line);

jpx_bool					jpc_decode_subband_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_uint32 bnd, jpx_coeff_line line);
jpx_bool					jpc_decode_codeblocks_row(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_uint32 sb);



void						jpx_dwt_interleave(jpx_coeff_line line, jpx_coeff_line low, jpx_coeff_line high, jpx_uint32 width, jpx_uint32 u0)
{
	jpx_uint32 i;
	jpx_int32p line1=line.buf32, low1=low.buf32, high1=high.buf32;

	if (u0&1) {
		for (i = 0; i < width; i++) {
			if (i & 1) {
				line1[i] = *low1++;
			} else {
				line1[i] = *high1++;
			}
		}
	} else {
		for (i = 0; i < width; i++) {
			if (i & 1) {
				line1[i] = *high1++;
			} else {
				line1[i] = *low1++;
			}
		}	
	}
}
void						jpx_dwt_rev_horz(jpx_coeff_line edgel, jpx_coeff_line edger, jpx_uint32 i0, jpx_uint32 i1)
{
	jpx_int32 i, max;

	if (i0 == i1 - 1) {
		if(i0 & 1) edgel.buf32[0] >>= 1;
		return ;
	}
	
	max = i1 - i0;
	// Extension to the left and right.
	if (i0 & 1) { // i0 odd
		if (i1 & 1) { // i1 odd
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-2] = edgel.buf32[2];
		} else { // i1 even
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-2] = edgel.buf32[2];
			edger.buf32[2] = edger.buf32[-2]; 
		}
		
		for (i = -1; i <= max; i += 2) {
			edgel.buf32[i] -= ((edgel.buf32[i-1]+edgel.buf32[i+1]+8)>>4) << 2;
		}
		for (i = 0; i < max; i += 2) {
			edgel.buf32[i] += ((edgel.buf32[i-1]+edgel.buf32[i+1])>>3) << 2;
		}
	} else { // i0 even
		if (i1 & 1) { // i1 odd
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
		} else { // i1 even
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[2] = edger.buf32[-2];
		}
		
		for (i = 0; i <= max; i += 2) {
			edgel.buf32[i] -= ((edgel.buf32[i-1]+edgel.buf32[i+1]+8)>>4) << 2;
		}
		for (i = 1; i < max; i += 2) {
			edgel.buf32[i] += ((edgel.buf32[i-1]+edgel.buf32[i+1])>>3) << 2;
		}
	}
}

void						jpx_dwt_inrev_horz(jpx_coeff_line edgel, jpx_coeff_line edger, jpx_uint32 i0, jpx_uint32 i1)
{
	jpx_int32 i, max;
	
	if (i0 == i1-1) {
		if(i0 & 1) edgel.buf32[0] >>= 1;
		return ;
	}
	
	max = i1 - i0;
	// Extension to the left and right.
	if (i0 & 1) { // i0 odd
		if (i1 & 1) { // i1 odd
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-2] = edgel.buf32[2];
			edger.buf32[2] = edger.buf32[-2];
			edgel.buf32[-3] = edgel.buf32[3];
			edger.buf32[3] = edger.buf32[-3];
			edgel.buf32[-4] = edgel.buf32[4];
		} else { // i1 even
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-2] = edgel.buf32[2];
			edger.buf32[2] = edger.buf32[-2];
			edgel.buf32[-3] = edgel.buf32[3];
			edger.buf32[3] = edger.buf32[-3];
			edgel.buf32[-4] = edgel.buf32[4];
			edger.buf32[4] = edger.buf32[-4];
		}
		
		for (i = -3; i <= max + 2; i += 2) {
			edgel.buf32[i] = JPX_FIXMUL(edgel.buf32[i], JPX_KAPPA);
		}
		for (i = -4; i <= max + 3; i += 2) {
			edgel.buf32[i] = JPX_FIXDIV(edgel.buf32[i], JPX_KAPPA);
		}
		for (i = -3; i <= max + 2; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_DELTA); 
		}
		for (i = -2; i <= max + 1; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_GAMMA);
		}
		for (i = -1; i <= max; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_BETA);
		}
		for (i = 0; i <= max - 1; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_ALPHA);
		}
	} else { // i0 even
		if (i1 & 1) { // i1 odd
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-2] = edgel.buf32[2];
			edger.buf32[2] = edger.buf32[-2];
			edgel.buf32[-3] = edgel.buf32[3];
			edger.buf32[3] = edger.buf32[-3];
		} else { // i1 even
			edger.buf32[1] = edger.buf32[-1];
			edgel.buf32[-1] = edgel.buf32[1];
			edger.buf32[2] = edger.buf32[-2];
			edgel.buf32[-2] = edgel.buf32[2];
			edger.buf32[3] = edger.buf32[-3];
			edgel.buf32[-3] = edgel.buf32[3];
			edger.buf32[4] = edger.buf32[-4];
		}
		
		for (i = -2; i <= max + 2; i += 2) {
			edgel.buf32[i] = JPX_FIXMUL(edgel.buf32[i],JPX_KAPPA);
		}
		for (i = -3; i <= max + 3; i += 2) {
			edgel.buf32[i] = JPX_FIXDIV(edgel.buf32[i], JPX_KAPPA);
		}
		for (i = -2; i <= max + 2; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_DELTA); 
		}
		for (i = -1; i <= max + 1; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_GAMMA);
		}
		for(i = 0; i <= max; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_BETA);
		}
		for (i = 1; i <= max - 1; i += 2) {
			edgel.buf32[i] -= JPX_FIXMUL(edgel.buf32[i-1]+edgel.buf32[i+1], JPX_ALPHA);
		}
	}

#ifdef _JPX_ENABLE_SYNTHESIS_HORZ_LOG_
	for (i=0; i<max; i++) {
		JPX_LOG("%d\n", edgel.buf32[i]);
	}
#endif
}

jpx_bool					jpc_dwt_rev_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_coeff_line line)
{
	jpc_reslevelp reslevel;
	jpx_uint32 k, i, y;
	jpx_coeff_line ptr1, ptr2, ptr3;
	jpx_coeff_line lows[3], highs[3];

#ifdef _JPX_ENABLE_SYNTHESIS_LOG_
	{
		jpx_uint16 comp_index, tile_index;
		for (tile_index=0; tile_index<jpcdec->tilenum; tile_index++) {
			for (comp_index=0; comp_index<jpcdec->component_num; comp_index++) {
				if (&jpcdec->tiles[tile_index].components[comp_index] == tc) {
					goto ADDLOG;
				}
			}
		}
ADDLOG:
		JPX_LOG("decoding resolution: tileidx=%d, compidx=%d, r=%d\n", tile_index, comp_index, r);
	}
#endif
	
	reslevel = &tc->reslevels[r];
	if (reslevel->try1-reslevel->try0 <= 3) { // Less than 3 lines
		if (reslevel->y == reslevel->try0) { // Do wavelet transform at first reach
			jpc_linebufp curline, curlow, curhigh;
			curline = reslevel->lines.cur;
			curlow = reslevel->lows.cur;
			curhigh = reslevel->highs.cur;

			for (y=reslevel->try0; y<reslevel->try1; y++) {
				if (y&1) {
					jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
					jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
				} else {
					if (r == 1) {
						jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
					} else {
						jpc_dwt_rev_line(jpcdec, tc, r-1, reslevel->templow.buf);
					}
					jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
				}
				
				ptr1.buf32 = curlow->buf.buf32;
				ptr2.buf32 = reslevel->templow.buf.buf32;
				ptr3.buf32 = reslevel->temphigh.buf.buf32;
				jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
				ptr2.buf32 = curhigh->buf.buf32+reslevel->high_width-1;
				jpx_dwt_rev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
				
				curline = curline->next;
				curlow = curlow->next;
				curhigh = curhigh->next;
			}

			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			
			// Performing lifting factors.
			switch (reslevel->try1-reslevel->try0) {
			case 3:
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
					if (reslevel->try0 & 1) {
						// STEP1
						lows[1].buf32[i] -= ((lows[0].buf32[i]+lows[2].buf32[i]+8)>>4) << 2;
						// STEP2
						lows[0].buf32[i] += lows[1].buf32[i];
						lows[2].buf32[i] += lows[1].buf32[i];
						
					} else {
						// STEP1
						lows[0].buf32[i] -= (((lows[1].buf32[i]<<1)+8)>>4) << 2;
						lows[2].buf32[i] -= (((lows[1].buf32[i]<<1)+8)>>4) << 2;
						// STEP2
						lows[1].buf32[i] += ((lows[0].buf32[i]+lows[2].buf32[i])>>3) << 2;
					}
				}
				break;
			case 2:
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
					if (reslevel->try0 & 1) {
						lows[1].buf32[i] -= (((lows[0].buf32[i]<<1)+8)>>4) << 2;
						lows[0].buf32[i] += lows[1].buf32[i];
						
					} else {
						lows[0].buf32[i] -= (((lows[1].buf32[i]<<1)+8)>>4) << 2;
						lows[1].buf32[i] += lows[0].buf32[i];
					}
				}
				break;
			case 1:
				if (reslevel->try0 & 1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
						lows[0].buf32[i] = (lows[0].buf32[i]>>3) << 2;
					}
				}
				break;
			}
		}
	} else { // More than 3 lines
		if (reslevel->y == reslevel->try0) { // Do special wavelet transform at first reach
			jpc_linebufp curline, curlow, curhigh;
			curline = reslevel->lines.cur;
			curlow = reslevel->lows.cur;
			curhigh = reslevel->highs.cur;
			for (k=0, y=reslevel->try0; k<3; k++,y++) { // Decode the first 3 lines to use row-based transform
				if (y&1) {
					jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
					jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
				} else {
					if (r == 1) {
						jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
					} else {
						jpc_dwt_rev_line(jpcdec, tc, r-1, reslevel->templow.buf);
					}
					jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
				}
				
				ptr1.buf32 = curlow->buf.buf32;
				ptr2.buf32 = reslevel->templow.buf.buf32;
				ptr3.buf32 = reslevel->temphigh.buf.buf32;
				jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
				ptr2.buf32 = curhigh->buf.buf32+reslevel->high_width-1;
				jpx_dwt_rev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);

				curline = curline->next;
				curlow = curlow->next;
				curhigh = curhigh->next;
			}
				
			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			
			// Performing lifting factors.
			for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
				if (reslevel->try0 & 1) {
					lows[1].buf32[i] -= ((lows[0].buf32[i]+lows[2].buf32[i]+8)>>4) << 2;

					lows[0].buf32[i] += lows[1].buf32[i];
				} else {
					lows[0].buf32[i] -= (((lows[1].buf32[i]<<1)+8)>>4) << 2;
					lows[2].buf32[i] -= (lows[1].buf32[i]+8) >> 2;

					lows[1].buf32[i] += lows[0].buf32[i] >> 1;
				}
			}
		} else if (reslevel->y == reslevel->try1-3) { // Do special wavelet transform when reached final lines.
			if (reslevel->y&1) {
				jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
				jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
			} else {
				if (r == 1) {
					jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
				} else {
					jpc_dwt_rev_line(jpcdec, tc, r-1, reslevel->templow.buf);
				}
				jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
			}
			
			ptr1.buf32 =  reslevel->lows.cur->prev->buf.buf32;
			ptr2.buf32 = reslevel->templow.buf.buf32;
			ptr3.buf32 = reslevel->temphigh.buf.buf32;
			jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
			ptr2.buf32 = reslevel->highs.cur->prev->buf.buf32+reslevel->high_width-1;
			jpx_dwt_rev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
			
			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			
			// Performing lifting factors.
			if(reslevel->y&1) {
				for (i = 0; i < reslevel->low_width + reslevel->high_width; i++) {	
					lows[1].buf32[i] -= lows[2].buf32[i] >> 2;
					lows[1].buf32[i] = (lows[1].buf32[i]+3) & ~3;
					
					lows[0].buf32[i] += lows[1].buf32[i] >> 1;
					lows[0].buf32[i] = lows[0].buf32[i] & ~3;
					
					lows[2].buf32[i] += lows[1].buf32[i];
				}
			} else {
				for(i = 0; i < reslevel->low_width + reslevel->high_width; i++) {
					lows[2].buf32[i] -= (lows[1].buf32[i]+8) >> 2;
					lows[1].buf32[i] += ((lows[0].buf32[i]+lows[2].buf32[i])>>3) << 2;
				}
			}
		} else if (reslevel->y < reslevel->try1-3) { // Step for row-based wavelet transform for intermediate lines.
			if (reslevel->y&1) {
				jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
				jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
			} else {
				if (r == 1) {
					jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
				} else {
					jpc_dwt_rev_line(jpcdec, tc, r-1, reslevel->templow.buf);
				}
				jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
			}
			
			ptr1.buf32 =  reslevel->lows.cur->prev->buf.buf32;
			ptr2.buf32 = reslevel->templow.buf.buf32;
			ptr3.buf32 = reslevel->temphigh.buf.buf32;
			jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
			ptr2.buf32 = reslevel->highs.cur->prev->buf.buf32+reslevel->high_width-1;
			jpx_dwt_rev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
			
			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;

			// Performing lifting factors.
			if (reslevel->y&1) {
				for (i = 0; i < reslevel->low_width+reslevel->high_width; i++) {
					lows[1].buf32[i] -= lows[2].buf32[i] >> 2;
					lows[1].buf32[i] = (lows[1].buf32[i]+3) & ~3;
					lows[0].buf32[i] += lows[1].buf32[i] >> 1;
					lows[0].buf32[i] = lows[0].buf32[i] & ~3;
				}
			} else {
				for (i = 0; i < reslevel->low_width+reslevel->high_width; i++) {
					lows[2].buf32[i] -= (lows[1].buf32[i]+8) >> 2;
					lows[1].buf32[i] += lows[0].buf32[i] >> 1;
				}
			}
		}
	}

	if (reslevel->y < reslevel->try1) {
		jpx_memcpy(line.buf32, reslevel->lines.cur->buf.buf32+4, (reslevel->trx1-reslevel->trx0)*sizeof(jpx_int32));
		
#ifdef _JPX_ENABLE_SYNTHESIS_VERT_LOG_
		for (i=0; i<reslevel->trx1-reslevel->trx0; i++) {
			JPX_LOG("%d\n", line.buf32[i]);
		}
#endif

	} else {
		jpx_memset(line.buf32, 0, (reslevel->trx1-reslevel->trx0)*sizeof(jpx_int32));
	}
	
	reslevel->y++;
	reslevel->lines.cur = reslevel->lines.cur->next;
	reslevel->lows.cur = reslevel->lows.cur->next;
	reslevel->highs.cur = reslevel->highs.cur->next;

	return jpx_true;
}
jpx_bool					jpc_dwt_inrev_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_coeff_line line)
{
	jpc_reslevelp reslevel;
	jpx_uint32 k, i, y;
	jpx_coeff_line ptr1, ptr2, ptr3;
	jpx_coeff_line lows[5], highs[5];

#ifdef _JPX_ENABLE_SYNTHESIS_LOG_
	{
		jpx_uint16 comp_index, tile_index;
		for (tile_index=0; tile_index<jpcdec->tilenum; tile_index++) {
			for (comp_index=0; comp_index<jpcdec->component_num; comp_index++) {
				if (&jpcdec->tiles[tile_index].components[comp_index] == tc) {
					goto ADDLOG;
				}
			}
		}
ADDLOG:
		JPX_LOG("decoding resolution: tileidx=%d, compidx=%d, r=%d\n", tile_index, comp_index, r);
	}
#endif

	reslevel = &tc->reslevels[r];
	if (reslevel->try1-reslevel->try0 <= 5) { // Less than 5 lines
		if (reslevel->y == reslevel->try0) { // Do wavelet transform at first reach
			jpc_linebufp curline, curlow, curhigh;
			curline = reslevel->lines.cur;
			curlow = reslevel->lows.cur;
			curhigh = reslevel->highs.cur;

			for (y=reslevel->try0; y<reslevel->try1; y++) {
				if (y&1) {
					jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
					jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
				} else {
					if (r == 1) {
						jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
					} else {
						jpc_dwt_inrev_line(jpcdec, tc, r-1, reslevel->templow.buf);
					}
					jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
				}

				ptr1.buf32 = curlow->buf.buf32;
				ptr2.buf32 = reslevel->templow.buf.buf32;
				ptr3.buf32 = reslevel->temphigh.buf.buf32;
				jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
				ptr2.buf32 = curhigh->buf.buf32+reslevel->high_width-1;
				jpx_dwt_inrev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);

				curline = curline->next;
				curlow = curlow->next;
				curhigh = curhigh->next;
			}

			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			lows[3] = reslevel->lows.cur->next->next->next->buf;
			lows[4] = reslevel->lows.cur->next->next->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			highs[3] = reslevel->highs.cur->next->next->next->buf;
			highs[4] = reslevel->highs.cur->next->next->next->next->buf;

			// Performing lifting factors.
			switch (reslevel->try1-reslevel->try0) {
			case 5:
				if (reslevel->try0&1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
						// Scaling (STEP1 and STEP2)
						lows[0].buf32[i] = JPX_FIXDIV(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXMUL(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXDIV(lows[2].buf32[i], JPX_KAPPA);
						lows[3].buf32[i] = JPX_FIXMUL(lows[3].buf32[i], JPX_KAPPA);
						lows[4].buf32[i] = JPX_FIXDIV(lows[4].buf32[i], JPX_KAPPA);
						
						// Lifting (STEP3)
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_DELTA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_DELTA);
						
						// Lifting (STEP4)
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_GAMMA);
						lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_GAMMA);
						
						// Lifting (STEP5)
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_BETA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_BETA);
						
						// Lifting (STEP6)
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_ALPHA);
						lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_ALPHA);
					}
				} else {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXMUL(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXDIV(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXMUL(lows[2].buf32[i], JPX_KAPPA);
						lows[3].buf32[i] = JPX_FIXDIV(lows[3].buf32[i], JPX_KAPPA);
						lows[4].buf32[i] = JPX_FIXMUL(lows[4].buf32[i], JPX_KAPPA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_DELTA);
						lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_DELTA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_GAMMA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_GAMMA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_BETA);
						lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_BETA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_ALPHA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_ALPHA);
					}
				}
				break;
			case 4:
				if (reslevel->try0&1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXDIV(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXMUL(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXDIV(lows[2].buf32[i], JPX_KAPPA);
						lows[3].buf32[i] = JPX_FIXMUL(lows[3].buf32[i], JPX_KAPPA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_DELTA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]<<1, JPX_DELTA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_GAMMA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_BETA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]<<1, JPX_BETA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_ALPHA);
					}
				} else {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXMUL(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXDIV(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXMUL(lows[2].buf32[i], JPX_KAPPA);
						lows[3].buf32[i] = JPX_FIXDIV(lows[3].buf32[i], JPX_KAPPA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_DELTA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_GAMMA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]<<1, JPX_GAMMA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_BETA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_ALPHA);
						lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]<<1, JPX_ALPHA);
					}
				}
				break;
			case 3:
				if (reslevel->try0&1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
						lows[0].buf32[i] = JPX_FIXDIV(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXMUL(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXDIV(lows[2].buf32[i], JPX_KAPPA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_DELTA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_BETA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
					}
				} else {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXMUL(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXDIV(lows[1].buf32[i], JPX_KAPPA);
						lows[2].buf32[i] = JPX_FIXMUL(lows[2].buf32[i], JPX_KAPPA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_GAMMA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
						lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_ALPHA);
					}
				}
				break;
			case 2:
				if (reslevel->try0&1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXDIV(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXMUL(lows[1].buf32[i], JPX_KAPPA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]<<1, JPX_DELTA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]<<1, JPX_BETA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
					}
				} else {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
						lows[0].buf32[i] = JPX_FIXMUL(lows[0].buf32[i], JPX_KAPPA);
						lows[1].buf32[i] = JPX_FIXDIV(lows[1].buf32[i], JPX_KAPPA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]<<1, JPX_GAMMA);
						
						lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
						
						lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]<<1, JPX_ALPHA);
					}
				}
				break;
			case 1:
				if (reslevel->try0 & 1) {
					for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
							lows[0].buf32[i] >>= 1;
					}
				}
				break;
			}
		}
	} else { // More than 5 lines
		if (reslevel->y == reslevel->try0) { // Do special wavelet transform at first reach
			jpc_linebufp curline, curlow, curhigh;
			curline = reslevel->lines.cur;
			curlow = reslevel->lows.cur;
			curhigh = reslevel->highs.cur;

			for (k=0, y=reslevel->try0; k<5; k++,y++) { // Decode the first 5 lines to use row-based transform
				if (y&1) {
					jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
					jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
				} else {
					if (r == 1) {
						jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
					} else {
						jpc_dwt_inrev_line(jpcdec, tc, r-1, reslevel->templow.buf);
					}
					jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
				}
				
				ptr1.buf32 = curlow->buf.buf32;
				ptr2.buf32 = reslevel->templow.buf.buf32;
				ptr3.buf32 = reslevel->temphigh.buf.buf32;
				jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
				ptr2.buf32 = curhigh->buf.buf32+reslevel->high_width-1;
				jpx_dwt_inrev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
			
				curline = curline->next;
				curlow = curlow->next;
				curhigh = curhigh->next;
			}

			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			lows[3] = reslevel->lows.cur->next->next->next->buf;
			lows[4] = reslevel->lows.cur->next->next->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			highs[3] = reslevel->highs.cur->next->next->next->buf;
			highs[4] = reslevel->highs.cur->next->next->next->next->buf;

			// Performing lifting factors.
			if (reslevel->try0&1) {
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
					lows[0].buf32[i] = JPX_FIXDIV(lows[0].buf32[i], JPX_KAPPA);
					lows[1].buf32[i] = JPX_FIXMUL(lows[1].buf32[i], JPX_KAPPA);
					lows[2].buf32[i] = JPX_FIXDIV(lows[2].buf32[i], JPX_KAPPA);
					lows[3].buf32[i] = JPX_FIXMUL(lows[3].buf32[i], JPX_KAPPA);
					lows[4].buf32[i] = JPX_FIXDIV(lows[4].buf32[i], JPX_KAPPA);
					
					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_DELTA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_DELTA);

					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_GAMMA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_GAMMA);

					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_BETA);

					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_ALPHA);
				}
			} else {
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {
					lows[0].buf32[i] = JPX_FIXMUL(lows[0].buf32[i], JPX_KAPPA);
					lows[1].buf32[i] = JPX_FIXDIV(lows[1].buf32[i], JPX_KAPPA);
					lows[2].buf32[i] = JPX_FIXMUL(lows[2].buf32[i], JPX_KAPPA);
					lows[3].buf32[i] = JPX_FIXDIV(lows[3].buf32[i], JPX_KAPPA);
					lows[4].buf32[i] = JPX_FIXMUL(lows[4].buf32[i], JPX_KAPPA);

					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_DELTA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_DELTA);
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i], JPX_DELTA);

					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_GAMMA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i], JPX_GAMMA);

					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]<<1, JPX_BETA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i], JPX_BETA);

					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i], JPX_ALPHA);
				}
			}
		} else if (reslevel->y == reslevel->try1-5) { // Do special wavelet transform when reached final lines.
			if (reslevel->y&1) {
				jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
				jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
			} else {
				if (r == 1) {
					jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
				} else {
					jpc_dwt_inrev_line(jpcdec, tc, r-1, reslevel->templow.buf);
				}
				jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
			}
			
			ptr1.buf32 = reslevel->lows.cur->prev->buf.buf32;
			ptr2.buf32 = reslevel->templow.buf.buf32;
			ptr3.buf32 = reslevel->temphigh.buf.buf32;
			jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
			ptr2.buf32 = reslevel->highs.cur->prev->buf.buf32+reslevel->high_width-1;
			jpx_dwt_inrev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
			
			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			lows[3] = reslevel->lows.cur->next->next->next->buf;
			lows[4] = reslevel->lows.cur->next->next->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			highs[3] = reslevel->highs.cur->next->next->next->buf;
			highs[4] = reslevel->highs.cur->next->next->next->next->buf;
			
			// Performing lifting factors.
			if (reslevel->y&1) {
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
					lows[4].buf32[i] = JPX_FIXDIV(lows[4].buf32[i], JPX_KAPPA);
					
					lows[3].buf32[i] -= JPX_FIXMUL(lows[4].buf32[i], JPX_DELTA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i], JPX_GAMMA);
					lows[1].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i], JPX_BETA);
					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i], JPX_ALPHA);
					
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_GAMMA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_BETA);
					
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_ALPHA);
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_ALPHA);
				}
			} else {
				for (i=0; i<reslevel->low_width+reslevel->high_width; i++) {	
					lows[4].buf32[i] = JPX_FIXMUL(lows[4].buf32[i], JPX_KAPPA);
					
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_DELTA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_GAMMA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i]+lows[3].buf32[i], JPX_BETA);
					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i]+lows[2].buf32[i], JPX_ALPHA);
					
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i]<<1, JPX_BETA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i]+lows[4].buf32[i], JPX_ALPHA);
				}
			}
		} else if (reslevel->y < reslevel->try1-5) { // Step for row-based wavelet transform for intermediate lines.
			if (reslevel->y&1) {
				jpc_decode_subband_line(jpcdec, tc, r, 1, reslevel->templow.buf);
				jpc_decode_subband_line(jpcdec, tc, r, 2, reslevel->temphigh.buf);
			} else {
				if (r == 1) {
					jpc_decode_subband_line(jpcdec, tc, 0, 0, reslevel->templow.buf);
				} else {
					jpc_dwt_inrev_line(jpcdec, tc, r-1, reslevel->templow.buf);
				}
				jpc_decode_subband_line(jpcdec, tc, r, 0, reslevel->temphigh.buf);
			}
			
			ptr1.buf32 = reslevel->lows.cur->prev->buf.buf32;
			ptr2.buf32 = reslevel->templow.buf.buf32;
			ptr3.buf32 = reslevel->temphigh.buf.buf32;
			jpx_dwt_interleave(ptr1, ptr2, ptr3, reslevel->low_width+reslevel->high_width, reslevel->trx0);
			ptr2.buf32 = reslevel->highs.cur->prev->buf.buf32+reslevel->high_width-1;
			jpx_dwt_inrev_horz(ptr1, ptr2, reslevel->trx0, reslevel->trx1);
		
			lows[0] = reslevel->lows.cur->buf;
			lows[1] = reslevel->lows.cur->next->buf;
			lows[2] = reslevel->lows.cur->next->next->buf;
			lows[3] = reslevel->lows.cur->next->next->next->buf;
			lows[4] = reslevel->lows.cur->next->next->next->next->buf;
			highs[0] = reslevel->highs.cur->buf;
			highs[1] = reslevel->highs.cur->next->buf;
			highs[2] = reslevel->highs.cur->next->next->buf;
			highs[3] = reslevel->highs.cur->next->next->next->buf;
			highs[4] = reslevel->highs.cur->next->next->next->next->buf;
			
			if (reslevel->y&1) {
				for(i = 0; i < reslevel->low_width+reslevel->high_width; i++) {
					lows[4].buf32[i] = JPX_FIXDIV(lows[4].buf32[i], JPX_KAPPA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[4].buf32[i], JPX_DELTA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i], JPX_GAMMA);
					lows[1].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i], JPX_BETA);
					lows[0].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i], JPX_ALPHA);
				}
			} else {
				for(i = 0; i < reslevel->low_width+reslevel->high_width; i++) {
					lows[4].buf32[i] = JPX_FIXMUL(lows[4].buf32[i], JPX_KAPPA);
					lows[4].buf32[i] -= JPX_FIXMUL(lows[3].buf32[i], JPX_DELTA);
					lows[3].buf32[i] -= JPX_FIXMUL(lows[2].buf32[i], JPX_GAMMA);
					lows[2].buf32[i] -= JPX_FIXMUL(lows[1].buf32[i], JPX_BETA);
					lows[1].buf32[i] -= JPX_FIXMUL(lows[0].buf32[i], JPX_ALPHA);
				}
			}
		}
	}
	
	if (reslevel->y < reslevel->try1) { // Transfer data for row
		jpx_memcpy(line.buf32, reslevel->lines.cur->buf.buf32+4, (reslevel->trx1-reslevel->trx0)*sizeof(jpx_int32));

#ifdef _JPX_ENABLE_SYNTHESIS_VERT_LOG_
		for (i=0; i<reslevel->trx1-reslevel->trx0; i++) {
			JPX_LOG("%d\n", line.buf32[i]);
		}
#endif
	} else {
		jpx_memset(line.buf32, 0, (reslevel->trx1-reslevel->trx0)*sizeof(jpx_int32));
	}
	
	reslevel->y++;
	reslevel->lines.cur = reslevel->lines.cur->next;
	reslevel->lows.cur = reslevel->lows.cur->next;
	reslevel->highs.cur = reslevel->highs.cur->next;

	return jpx_true;
}

jpx_bool					jpc_decode_resolution_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_coeff_line line)
{
	if (tc->transformation) {
		if (r == 0) {
			return jpc_decode_subband_line(jpcdec, tc, 0, 0, line);
		} else {
			return jpc_dwt_rev_line(jpcdec, tc, r, line);
		}
	} else {
		if (r == 0) {
			return jpc_decode_subband_line(jpcdec, tc, 0, 0, line);
		} else {
			return jpc_dwt_inrev_line(jpcdec, tc, r, line);
		}
	}
}
jpx_bool				jpc_decode_subband_line(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_uint32 bnd, jpx_coeff_line line)
{
	jpc_subbandp subband;

#ifdef _JPX_ENABLE_SYNTHESIS_LOG_
	{
		jpx_uint16 comp_index, tile_index;
		for (tile_index=0; tile_index<jpcdec->tilenum; tile_index++) {
			for (comp_index=0; comp_index<jpcdec->component_num; comp_index++) {
				if (&jpcdec->tiles[tile_index].components[comp_index] == tc) {
					goto ADDLOG;
				}
			}
		}
ADDLOG:
		JPX_LOG("decoding suband: tileidx=%d, compidx=%d, r=%d, bnd=%d\n", tile_index, comp_index, r, bnd);
	}
#endif
	
	subband = &tc->reslevels[r].subbands[bnd];
	if ((!subband->refs) || (subband->y >= subband->tby1)) return jpx_false;
	
	if (subband->used_height >= subband->current_decoded_height) {
		if (!jpc_decode_codeblocks_row(jpcdec, tc, r, bnd)) {
			return jpx_false;
		}
	}

	jpx_memcpy(line.buf32,subband->data+subband->used_height*subband->data_width,
		subband->data_width*sizeof(jpx_int32));	

	subband->used_height++;
	subband->y++;

	return jpx_true;
}

jpx_bool				jpc_decode_codeblocks_row(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r, jpx_uint32 sb)
{
	jpc_subbandp subband;
	jpx_uint32 cbx, x;
	jpc_codeblockp cb;

	subband = &tc->reslevels[r].subbands[sb];
	if (subband->cby >= subband->vcodeblock_num) return jpx_false;

	jpx_memset(subband->data, 0, subband->data_width*subband->data_height*sizeof(jpx_int32));
	for (cbx=0, x=subband->tbx0; cbx < subband->hcodeblock_num; cbx++) {
		cb = subband->refs[subband->cby*subband->hcodeblock_num+cbx];
		if (!jpc_decode_codeblock(jpcdec, tc, r, sb, cb, subband->data+x-subband->tbx0, subband->data_width)) {
			return jpx_false;
		}
		x += cb->x1 - cb->x0;
	}

	cb = subband->refs[subband->cby*subband->hcodeblock_num];
	subband->current_decoded_height = cb->y1 - cb->y0;
	subband->used_height = 0;

	subband->cby++;

	return jpx_true;	
}
