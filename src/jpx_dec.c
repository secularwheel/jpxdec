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

jpx_bool				jpx_is_jp2(jpx_streamp stream)
{
	const jpx_byte jp2signature[] = {0x00,0x00,0x00,0x0C,0x6A,0x50,0x20,0x20,0x0D,0x0A,0x87,0x0A};
	jpx_byte buf[12];
	jpx_uint32 savepos = stream->getpos_func(stream);

	if (!stream->read_func(stream, buf, 12) || jpx_memcmp(buf, jp2signature, 12)) {
		stream->seekpos_func(stream, savepos);
		return jpx_false;
	}
	stream->seekpos_func(stream, savepos);
	return jpx_true;
}
jpx_bool				jpx_is_jpc(jpx_streamp stream)
{
	jpx_byte buf[2];
	jpx_uint32 savepos = stream->getpos_func(stream);

	if (!stream->read_func(stream, buf, 2) || !((buf[0]==0xff) && (buf[1]==0x4f))) {
		stream->seekpos_func(stream, savepos);
		return jpx_false;
	}
	stream->seekpos_func(stream, savepos);
	return jpx_true;
}

