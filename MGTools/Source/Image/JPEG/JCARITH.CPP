//
// Stingray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All Rights Reserved
//
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// See the SEC help files for detailed information
// regarding using SEC classes.
//
//  Author:		AAB
//  Description:	jcarith.cpp
//		This file contains arithmetic entropy encoding routines.
//		These routines are invoked via the methods entropy_encode,
//		entropy_encode_init/term, and entropy_optimize.
//
//  CHANGELOG:
//	7/15/95	AAB	Initial implementation
//
//	This code based originally on the Independent JPEG Group's software,
//	Copyright (C) 1991, 1992, Thomas G. Lane.


#include "..\..\..\stdafx.h"

#ifndef __SECJPEG_H__
#include "..\..\..\Include\Image\secjpeg.h"
#endif

#ifdef C_ARITH_CODING_SUPPORTED


/*
 * The arithmetic coding option of the JPEG standard specifies Q-coding,
 * which is covered by patents held by IBM (and possibly AT&T and Mitsubishi).
 * At this time it does not appear to be legal for the Independent JPEG
 * Group to distribute software that implements arithmetic coding.
 * We have therefore removed arithmetic coding support from the
 * distributed source code.
 *
 * We're not happy about it either.
 */


/*
 * The method selection routine for arithmetic entropy encoding.
 */

// GLOBAL void
SECJpeg::jselcarithmetic (compress_info_ptr cinfo)
{
  if (cinfo->arith_code) {
    SEC_ERREXIT(cinfo->emethods, "Sorry, there are legal restrictions on arithmetic coding");
  }
}

#endif /* C_ARITH_CODING_SUPPORTED */
