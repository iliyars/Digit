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
//  Description:	tif_flus.cpp
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

long 
SECTiff::TIFFFlush(TIFF* tif)
{

	if (tif->tif_mode != O_RDONLY) {
		if (!TIFFFlushData(tif))
			return (0);
		if ((tif->tif_flags & TIFF_DIRTYDIRECT) &&
		    !TIFFWriteDirectory(tif))
			return (0);
	}
	return (1);
}

/*
 * Flush buffered data to the file.
 */
long
SECTiff::TIFFFlushData(TIFF* tif)
{
	if ((tif->tif_flags & TIFF_BEENWRITING) == 0)
		return (0);
	if (tif->tif_flags & TIFF_POSTENCODE) {
		tif->tif_flags &= ~TIFF_POSTENCODE;
		m_fpBool = tif->tif_postencode;
		// if (tif->tif_postencode && !(*tif->tif_postencode)(tif))
		if (m_fpBool && !(this->*m_fpBool)(tif))
			return (0);
	}
	return (TIFFFlushData1(tif));
}
