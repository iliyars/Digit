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
//  Description:	tif_erro.cpp
//		error handling
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

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

static void
defaultHandler(const TCHAR* module, const TCHAR* fmt, va_list ap)
{

#ifdef WIN32
	if (module != NULL)
		_ftprintf(stderr, _T("%s: "), module);
	_vftprintf(stderr, fmt, ap);
	_ftprintf(stderr, _T(".\n"));
#else 
module; //UNUSED
fmt;
ap;
#endif
}

static TIFFErrorHandler _errorHandler = defaultHandler;

TIFFErrorHandler
SECTiff::TIFFSetErrorHandler(TIFFErrorHandler handler)
{
	TIFFErrorHandler prev = _errorHandler;
	_errorHandler = handler;
	return (prev);
}

void
SECTiff::TIFFError(const TCHAR* module, const TCHAR* fmt, ...)
{
	if (_errorHandler) {
		va_list ap;
		va_start(ap, fmt);
		(*_errorHandler)(module, fmt, ap);
		va_end(ap);
	}
}
