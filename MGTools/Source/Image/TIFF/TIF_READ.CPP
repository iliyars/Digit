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
//  Description:	tif_read.cpp
//		scanline oriented read support
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
 * Scanline-oriented Read Support
 */
#include "..\..\..\stdafx.h"

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

#include <assert.h>

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

//#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

static	int TIFFFillStrip(TIFF*, tstrip_t);
static	int TIFFFillTile(TIFF*, ttile_t);
static	int TIFFStartStrip(TIFF*, tstrip_t);
static	int TIFFStartTile(TIFF*, ttile_t);
static	int TIFFCheckRead(TIFF*, int);

/*
 * Seek to a random row+sample in a file.
 */
int
SECTiff::TIFFSeek(TIFF* tif, DWORD row, tsample_t sample)
{
	register TIFFDirectory *td = &tif->tif_dir;
	tstrip_t strip;

	if (row >= td->td_imagelength) {	/* out of range */
		TIFFError(tif->tif_name, _T("%lu: Row out of range, max %lu"),
		    (DWORD) row, (DWORD) td->td_imagelength);
		return (0);
	}
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
		if (sample >= td->td_samplesperpixel) {
			TIFFError(tif->tif_name,
			    _T("%lu: Sample out of range, max %lu"),
			    (DWORD) sample, (DWORD) td->td_samplesperpixel);
			return (0);
		}
		strip = sample*td->td_stripsperimage + row/td->td_rowsperstrip;
	} else
		strip = row / td->td_rowsperstrip;
	if (strip != tif->tif_curstrip) { 	/* different strip, refill */
		if (!TIFFFillStrip(tif, strip))
			return (0);
	} else if (row < (DWORD)tif->tif_row) {
		/*
		 * Moving backwards within the same strip: backup
		 * to the start and then decode forward (below).
		 *
		 * NB: If you're planning on lots of random access within a
		 * strip, it's better to just read and decode the entire
		 * strip, and then access the decoded data in a random fashion.
		 */
		if (!TIFFStartStrip(tif, strip))
			return (0);
	}
	if (row != (DWORD)tif->tif_row) {
		if (tif->tif_seek) {
			/*
			 * Seek forward to the desired row.
			 */
			m_fpSeek=tif->tif_seek;
			//AAB if (!(*tif->tif_seek)(tif, row - tif->tif_row))
			if (!(this->*m_fpSeek)(tif, row - tif->tif_row))
				return (0);
			tif->tif_row = row;
		} else {
			TIFFError(tif->tif_name,
		    _T("Compression algorithm does not support random access"));
			return (0);
		}
	}
	return (1);
}

long
SECTiff::TIFFReadScanline(TIFF* tif, tdata_t buf, DWORD row, tsample_t sample)
{
	long e;

	if (!TIFFCheckRead(tif, 0))
		return (-1);
	e = TIFFSeek(tif, row, sample);
	if (e) {
		/*
		 * Decompress desired row into user buffer.
		 */
		m_fpCode = tif->tif_decoderow;
		e = (this->*m_fpCode)
		    (tif, (tidata_t)buf, (tsize_t)tif->tif_scanlinesize, sample);
		tif->tif_row++;
		if (e)
		{
			m_fpPost = tif->tif_postdecode;
			(this->*m_fpPost)(tif, (tidata_t)buf, (tsize_t)tif->tif_scanlinesize);
		}
	}
	return (e ? 1 : -1);
}

/*
 * Read a strip of data and decompress the specified
 * amount into the user-supplied buffer.
 */
