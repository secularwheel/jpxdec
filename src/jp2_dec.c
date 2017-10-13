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
#include "../include/jpx_synthesis.h"

jpx_bool				jp2_dec_parse_boxes(jp2_decp jp2dec);
jpx_bool				jp2_dec_parse_boxheader(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_ftyp(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_jp2h(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_ihdr(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_bpcc(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_colr(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_pclr(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_cmap(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_cdef(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_res(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_resc(jp2_decp jp2dec, jp2_box_headerp boxheader);
jpx_bool				jp2_dec_parse_resd(jp2_decp jp2dec, jp2_box_headerp boxheader);
void					jp2_box_cdef_reorder(jp2_box_cdefp cdef, jpx_int32 l, jpx_int32 r);

void					jp2_transfer_data(jp2_decp jp2dec, jpx_bytepp bufs, jpx_uint32 pixgap, jpx_uint16 channum, jpx_bytepp lines, jpx_uint32 length);

jp2_box_ftypp			jp2_box_ftyp_create(jp2_decp jp2dec)
{
	jp2_box_ftypp ftyp = (jp2_box_ftypp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_ftyp));
	jpx_memset(ftyp, 0, sizeof(jp2_box_ftyp));
	return ftyp;
}
void					jp2_box_ftyp_destroy(jp2_decp jp2dec, jp2_box_ftypp ftyp)
{
	if (ftyp->cls) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, ftyp->cls);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, ftyp);
}
jp2_box_ihdrp			jp2_box_ihdr_create(jp2_decp jp2dec)
{
	jp2_box_ihdrp ihdr = (jp2_box_ihdrp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_ihdr));
	jpx_memset(ihdr, 0, sizeof(jp2_box_ihdr));
	return ihdr;
}
void					jp2_box_ihdr_destroy(jp2_decp jp2dec, jp2_box_ihdrp ihdr)
{
	jp2dec->memmgr->free_func(jp2dec->memmgr, ihdr);
}
jp2_box_bpccp			jp2_box_bpcc_create(jp2_decp jp2dec)
{
	jp2_box_bpccp bpcc = (jp2_box_bpccp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_bpcc));
	jpx_memset(bpcc, 0, sizeof(jp2_box_bpcc));
	return bpcc;
}
void					jp2_box_bpcc_destroy(jp2_decp jp2dec, jp2_box_bpccp bpcc)
{
	if (bpcc->bpcs) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, bpcc->bpcs);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, bpcc);
}
jp2_box_colrp			jp2_box_colr_create(jp2_decp jp2dec)
{
	jp2_box_colrp colr = (jp2_box_colrp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_colr));
	jpx_memset(colr, 0, sizeof(jp2_box_colr));
	return colr;
}
void					jp2_box_colr_destroy(jp2_decp jp2dec, jp2_box_colrp colr)
{
	if (colr->profile) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, colr->profile);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, colr);
}
jp2_box_pclrp			jp2_box_pclr_create(jp2_decp jp2dec)
{
	jp2_box_pclrp pclr = (jp2_box_pclrp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_pclr));
	jpx_memset(pclr, 0, sizeof(jp2_box_pclr));
	return pclr;
}
void					jp2_box_pclr_destroy(jp2_decp jp2dec, jp2_box_pclrp pclr)
{
	if (pclr->bs) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, pclr->bs);
	}
	if (pclr->cs) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, pclr->cs);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, pclr);
}
jp2_box_cmapp			jp2_box_cmap_create(jp2_decp jp2dec)
{
	jp2_box_cmapp cmap = (jp2_box_cmapp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_cmap));
	jpx_memset(cmap, 0, sizeof(jp2_box_cmap));
	return cmap;
}
void					jp2_box_cmap_destroy(jp2_decp jp2dec, jp2_box_cmapp cmap)
{
	if (cmap->cmps) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cmap->cmps);
	}
	if (cmap->mtyps) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cmap->mtyps);
	}
	if (cmap->pcols) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cmap->pcols);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, cmap);
}
jp2_box_cdefp			jp2_box_cdef_create(jp2_decp jp2dec)
{
	jp2_box_cdefp cdef = (jp2_box_cdefp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_cdef));
	jpx_memset(cdef, 0, sizeof(jp2_box_cdef));
	return cdef;
}
void					jp2_box_cdef_destroy(jp2_decp jp2dec, jp2_box_cdefp cdef)
{
	if (cdef->cns) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cdef->cns);
	}
	if (cdef->typs) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cdef->typs);
	}
	if (cdef->asocs) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, cdef->asocs);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, cdef);
}
jp2_box_rescp			jp2_box_resc_create(jp2_decp jp2dec)
{
	jp2_box_rescp resc = (jp2_box_rescp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_resc));
	jpx_memset(resc, 0, sizeof(jp2_box_resc));
	return resc;
}
void					jp2_box_resc_destroy(jp2_decp jp2dec, jp2_box_rescp resc)
{
	jp2dec->memmgr->free_func(jp2dec->memmgr, resc);
}
jp2_box_resdp			jp2_box_resd_create(jp2_decp jp2dec)
{
	jp2_box_resdp resd = (jp2_box_resdp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_resd));
	jpx_memset(resd, 0, sizeof(jp2_box_resd));
	return resd;
}
void					jp2_box_resd_destroy(jp2_decp jp2dec, jp2_box_resdp resd)
{
	jp2dec->memmgr->free_func(jp2dec->memmgr, resd);
}
jp2_box_resp			jp2_box_res_create(jp2_decp jp2dec)
{
	jp2_box_resp res = (jp2_box_resp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_res));
	jpx_memset(res, 0, sizeof(jp2_box_res));
	return res;
}
void					jp2_box_res_destroy(jp2_decp jp2dec, jp2_box_resp res)
{
	if (res->resc) {
		jp2_box_resc_destroy(jp2dec, res->resc);
	}
	if (res->resd) {
		jp2_box_resd_destroy(jp2dec, res->resd);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, res);
}
jp2_box_jp2hp			jp2_box_jp2h_create(jp2_decp jp2dec)
{
	jp2_box_jp2hp jp2h = (jp2_box_jp2hp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_box_jp2h));
	jpx_memset(jp2h, 0, sizeof(jp2_box_jp2h));
	return jp2h;
}
void					jp2_box_jp2h_destroy(jp2_decp jp2dec, jp2_box_jp2hp jp2h)
{
	if (jp2h->ihdr) {
		jp2_box_ihdr_destroy(jp2dec, jp2h->ihdr);
	}
	if (jp2h->bpcc) {
		jp2_box_bpcc_destroy(jp2dec, jp2h->bpcc);
	}
	if (jp2h->colr) {
		jp2_box_colr_destroy(jp2dec, jp2h->colr);
	}
	if (jp2h->pclr) {
		jp2_box_pclr_destroy(jp2dec, jp2h->pclr);
	}
	if (jp2h->cmap) {
		jp2_box_cmap_destroy(jp2dec, jp2h->cmap);
	}
	if (jp2h->cdef) {
		jp2_box_cdef_destroy(jp2dec, jp2h->cdef);
	}
	if (jp2h->res) {
		jp2_box_res_destroy(jp2dec, jp2h->res);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, jp2h);
}
jp2_channel_listp		jp2_channel_list_create(jp2_decp jp2dec)
{
	jp2_channel_listp channellist = (jp2_channel_listp)jp2dec->memmgr->alloc_func(jp2dec->memmgr, sizeof(jp2_channel_list));
	jpx_memset(channellist, 0, sizeof(jp2_channel_list));
	return channellist;
}
void					jp2_channel_list_destroy(jp2_decp jp2dec, jp2_channel_listp channellist)
{
	if (channellist->channels) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, channellist->channels);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, channellist);
}

