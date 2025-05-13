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
//  Description:	tif_writ.cpp
//		scanline oriented write support
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
 * Scanline-oriented Write Support
 */
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

#define	STRIPINCR	20		/* expansion factor on strip array */

long
SECTiff::TIFFWriteScanline(TIFF* tif, tdata_t buf, DWORD row, tsample_t sample)
{
	static const TCHAR module[] = _T("TIFFWriteScanline");
	register TIFFDirectory *td;
	long status, imagegrew = 0;
	tstrip_t strip;

	if (!TIFFWriteCheck(tif, 0, module))
		return (-1);
	/*
	 * Handle delayed allocation of data buffer.  This
	 * permits it to be sized more intelligently (using
	 * directory information).
	 */
	if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
		if (!TIFFBufferSetup(tif, module))
			return (-1);
		tif->tif_flags |= TIFF_BUFFERSETUP;
	}
	td = &tif->tif_dir;
	/*
	 * Extend image length if needed
	 * (but only for PlanarConfig=1).
	 */
	if (row >= td->td_imagelength) {	/* extend image */
		if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
			TIFFError(tif->tif_name,
		_T("Can not change \"ImageLength\" when using separate planes"));
			return (-1);
		}
		td->td_imagelength = row+1;
		imagegrew = 1;
	}
	/*
	 * Calculate strip and check for crossings.
	 */
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
		if (sample >= td->td_samplesperpixel) {
			TIFFError(tif->tif_name,
			    _T("%d: Sample out of range, max %d"),
			    sample, td->td_samplesperpixel);
			return (-1);
		}
		strip = sample*td->td_stripsperimage + row/td->td_rowsperstrip;
	} else
		strip = row / td->td_rowsperstrip;
	if (strip != tif->tif_curstrip) {
		/*
		 * Changing strips -- flush any data present.
		 */
		if (!TIFFFlushData(tif))
			return (-1);
		tif->tif_curstrip = strip;
		/*
		 * Watch out for a growing image.  The value of
		 * strips/image will initially be 1 (since it
		 * can't be deduced until the imagelength is known).
		 */
		if (strip >= td->td_stripsperimage && imagegrew)
			td->td_stripsperimage =
			    howmany(td->td_imagelength, td->td_rowsperstrip);
		tif->tif_row =
		    (strip % td->td_stripsperimage) * td->td_rowsperstrip;
		m_fpBool = tif->tif_preencode;
		if (tif->tif_preencode && !(this->*m_fpBool)(tif))
			return (-1);
		tif->tif_flags |= TIFF_POSTENCODE;
	}
	/*
	 * Check strip array to make sure there's space.
	 * We don't support dynamically growing files that
	 * have data organized in separate bitplanes because
	 * it's too painful.  In that case we require that
	 * the imagelength be set properly before the first
	 * write (so that the strips array will be fully
	 * allocated above).
	 */
	if (strip >= td->td_nstrips && !TIFFGrowStrips(tif, 1, module))
		return (-1);
	/*
	 * Ensure the write is either sequential or at the
	 * beginning of a strip (or that we can randomly
	 * access the data -- i.e. no encoding).
	 */
	if (row != (DWORD)tif->tif_row) {
		if (tif->tif_seek) {
			if (row < (DWORD)tif->tif_row) {
				/*
				 * Moving backwards within the same strip:
				 * backup to the start and then decode
				 * forward (below).
				 */
				tif->tif_row = (strip % td->td_stripsperimage) *
				    td->td_rowsperstrip;
				tif->tif_rawcp = tif->tif_rawdata;
			}
			/*
			 * Seek forward to the desired row.
			 */
			m_fpSeek = tif->tif_seek;
			if (!(this->*m_fpSeek)(tif, row - tif->tif_row))
				return (-1);
			tif->tif_row = row;
		} else {
			TIFFError(tif->tif_name,
		    _T("Compression algorithm does not support random access"));
			return (-1);
		}
	}

	m_fpCode = tif->tif_encoderow;
	status = (this->*m_fpCode)(tif, (tidata_t)buf, tif->tif_scanlinesize, (tsample_t)sample);
	tif->tif_row++;
	return (status);
}

