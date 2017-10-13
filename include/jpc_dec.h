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

#ifndef _JPC_DEC_H_
#define _JPC_DEC_H_

//--------------------------------------------------------------------------------
//			JPEG2000 codestream markers and marker segments
//--------------------------------------------------------------------------------
#define JPC_MARKER_SOC		0xFF4F	// Start of codestream
#define JPC_MARKER_SOT		0xFF90	// Start of tile-part
#define JPC_MARKER_SOD		0xFF93	// Start of data
#define JPC_MARKER_EOC		0xFFD9	// End of codestream
#define JPC_MARKER_SIZ		0xFF51	// Image and tile size
#define JPC_MARKER_COD		0xFF52	// Coding style default
#define JPC_MARKER_COC		0xFF53	// Coding style component
#define JPC_MARKER_RGN		0xFF5E	// Region-of-interest
#define JPC_MARKER_QCD		0xFF5C	// Quantization default
#define JPC_MARKER_QCC		0xFF5D	// Quantization component
#define JPC_MARKER_POC		0xFF5F	// Progression order change
#define JPC_MARKER_TLM		0xFF55	// Tile-part lengths
#define JPC_MARKER_PLM		0xFF57	// Packet length, main header
#define JPC_MARKER_PLT		0xFF58	// Packet length, tile-part header
#define JPC_MARKER_PPM		0xFF60	// Packed packet headers, main header
#define JPC_MARKER_PPT		0xFF61	// Packed packet headers, tile-part header
#define JPC_MARKER_SOP		0xFF91	// Start of packet
#define JPC_MARKER_EPH		0xFF92	// End of packet header
#define JPC_MARKER_CRG		0xFF63	// Component registration
#define JPC_MARKER_COM		0xFF64	// Comment

#define JPC_PO_LRCP						0	// Layer-resolution level-component-position progression
#define JPC_PO_RLCP						1	// Resolution level-layer-component-position progression
#define JPC_PO_RPCL						2	// Resolution level-position-component-layer progression
#define JPC_PO_PCRL						3	// Position-component-resolution level-layer progression
#define JPC_PO_CPRL						4	// Component-position-resolution level-layer progression

#define JPC_CAPABILITY_STANDARD			0
#define JPC_CAPABILITY_PROFILE0			1
#define JPC_CAPABILITY_PROFILE1			2

#define JPC_SEGMENT_RAW					0
#define JPC_SEGMENT_ARITH				1

#define JPC_SIG_PASS					0
#define JPC_REF_PASS					1
#define JPC_CLN_PASS					2

#define JPC_CBSTYLE_SELECTIVE			0x01	// Selective arithmetic coding bypass
#define JPC_CBSTYLE_RESET				0x02	// Reset context probabilities on coding pass boundaries
#define JPC_CBSTYLE_RESTART				0x04	// Termination on each coding pass
#define JPC_CBSTYLE_CAUSAL				0x08	// Vertically causal context
#define JPC_CBSTYLE_PREDICT				0x10	// Predictable termination
#define JPC_CBSTYLE_SYMBOL				0x20	// Segmentation symbols are used

#define JPC_MAX_CONTEXTS				19

typedef struct jpc_dec_struct FAR * jpc_decp;
typedef struct jpc_marker_poc_struct FAR * jpc_marker_pocp;
typedef struct jpc_segment_struct FAR * jpc_segmentp;
typedef struct jpc_po_change_struct FAR * jpc_po_changep;
typedef struct jpc_po_struct FAR * jpc_pop;
typedef struct jpc_reslevel_struct FAR * jpc_reslevelp;
typedef struct jpc_tilecomp_struct FAR * jpc_tilecompp;
typedef struct jpc_tile_struct FAR * jpc_tilep;
typedef struct jpc_linebuf_struct FAR * jpc_linebufp;

