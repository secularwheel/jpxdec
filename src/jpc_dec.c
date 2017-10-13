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
#include "../include/jpx_codeblock.h"

jpx_bool					jpc_parse_marker_header(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpx_bool					jpc_parse_marker_siz(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_codp				jpc_parse_marker_cod(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_cocp				jpc_parse_marker_coc(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_qcdp				jpc_parse_marker_qcd(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_qccp				jpc_parse_marker_qcc(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_rgnp				jpc_parse_marker_rgn(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpc_marker_pocp				jpc_parse_marker_poc(jpc_decp jpcdec, jpc_marker_headerp markerheader);
jpx_bool					jpc_parse_marker_sot(jpc_decp jpcdec, jpc_marker_sotp sot);
jpx_bool					jpc_parse_marker_sop(jpc_decp jpcdec, jpx_uint16p packet_sequence);
jpc_headerp					jpc_parse_main_header(jpc_decp jpcdec);
jpc_headerp					jpc_parse_tile_header0(jpc_decp jpcdec, jpc_marker_sotp sot);
jpc_headerp					jpc_parse_tile_header1(jpc_decp jpcdec, jpc_marker_sotp sot);
jpx_bool					jpc_parse_tile_data(jpc_decp jpcdec, jpc_marker_sotp sot);

jpx_bool					jpc_po_next_lrcp(jpc_pop po);
jpx_bool					jpc_po_next_rlcp(jpc_pop po);
jpx_bool					jpc_po_next_rpcl(jpc_pop po);
jpx_bool					jpc_po_next_pcrl(jpc_pop po);
jpx_bool					jpc_po_next_cprl(jpc_pop po);
void						jpc_po_compute(jpc_pop po);
void						jpc_po_normalize(jpc_pop po);
void						jpc_po_init(jpc_pop po);

void						jpc_endow_cox_and_rgn(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader);
void						jpc_endow_qcx(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader);
void						jpc_endow_poc(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader);


void						jpc_marker_siz_init(jpc_decp jpcdec, jpc_marker_sizp siz)
{
	siz->ssizs = NULL;
	siz->xrsizs = NULL;
	siz->yrsizs = NULL;
}
void						jpc_marker_siz_finalize(jpc_decp jpcdec, jpc_marker_sizp siz)
{
	if (siz->ssizs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, siz->ssizs);
	}
	if (siz->xrsizs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, siz->xrsizs);
	}
	if (siz->yrsizs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, siz->yrsizs);
	}
}
jpc_marker_codp				jpc_marker_cod_create(jpc_decp jpcdec)
{
	jpc_marker_codp cod = (jpc_marker_codp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_cod));
	jpx_memset(cod, 0, sizeof(jpc_marker_cod));
	return cod;
}
void						jpc_marker_cod_destroy(jpc_decp jpcdec, jpc_marker_codp cod)
{
	if (cod->spcod.precinct_sizes) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, cod->spcod.precinct_sizes);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, cod);
}
jpc_marker_cocp				jpc_marker_coc_create(jpc_decp jpcdec)
{
	jpc_marker_cocp coc = (jpc_marker_cocp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_coc));
	jpx_memset(coc, 0, sizeof(jpc_marker_coc));
	return coc;
}
void						jpc_marker_coc_destroy(jpc_decp jpcdec, jpc_marker_cocp coc)
{
	if (coc->spcoc.precinct_sizes) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, coc->spcoc.precinct_sizes);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, coc);
}
jpc_marker_rgnp				jpc_marker_rgn_create(jpc_decp jpcdec)
{
	jpc_marker_rgnp rgn = (jpc_marker_rgnp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_rgn));
	jpx_memset(rgn, 0, sizeof(jpc_marker_rgn));
	return rgn;
}
void						jpc_marker_rgn_destroy(jpc_decp jpcdec, jpc_marker_rgnp rgn)
{
	jpcdec->memmgr->free_func(jpcdec->memmgr, rgn);
}
jpc_marker_qcdp				jpc_marker_qcd_create(jpc_decp jpcdec)
{
	jpc_marker_qcdp qcd = (jpc_marker_qcdp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_qcd));
	jpx_memset(qcd, 0, sizeof(jpc_marker_qcd));
	return qcd;
}
void						jpc_marker_qcd_destroy(jpc_decp jpcdec, jpc_marker_qcdp qcd)
{
	if (qcd->spqcds) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, qcd->spqcds);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, qcd);
}
jpc_marker_qccp				jpc_marker_qcc_create(jpc_decp jpcdec)
{
	jpc_marker_qccp qcc = (jpc_marker_qccp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_qcc));
	jpx_memset(qcc, 0, sizeof(jpc_marker_qcc));
	return qcc;
}
void						jpc_marker_qcc_destroy(jpc_decp jpcdec, jpc_marker_qccp qcc)
{
	if (qcc->spqccs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, qcc->spqccs);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, qcc);
}
jpc_marker_pocp				jpc_marker_poc_create(jpc_decp jpcdec)
{
	jpc_marker_pocp poc = (jpc_marker_pocp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_marker_poc));
	jpx_memset(poc, 0, sizeof(jpc_marker_poc));
	return poc;
}
void						jpc_marker_poc_destroy(jpc_decp jpcdec, jpc_marker_pocp poc)
{
	if (poc->rspocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->rspocs);
	}
	if (poc->cspocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->cspocs);
	}
	if (poc->lyepocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->lyepocs);
	}
	if (poc->repocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->repocs);
	}
	if (poc->cepocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->cepocs);
	}
	if (poc->ppocs) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, poc->ppocs);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, poc);
}
jpc_headerp					jpc_header_create(jpc_decp jpcdec, jpx_uint16 componentnum)
{
	jpc_headerp header = (jpc_headerp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_header));
	jpx_memset(header, 0, sizeof(jpc_header));

	header->componentnum = componentnum;
	header->cocs = (jpc_marker_cocpp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, componentnum*sizeof(jpc_marker_cocp));
	jpx_memset(header->cocs, 0, componentnum*sizeof(jpc_marker_cocp));
	header->qccs = (jpc_marker_qccpp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, componentnum*sizeof(jpc_marker_qccp));
	jpx_memset(header->qccs, 0, componentnum*sizeof(jpc_marker_qccp));
	header->rgns = (jpc_marker_rgnpp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, componentnum*sizeof(jpc_marker_rgnp));
	jpx_memset(header->rgns, 0, componentnum*sizeof(jpc_marker_rgnp));
	
	return header;
}
void						jpc_header_destroy(jpc_decp jpcdec, jpc_headerp header)
{
	jpx_uint16 i;
	jpc_marker_pocp poc, next;

	for (i = 0; i < header->componentnum; i++) {
		if (header->cocs[i]) {
			jpc_marker_coc_destroy(jpcdec, header->cocs[i]);
		}
		if (header->qccs[i]) {
			jpc_marker_qcc_destroy(jpcdec, header->qccs[i]);
		}
		if (header->rgns[i]) {
			jpc_marker_rgn_destroy(jpcdec, header->rgns[i]);
		}
	}

	jpcdec->memmgr->free_func(jpcdec->memmgr, header->cocs);
	jpcdec->memmgr->free_func(jpcdec->memmgr, header->qccs);
	jpcdec->memmgr->free_func(jpcdec->memmgr, header->rgns);
	if (header->cod) {
		jpc_marker_cod_destroy(jpcdec, header->cod);
	}
	if (header->qcd) {
		jpc_marker_qcd_destroy(jpcdec, header->qcd);
	}

	poc = header->poclist;
	while (poc) {
		next = poc->next;
		jpc_marker_poc_destroy(jpcdec, poc);
		poc = next;
	}

	jpcdec->memmgr->free_func(jpcdec->memmgr, header);
}
void					jpc_linebufs_init(jpc_linebufsp linebufs, jpc_decp jpcdec, jpx_uint32 size)
{
	jpx_uint32 index;

	linebufs->size = size;
	linebufs->lines = (jpc_linebufp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, size*sizeof(jpc_linebuf));
	jpx_memset(linebufs->lines, 0, size*sizeof(jpc_linebuf));
	linebufs->cur = &linebufs->lines[0];

	for (index = 0; index < size; index++) {
		if (index == 0) {
			linebufs->lines[index].prev = &linebufs->lines[size-1];
			linebufs->lines[index].next = &linebufs->lines[index+1];
		} else if (index == size-1) {
			linebufs->lines[index].prev = &linebufs->lines[index-1];
			linebufs->lines[index].next = &linebufs->lines[0];
		} else {
			linebufs->lines[index].prev = &linebufs->lines[index-1];
			linebufs->lines[index].next = &linebufs->lines[index+1];
		}
	}
}
void					jpc_linebufs_finalize(jpc_linebufsp linebufs, jpc_decp jpcdec)
{
	jpcdec->memmgr->free_func(jpcdec->memmgr, linebufs->lines);
}


