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
//  Description:	tif_geti.cpp
//		read and return a packed RGBA image
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
 * Read and return a packed RGBA image.
 */
#include "..\..\..\stdafx.h"

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif



long
SECTiff::TIFFReadRGBAImage(TIFF* tif,
    DWORD rwidth, DWORD rheight, LPDWORD raster, long stop)
{
	int ok, alpha;
	DWORD width, height;
	LPWORD sampleinfo;

	TIFFGetFieldDefaulted(tif, TIFFTAG_BITSPERSAMPLE, &m_wBPS);
	switch (m_wBPS) {
	case 1: case 2: case 4:
	case 8: case 16:
		break;
	default:
		TIFFError(TIFFFileName(tif),
		    _T("Sorry, can not handle %d-bit images"), m_wBPS);
		return (0);
	}
	TIFFGetFieldDefaulted(tif, TIFFTAG_SAMPLESPERPIXEL, &m_wSPP);
	if (m_wSPP > 4) {
		TIFFError(TIFFFileName(tif),
		    _T("Sorry, can not handle images with %d-samples/pixel"),
		    m_wSPP);
		return (0);
	}
	TIFFGetFieldDefaulted(tif, TIFFTAG_EXTRASAMPLES,
	    &m_wExtraSamples, &sampleinfo);
	alpha = (m_wExtraSamples == 1 && sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
	TIFFGetFieldDefaulted(tif, TIFFTAG_PLANARCONFIG, &m_wPlanarConfig);
	switch (m_wSPP - m_wExtraSamples) {
	case 3:
		break;
	case 1: case 4:
/* XXX */
		if (!alpha || m_wPlanarConfig != PLANARCONFIG_CONTIG)
			break;
		/* fall thru... */
	default:
		TIFFError(TIFFFileName(tif),
		    _T("Sorry, can not handle %d-channel %s images%s"),
		    m_wSPP,
		    m_wPlanarConfig == PLANARCONFIG_CONTIG ?
			_T("packed") : _T("separated"),
		    alpha ? _T(" with alpha") : _T(""));
		return (0);
	}
	if (!TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &m_wPhoto)) {
		switch (m_wSPP) {
		case 1:
			m_wPhoto = PHOTOMETRIC_MINISBLACK;
			break;
		case 3: case 4:
			m_wPhoto = PHOTOMETRIC_RGB;
			break;
		default:
			TIFFError(TIFFFileName(tif),
			    _T("Missing needed \"PhotometricInterpretation\" tag"));
			return (0);
		}
		TIFFError(TIFFFileName(tif),
		    _T("No \"PhotometricInterpretation\" tag, assuming %s\n"),
		    m_wPhoto == PHOTOMETRIC_RGB ? _T("RGB") : _T("min-is-black"));
	}
	switch (m_wPhoto) {
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_RGB:
	case PHOTOMETRIC_PALETTE:
	case PHOTOMETRIC_YCBCR:
		break;
	case PHOTOMETRIC_SEPARATED: {
		WORD inkset;
		TIFFGetFieldDefaulted(tif, TIFFTAG_INKSET, &inkset);
		if (inkset != INKSET_CMYK) {
			TIFFError(TIFFFileName(tif),
			    _T("Sorry, can not handle separated image with %s=%d"),
			    _T("InkSet"), inkset);
			return (0);
		}
		break;
	}
	default:
		TIFFError(TIFFFileName(tif),
		    _T("Sorry, can not handle image with %s=%d"),
		    _T("PhotometricInterpretation"), m_wPhoto);
		return (0);
	}
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
	/* XXX verify rwidth and rheight against width and height */
	m_nStopOnErr = (int)stop;
	m_lpgetiBWmap = NULL;
	m_lpgetiPALmap = NULL;
	ok = geti_gt(tif, rwidth, height, raster + (rheight-height)*rwidth);
	if (m_lpgetiBWmap)
		_TIFFfree(m_lpgetiBWmap);
	if (m_lpgetiPALmap)
		_TIFFfree(m_lpgetiPALmap);
	return (ok);
}

WORD 
SECTiff::geti_checkcmap(int n, LPWORD r, LPWORD g, LPWORD b)
{
	while (n-- > 0)
		if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
			return (16);
	return (8);
}

/*
 * Construct a mapping table to convert from the range
 * of the data samples to [0,255] --for display.  This
 * process also handles inverting B&W images when needed.
 */ 