jpx_bool				jp2_dec_parse_boxes(jp2_decp jp2dec)
{
	jp2_box_header boxheader;

	// Parse the File Type box
	if (!jp2_dec_parse_boxheader(jp2dec, &boxheader)) {
		return jpx_false;
	}
	if (boxheader.type != JP2_BOX_FTYP) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Missing File Type box.");
		}
		return jpx_false;
	}
	if (!jp2_dec_parse_ftyp(jp2dec, &boxheader)) {
		return jpx_false;
	}

	while ((jpx_bitstream_getbytesleft(jp2dec->bitstream) > 0) && !jp2dec->jpcdec) {
		if (!jp2_dec_parse_boxheader(jp2dec, &boxheader)) {
			return jpx_false;
		}

		switch (boxheader.type)
		{
		case JP2_BOX_JP2H:
			{
				if (!jp2_dec_parse_jp2h(jp2dec, &boxheader)) {
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_JP2C:
			{
				if (!jp2dec->jp2h) {
					if (jp2dec->warn_func) {
						jp2dec->warn_func("Contiguous Codestream box should appear after JP2 Header box, ignored.");
					}
					jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
				} else {
					jp2dec->jpcdec = jpc_dec_create_with_bitstream(jp2dec->memmgr, jp2dec->bitstream);
				}
			}
			break;
		case JP2_BOX_JP2I:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
			}
			break;
		case JP2_BOX_XML:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
			}
			break;
		case JP2_BOX_UUID:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
			}
			break;
		case JP2_BOX_UINF:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
			}
			break;
		case JP2_BOX_CMAP:
			{
				if (jp2dec->jp2h && !jp2dec->jp2h->cmap) {
					if (!jp2_dec_parse_cmap(jp2dec, &boxheader)) {
						return jpx_false;
					}
				} else {
					jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
				}
			}
			break;
		default:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader.len-8);
			}
		}
	}
	
	if (jp2dec->jp2h == NULL) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Missing JP2 Header box.");
		}
		return jpx_false;
	}
	if (jp2dec->jp2h->colr == NULL) {
		if (jp2dec->error_func) {
			jp2dec->error_func("JP2 must contain at least one Colour Specification box.");
		}
		return jpx_false;
	}
	if (!jp2dec->jpcdec) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Missing Contiguous Codestream box.");
		}
		return jpx_false;
	}

	return jpx_true;
}
jpx_bool				jp2_dec_parse_boxheader(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	if (!jpx_bitstream_readinteger(jp2dec->bitstream, &boxheader->len)
		|| !jpx_bitstream_readinteger(jp2dec->bitstream, &boxheader->type)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing JP2 Box header error.");
			return jpx_false;
		}
	}

	if (boxheader->len == 0) {
		boxheader->len = jpx_bitstream_getbytesleft(jp2dec->bitstream);
	} else if (boxheader->len == 1) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Unsupported extended length of JP2 box.");
		}
		return jpx_false;
	} else if (boxheader->len >= 8) {
		boxheader->len -= 8;
		if (jpx_bitstream_getbytesleft(jp2dec->bitstream) < boxheader->len) {
			boxheader->len = jpx_bitstream_getbytesleft(jp2dec->bitstream);
		}
	} else {
		if (jp2dec->error_func) {
			jp2dec->error_func("LBox cann't be value of 2~7.");
		}
		return jpx_false;
	}

	return jpx_true;
}
jpx_bool				jp2_dec_parse_ftyp(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset, i;

	if (jp2dec->ftyp) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 shall not contain more than one file type box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->ftyp = jp2_box_ftyp_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->ftyp->br)
		|| !jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->ftyp->minv)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing File Type box error.");
		}
		jp2_box_ftyp_destroy(jp2dec, jp2dec->ftyp);
		jp2dec->ftyp = NULL;
		return jpx_false;
	}
	offset += 8;

	jp2dec->ftyp->clnum = (boxheader->len - 8) >> 2;
	jp2dec->ftyp->cls = (jpx_uint32 *)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->ftyp->clnum*sizeof(jpx_uint32));
	for (i = 0; i < jp2dec->ftyp->clnum; i++) {
		if (!jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->ftyp->cls[i])) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing File Type box error.");
			}
			jp2_box_ftyp_destroy(jp2dec, jp2dec->ftyp);
			jp2dec->ftyp = NULL;
			return jpx_false;
		}
	}
	offset += jp2dec->ftyp->clnum*sizeof(jpx_uint32);
	
	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_jp2h(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32		offset;
	jp2_box_header	subboxheader;
	
	if (jp2dec->jp2h) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 shall not contain more than one JP2 Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h = jp2_box_jp2h_create(jp2dec);

	offset = 0;
	while ((jpx_bitstream_getbytesleft(jp2dec->bitstream) > 0) && (offset < boxheader->len)) {
		if (!jp2_dec_parse_boxheader(jp2dec, &subboxheader)) {
			jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
			jp2dec->jp2h = NULL;
			return jpx_false;
		}
		offset += 8;
		
		switch (subboxheader.type)
		{
		case JP2_BOX_IHDR:
			{
				if (!jp2_dec_parse_ihdr(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_BPCC:
			{
				if (!jp2_dec_parse_bpcc(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_COLR:
			{
				if (!jp2_dec_parse_colr(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_PCLR:
			{
				if (!jp2_dec_parse_pclr(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_CMAP:
			{
				if (!jp2_dec_parse_cmap(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_CDEF:
			{
				if (!jp2_dec_parse_cdef(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_RES:
			{
				if (!jp2_dec_parse_res(jp2dec, &subboxheader)) {
					jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
					jp2dec->jp2h = NULL;
					return jpx_false;
				}
			}
			break;
		default:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+subboxheader.len);
			}
		}
		offset += subboxheader.len;
	}

	return jpx_true;
}
jpx_bool				jp2_dec_parse_ihdr(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	
	if (jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->ihdr = jp2_box_ihdr_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->jp2h->ihdr->height)
		|| !jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->jp2h->ihdr->width)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->ihdr->nc)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->ihdr->bpc)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->ihdr->c)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->ihdr->unkc)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->ihdr->ipr)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Image Header box error.");
		}
		jp2_box_ihdr_destroy(jp2dec, jp2dec->jp2h->ihdr);
		jp2dec->jp2h->ihdr = NULL;
		return jpx_false;
	}
	offset += 14;
	
	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_bpcc(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	jpx_uint16 i;
	
	if (jp2dec->jp2h->bpcc) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Bits Per Component box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Bits Per Component box should appear after Image Header box,ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	
	jp2dec->jp2h->bpcc = jp2_box_bpcc_create(jp2dec);
	jp2dec->jp2h->bpcc->bpcs = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->ihdr->nc*sizeof(jpx_byte));

	offset = 0;
	for (i = 0; i < jp2dec->jp2h->ihdr->nc; i++) {
		if (!jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->bpcc->bpcs[i])) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Bits Per Component box error.");
			}
			jp2_box_bpcc_destroy(jp2dec, jp2dec->jp2h->bpcc);
			jp2dec->jp2h->bpcc = NULL;
			return jpx_false;
		}
	}
	offset += jp2dec->jp2h->ihdr->nc;

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_colr(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	
	if (jp2dec->jp2h->colr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("We ignores all Colour Specification boxes after the first.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Colour Specification box should appear after Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	
	jp2dec->jp2h->colr = jp2_box_colr_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->colr->meth)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->colr->prec)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->colr->approx)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Colour Specification box error.");
		}
		jp2_box_colr_destroy(jp2dec, jp2dec->jp2h->colr);
		jp2dec->jp2h->colr = NULL;
		return jpx_false;
	}
	offset += 3;
	
	if (jp2dec->jp2h->colr->meth == 1) {
		if (!jpx_bitstream_readinteger(jp2dec->bitstream, &jp2dec->jp2h->colr->enumcs)) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Colour Specification box error.");
			}
			jp2_box_colr_destroy(jp2dec, jp2dec->jp2h->colr);
			jp2dec->jp2h->colr = NULL;
			return jpx_false;
		}
		offset += 4;
	} else if (jp2dec->jp2h->colr->meth != 2) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Colour Specification box field METH is not 1 or 2, ignored entirely.");
		}
		jp2_box_colr_destroy(jp2dec, jp2dec->jp2h->colr);
		jp2dec->jp2h->colr = NULL;
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
		return jpx_true;
	}
	jp2dec->colorspace = jp2dec->jp2h->colr->enumcs;

	jp2dec->jp2h->colr->profilelen = boxheader->len-offset;
	if (jp2dec->jp2h->colr->profilelen > 0) {
		jp2dec->jp2h->colr->profile = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->colr->profilelen);
		if (!jpx_bitstream_readbuf(jp2dec->bitstream, jp2dec->jp2h->colr->profile, jp2dec->jp2h->colr->profilelen)) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Colour Specification box error.");
			}
			jp2_box_colr_destroy(jp2dec, jp2dec->jp2h->colr);
			jp2dec->jp2h->colr = NULL;
			return jpx_false;
		}
		offset += jp2dec->jp2h->colr->profilelen;
	}

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_pclr(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	jpx_uint16 i;
	jpx_byte j, c;
	
	if (jp2dec->jp2h->pclr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Palette box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Palette box should appear after Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->pclr = jp2_box_pclr_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->pclr->ne)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->pclr->npc)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Palette box error.");
		}
		jp2_box_pclr_destroy(jp2dec, jp2dec->jp2h->pclr);
		jp2dec->jp2h->pclr = NULL;
		return jpx_false;
	}
	offset += 3;

	jp2dec->jp2h->pclr->bs = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->pclr->npc);
	jp2dec->jp2h->pclr->cs = (jpx_uint32p)jp2dec->memmgr->alloc_func(jp2dec->memmgr, 
		jp2dec->jp2h->pclr->ne*jp2dec->jp2h->pclr->npc*sizeof(jpx_uint32));

	for (j = 0; j < jp2dec->jp2h->pclr->npc; j++) {
		if (!jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->pclr->bs[j])) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Palette box error.");
			}
			jp2_box_pclr_destroy(jp2dec, jp2dec->jp2h->pclr);
			jp2dec->jp2h->pclr = NULL;
			return jpx_false;
		}
		if (((jp2dec->jp2h->pclr->bs[j] & 0x7f) > 32) || ((jp2dec->jp2h->pclr->bs[j] & 0x7f) < 1)) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Bit depth of palette columen %d is more than 32 or less than 1, unsupported.");
			}
			jp2_box_pclr_destroy(jp2dec, jp2dec->jp2h->pclr);
			jp2dec->jp2h->pclr = NULL;
			return jpx_false;
		}
	}
	offset += jp2dec->jp2h->pclr->npc;
	
	for (i = 0; i < jp2dec->jp2h->pclr->ne; i++) {
		for (j = 0; j < jp2dec->jp2h->pclr->npc; j++) {
			c = (jp2dec->jp2h->pclr->bs[j] & 0x7f) + 1;
			if (!jpx_bitstream_readbits(jp2dec->bitstream, c, jp2dec->jp2h->pclr->cs+i*jp2dec->jp2h->pclr->npc+j)) {
				if (jp2dec->error_func) {
					jp2dec->error_func("Parsing Palette box error.");
				}
				jp2_box_pclr_destroy(jp2dec, jp2dec->jp2h->pclr);
				jp2dec->jp2h->pclr = NULL;
				return jpx_false;
			}
			jpx_bitstream_alignbyte(jp2dec->bitstream);
			
			offset += (c + 7) >> 3;
		}
	}

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_cmap(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset, i;
	
	if (jp2dec->jp2h->cmap) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Component Mapping box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Component Mapping box should appear after Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->cmap = jp2_box_cmap_create(jp2dec);
	jp2dec->jp2h->cmap->channelnum = boxheader->len>>2;
	jp2dec->jp2h->cmap->cmps = (jpx_uint16p)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->cmap->channelnum * sizeof(jpx_uint16));
	jp2dec->jp2h->cmap->mtyps = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->cmap->channelnum * sizeof(jpx_byte));
	jp2dec->jp2h->cmap->pcols = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jp2h->cmap->channelnum * sizeof(jpx_byte));

	offset = 0;
	for (i = 0; i < jp2dec->jp2h->cmap->channelnum; i++) {
		if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->cmap->cmps[i])
			|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->cmap->mtyps[i])
			|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->cmap->pcols[i])) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Component Mapping box error.");
			}
			jp2_box_cmap_destroy(jp2dec, jp2dec->jp2h->cmap);
			jp2dec->jp2h->cmap = NULL;
			return jpx_false;
		}
	}
	offset += jp2dec->jp2h->cmap->channelnum<<2;

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_cdef(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	jpx_uint16 i;
	
	if (jp2dec->jp2h->cdef) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Channel Definition box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Channel Definition box should appear after Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->cdef = jp2_box_cdef_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->cdef->n)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Channel Definition box error.");
		}
		jp2_box_cdef_destroy(jp2dec, jp2dec->jp2h->cdef);
		jp2dec->jp2h->cdef = NULL;
		return jpx_false;
	}
	offset += 2;

	if (jp2dec->jp2h->cdef->n == 0) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Channel Definition box contains zero channel descriptions, ignored.");
		}
		jp2_box_cdef_destroy(jp2dec, jp2dec->jp2h->cdef);
		jp2dec->jp2h->cdef = NULL;
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
		return jpx_false;
	}

	jp2dec->jp2h->cdef->cns = (jpx_uint16p)jp2dec->memmgr->alloc_func(jp2dec->memmgr, (jp2dec->jp2h->cdef->n+1)*sizeof(jpx_uint16));
	jp2dec->jp2h->cdef->typs = (jpx_uint16p)jp2dec->memmgr->alloc_func(jp2dec->memmgr, (jp2dec->jp2h->cdef->n+1)*sizeof(jpx_uint16));
	jp2dec->jp2h->cdef->asocs = (jpx_uint16p)jp2dec->memmgr->alloc_func(jp2dec->memmgr, (jp2dec->jp2h->cdef->n+1)* sizeof(jpx_uint16));
	
	for (i = 0; i < jp2dec->jp2h->cdef->n; i++) {
		if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->cdef->cns[i])
			|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->cdef->typs[i])
			|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->cdef->asocs[i])) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Parsing Channel Definition box error.");
			}
			jp2_box_cdef_destroy(jp2dec, jp2dec->jp2h->cdef);
			jp2dec->jp2h->cdef = NULL;
			return jpx_false;
		}
		if (jp2dec->jp2h->cdef->cns[i] >= jp2dec->jp2h->cdef->n) {
			if (jp2dec->error_func) {
				jp2dec->error_func("Channel Definition box's Cns(i) >= N.");
			}
			jp2_box_cdef_destroy(jp2dec, jp2dec->jp2h->cdef);
			jp2dec->jp2h->cdef = NULL;
			return jpx_false;
		}
	}
	offset += jp2dec->jp2h->cdef->n * 6;

	jp2dec->jp2h->cdef->cns[jp2dec->jp2h->cdef->n] = 0xffff;
	jp2dec->jp2h->cdef->typs[jp2dec->jp2h->cdef->n] = 0xffff;
	jp2dec->jp2h->cdef->asocs[jp2dec->jp2h->cdef->n] = 0xffff;
	jp2_box_cdef_reorder(jp2dec->jp2h->cdef, 0, jp2dec->jp2h->cdef->n - 1);

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_res(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;
	jp2_box_header subboxheader;
	
	if (jp2dec->jp2h->res) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("JP2 Header box shall contain only one Resolution box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}
	if (!jp2dec->jp2h->ihdr) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Resolution box should appear after Image Header box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->res = jp2_box_res_create(jp2dec);

	offset = 0;
	while ((jpx_bitstream_getbytesleft(jp2dec->bitstream) > 0) && (offset < boxheader->len)) {
		if (!jp2_dec_parse_boxheader(jp2dec, &subboxheader)) {
			jp2_box_res_destroy(jp2dec, jp2dec->jp2h->res);
			jp2dec->jp2h->res = NULL;
			return jpx_false;
		}
		offset += 8;

		switch(subboxheader.type)
		{
		case JP2_BOX_RESC:
			{
				if (!jp2_dec_parse_resc(jp2dec, &subboxheader)) {
					jp2_box_res_destroy(jp2dec, jp2dec->jp2h->res);
					jp2dec->jp2h->res = NULL;
					return jpx_false;
				}
			}
			break;
		case JP2_BOX_RESD:
			{
				if (!jp2_dec_parse_resd(jp2dec, &subboxheader)) {
					jp2_box_res_destroy(jp2dec, jp2dec->jp2h->res);
					jp2dec->jp2h->res = NULL;
					return jpx_false;
				}
			}
			break;
		default:
			{
				// Skip unknown boxes.
				jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+subboxheader.len);
			}
		}
		offset += subboxheader.len;
	}

	return jpx_true;
}
jpx_bool				jp2_dec_parse_resc(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;

	if (jp2dec->jp2h->res->resc) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Resolution box shall contain only one Capture Resolution box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->res->resc = jp2_box_resc_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resc->vrcn)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resc->vrcd)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resc->hrcn)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resc->hrcd)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->res->resc->vrce)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->res->resc->hrce)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Capture Resolution box error.");
		}
		jp2_box_resc_destroy(jp2dec, jp2dec->jp2h->res->resc);
		jp2dec->jp2h->res->resc = NULL;
		return jpx_false;
	}
	offset += 10;

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
jpx_bool				jp2_dec_parse_resd(jp2_decp jp2dec, jp2_box_headerp boxheader)
{
	jpx_uint32 offset;

	if (jp2dec->jp2h->res->resd) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Resolution box shall contain only one Default Display Resolution box, ignored.");
		}
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len);
		return jpx_true;
	}

	jp2dec->jp2h->res->resd = jp2_box_resd_create(jp2dec);

	offset = 0;
	if (!jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resd->vrdn)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resd->vrdd)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resd->hrdn)
		|| !jpx_bitstream_readshort(jp2dec->bitstream, &jp2dec->jp2h->res->resd->hrdd)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->res->resd->vrde)
		|| !jpx_bitstream_readbyte(jp2dec->bitstream, &jp2dec->jp2h->res->resd->hrde)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Parsing Default Display Resolution box error.");
		}
		jp2_box_resd_destroy(jp2dec, jp2dec->jp2h->res->resd);
		jp2dec->jp2h->res->resd = NULL;
		return jpx_false;
	}
	offset += 10;

	if (offset < boxheader->len) {
		jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+boxheader->len-offset);
	}
	return jpx_true;
}
// Quick sort.
// Reorder according to channel->type and channel->asoc.
void					jp2_box_cdef_reorder(jp2_box_cdefp cdef, jpx_int32 l, jpx_int32 r)
{
	jpx_int32 i, j;
	jpx_uint16 temp;

	if (l >= r) return;
	
	i = l; 
	j = r + 1;
	
	while (jpx_true) {
		do { // Find >= [l] element.
			i = i + 1;
		} while ((cdef->typs[i] < cdef->typs[l]) || 
			((cdef->typs[i] == cdef->typs[l]) && (cdef->asocs[i] < cdef->asocs[l])));
		
		do { // Reverse find <= [l] element.
			j = j - 1;
		} while ((cdef->typs[j] > cdef->typs[l]) || 
			((cdef->typs[j] == cdef->typs[l]) && (cdef->asocs[j] > cdef->asocs[l])));

		if (i >= j) break; // No swap-needed element.

		temp = cdef->cns[i];
		cdef->cns[i] = cdef->cns[j];
		cdef->cns[j] = temp;
		temp = cdef->typs[i];
		cdef->typs[i] = cdef->typs[j];
		cdef->typs[j] = temp;
		temp = cdef->asocs[i];
		cdef->asocs[i] = cdef->asocs[j];
		cdef->asocs[j] = temp;
	}
	
	temp = cdef->cns[l];
	cdef->cns[l] = cdef->cns[j];
	cdef->cns[j] = temp;
	temp = cdef->typs[l];
	cdef->typs[l] = cdef->typs[j];
	cdef->typs[j] = temp;
	temp = cdef->asocs[l];
	cdef->asocs[l] = cdef->asocs[j];
	cdef->asocs[j] = temp;

	jp2_box_cdef_reorder(cdef, l, j-1); // Left sort.
	jp2_box_cdef_reorder(cdef, j+1, r); // Right sort.
}



