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
//  Description:	tif_dump.cpp
//		"Null" compression algorithm support
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
 * Encode a hunk of pixels.
 */
int
SECTiff::DumpModeEncode(TIFF* tif, tidata_t pp, tsize_t cc, tsample_t s)
{
	while (cc > 0) {
		tsize_t n;

		n = cc;
		if (tif->tif_rawcc + n > tif->tif_rawdatasize)
			n = tif->tif_rawdatasize - tif->tif_rawcc;
		/*
		 * Avoid copy if client has setup raw
		 * data buffer to avoid extra copy.
		 */
		if (tif->tif_rawcp != pp)
			switch(m_nSrcBitsPerPixel) {
			case 24:
			case 8:
			case 1:
				    
#ifndef WIN32
			_fmemcpy(tif->tif_rawcp, pp, (size_t)n);
#else
			memcpy(tif->tif_rawcp, pp, (size_t)n);
#endif
			break;

		case 4:
			int check;
			BYTE *pCheck = (BYTE *)pp;
			m_nBitHolder = 0;
			for (check=0;check<n; check++)
			{
				NEXT_IMAGE_PIXEL(*(tif->tif_rawcp), pCheck);
				tif->tif_rawcp++;
			}
			break;
		}

		switch (m_nSrcBitsPerPixel) {
		case 24:
		case 8:
		case 1:
			tif->tif_rawcp += n;
			tif->tif_rawcc += n;
			pp += n;
			break;
		case 4:
			tif->tif_rawcp += n/2;
			tif->tif_rawcc += n/2;
			pp += n/2;
			break;
			break;
		}	 
		cc -= n;

		if (tif->tif_rawcc >= tif->tif_rawdatasize &&
		    !TIFFFlushData1(tif))
			return (-1);
	}
	return (1);
s; //UNUSED
}

/*
 * Decode a hunk of pixels.
 */
int
SECTiff::DumpModeDecode(TIFF* tif, tidata_t buf, tsize_t cc, tsample_t s)
{
	if (tif->tif_rawcc < cc) {
		TIFFError(tif->tif_name,
		    _T("DumpModeDecode: Not enough data for scanline %d"),
		    tif->tif_row);
		return (0);
	}
	/*
	 * Avoid copy if client has setup raw
	 * data buffer to avoid extra copy.
	 */
	if (tif->tif_rawcp != buf)
#ifdef WIN32
		memcpy(buf, tif->tif_rawcp, (size_t)cc);
#else
		_fmemcpy(buf, tif->tif_rawcp, (size_t)cc);
#endif
	tif->tif_rawcp += cc;
	tif->tif_rawcc -= cc;
	return (1);
	s; //UNUSED
}

/*
 * Seek forwards nrows in the current strip.
 */
int
SECTiff::DumpModeSeek(TIFF* tif, DWORD nrows)
{
	tif->tif_rawcp += nrows * tif->tif_scanlinesize;
	tif->tif_rawcc -= nrows * tif->tif_scanlinesize;
	return (1);
}

/*
 * Initialize dump mode.
 */
long
SECTiff::TIFFInitDumpMode(TIFF* tif)
{
	tif->tif_decoderow = (TIFFCodeMethod) &SECTiff::DumpModeDecode;
	tif->tif_decodestrip = (TIFFCodeMethod) &SECTiff::DumpModeDecode;
	tif->tif_decodetile = (TIFFCodeMethod) &SECTiff::DumpModeDecode;
	tif->tif_encoderow = (TIFFCodeMethod) &SECTiff::DumpModeEncode;
	tif->tif_encodestrip = (TIFFCodeMethod) &SECTiff::DumpModeEncode;
	tif->tif_encodetile = (TIFFCodeMethod) &SECTiff::DumpModeEncode;
	tif->tif_seek = (TIFFSeekMethod)&SECTiff::DumpModeSeek;
	return (1);
}