// Codestream marker segment header
typedef struct jpc_marker_header_struct {
	jpx_uint16				code;	// Marker code
	jpx_uint16				len;	// Marker segment length
} jpc_marker_header;
typedef jpc_marker_header FAR * jpc_marker_headerp;

// Start of tile-part
typedef struct jpc_marker_sot_struct {
	jpx_uint16				isot;	// Tile index
	jpx_uint32				psot;	// Length from the beginning of the SOT marker segment to the end of the data of that tile-part.
	jpx_byte				tpsot;	// Tile-part index
	jpx_byte				tnsot;	// Number of tile-parts of a tile in the codestream.
	jpx_uint32				offset;
} jpc_marker_sot;
typedef jpc_marker_sot FAR * jpc_marker_sotp;

// Image and tile size
typedef struct jpc_marker_siz_struct {
	jpx_uint32				xsiz;	// Width of the reference grid.
	jpx_uint32				ysiz;	// Height of the reference grid.
	jpx_uint32				xosiz;	// Horizontal offset from the origin of the reference grid to the left side of the image area.
	jpx_uint32				yosiz;	// Vertical offset from the origin of the reference grid to the top side of the image area.
	jpx_uint32				xtsiz;	// Width of one reference tile with respect to the reference grid.
	jpx_uint32				ytsiz;	// Height of one reference tile with respect to the reference grid.
	jpx_uint32				xtosiz;	// Horizontal offset from the origin of the reference grid to the left side of the first tile.
	jpx_uint32				ytosiz;	// Vertical offset from the origin of the reference grid to the top side of the first tile.
	jpx_uint16				csiz;	// Number of components in the image.
	jpx_bytep				ssizs;	// Precision (depth) in bits and sign of the ith component samples.
	jpx_bytep				xrsizs;	// Horizontal separation of a sample of ith component with respect to the reference grid.
	jpx_bytep				yrsizs;	// Vertical separation of a sample of ith component with respect to the reference grid.
} jpc_marker_siz;
typedef jpc_marker_siz FAR * jpc_marker_sizp;

void						jpc_marker_siz_init(jpc_decp jpcdec, jpc_marker_sizp siz);
void						jpc_marker_siz_finalize(jpc_decp jpcdec, jpc_marker_sizp siz);

typedef struct jpc_spcox_struct {
	jpx_byte			decomposition_level_num;	// Number of decomposition levels, NL, Zero implies no transformation
	jpx_byte			code_block_width;			// Code-block width
	jpx_byte			code_block_height;			// Code-block height
	jpx_byte			code_block_style;			// Code-block style
	jpx_byte			transformation;				// Wavelet transformation used
	jpx_bytep			precinct_sizes;				// Precinct size
} jpc_spcox;
typedef jpc_spcox FAR * jpc_spcoxp;

// Coding style default
typedef struct jpc_marker_cod_struct {
	jpx_byte				scod;	// Coding style for all components
	struct {
		jpx_byte			progression_order;	// Progression order
		jpx_uint16			layer_num;			// Number of layers
		jpx_byte			multiple_component;	// Multiple component transformation	
	} sgcod;						// Parameters for coding style designated in Scod.
	jpc_spcox				spcod;	// Parameters for coding style designated in Scod.
} jpc_marker_cod;
typedef jpc_marker_cod FAR * jpc_marker_codp;

jpc_marker_codp				jpc_marker_cod_create(jpc_decp jpcdec);
void						jpc_marker_cod_destroy(jpc_decp jpcdec, jpc_marker_codp cod);

// Coding style component
typedef struct jpc_marker_coc_struct {
	jpx_uint16				ccoc;	// The index of the component to which this marker segment relates
	jpx_byte				scoc;
	jpc_spcox				spcoc;
} jpc_marker_coc;
typedef jpc_marker_coc FAR * jpc_marker_cocp;
typedef jpc_marker_coc FAR * FAR * jpc_marker_cocpp;

jpc_marker_cocp				jpc_marker_coc_create(jpc_decp jpcdec);
void						jpc_marker_coc_destroy(jpc_decp jpcdec, jpc_marker_cocp coc);

