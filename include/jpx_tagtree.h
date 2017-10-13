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

#ifndef _JPX_TAGTREE_H_
#define _JPX_TAGTREE_H_

typedef struct jpx_tagtree_node_struct FAR * jpx_tagtree_nodep;
							
typedef struct jpx_tagtree_node_struct {
	jpx_tagtree_nodep		parent;
	jpx_bool				done;
	jpx_uint32				val;
} jpx_tagtree_node;
typedef jpx_tagtree_node FAR * FAR * jpx_tagtree_nodepp;

typedef struct jpx_tagtree_struct {
	jpx_uint32				xleafnum;
	jpx_uint32				yleafnum;
	jpx_uint32				nodenum;
	jpx_tagtree_nodep		nodes;
} jpx_tagtree;
typedef jpx_tagtree FAR * jpx_tagtreep;

jpx_tagtreep				jpx_tagtree_create(jpx_memmgrp memmgr, jpx_uint32 xleafnum, jpx_uint32 yleafnum);
void						jpx_tagtree_destroy(jpx_memmgrp memmgr, jpx_tagtreep tagtree);
jpx_uint32					jpx_tagtree_decode(jpx_tagtreep tagtree, jpx_bitstuffp bitstuff, jpx_uint32 xleaf, jpx_uint32 yleaf, jpx_uint32 threshold);

#endif	// _JPX_TAGTREE_H_
