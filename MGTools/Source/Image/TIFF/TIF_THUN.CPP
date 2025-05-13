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
//  Description:	tif_thun.cpp
//		ThunderScan 4-bit compression algorithm support
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//


#include "..\..\..\stdafx.h"

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif


/*
 * ThunderScan uses an encoding scheme designed for
 * 4-bit pixel values.  Data is encoded in bytes, with
 * each byte split into a 2-bit code word and a 6-bit
 * data value.  The encoding gives raw data, runs of
 * pixels, or pixel values encoded as a delta from the
 * previous pixel value.  For the latter, either 2-bit
 * or 3-bit delta values are used, with the deltas packed
 * into a single byte.
 */
#define	THUNDER_DATA		0x3f	/* mask for 6-bit data */
#define	THUNDER_CODE		0xc0	/* mask for 2-bit code word */
/* code values */
#define	THUNDER_RUN		0x00	/* run of pixels w/ encoded count */
#define	THUNDER_2BITDELTAS	0x40	/* 3 pixels w/ encoded 2-bit deltas */
#define	    DELTA2_SKIP		2	/* skip code for 2-bit deltas */
#define	THUNDER_3BITDELTAS	0x80	/* 2 pixels w/ encoded 3-bit deltas */
#define	    DELTA3_SKIP		4	/* skip code for 3-bit deltas */
#define	THUNDER_RAW		0xc0	/* raw data encoded */

static const short twobitdeltas[4] = { 0, 1, 0, -1 };
static const short threebitdeltas[8] = { 0, 1, 2, 3, 0, -3, -2, -1 };

#define	SETPIXEL(op, v) { \
	lastpixel = (WORD)((v) & 0xf); \
	if (npixels++ & 1) \
	    *op++ |= (char)lastpixel; \
	else \
	    op[0] = (char)(lastpixel << 4); \
}

long
SECTiff::ThunderDecode(TIFF* tif, tidata_t op, tsize_t maxpixels)
{
	//AAB register u_char *bp;
	register BYTE *bp;
	register tsize_t cc;
	//AAB u_int lastpixel;
	WORD lastpixel;
	tsize_t npixels;

	bp = (BYTE *)tif->tif_rawcp;
	cc = tif->tif_rawcc;
	lastpixel = 0;
	npixels = 0;
	while (cc > 0 && npixels < maxpixels) {
		int n, delta;

		n = *bp++, cc--;
		switch (n & THUNDER_CODE) {
		case THUNDER_RUN:		/* pixel run */
			/*
			 * Replicate the last pixel n times,
			 * where n is the lower-order 6 bits.
			 */
			if (npixels & 1) {
				op[0] |= lastpixel;
				lastpixel = *op++; npixels++; n--;
			} else
				lastpixel |= lastpixel << 4;
			npixels += n;
			for (; n > 0; n -= 2)
				*op++ = (char)lastpixel;
			if (n == -1)
				*--op &= 0xf0;
			lastpixel &= 0xf;
			break;
		case THUNDER_2BITDELTAS:	/* 2-bit deltas */
			if ((delta = ((n >> 4) & 3)) != DELTA2_SKIP)
				SETPIXEL(op, lastpixel + twobitdeltas[delta]);
			if ((delta = ((n >> 2) & 3)) != DELTA2_SKIP)
				SETPIXEL(op, lastpixel + twobitdeltas[delta]);
			if ((delta = (n & 3)) != DELTA2_SKIP)
				SETPIXEL(op, lastpixel + twobitdeltas[delta]);
			break;
		case THUNDER_3BITDELTAS:	/* 3-bit deltas */
			if ((delta = ((n >> 3) & 7)) != DELTA3_SKIP)
				SETPIXEL(op, lastpixel + threebitdeltas[delta]);
			if ((delta = (n & 7)) != DELTA3_SKIP)
				SETPIXEL(op, lastpixel + threebitdeltas[delta]);
			break;
		case THUNDER_RAW:		/* raw data */
			SETPIXEL(op, n);
			break;
		}
	}
	tif->tif_rawcp = (tidata_t) bp;
	tif->tif_rawcc = cc;
	if (npixels != maxpixels) {
		TIFFError(tif->tif_name,
		    _T("ThunderDecode: %s data at scanline %ld (%lu != %lu)"),
		    npixels < maxpixels ? _T("Not enough") : _T("Too much"),
		    (long) tif->tif_row, (long) npixels, (long) maxpixels);
		return (0);
	}
	return (1);
}

long
SECTiff::ThunderDecodeRow(TIFF* tif, tidata_t buf, tsize_t occ, tsample_t s)
{
	tidata_t row = buf;
	
	while ((long)occ > 0) {
		if (!ThunderDecode(tif, row, tif->tif_dir.td_imagewidth))
			return (0);
		occ -= tif->tif_scanlinesize;
		row += tif->tif_scanlinesize;
	}
s; //UNUSED
	return (1);
}

long
SECTiff::TIFFInitThunderScan(TIFF* tif)
{
	tif->tif_decoderow = &SECTiff::ThunderDecodeRow;
	tif->tif_decodestrip = &SECTiff::ThunderDecodeRow;
	return (1L);
}
