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
//  Description:	tif_open.cpp
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

#ifndef _INC_MALLOC
#include <malloc.h>
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif


//#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

static const long typemask[13] = {
	0L,		/* TIFF_NOTYPE */
	0x000000ffL,	/* TIFF_BYTE */
	0xffffffffL,	/* TIFF_ASCII */
	0x0000ffffL,	/* TIFF_SHORT */
	0xffffffffL,	/* TIFF_LONG */
	0xffffffffL,	/* TIFF_RATIONAL */
	0x000000ffL,	/* TIFF_SBYTE */
	0x000000ffL,	/* TIFF_UNDEFINED */
	0x0000ffffL,	/* TIFF_SSHORT */
	0xffffffffL,	/* TIFF_SLONG */
	0xffffffffL,	/* TIFF_SRATIONAL */
	0xffffffffL,	/* TIFF_FLOAT */
	0xffffffffL,	/* TIFF_DOUBLE */
};
static const long bigTypeshift[13] = {
	0,		/* TIFF_NOTYPE */
	24,		/* TIFF_BYTE */
	0,		/* TIFF_ASCII */
	16,		/* TIFF_SHORT */
	0,		/* TIFF_LONG */
	0,		/* TIFF_RATIONAL */
	16,		/* TIFF_SBYTE */
	16,		/* TIFF_UNDEFINED */
	24,		/* TIFF_SSHORT */
	0,		/* TIFF_SLONG */
	0,		/* TIFF_SRATIONAL */
	0,		/* TIFF_FLOAT */
	0,		/* TIFF_DOUBLE */
};
static const long litTypeshift[13] = {
	0,		/* TIFF_NOTYPE */
	0,		/* TIFF_BYTE */
	0,		/* TIFF_ASCII */
	0,		/* TIFF_SHORT */
	0,		/* TIFF_LONG */
	0,		/* TIFF_RATIONAL */
	0,		/* TIFF_SBYTE */
	0,		/* TIFF_UNDEFINED */
	0,		/* TIFF_SSHORT */
	0,		/* TIFF_SLONG */
	0,		/* TIFF_SRATIONAL */
	0,		/* TIFF_FLOAT */
	0,		/* TIFF_DOUBLE */
};

/*
 * Initialize the bit fill order, the
 * shift & mask tables, and the byte
 * swapping state according to the file
 * contents and the machine architecture.
 */
void
SECTiff::TIFFInitOrder(register TIFF* tif, long magic, long bigendian)
{
	/* XXX how can we deduce this dynamically? */
	tif->tif_fillorder = FILLORDER_MSB2LSB;

	tif->tif_typemask = typemask;
	if (magic == TIFF_BIGENDIAN) {
		tif->tif_typeshift = bigTypeshift;
		if (!bigendian)
			tif->tif_flags |= TIFF_SWAB;
	} else {
		tif->tif_typeshift = litTypeshift;
		if (bigendian)
			tif->tif_flags |= TIFF_SWAB;
	}
}

UINT
SECTiff::_TIFFgetMode(const TCHAR* mode, const TCHAR* module)
{
	UINT m = 9999;

	switch (mode[0]) {
	case 'r':
		//m = O_RDONLY;
		m = OF_READ;
		if (mode[1] == '+')
			m = O_RDWR;
		break;
	case 'w':
	case 'a':
		m = OF_READWRITE|OF_CREATE;
		/***
		if (mode[0] == 'w')
			m |= O_TRUNC;
		***/
		break;
	default:
		TIFFError(module, _T("\"%s\": Bad mode"), mode);
		break;
	}
	return (m);
}