// Region of interest
typedef struct jpc_marker_rgn_struct {
	jpx_uint16				crgn;	// The index of the component to which this marker segment relates.
	jpx_byte				srgn;	// ROI style for the current ROI.
	jpx_byte				sprgn;	// Parameter for ROI style designated in Srgn.
} jpc_marker_rgn;
typedef jpc_marker_rgn FAR * jpc_marker_rgnp;
typedef jpc_marker_rgn FAR * FAR * jpc_marker_rgnpp;

jpc_marker_rgnp				jpc_marker_rgn_create(jpc_decp jpcdec);
void						jpc_marker_rgn_destroy(jpc_decp jpcdec, jpc_marker_rgnp rgn);

// Quantization default
typedef struct jpc_marker_qcd_struct {
	jpx_byte				sqcd;	// Quantization style for all components
	jpx_uint16p				spqcds;	// Quantization step size value for the ith sub-band in the defined order
	jpx_uint16				spqcdnum;
} jpc_marker_qcd;
typedef jpc_marker_qcd FAR * jpc_marker_qcdp;

jpc_marker_qcdp				jpc_marker_qcd_create(jpc_decp jpcdec);
void						jpc_marker_qcd_destroy(jpc_decp jpcdec, jpc_marker_qcdp qcd);

// Quantization component
typedef struct jpc_marker_qcc_struct {
	jpx_uint16				cqcc;	// The index of the component to which this marker segment relates
	jpx_byte				sqcc;	// Quantization style for this component.
	jpx_uint16p				spqccs;	// Quantization value for each sub-band in the defined order
	jpx_uint16				spqccnum;
} jpc_marker_qcc;
typedef jpc_marker_qcc FAR * jpc_marker_qccp;
typedef jpc_marker_qcc FAR * FAR * jpc_marker_qccpp;

jpc_marker_qccp				jpc_marker_qcc_create(jpc_decp jpcdec);
void						jpc_marker_qcc_destroy(jpc_decp jpcdec, jpc_marker_qccp qcc);

// Progression order change
typedef struct jpc_marker_poc_struct {
	jpx_bytep				rspocs;	// Resolution level index (inclusive) for the start of a progression
	jpx_uint16p				cspocs;	// Component index (inclusive) for the start of a progression
	jpx_uint16p				lyepocs;// Layer index (exclusive) for the end of a progression.
	jpx_bytep				repocs;	// Resolution Level index (exclusive) for the end of a progression
	jpx_uint16p				cepocs;	// Component index (exclusive) for the end of a progression
	jpx_bytep				ppocs;	// Progression order
	jpx_uint16				povnum;
	jpc_marker_pocp			next;
} jpc_marker_poc;

jpc_marker_pocp				jpc_marker_poc_create(jpc_decp jpcdec);
void						jpc_marker_poc_destroy(jpc_decp jpcdec, jpc_marker_pocp poc);

// Main header / Tile-part header
typedef struct jpc_header_struct {
	jpx_uint16				componentnum;
	jpc_marker_codp			cod;
	jpc_marker_cocpp		cocs;
	jpc_marker_qcdp			qcd;
	jpc_marker_qccpp		qccs;
	jpc_marker_rgnpp		rgns;
	jpc_marker_pocp			poclist;
} jpc_header;
typedef jpc_header FAR * jpc_headerp;

jpc_headerp					jpc_header_create(jpc_decp jpcdec, jpx_uint16 componentnum);
void						jpc_header_destroy(jpc_decp jpcdec, jpc_headerp header);


// Coefficient state
typedef struct jpc_coeffstate_struct {
	jpx_uint32				sign		: 1;	// Sign flag bits.
	jpx_uint32				significant	: 1;	// Significant state. just become significant in the immediately preceding significance propagation pass.
	jpx_uint32				refined		: 1;	// Refined for this coefficient.
	jpx_uint32				handled		: 1;	// Handled.
	jpx_uint32				len;				// Number of MSBs.
} jpc_coeffstate;
typedef jpc_coeffstate FAR * jpc_coeffstatep;