int
SECTiff::geti_setupMap(TIFF* tif,
    WORD minsamplevalue, WORD maxsamplevalue, LPPRGBvalue pMap)
{
	register int x, range;
	LPRGBvalue Map;

	range = maxsamplevalue - minsamplevalue;
	Map = (LPRGBvalue)_TIFFmalloc((size_t)((range + 1) * sizeof (RGBvalue)));
	if (Map == NULL) {
		TIFFError(TIFFFileName(tif),
		    _T("No space for photometric conversion table"));
		return (0);
	}
	if (m_wPhoto == PHOTOMETRIC_MINISWHITE) {
		for (x = 0; x <= range; x++)
			Map[x] = (RGBvalue)(((range - x) * 255) / range);
	} else {
		for (x = 0; x <= range; x++)
			Map[x] = (RGBvalue)((x * 255) / range);
	}
	if (m_wBPS <= 8 &&
	    (m_wPhoto == PHOTOMETRIC_MINISBLACK ||
	     m_wPhoto == PHOTOMETRIC_MINISWHITE)) {
		/*
		 * Use photometric mapping table to construct
		 * unpacking tables for samples <= 8 bits.
		 */
		if (!geti_makebwmap(tif, Map))
			return (0);
		/* no longer need Map, free it */
		_TIFFfree(Map);
		Map = NULL;
	}
	*pMap = Map;
	return (1);
}

int
SECTiff::geti_gt(TIFF* tif, DWORD w, DWORD h, LPDWORD raster)
{
	WORD minsamplevalue, maxsamplevalue;
	LPRGBvalue Map;
	int e, ncomps;

	TIFFGetFieldDefaulted(tif, TIFFTAG_MINSAMPLEVALUE, &minsamplevalue);
	TIFFGetFieldDefaulted(tif, TIFFTAG_MAXSAMPLEVALUE, &maxsamplevalue);
	Map = NULL;
	switch (m_wPhoto) {
	case PHOTOMETRIC_YCBCR:
		TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRCOEFFICIENTS,
		    &m_lpYCbCrCoeffs);
		TIFFGetFieldDefaulted(tif, TIFFTAG_YCBCRSUBSAMPLING,
		    &m_wYCbCrHorizSampling, &m_wYCbCrVertSampling);
		TIFFGetFieldDefaulted(tif, TIFFTAG_REFERENCEBLACKWHITE,
		    &m_lpRefBlackWhite);
		geti_initYCbCrConversion();
		/* fall thru... */
	case PHOTOMETRIC_RGB:
	case PHOTOMETRIC_SEPARATED:
		if (minsamplevalue == 0 && maxsamplevalue == 255)
			break;
		/* fall thru... */
	case PHOTOMETRIC_MINISBLACK:
	case PHOTOMETRIC_MINISWHITE:
		if (!geti_setupMap(tif, minsamplevalue, maxsamplevalue, &Map))
			return (0);
		break;
	case PHOTOMETRIC_PALETTE:
		if (!TIFFGetField(tif, TIFFTAG_COLORMAP,
				  &redcmap, &greencmap, &bluecmap)) {
			TIFFError(TIFFFileName(tif),
			    _T("Missing required \"Colormap\" tag"));
			return (0);
		}
		/*
		 * Convert 16-bit colormap to 8-bit (unless it looks
		 * like an old-style 8-bit colormap).
		 */
		if (geti_checkcmap(1<<m_wBPS, redcmap, greencmap, bluecmap) == 16) {
			int i;
			for (i = (1<<m_wBPS)-1; i > 0; i--) {
#define	CVT(x)		(((x) * 255) / ((1<<16)-1))
				redcmap[i] = (WORD)CVT(redcmap[i]);
				greencmap[i] = (WORD)CVT(greencmap[i]);
				bluecmap[i] = (WORD)CVT(bluecmap[i]);
			}
		} else
			TIFFWarning(TIFFFileName(tif), _T("Assuming 8-bit colormap"));
		if (m_wBPS <= 8) {
			/*
			 * Use mapping table and colormap to construct
			 * unpacking tables for samples < 8 bits.
			 */
			if (!geti_makecmap(tif, redcmap, greencmap, bluecmap))
				return (0);
		}
		break;
	}
	ncomps = m_wSPP - m_wExtraSamples;
	if (m_wPlanarConfig == PLANARCONFIG_SEPARATE && ncomps > 1) {
		e = TIFFIsTiled(tif) ?
		    geti_gtTileSeparate(tif, raster, Map, h, w) :
		    geti_gtStripSeparate(tif, raster, Map, h, w);
	} else {
		e = TIFFIsTiled(tif) ? 
		    geti_gtTileContig(tif, raster, Map, h, w) :
		    geti_gtStripContig(tif, raster, Map, h, w);
	}
	if (Map)
		_TIFFfree(Map);
	return (e);
}