jpx_bool				jpc_parse_marker_header(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_byte c;

	// Looking for 0xff
	do {
		if (!jpx_bitstream_readbyte(jpcdec->bitstream, &c)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing marker header error!");
			}
			return jpx_false;
		}
	} while (c != 0xff);

	// Read the marker code.
	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &c)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing marker header error!");
		}
		return jpx_false;
	}
	markerheader->code = (0xff<<8) | c;
	
	// Check simple marker
	if ((markerheader->code==JPC_MARKER_SOC)
		|| (markerheader->code==JPC_MARKER_SOD)
		|| (markerheader->code==JPC_MARKER_EOC)
		|| (markerheader->code==JPC_MARKER_EPH)
		|| ((markerheader->code>=0xff30)&&(markerheader->code<=0xff3f))) {
		markerheader->len = 0;
		return jpx_true;
	} else if ((markerheader->code<0xff00) || (markerheader->code>0xffff)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("JPC marker code out of range.");
		}
		return jpx_false;
	}

	// Read the marker segment length.
	if (!jpx_bitstream_readshort(jpcdec->bitstream, &markerheader->len)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing marker header error!");
		}
		return jpx_false;
	}

	return jpx_true;
}
jpx_bool				jpc_parse_marker_siz(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, comp;
	jpx_byte ssiz, xrsiz, yrsiz;
	jpc_marker_sizp siz = &jpcdec->siz;

	jpc_marker_siz_init(jpcdec, siz);

	offset = 2;
	if (!jpx_bitstream_readshort(jpcdec->bitstream, &jpcdec->capability)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->xsiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->ysiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->xosiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->yosiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->xtsiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->ytsiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->xtosiz)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &siz->ytosiz)
		|| !jpx_bitstream_readshort(jpcdec->bitstream, &siz->csiz)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing SIZ marker segment error.");
		}
		jpc_marker_siz_finalize(jpcdec, siz);
		return jpx_false;
	}
	jpcdec->component_num = siz->csiz;
	offset += 36;

	if (jpcdec->capability == JPC_CAPABILITY_PROFILE0) {
		if (!((siz->xsiz < (1<<31)) && (siz->ysiz < (1<<31)))) {
			if (jpcdec->warn_func) {
				jpcdec->warn_func("Violate profile0: Xsiz,Ysiz < 2~31.");
			}
		}
		if (!(((siz->ytsiz==128) && (siz->xtsiz==128))
			|| ((siz->ytsiz+siz->ytosiz >= siz->ysiz) && (siz->xtsiz+siz->xtosiz >= siz->xsiz)))) {
			if (jpcdec->warn_func) {
				jpcdec->warn_func("Violate profile0: "
					"tiles of a dimension 128 x 128:"
					"YTsiz = XTsiz = 128"
					"or one tile for the whole image:"
					"YTsiz + YTOsiz >= Ysiz"
					"XTsiz + XTOsiz >= Xsiz");
			}
		}
		if (!((siz->xosiz==0) && (siz->yosiz==0) && (siz->xtosiz!=0) && (siz->ytosiz==0))) {
			if (jpcdec->warn_func) {
				jpcdec->warn_func("Violate profile0: XOsiz = YOsiz = XTOsiz = YTOsiz = 0");
			}
		}
	} else if (jpcdec->capability == JPC_CAPABILITY_PROFILE1) {
		if (!((siz->xsiz < (1<<31)) && (siz->ysiz < (1<<31)))) {
			if (jpcdec->warn_func) {
				jpcdec->warn_func("Violate profile1: Xsiz,Ysiz < 2~31");
			}
		}
		if (!((siz->xosiz < (1<<31)) && (siz->yosiz < (1<<31))
			&& (siz->xtosiz < (1<<31)) && (siz->ytosiz < (1<<31)))) {
			if (jpcdec->warn_func) {
				jpcdec->warn_func("Violate profile1: XOsiz,YOsiz,XTOsiz,YTOsiz < 2~31");
			}
		}
	}

	siz->ssizs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, jpcdec->component_num*sizeof(jpx_byte));
	siz->xrsizs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, jpcdec->component_num*sizeof(jpx_byte));
	siz->yrsizs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, jpcdec->component_num*sizeof(jpx_byte));
	for (comp = 0; comp < jpcdec->component_num; comp++) {
		if (!jpx_bitstream_readbyte(jpcdec->bitstream, &ssiz)
			|| !jpx_bitstream_readbyte(jpcdec->bitstream, &xrsiz)
			|| !jpx_bitstream_readbyte(jpcdec->bitstream, &yrsiz)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing SIZ marker segment error.");
			}
			jpc_marker_siz_finalize(jpcdec, siz);
			return jpx_false;
		}
		siz->ssizs[comp] = ssiz;
		siz->xrsizs[comp] = xrsiz;
		siz->yrsizs[comp] = yrsiz;
		offset += 3;
		
		if (jpcdec->capability == 1) {
			if (!(((xrsiz==1) || (xrsiz==2) || (xrsiz==4))
				&& ((yrsiz==1) || (yrsiz==2) || (yrsiz==4)))) {
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Violate profile0: XRsiz(i) = 1,2,or 4 and YRsiz(i) = 1,2,or 4");
				}
			}
		} else if (jpcdec->capability == 2) {
			jpx_byte min = xrsiz > yrsiz ? yrsiz : xrsiz;
			if (!(((siz->xtsiz/min >= 1024) && (siz->xtsiz == siz->ytsiz))
				|| ((siz->ytsiz+siz->ytosiz >= siz->ysiz) && (siz->xtsiz+siz->xtosiz >= siz->xsiz)))) {
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Violate profile1: "
						"XTsiz/min(XRsiz(i),YRsiz(i)) >= 1024"
						"XTsiz = YTsiz"
						"or one tile for the whole image:"
						"YTsiz + YTOsiz >= Ysiz"
						"XTsiz + XTOsiz >= Xsiz");
				}
			}
		}
	}
	
	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return jpx_true;
}
jpc_marker_codp			jpc_parse_marker_cod(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset;
	jpx_byte i;
	jpc_marker_codp cod;
	
	cod = jpc_marker_cod_create(jpcdec);

	offset = 2;
	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &cod->scod)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->sgcod.progression_order)
		|| !jpx_bitstream_readshort(jpcdec->bitstream, &cod->sgcod.layer_num)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->sgcod.multiple_component)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.decomposition_level_num)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.code_block_width)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.code_block_height)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.code_block_style)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.transformation)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing COD marker segment error.");
		}
		jpc_marker_cod_destroy(jpcdec, cod);
		return NULL;
	}
	offset += 10;
	
	if (cod->scod & 1) {
		cod->spcod.precinct_sizes = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, (1+cod->spcod.decomposition_level_num)*sizeof(jpx_byte));
		for (i = 0; i <= cod->spcod.decomposition_level_num; i++) {
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &cod->spcod.precinct_sizes[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing COD marker segment error.");
				}
				jpc_marker_cod_destroy(jpcdec, cod);
				return NULL;
			}
		}
		offset += 1+cod->spcod.decomposition_level_num;
	}

	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return cod;
}
jpc_marker_cocp			jpc_parse_marker_coc(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, comp;
	jpx_byte i;
	jpc_marker_cocp coc;
	
	offset = 2;
	if (jpcdec->component_num < 257) {
		jpx_byte c;
		if (!jpx_bitstream_readbyte(jpcdec->bitstream, &c)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing COC marker segment error.");
			}
			return NULL;
		}
		comp = c;
		offset += 1;
	} else {
		if (!jpx_bitstream_readshort(jpcdec->bitstream, &comp)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing COC marker segment error.");
			}
			return NULL;
		}
		offset += 2;
	}
	if (comp >= jpcdec->component_num) {
		if (jpcdec->error_func) {
			jpcdec->error_func("COC component index is >= componentNum.");
		}
		return NULL;
	}

	coc = jpc_marker_coc_create(jpcdec);
	coc->ccoc = comp;

	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &coc->scoc)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.decomposition_level_num)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.code_block_width)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.code_block_height)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.code_block_style)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.transformation)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing COC marker segment error.");
		}
		jpc_marker_coc_destroy(jpcdec, coc);
		return NULL;
	}
	offset += 6;
	
	if (coc->scoc & 1) {
		coc->spcoc.precinct_sizes = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, (1+coc->spcoc.decomposition_level_num)*sizeof(jpx_byte));
		for (i = 0; i <= coc->spcoc.decomposition_level_num; i++) {
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &coc->spcoc.precinct_sizes[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing COC marker segment error.");
				}
				jpc_marker_coc_destroy(jpcdec, coc);
				return NULL;
			}
		}
		offset += 1+coc->spcoc.decomposition_level_num;
	}

	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return coc;
}
jpc_marker_qcdp			jpc_parse_marker_qcd(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, i;
	jpc_marker_qcdp	qcd;

	qcd = jpc_marker_qcd_create(jpcdec);

	offset = 2;
	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &qcd->sqcd)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing QCD marker segment error.");
		}
		jpc_marker_qcd_destroy(jpcdec, qcd);
		return NULL;
	}
	offset += 1;
	
	if ((qcd->sqcd&0x1f) == 0) {
		jpx_byte quantization_step;
		qcd->spqcdnum = markerheader->len - offset;
		qcd->spqcds = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcd->spqcdnum*sizeof(jpx_uint16));
		for (i = 0; i < qcd->spqcdnum; i++) {
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &quantization_step)) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing QCD marker segment error.");
				}
				jpc_marker_qcd_destroy(jpcdec, qcd);
				return NULL;
			}
			qcd->spqcds[i] = quantization_step;
		}
		offset += qcd->spqcdnum;
	} else if ((qcd->sqcd&0x1f) == 1) {
		qcd->spqcdnum = 1;
		qcd->spqcds = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcd->spqcdnum*sizeof(jpx_uint16));
		if (!jpx_bitstream_readshort(jpcdec->bitstream, &qcd->spqcds[0])) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing QCD marker segment error.");
			}
			jpc_marker_qcd_destroy(jpcdec, qcd);
			return NULL;
		}
		offset += 2;
	} else if ((qcd->sqcd&0x1f) == 2) {
		qcd->spqcdnum = (markerheader->len-offset) >> 1;
		qcd->spqcds = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcd->spqcdnum*sizeof(jpx_uint16));
		for (i = 0; i < qcd->spqcdnum; i++) {
			if (!jpx_bitstream_readshort(jpcdec->bitstream, &qcd->spqcds[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing QCD marker segment error.");
				}
				jpc_marker_qcd_destroy(jpcdec, qcd);
				return NULL;
			}
		}
		offset += qcd->spqcdnum<<1;
	} else {
		if (jpcdec->error_func) {
			jpcdec->error_func("QCD marker segment unknown quantization style!");
		}
		jpc_marker_qcd_destroy(jpcdec, qcd);
		return NULL;
	}
	
	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return qcd;
}
jpc_marker_qccp			jpc_parse_marker_qcc(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, i, comp;
	jpc_marker_qccp	qcc;

	offset = 2;
	if (jpcdec->component_num < 257) {
		jpx_byte c;
		if (!jpx_bitstream_readbyte(jpcdec->bitstream, &c)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing QCC marker segment error.");
			}
			return NULL;
		}
		comp = c;
		offset += 1;
	} else {
		if (!jpx_bitstream_readshort(jpcdec->bitstream, &comp)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing QCC marker segment error.");
			}
			return NULL;
		}
		offset += 2;
	}
	if (comp >= jpcdec->component_num) {
		if (jpcdec->error_func) {
			jpcdec->error_func("QCC component index is >= componentNum.");
		}
		return NULL;
	}

	qcc = jpc_marker_qcc_create(jpcdec);
	qcc->cqcc = comp;

	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &qcc->sqcc)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing QCC marker segment error.");
		}
		jpc_marker_qcc_destroy(jpcdec, qcc);
		return NULL;
	}
	offset += 1;

	if ((qcc->sqcc&0x1f) == 0) {
		jpx_byte quantization_step;
		qcc->spqccnum = markerheader->len - offset;
		qcc->spqccs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcc->spqccnum*sizeof(jpx_uint16));
		for (i = 0; i < qcc->spqccnum; i++) {
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &quantization_step)) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing QCC marker segment error.");
				}
				jpc_marker_qcc_destroy(jpcdec, qcc);
				return NULL;
			}
			qcc->spqccs[i] = quantization_step;
		}
		offset += qcc->spqccnum;
	} else if ((qcc->sqcc&0x1f) == 1) {
		qcc->spqccnum = 1;
		qcc->spqccs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcc->spqccnum*sizeof(jpx_uint16));
		if (!jpx_bitstream_readshort(jpcdec->bitstream, &qcc->spqccs[0])) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing QCC marker segment error.");
			}
			jpc_marker_qcc_destroy(jpcdec, qcc);
			return NULL;
		}
		offset += 2;
	} else if ((qcc->sqcc&0x1f) == 2) {
		qcc->spqccnum = (markerheader->len-offset) >> 1;
		qcc->spqccs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, qcc->spqccnum*sizeof(jpx_uint16));
		for (i = 0; i < qcc->spqccnum; i++) {
			if (!jpx_bitstream_readshort(jpcdec->bitstream, &qcc->spqccs[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing QCC marker segment error.");
				}
				jpc_marker_qcc_destroy(jpcdec, qcc);
				return NULL;
			}
		}
		offset += qcc->spqccnum<<1;
	} else {
		if (jpcdec->error_func) {
			jpcdec->error_func("QCC marker segment unknown quantization style!");
		}
		jpc_marker_qcc_destroy(jpcdec, qcc);
		return NULL;
	}
	
	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return qcc;
}
jpc_marker_rgnp			jpc_parse_marker_rgn(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, comp;
	jpc_marker_rgnp	rgn;

	offset = 2;
	if (jpcdec->component_num < 257) {
		jpx_byte c;
		if (!jpx_bitstream_readbyte(jpcdec->bitstream, &c)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing RGN marker segment error.");
			}
			return NULL;
		}
		comp = c;
		offset += 1;
	} else {
		if (!jpx_bitstream_readshort(jpcdec->bitstream, &comp)) {
			if (jpcdec->error_func) {
				jpcdec->error_func("Parsing RGN marker segment error.");
			}
			return NULL;
		}
		offset += 2;
	}
	if (comp >= jpcdec->component_num) {
		if (jpcdec->error_func) {
			jpcdec->error_func("RGN component index is >= componentNum.");
		}
		return NULL;
	}

	rgn = jpc_marker_rgn_create(jpcdec);
	rgn->crgn = comp;

	if (!jpx_bitstream_readbyte(jpcdec->bitstream, &rgn->srgn)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &rgn->sprgn)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing RGN marker segment error.");
		}
		jpc_marker_rgn_destroy(jpcdec, rgn);
		return NULL;
	}
	offset += 2;

	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return rgn;
}
jpc_marker_pocp			jpc_parse_marker_poc(jpc_decp jpcdec, jpc_marker_headerp markerheader)
{
	jpx_uint16 offset, i;
	jpc_marker_pocp poc;

	poc = jpc_marker_poc_create(jpcdec);

	offset = 2;
	if (jpcdec->component_num < 257) {
		poc->povnum = (markerheader->len - offset) / 7;
		poc->rspocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));
		poc->cspocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->lyepocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->repocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));
		poc->cepocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->ppocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));

		for (i = 0; i < poc->povnum; i++) {
			jpx_byte cs, ce;
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &poc->rspocs[i])
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &cs)
				|| !jpx_bitstream_readshort(jpcdec->bitstream, &poc->lyepocs[i])
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &poc->repocs[i])
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &ce)
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &poc->ppocs[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing POC marker segment error.");
				}
				jpc_marker_poc_destroy(jpcdec, poc);
				return NULL;
			}
			poc->cspocs[i] = cs;
			poc->cepocs[i] = ce;
			offset += 7;
		}
	} else {
		poc->povnum = (markerheader->len-offset) / 9;
		poc->rspocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));
		poc->cspocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->lyepocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->repocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));
		poc->cepocs = (jpx_uint16p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_uint16));
		poc->ppocs = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, poc->povnum*sizeof(jpx_byte));

		for (i = 0; i < poc->povnum; i++) {
			if (!jpx_bitstream_readbyte(jpcdec->bitstream, &poc->rspocs[i])
				|| !jpx_bitstream_readshort(jpcdec->bitstream, &poc->cspocs[i])
				|| !jpx_bitstream_readshort(jpcdec->bitstream, &poc->lyepocs[i])
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &poc->repocs[i])
				|| !jpx_bitstream_readshort(jpcdec->bitstream, &poc->cepocs[i])
				|| !jpx_bitstream_readbyte(jpcdec->bitstream, &poc->ppocs[i])) {
				if (jpcdec->error_func) {
					jpcdec->error_func("Parsing POC marker segment error.");
				}
				jpc_marker_poc_destroy(jpcdec, poc);
				return NULL;
			}
			offset += 9;
		}
	}

	if (offset < markerheader->len) {
		jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader->len-offset);
	}
	return poc;
}
jpx_bool				jpc_parse_marker_sot(jpc_decp jpcdec, jpc_marker_sotp sot)
{
	if (!jpx_bitstream_readshort(jpcdec->bitstream, &sot->isot)
		|| !jpx_bitstream_readinteger(jpcdec->bitstream, &sot->psot)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &sot->tpsot)
		|| !jpx_bitstream_readbyte(jpcdec->bitstream, &sot->tnsot)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing SOT marker error.");
		}
		return jpx_false;
	}
	
	if (sot->isot >= jpcdec->tilenum) {
		if (jpcdec->error_func) {
			jpcdec->error_func("SOT marker segment's Isot is >= tileNum.");
		}
		return jpx_false;
	}
	if (sot->tpsot >= sot->tnsot) {
		if (jpcdec->warn_func) {
			jpcdec->warn_func("SOT marker segment's TPsot is >= TNsot.");
		}
	}
	sot->offset = 12;

	return jpx_true;
}
jpx_bool					jpc_parse_marker_sop(jpc_decp jpcdec, jpx_uint16p packet_sequence)
{
	if (!jpx_bitstream_readshort(jpcdec->bitstream, packet_sequence)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Parsing SOP marker error.");
		}
		return jpx_false;
	}
	return jpx_true;
}
jpc_headerp					jpc_parse_main_header(jpc_decp jpcdec)
{
	jpc_marker_header markerheader;
	jpc_headerp	mainheader;
	jpx_bool foundsot;

	// Check JPEG2000 codestream format.
	jpcdec->bitstream->stream->seekpos_func(jpcdec->bitstream->stream, jpcdec->bitstream->pos);
	if (!jpx_is_jpc(jpcdec->bitstream->stream)) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Missing marker of JPEG2000 codestream.");
		}
		return NULL;
	}
	jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+2);

	// Parse SIZ marker
	if (!jpc_parse_marker_header(jpcdec, &markerheader)) return NULL;
	if (markerheader.code != JPC_MARKER_SIZ) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Missing SIZ marker");
		}
		return NULL;
	}
	if (!jpc_parse_marker_siz(jpcdec, &markerheader)) {
		return NULL;
	}

	// Check and allocate tiles buffer
	jpcdec->xtilenum = (jpcdec->siz.xsiz-jpcdec->siz.xtosiz+jpcdec->siz.xtsiz-1) / jpcdec->siz.xtsiz;
	jpcdec->ytilenum = (jpcdec->siz.ysiz-jpcdec->siz.ytosiz+jpcdec->siz.ytsiz-1) / jpcdec->siz.ytsiz;
	jpcdec->tilenum = jpcdec->xtilenum*jpcdec->ytilenum;
	if (jpcdec->tilenum <= 0) {
		if (jpcdec->error_func) {
			jpcdec->error_func("There are none tiles in the codestream.");
		}
		return NULL;
	}
	jpcdec->tiles = (jpc_tilep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, jpcdec->tilenum*sizeof(jpc_tile));
	jpx_memset(jpcdec->tiles, 0, jpcdec->tilenum*sizeof(jpc_tile));

	mainheader = jpc_header_create(jpcdec, jpcdec->siz.csiz);
	foundsot = jpx_false;
	while ((jpx_bitstream_getbytesleft(jpcdec->bitstream) > 0) && !foundsot) {
		if (!jpc_parse_marker_header(jpcdec, &markerheader)) {
			jpc_header_destroy(jpcdec, mainheader);
			return NULL;
		}
		switch (markerheader.code)
		{
		case JPC_MARKER_COD:
			{
				if (mainheader->cod) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("COD marker has been already parsed, ignored.");
					}
					jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
				} else {
					mainheader->cod = jpc_parse_marker_cod(jpcdec, &markerheader);
					if (mainheader->cod == NULL) {
						jpc_header_destroy(jpcdec, mainheader);
						return NULL;
					}
				}
			}
			break;
		case JPC_MARKER_COC:
			{
				jpc_marker_cocp coc = jpc_parse_marker_coc(jpcdec, &markerheader);
				if (coc == NULL) {
					jpc_header_destroy(jpcdec, mainheader);
					return NULL;
				}
				if (mainheader->cocs[coc->ccoc]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("COC marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_coc_destroy(jpcdec, coc);
				} else {
					mainheader->cocs[coc->ccoc] = coc;
				}
			}
			break;
		case JPC_MARKER_QCD:
			{
				if (mainheader->qcd) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("QCD marker has been already parsed, ignored.");
					}
					jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
				} else {
					mainheader->qcd = jpc_parse_marker_qcd(jpcdec, &markerheader);
					if (mainheader->qcd == NULL) {
						jpc_header_destroy(jpcdec, mainheader);
						return NULL;
					}
				}
			}
			break;
		case JPC_MARKER_QCC:
			{
				jpc_marker_qccp qcc = jpc_parse_marker_qcc(jpcdec, &markerheader);
				if (qcc == NULL) {
					jpc_header_destroy(jpcdec, mainheader);
					return NULL;
				}
				if (mainheader->qccs[qcc->cqcc]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("QCC marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_qcc_destroy(jpcdec, qcc);
				} else {
					mainheader->qccs[qcc->cqcc] = qcc;
				}
			}
			break;
		case JPC_MARKER_RGN:
			{
				jpc_marker_rgnp rgn = jpc_parse_marker_rgn(jpcdec, &markerheader);
				if (rgn == NULL) {
					jpc_header_destroy(jpcdec, mainheader);
					return NULL;
				}
				if (mainheader->rgns[rgn->crgn]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("RGN marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_rgn_destroy(jpcdec, rgn);
				} else {
					mainheader->rgns[rgn->crgn] = rgn;
				}
			}
			break;
		case JPC_MARKER_POC:
			{
				jpc_marker_pocp poc = jpc_parse_marker_poc(jpcdec, &markerheader);
				if (poc == NULL) {
					jpc_header_destroy(jpcdec, mainheader);
					return NULL;
				}
				if (mainheader->poclist) {
					jpc_marker_pocp cur = mainheader->poclist, prev;
					do {
						prev = cur;
						cur = prev->next;
					} while (cur);
					prev->next = poc;
				} else {
					mainheader->poclist = poc;
				}
			}
			break;
		case JPC_MARKER_PPM:
		case JPC_MARKER_TLM:
		case JPC_MARKER_PLM:
		case JPC_MARKER_CRG:
		case JPC_MARKER_COM:
			{
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Unsupported markers, ignored.");
				}
				jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
			}
			break;
		case JPC_MARKER_SOT:
			{
				foundsot = jpx_true;
			}
			break;
		default:
			jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
		}
	}
	
	if (!mainheader->cod) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Missing COD marker.");
		}
		jpc_header_destroy(jpcdec, mainheader);
		return NULL;
	}
	if (!mainheader->qcd) {
		if (jpcdec->error_func) {
			jpcdec->error_func("Missing QCD marker.");
		}
		jpc_header_destroy(jpcdec, mainheader);
		return NULL;
	}

	return mainheader;
}
jpc_headerp					jpc_parse_tile_header0(jpc_decp jpcdec, jpc_marker_sotp sot)
{
	jpc_marker_header markerheader;
	jpx_bool foundsod;
	jpc_headerp tileheader;

	tileheader = jpc_header_create(jpcdec, jpcdec->siz.csiz);
	foundsod = jpx_false;
	while (!foundsod) {
		if (sot->psot != 0) {
			if (sot->offset >= sot->psot) break;
		}
		
		if (!jpc_parse_marker_header(jpcdec, &markerheader)) {
			jpc_header_destroy(jpcdec, tileheader);
			return NULL;
		}
		sot->offset += 2+markerheader.len;
		
		switch (markerheader.code)
		{
		case JPC_MARKER_COD:
			{
				if (tileheader->cod) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("COD marker has been already parsed, ignored.");
					}
					jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
				} else {
					tileheader->cod = jpc_parse_marker_cod(jpcdec, &markerheader);
					if (tileheader->cod == NULL) {
						jpc_header_destroy(jpcdec, tileheader);
						return NULL;
					}
				}
			}
			break;
		case JPC_MARKER_COC:
			{
				jpc_marker_cocp coc = jpc_parse_marker_coc(jpcdec, &markerheader);
				if (coc == NULL) {
					jpc_header_destroy(jpcdec, tileheader);
					return NULL;
				}
				if (tileheader->cocs[coc->ccoc]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("COC marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_coc_destroy(jpcdec, coc);
				} else {
					tileheader->cocs[coc->ccoc] = coc;
				}
			}
			break;
		case JPC_MARKER_QCD:
			{
				if (tileheader->qcd) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("QCD marker has been already parsed, ignored.");
					}
					jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
				} else {
					tileheader->qcd = jpc_parse_marker_qcd(jpcdec, &markerheader);
					if (tileheader->qcd == NULL) {
						jpc_header_destroy(jpcdec, tileheader);
						return NULL;
					}
				}
			}
			break;
		case JPC_MARKER_QCC:
			{
				jpc_marker_qccp qcc = jpc_parse_marker_qcc(jpcdec, &markerheader);
				if (qcc == NULL) {
					jpc_header_destroy(jpcdec, tileheader);
					return NULL;
				}
				if (tileheader->qccs[qcc->cqcc]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("QCC marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_qcc_destroy(jpcdec, qcc);
				} else {
					tileheader->qccs[qcc->cqcc] = qcc;
				}
			}
			break;
		case JPC_MARKER_RGN:
			{
				jpc_marker_rgnp rgn = jpc_parse_marker_rgn(jpcdec, &markerheader);
				if (rgn == NULL) {
					jpc_header_destroy(jpcdec, tileheader);
					return NULL;
				}
				if (tileheader->rgns[rgn->crgn]) {
					if (jpcdec->warn_func) {
						jpcdec->warn_func("RGN marker of specified component has been already parsed, ignored.");
					}
					jpc_marker_rgn_destroy(jpcdec, rgn);
				} else {
					tileheader->rgns[rgn->crgn] = rgn;
				}
			}
			break;
		case JPC_MARKER_POC:
			{
				jpc_marker_pocp poc = jpc_parse_marker_poc(jpcdec, &markerheader);
				if (poc == NULL) {
					jpc_header_destroy(jpcdec, tileheader);
					return NULL;
				}
				if (tileheader->poclist) {
					jpc_marker_pocp cur = tileheader->poclist, prev;
					do {
						prev = cur;
						cur = prev->next;
					} while (cur);
					prev->next = poc;
				} else {
					tileheader->poclist = poc;
				}
			}
			break;
		case JPC_MARKER_PPT:
		case JPC_MARKER_PLT:
		case JPC_MARKER_COM:
			{
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Unsupported markers, ignored.");
				}
				jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
			}
			break;
		case JPC_MARKER_SOD:
			{
				foundsod = jpx_true;
			}
			break;
		default:
			jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
		}
	}

	return tileheader;
}
jpc_headerp					jpc_parse_tile_header1(jpc_decp jpcdec, jpc_marker_sotp sot)
{
	jpc_marker_header markerheader;
	jpx_bool foundsod;
	jpc_headerp tileheader;

	tileheader = jpc_header_create(jpcdec, jpcdec->siz.csiz);
	foundsod = jpx_false;
	while (!foundsod) {
		if (sot->psot != 0) {
			if (sot->offset >= sot->psot) break;
		}

		if (!jpc_parse_marker_header(jpcdec, &markerheader)) {
			jpc_header_destroy(jpcdec, tileheader);
			return NULL;
		}
		sot->offset += 2+markerheader.len;
		
		switch (markerheader.code)
		{
		case JPC_MARKER_POC:
			{
				jpc_marker_pocp poc = jpc_parse_marker_poc(jpcdec, &markerheader);
				if (poc == NULL) {
					jpc_header_destroy(jpcdec, tileheader);
					return NULL;
				}
				if (tileheader->poclist) {
					jpc_marker_pocp cur = tileheader->poclist, prev;
					do {
						prev = cur;
						cur = prev->next;
					} while (cur);
					prev->next = poc;
				} else {
					tileheader->poclist = poc;
				}
			}
			break;
		case JPC_MARKER_PPT:
		case JPC_MARKER_PLT:
		case JPC_MARKER_COM:
			{
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Unsupported markers, ignored.");
				}
				jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
			}
			break;
		case JPC_MARKER_SOD:
			{
				foundsod = jpx_true;
			}
			break;
		default:
			jpx_bitstream_setpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+markerheader.len-2);
		}
	}

	return tileheader;
}