// Coefficient line buffer
typedef struct jpc_linebuf_struct {
	jpx_coeff_line			buf;
	jpc_linebufp			prev;
	jpc_linebufp			next;
} jpc_linebuf;

// Coefficient line buffer loop list
typedef struct jpc_linebufs_struct {
	jpc_linebufp			lines;		// Lines
	jpx_uint32				size;		// Count of lines
	jpc_linebufp			cur;		// Current
} jpc_linebufs;
typedef jpc_linebufs FAR * jpc_linebufsp;

void					jpc_linebufs_init(jpc_linebufsp linebufs, jpc_decp jpcdec, jpx_uint32 size);
void					jpc_linebufs_finalize(jpc_linebufsp linebufs, jpc_decp jpcdec);

// Segment
typedef struct jpc_segment_struct {
	jpx_int32				type;
	jpx_uint32				len;
	jpx_uint32				lentotal;
	jpx_bytep				buf;
	jpx_uint32				startpass;
	jpx_uint32				passnum;
	jpx_uint32				passtotal;
	jpc_segmentp			next;
} jpc_segment;

jpc_segmentp				jpc_segment_create(jpc_decp jpcdec);
void						jpc_segment_destroy(jpc_decp jpcdec, jpc_segmentp segment);

// Segment list
typedef struct jpc_segment_list_struct {
	jpc_segmentp			header;
	jpc_segmentp			tail;
	jpc_segmentp			cur;
} jpc_segment_list;
typedef jpc_segment_list FAR * jpc_segment_listp;

jpc_segment_listp			jpc_segment_list_create(jpc_decp jpcdec);
void						jpc_segment_list_destroy(jpc_decp jpcdec, jpc_segment_listp seglist);

// Codeblock
typedef struct jpc_codeblock_struct {
	jpx_uint32				x0;
	jpx_uint32				y0;
	jpx_uint32				x1;
	jpx_uint32				y1;
	jpx_uint32				p;	// Zero bit-plane
	jpx_uint32				lblock;
	jpx_uint32				passtype;
	jpx_uint32				passnum;
	jpx_bool				included;
	jpx_bool				seen;
	jpx_arith_ctxp			contexts;
	jpc_segment_listp		seglist;
} jpc_codeblock;
typedef jpc_codeblock FAR * jpc_codeblockp;
typedef jpc_codeblock FAR * FAR * jpc_codeblockpp;

// Precinct band
typedef struct jpc_precinct_band_struct {
	jpx_uint32				x0;
	jpx_uint32				y0;
	jpx_uint32				x1;
	jpx_uint32				y1;

	jpx_uint32				hcodeblock_num;
	jpx_uint32				vcodeblock_num;
	jpc_codeblockp			cbs;
	jpx_tagtreep			inctt;
	jpx_tagtreep			zbptt;
} jpc_precinct_band;
typedef jpc_precinct_band FAR * jpc_precinct_bandp;

// Precinct
typedef struct jpc_precinct_struct {
	jpx_uint32				x0;
	jpx_uint32				y0;
	jpx_uint32				x1;
	jpx_uint32				y1;
	jpc_precinct_bandp		bands;
	jpx_uint16				layer;
} jpc_precinct;
typedef jpc_precinct FAR * jpc_precinctp;

// Sub-band
typedef struct jpc_subband_struct {
	jpx_uint32				tbx0;
	jpx_uint32				tby0;
	jpx_uint32				tbx1;
	jpx_uint32				tby1;
	jpx_byte				mb;
	jpx_int32				delta;
	jpx_int32				shift;

	jpx_uint32				hcodeblock_num;
	jpx_uint32				vcodeblock_num;
	jpc_codeblockpp			refs;
	jpx_int32p				data;
	jpx_uint32				data_height;
	jpx_uint32				data_width;
	jpx_uint32				cby;
	jpx_uint32				y;
	jpx_uint32				current_decoded_height;
	jpx_uint32				used_height;
} jpc_subband;
typedef jpc_subband FAR * jpc_subbandp;

