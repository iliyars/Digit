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
//  Description:	tif_pack.cpp
//		packbits compression algorithm support
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

#include <assert.h>

#ifndef _INC_STDIO
#include <stdio.h>
#endif

//#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

int
SECTiff::PackBitsPreEncode(TIFF* tif)
{
	/*
	 * Calculate the scanline/tile-width size in bytes.
	 */
	if (isTiled(tif))
		tif->tif_data = (tidata_t) TIFFTileRowSize(tif);
	else
		tif->tif_data = (tidata_t) TIFFScanlineSize(tif);
	return (1);
}

/*
 * Encode a run of pixels.
 */
int
SECTiff::PackBitsEncode(TIFF* tif, tidata_t buf, tsize_t cc, tsample_t s)
{
	LPBYTE bp = (LPBYTE) buf;
	tidata_t op, ep, lastliteral;
	long n, slop;
	int b;
	enum { BASE, LITERAL, RUN, LITERAL_RUN } state;

	op = tif->tif_rawcp;
	ep = tif->tif_rawdata + tif->tif_rawdatasize;
	state = BASE;
	lastliteral = 0;
	while (cc > 0) {
		/*
		 * Find the longest string of identical bytes.
		 */
		b = *bp++, cc--, n = 1;
		for (; cc > 0 && b == *bp; cc--, bp++)
			n++;
	again:
		if (op + 2 >= ep) {		/* insure space for new data */
			/*
			 * Be careful about writing the last
			 * literal.  Must write up to that point
			 * and then copy the remainder to the
			 * front of the buffer.
			 */
			if (state == LITERAL || state == LITERAL_RUN) {
				slop = op - lastliteral;
				tif->tif_rawcc += lastliteral - tif->tif_rawcp;
				if (!TIFFFlushData1(tif))
					return (-1);
				op = tif->tif_rawcp;
				while (slop-- > 0)
					*op++ = *lastliteral++;
				lastliteral = tif->tif_rawcp;
			} else {
				tif->tif_rawcc += op - tif->tif_rawcp;
				if (!TIFFFlushData1(tif))
					return (-1);
				op = tif->tif_rawcp;
			}
		}
		switch (state) {
		case BASE:		/* initial state, set run/literal */
			if (n > 1) {
				state = RUN;
				if (n > 128) {
					*op++ = (BYTE)((char)-127);
					*op++ = (BYTE)b;
					n -= 128;
					goto again;
				}
				*op++ = (char)-(n-1);
				*op++ = (BYTE)b;
			} else {
				lastliteral = op;
				*op++ = 0;
				*op++ = (BYTE)b;
				state = LITERAL;
			}
			break;
		case LITERAL:		/* last object was literal string */
			if (n > 1) {
				state = LITERAL_RUN;
				if (n > 128) {
					*op++ = (BYTE)((char)-127);
					*op++ = (BYTE)b;
					n -= 128;
					goto again;
				}
				*op++ = (char)-(n-1);		/* encode run */
				*op++ = (BYTE)b;
			} else {			/* extend literal */
				if (++(*lastliteral) == 127)
					state = BASE;
				*op++ = (BYTE)b;
			}
			break;
		case RUN:		/* last object was run */
			if (n > 1) {
				if (n > 128) {
					*op++ = (BYTE)((char)-127);
					*op++ = (BYTE)b;
					n -= 128;
					goto again;
				}
				*op++ = (char)-(n-1);
				*op++ = (BYTE)b;
			} else {
				lastliteral = op;
				*op++ = 0;
				*op++ = (BYTE)b;
				state = LITERAL;
			}
			break;
		case LITERAL_RUN:	/* literal followed by a run */
			/*
			 * Check to see if previous run should
			 * be converted to a literal, in which
			 * case we convert literal-run-literal
			 * to a single literal.
			 */
			if (n == 1 && -op[-2] == 1 && *lastliteral < 126) {
				state = (((*lastliteral) += 2) == 127 ?
				    BASE : LITERAL);
				op[-2] = op[-1];	/* replicate */
			} else
				state = RUN;
			goto again;
		}
	}
	tif->tif_rawcc += op - tif->tif_rawcp;
	tif->tif_rawcp = op;
s; //UNUSED
	return (1);
}

/*
 * Encode a rectangular chunk of pixels.  We break it up
 * into row-sized pieces to insure that encoded runs do
 * not span rows.  Otherwise, there can be problems with
 * the decoder if data is read, for example, by scanlines
 * when it was encoded by strips.
 */
int
SECTiff::PackBitsEncodeChunk(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	tsize_t rowsize = (tsize_t) tif->tif_data;

	assert(rowsize > 0);
	while ((long)cc > 0) {
		if (PackBitsEncode(tif, bp, rowsize, s) < 0)
			return (-1);
		bp += rowsize;
		cc -= rowsize;
	}
	return (1);
}

int
SECTiff::PackBitsDecode(TIFF* tif, tidata_t op, tsize_t occ, tsample_t s)
{
	LPBYTE bp;
	tsize_t cc;

	bp = (LPBYTE) tif->tif_rawcp;
	cc = tif->tif_rawcc;
	while (cc > 0 && (long)occ > 0) {
		long n = (long) *bp++;
		int b;
		/*
		 * Watch out for compilers that
		 * don't sign extend chars...
		 */
		if (n >= 128)
			n -= 256;
		if (n < 0) {		/* replicate next byte -n+1 times */
			cc--;
			if (n == -128)	/* nop */
				continue;
			n = -n + 1;
			occ -= n;
			for (b = *bp++; n-- > 0;)
				*op++ = (BYTE)b;
		} else {		/* copy next n+1 bytes literally */
#ifdef WIN32
			memcpy(op, bp, (size_t)++n);
#else
			_fmemcpy(op, bp, (size_t)++n);
#endif
			op += n; occ -= n;
			bp += n; cc -= n;
		}
	}
	tif->tif_rawcp = (tidata_t) bp;
	tif->tif_rawcc = cc;
	if (occ > 0) {
		TIFFError(tif->tif_name,
		    _T("PackBitsDecode: Not enough data for scanline %ld"),
		    (long) tif->tif_row);
		return (0);
	}   
	s; //UNUSED
	/* check for buffer overruns? */
	return (1);
}

long
SECTiff::TIFFInitPackBits(TIFF* tif)
{
	tif->tif_decoderow = (TIFFCodeMethod) &SECTiff::PackBitsDecode;
	tif->tif_decodestrip = (TIFFCodeMethod)&SECTiff::PackBitsDecode;
	tif->tif_decodetile = (TIFFCodeMethod)&SECTiff::PackBitsDecode;
	tif->tif_preencode = (TIFFBoolMethod)&SECTiff::PackBitsPreEncode;
	tif->tif_encoderow = (TIFFCodeMethod)&SECTiff::PackBitsEncode;
	tif->tif_encodestrip = (TIFFCodeMethod)&SECTiff::PackBitsEncodeChunk;
	tif->tif_encodetile = (TIFFCodeMethod)&SECTiff::PackBitsEncodeChunk;
	return (1);
}
