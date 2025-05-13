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
//  Description:	tif_fax3.cpp
//		CCITT Group 3 and Group 4 Compression support
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//



#include "..\..\..\stdafx.h"


//AAB #ifdef WIN32

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __TIF_FAX3_H__
#include "..\..\..\Include\Image\tif_fax3.h" 
#endif

#ifndef __T4_H__
#include "..\..\..\Include\Image\t4.h" //AAB NEW
#endif

//#ifdef OLDCODE //SCOT, this isn't getting in here?
//#define	G3CODES
//#include "t4.h"
//#endif //OLDCODE

#ifndef NEWCODE

#define	EOL	0x001	/* EOL code value - 0000 0000 0000 1 */

/* status values returned instead of a run length */
#define	G3CODE_INVALID	-1
#define	G3CODE_INCOMP	-2
#define	G3CODE_EOL	-3
#define	G3CODE_EOF	-4

/*
 * Note that these tables are ordered such that the
 * index into the table is known to be either the
 * run length, or (run length / 64) + a fixed offset.
 *
 * NB: The G3CODE_INVALID entries are only used
 *     during state generation (see mkg3states.c).
 */
//#ifdef G3CODES
//AAB const tableentry TIFFFaxWhiteCodes[] = {
tableentry TIFFFaxWhiteCodes[ ] = {
    { 8, 0x35, 0 },	/* 0011 0101 */
    { 6, 0x7, 1 },	/* 0001 11 */
    { 4, 0x7, 2 },	/* 0111 */
    { 4, 0x8, 3 },	/* 1000 */
    { 4, 0xB, 4 },	/* 1011 */
    { 4, 0xC, 5 },	/* 1100 */
    { 4, 0xE, 6 },	/* 1110 */
    { 4, 0xF, 7 },	/* 1111 */
    { 5, 0x13, 8 },	/* 1001 1 */
    { 5, 0x14, 9 },	/* 1010 0 */
    { 5, 0x7, 10 },	/* 0011 1 */
    { 5, 0x8, 11 },	/* 0100 0 */
    { 6, 0x8, 12 },	/* 0010 00 */
    { 6, 0x3, 13 },	/* 0000 11 */
    { 6, 0x34, 14 },	/* 1101 00 */
    { 6, 0x35, 15 },	/* 1101 01 */
    { 6, 0x2A, 16 },	/* 1010 10 */
    { 6, 0x2B, 17 },	/* 1010 11 */
    { 7, 0x27, 18 },	/* 0100 111 */
    { 7, 0xC, 19 },	/* 0001 100 */
    { 7, 0x8, 20 },	/* 0001 000 */
    { 7, 0x17, 21 },	/* 0010 111 */
    { 7, 0x3, 22 },	/* 0000 011 */
    { 7, 0x4, 23 },	/* 0000 100 */
    { 7, 0x28, 24 },	/* 0101 000 */
    { 7, 0x2B, 25 },	/* 0101 011 */
    { 7, 0x13, 26 },	/* 0010 011 */
    { 7, 0x24, 27 },	/* 0100 100 */
    { 7, 0x18, 28 },	/* 0011 000 */
    { 8, 0x2, 29 },	/* 0000 0010 */
    { 8, 0x3, 30 },	/* 0000 0011 */
    { 8, 0x1A, 31 },	/* 0001 1010 */
    { 8, 0x1B, 32 },	/* 0001 1011 */
    { 8, 0x12, 33 },	/* 0001 0010 */
    { 8, 0x13, 34 },	/* 0001 0011 */
    { 8, 0x14, 35 },	/* 0001 0100 */
    { 8, 0x15, 36 },	/* 0001 0101 */
    { 8, 0x16, 37 },	/* 0001 0110 */
    { 8, 0x17, 38 },	/* 0001 0111 */
    { 8, 0x28, 39 },	/* 0010 1000 */
    { 8, 0x29, 40 },	/* 0010 1001 */
    { 8, 0x2A, 41 },	/* 0010 1010 */
    { 8, 0x2B, 42 },	/* 0010 1011 */
    { 8, 0x2C, 43 },	/* 0010 1100 */
    { 8, 0x2D, 44 },	/* 0010 1101 */
    { 8, 0x4, 45 },	/* 0000 0100 */
    { 8, 0x5, 46 },	/* 0000 0101 */
    { 8, 0xA, 47 },	/* 0000 1010 */
    { 8, 0xB, 48 },	/* 0000 1011 */
    { 8, 0x52, 49 },	/* 0101 0010 */
    { 8, 0x53, 50 },	/* 0101 0011 */
    { 8, 0x54, 51 },	/* 0101 0100 */
    { 8, 0x55, 52 },	/* 0101 0101 */
    { 8, 0x24, 53 },	/* 0010 0100 */
    { 8, 0x25, 54 },	/* 0010 0101 */
    { 8, 0x58, 55 },	/* 0101 1000 */
    { 8, 0x59, 56 },	/* 0101 1001 */
    { 8, 0x5A, 57 },	/* 0101 1010 */
    { 8, 0x5B, 58 },	/* 0101 1011 */
    { 8, 0x4A, 59 },	/* 0100 1010 */
    { 8, 0x4B, 60 },	/* 0100 1011 */
    { 8, 0x32, 61 },	/* 0011 0010 */
    { 8, 0x33, 62 },	/* 0011 0011 */
    { 8, 0x34, 63 },	/* 0011 0100 */
    { 5, 0x1B, 64 },	/* 1101 1 */
    { 5, 0x12, 128 },	/* 1001 0 */
    { 6, 0x17, 192 },	/* 0101 11 */
    { 7, 0x37, 256 },	/* 0110 111 */
    { 8, 0x36, 320 },	/* 0011 0110 */
    { 8, 0x37, 384 },	/* 0011 0111 */
    { 8, 0x64, 448 },	/* 0110 0100 */
    { 8, 0x65, 512 },	/* 0110 0101 */
    { 8, 0x68, 576 },	/* 0110 1000 */
    { 8, 0x67, 640 },	/* 0110 0111 */
    { 9, 0xCC, 704 },	/* 0110 0110 0 */
    { 9, 0xCD, 768 },	/* 0110 0110 1 */
    { 9, 0xD2, 832 },	/* 0110 1001 0 */
    { 9, 0xD3, 896 },	/* 0110 1001 1 */
    { 9, 0xD4, 960 },	/* 0110 1010 0 */
    { 9, 0xD5, 1024 },	/* 0110 1010 1 */
    { 9, 0xD6, 1088 },	/* 0110 1011 0 */
    { 9, 0xD7, 1152 },	/* 0110 1011 1 */
    { 9, 0xD8, 1216 },	/* 0110 1100 0 */
    { 9, 0xD9, 1280 },	/* 0110 1100 1 */
    { 9, 0xDA, 1344 },	/* 0110 1101 0 */
    { 9, 0xDB, 1408 },	/* 0110 1101 1 */
    { 9, 0x98, 1472 },	/* 0100 1100 0 */
    { 9, 0x99, 1536 },	/* 0100 1100 1 */
    { 9, 0x9A, 1600 },	/* 0100 1101 0 */
    { 6, 0x18, 1664 },	/* 0110 00 */
    { 9, 0x9B, 1728 },	/* 0100 1101 1 */
    { 11, 0x8, 1792 },	/* 0000 0001 000 */
    { 11, 0xC, 1856 },	/* 0000 0001 100 */
    { 11, 0xD, 1920 },	/* 0000 0001 101 */
    { 12, 0x12, 1984 },	/* 0000 0001 0010 */
    { 12, 0x13, 2048 },	/* 0000 0001 0011 */
    { 12, 0x14, 2112 },	/* 0000 0001 0100 */
    { 12, 0x15, 2176 },	/* 0000 0001 0101 */
    { 12, 0x16, 2240 },	/* 0000 0001 0110 */
    { 12, 0x17, 2304 },	/* 0000 0001 0111 */
    { 12, 0x1C, 2368 },	/* 0000 0001 1100 */
    { 12, 0x1D, 2432 },	/* 0000 0001 1101 */
    { 12, 0x1E, 2496 },	/* 0000 0001 1110 */
    { 12, 0x1F, 2560 },	/* 0000 0001 1111 */
    { 12, 0x1, G3CODE_EOL },	/* 0000 0000 0001 */
    { 9, 0x1, G3CODE_INVALID },	/* 0000 0000 1 */
    { 10, 0x1, G3CODE_INVALID },	/* 0000 0000 01 */
    { 11, 0x1, G3CODE_INVALID },	/* 0000 0000 001 */
    { 12, 0x0, G3CODE_INVALID },	/* 0000 0000 0000 */
};