/*
 * Encode the supplied data and write it to the
 * specified strip.  There must be space for the
 * data; we don't check if strips overlap!
 *
 * NB: Image length must be setup before writing; this
 *     interface does not support automatically growing
 *     the image on each write (as TIFFWriteScanline does).
 */
tsize_t
SECTiff::TIFFWriteEncodedStrip(TIFF* tif, tstrip_t strip, tdata_t data, tsize_t cc)
{
	static const TCHAR module[] = _T("TIFFWriteEncodedStrip");
	TIFFDirectory *td = &tif->tif_dir;

	if (!TIFFWriteCheck(tif, 0, module))
		return ((tsize_t) -1);
	if (strip >= td->td_nstrips) {
		TIFFError(module, _T("%s: Strip %lu out of range, max %lu"),
		    tif->tif_name, (DWORD) strip, (DWORD) td->td_nstrips);
		return ((tsize_t) -1);
	}
	/*
	 * Handle delayed allocation of data buffer.  This
	 * permits it to be sized according to the directory
	 * info.
	 */
	if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
		if (!TIFFBufferSetup(tif, module))
			return ((tsize_t) -1);
		tif->tif_flags |= TIFF_BUFFERSETUP;
	}
	tif->tif_curstrip = strip;
	tif->tif_flags &= ~TIFF_POSTENCODE;
	m_fpBool = tif->tif_preencode;
	if (tif->tif_preencode && !(this->*m_fpBool)(tif))
		return ((tsize_t) -1);
	m_fpCode = tif->tif_encodestrip;
	if (!(this->*m_fpCode)(tif,
	    (tidata_t)data, cc, (tsample_t)(strip / td->td_stripsperimage)))
		return ((tsize_t) 0);
	m_fpBool = tif->tif_postencode;
	if (tif->tif_postencode && !(this->*m_fpBool)(tif))
		return (-1);
	if (td->td_fillorder != (WORD)tif->tif_fillorder &&
	    (tif->tif_flags & TIFF_NOBITREV) == 0)
		TIFFReverseBits(tif->tif_rawdata, tif->tif_rawcc);
	if (tif->tif_rawcc > 0 &&
	    !TIFFAppendToStrip(tif, strip, tif->tif_rawdata, tif->tif_rawcc))
		return (-1);
	tif->tif_rawcc = 0;
	tif->tif_rawcp = tif->tif_rawdata;
	return (cc);
}

/*
 * Write the supplied data to the specified strip.
 * There must be space for the data; we don't check
 * if strips overlap!
 *
 * NB: Image length must be setup before writing; this
 *     interface does not support automatically growing
 *     the image on each write (as TIFFWriteScanline does).
 */
tsize_t
SECTiff::TIFFWriteRawStrip(TIFF* tif, tstrip_t strip, tdata_t data, tsize_t cc)
{
	static const TCHAR module[] = _T("TIFFWriteRawStrip");

	if (!TIFFWriteCheck(tif, 0, module))
		return ((tsize_t) -1);
	if (strip >= tif->tif_dir.td_nstrips) {
		TIFFError(module, _T("%s: Strip %lu out of range, max %lu"),
		    tif->tif_name, (DWORD) strip,
		    (DWORD) tif->tif_dir.td_nstrips);
		return ((tsize_t) -1);
	}
	return (TIFFAppendToStrip(tif, strip, (tidata_t)data, cc) ? cc : (tsize_t) -1);
}

/*
 * Write and compress a tile of data.  The
 * tile is selected by the (x,y,z,s) coordinates.
 */
tsize_t
SECTiff::TIFFWriteTile(TIFF* tif,
    tdata_t buf, DWORD x, DWORD y, DWORD z, tsample_t s)
{
	if (!TIFFCheckTile(tif, x, y, z, s))
		return (-1);
	/*
	 * NB: A tile size of -1 is used instead of tif_tilesize knowing
	 *     that TIFFWriteEncodedTile will clamp this to the tile size.
	 *     This is done because the tile size may not be defined until
	 *     after the output buffer is setup in TIFFBufferSetup.
	 */
	return (TIFFWriteEncodedTile(tif,
	    TIFFComputeTile(tif, x, y, z, s), buf, (tsize_t) -1));
}

