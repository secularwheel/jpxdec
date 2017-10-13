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
#include "../include/jpx_bitstream.h"

jpx_bitstreamp			jpx_bitstream_create(jpx_memmgrp memmgr, jpx_streamp stream)
{
	jpx_bitstreamp bitstream = (jpx_bitstreamp)memmgr->alloc_func(memmgr, sizeof(jpx_bitstream));
	bitstream->stream = stream;
	bitstream->stream_pos = stream->getpos_func(stream);
	bitstream->size = stream->getsize_func(stream)-bitstream->stream_pos;
	bitstream->pos = 0;
	bitstream->bit_pos = 0;
	bitstream->buffer_offset = 0;
	bitstream->buffer_size = JPX_BUFFER_SIZE;
	stream->read_func(stream, bitstream->buffer, 
		bitstream->pos+bitstream->buffer_size<bitstream->size ? bitstream->buffer_size:bitstream->size);
	return bitstream;
}
void					jpx_bitstream_destroy(jpx_memmgrp memmgr, jpx_bitstreamp bitstream)
{
	memmgr->free_func(memmgr, bitstream);
}
jpx_uint32				jpx_bitstream_getsize(jpx_bitstreamp bitstream)
{
	return bitstream->size;
}
jpx_uint32				jpx_bitstream_getpos(jpx_bitstreamp bitstream)
{
	return bitstream->pos;
}
void					jpx_bitstream_setpos(jpx_bitstreamp bitstream, jpx_uint32 pos)
{
	bitstream->pos = pos;
}
jpx_uint32				jpx_bitstream_getbitpos(jpx_bitstreamp bitstream)
{
	return (bitstream->pos<<3)+bitstream->bit_pos;
}
void					jpx_bitstream_setbitpos(jpx_bitstreamp bitstream, jpx_uint32 bitpos)
{
	bitstream->pos = bitpos>>3;
	bitstream->bit_pos = bitpos&7;
}
jpx_bool				jpx_bitstream_readbyte(jpx_bitstreamp bitstream, jpx_bytep value)
{
	if (!jpx_bitstream_getbyte(bitstream, value)) {
		return jpx_false;
	}
	++bitstream->pos;
	return jpx_true;
}
jpx_bool				jpx_bitstream_readshort(jpx_bitstreamp bitstream, jpx_uint16p value)
{
	jpx_byte b1, b2;
	if (!jpx_bitstream_readbyte(bitstream, &b1)) {
		return jpx_false;
	}
	if (!jpx_bitstream_readbyte(bitstream, &b2)) {
		return jpx_false;
	}
	*value = (b1<<8)|b2;
	return jpx_true;
}
jpx_bool				jpx_bitstream_readinteger(jpx_bitstreamp bitstream, jpx_uint32p value)
{
	jpx_byte b1, b2, b3, b4;
	if (!jpx_bitstream_readbyte(bitstream, &b1)) {
		return jpx_false;
	}
	if (!jpx_bitstream_readbyte(bitstream, &b2)) {
		return jpx_false;
	}
	if (!jpx_bitstream_readbyte(bitstream, &b3)) {
		return jpx_false;
	}
	if (!jpx_bitstream_readbyte(bitstream, &b4)) {
		return jpx_false;
	}
	*value = (b1<<24)|(b2<<16)|(b3<<8)|b4;
	return jpx_true;
}
jpx_bool				jpx_bitstream_readbits(jpx_bitstreamp bitstream, jpx_uint32 num, jpx_uint32p value)
{
	jpx_byte b;

	if (!jpx_bitstream_getbyte(bitstream, &b)) {
		return jpx_false;
	}

	*value = 0;
	while (num > 0) {
		*value = (*value<<1)|((b>>(7-bitstream->bit_pos))&0x01);
		if (bitstream->bit_pos == 7) {
			bitstream->bit_pos = 0;
			bitstream->pos ++;
			if (!jpx_bitstream_getbyte(bitstream, &b)) {
				return jpx_false;
			}
		} else {
			bitstream->bit_pos ++;
		}
		num --;
	}
	return jpx_true;
}
jpx_bool				jpx_bitstream_readbit(jpx_bitstreamp bitstream, jpx_boolp value)
{
	jpx_byte b;
	
	if (!jpx_bitstream_getbyte(bitstream, &b)) {
		return jpx_false;
	}

	*value = (b >> (7 - bitstream->bit_pos)) & 0x01;
	if (bitstream->bit_pos == 7) {
		bitstream->bit_pos = 0;
		bitstream->pos ++;
	} else {
		bitstream->bit_pos ++;
	}
	return jpx_true;
}
jpx_bool				jpx_bitstream_readbuf(jpx_bitstreamp bitstream, jpx_voidp buf, jpx_uint32 size)
{
	if (bitstream->pos+size > bitstream->size) {
		return jpx_false;
	}
	
	// Make a seek first.
	if (!bitstream->stream->seekpos_func(bitstream->stream, bitstream->stream_pos+bitstream->pos)) {
		return jpx_false;
	}
	// We always read buffer from stream directly.
	if (!bitstream->stream->read_func(bitstream->stream, buf, size)) {
		return jpx_false;
	}
	jpx_bitstream_setpos(bitstream, bitstream->pos+size);
	return jpx_true;
}
void					jpx_bitstream_alignbyte(jpx_bitstreamp bitstream)
{
	if (bitstream->bit_pos != 0) {
		bitstream->bit_pos = 0;
		bitstream->pos ++;
	}
}
void					jpx_bitstream_align4byte(jpx_bitstreamp bitstream)
{
	if (bitstream->bit_pos != 0) {
		bitstream->bit_pos = 0;
		bitstream->pos ++;
	}
	bitstream->pos = (bitstream->pos+3)&-4;
}
jpx_bool				jpx_bitstream_getbyte(jpx_bitstreamp bitstream, jpx_bytep value)
{
	if (bitstream->pos >= bitstream->size) { // Stream end
		return jpx_false;
	}

	if (bitstream->pos<bitstream->buffer_offset || bitstream->pos>=bitstream->buffer_offset+bitstream->buffer_size) {
		jpx_uint32 bytesleft, bytesread;
		// Make a seek first.
		if (!bitstream->stream->seekpos_func(bitstream->stream, bitstream->stream_pos+bitstream->pos)) {
			return jpx_false;
		}
		// Pos out of buffer, then we re-prepare the buffer.
		bytesleft = jpx_bitstream_getbytesleft(bitstream);
		bytesread = bytesleft < bitstream->buffer_size ? bytesleft : bitstream->buffer_size;
		if (!bitstream->stream->read_func(bitstream->stream, bitstream->buffer, bytesread)) {
			return jpx_false;
		}
		bitstream->buffer_offset = bitstream->pos;
	}

	// Got the byte and auto-increase pos.
	*value = bitstream->buffer[bitstream->pos-bitstream->buffer_offset];
	return jpx_true;
}
jpx_bool				jpx_bitstream_getbyteat(jpx_bitstreamp bitstream, jpx_uint32 pos, jpx_bytep value)
{
	jpx_uint32 savepos = bitstream->pos;
	jpx_bool ret = jpx_bitstream_getbyte(bitstream, value);
	bitstream->pos = savepos;
	return ret;
}
jpx_bool				jpx_bitstream_getshort(jpx_bitstreamp bitstream, jpx_uint16p value)
{
	jpx_uint32 savepos = bitstream->pos;
	jpx_byte b1, b2;
	
	if (!jpx_bitstream_getbyte(bitstream, &b1)) {
		return jpx_false;
	}
	bitstream->pos ++;
	if (!jpx_bitstream_getbyte(bitstream, &b2)) {
		bitstream->pos = savepos;
		return jpx_false;
	}
	*value = (b1<<8)|b2;
	bitstream->pos = savepos;
	return jpx_true;
}
jpx_uint32				jpx_bitstream_getbytesleft(jpx_bitstreamp bitstream)
{
	return bitstream->size-bitstream->pos;
}
void					jpx_bitstream_rewind(jpx_bitstreamp bitstream)
{
	bitstream->pos = 0;
	bitstream->bit_pos = 0;
}
