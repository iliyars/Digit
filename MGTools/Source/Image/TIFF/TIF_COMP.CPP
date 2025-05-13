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
//  Description:	tif_comp.cpp
//		compression routines
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//

/*
 * TIFF Library
 *
 * Compression Scheme Configuration Support.
 */
#include "..\..\..\stdafx.h"

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

static const cscheme_t CompressionSchemes[] = {
    { _T("Null"),		COMPRESSION_NONE,	&SECTiff::TIFFInitDumpMode },
//AAB #ifdef LZW_SUPPORT
    { _T("LZW"),		COMPRESSION_LZW,	&SECTiff::TIFFInitLZW },
//AAB #endif
#ifdef PACKBITS_SUPPORT
    { _T("PackBits"),	COMPRESSION_PACKBITS,	&SECTiff::TIFFInitPackBits },
#endif
//AAB #ifdef THUNDER_SUPPORT
    { _T("ThunderScan"),	COMPRESSION_THUNDERSCAN,&SECTiff::TIFFInitThunderScan },
//AAB #endif
#ifdef NEXT_SUPPORT_AAB_NOTSUPPORTED
    { _T("NeXT"),		COMPRESSION_NEXT,	&SECTiff::TIFFInitNeXT },
#endif
#ifdef JPEG_SUPPORT
    { _T("JPEG"),		COMPRESSION_JPEG,	&SECTiff::TIFFInitJPEG },
#endif
//AABTEST #ifdef WIN32 //AAB for now due to huge data segs
    { _T("CCITT RLE"),	COMPRESSION_CCITTRLE,	&SECTiff::TIFFInitCCITTRLE },
    { _T("CCITT RLE/W"),	COMPRESSION_CCITTRLEW,	&SECTiff::TIFFInitCCITTRLEW },
    { _T("CCITT Group3"),	COMPRESSION_CCITTFAX3,	&SECTiff::TIFFInitCCITTFax3 },
    { _T("CCITT Group4"),	COMPRESSION_CCITTFAX4,	&SECTiff::TIFFInitCCITTFax4 },
//AABTEST #endif /*Win32*/
// AAB #endif
};
#define	NSCHEMES (sizeof (CompressionSchemes) / sizeof (CompressionSchemes[0]))

//AAB static const cscheme_t *
const cscheme_t *
SECTiff::compfindScheme(long scheme)
{
	register const cscheme_t *c;

	for (c = CompressionSchemes; c < &CompressionSchemes[NSCHEMES]; c++)
	{
		if (c->scheme == scheme)
			return (c);
	}
	return ((const cscheme_t *)0);
}

long
SECTiff::TIFFNoEncode(TIFF* tif, TCHAR* method)
{
	const cscheme_t *c = compfindScheme(tif->tif_dir.td_compression);
	TIFFError(tif->tif_name,
	    _T("%s %s encoding is not implemented"), c->name, method);
	return (-1);
}

long
SECTiff::TIFFNoRowEncode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{   
s; //UNUSED
cc;
pp;
	return (TIFFNoEncode(tif, _T("scanline")));      
	
}

long
SECTiff::TIFFNoStripEncode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{
	   
	s; //UNUSED
	pp;
	cc;   
	
	return (TIFFNoEncode(tif, _T("strip")));
}

long
SECTiff::TIFFNoTileEncode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{                          
pp; //UNUSED
cc;
s;
	return (TIFFNoEncode(tif, _T("tile")));
}

long
SECTiff::TIFFNoDecode(TIFF* tif, TCHAR* method)
{
	const cscheme_t *c = compfindScheme(tif->tif_dir.td_compression);
	TIFFError(tif->tif_name,
	    _T("%s %s decoding is not implemented"), c->name, method);
	return (-1);
}

long
SECTiff::TIFFNoRowDecode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{   
pp; //UNUSED
cc;
s;
	return (TIFFNoDecode(tif, _T("scanline")));
}

long
SECTiff::TIFFNoStripDecode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{ 
pp; //UNUSED
cc;
s;
	return (TIFFNoDecode(tif, _T("strip")));
}

long
SECTiff::TIFFNoTileDecode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{
pp; //UNUSED
cc;
s;
	return (TIFFNoDecode(tif, _T("tile")));
}

long
SECTiff::TIFFSetCompressionScheme(TIFF* tif, long scheme)
{
	const cscheme_t *c = compfindScheme(scheme);

	if (!c) {
		TIFFError(tif->tif_name,
		    _T("Unknown data compression algorithm %u (0x%x)"),
		    scheme, scheme);
		return (0);
	}
	tif->tif_predecode = NULL;
	tif->tif_decoderow = &SECTiff::TIFFNoRowDecode;
	tif->tif_decodestrip = &SECTiff::TIFFNoStripDecode;
	tif->tif_decodetile = &SECTiff::TIFFNoTileDecode;
	tif->tif_preencode = NULL;
	tif->tif_postencode = NULL;
	tif->tif_encoderow = &SECTiff::TIFFNoRowEncode;
	tif->tif_encodestrip = &SECTiff::TIFFNoStripEncode;
	tif->tif_encodetile = &SECTiff::TIFFNoTileEncode;
	tif->tif_close = NULL;
	tif->tif_seek = NULL;
	tif->tif_cleanup = NULL;
	tif->tif_flags &= ~TIFF_NOBITREV;
	tif->tif_options = 0;
	m_fpBool = c->init;
	return (this->*m_fpBool)(tif);
	// return ((*c->init)(tif));
}