const tableentry TIFFFaxBlackCodes[] = {
    { 10, 0x37, 0 },	/* 0000 1101 11 */
    { 3, 0x2, 1 },	/* 010 */
    { 2, 0x3, 2 },	/* 11 */
    { 2, 0x2, 3 },	/* 10 */
    { 3, 0x3, 4 },	/* 011 */
    { 4, 0x3, 5 },	/* 0011 */
    { 4, 0x2, 6 },	/* 0010 */
    { 5, 0x3, 7 },	/* 0001 1 */
    { 6, 0x5, 8 },	/* 0001 01 */
    { 6, 0x4, 9 },	/* 0001 00 */
    { 7, 0x4, 10 },	/* 0000 100 */
    { 7, 0x5, 11 },	/* 0000 101 */
    { 7, 0x7, 12 },	/* 0000 111 */
    { 8, 0x4, 13 },	/* 0000 0100 */
    { 8, 0x7, 14 },	/* 0000 0111 */
    { 9, 0x18, 15 },	/* 0000 1100 0 */
    { 10, 0x17, 16 },	/* 0000 0101 11 */
    { 10, 0x18, 17 },	/* 0000 0110 00 */
    { 10, 0x8, 18 },	/* 0000 0010 00 */
    { 11, 0x67, 19 },	/* 0000 1100 111 */
    { 11, 0x68, 20 },	/* 0000 1101 000 */
    { 11, 0x6C, 21 },	/* 0000 1101 100 */
    { 11, 0x37, 22 },	/* 0000 0110 111 */
    { 11, 0x28, 23 },	/* 0000 0101 000 */
    { 11, 0x17, 24 },	/* 0000 0010 111 */
    { 11, 0x18, 25 },	/* 0000 0011 000 */
    { 12, 0xCA, 26 },	/* 0000 1100 1010 */
    { 12, 0xCB, 27 },	/* 0000 1100 1011 */
    { 12, 0xCC, 28 },	/* 0000 1100 1100 */
    { 12, 0xCD, 29 },	/* 0000 1100 1101 */
    { 12, 0x68, 30 },	/* 0000 0110 1000 */
    { 12, 0x69, 31 },	/* 0000 0110 1001 */
    { 12, 0x6A, 32 },	/* 0000 0110 1010 */
    { 12, 0x6B, 33 },	/* 0000 0110 1011 */
    { 12, 0xD2, 34 },	/* 0000 1101 0010 */
    { 12, 0xD3, 35 },	/* 0000 1101 0011 */
    { 12, 0xD4, 36 },	/* 0000 1101 0100 */
    { 12, 0xD5, 37 },	/* 0000 1101 0101 */
    { 12, 0xD6, 38 },	/* 0000 1101 0110 */
    { 12, 0xD7, 39 },	/* 0000 1101 0111 */
    { 12, 0x6C, 40 },	/* 0000 0110 1100 */
    { 12, 0x6D, 41 },	/* 0000 0110 1101 */
    { 12, 0xDA, 42 },	/* 0000 1101 1010 */
    { 12, 0xDB, 43 },	/* 0000 1101 1011 */
    { 12, 0x54, 44 },	/* 0000 0101 0100 */
    { 12, 0x55, 45 },	/* 0000 0101 0101 */
    { 12, 0x56, 46 },	/* 0000 0101 0110 */
    { 12, 0x57, 47 },	/* 0000 0101 0111 */
    { 12, 0x64, 48 },	/* 0000 0110 0100 */
    { 12, 0x65, 49 },	/* 0000 0110 0101 */
    { 12, 0x52, 50 },	/* 0000 0101 0010 */
    { 12, 0x53, 51 },	/* 0000 0101 0011 */
    { 12, 0x24, 52 },	/* 0000 0010 0100 */
    { 12, 0x37, 53 },	/* 0000 0011 0111 */
    { 12, 0x38, 54 },	/* 0000 0011 1000 */
    { 12, 0x27, 55 },	/* 0000 0010 0111 */
    { 12, 0x28, 56 },	/* 0000 0010 1000 */
    { 12, 0x58, 57 },	/* 0000 0101 1000 */
    { 12, 0x59, 58 },	/* 0000 0101 1001 */
    { 12, 0x2B, 59 },	/* 0000 0010 1011 */
    { 12, 0x2C, 60 },	/* 0000 0010 1100 */
    { 12, 0x5A, 61 },	/* 0000 0101 1010 */
    { 12, 0x66, 62 },	/* 0000 0110 0110 */
    { 12, 0x67, 63 },	/* 0000 0110 0111 */
    { 10, 0xF, 64 },	/* 0000 0011 11 */
    { 12, 0xC8, 128 },	/* 0000 1100 1000 */
    { 12, 0xC9, 192 },	/* 0000 1100 1001 */
    { 12, 0x5B, 256 },	/* 0000 0101 1011 */
    { 12, 0x33, 320 },	/* 0000 0011 0011 */
    { 12, 0x34, 384 },	/* 0000 0011 0100 */
    { 12, 0x35, 448 },	/* 0000 0011 0101 */
    { 13, 0x6C, 512 },	/* 0000 0011 0110 0 */
    { 13, 0x6D, 576 },	/* 0000 0011 0110 1 */
    { 13, 0x4A, 640 },	/* 0000 0010 0101 0 */
    { 13, 0x4B, 704 },	/* 0000 0010 0101 1 */
    { 13, 0x4C, 768 },	/* 0000 0010 0110 0 */
    { 13, 0x4D, 832 },	/* 0000 0010 0110 1 */
    { 13, 0x72, 896 },	/* 0000 0011 1001 0 */
    { 13, 0x73, 960 },	/* 0000 0011 1001 1 */
    { 13, 0x74, 1024 },	/* 0000 0011 1010 0 */
    { 13, 0x75, 1088 },	/* 0000 0011 1010 1 */
    { 13, 0x76, 1152 },	/* 0000 0011 1011 0 */
    { 13, 0x77, 1216 },	/* 0000 0011 1011 1 */
    { 13, 0x52, 1280 },	/* 0000 0010 1001 0 */
    { 13, 0x53, 1344 },	/* 0000 0010 1001 1 */
    { 13, 0x54, 1408 },	/* 0000 0010 1010 0 */
    { 13, 0x55, 1472 },	/* 0000 0010 1010 1 */
    { 13, 0x5A, 1536 },	/* 0000 0010 1101 0 */
    { 13, 0x5B, 1600 },	/* 0000 0010 1101 1 */
    { 13, 0x64, 1664 },	/* 0000 0011 0010 0 */
    { 13, 0x65, 1728 },	/* 0000 0011 0010 1 */
    { 11, 0x8, 1792 },	/* 0000 0001 000 */
    { 11, 0xC, 1856 },	/* 0000 0001 100 */
    { 11, 0xD, 1920 },	/* 0000 0001 101 */
    { 12, 0x12, 1984 },	/* 0000 0001 0010 */
    { 12, 0x13, 2048 },	/* 0000 0001 0011 */
    { 12, 0x14, 2112 },	/* 0000 0001 0100 */
    { 12, 0x15, 2176 },	/* 0000 0001 0101 */
    { 12, 0x16, 2240 },	/* 0000 0001 0110 */
    { 12, 0x17, 2304 },	/* 0000 0001 0111 */
    { 12, 0x1C, 2368 },	/* 0000 0001 1100 */
    { 12, 0x1D, 2432 },	/* 0000 0001 1101 */
    { 12, 0x1E, 2496 },	/* 0000 0001 1110 */
    { 12, 0x1F, 2560 },	/* 0000 0001 1111 */
    { 12, 0x1, G3CODE_EOL },	/* 0000 0000 0001 */
    { 9, 0x1, G3CODE_INVALID },	/* 0000 0000 1 */
    { 10, 0x1, G3CODE_INVALID },	/* 0000 0000 01 */
    { 11, 0x1, G3CODE_INVALID },	/* 0000 0000 001 */
    { 12, 0x0, G3CODE_INVALID },	/* 0000 0000 0000 */
};