tsize_t
SECTiff::TIFFReadEncodedStrip(TIFF* tif, tstrip_t strip, tdata_t buf, tsize_t size)
{
	TIFFDirectory *td = &tif->tif_dir;
	DWORD nrows;
	tsize_t stripsize;

	if (!TIFFCheckRead(tif, 0))
		return (-1);
	if (strip >= td->td_nstrips) {
		TIFFError(tif->tif_name, _T("%ld: Strip out of range, max %ld"),
		    (long) strip, (long) td->td_nstrips);
		return (-1);
	}
	/*
	 * Calculate the strip size according to the number of
	 * rows in the strip (check for truncated last strip).
	 */
	if (strip != td->td_nstrips-1 ||
	    (nrows = td->td_imagelength % td->td_rowsperstrip) == 0)
		nrows = td->td_rowsperstrip;
	stripsize = TIFFVStripSize(tif, nrows);
	if (size == (tsize_t) -1)
		size = stripsize;
	else if (size > stripsize)
		size = stripsize;
	m_fpPost = tif->tif_postdecode;
	m_fpCode = tif->tif_decodestrip;
	if (TIFFFillStrip(tif, strip) &&
    (this->*m_fpCode)(tif, (tidata_t)buf, (tsize_t)size, (tsample_t)(strip / td->td_stripsperimage))) {
		(this->*m_fpPost)(tif, (tidata_t)buf, (tsize_t)size);
		return (size);
	} else
		return ((tsize_t) -1);
}

tsize_t
SECTiff::TIFFReadRawStrip1(TIFF* tif,
    tstrip_t strip, tdata_t buf, tsize_t size, const TCHAR* module)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (!isMapped(tif)) {
		if (!SeekOK(tif, td->td_stripoffset[strip])) {
			TIFFError(module,
			    _T("%s: Seek error at scanline %lu, strip %lu"),
			    tif->tif_name,
			    (DWORD) tif->tif_row, (DWORD) strip);
			return (-1);
		}
		if (!ReadOK(tif, buf, size)) {
			TIFFError(module, _T("%s: Read error at scanline %lu"),
			    tif->tif_name, (DWORD) tif->tif_row);
			return (-1);
		}
	} else {
		if ((tsize_t)td->td_stripoffset[strip] + size > (tsize_t)tif->tif_size) {
			TIFFError(module,
			    _T("%s: Seek error at scanline %lu, strip %lu"),
			    tif->tif_name,
			    (DWORD) tif->tif_row, (DWORD) strip);
			return (-1);
		}
#ifdef WIN32
		memcpy(buf, tif->tif_base+td->td_stripoffset[strip], (size_t)size);
#else
		_fmemcpy(buf, tif->tif_base + td->td_stripoffset[strip], (size_t)size);
#endif
	}
	return (size);
}

/*
 * Read a strip of data from the file.
 */
tsize_t
SECTiff::TIFFReadRawStrip(TIFF* tif, tstrip_t strip, tdata_t buf, tsize_t size)
{
	static const TCHAR module[] = _T("TIFFReadRawStrip");
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t bytecount;

	if (!TIFFCheckRead(tif, 0))
		return (-1);
	if (strip >= td->td_nstrips) {
		TIFFError(tif->tif_name, _T("%lu: Strip out of range, max %lu"),
		    (DWORD) strip, (DWORD) td->td_nstrips);
		return (-1);
	}
	bytecount = td->td_stripbytecount[strip];
	if (size != (tsize_t)-1 && size < bytecount)
		bytecount = size;
	return (TIFFReadRawStrip1(tif, strip, buf, bytecount, module));
}

/*
 * Read the specified strip and setup for decoding. 
 * The data buffer is expanded, as necessary, to
 * hold the strip's data.
 */