DWORD
SECTiff::geti_setorientation(TIFF* tif, DWORD h)
{
	DWORD y;

	TIFFGetFieldDefaulted(tif, TIFFTAG_ORIENTATION, &m_wOrientation);
	switch (m_wOrientation) {
	case ORIENTATION_BOTRIGHT:
	case ORIENTATION_RIGHTBOT:	/* XXX */
	case ORIENTATION_LEFTBOT:	/* XXX */
		TIFFWarning(TIFFFileName(tif), _T("using bottom-left orientation"));
		m_wOrientation = ORIENTATION_BOTLEFT;
		/* fall thru... */
	case ORIENTATION_BOTLEFT:
		y = 0;
		break;
	case ORIENTATION_TOPRIGHT:
	case ORIENTATION_RIGHTTOP:	/* XXX */
	case ORIENTATION_LEFTTOP:	/* XXX */
	default:
		TIFFWarning(TIFFFileName(tif), _T("using top-left orientation"));
		m_wOrientation = ORIENTATION_TOPLEFT;
		/* fall thru... */
	case ORIENTATION_TOPLEFT:
		y = h-1;
		break;
	}
	return (y);
}



/*
 * Get an tile-organized image that has
 *	PlanarConfiguration contiguous if m_wSPP > 1
 * or
 *	m_wSPP == 1
 */	
int
SECTiff::geti_gtTileContig(TIFF* tif, LPDWORD raster, LPRGBvalue Map, DWORD h, DWORD w)
{
	DWORD col, row, y;
	DWORD tw, th;
	LPBYTE buf;
	long fromskew, toskew;
	WORD nrow;


	m_fpgetiContigRoutine = geti_pickTileContigCase(tif, Map);
	if (m_fpgetiContigRoutine == 0)
		return (0);
	buf = (LPBYTE)_TIFFmalloc((size_t)(TIFFTileSize(tif)));
	if (buf == 0) {
		TIFFError(TIFFFileName(tif), _T("No space for tile buffer"));
		return (0);
	}
	TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
	TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
	y = geti_setorientation(tif, h);
	//AAB toskew = ((orientation == ORIENTATION_TOPLEFT) ? -tw + -w : -tw + w);
	toskew = -1 * (long)tw;
	if (m_wOrientation==ORIENTATION_TOPLEFT)
		toskew -= w;
	else
		toskew += w;
	for (row = 0; row < h; row += th) {
		nrow = (WORD)(row + th > h ? h - row : th);
		for (col = 0; col < w; col += tw) {
			if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0 &&
			    m_nStopOnErr)
				break;
			if (col + tw > w) {
				/*
				 * Tile is clipped horizontally.  Calculate
				 * visible portion and skewing factors.
				 */
				DWORD npix = w - col;
				fromskew = tw - npix;
				(this->*m_fpgetiContigRoutine)(raster + y*w + col, buf, Map,
				    npix, nrow, (long)fromskew, 
				    (long)(toskew + fromskew));
			} else
				(this->*m_fpgetiContigRoutine)(raster + y*w + col, buf, Map,
				    tw, nrow, 0L, toskew);
		}
		// y += (orientation == ORIENTATION_TOPLEFT ? -nrow : nrow);
		if (m_wOrientation == ORIENTATION_TOPLEFT)
			y -= nrow;
		else
			y += nrow;
	}
	_TIFFfree(buf);
	return (1);
}


/*
 * Get an tile-organized image that has
 *	 m_wSPP > 1
 *	 PlanarConfiguration separated
 * We assume that all such images are RGB.
 */	
int
SECTiff::geti_gtTileSeparate(TIFF* tif, LPDWORD raster, LPRGBvalue Map, DWORD h, DWORD w)
{
	DWORD col, row, y;
	DWORD tw, th;
	LPBYTE buf;
	LPBYTE r, g, b;
	DWORD tilesize;
	long fromskew, toskew;
	WORD nrow;


	m_fpgetiTileSeparate = geti_pickTileSeparateCase(tif, Map);
	if (m_fpgetiTileSeparate == 0)
		return (0);
	tilesize = TIFFTileSize(tif);
	buf = (LPBYTE)_TIFFmalloc((size_t)(3*tilesize));
	if (buf == 0) {
		TIFFError(TIFFFileName(tif), _T("No space for tile buffer"));
		return (0);
	}
	r = buf;
	g = r + tilesize;
	b = g + tilesize;
	TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
	TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
	y = geti_setorientation(tif, h);
	toskew = (m_wOrientation == ORIENTATION_TOPLEFT ? -1*(long)tw + -1*(long)w : -1*(long)tw + (long)w);
	for (row = 0; row < h; row += th) {
		nrow = (row + th > h ? (WORD)(h - row) : (WORD)th);
		for (col = 0; col < w; col += tw) {
			if (TIFFReadTile(tif, r, col, row,0,0) < 0 && m_nStopOnErr)
				break;
			if (TIFFReadTile(tif, g, col, row,0,1) < 0 && m_nStopOnErr)
				break;
			if (TIFFReadTile(tif, b, col, row,0,2) < 0 && m_nStopOnErr)
				break;
			if (col + tw > w) {
				/*
				 * Tile is clipped horizontally.  Calculate
				 * visible portion and skewing factors.
				 */
				DWORD npix = w - col;
				fromskew = tw - npix;
				(this->*m_fpgetiTileSeparate)(raster + y*w + col, r, g, b, Map,
				    npix, nrow, (long)fromskew, (long)(toskew + fromskew));
			} else
				(this->*m_fpgetiTileSeparate)(raster + y*w + col, r, g, b, Map,
				    tw, nrow, 0L, toskew);
		}
		//AAB y += (orientation == ORIENTATION_TOPLEFT ? -nrow : nrow);
		if (m_wOrientation == ORIENTATION_TOPLEFT)
			y -= nrow;
		else
			y += nrow;
	}
	_TIFFfree(buf);
	return (1);
}

