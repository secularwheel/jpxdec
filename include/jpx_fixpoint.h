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

#ifndef _JPX_FIXPOINT_H_
#define _JPX_FIXPOINT_H_

typedef jpx_int32			jpx_fix;

#define JPX_FIX_POINT		16
#define JPX_FIX_ONE			0x10000
#define JPX_FIX_HALF		0x8000

#define JPX_INTTOFIX(x)		((x) << JPX_FIX_POINT)
#define JPX_FIXTOINT(x)		((x) >> JPX_FIX_POINT)

#define JPX_DBLTOFIX(x)		((jpx_fix)((x) * JPX_FIX_ONE))
#define JPX_FIXTODBL(x)		(((double)(x)) / JPX_FIX_ONE)

#define JPX_FIXMUL(x,y)		((jpx_fix)(((jpx_int64)(x)*(jpx_int64)(y)) >> JPX_FIX_POINT))
#define JPX_FIXDIV(x,y)		((jpx_fix)((((jpx_int64)(x)) << JPX_FIX_POINT)/(y)))

#define	JPX_FIX_FLOOR(x)		((x) & (~(JPX_FIX_ONE-1)))
#define	JPX_FIX_ROUND(x)		(((x) > 0) ? JPX_FIX_FLOOR((x)+JPX_FIX_HALF) : JPX_FIX_FLOOR((x)-JPX_FIX_HALF))

#endif // _JPX_FIXPOINT_H_
