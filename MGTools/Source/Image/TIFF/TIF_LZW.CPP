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
//		Originally based on implementation by Sam Leffler
//		Copyright (c) 1988, 1989, 1990, 1991, 1992 Sam Leffler
//		Copyright (c) 1991, 1992 Silicon Graphics, Inc.
//		
//  Description:	STUBBED tif_lzw.cpp
//		auxillary support routines
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//


/*
 * TIFF Library.
 * Rev 5.0 Lempel-Ziv & Welch Compression Support
 *
 * This code is derived from the compress program whose code is
 * derived from software contributed to Berkeley by James A. Woods,
 * derived from original work by Spencer Thomas and Joseph Orost.
 *
 * The original Berkeley copyright notice appears below in its entirety.
 */
#include "..\..\..\stdafx.h"


#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#include <assert.h>

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

#define TRACE_TIFF	TRACE(_T("TIFF/LZW decoding module not enabled!  Contact Stingray Software for details on unlocking the GIF module\n"));



static	void cl_hash(LZWEncodeState FAR*);


int
SECTiff::LZWCheckPredictor(TIFF* tif,
    LZWState FAR* sp, predictorFunc pred8bit, predictorFunc pred16bit)
{
	tif; sp; pred8bit; pred16bit;
	TRACE_TIFF
	    
	    cl_hash(NULL); //MSW

	return (0);
}


static void
horAcc8(register LPBYTE cp, register DWORD cc, register long stride)
{
	cp; cc; stride;
}



static void LZWSwabArrayOfShort(LPWORD wp, register DWORD n)
{
	wp; n;
}




static void
swabHorAcc16(LPBYTE cp, DWORD cc, register long stride)
{
	cp; cc; stride;
}

static void
horAcc16(LPBYTE cp, DWORD cc, register long stride)
{
	cp; cc; stride;
}

/*
 * Setup state for decoding a strip.
 */
int
SECTiff::LZWPreDecode(TIFF* tif)
{
	tif;
	TRACE_TIFF
	return (0);
}

void
SECTiff::codeLoop(TIFF* tif)
{
	tif;
}

long
SECTiff::LZWDecode(TIFF* tif, tidata_t op0, tsize_t occ0, tsample_t s)
{
	tif; op0; occ0; s;
	TRACE_TIFF
	return (0);
}


long
SECTiff::LZWDecodeCompat(TIFF* tif, tidata_t op0, tsize_t occ0, tsample_t s)
{
	tif; op0; occ0; s;
	TRACE_TIFF
	return (0);
}

/*
 * Decode a scanline and apply the predictor routine.
 */
long
SECTiff::LZWDecodePredRow(TIFF* tif, tidata_t op0, tsize_t occ0, tsample_t s)
{
	tif; op0; occ0; s;
	TRACE_TIFF
	return 0;
}


long
SECTiff::LZWDecodePredTile(TIFF* tif, tidata_t op0, tsize_t occ0, tsample_t s)
{
	tif; op0; occ0; s;
	TRACE_TIFF
	return (0);
}

/*
 * LZW Encoding.
 */

static void
horDiff8(register LPBYTE cp, register DWORD cc, register long stride)
{
	cp; cc; stride;
}

static void
horDiff16(LPBYTE cp, DWORD cc, register long stride)
{
	cp; cc; stride;	
}

/*
 * Reset encoding state at the start of a strip.
 */
int
SECTiff::LZWPreEncode(TIFF* tif)
{
	tif;
	TRACE_TIFF
	return 0;
}


int
SECTiff::LZWEncode(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	tif; bp; cc; s;
	TRACE_TIFF
	return (0);
}

long
SECTiff::LZWEncodePredRow(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	tif; bp; cc; s;
	TRACE_TIFF
	return(0);
}

long
SECTiff::LZWEncodePredTile(TIFF* tif, tidata_t bp0, tsize_t cc0, tsample_t s)
{
	tif; bp0; cc0; s;
	TRACE_TIFF
	return (0);
}


int
SECTiff::LZWPostEncode(TIFF* tif)
{
	tif;
	TRACE_TIFF
	return (0);
}

/*
 * Reset encoding hash table.
 */
static void
cl_hash(LZWEncodeState FAR* sp)
{
	sp;	
	//MSW
	horAcc8(NULL,NULL,NULL);
	LZWSwabArrayOfShort(NULL,NULL);
	swabHorAcc16(NULL,NULL,NULL);
	horAcc16(NULL,NULL,NULL);
	horDiff8(NULL,NULL,NULL);
	horDiff16(NULL,NULL,NULL);
}

void
SECTiff::LZWCleanup(TIFF* tif)
{
	tif;
	
}

long
SECTiff::TIFFInitLZW(TIFF* tif)
{
	tif;
	TRACE_TIFF
	return (0);
}