/*
 * Get a strip-organized image that has
 *	PlanarConfiguration contiguous if m_wSPP > 1
 * or
 *	m_wSPP == 1
 */	
int
SECTiff::geti_gtStripContig(TIFF* tif, LPDWORD raster, LPRGBvalue Map, DWORD h, DWORD w)
{
	DWORD row, y, nrow;
	LPBYTE buf;
	// tileContigRoutine put;
	DWORD rowsperstrip;
	DWORD imagewidth;
	DWORD scanline;
	long fromskew, toskew;

	m_fpgetiContigRoutine = geti_pickTileContigCase(tif, Map);
	if (m_fpgetiContigRoutine == 0)
		return (0);
	buf = (LPBYTE)_TIFFmalloc((size_t)(TIFFStripSize(tif)));
	if (buf == 0) {
		TIFFError(TIFFFileName(tif), _T("No space for strip buffer"));
		return (0);
	}
	y = geti_setorientation(tif, h);
	toskew = (m_wOrientation == ORIENTATION_TOPLEFT ? -1*(long)(w+w) : -1*(long)w + (long)w);
	TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
	scanline = TIFFScanlineSize(tif);
	fromskew = (w < imagewidth ? imagewidth - w : 0);
	for (row = 0; row < h; row += rowsperstrip) {
		nrow = (row + rowsperstrip > h ? h - row : rowsperstrip);
		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 0),
		    buf, nrow*scanline) < 0 && m_nStopOnErr)
			break;
		(this->*m_fpgetiContigRoutine)(raster + y*w, buf, Map, w, nrow, fromskew, toskew);
		//AAB y += (orientation == ORIENTATION_TOPLEFT ? -nrow : nrow);
		if (m_wOrientation == ORIENTATION_TOPLEFT)
			y -= nrow;
		else
			y += nrow;
	}
	_TIFFfree(buf);
	return (1);
}

/*
 * Get a strip-organized image with
 *	 m_wSPP > 1
 *	 PlanarConfiguration separated
 * We assume that all such images are RGB.
 */
int
SECTiff::geti_gtStripSeparate(TIFF* tif, LPDWORD raster, LPRGBvalue Map, DWORD h, DWORD w)
{
	LPBYTE buf;
	LPBYTE r, g, b;
	DWORD row, y, nrow;
	DWORD scanline;
	// tileSeparateRoutine put;
	DWORD rowsperstrip;
	DWORD imagewidth;
	DWORD stripsize;
	long fromskew, toskew;

	stripsize = TIFFStripSize(tif);
	r = buf = (LPBYTE)_TIFFmalloc((size_t)(3*stripsize));
	if (buf == 0)
		return (0);
	g = r + stripsize;
	b = g + stripsize;
	m_fpgetiTileSeparate = geti_pickTileSeparateCase(tif, Map);
	if (m_fpgetiTileSeparate == 0) {
		TIFFError(TIFFFileName(tif), _T("Can not handle format"));
		return (0);
	}
	y = geti_setorientation(tif, h);
	toskew = (m_wOrientation == ORIENTATION_TOPLEFT ? -1*(long)(2*w) : -1*(long)w + (long)w);
	TIFFGetFieldDefaulted(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imagewidth);
	scanline = TIFFScanlineSize(tif);
	fromskew = (w < imagewidth ? imagewidth - w : 0);
	for (row = 0; row < h; row += rowsperstrip) {
		nrow = (row + rowsperstrip > h ? h - row : rowsperstrip);
		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 0),
		    r, nrow*scanline) < 0 && m_nStopOnErr)
			break;
		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 1),
		    g, nrow*scanline) < 0 && m_nStopOnErr)
			break;
		if (TIFFReadEncodedStrip(tif, TIFFComputeStrip(tif, row, 2),
		    b, nrow*scanline) < 0 && m_nStopOnErr)
			break;
		(this->*m_fpgetiTileSeparate)(raster + y*w, r, g, b, Map, w, nrow, fromskew, toskew);
		//AAB y += (orientation == ORIENTATION_TOPLEFT ? -nrow : nrow);
		if (m_wOrientation==ORIENTATION_TOPLEFT)
			y -= nrow;
		else
			y  += nrow;
	}
	_TIFFfree(buf);
	return (1);
}