jp2_decp				jp2_dec_create(jpx_memmgrp memmgr, jpx_streamp stream)
{
	jp2_decp jp2dec = (jp2_decp)memmgr->alloc_func(memmgr, sizeof(jp2_dec));
	jpx_memset(jp2dec, 0, sizeof(jp2_dec));
	jp2dec->memmgr = memmgr;
	jp2dec->bitstream = jpx_bitstream_create(memmgr, stream);
	
	jp2dec->jpcdec = NULL;
	jp2dec->channel_list = NULL;
	jp2dec->colorspace = JPX_CS_UNKNOWN;
	
	jp2dec->output_convert2rgb = jpx_true;
	jp2dec->output_width = 0;
	jp2dec->output_height = 0;
	jp2dec->output_channum = 0;
	jp2dec->output_pixgap = 0;

	return jp2dec;
}
void					jp2_dec_destroy(jp2_decp jp2dec)
{
	if (jp2dec->channel_list) {
		jp2_channel_list_destroy(jp2dec, jp2dec->channel_list);
	}
	if (jp2dec->jpcdec) {
		jpc_dec_destroy(jp2dec->jpcdec);
	}

	if (jp2dec->ftyp) {
		jp2_box_ftyp_destroy(jp2dec, jp2dec->ftyp);
	}
	if (jp2dec->jp2h) {
		jp2_box_jp2h_destroy(jp2dec, jp2dec->jp2h);
	}
	if (jp2dec->bitstream) {
		jpx_bitstream_destroy(jp2dec->memmgr, jp2dec->bitstream);
	}
	jp2dec->memmgr->free_func(jp2dec->memmgr, jp2dec);
}
void					jp2_dec_set_error_func(jp2_decp jp2dec, jpx_error_funcp error_func)
{
	jp2dec->error_func = error_func;
}
void					jp2_dec_set_warn_func(jp2_decp jp2dec, jpx_warn_funcp warn_func)
{
	jp2dec->warn_func = warn_func;
}
jpx_bool				jp2_dec_parse(jp2_decp jp2dec)
{
	jpx_bool samebpc;
	jpx_byte bpc;
	jpx_uint16 comp, channel_num;

	// Check JP2 format
	jp2dec->bitstream->stream->seekpos_func(jp2dec->bitstream->stream, jp2dec->bitstream->pos);
	if (!jpx_is_jp2(jp2dec->bitstream->stream)) {
		if (jp2dec->error_func) {
			jp2dec->error_func("Missing JPEG 2000 Signature box.");
		}
		return jpx_false;
	}
	jpx_bitstream_setpos(jp2dec->bitstream, jpx_bitstream_getpos(jp2dec->bitstream)+12);

	// Parse JP2 boxes
	if (!jp2_dec_parse_boxes(jp2dec)) {
		return jpx_false;
	}

	// Parse JPEG2000 codestream
	if (!jpc_dec_parse(jp2dec->jpcdec)) {
		return jpx_false;
	}
	
	// Checking image area with and height
	if ((jp2dec->jp2h->ihdr->width != jp2dec->jpcdec->siz.xsiz-jp2dec->jpcdec->siz.xosiz)
		|| (jp2dec->jp2h->ihdr->height != jp2dec->jpcdec->siz.ysiz-jp2dec->jpcdec->siz.yosiz)) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Image area width or height of Image Header box and codestream are mismatch.");
		}
	}
	if (jp2dec->jp2h->ihdr->nc != jp2dec->jpcdec->component_num) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Component number of Image Header box and codestream are mismatch.");
		}
	}
	
	// Checking bpc
	samebpc = jpx_true;
	bpc = jp2dec->jpcdec->siz.ssizs[0];
	for (comp=0; comp<jp2dec->jpcdec->component_num; comp++) {
		if (bpc != jp2dec->jpcdec->siz.ssizs[comp]) {
			samebpc = jpx_false;
			break;
		}
	}
	if ((samebpc && jp2dec->jp2h->ihdr->bpc!=bpc) || (!samebpc && bpc!=255)) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Bits per component of Image Header box and codestream are mismatch.");
		}
	}
	if (jp2dec->jp2h->bpcc) {
		if (jp2dec->jp2h->ihdr->nc == jp2dec->jpcdec->component_num) {
			if (!samebpc) {
				for (comp=0; comp<jp2dec->jpcdec->component_num; comp++) {
					if (jp2dec->jp2h->bpcc->bpcs[comp] != jp2dec->jpcdec->siz.ssizs[comp]) {
						if (jp2dec->warn_func) {
							jp2dec->warn_func("Bits per component of Bits Per Component box and codestream are mismatch.");
						}
						break;
					}
				}
			}
		}
	}

	// Checking compress algorithm
	if (jp2dec->jp2h->ihdr->c != 7) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Unknown compression algorithm.");
		}
	}
	
	// Checking palettes
	if ((jp2dec->jp2h->pclr&&!jp2dec->jp2h->cmap) || (!jp2dec->jp2h->pclr&&jp2dec->jp2h->cmap)) {
		if (jp2dec->warn_func) {
			jp2dec->warn_func("Palette box shall appear with Component Mapping box.");
		}
		if (jp2dec->jp2h->pclr) {
			jp2_box_pclr_destroy(jp2dec, jp2dec->jp2h->pclr);
			jp2dec->jp2h->pclr = NULL;
		}
		if (jp2dec->jp2h->cmap) {
			jp2_box_cmap_destroy(jp2dec, jp2dec->jp2h->cmap);
			jp2dec->jp2h->cmap = NULL;
		}
	}
	
	// Checking component mapping
	if (jp2dec->jp2h->cmap) {
		channel_num = 0;
		for (comp=0; comp<jp2dec->jp2h->cmap->channelnum; comp++) {
			if (jp2dec->jp2h->cmap->cmps[comp] >= jp2dec->jpcdec->component_num) {
				if (jp2dec->warn_func) {
					jp2dec->warn_func("Component Mapping box's CMP(n) >= jpcdec->component_num.");
				}
				break;
			}
			channel_num ++;
		}
	} else {
		channel_num = jp2dec->jpcdec->component_num;
	}

	// Check channel definition
	if (jp2dec->jp2h->cdef) {
		for (comp=0; comp<jp2dec->jp2h->cdef->n; comp++) {
			if (jp2dec->jp2h->cdef->cns[comp] >= channel_num) {
				if (jp2dec->error_func) {
					jp2dec->error_func("Channel Definition box's Cn(n) >= channel_list->channel_num.");
				}
				return jpx_false;
			}
		}
		channel_num = jp2dec->jp2h->cdef->n;
	}

	jp2dec->channel_list = jp2_channel_list_create(jp2dec);
	jp2dec->channel_list->channel_num = channel_num;
	jp2dec->channel_list->type = jp2dec->jp2h->colr->enumcs;
	jp2dec->channel_list->channels = jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->channel_list->channel_num*sizeof(jp2_channel));
	for (comp=0; comp<jp2dec->channel_list->channel_num; comp++) {
		if (jp2dec->jp2h->cdef) {
			if (jp2dec->jp2h->cmap) {
				jp2dec->channel_list->channels[comp].comp = jp2dec->jp2h->cmap->cmps[jp2dec->jp2h->cdef->cns[comp]];
			} else {
				jp2dec->channel_list->channels[comp].comp = jp2dec->jp2h->cdef->cns[comp];
			}
			jp2dec->channel_list->channels[comp].cn = jp2dec->jp2h->cdef->cns[comp];
			jp2dec->channel_list->channels[comp].type = jp2dec->jp2h->cdef->typs[comp];
			jp2dec->channel_list->channels[comp].asoc = jp2dec->jp2h->cdef->asocs[comp];
		} else {
			if (jp2dec->jp2h->cmap) {
				jp2dec->channel_list->channels[comp].comp = jp2dec->jp2h->cmap->cmps[comp];
			} else {
				jp2dec->channel_list->channels[comp].comp = comp;
			}
			jp2dec->channel_list->channels[comp].cn = comp;
			jp2dec->channel_list->channels[comp].type = 0;
			jp2dec->channel_list->channels[comp].asoc = comp + 1;
		}
	}
	
	if (jp2dec->jp2h->cdef) {
		jp2dec->alpha = jpx_false;
		for (comp=0; comp<jp2dec->channel_list->channel_num; comp++) {
			if (jp2dec->channel_list->channels[comp].type==1 || jp2dec->channel_list->channels[comp].type==2) {
				jp2dec->alpha = jpx_true;
				break;
			}
		}
	}

	return jpx_true;
}
void					jp2_dec_get_imagesize(jp2_decp jp2dec, jpx_uint32p width, jpx_uint32p height)
{
	jpc_dec_get_imagesize(jp2dec->jpcdec, width, height);
}
jpx_bool				jp2_dec_has_alpha(jp2_decp jp2dec)
{
	return jp2dec->alpha;
}
jpx_int32				jp2_dec_get_colorspace(jp2_decp jp2dec)
{
	return jp2dec->colorspace;
}
void					jp2_dec_set_convert2rgb(jp2_decp jp2dec, jpx_bool convert2rgb)
{
	jp2dec->output_convert2rgb = convert2rgb;
}
void					jp2_transfer_data(jp2_decp jp2dec, jpx_bytepp bufs, jpx_uint32 pixgap, jpx_uint16 channum, jpx_bytepp lines, jpx_uint32 length)
{
	if (jp2dec->output_convert2rgb && jp2dec->colorspace!=JPX_CS_SRGB) {
		switch (jp2dec->channel_list->type)
		{
		case JPX_CS_GREYSCALE:
			jpx_convert_greyscale_to_rgb(bufs, pixgap, channum, lines, length);
			break;
		case JPX_CS_SYCC:
			jpx_convert_sycc_to_rgb(bufs, pixgap, channum, lines, length);
			break;
		case JPX_CS_CMYK:
			jpx_convert_cmyk_to_rgb(bufs, pixgap, channum, lines, length);
			break;
		default:
			if (jp2dec->warn_func) {
				jp2dec->warn_func("Unsupport colorspace!");
			}
			break;
		}
	} else {
		jpx_uint16 chan;
		jpx_bytep dst, src;
		jpx_uint32 i;
		
		for (chan=0; chan < channum; chan++) {
			src = lines[chan];
			dst = bufs[chan];
			for (i = 0; i < length; i++) {
				*dst = *src++;
				dst += pixgap;
			}
		}
	}
}