int
SECTiff::TIFFFillStrip(TIFF* tif, tstrip_t strip)
{
	static const TCHAR module[] = _T("TIFFFillStrip");
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t bytecount;

	bytecount = td->td_stripbytecount[strip];
	if (isMapped(tif) &&
	    (td->td_fillorder == (WORD)tif->tif_fillorder || (tif->tif_flags & TIFF_NOBITREV))) {
		/*
		 * The image is mapped into memory and we either don't
		 * need to flip bits or the compression routine is going
		 * to handle this operation itself.  In this case, avoid
		 * copying the raw data and instead just reference the
		 * data from the memory mapped file image.  This assumes
		 * that the decompression routines do not modify the
		 * contents of the raw data buffer (if they try to,
		 * the application will get a fault since the file is
		 * mapped read-only).
		 */
		if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata)
			_TIFFfree(tif->tif_rawdata);
		tif->tif_flags &= ~TIFF_MYBUFFER;
		if ((tsize_t)td->td_stripoffset[strip] + bytecount > (tsize_t)tif->tif_size) {
			/*
			 * This error message might seem strange, but it's
			 * what would happen if a read were done instead.
			 */
			TIFFError(module, _T("%s: Read error on strip %lu"),
			    tif->tif_name, (DWORD) strip);
			tif->tif_curstrip = -1;		/* unknown state */
			return (0);
		}
		tif->tif_rawdatasize = bytecount;
		tif->tif_rawdata = tif->tif_base + td->td_stripoffset[strip];
	} else {
		/*
		 * Expand raw data buffer, if needed, to
		 * hold data strip coming from file
		 * (perhaps should set upper bound on
		 *  the size of a buffer we'll use?).
		 */
		if (bytecount > tif->tif_rawdatasize) {
			tif->tif_curstrip = -1;		/* unknown state */
			if ((tif->tif_flags & TIFF_MYBUFFER) == 0) {
				TIFFError(module,
				_T("%s: Data buffer too small to hold strip %lu"),
				    tif->tif_name, (DWORD) strip);
				return (0);
			}
			if (!TIFFReadBufferSetup(tif, 0,
			    roundup(bytecount, 1024)))
				return (0);
		}
		if (TIFFReadRawStrip1(tif, strip, (LPBYTE)tif->tif_rawdata,
		    bytecount, module) != bytecount)
			return (0);
		if (td->td_fillorder != (WORD)tif->tif_fillorder &&
		    (tif->tif_flags & TIFF_NOBITREV) == 0)
			TIFFReverseBits((LPBYTE)tif->tif_rawdata, bytecount);
	}
	return (TIFFStartStrip(tif, strip));
}

/*
 * Tile-oriented Read Support
 * Contributed by Nancy Cam (Silicon Graphics).
 */

/*
 * Read and decompress a tile of data.  The
 * tile is selected by the (x,y,z,s) coordinates.
 */
tsize_t
SECTiff::TIFFReadTile(TIFF* tif,
    tdata_t buf, DWORD x, DWORD y, DWORD z, tsample_t s)
{
	if (!TIFFCheckRead(tif, 1) || !TIFFCheckTile(tif, x, y, z, s))
		return (-1);
	return (TIFFReadEncodedTile(tif,
	    TIFFComputeTile(tif, x, y, z, s), buf, (tsize_t) -1));
}

/*
 * Read a tile of data and decompress the specified
 * amount into the user-supplied buffer.
 */
tsize_t
SECTiff::TIFFReadEncodedTile(TIFF* tif, ttile_t tile, tdata_t buf, tsize_t size)
{
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t tilesize = tif->tif_tilesize;

	if (!TIFFCheckRead(tif, 1))
		return (-1);
	if (tile >= (ttile_t)td->td_nstrips) {
		TIFFError(tif->tif_name, _T("%ld: Tile out of range, max %ld"),
		    (long) tile, (DWORD) td->td_nstrips);
		return (-1);
	}
	if (size == (tsize_t) -1)
		size = tilesize;
	else if (size > tilesize)
		size = tilesize;
	m_fpCode = tif->tif_decodetile;
	m_fpPost = tif->tif_postdecode;
	if (TIFFFillTile(tif, tile) && 
	    (this->*m_fpCode)(tif, (tidata_t)buf, (tsize_t)size, (tsample_t)(tile/td->td_stripsperimage))) {
		(this->*m_fpPost)(tif, (tidata_t)buf, (tsize_t)size);
		return (size);
	} else
		return (-1);
}

tsize_t
SECTiff::TIFFReadRawTile1(TIFF* tif,
    ttile_t tile, tdata_t buf, tsize_t size, const TCHAR* module)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (!isMapped(tif)) {
		if (!SeekOK(tif, td->td_stripoffset[tile])) {
			TIFFError(module,
			    _T("%s: Seek error at row %ld, col %ld, tile %ld"),
			    tif->tif_name,
			    (long) tif->tif_row,
			    (long) tif->tif_col,
			    (long) tile);
			return ((tsize_t) -1);
		}
		if (!ReadOK(tif, buf, size)) {
			TIFFError(module, _T("%s: Read error at row %ld, col %ld"),
			    tif->tif_name,
			    (long) tif->tif_row,
			    (long) tif->tif_col);
			return ((tsize_t) -1);
		}
	} else {
		if ((tsize_t)td->td_stripoffset[tile] + size > tif->tif_size) {
			TIFFError(module,
			    _T("%s: Seek error at row %ld, col %ld, tile %ld"),
			    tif->tif_name,
			    (long) tif->tif_row,
			    (long) tif->tif_col,
			    (long) tile);
			return ((tsize_t) -1);
		}
#ifdef WIN32
		memcpy(buf, tif->tif_base+td->td_stripoffset[tile], (size_t)size);
#else
		_fmemcpy(buf, tif->tif_base + td->td_stripoffset[tile], (size_t)size);
#endif
	}
	return (size);
}

