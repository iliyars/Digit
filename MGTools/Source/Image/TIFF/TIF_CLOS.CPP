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
//  Description:	tif_clos.cpp
//		file i/o support routines
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
 
#ifndef _INC_MALLOC
#include "malloc.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

void
SECTiff::TIFFClose(TIFF* tif)
{
	if (tif->tif_mode != O_RDONLY)
		/*
		 * Flush buffered data and directory (if dirty).
		 */
		TIFFFlush(tif);
	if (tif->tif_cleanup)
	{
		m_fpVoid = tif->tif_cleanup;
		(this->*m_fpVoid)(tif);
	}
		//orig (*tif->tif_cleanup)(tif);
	TIFFFreeDirectory(tif);
	if (tif->tif_rawdata && (tif->tif_flags&TIFF_MYBUFFER))
		_TIFFfree(tif->tif_rawdata);
	if (isMapped(tif))
		TIFFUnmapFileContents(tif, tif->tif_base, tif->tif_size);
	(void) TIFFCloseFile(tif);
	//AAB _TIFFfree((char *)tif);  
	free(tif);
}