TIFF*
SECTiff::TIFFClientOpen(
	const TCHAR* name, const TCHAR* mode,
	thandle_t clientdata,
	TIFFReadWriteProc readproc,
	TIFFReadWriteProc writeproc,
	TIFFSeekProc seekproc,
	TIFFCloseProc closeproc,
	TIFFSizeProc sizeproc,
	TIFFMapFileProc mapproc,
	TIFFUnmapFileProc unmapproc
)
{
	static const TCHAR module[] = _T("TIFFClientOpen");
	TIFF *tif;
	long m, bigendian;
	size_t tLenTif = sizeof(TIFF) + (_tcslen(name) + 1) * sizeof(TCHAR);

	m = _TIFFgetMode(mode, module);
	if (m == -1)
		goto bad2;

	tif = (TIFF *)malloc(tLenTif);
	if (tif == NULL) {
		TIFFError(module, _T("%s: Out of memory (TIFF structure)"), name);
		goto bad2;
	}
	memset(tif, 0, tLenTif);
	tif->tif_name = (TCHAR *)((BYTE *)tif + sizeof (TIFF));
	_tcscpy(tif->tif_name, name);
	tif->tif_mode = (short)(m &~ (O_CREAT|O_TRUNC));
	tif->tif_curdir = -1;		/* non-existent directory */
	tif->tif_curoff = 0;
	tif->tif_curstrip = -1;		/* invalid strip */
	tif->tif_row = -1;		/* read/write pre-increment */
	tif->tif_clientdata = clientdata;
	tif->tif_readproc = readproc;
	tif->tif_writeproc = writeproc;
	tif->tif_seekproc = seekproc;
	tif->tif_closeproc = closeproc;
	tif->tif_sizeproc = sizeproc;
	tif->tif_mapproc = mapproc;
	tif->tif_unmapproc = unmapproc;

	{ long one = 1; char* cp = (char*)&one; bigendian = (*cp == 0); }
	/*
	 * Read in TIFF header.
	 */
	if (!ReadOK(tif, &tif->tif_header, sizeof (TIFFHeader))) {
		if (tif->tif_mode == O_RDONLY) {
			TIFFError(name, _T("Cannot read TIFF header"));
			goto bad;
		}
		/*
		 * Setup header and write.
		 */
		tif->tif_header.tiff_magic =  (WORD)(bigendian ?
		    TIFF_BIGENDIAN : TIFF_LITTLEENDIAN);
		tif->tif_header.tiff_version = TIFF_VERSION;
		tif->tif_header.tiff_diroff = 0;	/* filled in later */
		if (!WriteOK(tif, &tif->tif_header, sizeof (TIFFHeader))) {
			TIFFError(name, _T("Error writing TIFF header"));
			goto bad;
		}
		/*
		 * Setup the byte order handling.
		 */
		TIFFInitOrder(tif, tif->tif_header.tiff_magic, bigendian);
		/*
		 * Setup default directory.
		 */
		if (!TIFFDefaultDirectory(tif))
			goto bad;
		tif->tif_diroff = 0;
		return (tif);
	}
	/*
	 * Setup the byte order handling.
	 */
	if (tif->tif_header.tiff_magic != TIFF_BIGENDIAN &&
	    tif->tif_header.tiff_magic != TIFF_LITTLEENDIAN) {
		TIFFError(name,  _T("Not a TIFF file, bad magic number %d (0x%x)"),
		    tif->tif_header.tiff_magic,
		    tif->tif_header.tiff_magic);
		goto bad;
	}
	TIFFInitOrder(tif, tif->tif_header.tiff_magic, bigendian);
	/*
	 * Swap header if required.
	 */
	if (tif->tif_flags & TIFF_SWAB) {
		TIFFSwabShort(&tif->tif_header.tiff_version);
		TIFFSwabLong(&tif->tif_header.tiff_diroff);
	}
	/*
	 * Now check version (if needed, it's been byte-swapped).
	 * Note that this isn't actually a version number, it's a
	 * magic number that doesn't change (stupid).
	 */
	if (tif->tif_header.tiff_version != TIFF_VERSION) {
		TIFFError(name,
		    _T("Not a TIFF file, bad version number %d (0x%x)"),
		    tif->tif_header.tiff_version,
		    tif->tif_header.tiff_version); 
		goto bad;
	}
	tif->tif_flags |= TIFF_MYBUFFER;
	tif->tif_rawcp = tif->tif_rawdata = 0;
	tif->tif_rawdatasize = 0;
	/*
	 * Setup initial directory.
	 */
	switch (mode[0]) {
	case 'r':
		tif->tif_nextdiroff = tif->tif_header.tiff_diroff;
		if (TIFFReadDirectory(tif)) {
			tif->tif_rawcc = -1;
			tif->tif_flags |= TIFF_BUFFERSETUP;
			return (tif);
		}
		break;
	case 'a':
		/*
		 * Don't append to file that has information
		 * byte swapped -- we will write data that is
		 * in the opposite order.
		 */
		if (tif->tif_flags & TIFF_SWAB) {
			TIFFError(name,
		_T("Cannot append to file that has opposite byte ordering"));
			goto bad;
		}
		/*
		 * New directories are automatically append
		 * to the end of the directory chain when they
		 * are written out (see TIFFWriteDirectory).
		 */
		if (!TIFFDefaultDirectory(tif))
			goto bad;
		return (tif);
	}
bad:
	tif->tif_mode = O_RDONLY;	/* XXX avoid flush */
	TIFFClose(tif);
	return ((TIFF*)0);
bad2:
	(void) (*closeproc)(clientdata);
	return ((TIFF*)0);
}

tsize_t
SECTiff::TIFFScanlineSize(TIFF* tif)
{
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t scanline;
	
	scanline = td->td_bitspersample * td->td_imagewidth;
	if (td->td_planarconfig == PLANARCONFIG_CONTIG)
		scanline *= td->td_samplesperpixel;
	return (howmany(scanline, 8));
}

/*
 * Query functions to access private data.
 */

/*
 * Return open file's name.
 */
const TCHAR *
SECTiff::TIFFFileName(TIFF* tif)
{
	return (tif->tif_name);
}

/*
 * Return open file's I/O descriptor.
 */
long
SECTiff::TIFFFileno(TIFF* tif)
{
	return (tif->tif_fd);
}

/*
 * Return read/write mode.
 */
UINT
SECTiff::TIFFGetMode(TIFF* tif)
{
	return (tif->tif_mode);
}

/*
 * Return nonzero if file is organized in
 * tiles; zero if organized as strips.
 */
long
SECTiff::TIFFIsTiled(TIFF* tif)
{
	return (isTiled(tif));
}

/*
 * Return current row being read/written.
 */
DWORD
SECTiff::TIFFCurrentRow(TIFF* tif)
{
	return (tif->tif_row);
}

/*
 * Return index of the current directory.
 */
tdir_t
SECTiff::TIFFCurrentDirectory(TIFF* tif)
{
	return (tif->tif_curdir);
}

/*
 * Return current strip.
 */
tstrip_t
SECTiff::TIFFCurrentStrip(TIFF* tif)
{
	return (tif->tif_curstrip);
}

/*
 * Return current tile.
 */
ttile_t
SECTiff::TIFFCurrentTile(TIFF* tif)
{
	return (tif->tif_curtile);
}

/*
 * Return nonzero if the file has byte-swapped data.
 */
long
SECTiff::TIFFIsByteSwapped(TIFF* tif)
{
	return ((tif->tif_flags & TIFF_SWAB) != 0);
}