/*
 * Read a tile of data from the file.
 */
tsize_t
SECTiff::TIFFReadRawTile(TIFF* tif, ttile_t tile, tdata_t buf, tsize_t size)
{
	static const TCHAR module[] = _T("TIFFReadRawTile");
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t bytecount;

	if (!TIFFCheckRead(tif, 1))
		return ((tsize_t) -1);
	if (tile >= (ttile_t)td->td_nstrips) {
		TIFFError(tif->tif_name, _T("%lu: Tile out of range, max %lu"),
		    (DWORD) tile, (DWORD) td->td_nstrips);
		return ((tsize_t) -1);
	}
	bytecount = td->td_stripbytecount[tile];
	if (size != (tsize_t) -1 && size < bytecount)
		bytecount = size;
	return (TIFFReadRawTile1(tif, tile, buf, bytecount, module));
}

/*
 * Read the specified tile and setup for decoding. 
 * The data buffer is expanded, as necessary, to
 * hold the tile's data.
 */
int
SECTiff::TIFFFillTile(TIFF* tif, ttile_t tile)
{
	static const TCHAR module[] = _T("TIFFFillTile");
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t bytecount;

	bytecount = td->td_stripbytecount[tile];
	if (isMapped(tif) &&
	    (td->td_fillorder == (WORD)tif->tif_fillorder || (tif->tif_flags & TIFF_NOBITREV))) {
		/*
		 * The image is mapped into memory and we either don't
		 * need to flip bits or the compression routine is going
		 * to handle this operation itself.  In this case, avoid
		 * copying the raw data and instead just reference the
		 * data from the memory mapped file image.  This assumes
		 * that the decompression routines do not modify the
		 * contents of the raw data buffer (if they try to,
		 * the application will get a fault since the file is
		 * mapped read-only).
		 */
		if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata)
			_TIFFfree(tif->tif_rawdata);
		tif->tif_flags &= ~TIFF_MYBUFFER;
		if ((tsize_t)td->td_stripoffset[tile] + bytecount > tif->tif_size) {
			tif->tif_curtile = (ttile_t)-1;		/* AAB : their terrible cast... FUTURE: Need better method here*/
			return (0);
		}
		tif->tif_rawdatasize = bytecount;
		tif->tif_rawdata = tif->tif_base + td->td_stripoffset[tile];
	} else {
		/*
		 * Expand raw data buffer, if needed, to
		 * hold data tile coming from file
		 * (perhaps should set upper bound on
		 *  the size of a buffer we'll use?).
		 */
		if (bytecount > tif->tif_rawdatasize) {
			tif->tif_curtile = (ttile_t)-1;		/* unknown state */
			if ((tif->tif_flags & TIFF_MYBUFFER) == 0) {
				TIFFError(module,
				_T("%s: Data buffer too small to hold tile %ld"),
				    tif->tif_name, (long) tile);
				return (0);
			}
			if (!TIFFReadBufferSetup(tif, 0,
			    roundup(bytecount, 1024)))
				return (0);
		}
		if (TIFFReadRawTile1(tif, tile, (LPBYTE)tif->tif_rawdata,
		    bytecount, module) != bytecount)
			return (0);
		if (td->td_fillorder != (WORD)tif->tif_fillorder &&
		    (tif->tif_flags & TIFF_NOBITREV) == 0)
			TIFFReverseBits((LPBYTE)tif->tif_rawdata, bytecount);
	}
	return (TIFFStartTile(tif, tile));
}