#endif //NEWCODE //SCOT
#define	G3STATES

#ifndef __G3STATES_H__
#include "..\..\..\Include\Image\g3states.h"
#endif

#include <assert.h>

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif


typedef struct {
	Fax3BaseState b;
} Fax3DecodeState;

typedef struct {
	Fax3BaseState b;
	const BYTE *wruns;
	const BYTE *bruns;
	short	k;			/* #rows left that can be 2d encoded */
	short	maxk;			/* max #rows that can be 2d encoded */
} Fax3EncodeState;

static	int Fax3Decode1DRow(TIFF*, BYTE *, int);
static	int Fax3Encode1DRow(TIFF*, BYTE *, int);
static	int findspan(BYTE **, int, int, const BYTE *);
static	int finddiff(BYTE *, int, int, int);

void
SECTiff::TIFFModeCCITTFax3(TIFF* tif, long isClassF)
{
	if (isClassF)
		tif->tif_options |= FAX3_CLASSF;
	else
		tif->tif_options &= ~FAX3_CLASSF;
}

static BYTE bitMask[8] =
    { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
#define	isBitSet(sp)	((sp)->b.data & bitMask[(sp)->b.bit])

#define	is2DEncoding(tif) \
	(tif->tif_dir.td_group3options & GROUP3OPT_2DENCODING)
#define	fetchByte(tif, sp) \
	((tif)->tif_rawcc--, (sp)->b.bitmap[*(BYTE *)(tif)->tif_rawcp++])

#define	BITCASE(b)			\
    case b:				\
	code <<= 1;			\
	if (data & (1<<(7-b))) code |= 1;\
	len++;				\
	if (code > 0) { bit = b+1; break; }

/*
 * Skip over input until an EOL code is found.  The
 * value of len is passed as 0 except during error
 * recovery when decoding 2D data.  Note also that
 * we don't use the optimized state tables to locate
 * an EOL because we can't assume much of anything
 * about our state (e.g. bit position).
 */
static void
skiptoeol(TIFF* tif, int len)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	register int bit = sp->b.bit;
	register int data = sp->b.data;
	int code = 0;

	/*
	 * Our handling of ``bit'' is painful because
	 * the rest of the code does not maintain it as
	 * exactly the bit offset in the current data
	 * byte (bit == 0 means refill the data byte).
	 * Thus we have to be careful on entry and
	 * exit to insure that we maintain a value that's
	 * understandable elsewhere in the decoding logic.
	 */
	if (bit == 0)			/* force refill */
		bit = 8;
	for (;;) {
		switch (bit) {
	again:	BITCASE(0);
		BITCASE(1);
		BITCASE(2);
		BITCASE(3);
		BITCASE(4);
		BITCASE(5);
		BITCASE(6);
		BITCASE(7);
		default:
			if (tif->tif_rawcc <= 0)
				return;
			data = fetchByte(tif, sp);
			goto again;
		}
		if (len >= 12 && code == EOL)
			break;
		code = len = 0;
	}
	sp->b.bit = (short)(bit > 7 ? 0 : bit);	/* force refill */
	sp->b.data = (short)data;
}

