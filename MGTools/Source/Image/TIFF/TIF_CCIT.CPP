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
//  Description:	tif_ccit.cpp
//		CCITT Group 3 1-D Modified Huffman RLE compression
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
 *
 * CCITT Group 3 1-D Modified Huffman
 * Run Length Encoding Compression Support
 */
#include "..\..\..\stdafx.h"

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __TIF_FAX3_H__
#include "..\..\..\Include\Image\tif_fax3.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

long 
SECTiff::TIFFInitCCITTRLE(TIFF* tif)
{
//AAB #ifdef WIN32
	TIFFInitCCITTFax3(tif);		/* reuse G3 compression */
//AAB #endif
	tif->tif_preencode = NULL;
	tif->tif_postencode = NULL;
	tif->tif_encoderow = &SECTiff::TIFFNoRowEncode;
	tif->tif_encodestrip = &SECTiff::TIFFNoStripEncode;
	tif->tif_encodetile = &SECTiff::TIFFNoTileEncode;
	tif->tif_close = NULL;
	/*
	 * This magic causes the regular G3 decompression
	 * code to not skip to the EOL mark at the end of
	 * a row, and to flush input data to a byte boundary
	 * at the end of each row.
	 */
	tif->tif_options |= FAX3_NOEOL|FAX3_BYTEALIGN;
	return (1);
}

long
SECTiff::TIFFInitCCITTRLEW(TIFF* tif)
{
//AAB #ifdef WIN32
	TIFFInitCCITTFax3(tif);		/* reuse G3 compression */
//AAB #endif
	tif->tif_preencode = NULL;
	tif->tif_postencode = NULL;
	tif->tif_encoderow = &SECTiff::TIFFNoRowEncode;
	tif->tif_encodestrip = &SECTiff::TIFFNoStripEncode;
	tif->tif_encodetile = &SECTiff::TIFFNoTileEncode;
	tif->tif_close = NULL;
	/*
	 * This magic causes the regular G3 decompression
	 * code to not skip to the EOL mark at the end of
	 * a row, and to flush input data to a byte boundary
	 * at the end of each row.
	 */
	tif->tif_options |= FAX3_NOEOL|FAX3_WORDALIGN;
	return (1);
}