/*
 * Encode the supplied data and write it to the
 * specified tile.  There must be space for the
 * data.  The function clamps individual writes
 * to a tile to the tile size, but does not (and
 * can not) check that multiple writes to the same
 * tile do not write more than tile size data.
 *
 * NB: Image length must be setup before writing; this
 *     interface does not support automatically growing
 *     the image on each write (as TIFFWriteScanline does).
 */
tsize_t
SECTiff::TIFFWriteEncodedTile(TIFF* tif, ttile_t tile, tdata_t data, tsize_t cc)
{
	static const TCHAR module[] = _T("TIFFWriteEncodedTile");
	TIFFDirectory *td;

	if (!TIFFWriteCheck(tif, 1, module))
		return ((tsize_t) -1);
	td = &tif->tif_dir;
	if (tile >= (ttile_t)td->td_nstrips) {
		TIFFError(module, _T("%s: Tile %lu out of range, max %lu"),
		    tif->tif_name, (DWORD) tile, (DWORD) td->td_nstrips);
		return ((tsize_t) -1);
	}
	/*
	 * Handle delayed allocation of data buffer.  This
	 * permits it to be sized more intelligently (using
	 * directory information).
	 */
	if ((tif->tif_flags & TIFF_BUFFERSETUP) == 0) {
		if (!TIFFBufferSetup(tif, module))
			return ((tsize_t) -1);
		tif->tif_flags |= TIFF_BUFFERSETUP;
	}
	tif->tif_curtile = tile;
	/* 
	 * Compute tiles per row & per column to compute
	 * current row and column
	 */
	tif->tif_row = (tile % howmany(td->td_imagelength, td->td_tilelength))
		* td->td_tilelength;
	tif->tif_col = (tile % howmany(td->td_imagewidth, td->td_tilewidth))
		* td->td_tilewidth;

	tif->tif_flags &= ~TIFF_POSTENCODE;
	m_fpBool = tif->tif_preencode;
	if (tif->tif_preencode && !(this->*m_fpBool)(tif))
		return ((tsize_t) -1);
	/*
	 * Clamp write amount to the tile size.  This is mostly
	 * done so that callers can pass in some large number
	 * (e.g. -1) and have the tile size used instead.
	 */
	if (cc > tif->tif_tilesize)
		cc = tif->tif_tilesize;
	m_fpCode = tif->tif_encodetile;
	if (!(this->*m_fpCode)(tif, (tidata_t)data, cc, (tsample_t)(tile/td->td_stripsperimage)))
		return ((tsize_t) 0);
	m_fpBool = tif->tif_postencode;
	if (tif->tif_postencode && !(this->*m_fpBool)(tif))
		return ((tsize_t) -1);
	if (td->td_fillorder != (WORD)tif->tif_fillorder &&
	    (tif->tif_flags & TIFF_NOBITREV) == 0)
		TIFFReverseBits((LPBYTE)tif->tif_rawdata, tif->tif_rawcc);
	if (tif->tif_rawcc > 0 && !TIFFAppendToStrip(tif, tile,
	    tif->tif_rawdata, tif->tif_rawcc))
		return ((tsize_t) -1);
	tif->tif_rawcc = 0;
	tif->tif_rawcp = tif->tif_rawdata;
	return (cc);
}

/*
 * Write the supplied data to the specified strip.
 * There must be space for the data; we don't check
 * if strips overlap!
 *
 * NB: Image length must be setup before writing; this
 *     interface does not support automatically growing
 *     the image on each write (as TIFFWriteScanline does).
 */
tsize_t
SECTiff::TIFFWriteRawTile(TIFF* tif, ttile_t tile, tdata_t data, tsize_t cc)
{
	static const TCHAR module[] = _T("TIFFWriteRawTile");

	if (!TIFFWriteCheck(tif, 1, module))
		return ((tsize_t) -1);
	if (tile >= (ttile_t)tif->tif_dir.td_nstrips) {
		TIFFError(module, _T("%s: Tile %lu out of range, max %lu"),
		    tif->tif_name, (DWORD) tile,
		    (DWORD) tif->tif_dir.td_nstrips);
		return ((tsize_t) -1);
	}
	return (TIFFAppendToStrip(tif, tile, (LPBYTE)data, cc) ? cc : (tsize_t) -1);
}