/*
 * Return the next bit in the input stream.  This is
 * used to extract 2D tag values and the color tag
 * at the end of a terminating uncompressed data code.
 */
static int
nextbit(TIFF* tif)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int bit;

	if (sp->b.bit == 0 && tif->tif_rawcc > 0)
		sp->b.data = fetchByte(tif, sp);
	bit = isBitSet(sp);
	if (++(sp->b.bit) > 7)
		sp->b.bit = 0;
	return (bit);
}

/*
 * Setup G3-related compression/decompression
 * state before data is processed.  This routine
 * is called once per image -- it sets up different
 * state based on whether or not 2D encoding is used.
 */
 void *
SECTiff::Fax3SetupState(TIFF* tif, long space)
{
	TIFFDirectory *td = &tif->tif_dir;
	Fax3BaseState *sp;
	long cc = space;
	long rowbytes, rowpixels;

	if (td->td_bitspersample != 1) {
		TIFFError(tif->tif_name,
		    _T("Bits/sample must be 1 for Group 3/4 encoding/decoding"));
		return (0);
	}
	/*
	 * Calculate the scanline/tile widths.
	 */
	if (isTiled(tif)) {
		rowbytes = TIFFTileRowSize(tif);
		rowpixels = tif->tif_dir.td_tilewidth;
	} else {
		rowbytes = TIFFScanlineSize(tif);
		rowpixels = tif->tif_dir.td_imagewidth;
	}
	if (is2DEncoding(tif) || td->td_compression == COMPRESSION_CCITTFAX4)
		cc += rowbytes+1;
	tif->tif_data = (LPBYTE)_TIFFmalloc((size_t)cc);
	if (tif->tif_data == NULL) {
		TIFFError(_T("Fax3SetupState"),
		    _T("%s: No space for Fax3 state block"), tif->tif_name);
		return (0);
	}
	sp = (Fax3BaseState *)tif->tif_data;
	sp->rowbytes = rowbytes;
	sp->rowpixels = rowpixels;
	sp->bitmap = TIFFGetBitRevTable((WORD)tif->tif_fillorder != td->td_fillorder);
	sp->white = (short)(td->td_photometric == PHOTOMETRIC_MINISBLACK);
	if (is2DEncoding(tif) || td->td_compression == COMPRESSION_CCITTFAX4) {
		/*
		 * 2d encoding/decoding requires a scanline
		 * buffer for the ``reference line''; the
		 * scanline against which delta encoding
		 * is referenced.  The reference line must
		 * be initialized to be ``white'' (done elsewhere).
		 */
		sp->refline = (BYTE *)tif->tif_data + space + 1;
		/*
		 * Initialize pixel just to the left of the
		 * reference line to white.  This extra pixel
		 * simplifies the edge-condition logic.
		 */
		sp->refline[-1] = (BYTE)(sp->white ? 0xff : 0x00);
	} else
		sp->refline = 0;
	return (sp);
}

/*
 * Setup state for decoding a strip.
 */
long
SECTiff::Fax3PreDecode(TIFF* tif)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;

	if (sp == NULL) {
		sp = (Fax3DecodeState *)Fax3SetupState(tif, sizeof (*sp));
		if (!sp)
			return (0);
	}
	sp->b.bit = 0;			/* force initial read */
	sp->b.data = 0;
	sp->b.tag = Fax3BaseState::G3_1D;
	if (sp->b.refline)
		memset(sp->b.refline, sp->b.white ? 0xff:0x00, (size_t)sp->b.rowbytes);
	/*
	 * If image has EOL codes, they precede each line
	 * of data.  We skip over the first one here so that
	 * when we decode rows, we can use an EOL to signal
	 * that less than the expected number of pixels are
	 * present for the scanline.
	 */
	if ((tif->tif_options & FAX3_NOEOL) == 0) {
		skiptoeol(tif, 0);
		if (is2DEncoding(tif))
			/* tag should always be 1D! */
			sp->b.tag = nextbit(tif) ? Fax3BaseState::G3_1D : Fax3BaseState::G3_2D;
	}
	return (1);
}

/*
 * Fill a span with ones.
 */
static void
fillspan(register char* cp, register int x, register int count)
{
	static const unsigned char masks[] =
	    { 0, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff };

	if (count <= 0)
		return;
	cp += x>>3;
	x&=7;
	if (x) {			/* align to byte boundary */
		if (count < 8 - x) {
			*cp++ |= masks[count] >> x;
			return;
		}
		*cp++ |= 0xff >> x;
		count -= 8 - x;
	}
	while (count >= 8) {
		*cp++ = (BYTE)0xff;
		count -= 8;
	}
	*cp |= masks[count];
}

/*
 * Decode the requested amount of data.
 */
long
SECTiff::Fax3Decode(TIFF* tif, tidata_t buf, tsize_t occ, tsample_t s)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	long status;

	memset(buf, 0, (size_t)occ);		/* decoding only sets non-zero bits */
	while ((long)occ > 0) {
		if (sp->b.tag == Fax3BaseState::G3_1D)
			status = Fax3Decode1DRow(tif, buf, sp->b.rowpixels);
		else
			status = Fax3Decode2DRow(tif, buf, sp->b.rowpixels);
		/*
		 * For premature EOF, stop decoding and return
		 * the buffer with the remainder white-filled.
		 */
		if (status < 0)
			return (status == G3CODE_EOF);
		if (is2DEncoding(tif)) {
			/*
			 * Fetch the tag bit that indicates
			 * whether the next row is 1d or 2d
			 * encoded.  If 2d-encoded, then setup
			 * the reference line from the decoded
			 * scanline just completed.
			 */
			sp->b.tag = nextbit(tif) ? Fax3BaseState::G3_1D : Fax3BaseState::G3_2D;
			if (sp->b.tag == Fax3BaseState::G3_2D)
				memcpy(sp->b.refline, buf, (size_t)sp->b.rowbytes);
		}
		buf += sp->b.rowbytes;
		occ -= sp->b.rowbytes;
		if (occ != 0)
			tif->tif_row++;
	}
	s; //UNUSED
	return (1);
}