// Resolution level
typedef struct jpc_reslevel_struct {
	jpc_tilecompp			tc;
	jpx_uint32				r;

	jpx_uint32				subband_num;
	jpx_uint32				trx0;
	jpx_uint32				try0;
	jpx_uint32				trx1;
	jpx_uint32				try1;
	jpx_uint32				llx0;
	jpx_uint32				lly0;
	jpx_uint32				llx1;
	jpx_uint32				lly1;
	jpc_subband				subbands[3];
	jpc_precinctp			precincts;
	jpx_uint32				numprecinctswide;
	jpx_uint32				numprecinctshigh;
	jpx_uint32				numprecinct;
	
	jpx_byte				ppx;
	jpx_byte				ppy;
	jpc_linebufs			lines;
	jpc_linebufs			lows;
	jpc_linebufs			highs;
	jpc_linebuf				templine;
	jpc_linebuf				templow;
	jpc_linebuf				temphigh;
	jpx_uint32				low_width;
	jpx_uint32				high_width;
	jpx_uint32				y;
} jpc_reslevel;

void						jpc_reslevel_init(jpc_decp jpcdec, jpc_tilecompp tc, jpx_uint32 r);
void						jpc_reslevel_finalize(jpc_decp jpcdec, jpc_reslevelp reslevel);

// Tile component
typedef struct jpc_tilecomp_struct {
	jpc_tilep				tile;

	jpx_uint32				tcx0;
	jpx_uint32				tcy0;
	jpx_uint32				tcx1;
	jpx_uint32				tcy1;
	jpx_uint32				reslevel_num;
	jpc_reslevelp			reslevels; // NL+1 levels.
	
	//  SIZ marker segment
	jpx_bool				sgned;
	jpx_byte				precision;
	jpx_byte				xrsiz;
	jpx_byte				yrsiz;
	//  COD/COC marker segment
	jpx_byte				decomposition_level_num;
	jpx_byte				xcb;
	jpx_byte				ycb;
	jpx_byte				code_block_style;
	jpx_byte				transformation;
	//	QCD/QCC marker segment
	jpx_byte				quantization_style;
	//	RGN marker segment
	jpx_byte				roi_style;
	jpx_byte				roi_shift;
} jpc_tilecomp;

void						jpc_tilecomp_init(jpc_decp jpcdec, jpc_tilep tile, jpx_uint16 compIdx);
void						jpc_tilecomp_finalize(jpc_decp jpcdec, jpc_tilecompp tc);

// Progression order change
typedef struct jpc_po_change_struct {
	jpx_byte				rspoc;
	jpx_byte				repoc;
	jpx_uint16				cspoc;
	jpx_uint16				cepoc;
	jpx_uint16				lyepoc;
	jpx_byte				ppoc;
	jpc_po_changep			next;
} jpc_po_change;

jpc_po_changep				jpc_po_change_create(jpc_decp jpcdec);
void						jpc_po_change_destroy(jpc_decp jpcdec, jpc_po_changep pochange);

// Progression order
typedef struct jpc_po_struct {
	jpc_tilep				tile;

	jpc_po_changep			header;
	jpc_po_changep			tail;
	jpc_po_changep			cur;
	jpx_uint16				layer;
	jpx_uint16				comp;
	jpx_byte				res;
	jpx_uint32				prct;
	jpx_uint32				x;
	jpx_uint32				y;
	
	jpx_uint16				layer_start;
	jpx_uint16				comp_start;
	jpx_byte				res_start;

	jpx_uint32				xstep;
	jpx_uint32				ystep;
	jpx_uint32p				comp_xsteps;
	jpx_uint32p				comp_ysteps;
	jpx_bool				normal_xrsiz_yrsiz;
} jpc_po;

