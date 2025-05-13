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
//  Description:	tif_msdo.cpp
//		MSDOS specific routines
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//


#include "..\..\..\stdafx.h"

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifdef WIN32
#ifndef _WINBASE_
#include <winbase.h>
#endif
#endif

#if defined(__WATCOMC__) || defined(__BORLANDC__)
#ifndef _INC_IO
#include <io.h>		/* for open, close, etc. function prototypes */
#endif
#endif



static tsize_t 
_tiffReadProc(thandle_t fd, tdata_t buf, tsize_t size)
{
UINT test = (UINT)size;
return _lread((HFILE)fd, buf, test);
}

static tsize_t
_tiffWriteProc(thandle_t fd, tdata_t buf, tsize_t size)
{
	return (_lwrite(fd, (const char *)buf, (size_t)size));
}

static toff_t
_tiffSeekProc(thandle_t fd, toff_t off, short whence)
{
	return (_llseek(fd, (off_t) off, whence));
}

static thandle_t
_tiffCloseProc(thandle_t fd)
{
	return (_lclose((int) fd));
}

#ifndef _INC_STAT
#include <sys/stat.h>
#endif

static toff_t
_tiffSizeProc(thandle_t fd)
{

	toff_t size;
	size = (toff_t)_llseek(fd, 0, 1);
	return size;
}

static long
_tiffMapProc(thandle_t fd, tdata_t* pbase, toff_t* psize)
{
psize; //UNUSED
fd;
pbase;
	return (0);
}

static void
_tiffUnmapProc(thandle_t fd, tdata_t base, toff_t size)
{    
//UNUSED 
fd;
base;
size;
}

/*
 * Open a TIFF file descriptor for read/writing.
 */
TIFF*
SECTiff::TIFFFdOpen(thandle_t fd, const TCHAR* name, const TCHAR* mode)
{
	TIFF* tif;

	tif = TIFFClientOpen(name, mode,
	    (thandle_t)fd,
	    _tiffReadProc, 
	    _tiffWriteProc, 
	    _tiffSeekProc, 
	    _tiffCloseProc,
	    _tiffSizeProc, 
	    _tiffMapProc, 
	    _tiffUnmapProc);
	if (tif)
		tif->tif_fd = fd;
	return (tif);
}

/*
 * Open a TIFF file for read/writing.
 */
TIFF*
SECTiff::TIFFOpen(const TCHAR* name, const TCHAR* mode)
{
	static const TCHAR module[] = _T("TIFFOpen");
	UINT m;
	HFILE fd;
	OFSTRUCT myofstruct;

	m = _TIFFgetMode(mode, module);	 
	if (m == 9999)
		return ((TIFF*)0);
	myofstruct; // UNUSED
	fd = NULL;

#ifdef WIN32
	if (m == OF_READ){
		fd = (HFILE)CreateFile(name, 
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
	} else {
		fd = (HFILE)CreateFile(name, 
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
	}
#else
	fd = OpenFile(name, &myofstruct, m);
#endif

	if (fd == HFILE_ERROR) {
		TIFFError(module, _T("%s: Cannot open"), name);
		return ((TIFF*)0);
	}
	return (TIFFFdOpen(fd, name, mode));
}

#ifdef __GNUC__
extern	char *malloc();
extern	char *realloc();
#else

#ifndef _INC_MALLOC
#include <malloc.h>
#endif // _INC_MALLOC

#ifndef _INC_WINDOWSX
#include <windowsx.h> //AAB
#endif // _INC_WINDOWSX

#endif // __GNUC__

LPVOID 
SECTiff::_TIFFmalloc(size_t s)
{
	return (LPVOID)GlobalAllocPtr(GHND, s);
}

void
SECTiff::_TIFFfree(LPVOID p)
{
	GlobalFreePtr(p);
}

LPVOID
SECTiff::_TIFFrealloc(LPVOID p, size_t s)
{
	return (LPVOID)GlobalReAllocPtr(p, s, 0);
}