/*
 * Decode a run of white.
 */
static int
decode_white_run(TIFF* tif)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short state = sp->b.bit;
	short action;
	int runlen = 0;

	for (;;) {
		if (sp->b.bit == 0) {
	nextbyte:
			if (tif->tif_rawcc <= 0)
				return (G3CODE_EOF);
			sp->b.data = fetchByte(tif, sp);
		}
		action = TIFFFax1DAction[state][sp->b.data];
		state = TIFFFax1DNextState[state][sp->b.data];
		if (action == ACT_INCOMP)
			goto nextbyte;
		if (action == ACT_INVALID)
			return (G3CODE_INVALID);
		if (action == ACT_EOL)
			return (G3CODE_EOL);
		sp->b.bit = state;
		action = RUNLENGTH(action - ACT_WRUNT);
		runlen += action;
		if (action < 64)
			return (runlen);
	}
	/*NOTREACHED*/
}

/*
 * Decode a run of black.
 */
static int
decode_black_run(TIFF* tif)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short state = (short)(sp->b.bit + 8);
	short action;
	int runlen = 0;

	for (;;) {
		if (sp->b.bit == 0) {
	nextbyte:
			if (tif->tif_rawcc <= 0)
				return (G3CODE_EOF);
			sp->b.data = fetchByte(tif, sp);
		}
		action = TIFFFax1DAction[state][sp->b.data];
		state = TIFFFax1DNextState[state][sp->b.data];
		if (action == ACT_INCOMP)
			goto nextbyte;
		if (action == ACT_INVALID)
			return (G3CODE_INVALID);
		if (action == ACT_EOL)
			return (G3CODE_EOL);
		sp->b.bit = state;
		action = RUNLENGTH(action - ACT_BRUNT);
		runlen += action;
		if (action < 64)
			return (runlen);
		state += 8;
	}
	/*NOTREACHED*/
}

/*
 * Process one row of 1d Huffman-encoded data.
 */
long
SECTiff::Fax3Decode1DRow(TIFF* tif, BYTE *buf, long npels)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int x = 0;
	long runlen;
	// short action;
	short color = sp->b.white;
	static const TCHAR module[] = _T("Fax3Decode1D");

	for (;;) {
		if (color == sp->b.white)
			runlen = decode_white_run(tif);
		else
			runlen = decode_black_run(tif);
		switch (runlen) {
		case G3CODE_EOF:
			TIFFWarning(module,
			    _T("%s: Premature EOF at scanline %d (x %d)"),
			    tif->tif_name, tif->tif_row, x);
			return (G3CODE_EOF);
		case G3CODE_INVALID:	/* invalid code */
			/*
			 * An invalid code was encountered.
			 * Flush the remainder of the line
			 * and allow the caller to decide whether
			 * or not to continue.  Note that this
			 * only works if we have a G3 image
			 * with EOL markers.
			 */
			TIFFError(module,
			   _T("%s: Bad code word at scanline %d (x %d)"),
			   tif->tif_name, tif->tif_row, x);
			goto done;
		case G3CODE_EOL:	/* premature end-of-line code */
			TIFFWarning(module,
			    _T("%s: Premature EOL at scanline %d (x %d)"),
			    tif->tif_name, tif->tif_row, x);
			return (1);	/* try to resynchronize... */
		}
		if (x+runlen > npels)
			runlen = npels-x;
		if (runlen > 0) {
			if (color)
				fillspan((char *)buf, (int)x, (int)runlen);
			x += (int)runlen;
			if (x >= npels)
				break;
		}
		color = (short)(!color);
	}
done:
	/*
	 * Cleanup at the end of the row.  This convoluted
	 * logic is merely so that we can reuse the code with
	 * two other related compression algorithms (2 & 32771).
	 *
	 * Note also that our handling of word alignment assumes
	 * that the buffer is at least word aligned.  This is
	 * the case for most all versions of malloc (typically
	 * the buffer is returned longword aligned).
	 */
	if ((tif->tif_options & FAX3_NOEOL) == 0)
		skiptoeol(tif, 0);
	if (tif->tif_options & FAX3_BYTEALIGN)
		sp->b.bit = 0;
	if ((tif->tif_options & FAX3_WORDALIGN) && ((long)tif->tif_rawcp & 1))
		(void) fetchByte(tif, sp);
	return (x == npels ? 1 : G3CODE_EOL);
}

/*
 * Group 3 2d Decoding support.
 */

/*
 * Return the next uncompressed mode code word.
 */
static int
decode_uncomp_code(TIFF* tif)
{
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	short code;

	do {
		if (sp->b.bit == 0 || sp->b.bit > 7) {
			if (tif->tif_rawcc <= 0)
				return (UNCOMP_EOF);
			sp->b.data = fetchByte(tif, sp);
		}
		code = TIFFFaxUncompAction[sp->b.bit][sp->b.data];
		sp->b.bit = TIFFFaxUncompNextState[sp->b.bit][sp->b.data];
	} while (code == ACT_INCOMP);
	return (code);
}

/*
 * Process one row of 2d encoded data.
 */
