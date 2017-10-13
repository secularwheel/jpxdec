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
#include "../include/jpx_tagtree.h"

#define JPX_TAGTREE_MAXDEPTH			32

void						jpx_tagtree_init(jpx_memmgrp memmgr, jpx_tagtreep tagtree);

jpx_tagtreep				jpx_tagtree_create(jpx_memmgrp memmgr, jpx_uint32 xleafnum, jpx_uint32 yleafnum)
{
	jpx_tagtreep tagtree = (jpx_tagtreep)memmgr->alloc_func(memmgr, sizeof(jpx_tagtree));
	jpx_memset(tagtree, 0, sizeof(jpx_tagtree));
	tagtree->xleafnum = xleafnum;
	tagtree->yleafnum = yleafnum;

	jpx_tagtree_init(memmgr, tagtree);

	return tagtree;
}
void						jpx_tagtree_init(jpx_memmgrp memmgr, jpx_tagtreep tagtree)
{
	jpx_uint32 levelnum, level, offset, x, y;
	jpx_uint32 xnodenum[JPX_TAGTREE_MAXDEPTH];
	jpx_uint32 ynodenum[JPX_TAGTREE_MAXDEPTH];
	jpx_tagtree_nodep node;
	jpx_uint32 leafnum, xleafnum, yleafnum;

	levelnum = 1;
	if (tagtree->xleafnum > tagtree->yleafnum) {
		leafnum = tagtree->xleafnum;
	} else {
		leafnum = tagtree->yleafnum;
	}
	while (leafnum > 1) {
		leafnum = (leafnum + 1) >> 1;
		levelnum ++;
	}

	xleafnum = tagtree->xleafnum;
	yleafnum = tagtree->yleafnum;
	for (level = 0; level < levelnum; level++) {
		xnodenum[level] = xleafnum;
		ynodenum[level] = yleafnum;
		tagtree->nodenum += xleafnum*yleafnum;
		xleafnum = (xleafnum + 1) >> 1;
		yleafnum = (yleafnum + 1) >> 1;
	}
	tagtree->nodes = (jpx_tagtree_nodep)memmgr->alloc_func(memmgr, tagtree->nodenum*sizeof(jpx_tagtree_node));
	jpx_memset(tagtree->nodes, 0, tagtree->nodenum*sizeof(jpx_tagtree_node));

	if (levelnum > 1) {
		for (y = 0; y < tagtree->yleafnum; y++) {
			for (x = 0; x < tagtree->xleafnum; x++) {
				node = tagtree->nodes;
				xleafnum = x;
				yleafnum = y;
				for (level = 0; level < levelnum-1; level++) {
					offset = yleafnum*xnodenum[level] + xleafnum;
					if (node[offset].parent) {
						goto NEXT;
					} else {
						node[offset].parent = node + xnodenum[level]*ynodenum[level] 
							+ (yleafnum>>1)*xnodenum[level+1] + (xleafnum>>1);
					}
					node += xnodenum[level]*ynodenum[level];
					xleafnum >>= 1;
					yleafnum >>= 1;
				}
NEXT:			;
			}
		}
	}
}
void						jpx_tagtree_destroy(jpx_memmgrp memmgr, jpx_tagtreep tagtree)
{
	if (tagtree->nodes) {
		memmgr->free_func(memmgr, tagtree->nodes);
	}
	memmgr->free_func(memmgr, tagtree);
}
jpx_uint32					jpx_tagtree_decode(jpx_tagtreep tagtree, jpx_bitstuffp bitstuff, jpx_uint32 xleaf, jpx_uint32 yleaf, jpx_uint32 threshold)
{
	jpx_tagtree_nodep	nodearray[JPX_TAGTREE_MAXDEPTH];
	jpx_tagtree_nodep	node;
	jpx_tagtree_nodepp	nodeptr;
	jpx_uint32 val;

	nodeptr = nodearray;
	node = &tagtree->nodes[yleaf*tagtree->xleafnum + xleaf];
	while (node->parent) {
		*nodeptr = node;
		node = node->parent;
		nodeptr++;
	}

	val = 0;
	do {
		if (val < node->val) {
			val = node->val;
		} else {
			node->val = val;
		}

		while (!node->done && (val<threshold)) {
			if (jpx_bitstuff_readbit(bitstuff)) {
				node->done = 1;
				break;
			} else {
				val++;
			}
		}
		node->val = val;
		if (nodeptr == nodearray) break;
		--nodeptr;
		node = *nodeptr;
	} while (1);

	return node->val;
}