#define	PACK(r,g,b)	((DWORD)(r)|((DWORD)(g)<<8)|((DWORD)(b)<<16))

/*
 * Greyscale images with less than 8 bits/sample are handled
 * with a table to avoid lots of shifts and masks.  The table
 * is setup so that put*bwtile (below) can retrieve 8/m_wBPS
 * pixel values simply by indexing into the table with one
 * number.
 */
int
SECTiff::geti_makebwmap(TIFF* tif, LPRGBvalue Map)
{
	register int i;
	int nsamples = 8 / m_wBPS;
	register LPDWORD p;

	m_lpgetiBWmap = (LPPDWORD)_TIFFmalloc(
	    (size_t)(256*sizeof (LPDWORD)+(256*nsamples*sizeof(DWORD))));
	if (m_lpgetiBWmap == NULL) {
		TIFFError(TIFFFileName(tif), _T("No space for B&W mapping table"));
		return (0);
	}
	p = (LPDWORD)(m_lpgetiBWmap + 256);
	for (i = 0; i < 256; i++) {
		m_lpgetiBWmap[i] = p;
		switch (m_wBPS) {
			register RGBvalue c;
#define	GREY(x)	c = Map[x]; *p++ = PACK(c,c,c);
		case 1:
			GREY(i>>7);
			GREY((i>>6)&1);
			GREY((i>>5)&1);
			GREY((i>>4)&1);
			GREY((i>>3)&1);
			GREY((i>>2)&1);
			GREY((i>>1)&1);
			GREY(i&1);
			break;
		case 2:
			GREY(i>>6);
			GREY((i>>4)&3);
			GREY((i>>2)&3);
			GREY(i&3);
			break;
		case 4:
			GREY(i>>4);
			GREY(i&0xf);
			break;
		case 8:
			GREY(i);
			break;
		}
#undef	GREY
	}
	return (1);
}

/*
 * Palette images with <= 8 bits/sample are handled
 * with a table to avoid lots of shifts and masks.  The table
 * is setup so that put*cmaptile (below) can retrieve 8/m_wBPS
 * pixel values simply by indexing into the table with one
 * number.
 */
int
SECTiff::geti_makecmap(TIFF* tif, LPWORD rmap, LPWORD gmap, LPWORD bmap)
{
	register int i;
	int nsamples = 8 / m_wBPS;
	register LPDWORD p;

	m_lpgetiPALmap = (LPPDWORD)_TIFFmalloc(
	    (size_t)(256*sizeof (LPDWORD)+(256*nsamples*sizeof(DWORD))));
	if (m_lpgetiPALmap == NULL) {
		TIFFError(TIFFFileName(tif), _T("No space for Palette mapping table"));
		return (0);
	}
	p = (LPDWORD)(m_lpgetiPALmap + 256);
	for (i = 0; i < 256; i++) {
		m_lpgetiPALmap[i] = p;
#define	CMAP(x)	\
c = (RGBvalue)(x); *p++ = (DWORD)(PACK(rmap[c]&0xff, (gmap[c]&0xff), (bmap[c]&0xff)));
		switch (m_wBPS) {
			register RGBvalue c;
		case 1:
			CMAP(i>>7);
			CMAP((i>>6)&1);
			CMAP((i>>5)&1);
			CMAP((i>>4)&1);
			CMAP((i>>3)&1);
			CMAP((i>>2)&1);
			CMAP((i>>1)&1);
			CMAP(i&1);
			break;
		case 2:
			CMAP(i>>6);
			CMAP((i>>4)&3);
			CMAP((i>>2)&3);
			CMAP(i&3);
			break;
		case 4:
			CMAP(i>>4);
			CMAP(i&0xf);
			break;
		case 8:
			CMAP(i);
			break;
		}
#undef CMAP
	}
	return (1);
}

/*
 * The following routines move decoded data returned
 * from the TIFF library into rasters filled with packed
 * ABGR pixels (i.e. suitable for passing to lrecwrite.)
 *
 * The routines have been created according to the most
 * important cases and optimized.  pickTileContigCase and
 * pickTileSeparateCase analyze the parameters and select
 * the appropriate "put" routine to use.
 */
#define	REPEAT8(op)	REPEAT4(op); REPEAT4(op)
#define	REPEAT4(op)	REPEAT2(op); REPEAT2(op)
#define	REPEAT2(op)	op; op
#define	CASE8(x,op)				\
	switch (x) {				\
	case 7: op; case 6: op; case 5: op;	\
	case 4: op; case 3: op; case 2: op;	\
	case 1: op;				\
	}