long
SECTiff::Fax3Decode2DRow(TIFF* tif, BYTE *buf, DWORD npels)
{
#define	PIXEL(buf,ix)	((((buf)[(ix)>>3]) >> (7-((ix)&7))) & 1)
	Fax3DecodeState *sp = (Fax3DecodeState *)tif->tif_data;
	int a0 = -1;
	int b1=-1, b2=-1;
	int run1=0, run2=0;		/* for horizontal mode */
	short mode;
	short color = sp->b.white;
	static const TCHAR module[] = _T("Fax3Decode2D");

	do {
		if (sp->b.bit == 0 || sp->b.bit > 7) {
			if (tif->tif_rawcc <= 0) {
				TIFFError(module,
				    _T("%s: Premature EOF at scanline %d"),
				    tif->tif_name, tif->tif_row);
				return (G3CODE_EOF);
			}
			sp->b.data = fetchByte(tif, sp);
		}
		mode = TIFFFax2DMode[sp->b.bit][sp->b.data];
		sp->b.bit = TIFFFax2DNextState[sp->b.bit][sp->b.data];
		switch (mode) {
		case MODE_NULL:
			break;
		case MODE_PASS:
			b2 = finddiff(sp->b.refline, a0, (int)npels, !color);
			b1 = finddiff(sp->b.refline, b2, (int)npels, color);
			b2 = finddiff(sp->b.refline, b1, (int)npels, !color);
			if (color) {
				if (a0 < 0)
					a0 = 0;
				fillspan((char *)buf, a0, b2 - a0);
			}
			a0 = b2;
			break;
		case MODE_HORIZ:
			if (color == sp->b.white) {
				run1 = decode_white_run(tif);
				run2 = decode_black_run(tif);
			} else {
				run1 = decode_black_run(tif);
				run2 = decode_white_run(tif);
			}
			if (run1 >= 0 && run2 >= 0) {
				/*
				 * Do the appropriate fill.  Note that we exit
				 * this logic with the same color that we enter
				 * with since we do 2 fills.  This explains the
				 * somewhat obscure logic below.
				 */
				if (a0 < 0)
					a0 = 0;
				if ((DWORD)(a0 + run1) > npels)
					run1 = (int)npels - a0;
				if (color)
					fillspan((char *)buf, a0, run1);
				a0 += run1;
				if ((DWORD)(a0 + run2) > npels)
					run2 =(int)npels - a0;
				if (!color)
					fillspan((char *)buf, a0, run2);
				a0 += run2;
			}
			break;
		case MODE_VERT_V0:
		case MODE_VERT_VR1:
		case MODE_VERT_VR2:
		case MODE_VERT_VR3:
		case MODE_VERT_VL1:
		case MODE_VERT_VL2:
		case MODE_VERT_VL3:
			b2 = finddiff(sp->b.refline, a0, (int)npels, !color);
			b1 = finddiff(sp->b.refline, b2, (int)npels, color);
			b1 += mode - MODE_VERT_V0;
			if (color) {
				if (a0 < 0)
					a0 = 0;
				fillspan((char *)buf, a0, b1 - a0);
			}
			color = (short)(!color);
			a0 = b1;
			break;
	        case MODE_UNCOMP:
			/*
			 * Uncompressed mode: select from the
			 * special set of code words.
			 */
			if (a0 < 0)
				a0 = 0;
			do {
				mode = (short)(decode_uncomp_code(tif));
				switch (mode) {
				case UNCOMP_RUN1:
				case UNCOMP_RUN2:
				case UNCOMP_RUN3:
				case UNCOMP_RUN4:
				case UNCOMP_RUN5:
					run1 = mode - UNCOMP_RUN0;
					fillspan((char *)buf, a0+run1-1, 1);
					a0 += run1;
					break;
				case UNCOMP_RUN6:
					a0 += 5;
					break;
				case UNCOMP_TRUN0:
				case UNCOMP_TRUN1:
				case UNCOMP_TRUN2:
				case UNCOMP_TRUN3:
				case UNCOMP_TRUN4:
					run1 = mode - UNCOMP_TRUN0;
					a0 += run1;
					color = (short)(nextbit(tif) ?
					    !sp->b.white : sp->b.white);
					break;
				case UNCOMP_INVALID:
					TIFFError(module,
				_T("%s: Bad uncompressed code word at scanline %d"),
					    tif->tif_name, tif->tif_row);
					goto bad;
				case UNCOMP_EOF:
					TIFFError(module,
					    _T("%s: Premature EOF at scanline %d"),
					    tif->tif_name, tif->tif_row);
					return (G3CODE_EOF);
				}
			} while (mode < UNCOMP_EXIT);
			break;
	        case MODE_ERROR_1:
			if ((tif->tif_options & FAX3_NOEOL) == 0) {
				TIFFWarning(module,
				    _T("%s: Premature EOL at scanline %d (x %d)"),
				    tif->tif_name, tif->tif_row, a0);
				skiptoeol(tif, 7);	/* seen 7 0's already */
				return (1);		/* try to synchronize */
			}
			/* fall thru... */
	        case MODE_ERROR:
			TIFFError(module,
			    _T("%s: Bad 2D code word at scanline %d"),
			    tif->tif_name, tif->tif_row);
			goto bad;
	        default:
			TIFFError(module,
			    _T("%s: Panic, bad decoding state at scanline %d"),
			    tif->tif_name, tif->tif_row);
			return (0);
		}
	} while (a0 < (long)npels);
bad:
	/*
	 * Cleanup at the end of row.  We check for
	 * EOL separately so that this code can be
	 * reused by the Group 4 decoding routine.
	 */
	if ((tif->tif_options & FAX3_NOEOL) == 0)
		skiptoeol(tif, 0);
	return ((DWORD)a0 >= npels ? 1 : G3CODE_EOL);
#undef	PIXEL
}

/*
 * CCITT Group 3 FAX Encoding.
 */

/*
 * Write a variable-length bit-value to
 * the output stream.  Values are
 * assumed to be at most 16 bits.
 */
void
SECTiff::Fax3PutBits(TIFF* tif, DWORD bits, long length)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;
	static const int mask[9] =
	    { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

	while (length > sp->bit) {
		sp->data |= bits >> (length - sp->bit);
		length -= sp->bit;
		Fax3FlushBits(tif, sp);
	}
	sp->data |= (bits & mask[length]) << (sp->bit - length);
	sp->bit = (short)(sp->bit - length);
	if (sp->bit == 0)
		Fax3FlushBits(tif, sp);
}

/*
 * Write a code to the output stream.
 */
 void
SECTiff::fax3_putcode(TIFF* tif, const tableentry* te)
{
	Fax3PutBits(tif, te->code, te->length);
}

/*
 * Write the sequence of codes that describes
 * the specified span of zero's or one's.  The
 * appropriate table that holds the make-up and
 * terminating codes is supplied.
 */
