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

void					jpx_convert_greyscale_to_rgb(jpx_bytepp bufs, jpx_uint32 pixgap, jpx_uint16 channum, jpx_bytepp lines, jpx_uint32 length)
{
	jpx_uint16 chan;
	jpx_bytep dst, src;
	jpx_uint32 i;
	
	for (chan=0; chan < channum; chan++) {
		src = lines[chan];
		dst = bufs[chan];
		for (i=0; i < length; i++) {
			*dst = *src++;
			dst += pixgap;
		}
	}
}
void					jpx_convert_cmyk_to_rgb(jpx_bytepp bufs, jpx_uint32 pixgap, jpx_uint16 channum, jpx_bytepp lines, jpx_uint32 length)
{
	jpx_uint32 i;
	jpx_bytep c, m, y, k;
	
	c = lines[0];
	m = lines[1];
	y = lines[2];
	k= lines[3];
	
	for (i=0; i < length; i++) {
		*bufs[0] = 255 - (255<*c+*k ? 255:*c+*k);
		*bufs[1] = 255 - (255<*m+*k ? 255:*m+*k);
		*bufs[2] = 255 - (255<*y+*k ? 255:*y+*k);
		
		bufs[0] += pixgap;
		bufs[1] += pixgap;
		bufs[2] += pixgap;
		c++;
		m++;
		y++;
		k++;
	}
}
void					jpx_convert_sycc_to_rgb(jpx_bytepp bufs, jpx_uint32 pixgap, jpx_uint16 channum, jpx_bytepp lines, jpx_uint32 length)
{
	jpx_uint32 i;
	jpx_bytep y, cb, cr;
	
	y = lines[0];
	cr = lines[1];
	cb = lines[2];
	
	for (i=0; i < length; i++) {
		*bufs[0] = (jpx_byte)(*y + 1.402**cb + 0.5);
		*bufs[1] = (jpx_byte)(*y - 0.34413**cr - 0.71414**cb + 0.5);
		*bufs[2] = (jpx_byte)(*y + 1.772**cr + 0.5);
		
		bufs[0] += pixgap;
		bufs[1] += pixgap;
		bufs[2] += pixgap;
		y++;
		cr++;
		cb++;
	}
}