#define	CASE4(x,op)	switch (x) { case 3: op; case 2: op; case 1: op; }

#define	UNROLL8(w, op1, op2) {		\
	register DWORD x;		\
	for (x = (DWORD)w; x >= 8; x -= 8) {	\
		op1;			\
		REPEAT8(op2);		\
	}				\
	if (x > 0) {			\
		op1;			\
		CASE8(x,op2);		\
	}				\
}
#define	UNROLL4(w, op1, op2) {		\
	register DWORD x;		\
	for (x = w; x >= 4; x -= 4) {	\
		op1;			\
		REPEAT4(op2);		\
	}				\
	if (x > 0) {			\
		op1;			\
		CASE4(x,op2);		\
	}				\
}
#define	UNROLL2(w, op1, op2) {		\
	register DWORD x;		\
	for (x = w; x >= 2; x -= 2) {	\
		op1;			\
		REPEAT2(op2);		\
	}				\
	if (x) {			\
		op1;			\
		op2;			\
	}				\
}
			

#define	SKEW(r,g,b,skew)	{ r += skew; g += skew; b += skew; }

#define	DECLAREContigPutFunc(name) \
void SECTiff::name(\
    LPDWORD cp, \
    LPBYTE pp, \
    LPRGBvalue Map, \
    DWORD w, DWORD h, \
    long fromskew, long toskew \
)

/*
 * 8-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(geti_put8bitcmaptile)
{
	while (h-- > 0) {
		UNROLL8(w, NULL, *cp++ = m_lpgetiPALmap[*pp++][0]);
		cp += toskew;
		pp += fromskew;
	}
Map; //UNUSED
}

/*
 * 4-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(geti_put4bitcmaptile)
{
	register LPDWORD bw;

	fromskew /= 2;
	while (h-- > 0) {
		UNROLL2(w, bw = m_lpgetiPALmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}        
	Map; //UNUSED
}

/*
 * 2-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(geti_put2bitcmaptile)
{
	register LPDWORD bw;

	fromskew /= 4;
	while (h-- > 0) {
		UNROLL4(w, bw = m_lpgetiPALmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}            
	Map; //UNUSED
}

/*
 * 1-bit palette => colormap/RGB
 */