void
SECTiff::fax3_putspan(TIFF* tif, long span, const tableentry* tab)
{
	while (span >= 2624) {
		const tableentry *te = &tab[63 + (2560>>6)];
		fax3_putcode(tif, te);
		span -= te->runlen;
	}
	if (span >= 64) {
		const tableentry *te = &tab[63 + (span>>6)];
		assert(te->runlen == 64*(span>>6));
		fax3_putcode(tif, te);
		span -= te->runlen;
	}
	fax3_putcode(tif, &tab[span]);
}

/*
 * Write an EOL code to the output stream.  The zero-fill
 * logic for byte-aligning encoded scanlines is handled
 * here.  We also handle writing the tag bit for the next
 * scanline when doing 2d encoding.
 */
void
SECTiff::Fax3PutEOL(TIFF* tif)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	if (tif->tif_dir.td_group3options & GROUP3OPT_FILLBITS) {
		/*
		 * Force bit alignment so EOL will terminate on
		 * a byte boundary.  That is, force the bit alignment
		 * to 16-12 = 4 before putting out the EOL code.
		 */
		int align = 8 - 4;
		if (align != sp->bit) {
			if (align > sp->bit)
				align = sp->bit + (8 - align);
			else
				align = sp->bit - align;
			Fax3PutBits(tif, 0, align);
		}
	}
	Fax3PutBits(tif, EOL, 12);
	if (is2DEncoding(tif))
		Fax3PutBits(tif, sp->tag == Fax3BaseState::G3_1D, 1);
}

static const BYTE zeroruns[256] = {
    8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4,	/* 0x00 - 0x0f */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0x10 - 0x1f */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x20 - 0x2f */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0x30 - 0x3f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x40 - 0x4f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x50 - 0x5f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x60 - 0x6f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x70 - 0x7f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x80 - 0x8f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x90 - 0x9f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xa0 - 0xaf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xb0 - 0xbf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xc0 - 0xcf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xd0 - 0xdf */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xe0 - 0xef */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0xf0 - 0xff */
};
static const BYTE oneruns[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x00 - 0x0f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x10 - 0x1f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x20 - 0x2f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x30 - 0x3f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x40 - 0x4f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x50 - 0x5f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x60 - 0x6f */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,	/* 0x70 - 0x7f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x80 - 0x8f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0x90 - 0x9f */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xa0 - 0xaf */
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,	/* 0xb0 - 0xbf */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xc0 - 0xcf */
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,	/* 0xd0 - 0xdf */
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,	/* 0xe0 - 0xef */
    4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 7, 8,	/* 0xf0 - 0xff */
};

/*
 * Reset encoding state at the start of a strip.
 */
long
SECTiff::Fax3PreEncode(TIFF* tif)
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

	if (sp == NULL) {
		sp = (Fax3EncodeState *)Fax3SetupState(tif, (long)(sizeof (*sp)));
		if (!sp)
			return (0);
		if (sp->b.white == 0) {
			sp->wruns = zeroruns;
			sp->bruns = oneruns;
		} else {
			sp->wruns = oneruns;
			sp->bruns = zeroruns;
		}
	}
	sp->b.bit = 8;
	sp->b.data = 0;
	sp->b.tag = Fax3BaseState::G3_1D;
	/*
	 * This is necessary for Group 4; otherwise it isn't
	 * needed because the first scanline of each strip ends
	 * up being copied into the refline.
	 */
	if (sp->b.refline)
		memset(sp->b.refline, sp->b.white ? 0xff:0x00, (size_t)sp->b.rowbytes);
	if (is2DEncoding(tif)) {
		float res = tif->tif_dir.td_yresolution;
		/*
		 * The CCITT spec says that when doing 2d encoding, you
		 * should only do it on K consecutive scanlines, where K
		 * depends on the resolution of the image being encoded
		 * (2 for <= 200 lpi, 4 for > 200 lpi).  Since the directory
		 * code initializes td_yresolution to 0, this code will
		 * select a K of 2 unless the YResolution tag is set
		 * appropriately.  (Note also that we fudge a little here
		 * and use 150 lpi to avoid problems with units conversion.)
		 */
		if (tif->tif_dir.td_resolutionunit == RESUNIT_CENTIMETER)
			res = (float)(res * (float).3937) / (float)2.54;	/* convert to inches */
		sp->maxk = (short)(res > 150 ? 4 : 2);
		sp->k = (short)(sp->maxk-1);
	} else
		sp->k = sp->maxk = 0;
	return (1);
}

/*
 * 1d-encode a row of pixels.  The encoding is
 * a sequence of all-white or all-black spans
 * of pixels encoded with Huffman codes.
 */
 long
SECTiff::Fax3Encode1DRow(TIFF* tif, BYTE *bp, long bits)
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;
	int bs = 0, span;

	for (;;) {
		span = findspan(&bp, (int)bs, (int)bits, sp->wruns);	/* white span */
		fax3_putspan(tif, span, TIFFFaxWhiteCodes);
		bs += span;
		if (bs >= bits)
			break;
		span = findspan(&bp, (int)bs, (int)bits, sp->bruns);	/* black span */
		fax3_putspan(tif, span, TIFFFaxBlackCodes);
		bs += span;
		if (bs >= bits)
			break;
	}
	return (1);
}

static const tableentry horizcode =
    { 3, 0x1 };		/* 001 */
static const tableentry passcode =
    { 4, 0x1 };		/* 0001 */
static const tableentry vcodes[7] = {
    { 7, 0x03 },	/* 0000 011 */
    { 6, 0x03 },	/* 0000 11 */
    { 3, 0x03 },	/* 011 */
    { 1, 0x1 },		/* 1 */
    { 3, 0x2 },		/* 010 */
    { 6, 0x02 },	/* 0000 10 */
    { 7, 0x02 }		/* 0000 010 */
};
    
/*
 * 2d-encode a row of pixels.  Consult the CCITT
 * documentation for the algorithm.
 */