void					jp2_dec_init(jp2_decp jp2dec, jpx_uint16 channum, jpx_int32p offsets, jpx_int32 pixgap)
{
	jpx_uint16 chan;

	jp2dec->output_pixgap = pixgap;
	jp2dec->output_channum = channum>jp2dec->channel_list->channel_num? jp2dec->channel_list->channel_num:channum;
	if (jp2dec->output_channum > JPX_MAX_CHANNEL) jp2dec->output_channum = JPX_MAX_CHANNEL;
	for (chan=0; chan<jp2dec->output_channum; chan++) {
		jp2dec->output_offsets[chan] = offsets ? offsets[chan]:chan;
	}

	jp2_dec_get_imagesize(jp2dec, &jp2dec->output_width, &jp2dec->output_height);
	jp2dec->full_comp_raw_line = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->jpcdec->component_num*jp2dec->output_width*sizeof(jpx_byte));
	for (chan=0; chan<jp2dec->channel_list->channel_num; chan++) {
		jp2dec->chan_raw_lines[chan] = (jpx_bytep)jp2dec->memmgr->alloc_func(jp2dec->memmgr, jp2dec->output_width*sizeof(jpx_byte));
	}

	jpc_dec_init(jp2dec->jpcdec, jp2dec->jpcdec->component_num, NULL, jp2dec->jpcdec->component_num);
}
jpx_bool				jp2_dec_scanline(jp2_decp jp2dec, jpx_bytep line)
{
	jpx_uint16 chan;
	jpx_uint32 x;

	// Scan the full components line in codestream
	if (!jpc_dec_scanline(jp2dec->jpcdec, jp2dec->full_comp_raw_line)) {
		return jpx_false;
	}
	
	// Deal with palettes
	for (chan=0; chan<jp2dec->channel_list->channel_num; chan++) {
		if (jp2dec->jp2h->cmap && jp2dec->jp2h->cmap->mtyps[jp2dec->channel_list->channels[chan].cn]) {
			jpx_byte v;
			for (x=0; x < jp2dec->output_width; x++) {
				v = jp2dec->full_comp_raw_line[jp2dec->channel_list->channels[chan].comp+jp2dec->jpcdec->component_num*x];
				if (v >= jp2dec->jp2h->pclr->ne) { v = jp2dec->jp2h->pclr->ne - 1; }

				jp2dec->chan_raw_lines[chan][x] = jp2dec->jp2h->pclr->cs[v*jp2dec->jp2h->pclr->npc+
					jp2dec->jp2h->cmap->pcols[jp2dec->channel_list->channels[chan].cn]];
			}
		} else {
			for (x=0; x < jp2dec->output_width; x++) {
				jp2dec->chan_raw_lines[chan][x] = jp2dec->full_comp_raw_line[jp2dec->channel_list->channels[chan].comp+jp2dec->jpcdec->component_num*x];
			}
		}
	}
	
	// Transfer data.
	for (chan=0; chan<jp2dec->output_channum; chan++) {
		jp2dec->chan_lines[chan] = line + jp2dec->output_offsets[chan];
	}
	// TODO: deal with output_channum in colorspace conversion.
	jp2_transfer_data(jp2dec, jp2dec->chan_lines, jp2dec->output_pixgap, jp2dec->output_channum, jp2dec->chan_raw_lines, jp2dec->output_width);

	return jpx_true;
}
void					jp2_dec_finalize(jp2_decp jp2dec)
{
	jpx_uint16 chan;

	jp2dec->memmgr->free_func(jp2dec->memmgr, jp2dec->full_comp_raw_line);
	jp2dec->full_comp_raw_line = NULL;
	for (chan=0; chan<jp2dec->channel_list->channel_num; chan++) {
		jp2dec->memmgr->free_func(jp2dec->memmgr, jp2dec->chan_raw_lines[chan]);
	}

	jpc_dec_finalize(jp2dec->jpcdec);
}