long
SECTiff::TIFFSetupStrips(TIFF* tif)
{
#define	isUnspecified(td, v) \
    (td->v == 0xffffffff || (td)->td_imagelength == 0)
	register TIFFDirectory *td = &tif->tif_dir;

	if (!isTiled(tif))
		td->td_stripsperimage = isUnspecified(td, td_rowsperstrip) ?
		    1 : howmany(td->td_imagelength, td->td_rowsperstrip);
	else
		td->td_stripsperimage = isUnspecified(td, td_tilelength) ?
		    1 : TIFFNumberOfTiles(tif);
	td->td_nstrips = td->td_stripsperimage;
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		td->td_nstrips *= td->td_samplesperpixel;
	td->td_stripoffset = (LPDWORD)
	    _TIFFmalloc((size_t)(td->td_nstrips * sizeof (DWORD)));
	td->td_stripbytecount = (LPDWORD)
	    _TIFFmalloc((size_t)(td->td_nstrips * sizeof (DWORD)));
	if (td->td_stripoffset == NULL || td->td_stripbytecount == NULL)
		return (0);
	/*
	 * Place data at the end-of-file
	 * (by setting offsets to zero).
	 */
	memset(td->td_stripoffset, 0, (size_t)(td->td_nstrips*sizeof (DWORD)));
	memset(td->td_stripbytecount, 0, (size_t)(td->td_nstrips*sizeof (DWORD)));
	TIFFSetFieldBit(tif, FIELD_STRIPOFFSETS);
	TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
	return (1);
#undef isUnspecified
}

/*
 * Verify file is writable and that the directory
 * information is setup properly.  In doing the latter
 * we also "freeze" the state of the directory so
 * that important information is not changed.
 */
long
SECTiff::TIFFWriteCheck(TIFF* tif, long tiles, const TCHAR* module)
{
	if (tif->tif_mode == O_RDONLY) {
		TIFFError(module, _T("%s: File not open for writing"),
		    tif->tif_name);
		return (0);
	}
	if (tiles ^ isTiled(tif)) {
		TIFFError(tif->tif_name, tiles ?
		    _T("Can not write tiles to a stripped image") :
		    _T("Can not write scanlines to a tiled image"));
		return (0);
	}
	/*
	 * On the first write verify all the required information
	 * has been setup and initialize any data structures that
	 * had to wait until directory information was set.
	 * Note that a lot of our work is assumed to remain valid
	 * because we disallow any of the important parameters
	 * from changing after we start writing (i.e. once
	 * TIFF_BEENWRITING is set, TIFFSetField will only allow
	 * the image's length to be changed).
	 */
	if ((tif->tif_flags & TIFF_BEENWRITING) == 0) {
		if (!TIFFFieldSet(tif, FIELD_IMAGEDIMENSIONS)) {
			TIFFError(module,
			    _T("%s: Must set \"ImageWidth\" before writing data"),
			    tif->tif_name);
			return (0);
		}
		if (!TIFFFieldSet(tif, FIELD_PLANARCONFIG)) {
			TIFFError(module,
		    _T("%s: Must set \"PlanarConfiguration\" before writing data"),
			    tif->tif_name);
			return (0);
		}
		if (tif->tif_dir.td_stripoffset == NULL &&
		    !TIFFSetupStrips(tif)) {
			tif->tif_dir.td_nstrips = 0;
			TIFFError(module, _T("%s: No space for %s arrays"),
			    tif->tif_name, isTiled(tif) ? _T("tile") : _T("strip"));
			return (0);
		}
		tif->tif_flags |= TIFF_BEENWRITING;
	}
	return (1);
}

/*
 * Setup the raw data buffer used for encoding.
 */
long
SECTiff::TIFFBufferSetup(TIFF* tif, const TCHAR* module)
{
	tsize_t size;

	if (isTiled(tif))
		tif->tif_tilesize = size = TIFFTileSize(tif);
	else
		tif->tif_scanlinesize = size = TIFFScanlineSize(tif);
	/*
	 * Make raw data buffer at least 8K
	 */
	if (size < 8*1024)
		size = 8*1024;
	tif->tif_rawdata = (LPBYTE)_TIFFmalloc((size_t)size);
	if (tif->tif_rawdata == NULL) {
		TIFFError(module, _T("%s: No space for output buffer"),
		    tif->tif_name);
		return (0);
	}
	tif->tif_flags |= TIFF_MYBUFFER;
	tif->tif_rawdatasize = size;
	tif->tif_rawcc = 0;
	tif->tif_rawcp = tif->tif_rawdata;
	return (1);
}