long
SECTiff::Fax3Encode2DRow(TIFF* tif, BYTE *bp, BYTE *rp, DWORD bits)
{
#define	PIXEL(buf,ix)	((((buf)[(ix)>>3]) >> (7-((ix)&7))) & 1)
	short white = ((Fax3BaseState *)tif->tif_data)->white;
	int a0 = 0;
	int a1 = (PIXEL(bp, 0) != white ? 0 : finddiff(bp, 0, (int)bits, white));
	int b1 = (PIXEL(rp, 0) != white ? 0 : finddiff(rp, 0, (int)bits, white));
	int a2, b2;

	for (;;) {
		b2 = finddiff(rp, b1, (int)bits, PIXEL(rp,b1));
		if (b2 >= a1) {
			int d = b1 - a1;
			if (!(-3 <= d && d <= 3)) {	/* horizontal mode */
				a2 = finddiff(bp, a1, (int)bits, PIXEL(bp,a1));
				fax3_putcode(tif, &horizcode);
				if (a0+a1 == 0 || PIXEL(bp, a0) == white) {
					fax3_putspan(tif, a1-a0, TIFFFaxWhiteCodes);
					fax3_putspan(tif, a2-a1, TIFFFaxBlackCodes);
				} else {
					fax3_putspan(tif, a1-a0, TIFFFaxBlackCodes);
					fax3_putspan(tif, a2-a1, TIFFFaxWhiteCodes);
				}
				a0 = a2;
			} else {			/* vertical mode */
				fax3_putcode(tif, &vcodes[d+3]);
				a0 = a1;
			}
		} else {				/* pass mode */
			fax3_putcode(tif, &passcode);
			a0 = b2;
		}
		if ((DWORD)a0 >= bits)
			break;
		a1 = finddiff(bp, a0, (int)bits, PIXEL(bp,a0));
		b1 = finddiff(rp, a0, (int)bits, !PIXEL(bp,a0));
		b1 = finddiff(rp, b1, (int)bits, PIXEL(bp,a0));
	}
	return (1);
#undef PIXEL
}

/*
 * Encode a buffer of pixels.
 */
long
SECTiff::Fax3Encode(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	Fax3EncodeState *sp = (Fax3EncodeState *)tif->tif_data;

	while ((long)cc > 0) {
		Fax3PutEOL(tif);
		if (is2DEncoding(tif)) {
			if (sp->b.tag == Fax3BaseState::G3_1D) {
				if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels))
					return (0);
				sp->b.tag = Fax3BaseState::G3_2D;
			} else {
				if (!Fax3Encode2DRow(tif, bp, sp->b.refline, sp->b.rowpixels))
					return (0);
				sp->k--;
			}
			if (sp->k == 0) {
				sp->b.tag = Fax3BaseState::G3_1D;
				sp->k = (short)(sp->maxk-1);
			} else
				memcpy(sp->b.refline, bp, (size_t)sp->b.rowbytes);
		} else {
			if (!Fax3Encode1DRow(tif, bp, sp->b.rowpixels))
				return (0);
		}
		bp += sp->b.rowbytes;
		cc -= sp->b.rowbytes;
		if (cc != 0)
			tif->tif_row++;
	}
	s; //UNUSED
	return (1);
}

long
SECTiff::Fax3PostEncode(TIFF* tif)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	if (sp->bit != 8)
		Fax3FlushBits(tif, sp);
	return (1);
}

 void
SECTiff::Fax3Close(TIFF* tif)
{
	if ((tif->tif_options & FAX3_CLASSF) == 0) {	/* append RTC */
		int i;
		for (i = 0; i < 6; i++) {
		    Fax3PutBits(tif, EOL, 12);
		    if (is2DEncoding(tif))
			    Fax3PutBits(tif, 1, 1);
		}
		(void) Fax3PostEncode(tif);
	}
}

void
SECTiff::Fax3Cleanup(TIFF* tif)
{
	if (tif->tif_data) {
		_TIFFfree(tif->tif_data);
		tif->tif_data = NULL;
	}
}

/*
 * Bit handling utilities.
 */

/*
 * Find a span of ones or zeros using the supplied
 * table.  The byte-aligned start of the bit string
 * is supplied along with the start+end bit indices.
 * The table gives the number of consecutive ones or
 * zeros starting from the msb and is indexed by byte
 * value.
 */
static int
findspan(BYTE **bpp, int bs, int be, register const BYTE *tab)
{
	register BYTE *bp = *bpp;
	register int bits = be - bs;
	//AAB register int n, span;
	register int n=0, span=0;

	/*
	 * Check partial byte on lhs.
	 */
	if (bits > 0) {
		n = (bs&7);
		if (n)
		{
			span = tab[(*bp << n) & 0xff];
			if (span > 8-n)		/* table value too generous */
				span = 8-n;
			if (span > bits)	/* constrain span to bit range */
				span = bits;
			if (n+span < 8)		/* doesn't extend to edge of byte */
				goto done;
			bits -= span;
			bp++;
		}
	} else
		span = 0;
	/*
	 * Scan full bytes for all 1's or all 0's.
	 */
	while (bits >= 8) {
		n = tab[*bp];
		span += n;
		bits -= n;
		if (n < 8)		/* end of run */
			goto done;
		bp++;
	}
	/*
	 * Check partial byte on rhs.
	 */
	if (bits > 0) {
		n = tab[*bp];
		span += (n > bits ? bits : n);
	}
done:
	*bpp = bp;
	return (span);
}

/*
 * Return the offset of the next bit in the range
 * [bs..be] that is different from the specified
 * color.  The end, be, is returned if no such bit
 * exists.
 */
static int
finddiff(BYTE *cp, int bs, int be, int color)
{
	cp += bs >> 3;			/* adjust byte offset */
	return (bs + findspan(&cp, bs, be, color ? oneruns : zeroruns));
}

long
SECTiff::TIFFInitCCITTFax3(TIFF* tif)
{
	tif->tif_predecode = &SECTiff::Fax3PreDecode;
	tif->tif_decoderow = &SECTiff::Fax3Decode;
	tif->tif_decodestrip = &SECTiff::Fax3Decode;
	tif->tif_decodetile = &SECTiff::Fax3Decode;
	tif->tif_preencode = &SECTiff::Fax3PreEncode;
	tif->tif_postencode = &SECTiff::Fax3PostEncode;
	tif->tif_encoderow = &SECTiff::Fax3Encode;
	tif->tif_encodestrip = &SECTiff::Fax3Encode;
	tif->tif_encodetile = &SECTiff::Fax3Encode;
	tif->tif_close = &SECTiff::Fax3Close;
	tif->tif_cleanup = &SECTiff::Fax3Cleanup;
	tif->tif_options |= FAX3_CLASSF;	/* default */
	tif->tif_flags |= TIFF_NOBITREV;	/* we handle bit reversal */
	return (1);
}

//AAB #endif /*WIN32*/
