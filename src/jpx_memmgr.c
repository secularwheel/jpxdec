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
						
static jpx_voidp		jpx_memmgr_alloc(jpx_memmgrp memmgr, jpx_uint32 size)
{
	return malloc(size);
}
static jpx_voidp		jpx_memmgr_realloc(jpx_memmgrp memmgr, jpx_voidp buf, jpx_uint32 size)
{
	return realloc(buf, size);
}
static void				jpx_memmgr_free(jpx_memmgrp memmgr, jpx_voidp buf)
{
	free(buf);
}
static void				jpx_memmgr_destroy(jpx_memmgrp memmgr)
{
	free(memmgr);
}

jpx_memmgrp				jpx_create_default_memmgr()
{
	jpx_memmgrp memmgr = (jpx_memmgrp)malloc(sizeof(jpx_memmgr));
	memmgr->alloc_func = jpx_memmgr_alloc;
	memmgr->realloc_func = jpx_memmgr_realloc;
	memmgr->free_func = jpx_memmgr_free;
	memmgr->destroy_func = jpx_memmgr_destroy;
	return memmgr;
}