jpc_segmentp				jpc_segment_create(jpc_decp jpcdec)
{
	jpc_segmentp segment = (jpc_segmentp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_segment));
	jpx_memset(segment, 0, sizeof(jpc_segment));
	return segment;
}
void						jpc_segment_destroy(jpc_decp jpcdec, jpc_segmentp segment)
{
	if (segment->buf) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, segment->buf);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, segment);
}
jpc_segment_listp			jpc_segment_list_create(jpc_decp jpcdec)
{
	jpc_segment_listp sglist = (jpc_segment_listp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_segment_list));
	jpx_memset(sglist, 0, sizeof(jpc_segment_list));
	return sglist;
}
void						jpc_segment_list_destroy(jpc_decp jpcdec, jpc_segment_listp seglist)
{
	if (seglist->header) {
		jpc_segmentp segment = seglist->header, next;
		while (segment) {
			next = segment->next;
			jpc_segment_destroy(jpcdec, segment);
			segment = next;
		}
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, seglist);
}

void						jpc_reslevel_init(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r)
{
	jpc_reslevelp		res_level;
	jpc_precinctp		precinct;
	jpc_precinct_bandp	band;
	jpc_subbandp		subband;
	jpc_codeblockp		cb;
	jpx_uint32 nb,xob,yob,bnd,hprctstart,vprctstart,hprctend,vprctend,hprctbstart,vprctbstart,
		xcb1,ycb1,prcth,prctv,hcbstartno,vcbstartno,hcbendno,vcbendno,cbh,cbv;
	jpx_byte ppx1, ppy1;
	jpx_int32 i;

	res_level = &tc->reslevels[r];
	res_level->tc = tc;
	res_level->r = r;
	// Compute resolution level matrix and sub-band matrix.
	res_level->trx0 = (tc->tcx0+(1<<(tc->decomposition_level_num-r))-1)
		/ (1<<(tc->decomposition_level_num-r));
	res_level->try0 = (tc->tcy0+(1<<(tc->decomposition_level_num-r))-1)
		/ (1<<(tc->decomposition_level_num-r));
	res_level->trx1 = (tc->tcx1+(1<<(tc->decomposition_level_num-r))-1)
		/ (1<<(tc->decomposition_level_num-r));
	res_level->try1 = (tc->tcy1+(1<<(tc->decomposition_level_num-r))-1)
		/ (1<<(tc->decomposition_level_num-r));
	
	res_level->subband_num = r == 0 ? 1 : 3;
	if (r == 0) {
		// nbLL sub-band, xob=0; yob=0;
		res_level->llx0 = res_level->trx0;
		res_level->lly0 = res_level->try0;
		res_level->llx1 = res_level->trx1;
		res_level->lly1 = res_level->try1;
		
		res_level->subbands[0].tbx0 = res_level->trx0;
		res_level->subbands[0].tby0 = res_level->try0;
		res_level->subbands[0].tbx1 = res_level->trx1;
		res_level->subbands[0].tby1 = res_level->try1;
		
		res_level->subbands[0].hcodeblock_num = 0;
		res_level->subbands[0].vcodeblock_num = 0;
	} else {
		res_level->llx0 = tc->reslevels[r-1].trx0;
		res_level->lly0 = tc->reslevels[r-1].try0;
		res_level->llx1 = tc->reslevels[r-1].trx1;
		res_level->lly1 = tc->reslevels[r-1].try1;
		
		nb = tc->decomposition_level_num - r + 1;
		for (bnd = 0; bnd < res_level->subband_num; bnd ++) {
			switch (bnd)
			{
			case 0:
				// nbHL(horizontal hight-pass)
				xob = 1; yob = 0;
				break;
			case 1:
				// nbLH(vertical high-pass)
				xob = 0; yob = 1;
				break;
			case 2:
				// nbHH sub-band
				xob = 1; yob = 1;
				break;
			}
			res_level->subbands[bnd].tbx0 = (tc->tcx0-(1<<(nb-1))*xob+(1<<nb)-1) / (1<<nb);
			res_level->subbands[bnd].tby0 = (tc->tcy0-(1<<(nb-1))*yob+(1<<nb)-1) / (1<<nb);
			res_level->subbands[bnd].tbx1 = (tc->tcx1-(1<<(nb-1))*xob+(1<<nb)-1) / (1<<nb);
			res_level->subbands[bnd].tby1 = (tc->tcy1-(1<<(nb-1))*yob+(1<<nb)-1) / (1<<nb);
			
			res_level->subbands[bnd].hcodeblock_num = 0;
			res_level->subbands[bnd].vcodeblock_num = 0;
		}
		
		res_level->low_width = res_level->llx1 - res_level->llx0;
		res_level->high_width = res_level->subbands[0].tbx1 - res_level->subbands[0].tbx0;
		
		if (tc->transformation) {
			jpc_linebufs_init(&res_level->lines, jpcdec, 3);
			jpc_linebufs_init(&res_level->lows, jpcdec, 3);
			jpc_linebufs_init(&res_level->highs, jpcdec, 3);
			
			for (i = 0; i < 3; i++) {
				res_level->lines.lines[i].buf.buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
				jpx_memset(res_level->lines.lines[i].buf.buf32, 0, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
				res_level->lows.lines[i].buf.buf32 = res_level->lines.lines[i].buf.buf32 + 4;
				res_level->highs.lines[i].buf.buf32 = res_level->lows.lines[i].buf.buf32 + res_level->low_width;
			}
			res_level->templine.buf.buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
			jpx_memset(res_level->templine.buf.buf32, 0, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
			res_level->templow.buf.buf32 = res_level->templine.buf.buf32 + 4;
			res_level->temphigh.buf.buf32 = res_level->templow.buf.buf32 + res_level->low_width;
		} else {
			jpc_linebufs_init(&res_level->lines, jpcdec, 5);
			jpc_linebufs_init(&res_level->lows, jpcdec, 5);
			jpc_linebufs_init(&res_level->highs, jpcdec, 5);
			
			for (i = 0; i < 5; i++) {
				res_level->lines.lines[i].buf.buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
				jpx_memset(res_level->lines.lines[i].buf.buf32, 0, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
				res_level->lows.lines[i].buf.buf32 = res_level->lines.lines[i].buf.buf32 + 4;
				res_level->highs.lines[i].buf.buf32 = res_level->lows.lines[i].buf.buf32 + res_level->low_width;
			}
			res_level->templine.buf.buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
			jpx_memset(res_level->templine.buf.buf32, 0, sizeof(jpx_int32)*(res_level->trx1-res_level->trx0+8));
			res_level->templow.buf.buf32 = res_level->templine.buf.buf32 + 4;
			res_level->temphigh.buf.buf32 = res_level->templow.buf.buf32 + res_level->low_width;
		}
	}
	
	// Compute precincts.
	if (res_level->trx0 == res_level->trx1) {
		res_level->numprecinctswide = 0;
	} else {
		hprctstart = (res_level->trx0>>res_level->ppx) << res_level->ppx;
		hprctend = ((res_level->trx1+(1<<res_level->ppx)-1)>>res_level->ppx) << res_level->ppx;
		res_level->numprecinctswide = (hprctend-hprctstart) >> res_level->ppx;
	}
	if (res_level->try0 == res_level->try1) {
		res_level->numprecinctshigh = 0;
	} else {
		vprctstart = (res_level->try0>>res_level->ppy) << res_level->ppy;
		vprctend = ((res_level->try1+(1<<res_level->ppy)-1)>>res_level->ppy) << res_level->ppy;
		res_level->numprecinctshigh = (vprctend-vprctstart) >> res_level->ppy;
	}
	res_level->numprecinct = res_level->numprecinctswide * res_level->numprecinctshigh;
	
	if (res_level->numprecinct > 0) {
		if (r == 0) {
			hprctbstart = hprctstart;
			vprctbstart = vprctstart;
			ppx1 = res_level->ppx;
			ppy1 = res_level->ppy;
			xcb1 = tc->xcb < res_level->ppx ? tc->xcb : res_level->ppx;
			ycb1 = tc->ycb < res_level->ppy ? tc->ycb : res_level->ppy;
		} else {
			hprctbstart = (hprctstart + 1) >> 1;
			vprctbstart = (vprctstart + 1) >> 1;
			ppx1 = res_level->ppx - 1;
			ppy1 = res_level->ppy - 1;
			xcb1 = tc->xcb < ppx1 ? tc->xcb : ppx1;
			ycb1 = tc->ycb < ppy1 ? tc->ycb : ppy1;
		}
		
		res_level->precincts = jpcdec->memmgr->alloc_func(jpcdec->memmgr, res_level->numprecinct*sizeof(jpc_precinct));
		for (prctv = 0; prctv < res_level->numprecinctshigh; prctv ++) {
			for (prcth = 0; prcth < res_level->numprecinctswide; prcth ++) {
				precinct = &res_level->precincts[prctv*res_level->numprecinctswide+prcth];
				precinct->x0 = (hprctstart+(prcth<<res_level->ppx)) > res_level->trx0 ?
					(hprctstart+(prcth<<res_level->ppx)) : res_level->trx0;
				precinct->y0 = (vprctstart+(prctv<<res_level->ppy)) > res_level->try0 ?
					(vprctstart+(prctv<<res_level->ppy)) : res_level->try0;
				precinct->x1 = (hprctstart+((prcth+1)<<res_level->ppx)) < res_level->trx1 ?
					(hprctstart+((prcth+1)<<res_level->ppx)) : res_level->trx1;
				precinct->y1 = (vprctstart+((prctv+1)<<res_level->ppy)) < res_level->try1 ?
					(vprctstart+((prctv+1)<<res_level->ppy)) : res_level->try1;
				
				precinct->layer = 0;
				
				precinct->bands = jpcdec->memmgr->alloc_func(jpcdec->memmgr, res_level->subband_num*sizeof(jpc_precinct_band));
				for (bnd = 0; bnd < res_level->subband_num; bnd++) {
					band = &precinct->bands[bnd];
					
					band->x0 = hprctbstart+(prcth<<ppx1) > res_level->subbands[bnd].tbx0 ?
						hprctbstart+(prcth<<ppx1) : res_level->subbands[bnd].tbx0;
					band->y0 = vprctbstart+(prctv<<ppy1) > res_level->subbands[bnd].tby0 ?
						vprctbstart+(prctv<<ppy1) : res_level->subbands[bnd].tby0;
					band->x1 = hprctbstart+((prcth+1)<<ppx1) < res_level->subbands[bnd].tbx1 ?
						hprctbstart+((prcth+1)<<ppx1) : res_level->subbands[bnd].tbx1;
					band->y1 = vprctbstart+((prctv + 1)<<ppy1) < res_level->subbands[bnd].tby1 ?
						vprctbstart+((prctv+1)<<ppy1) : res_level->subbands[bnd].tby1;
					
					if (band->x1 <= band->x0) {
						band->hcodeblock_num = 0;
					} else {
						hcbstartno = band->x0 / (1<<xcb1);
						hcbendno = (band->x1+(1<<xcb1)-1) / (1<<xcb1);
						band->hcodeblock_num = hcbendno - hcbstartno;
					}
					if (band->y1 <= band->y0) {
						band->vcodeblock_num = 0;
					} else {
						vcbstartno = band->y0 / (1<<xcb1);
						vcbendno = (band->y1+(1<<xcb1)-1) / (1<<xcb1);
						band->vcodeblock_num = vcbendno - vcbstartno;
					}
					
					if (band->hcodeblock_num*band->vcodeblock_num > 0) {
						band->inctt = jpx_tagtree_create(jpcdec->memmgr, band->hcodeblock_num, band->vcodeblock_num);
						band->zbptt = jpx_tagtree_create(jpcdec->memmgr, band->hcodeblock_num, band->vcodeblock_num);
						band->cbs = (jpc_codeblockp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, 
							band->hcodeblock_num*band->vcodeblock_num*sizeof(jpc_codeblock));
						for (cbv = 0; cbv < band->vcodeblock_num; cbv ++) {
							for (cbh = 0; cbh < band->hcodeblock_num; cbh ++) {
								cb = &band->cbs[cbv*band->hcodeblock_num+cbh];
								cb->x0 = ((hcbstartno+cbh)<<xcb1) > band->x0 ?
									((hcbstartno+cbh)<<xcb1) : band->x0;
								cb->y0 = ((vcbstartno+cbv)<<ycb1) > band->y0 ?
									((vcbstartno+cbv)<<ycb1) : band->y0;
								cb->x1 = ((hcbstartno+cbh+1)<<xcb1) < band->x1 ?
									((hcbstartno+cbh+1)<<xcb1) : band->x1;
								cb->y1 = ((vcbstartno+cbv+1)<<ycb1) < band->y1 ?
									((vcbstartno+cbv+1)<<ycb1) : band->y1;
								
								cb->p = 0;
								cb->lblock = 3;
								cb->seen = jpx_false;
								cb->passtype = JPC_CLN_PASS;
								cb->passnum = 0;
								cb->contexts = (jpx_arith_ctxp)jpcdec->memmgr->alloc_func(jpcdec->memmgr,
									JPC_MAX_CONTEXTS*sizeof(jpx_arith_ctx));
								jpx_arith_reset_contexts(cb->contexts);
								cb->seglist = jpc_segment_list_create(jpcdec);
							}
						}
					} else {
						band->cbs = 0;
						band->inctt = 0;
						band->zbptt = 0;
					}
				}
			}
		}
		
		res_level->y = res_level->try0;
		for (bnd = 0; bnd < res_level->subband_num; bnd++) {
			jpx_uint32 hcbnum=0,vcbnum=0;
			subband = &res_level->subbands[bnd];
			
			subband->vcodeblock_num = 0;
			precinct = res_level->precincts;
			for (prctv = 0; prctv < res_level->numprecinctshigh; prctv++) {
				subband->vcodeblock_num += precinct->bands[bnd].vcodeblock_num;
				precinct += res_level->numprecinctswide;
			}
			subband->hcodeblock_num = 0;
			precinct = res_level->precincts;
			for (prcth = 0; prcth < res_level->numprecinctswide; prcth++) {
				subband->hcodeblock_num += precinct->bands[bnd].hcodeblock_num;
				precinct += 1;
			}
			
			if (subband->vcodeblock_num*subband->hcodeblock_num > 0) {
				subband->y = subband->tby0;
				subband->cby = 0;
				subband->refs = (jpc_codeblockpp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, 
					subband->vcodeblock_num*subband->hcodeblock_num*sizeof(jpc_codeblockp));
				
				for (vcbnum = 0,prctv = 0; prctv < res_level->numprecinctshigh; prctv++) {
					precinct = &res_level->precincts[prctv*res_level->numprecinctswide];
					band = &precinct->bands[bnd];
					for (cbv = 0; cbv < band->vcodeblock_num; cbv++,vcbnum++) {
						for (hcbnum = 0,prcth = 0; prcth < res_level->numprecinctswide; prcth++) {
							band = &precinct[prcth].bands[bnd];
							for (cbh = 0; cbh < band->hcodeblock_num; cbh++, hcbnum++) {
								subband->refs[vcbnum*subband->hcodeblock_num+hcbnum]
									= &band->cbs[cbv*band->hcodeblock_num+cbh];
							}
						}
					}
				}
				
				subband->data_height = (jpx_uint32)(1<<ycb1) > subband->tby1-subband->tby0 ?
					subband->tby1-subband->tby0 : (1<<ycb1);
				subband->data_width = subband->tbx1 - subband->tbx0;
				subband->data = jpcdec->memmgr->alloc_func(jpcdec->memmgr, subband->data_height*subband->data_width*sizeof(jpx_int32));
				subband->current_decoded_height = subband->used_height = 0;
			} else {
				subband->refs = 0;
				subband->data = 0;
			}
		}
	} else {
		res_level->precincts = 0;
		for(bnd = 0; bnd < res_level->subband_num; bnd++) {
			res_level->subbands[bnd].refs = 0;
			res_level->subbands[bnd].data = 0;
		}
	}
}
void						jpc_reslevel_finalize(jpc_decp jpcdec, jpc_reslevelp reslevel)
{
	jpc_precinctp		precinct;
	jpc_precinct_bandp	band;
	jpc_codeblockp		cb;
	jpx_uint32 prct, bnd, cbx, cby, i;

	for (prct = 0; prct < reslevel->numprecinct; prct++) {
		precinct = &reslevel->precincts[prct];
		if (precinct->bands) {
			for (bnd = 0; bnd < reslevel->subband_num; bnd++) {
				band = &precinct->bands[bnd];
				if(band->cbs) {
					for (cby = 0; cby < band->vcodeblock_num; cby++) {
						for (cbx = 0; cbx < band->hcodeblock_num; cbx++) {
							cb = &band->cbs[cby * band->hcodeblock_num + cbx];
							if(cb->contexts) {
								jpcdec->memmgr->free_func(jpcdec->memmgr, cb->contexts);
							}
							if(cb->seglist) {
								jpc_segment_list_destroy(jpcdec, cb->seglist);
							}
						}
					}
					jpcdec->memmgr->free_func(jpcdec->memmgr, band->cbs);
				}
				if(band->inctt) {
					jpx_tagtree_destroy(jpcdec->memmgr, band->inctt);
				}
				if(band->zbptt) {
					jpx_tagtree_destroy(jpcdec->memmgr, band->zbptt);
				}
			}
			jpcdec->memmgr->free_func(jpcdec->memmgr, precinct->bands);
		}
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, reslevel->precincts);
	
	for (i = 0; i < reslevel->lines.size; i++) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, reslevel->lines.lines[i].buf.buf32);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, reslevel->templine.buf.buf32);
	
	jpc_linebufs_finalize(&reslevel->lines, jpcdec);
	jpc_linebufs_finalize(&reslevel->lows, jpcdec);
	jpc_linebufs_finalize(&reslevel->highs, jpcdec);
	
	for (bnd = 0; bnd < reslevel->subband_num; bnd++) {
		if (reslevel->subbands[bnd].refs) {
			jpcdec->memmgr->free_func(jpcdec->memmgr, reslevel->subbands[bnd].refs);
		}
		if (reslevel->subbands[bnd].data) {
			jpcdec->memmgr->free_func(jpcdec->memmgr, reslevel->subbands[bnd].data);
		}
	}
}

void						jpc_tilecomp_init(jpc_decp jpcdec, jpc_tilep tile, jpx_uint16 compIdx)
{
	jpc_tilecompp		component;

	component = &tile->components[compIdx];
	component->tile = tile;
	
	component->sgned = (jpcdec->siz.ssizs[compIdx]&0x80) ? jpx_true:jpx_false;
	component->precision = (jpcdec->siz.ssizs[compIdx] & 0x7f) + 1;
	component->xrsiz = jpcdec->siz.xrsizs[compIdx];
	component->yrsiz = jpcdec->siz.yrsizs[compIdx];

	component->tcx0 = (tile->tx0+component->xrsiz-1) / component->xrsiz;
	component->tcy0 = (tile->ty0+component->yrsiz-1) / component->yrsiz;
	component->tcx1 = (tile->tx1+component->xrsiz-1) / component->xrsiz;
	component->tcy1 = (tile->ty1+component->yrsiz-1) / component->yrsiz;
	
	component->reslevels = NULL;
}
void						jpc_tilecomp_finalize(jpc_decp jpcdec, jpc_tilecompp tc)
{
	jpx_uint32 r;

	for (r = 0; r < tc->reslevel_num; r++) {
		jpc_reslevel_finalize(jpcdec, &tc->reslevels[r]);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, tc->reslevels);
}

jpc_po_changep				jpc_po_change_create(jpc_decp jpcdec)
{
	jpc_po_changep pochange = (jpc_po_changep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_po_change));
	jpx_memset(pochange, 0, sizeof(jpc_po_change));
	return pochange;
}
void						jpc_po_change_destroy(jpc_decp jpcdec, jpc_po_changep pochange)
{
	jpcdec->memmgr->free_func(jpcdec->memmgr, pochange);
}
jpc_pop						jpc_po_create(jpc_decp jpcdec, jpc_tilep tile)
{
	jpc_pop po = (jpc_pop)jpcdec->memmgr->alloc_func(jpcdec->memmgr, sizeof(jpc_po));
	jpx_memset(po, 0, sizeof(jpc_po));

	po->tile = tile;
	po->comp_xsteps = (jpx_uint32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, tile->component_num*sizeof(jpx_uint32));
	jpx_memset(po->comp_xsteps, 0, tile->component_num*sizeof(jpx_uint32));
	po->comp_ysteps = (jpx_uint32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, tile->component_num*sizeof(jpx_uint32));
	jpx_memset(po->comp_ysteps, 0, tile->component_num*sizeof(jpx_uint32));

	jpc_po_compute(po);
	jpc_po_normalize(po);
	return po;
}
void						jpc_po_destroy(jpc_decp jpcdec, jpc_pop po)
{
	if (po->header) {
		jpc_po_changep pochange = po->header, next;
		while (pochange) {
			next = pochange->next;
			jpc_po_change_destroy(jpcdec, pochange);
			pochange = next;
		}
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, po->comp_xsteps);
	jpcdec->memmgr->free_func(jpcdec->memmgr, po->comp_ysteps);
	jpcdec->memmgr->free_func(jpcdec->memmgr, po);
}
void						jpc_po_compute(jpc_pop po)
{
	jpx_uint16		i;
	jpx_byte		r;
	jpc_tilecompp	tc;
	jpc_reslevelp	reslevel;
	jpx_uint32		temp;
	
	po->xstep = po->tile->tx1 - po->tile->tx0;
	po->ystep = po->tile->ty1 - po->tile->ty0;
	for (i = 0; i < po->tile->component_num; i++) {
		tc = po->tile->components + i;
		po->comp_xsteps[i] = po->tile->tx1 - po->tile->tx0;
		po->comp_ysteps[i] = po->tile->ty1 - po->tile->ty0;
		for (r = 0; r < tc->reslevel_num; r++) {
			reslevel = tc->reslevels + r;

			temp = 1 << (reslevel->ppx + tc->decomposition_level_num - r);
			temp *= tc->xrsiz;
			if(temp < po->xstep) po->xstep = temp;
			if(temp < po->comp_xsteps[i]) po->comp_xsteps[i] = temp;

			temp = 1 << (reslevel->ppy + tc->decomposition_level_num - r);
			temp *= tc->yrsiz;
			if(temp < po->ystep) po->ystep = temp;
			if(temp < po->comp_ysteps[i]) po->comp_ysteps[i] = temp;
		}
	}
}
void						jpc_po_normalize(jpc_pop po)
{
	jpx_byte		temp;
	jpx_uint16		i;
	jpc_tilecompp	tc;

	po->normal_xrsiz_yrsiz = jpx_true;
	for (i = 0; i < po->tile->component_num; i++) {
		tc = po->tile->components + i;

		temp = tc->xrsiz;
		while (temp > 1) {
			if (temp & 1) {
				po->normal_xrsiz_yrsiz = jpx_false;
				return;
			}
			temp >>= 1;
		}

		temp = tc->yrsiz;
		while (temp > 1) {
			if(temp & 1) {
				po->normal_xrsiz_yrsiz = jpx_false;
				return;
			}
			temp >>= 1;
		}
	}
}
void						jpc_po_init(jpc_pop po)
{
	po->layer_start = po->layer;
	po->res_start = po->cur->rspoc > po->res ? po->cur->rspoc : po->res;
	po->comp_start = po->cur->cspoc > po->comp ? po->cur->cspoc : po->comp;
	po->res = po->res_start;
	po->comp = po->comp_start;
	po->y = po->tile->ty0;
	po->x = po->tile->tx0;
	po->prct = 0;
}
void						jpc_po_add(jpc_pop po, jpc_po_changep pochange)
{
	if (po->header) {
		po->tail->next = pochange;
		pochange->next = 0;
		po->tail = pochange;
		if (!po->cur) {
			po->cur = pochange;
			jpc_po_init(po);
		}
	} else {
		po->tail = po->header = pochange;
		pochange->next = 0;
		po->cur = po->header;
	}
}
void						jpc_po_add_list(jpc_pop po, jpc_decp jpcdec, jpc_marker_pocp poclist)
{
	jpc_marker_pocp cur;
	jpc_po_changep pochange;
	jpx_uint16 i;
	
	cur = poclist;
	while (cur) {
		for (i = 0; i < cur->povnum; i++) {
			pochange = jpc_po_change_create(jpcdec);
			pochange->rspoc = cur->rspocs[i];
			pochange->repoc = cur->repocs[i];
			pochange->cspoc = cur->cspocs[i];
			pochange->cepoc = cur->cepocs[i];
			pochange->lyepoc = cur->lyepocs[i];
			pochange->ppoc = cur->ppocs[i];

			jpc_po_add(po, pochange);
		}
		cur = cur->next;
	}
}
jpx_bool					jpc_po_next_lrcp(jpc_pop po)
{
	for( ; (po->layer<po->cur->lyepoc) && (po->layer<po->tile->layer_num); po->layer++, po->res=po->res_start) {
		for ( ; (po->res<po->cur->repoc) && (po->res<po->tile->max_reslevel_num); po->res++, po->comp=po->comp_start) {
			for( ; (po->comp<po->cur->cepoc) && (po->comp<po->tile->component_num); po->comp++, po->prct=0) {
				if (po->res >= po->tile->components[po->comp].reslevel_num) continue;
				for ( ; po->prct < po->tile->components[po->comp].reslevels[po->res].numprecinct; po->prct++) {
					if (po->layer >= po->tile->components[po->comp].reslevels[po->res].precincts[po->prct].layer) {
						po->tile->components[po->comp].reslevels[po->res].precincts[po->prct].layer = po->layer+1;
						return jpx_true;
					}
				}
			}
		}
	}
	return jpx_false;
}
jpx_bool					jpc_po_next_rlcp(jpc_pop po)
{
	for ( ; (po->res<po->cur->repoc) && (po->res<po->tile->max_reslevel_num); po->res++, po->layer=po->layer_start) {
		for ( ; (po->layer<po->cur->lyepoc) && (po->layer<po->tile->layer_num); po->layer++, po->comp=po->comp_start) {
			for( ; (po->comp<po->cur->cepoc) && (po->comp<po->tile->component_num); po->comp++, po->prct=0) {
				if (po->res >= po->tile->components[po->comp].reslevel_num) continue;
				for ( ; po->prct < po->tile->components[po->comp].reslevels[po->res].numprecinct; po->prct++) {
					if (po->layer >= po->tile->components[po->comp].reslevels[po->res].precincts[po->prct].layer) {
						po->tile->components[po->comp].reslevels[po->res].precincts[po->prct].layer = po->layer+1;
						return jpx_true;
					}
				}
			}
		}
	}
	return jpx_false;
}
jpx_bool					jpc_po_next_rpcl(jpc_pop po)
{
	jpx_byte expx, expy;
	jpc_tilecompp	tc;
	jpc_reslevelp	reslevel;
	jpx_uint32 tempx, tempy;
	
	for ( ; (po->res<po->cur->repoc) && (po->res<po->tile->max_reslevel_num); po->res++, po->y=po->tile->ty0) {
		for ( ; po->y < po->tile->ty1; po->y+=po->ystep, po->x=po->tile->tx0) {
			for ( ; po->x < po->tile->tx1; po->x+=po->xstep, po->comp=po->comp_start) {
				for ( ; (po->comp < po->cur->cepoc) && (po->comp<po->tile->component_num); po->comp++, po->layer=po->layer_start) {
					tc = &po->tile->components[po->comp];
					if (po->res >= tc->reslevel_num) continue;
					reslevel = &tc->reslevels[po->res];
					if (reslevel->numprecinct == 0) continue;
					
					expx = reslevel->ppx + tc->decomposition_level_num - po->res;
					expy = reslevel->ppy + tc->decomposition_level_num - po->res;
					if (!(po->y%(tc->yrsiz<<expy)) || ((po->y==po->tile->ty0) && (reslevel->try0%(1<<reslevel->ppy)))) {
						if (!(po->x%(tc->xrsiz<<expx)) || ((po->x==po->tile->tx0) && (reslevel->trx0%(1<<reslevel->ppx)))) {
							tempx = tc->xrsiz << (tc->decomposition_level_num-po->res);
							tempx = (po->x+tempx-1) / tempx;
							tempx >>= reslevel->ppx;
							tempx -= reslevel->trx0>>reslevel->ppx;
							
							tempy = tc->yrsiz << (tc->decomposition_level_num-po->res);
							tempy = (po->y+tempy-1) / tempy;
							tempy >>= reslevel->ppy;
							tempy -= reslevel->try0>>reslevel->ppy;

							po->prct = tempx + reslevel->numprecinctswide*tempy;
							if (po->prct < reslevel->numprecinct) {
								for ( ; (po->layer<po->cur->lyepoc) && (po->layer<po->tile->layer_num); po->layer++) {
									if (po->layer >= reslevel->precincts[po->prct].layer) {
										reslevel->precincts[po->prct].layer = po->layer + 1;
										return jpx_true;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	return jpx_false;
}
jpx_bool					jpc_po_next_pcrl(jpc_pop po)
{
	jpx_byte expx, expy;
	jpc_tilecompp	tc;
	jpc_reslevelp	reslevel;
	jpx_uint32 tempx, tempy;
	
	for (; po->y < po->tile->ty1; po->y+=po->ystep, po->x=po->tile->tx0) {
		for ( ; po->x < po->tile->tx1; po->x+=po->xstep, po->comp=po->comp_start) {
			for (; (po->comp<po->cur->cepoc) && (po->comp<po->tile->component_num); po->comp++, po->res=po->res_start) {
				tc = &po->tile->components[po->comp];
				for (; (po->res<po->cur->repoc) && (po->res<po->tile->components[po->comp].reslevel_num); po->res++, po->layer=po->layer_start) {
					reslevel = &tc->reslevels[po->res];
					if (reslevel->numprecinct == 0) continue;
					
					expx = reslevel->ppx + tc->decomposition_level_num - po->res;
					expy = reslevel->ppy + tc->decomposition_level_num - po->res;
					if (!(po->y%(tc->yrsiz<<expy)) || ((po->y==po->tile->ty0) && (reslevel->try0%(1<<reslevel->ppy)))) {
						if (!(po->x%(tc->xrsiz<<expx)) || ((po->x==po->tile->tx0) && (reslevel->trx0%(1<<reslevel->ppx)))) {
							tempx = tc->xrsiz << (tc->decomposition_level_num-po->res);
							tempx = (po->x+tempx-1) / tempx;
							tempx >>= reslevel->ppx;
							tempx -= reslevel->trx0>>reslevel->ppx;
							
							tempy = tc->yrsiz << (tc->decomposition_level_num-po->res);
							tempy = (po->y+tempy-1) / tempy;
							tempy >>= reslevel->ppy;
							tempy -= reslevel->try0>>reslevel->ppy;

							po->prct = tempx + reslevel->numprecinctswide*tempy;
							if (po->prct < reslevel->numprecinct) {
								for ( ; (po->layer<po->cur->lyepoc) && (po->layer<po->tile->layer_num); po->layer++) {
									if (po->layer >= reslevel->precincts[po->prct].layer) {
										reslevel->precincts[po->prct].layer = po->layer + 1;
										return jpx_true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return jpx_false;	
}
jpx_bool					jpc_po_next_cprl(jpc_pop po)
{
	jpx_byte expx, expy;
	jpc_tilecompp	tc;
	jpc_reslevelp	reslevel;
	jpx_uint32 tempx, tempy;
	
	for ( ; (po->comp<po->cur->cepoc) && (po->comp<po->tile->component_num); po->comp++, po->y=po->tile->ty0) {
		for ( ; po->y < po->tile->ty1; po->y+=po->comp_ysteps[po->comp], po->x=po->tile->tx0) {
			for ( ; po->x < po->tile->tx1; po->x+=po->comp_xsteps[po->comp], po->res=po->res_start) {
				tc = &po->tile->components[po->comp];
				for ( ; (po->res<po->cur->repoc) && (po->res<tc->reslevel_num); po->res++, po->layer=po->layer_start) {
					reslevel = &tc->reslevels[po->res];
					if (reslevel->numprecinct == 0) continue;
					expx = reslevel->ppx + tc->decomposition_level_num - po->res;
					expy = reslevel->ppy + tc->decomposition_level_num - po->res;
					if (!(po->y%(tc->yrsiz<<expy)) || ((po->y==po->tile->ty0) && (reslevel->try0%(1<<reslevel->ppy)))) {
						if (!(po->x%(tc->xrsiz<<expx)) || ((po->x==po->tile->tx0) && (reslevel->trx0%(1<<reslevel->ppx)))) {
							tempx = tc->xrsiz << (tc->decomposition_level_num-po->res);
							tempx = (po->x+tempx-1) / tempx;
							tempx >>= reslevel->ppx;
							tempx -= reslevel->trx0>>reslevel->ppx;
							
							tempy = tc->yrsiz << (tc->decomposition_level_num-po->res);
							tempy = (po->y+tempy-1) / tempy;
							tempy >>= reslevel->ppy;
							tempy -= reslevel->try0>>reslevel->ppy;

							po->prct = tempx + reslevel->numprecinctswide*tempy;
							if (po->prct < reslevel->numprecinct) {
								for (po->layer=po->layer_start; (po->layer<po->cur->lyepoc) && (po->layer<po->tile->layer_num); po->layer++) {
									if (po->layer >= reslevel->precincts[po->prct].layer) {
										reslevel->precincts[po->prct].layer = po->layer + 1;
										return jpx_true;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return jpx_false;
}
jpx_bool					jpc_po_next(jpc_pop po)
{
	if (po->cur == NULL) return jpx_false;
	
	while (po->cur) {
		switch (po->cur->ppoc)
		{
		case JPC_PO_LRCP:
			if (jpc_po_next_lrcp(po)) {
				return jpx_true;
			}
			break;
		case JPC_PO_RLCP:
			if (jpc_po_next_rlcp(po)) {
				return jpx_true;
			}
			break;
		case JPC_PO_RPCL:
			if (jpc_po_next_rpcl(po)) {
				return jpx_true;
			}
			break;
		case JPC_PO_PCRL:
			if (jpc_po_next_pcrl(po)) {
				return jpx_true;
			}
			break;
		case JPC_PO_CPRL:
			if (jpc_po_next_cprl(po)) {
				return jpx_true;
			}
			break;
		default:
			break;
		}

		po->cur = po->cur->next;
		if (po->cur) {
			jpc_po_init(po);
		}
	}

	return jpx_false;
}

void					jpc_tile_init(jpc_decp jpcdec, jpx_uint32 tileIdx)
{
	jpc_tilep	tile;
	jpx_uint32 p, q;
	jpx_uint16	comp;

	tile = &jpcdec->tiles[tileIdx];
	p = tileIdx % jpcdec->xtilenum;
	q = tileIdx / jpcdec->xtilenum;
	tile->tx0 = jpcdec->siz.xtosiz+p*jpcdec->siz.xtsiz > jpcdec->siz.xosiz ? jpcdec->siz.xtosiz+p*jpcdec->siz.xtsiz : jpcdec->siz.xosiz;
	tile->ty0 = jpcdec->siz.ytosiz+q*jpcdec->siz.ytsiz > jpcdec->siz.yosiz ? jpcdec->siz.ytosiz+q*jpcdec->siz.ytsiz : jpcdec->siz.yosiz;
	tile->tx1 = jpcdec->siz.xtosiz+(p+1)*jpcdec->siz.xtsiz < jpcdec->siz.xsiz ? jpcdec->siz.xtosiz+(p+1)*jpcdec->siz.xtsiz : jpcdec->siz.xsiz;
	tile->ty1 = jpcdec->siz.ytosiz+(q+1)*jpcdec->siz.ytsiz < jpcdec->siz.ysiz ? jpcdec->siz.ytosiz+(q+1)*jpcdec->siz.ytsiz : jpcdec->siz.ysiz;
	tile->component_num = jpcdec->component_num;
	tile->max_reslevel_num = 0;
	
	tile->components = (jpc_tilecompp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, jpcdec->component_num*sizeof(jpc_tilecomp));
	jpx_memset(tile->components, 0, jpcdec->component_num*sizeof(jpc_tilecomp));
	for (comp = 0; comp < jpcdec->component_num; comp++) {
		jpc_tilecomp_init(jpcdec, tile, comp);
	}
}
void					jpc_tile_finalize(jpc_decp jpcdec, jpc_tilep tile)
{
	jpx_uint32 comp;

	for (comp = 0; comp < jpcdec->component_num; comp++) {
		jpc_tilecomp_finalize(jpcdec, &tile->components[comp]);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, tile->components);
	
	if (tile->po) {
		jpc_po_destroy(jpcdec, tile->po);
	}
}

jpc_decp				jpc_dec_create(jpx_memmgrp memmgr, jpx_streamp stream)
{
	jpc_decp jpcdec = (jpc_decp)memmgr->alloc_func(memmgr, sizeof(jpc_dec));
	jpx_memset(jpcdec, 0, sizeof(jpc_dec));

	jpcdec->memmgr = memmgr;
	jpcdec->bitstream = jpx_bitstream_create(memmgr, stream);
	jpcdec->external = jpx_false;
	
	jpcdec->output_ytile = 0;
	jpcdec->output_row_in_tile = 0;
	jpcdec->output_compnum = 0;
	jpcdec->output_pixgap = 0;

	return jpcdec;
}
jpc_decp				jpc_dec_create_with_bitstream(jpx_memmgrp memmgr, jpx_bitstreamp bitstream)
{
	jpc_decp jpcdec = (jpc_decp)memmgr->alloc_func(memmgr, sizeof(jpc_dec));
	jpx_memset(jpcdec, 0, sizeof(jpc_dec));

	jpcdec->memmgr = memmgr;
	jpcdec->bitstream = bitstream;
	jpcdec->external = jpx_true;

	jpcdec->output_ytile = 0;
	jpcdec->output_row_in_tile = 0;
	jpcdec->output_compnum = 0;
	jpcdec->output_pixgap = 0;

	return jpcdec;
}
void					jpc_dec_destroy(jpc_decp jpcdec)
{
	jpx_uint32 t;

	if (jpcdec->tiles) {
		for (t = 0; t < jpcdec->tilenum; t++) {
			jpc_tile_finalize(jpcdec, &jpcdec->tiles[t]);
		}
		jpcdec->memmgr->free_func(jpcdec->memmgr, jpcdec->tiles);
	}

	jpc_marker_siz_finalize(jpcdec, &jpcdec->siz);
	if (!jpcdec->external) {
		jpx_bitstream_destroy(jpcdec->memmgr, jpcdec->bitstream);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, jpcdec);
}
void					jpc_dec_set_error_func(jpc_decp jpcdec, jpx_error_funcp error_func)
{
	jpcdec->error_func = error_func;
}
void					jpc_dec_set_warn_func(jpc_decp jpcdec, jpx_warn_funcp warn_func)
{
	jpcdec->warn_func = warn_func;
}
void					jpc_endow_cox_and_rgn(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader)
{
	jpx_uint16	comp;
	jpx_uint32	r;
	jpc_tilep	tile;
	jpc_tilecompp	tc;
	jpc_marker_codp	cod;
	jpc_spcoxp	spcox;
	
	tile = &jpcdec->tiles[sot->isot];
	if (tileheader->cod) {
		cod = tileheader->cod;
	} else {
		cod = mainheader->cod;
	}
	
	tile->coding_style = cod->scod;
	tile->progression_order = cod->sgcod.progression_order;
	tile->layer_num = cod->sgcod.layer_num;
	tile->multiple_component = cod->sgcod.multiple_component;
	tile->transformation_cod = cod->spcod.transformation;
	
	for (comp = 0; comp < jpcdec->component_num; comp++) {
		tc = &tile->components[comp];
		
		// COC and COD parameters.
		if (tileheader->cocs[comp]) {
			spcox = &tileheader->cocs[comp]->spcoc;
		} else if (tileheader->cod) {
			spcox = &tileheader->cod->spcod;
		} else if (mainheader->cocs[comp]) {
			spcox = &mainheader->cocs[comp]->spcoc;
		} else {
			spcox = &mainheader->cod->spcod;
		}
		
		tc->decomposition_level_num = spcox->decomposition_level_num;
		tc->xcb = spcox->code_block_width + 2;
		tc->ycb = spcox->code_block_height + 2;
		tc->code_block_style = spcox->code_block_style;
		tc->transformation = spcox->transformation;
		
		if (jpcdec->capability == JPC_CAPABILITY_PROFILE0) {
			if (!(((tc->xcb==5) && (tc->ycb==5)) || ((tc->xcb==6) && (tc->ycb==6)))) {
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Violate profile0: xcb = ycb = 5 or xcb = ycb = 6");
				}
			}
			// TODO: profile0 for code-block style.
		} else if (jpcdec->capability == JPC_CAPABILITY_PROFILE1) {
			if (!((tc->xcb<=6) && (tc->ycb<=6))) {
				if (jpcdec->warn_func) {
					jpcdec->warn_func("Violate profile1: xcb <= 6,ycb <= 6");
				}
			}
		} else {
			if (!((tc->xcb>=2) && (tc->xcb<=10) && (tc->ycb>=2) && (tc->ycb<=10) && (tc->xcb+tc->ycb<=12))) {
				if (jpcdec->warn_func) {
					jpcdec->warn_func("The codestream width and height are limited to powers of two"
						"width the minimum size being 2~2 and the maximum being 2~10.Futher,the"
						"code-block size if restricted so that xcb + ycb <= 12.");
				}
			}
		}
		
		tc->reslevel_num = tc->decomposition_level_num + 1;
		if (tc->reslevel_num > tile->max_reslevel_num) {
			tile->max_reslevel_num = tc->reslevel_num;
		}
		tc->reslevels = (jpc_reslevelp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, tc->reslevel_num*sizeof(jpc_reslevel));
		jpx_memset(tc->reslevels, 0, tc->reslevel_num*sizeof(jpc_reslevel));
		
		if (spcox->precinct_sizes) {
			for (r = 0; r < tc->reslevel_num; r++) {
				tc->reslevels[r].ppx = spcox->precinct_sizes[r] & 0x0f;
				tc->reslevels[r].ppy = (spcox->precinct_sizes[r] >> 4) & 0x0f;

				jpc_reslevel_init(jpcdec, tc, r);
			}
		} else {
			for (r = 0; r < tc->reslevel_num; r++) {
				tc->reslevels[r].ppx = 15;
				tc->reslevels[r].ppy = 15;

				jpc_reslevel_init(jpcdec, tc, r);
			}
		}

		// RGN parameters.
		if (tileheader->rgns[comp]) {
			tc->roi_style = tileheader->rgns[comp]->srgn;
			tc->roi_shift = tileheader->rgns[comp]->sprgn;
		} else if (mainheader->rgns[comp]) {
			tc->roi_style = mainheader->rgns[comp]->srgn;
			tc->roi_shift = mainheader->rgns[comp]->sprgn;
		} else {
			tc->roi_style = 0;
			tc->roi_shift = 0;
		}
	}
}
void					jpc_endow_qcx(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader)
{
	jpx_uint16 comp, esb, mulb;
	jpx_uint32 r, bnd;
	jpc_tilep tile;
	jpc_tilecompp tc;
	jpc_reslevelp rl;
	jpc_subbandp sb;
	jpx_uint16p quantization_steps;
	jpx_uint16 quantization_step;
	jpx_int32 gain;
	jpx_byte guardbits;

	tile = &jpcdec->tiles[sot->isot];

	for (comp = 0; comp < jpcdec->component_num; comp++) {
		tc = &tile->components[comp];
		
		// QCC and QCD parameters.
		if (tileheader->qccs[comp]) {
			tc->quantization_style = tileheader->qccs[comp]->sqcc;
			quantization_steps = tileheader->qccs[comp]->spqccs;
		} else if (tileheader->qcd) {
			tc->quantization_style = tileheader->qcd->sqcd;
			quantization_steps = tileheader->qcd->spqcds;
		} else if (mainheader->qccs[comp]) {
			tc->quantization_style = mainheader->qccs[comp]->sqcc;
			quantization_steps = mainheader->qccs[comp]->spqccs;
		} else {
			tc->quantization_style = mainheader->qcd->sqcd;
			quantization_steps = mainheader->qcd->spqcds;
		}
		
		for (r = 0; r < tc->reslevel_num; r++) {
			rl = &tc->reslevels[r];
			for (bnd = 0; bnd < rl->subband_num; bnd++) {
				sb = &rl->subbands[bnd];
				
				guardbits = (tc->quantization_style >> 5) & 0x07;
				if ((tc->quantization_style & 0x1f) == 0) {
					quantization_step = quantization_steps[r == 0 ? 0 : 3*r-2+bnd];
					esb = (quantization_step>>3) & 0x1f;
					sb->mb = guardbits + esb - 1;
					sb->delta = 1;
				} else if ((tc->quantization_style&0x1f) == 1) {
					quantization_step = quantization_steps[0];
					mulb = quantization_step & 0x7ff;
					esb = ((quantization_step >> 11) & 0x1f) - r;
					gain = r == 0 ? 0 : bnd == 0 ? 1 : bnd == 1 ? 1 : 2;

					sb->delta = JPX_FIX_ONE | (mulb << (JPX_FIX_POINT - 11));
					sb->shift = JPX_FIX_POINT + tc->precision + gain - esb;
					sb->mb = guardbits + esb - 1;
				} else {
					quantization_step = quantization_steps[r == 0 ? 0 : 3*r - 2 + bnd];
					mulb = quantization_step & 0x7ff;
					esb = (quantization_step >> 11) & 0x1f;
					gain = r == 0 ? 0 : bnd == 0 ? 1 : bnd == 1 ? 1 : 2;
					
					sb->delta = JPX_FIX_ONE | (mulb << (JPX_FIX_POINT - 11));
					sb->shift = JPX_FIX_POINT + tc->precision + gain - esb;
					sb->mb = guardbits + esb - 1;
				}
			}
		}
	}
}
void					jpc_endow_poc(jpc_decp jpcdec, jpc_marker_sotp sot, jpc_headerp mainheader, jpc_headerp tileheader)
{
	jpc_tilep tile = &jpcdec->tiles[sot->isot];
	
	if (tile->po) {
		if (tileheader->poclist) {
			jpc_po_add_list(tile->po, jpcdec, tileheader->poclist);
		}
	} else {
		tile->po = jpc_po_create(jpcdec, tile);
		if (tileheader->poclist) {
			jpc_po_add_list(tile->po, jpcdec, tileheader->poclist);
		} else if (mainheader->poclist) {
			jpc_po_add_list(tile->po, jpcdec, mainheader->poclist);
		} else if (tileheader->cod) {
			jpc_po_changep pochange = jpc_po_change_create(jpcdec);
			pochange->rspoc = 0;
			pochange->cspoc = 0;
			pochange->repoc = tile->max_reslevel_num;
			pochange->cepoc = tile->component_num;
			pochange->lyepoc = tile->layer_num;
			pochange->ppoc = tileheader->cod->sgcod.progression_order;
			jpc_po_add(tile->po, pochange);
		} else {
			jpc_po_changep pochange = jpc_po_change_create(jpcdec);
			pochange->rspoc = 0;
			pochange->cspoc = 0;
			pochange->repoc = tile->max_reslevel_num;
			pochange->cepoc = tile->component_num;
			pochange->lyepoc = tile->layer_num;
			pochange->ppoc = mainheader->cod->sgcod.progression_order;
			jpc_po_add(tile->po, pochange);
		}
	}
}
jpx_bool				jpc_parse_tile_data(jpc_decp jpcdec, jpc_marker_sotp sot)
{
	jpc_tilep tile;
	jpc_tilecompp tilecomp;
	jpc_reslevelp reslevel;
	jpc_precinctp precinct;
	jpc_precinct_bandp band;
	jpc_codeblockp cb;
	jpx_bool bit;
	jpx_uint32 bits, xcb, ycb, i, n, sb;
	jpx_uint32 offset;
	jpx_bitstuffp bitstuff;
	jpc_pop po;
	jpx_uint16 packet_squence;
	jpc_marker_header markerheader;
	jpc_segmentp seg;
	jpx_uint32 passtotal, passnum, passno, maxpass;
	jpx_uint16 marker;

	tile = &jpcdec->tiles[sot->isot];
	po = tile->po;
	
	while (1) {
		if (sot->psot == 0) {
			if (!jpx_bitstream_getshort(jpcdec->bitstream, &marker) || marker==JPC_MARKER_EOC) {
				break;
			}
		} else if (sot->offset >= sot->psot) {
			break;
		}
		
		if (!jpc_po_next(po)) {
			break;
		}
		
		tilecomp = &tile->components[po->comp];
		reslevel = &tilecomp->reslevels[po->res];
		precinct = &reslevel->precincts[po->prct];

		// Check SOP marker.
		if (tile->coding_style & 0x02) {
			if(jpx_bitstream_getshort(jpcdec->bitstream, &marker) && marker==JPC_MARKER_SOP) {
				if (!jpc_parse_marker_header(jpcdec, &markerheader)) {
					return jpx_false;
				}
				if (!jpc_parse_marker_sop(jpcdec, &packet_squence)) {
					return jpx_false;
				}
				sot->offset += markerheader.len + 2;
			}
		}

		offset = jpx_bitstream_getpos(jpcdec->bitstream);
		bitstuff = jpx_bitstuff_create(jpcdec->memmgr, jpcdec->bitstream);
		
		// Parsing packet header
		// Zero-length flag
		bit = jpx_bitstuff_readbit(bitstuff);
		if (!bit) {
			// Packet is empty -- clear all code-block inclusion flags
			for (sb = 0; sb < reslevel->subband_num; ++sb) {
				band = &precinct->bands[sb];
				for (ycb = 0; ycb < band->vcodeblock_num; ++ycb) {
					for (xcb = 0; xcb < band->hcodeblock_num; ++xcb) {
						cb = &band->cbs[ycb*band->hcodeblock_num+xcb];
						cb->included = jpx_false;
					}
				}
			}
		} else {
			for (sb = 0; sb < reslevel->subband_num; ++sb) {
				band = &precinct->bands[sb];
				for (ycb = 0; ycb < band->vcodeblock_num; ++ycb) {
					for (xcb = 0; xcb < band->hcodeblock_num; ++xcb) {
						cb = &band->cbs[ycb*band->hcodeblock_num+xcb];

						// Skip code-blocks with no coefficients
						if ((cb->x0>=cb->x1) || (cb->y0>=cb->y1)) {
							cb->included = jpx_false;
							continue;
						}

						// Code-block inclusion
						if (cb->seen) {
							cb->included = jpx_bitstuff_readbit(bitstuff);
						} else {
							cb->included = jpx_tagtree_decode(band->inctt, bitstuff, xcb, ycb, po->layer+1)==po->layer;
						}
						
						if (cb->included) {
							// Zero bit-plane count
							if (!cb->seen) {
								cb->p = jpx_tagtree_decode(band->zbptt, bitstuff, xcb, ycb, 0xffffffff);
							}
							
							// Total coding passes
							if (!jpx_bitstuff_readbit(bitstuff)) {
								passtotal = 1;
							} else {
								if (!jpx_bitstuff_readbit(bitstuff)) {
									passtotal = 2;
								} else {
									bits = jpx_bitstuff_readbits(bitstuff, 2);
									if (bits < 3) {
										passtotal = 3 + bits;
									} else {
										bits = jpx_bitstuff_readbits(bitstuff, 5);
										if (bits < 31) {
											passtotal = 6 + bits;
										} else {
											passtotal = 37 + jpx_bitstuff_readbits(bitstuff, 7);
										}
									}
								}
							}
							
							// Update Lblock
							while (jpx_bitstuff_readbit(bitstuff)) cb->lblock++;
							
							seg = cb->seglist->cur;
							while (passtotal > 0) {
								passno = cb->passnum;
								
								// Computing pass number
								if (tilecomp->code_block_style & JPC_CBSTYLE_RESTART) {
									maxpass = 1;
								} else if (tilecomp->code_block_style & JPC_CBSTYLE_SELECTIVE) {
									if (passno < 10) maxpass = 10 - passno;
									else {
										switch (passno % 3)
										{
										case 0:
											maxpass = 1;
											break;
										case 1:
											maxpass = 2;
											break;
										case 2:
											maxpass = 1;
											break;
										}
									}
								} else {
									maxpass = 0xffffffff;
								}
								passnum = maxpass > passtotal ? passtotal : maxpass;
								
								if (!seg) {
									seg = jpc_segment_create(jpcdec);
									seg->startpass = passno;
									seg->passtotal = maxpass;
									if (tilecomp->code_block_style & JPC_CBSTYLE_SELECTIVE) {
										if (passno < 10) {
											seg->type = JPC_SEGMENT_ARITH;
										} else if((passno%3) == 0) {
											seg->type = JPC_SEGMENT_ARITH;
										} else {
											seg->type = JPC_SEGMENT_RAW;
										}
									} else {
										seg->type = JPC_SEGMENT_ARITH;
									}
									if (!cb->seglist->header) {
										cb->seglist->header = seg;
										cb->seglist->tail = seg;
										cb->seglist->cur = seg;
									} else {
										cb->seglist->tail->next = seg;
										cb->seglist->tail = seg;
									}
								}
								
								for (n=cb->lblock, i=passnum>>1; i; ++n,i>>=1) ;
								seg->len = jpx_bitstuff_readbits(bitstuff, n);

								seg->passnum += passnum;
								seg = seg->next;

								cb->passnum += passnum;
								passtotal -= passnum;
							}
#ifdef _JPX_ENABLE_PARSE_LOG_
 							if(sot->isot == 0) {
 								JPX_LOG("zero bitplanes=%d numpasses=%d datalength=%d\n",cb->p,passnum,cb->seglist->cur->len);	
  							}
#endif
						}
					}
				}
			}
		}
		jpx_bitstuff_alignbyte(bitstuff);
		jpx_bitstuff_destroy(jpcdec->memmgr, bitstuff);

		sot->offset += jpx_bitstream_getpos(jpcdec->bitstream)-offset;

		// Check EPH marker
		if (tile->coding_style & 0x04) {
			if (jpx_bitstream_getshort(jpcdec->bitstream, &marker) && marker==JPC_MARKER_EPH) {
				jpx_bitstream_setbitpos(jpcdec->bitstream, jpx_bitstream_getpos(jpcdec->bitstream)+2);
				sot->offset += 2;
			}
		}
		
		// Parsing packet data
		for (sb = 0; sb < reslevel->subband_num; ++sb) {
			band = &precinct->bands[sb];
			for (ycb = 0; ycb < band->vcodeblock_num; ++ycb) {
				for (xcb = 0; xcb < band->hcodeblock_num; ++xcb) {
					cb = &band->cbs[ycb*band->hcodeblock_num+xcb];
					if (cb->included) {
						seg = cb->seglist->cur;

#ifdef _JPX_ENABLE_PARSE_LOG_
						JPX_LOG("lyrno=%02d, compno=%02d, lvlno=%02d, prcno=%02d, bandno=%02d, cblkno=%02d," 
							" numpasses=%02d zb=%02d len=%02d\n", tile->po->layer, tile->po->comp, tile->po->res, tile->po->prct, 
							sb, ycb*band->hcodeblock_num+xcb, seg->passnum, cb->p, seg->len);
#endif

						while (seg) {
							if (seg->buf) {
								seg->buf = (jpx_bytep)jpcdec->memmgr->realloc_func(jpcdec->memmgr, seg->buf,
									seg->lentotal+seg->len);
							} else {
								seg->buf = (jpx_bytep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, seg->len);
							}
							if (!jpx_bitstream_readbuf(jpcdec->bitstream, seg->buf+seg->lentotal, seg->len)) {
								if (jpcdec->error_func) {
									jpcdec->error_func("Parsing parcket data error.");
								}
								return jpx_false;
							}
							seg->lentotal += seg->len;
							cb->seen = jpx_true;

							sot->offset += seg->len;
							
							if (seg->passnum >= seg->passtotal) {
								cb->seglist->cur = seg->next;
							}
							seg = seg->next;
						}
					}
				}
			}
		}
	}

	return jpx_true;
}
jpx_bool				jpc_dec_parse(jpc_decp jpcdec)
{
	jpc_headerp mainheader;
	jpc_headerp tileheader;
	jpc_marker_sot sot;
	jpc_marker_header markerheader;

	mainheader = jpc_parse_main_header(jpcdec);
	if (mainheader == NULL) {
		return jpx_false;
	}
	
	do {
		if (!jpc_parse_marker_sot(jpcdec, &sot)) {
			jpc_header_destroy(jpcdec, mainheader);
			return jpx_false;
		}
		
		if (sot.tpsot == 0) {
			tileheader = jpc_parse_tile_header0(jpcdec, &sot);
			if (tileheader == NULL) {
				jpc_header_destroy(jpcdec, mainheader);
				return jpx_false;
			}
			jpc_tile_init(jpcdec, sot.isot);
			jpc_endow_cox_and_rgn(jpcdec, &sot, mainheader, tileheader);
			jpc_endow_qcx(jpcdec, &sot, mainheader, tileheader);
		} else {
			tileheader = jpc_parse_tile_header1(jpcdec, &sot);
			if (tileheader == NULL) {
				jpc_header_destroy(jpcdec, mainheader);
				return jpx_false;
			}
		}
		jpc_endow_poc(jpcdec, &sot, mainheader, tileheader);

		if (!jpc_parse_tile_data(jpcdec, &sot)) {
			jpc_header_destroy(jpcdec, tileheader);
			jpc_header_destroy(jpcdec, mainheader);
			return jpx_false;
		}
		jpc_header_destroy(jpcdec, tileheader);

		if (!jpc_parse_marker_header(jpcdec, &markerheader)) {
			jpc_header_destroy(jpcdec, mainheader);
			return jpx_false;
		}
	} while(markerheader.code == JPC_MARKER_SOT);
	
	if (markerheader.code != JPC_MARKER_EOC) {
		if (jpcdec->warn_func) {
			jpcdec->warn_func("Missing EOC marker");
		}
	}

	jpc_header_destroy(jpcdec, mainheader);
	return jpx_true;
}
jpx_uint16				jpc_dec_get_componentnum(jpc_decp jpcdec)
{
	return jpcdec->component_num;
}
void					jpc_dec_get_imagesize(jpc_decp jpcdec, jpx_uint32p width, jpx_uint32p height)
{
	*width = jpcdec->siz.xsiz - jpcdec->siz.xosiz;
	*height = jpcdec->siz.ysiz - jpcdec->siz.yosiz;
}
void					jpc_dec_init(jpc_decp jpcdec, jpx_uint16 compnum, jpx_int32p offsets, jpx_int32 pixgap)
{
	jpx_uint16 comp;
	
	jpcdec->output_pixgap = pixgap;
	jpcdec->output_ytile = 0;
	jpcdec->output_row_in_tile = 0;

	jpcdec->output_compnum = compnum>jpcdec->component_num ? jpcdec->component_num:compnum;
	if (jpcdec->component_num > JPX_MAX_COMPONENT) jpcdec->component_num = JPX_MAX_COMPONENT;
	for (comp = 0; comp < jpcdec->output_compnum; comp++) {
		jpcdec->output_offsets[comp] = offsets ? offsets[comp] : comp;
		jpcdec->comp_raw_lines[comp].buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, (jpcdec->siz.xsiz-jpcdec->siz.xosiz)*sizeof(jpx_int32));
	}
}
jpx_bool				jpc_dec_scanline(jpc_decp jpcdec, jpx_bytep line)
{
	jpx_uint32 xtile, x, imgx;
	jpx_uint16 comp;
	jpc_tilep tile;
	jpc_tilecompp tc;
	jpx_int32 y0, y1, y2;
	jpx_int32 coeff, minval, maxval, zeroval;
	jpx_coeff_line comp_raw_lines[JPX_MAX_COMPONENT];
	
	tile = &jpcdec->tiles[jpcdec->output_ytile*jpcdec->xtilenum];
	if (jpcdec->output_row_in_tile >= tile->ty1-tile->ty0) {
		if (jpcdec->output_ytile >= jpcdec->ytilenum) {
			return jpx_false;
		} else {
			jpcdec->output_ytile++;
			jpcdec->output_row_in_tile = 0;
			tile += jpcdec->xtilenum;
		}
	}
	
	imgx = 0;
	for (xtile=0; xtile<jpcdec->xtilenum; xtile++,tile++) {
		// Offset the raw component lines
		for (comp=0; comp<jpcdec->output_compnum; comp++) {
			comp_raw_lines[comp].buf32 = jpcdec->comp_raw_lines[comp].buf32+imgx;
		}

		// Decode one tile line for all components
		for (comp=0; comp < jpcdec->output_compnum; comp++) {
			tc = &tile->components[comp];
			jpc_decode_resolution_line(jpcdec, tc, tc->decomposition_level_num, comp_raw_lines[comp]);
		}
		
		// Inverse multiple component transformation.
		tc = tile->components;
		if (tile->multiple_component && (jpcdec->component_num>=3) && (tc->xrsiz==(tc+1)->xrsiz) && (tc->xrsiz==(tc+2)->xrsiz)
			&& (tc->yrsiz==(tc+1)->yrsiz) && (tc->yrsiz==(tc+2)->yrsiz)) {
			if (tile->transformation_cod) {
				for (x=0; x < tc->tcx1-tc->tcx0; x++) {
					y0 = comp_raw_lines[0].buf32[x];
					y1 = comp_raw_lines[1].buf32[x];
					y2 = comp_raw_lines[2].buf32[x];
					
					comp_raw_lines[1].buf32[x] = y0 - (((y2+y1)>>4)<<2);
					comp_raw_lines[0].buf32[x] = y2 + comp_raw_lines[1].buf32[x];
					comp_raw_lines[2].buf32[x] = y1 + comp_raw_lines[1].buf32[x];							
				}
			} else {
				for (x=0; x < tc->tcx1-tc->tcx0; x++) {
					y0 = comp_raw_lines[0].buf32[x];
					y1 = comp_raw_lines[1].buf32[x];
					y2 = comp_raw_lines[2].buf32[x];
					
					comp_raw_lines[0].buf32[x] = y0 + JPX_FIXMUL(JPX_DBLTOFIX(1.402), y2);
					comp_raw_lines[1].buf32[x] = y0 - JPX_FIXMUL(JPX_DBLTOFIX(0.34413), y1) - 
						JPX_FIXMUL(JPX_DBLTOFIX(0.71414), y2);
					comp_raw_lines[2].buf32[x] = y0 + JPX_FIXMUL(JPX_DBLTOFIX(1.772), y1);
				}
			}
		}
		
		for (comp=0; comp < jpcdec->component_num; comp++) {
			tc = &tile->components[comp];
			if (tc->sgned) {
				minval = - (1<<(tc->precision-1));
				maxval = (1<<(tc->precision-1)) - 1;
				
				for (x=0; x < tc->tcx1-tc->tcx0; x++) {
					coeff = comp_raw_lines[comp].buf32[x];
					if ((tc->quantization_style&0x1f) != 0) {
						coeff = JPX_FIXTOINT(JPX_FIX_ROUND(coeff));
					} else {
						coeff >>= 2;
					}
					if (coeff < minval) {
						coeff = minval;
					} else if (coeff > maxval) {
						coeff = maxval;
					}
					comp_raw_lines[comp].buf32[x] = coeff;
				}
				// Unsigned: inverse DC level shift and clip
			} else {
				maxval = (1<<tc->precision) - 1;
				zeroval = 1 << (tc->precision-1);
				
				for (x=0; x < tc->tcx1 - tc->tcx0; x++) {
					coeff = comp_raw_lines[comp].buf32[x];
					if ((tc->quantization_style&0x1f) != 0) {
						coeff = JPX_FIXTOINT(JPX_FIX_ROUND(coeff));
					} else {
						coeff >>= 2;
					}
					coeff += zeroval;
					if (coeff < 0) {
						coeff = 0;
					} else {
						if (coeff > maxval) {
							coeff = maxval;
						}
					}
					comp_raw_lines[comp].buf32[x] = coeff;
				}
			}
		}
		
		// Transfer data.
		for (comp=0; comp < jpcdec->output_compnum; comp++) {
			tc = &tile->components[comp];
			jpcdec->comp_lines[comp] = line + imgx*jpcdec->output_pixgap + jpcdec->output_offsets[comp];
			for (x=0; x < tc->tcx1-tc->tcx0; x++) {
				*jpcdec->comp_lines[comp] = (jpx_byte)comp_raw_lines[comp].buf32[x];
				jpcdec->comp_lines[comp] += jpcdec->output_pixgap;
			}
		}

		imgx += tile->tx1-tile->tx0;
	}

	jpcdec->output_row_in_tile++;
	return jpx_true;
}
void					jpc_dec_finalize(jpc_decp jpcdec)
{
	jpx_uint16 comp;

	for (comp=0; comp < jpcdec->output_compnum; comp++) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, jpcdec->comp_raw_lines[comp].buf32);
	}
}
jpx_bool				jpc_dec_decode(jpc_decp jpcdec, jpx_bytep out, jpx_uint16 compnum, jpx_int32p offsets, jpx_int32 pixgap, jpx_int32 linebytes)
{
	jpx_uint32 tx, ty, comp, imgx, imgy, x, y;
	jpc_tilep tile;
	jpc_tilecompp tc;
	jpx_bytepp bufs;
	jpx_coeff_linep complines;
	jpx_int32 y0, y1, y2;
	jpx_int32 coeff, minval, maxval, zeroval;

	if (compnum > jpcdec->component_num) {
		compnum = jpcdec->component_num;
	}

	bufs = (jpx_bytepp)jpcdec->memmgr->alloc_func(jpcdec->memmgr, compnum * sizeof(jpx_bytep));
	complines = (jpx_coeff_linep)jpcdec->memmgr->alloc_func(jpcdec->memmgr, compnum * sizeof(jpx_coeff_line));
	for(comp = 0; comp < compnum; comp++) {
		complines[comp].buf32 = (jpx_int32p)jpcdec->memmgr->alloc_func(jpcdec->memmgr, (jpcdec->siz.xsiz-jpcdec->siz.xosiz)*sizeof(jpx_int32));
	}

	imgy = 0;
	for (ty = 0; ty < jpcdec->ytilenum; ty++) {
		imgx = 0;
		for (tx = 0; tx < jpcdec->xtilenum; tx++) {
			tile = &jpcdec->tiles[ty*jpcdec->xtilenum+tx];
			for (y = 0; y < tile->ty1 - tile->ty0; y++) {
				for (comp = 0; comp < compnum; comp ++) {
					tc = &tile->components[comp];
					jpc_decode_resolution_line(jpcdec, tc, tc->decomposition_level_num, complines[comp]);
				}

				// Inverse multiple component transformation.
				tc = tile->components;
				if (tile->multiple_component && (jpcdec->component_num>=3) && (tc->xrsiz==(tc+1)->xrsiz) && (tc->xrsiz==(tc+2)->xrsiz)
					&& (tc->yrsiz==(tc+1)->yrsiz) && (tc->yrsiz==(tc+2)->yrsiz)) {
					if (tile->transformation_cod) {
						for (x=0; x < tc->tcx1-tc->tcx0; x++) {
							y0 = complines[0].buf32[x];
							y1 = complines[1].buf32[x];
							y2 = complines[2].buf32[x];
							
							complines[1].buf32[x] = y0 - (((y2+y1)>>4)<<2);
							complines[0].buf32[x] = y2 + complines[1].buf32[x];
							complines[2].buf32[x] = y1 + complines[1].buf32[x];							
						}
					} else {
						for (x=0; x < tc->tcx1-tc->tcx0; x++) {
							y0 = complines[0].buf32[x];
							y1 = complines[1].buf32[x];
							y2 = complines[2].buf32[x];
							
							complines[0].buf32[x] = y0 + JPX_FIXMUL(JPX_DBLTOFIX(1.402), y2);
							complines[1].buf32[x] = y0 - JPX_FIXMUL(JPX_DBLTOFIX(0.34413), y1) - 
								JPX_FIXMUL(JPX_DBLTOFIX(0.71414), y2);
							complines[2].buf32[x] = y0 + JPX_FIXMUL(JPX_DBLTOFIX(1.772), y1);
						}
					}
				}
				
				for (comp=0; comp < jpcdec->component_num; comp++) {
					tc = &tile->components[comp];
					if (tc->sgned) {
						minval = - (1<<(tc->precision-1));
						maxval = (1<<(tc->precision-1)) - 1;
						
						for (x=0; x < tc->tcx1-tc->tcx0; x++) {
							coeff = complines[comp].buf32[x];
							if ((tc->quantization_style&0x1f) != 0) {
								coeff = JPX_FIXTOINT(JPX_FIX_ROUND(coeff));
							} else {
								coeff >>= 2;
							}
							if (coeff < minval) {
								coeff = minval;
							} else if (coeff > maxval) {
								coeff = maxval;
							}
							complines[comp].buf32[x] = coeff;
						}
						// Unsigned: inverse DC level shift and clip
					} else {
						maxval = (1<<tc->precision) - 1;
						zeroval = 1 << (tc->precision-1);
						
						for (x=0; x < tc->tcx1 - tc->tcx0; x++) {
							coeff = complines[comp].buf32[x];
							if ((tc->quantization_style&0x1f) != 0) {
								coeff = JPX_FIXTOINT(JPX_FIX_ROUND(coeff));
							} else {
								coeff >>= 2;
							}
							coeff += zeroval;
							if (coeff < 0) {
								coeff = 0;
							} else {
								if (coeff > maxval) {
									coeff = maxval;
								}
							}
							complines[comp].buf32[x] = coeff;
						}
					}
				}
			
				// Transfer data.
				for (comp = 0; comp < compnum; comp++) {
					tc = &tile->components[comp];
					bufs[comp] = out + (imgy+y)*linebytes + imgx*pixgap + offsets[comp];
					for (x = 0; x < tc->tcx1 - tc->tcx0; x++) {
						*bufs[comp] = complines[comp].buf32[x];
						bufs[comp] += pixgap;
					}
				}
			}

			imgx += tile->tx1 - tile->tx0;
		}
		imgy += jpcdec->tiles[ty*jpcdec->xtilenum].ty1 - jpcdec->tiles[ty*jpcdec->xtilenum].ty0;
	}

	jpcdec->memmgr->free_func(jpcdec->memmgr, bufs);
	for(comp = 0; comp < compnum; comp++) {
		jpcdec->memmgr->free_func(jpcdec->memmgr, complines[comp].buf32);
	}
	jpcdec->memmgr->free_func(jpcdec->memmgr, complines);

	return jpx_true;
}