/*
 * Grow the strip data structures by delta strips.
 */
long
SECTiff::TIFFGrowStrips(TIFF* tif, long delta, const TCHAR* module)
{
	TIFFDirectory *td = &tif->tif_dir;

	assert(td->td_planarconfig == PLANARCONFIG_CONTIG);
	td->td_stripoffset = (DWORD*)_TIFFrealloc(td->td_stripoffset,
	    (size_t)((td->td_nstrips + delta) * sizeof (DWORD)));
	td->td_stripbytecount = (DWORD*)_TIFFrealloc(td->td_stripbytecount,
	    (size_t)((td->td_nstrips + delta) * sizeof (DWORD)));
	if (td->td_stripoffset == NULL || td->td_stripbytecount == NULL) {
		td->td_nstrips = 0;
		TIFFError(module, _T("%s: No space to expand strip arrays"),
		    tif->tif_name);
		return (0);
	}
	memset(td->td_stripoffset+td->td_nstrips, 0, (size_t)(delta*sizeof (DWORD)));
	memset(td->td_stripbytecount+td->td_nstrips, 0, (size_t)(delta*sizeof (DWORD)));
	td->td_nstrips += delta;
	return (1);
}

/*
 * Append the data to the specified strip.
 *
 * NB: We don't check that there's space in the
 *     file (i.e. that strips do not overlap).
 */
long
SECTiff::TIFFAppendToStrip(TIFF* tif, tstrip_t strip, tidata_t data, tsize_t cc)
{
	TIFFDirectory *td = &tif->tif_dir;
	static const TCHAR module[] = _T("TIFFAppendToStrip");

	if (td->td_stripoffset[strip] == 0 || tif->tif_curoff == 0) {
		/*
		 * No current offset, set the current strip.
		 */
		if (td->td_stripoffset[strip] != 0) {
			if (!SeekOK(tif, td->td_stripoffset[strip])) {
				TIFFError(module,
				    _T("%s: Seek error at scanline %lu"),
				    tif->tif_name, (DWORD) tif->tif_row);
				return (0);
			}
		} else
			td->td_stripoffset[strip] =
			    TIFFSeekFile(tif, 0L, L_XTND);
		tif->tif_curoff = td->td_stripoffset[strip];
	}
	if (!WriteOK(tif, data, cc)) {
		TIFFError(module, _T("%s: Write error at scanline %lu"),
		    tif->tif_name, (DWORD) tif->tif_row);
		return (0);
	}
	tif->tif_curoff += cc;
	td->td_stripbytecount[strip] += cc;
	return (1);
}

/*
 * Internal version of TIFFFlushData that can be
 * called by ``encodestrip routines'' w/o concern
 * for infinite recursion.
 */
long
SECTiff::TIFFFlushData1(TIFF* tif)
{
	if (tif->tif_rawcc > 0) {
		if (tif->tif_dir.td_fillorder != (WORD)tif->tif_fillorder &&
		    (tif->tif_flags & TIFF_NOBITREV) == 0)
			TIFFReverseBits((BYTE *)tif->tif_rawdata,
			    tif->tif_rawcc);
		if (!TIFFAppendToStrip(tif,
		    isTiled(tif) ? tif->tif_curtile : tif->tif_curstrip,
		    tif->tif_rawdata, tif->tif_rawcc))
			return (0);
		tif->tif_rawcc = 0;
		tif->tif_rawcp = tif->tif_rawdata;
	}
	return (1);
}

/*
 * Set the current write offset.  This should only be
 * used to set the offset to a known previous location
 * (very carefully), or to 0 so that the next write gets
 * appended to the end of the file.
 */
void
SECTiff::TIFFSetWriteOffset(TIFF* tif, off_t off)
{
	tif->tif_curoff = off;
}