/*
 * Setup the raw data buffer in preparation for
 * reading a strip of raw data.  If the buffer
 * is specified as zero, then a buffer of appropriate
 * size is allocated by the library.  Otherwise,
 * the client must guarantee that the buffer is
 * large enough to hold any individual strip of
 * raw data.
 */
long
SECTiff::TIFFReadBufferSetup(TIFF* tif, tdata_t bp, tsize_t size)
{
	static const TCHAR module[] = _T("TIFFReadBufferSetup");

	if (tif->tif_rawdata) {
		if (tif->tif_flags & TIFF_MYBUFFER)
			_TIFFfree(tif->tif_rawdata);
		tif->tif_rawdata = NULL;
	}
	if (bp) {
		tif->tif_rawdatasize = size;
		tif->tif_rawdata = (tidata_t) bp;
		tif->tif_flags &= ~TIFF_MYBUFFER;
	} else {
		tif->tif_rawdatasize = roundup(size, 1024);
		tif->tif_rawdata = (LPBYTE) _TIFFmalloc((size_t)tif->tif_rawdatasize);
		tif->tif_flags |= TIFF_MYBUFFER;
	}
	if (tif->tif_rawdata == NULL) {
		TIFFError(module,
		    _T("%s: No space for data buffer at scanline %ld"),
		    tif->tif_name, (long) tif->tif_row);
		tif->tif_rawdatasize = 0;
		return (0);
	}
	return (1);
}

/*
 * Set state to appear as if a
 * strip has just been read in.
 */
int
SECTiff::TIFFStartStrip(TIFF* tif, tstrip_t strip)
{
	TIFFDirectory *td = &tif->tif_dir;

	tif->tif_curstrip = strip;
	tif->tif_row = (strip % td->td_stripsperimage) * td->td_rowsperstrip;
	tif->tif_rawcp = tif->tif_rawdata;
	tif->tif_rawcc = td->td_stripbytecount[strip];
	m_fpBool=tif->tif_predecode;
	return (m_fpBool== NULL || (this->*m_fpBool)(tif));
}

/*
 * Set state to appear as if a
 * tile has just been read in.
 */
int
SECTiff::TIFFStartTile(TIFF* tif, ttile_t tile)
{
	TIFFDirectory *td = &tif->tif_dir;

	tif->tif_curtile = tile;
	tif->tif_row =
	    (tile % howmany(td->td_imagewidth, td->td_tilewidth)) *
		td->td_tilelength;
	tif->tif_col =
	    (tile % howmany(td->td_imagelength, td->td_tilelength)) *
		td->td_tilewidth;
	tif->tif_rawcp = tif->tif_rawdata;
	tif->tif_rawcc = td->td_stripbytecount[tile];
	m_fpBool = tif->tif_predecode;
	return (m_fpBool == NULL || (this->*m_fpBool)(tif));
}

int
SECTiff::TIFFCheckRead(TIFF* tif, int tiles)
{
	if (tif->tif_mode == O_WRONLY) {
		TIFFError(tif->tif_name, _T("File not open for reading"));
		return (0);
	}
	if (tiles ^ isTiled(tif)) {
		TIFFError(tif->tif_name, tiles ?
		    _T("Can not read tiles from a stripped image") :
		    _T("Can not read scanlines from a tiled image"));
		return (0);
	}
	return (1);
}

void
SECTiff::TIFFNoPostDecode(TIFF* tif, tidata_t buf, tsize_t cc)
{    
	int aab_check=1; //16 bit needs some code in here otherwise bad things happen
	aab_check++;
//UNUSED
cc;
buf;
tif;
}

void
SECTiff::TIFFSwab16BitData(TIFF* tif, tidata_t buf, tsize_t cc)
{
    assert((cc & 1) == 0);
    TIFFSwabArrayOfShort((LPWORD) buf, cc/2);
tif; //UNUSED

}

void
SECTiff::TIFFSwab32BitData(TIFF* tif, tidata_t buf, tsize_t cc)
{
    assert((cc & 3) == 0);
    TIFFSwabArrayOfLong((LPDWORD) buf, cc/4);
tif; //UNUSED
}