jpc_pop						jpc_po_create(jpc_decp jpcdec, jpc_tilep tile);
void						jpc_po_destroy(jpc_decp jpcdec, jpc_pop po);
void						jpc_po_add(jpc_pop po, jpc_po_changep pochange);
void						jpc_po_add_list(jpc_pop po, jpc_decp jpcdec, jpc_marker_pocp poclist);
jpx_bool					jpc_po_next(jpc_pop po);

// Tile
typedef struct jpc_tile_struct {
	jpx_uint32				tx0;
	jpx_uint32				ty0;
	jpx_uint32				tx1;
	jpx_uint32				ty1;
	jpc_tilecompp			components;	// Components.

	jpx_byte				coding_style;
	jpx_byte				progression_order;	// Progression order.
	jpx_uint16				layer_num;		// Number of layers.
	jpx_byte				multiple_component;	// Multi-component
	jpx_byte				transformation_cod;
	
	// For progressive decoding. 
	jpx_byte				max_reslevel_num;
	jpx_uint16				component_num;
	jpc_pop					po;
} jpc_tile;

void						jpc_tile_init(jpc_decp jpcdec, jpx_uint32 tileIdx);
void						jpc_tile_finalize(jpc_decp jpcdec, jpc_tilep tile);

//--------------------------------------------------------------------------------
//			JPEG2000 codestream decoder
//--------------------------------------------------------------------------------
typedef struct jpc_dec_struct {
	jpx_memmgrp				memmgr;
	jpx_error_funcp			error_func;
	jpx_warn_funcp			warn_func;

	jpx_bitstreamp			bitstream;
	jpx_bool				external;
	
	jpc_marker_siz			siz;			// Image and tile size parameter values.
	jpx_uint16				capability;		// Capability.
	jpx_uint16				component_num;
	jpx_uint32				tilenum;
	jpx_uint32				xtilenum;
	jpx_uint32				ytilenum;
	jpc_tilep				tiles;
	
	jpx_uint32				max_layer_num;
	jpx_uint32				max_resolution_num;

	jpx_uint32				output_ytile;
	jpx_uint32				output_row_in_tile;
	jpx_uint16				output_compnum;
	jpx_uint32				output_offsets[JPX_MAX_COMPONENT];
	jpx_uint32				output_pixgap;
	
	jpx_bytep				comp_lines[JPX_MAX_COMPONENT];
	jpx_coeff_line			comp_raw_lines[JPX_MAX_COMPONENT];
} jpc_dec;

jpc_decp				jpc_dec_create(jpx_memmgrp memmgr, jpx_streamp stream);
jpc_decp				jpc_dec_create_with_bitstream(jpx_memmgrp memmgr, jpx_bitstreamp bitstream);
void					jpc_dec_destroy(jpc_decp jpcdec);
void					jpc_dec_set_error_func(jpc_decp jpcdec, jpx_error_funcp error_func);
void					jpc_dec_set_warn_func(jpc_decp jpcdec, jpx_warn_funcp warn_func);
jpx_bool				jpc_dec_parse(jpc_decp jpcdec);
jpx_uint16				jpc_dec_get_componentnum(jpc_decp jpcdec);
void					jpc_dec_get_imagesize(jpc_decp jpcdec, jpx_uint32p width, jpx_uint32p height);
jpx_bool				jpc_dec_decode(jpc_decp jpcdec, jpx_bytep out, jpx_uint16 compnum, jpx_int32p offsets, jpx_int32 pixgap, jpx_int32 linebytes);
void					jpc_dec_init(jpc_decp jpcdec, jpx_uint16 compnum, jpx_int32p offsets, jpx_int32 pixgap);
jpx_bool				jpc_dec_scanline(jpc_decp jpcdec, jpx_bytep line);
void					jpc_dec_finalize(jpc_decp jpcdec);

#endif // _JPC_DEC_H_
