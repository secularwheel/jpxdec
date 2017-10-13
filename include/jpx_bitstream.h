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

#ifndef _JPX_BITSTREAM_H_
#define _JPX_BITSTREAM_H_

//--------------------------------------------------------------------------------
//			Bit stream class
//--------------------------------------------------------------------------------
typedef struct jpx_bitstream_struct {
	jpx_streamp				stream;
	jpx_uint32				stream_pos;					// Bit stream pos in stream
	jpx_uint32				size;
	jpx_uint32				pos;						// 0~size-1
	jpx_uint32				bit_pos;					// 0~7
	jpx_uint32				buffer_offset;
	jpx_uint32				buffer_size;				// <=JPX_BUFFER_SIZE
	jpx_byte				buffer[JPX_BUFFER_SIZE];
} jpx_bitstream;
typedef jpx_bitstream FAR * jpx_bitstreamp;

jpx_bitstreamp			jpx_bitstream_create(jpx_memmgrp memmgr, jpx_streamp stream);
void					jpx_bitstream_destroy(jpx_memmgrp memmgr, jpx_bitstreamp bitstream);
jpx_uint32				jpx_bitstream_getsize(jpx_bitstreamp bitstream);
jpx_uint32				jpx_bitstream_getpos(jpx_bitstreamp bitstream);
void					jpx_bitstream_setpos(jpx_bitstreamp bitstream, jpx_uint32 pos);
jpx_uint32				jpx_bitstream_getbitpos(jpx_bitstreamp bitstream);
void					jpx_bitstream_setbitpos(jpx_bitstreamp bitstream, jpx_uint32 bitpos);
jpx_bool				jpx_bitstream_readbyte(jpx_bitstreamp bitstream, jpx_bytep value);
jpx_bool				jpx_bitstream_readshort(jpx_bitstreamp bitstream, jpx_uint16p value);
jpx_bool				jpx_bitstream_readinteger(jpx_bitstreamp bitstream, jpx_uint32p value);
jpx_bool				jpx_bitstream_readbits(jpx_bitstreamp bitstream, jpx_uint32 num, jpx_uint32p value);
jpx_bool				jpx_bitstream_readbit(jpx_bitstreamp bitstream, jpx_boolp value);
jpx_bool				jpx_bitstream_readbuf(jpx_bitstreamp bitstream, jpx_voidp buf, jpx_uint32 size);
void					jpx_bitstream_alignbyte(jpx_bitstreamp bitstream);
void					jpx_bitstream_align4byte(jpx_bitstreamp bitstream);
jpx_bool				jpx_bitstream_getbyte(jpx_bitstreamp bitstream, jpx_bytep value);
jpx_bool				jpx_bitstream_getbyteat(jpx_bitstreamp bitstream, jpx_uint32 pos, jpx_bytep value);
jpx_bool				jpx_bitstream_getshort(jpx_bitstreamp bitstream, jpx_uint16p value);
jpx_uint32				jpx_bitstream_getbytesleft(jpx_bitstreamp bitstream);
void					jpx_bitstream_rewind(jpx_bitstreamp bitstream);

#endif // _JPX_BITSTREAM_H_