DECLAREContigPutFunc(geti_put1bitcmaptile)
{
	register LPDWORD bw;

	fromskew /= 8;
	while (h-- > 0) {
		UNROLL8(w, bw = m_lpgetiPALmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}            
Map; //UNUSED	
}

/*
 * 8-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(geti_putgreytile)
{
	while (h-- > 0) {
		register DWORD x;
		for (x = w; x-- > 0;)
			*cp++ = m_lpgetiBWmap[*pp++][0];
		cp += toskew;
		pp += fromskew;
	}
	Map; //UNUSED
}

/*
 * 1-bit bilevel => colormap/RGB
 */
DECLAREContigPutFunc(geti_put1bitbwtile)
{
	register LPDWORD bw;

	fromskew /= 8;
	while (h-- > 0) {
		UNROLL8(w, bw = m_lpgetiBWmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}            
	Map; //UNUSED
}

/*
 * 2-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(geti_put2bitbwtile)
{
	register LPDWORD bw;

	fromskew /= 4;
	while (h-- > 0) {
		UNROLL4(w, bw = m_lpgetiBWmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}            
	Map; //UNUSED
}

/*
 * 4-bit greyscale => colormap/RGB
 */
DECLAREContigPutFunc(geti_put4bitbwtile)
{
	register LPDWORD bw;

	fromskew /= 2;
	while (h-- > 0) {
		UNROLL2(w, bw = m_lpgetiBWmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}            
	Map; //UNUSED
}

/*
 * 8-bit packed samples => RGB
 */
DECLAREContigPutFunc(geti_putRGBcontig8bittile)
{
	fromskew *= m_wSPP;
	if (Map) {
		while (h-- > 0) {
			register DWORD x;
			for (x = w; x-- > 0;) {
				*cp++ = PACK(Map[pp[0]], Map[pp[1]], Map[pp[2]]);
				pp += m_wSPP;
			}
			pp += fromskew;
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			UNROLL8(w, NULL,
			    *cp++ = PACK(pp[0], pp[1], pp[2]);
			    pp += m_wSPP);
			cp += toskew;
			pp += fromskew;
		}
	}
}

/*
 * 16-bit packed samples => RGB
 */
DECLAREContigPutFunc(geti_putRGBcontig16bittile)
{
	register LPWORD wp = (LPWORD)pp;
	register DWORD x;

	fromskew *= m_wSPP;
	if (Map) {
		while (h-- > 0) {
			for (x = w; x-- > 0;) {
				*cp++ = PACK(Map[wp[0]], Map[wp[1]], Map[wp[2]]);
				wp += m_wSPP;
			}
			cp += toskew;
			wp += fromskew;
		}
	} else {
		while (h-- > 0) {
			for (x = w; x-- > 0;) {
				*cp++ = PACK(wp[0], wp[1], wp[2]);
				wp += m_wSPP;
			}
			cp += toskew;
			wp += fromskew;
		}
	}
}

/*
 * 8-bit packed CMYK samples => RGB
 *
 * NB: The conversion of CMYK->RGB is *very* crude.
 */
DECLAREContigPutFunc(geti_putRGBcontig8bitCMYKtile)
{
	WORD r, g, b, k;

	fromskew *= m_wSPP;
	if (Map) {
		while (h-- > 0) {
			register DWORD x;
			for (x = w; x-- > 0;) {
				k = (WORD)(255 - pp[3]);
				r = (WORD)((k*(255-pp[0]))/255);
				g = (WORD)((k*(255-pp[1]))/255);
				b = (WORD)((k*(255-pp[2]))/255);
				*cp++ = PACK(Map[r], Map[g], Map[b]);
				pp += m_wSPP;
			}
			pp += fromskew;
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			UNROLL8(w, NULL,
			    k = (WORD)(255 - pp[3]);
			    r = (WORD)((k*(255-pp[0]))/255);
			    g = (WORD)((k*(255-pp[1]))/255);
			    b = (WORD)((k*(255-pp[2]))/255);
			    *cp++ = PACK(r, g, b);
			    pp += (WORD)(m_wSPP));
			cp += toskew;
			pp += fromskew;
		}
	}
}

#define	DECLARESepPutFunc(name) \
void SECTiff::name(\
    LPDWORD cp, \
    LPBYTE r, LPBYTE g, LPBYTE b, \
    LPRGBvalue Map, \
    DWORD w, DWORD h, \
    long fromskew, long toskew \
)

/*
 * 8-bit unpacked samples => RGB
 */
DECLARESepPutFunc(geti_putRGBseparate8bittile)
{
	if (Map) {
		while (h-- > 0) {
			register DWORD x;
			for (x = w; x > 0; x--)
				*cp++ = PACK(Map[*r++], Map[*g++], Map[*b++]);
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			UNROLL8(w, NULL, *cp++ = PACK(*r++, *g++, *b++));
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	}
}

/*
 * 16-bit unpacked samples => RGB
 */
DECLARESepPutFunc(geti_putRGBseparate16bittile)
{
	register LPWORD wr = (LPWORD)r;
	register LPWORD wg = (LPWORD )g;
	register LPWORD wb = (LPWORD)b;
	register DWORD x;

	if (Map) {
		while (h-- > 0) {
			for (x = w; x > 0; x--)
				*cp++ = PACK(Map[*wr++],Map[*wg++],Map[*wb++]);
			SKEW(wr, wg, wb, fromskew);
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			for (x = 0; x < w; x++)
				*cp++ = PACK(*wr++, *wg++, *wb++);
			SKEW(wr, wg, wb, fromskew);
			cp += toskew;
		}
	}
}

#define	Code2V(c, RB, RW, CR)	((((c)-RB)*(float)CR)/(float)(RW-RB))
#define	CLAMP(f,min,max) \
    (int)((f)+.5 < (min) ? (min) : (f)+.5 > (max) ? (max) : (f)+.5)

#define	LumaRed		m_lpYCbCrCoeffs[0]
#define	LumaGreen	m_lpYCbCrCoeffs[1]
#define	LumaBlue	m_lpYCbCrCoeffs[2]


void
SECTiff::geti_initYCbCrConversion(void)
{
	m_D1 = 2 - 2*LumaRed;
	m_D2 = m_D1*LumaRed / LumaGreen;
	m_D3 = 2 - 2*LumaBlue;
	m_D4 = m_D3*LumaBlue / LumaGreen;
}

 void
SECTiff::geti_putRGBContigYCbCrClump(
	LPDWORD cp, LPBYTE pp,
	DWORD cw, DWORD ch,
	DWORD w,
	int n, long fromskew, long toskew
)
{
	float Cb, Cr;
	WORD j, k;

	Cb = Code2V(pp[n],   m_lpRefBlackWhite[2], m_lpRefBlackWhite[3], 127);
	Cr = Code2V(pp[n+1], m_lpRefBlackWhite[4], m_lpRefBlackWhite[5], 127);
	for (j = 0; j < ch; j++) {
		for (k = 0; k < cw; k++) {
			float Y, R, G, B;
			Y = Code2V(*pp++,
			    m_lpRefBlackWhite[0], m_lpRefBlackWhite[1], 255);
			R = Y + Cr*m_D1;
			B = Y + Cb*m_D3;
			G = Y - Cb*m_D4 - Cr*m_D2;
			cp[k] = PACK(CLAMP(R,0,255),
				     CLAMP(G,0,255),
				     CLAMP(B,0,255));
		}
		cp += w+toskew;
		pp += fromskew;
	}
}
#undef LumaBlue
#undef LumaGreen
#undef LumaRed
#undef CLAMP
#undef Code2V

/*
 * 8-bit packed YCbCr samples => RGB
 */
DECLAREContigPutFunc(geti_putcontig8bitYCbCrtile)
{
	WORD Coff = (WORD)(m_wYCbCrVertSampling * m_wYCbCrHorizSampling);
	LPDWORD tp;
	DWORD x;

	/* XXX adjust fromskew */
	while (h >= m_wYCbCrVertSampling) {
		tp = cp;
		for (x = w; x >= m_wYCbCrHorizSampling; x -= m_wYCbCrHorizSampling) {
			geti_putRGBContigYCbCrClump(tp, pp,
			    (DWORD)m_wYCbCrHorizSampling, (DWORD)m_wYCbCrVertSampling,
			    w, Coff, 0, toskew);
			tp += m_wYCbCrHorizSampling;
			pp += Coff+2;
		}
		if (x > 0) {
			geti_putRGBContigYCbCrClump(tp, pp,
			    x, (DWORD)m_wYCbCrVertSampling,
			    w, Coff, m_wYCbCrHorizSampling - x, toskew);
			pp += Coff+2;
		}
		cp += m_wYCbCrVertSampling*(w + toskew);
		pp += fromskew;
		h -= m_wYCbCrVertSampling;
	}
	if (h > 0) {
		tp = cp;
		for (x = w; x >= m_wYCbCrHorizSampling; x -= m_wYCbCrHorizSampling) {
			geti_putRGBContigYCbCrClump(tp, pp, 
				(DWORD)m_wYCbCrHorizSampling, (DWORD)h,
			    w, Coff, 0, toskew);
			tp += m_wYCbCrHorizSampling;
			pp += Coff+2;
		}
		if (x > 0)
			geti_putRGBContigYCbCrClump(tp, pp, 
				x, (DWORD)h,
			    w, Coff, m_wYCbCrHorizSampling - x, toskew);
	}
	Map; //UNUSED
}

/*
 * Select the appropriate conversion routine for packed data.
 */
geti_tileContigRoutine
SECTiff::geti_pickTileContigCase(TIFF* tif, LPRGBvalue Map)
{
	geti_tileContigRoutine put = 0;

	tif;	// unused param

	switch (m_wPhoto) {
	case PHOTOMETRIC_RGB:
		if (m_wBPS == 8)
			put = &SECTiff::geti_putRGBcontig8bittile;
		else
			put = &SECTiff::geti_putRGBcontig16bittile;
		break;
	case PHOTOMETRIC_SEPARATED:
		if (m_wBPS == 8)
			put = &SECTiff::geti_putRGBcontig8bitCMYKtile;
		break;
	case PHOTOMETRIC_PALETTE:
		switch (m_wBPS) {
		case 8:	put = &SECTiff::geti_put8bitcmaptile; break;
		case 4: put = &SECTiff::geti_put4bitcmaptile; break;
		case 2: put = &SECTiff::geti_put2bitcmaptile; break;
		case 1: put = &SECTiff::geti_put1bitcmaptile; break;
		}
		break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
		switch (m_wBPS) {
		case 8:	put = &SECTiff::geti_putgreytile; break;
		case 4: put = &SECTiff::geti_put4bitbwtile; break;
		case 2: put = &SECTiff::geti_put2bitbwtile; break;
		case 1: put = &SECTiff::geti_put1bitbwtile; break;
		}
		break;
	case PHOTOMETRIC_YCBCR:
		switch (m_wBPS) {
		case 8: put = &SECTiff::geti_putcontig8bitYCbCrtile; break;
		}
		break;
	}
	//AAB Out for link errors
	//if (put == 0)
	//	TIFFError(TIFFFileName(tif), "Can not handle format");
	Map; //UNUSED
	return (put);
	
}

/*
 * Select the appropriate conversion routine for unpacked data.
 *
 * NB: we assume that unpacked single channel data is directed
 *	 to the "packed routines.
 */
geti_tileSeparateRoutine
SECTiff::geti_pickTileSeparateCase(TIFF* tif, LPRGBvalue Map)
{
	geti_tileSeparateRoutine put = 0;

	switch (m_wPhoto) {
	case PHOTOMETRIC_RGB:
		if (m_wBPS == 8)
			put = &SECTiff::geti_putRGBseparate8bittile;
		else
			put = &SECTiff::geti_putRGBseparate16bittile;
		break;
	}

	Map; //UNUSED
	tif; //UNUSED    
		return (put);
}
