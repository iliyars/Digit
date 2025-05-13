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
//  Description:	tif_tile.cpp
//		tiled image support routines
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


//#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

/*
 * Compute which tile an (x,y,z,s) value is in.
 */
ttile_t
SECTiff::TIFFComputeTile(TIFF* tif, DWORD x, DWORD y, DWORD z, tsample_t s)
{
	TIFFDirectory *td = &tif->tif_dir;
	DWORD dx = td->td_tilewidth;
	DWORD dy = td->td_tilelength;
	DWORD dz = td->td_tiledepth;
	ttile_t tile = 1;

	if (td->td_imagedepth == 1)
		z = 0;
	if (dx == (DWORD) -1)
		dx = td->td_imagewidth;
	if (dy == (DWORD) -1)
		dy = td->td_imagelength;
	if (dz == (DWORD) -1)
		dz = td->td_imagedepth;
	if (dx != 0 && dy != 0 && dz != 0) {
		DWORD xpt = howmany(td->td_imagewidth, dx); 
		DWORD ypt = howmany(td->td_imagelength, dy); 
		DWORD zpt = howmany(td->td_imagedepth, dz); 

		if (td->td_planarconfig == PLANARCONFIG_SEPARATE) 
			tile = (xpt*ypt*zpt)*s +
			     (xpt*ypt)*(z/dz) +
			     xpt*(y/dy) +
			     x/dx;
		else
			tile = (xpt*ypt)*(z/dz) + xpt*(y/dy) + x/dx + s;
	}
	return (tile);
}

/*
 * Check an (x,y,z,s) coordinate
 * against the image bounds.
 */
long
SECTiff::TIFFCheckTile(TIFF* tif, DWORD x, DWORD y, DWORD z, tsample_t s)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (x >= td->td_imagewidth) {
		TIFFError(tif->tif_name, _T("Col %ld out of range, max %lu"),
		    (long) x, (DWORD) td->td_imagewidth);
		return (0);
	}
	if (y >= td->td_imagelength) {
		TIFFError(tif->tif_name, _T("Row %ld out of range, max %lu"),
		    (long) y, (DWORD) td->td_imagelength);
		return (0);
	}
	if (z >= td->td_imagedepth) {
		TIFFError(tif->tif_name, _T("Depth %ld out of range, max %lu"),
		    (long) z, (DWORD) td->td_imagedepth);
		return (0);
	}
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE &&
	    s >= td->td_samplesperpixel) {
		TIFFError(tif->tif_name, _T("Sample %d out of range, max %u"),
		    (int) s, td->td_samplesperpixel);
		return (0);
	}
	return (1);
}

/*
 * Compute how many tiles are in an image.
 */
ttile_t
SECTiff::TIFFNumberOfTiles(TIFF* tif)
{
	TIFFDirectory *td = &tif->tif_dir;
	DWORD dx = td->td_tilewidth;
	DWORD dy = td->td_tilelength;
	DWORD dz = td->td_tiledepth;
	ttile_t ntiles;

	if (dx == (DWORD) -1)
		dx = td->td_imagewidth;
	if (dy == (DWORD) -1)
		dy = td->td_imagelength;
	if (dz == (DWORD) -1)
		dz = td->td_imagedepth;
	ntiles = (dx != 0 && dy != 0 && dz != 0) ?
	    (howmany(td->td_imagewidth, dx) * howmany(td->td_imagelength, dy) *
		howmany(td->td_imagedepth, dz)) :
	    0;
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		ntiles *= td->td_samplesperpixel;
	return (ntiles);
}

/*
 * Compute the # bytes in each row of a tile.
 */
tsize_t
SECTiff::TIFFTileRowSize(TIFF* tif)
{
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t rowsize;
	
	if (td->td_tilelength == 0 || td->td_tilewidth == 0)
		return ((tsize_t) 0);
	rowsize = td->td_bitspersample * td->td_tilewidth;
	if (td->td_planarconfig == PLANARCONFIG_CONTIG)
		rowsize *= td->td_samplesperpixel;
	return (howmany(rowsize, 8));
}

/*
 * Compute the # bytes in a variable length, row-aligned tile.
 */
tsize_t
SECTiff::TIFFVTileSize(TIFF* tif, DWORD nrows)
{
	TIFFDirectory *td = &tif->tif_dir;
	tsize_t tilesize;

	if (td->td_tilelength == 0 || td->td_tilewidth == 0 ||
	    td->td_tiledepth == 0)
		return ((tsize_t) 0);
#ifdef YCBCR_SUPPORT
	if (td->td_planarconfig == PLANARCONFIG_CONTIG &&
	    td->td_photometric == PHOTOMETRIC_YCBCR) {
		/*
		 * Packed YCbCr data contain one Cb+Cr for every
		 * HorizontalSampling*VerticalSampling Y values.
		 * Must also roundup width and height when calculating
		 * since images that are not a multiple of the
		 * horizontal/vertical subsampling area include
		 * YCbCr data for the extended image.
		 */
		tsize_t w =
		    roundup(td->td_tilewidth, td->td_ycbcrsubsampling[0]);
		tsize_t rowsize = howmany(w*td->td_bitspersample, 8);
		tsize_t samplingarea =
		    td->td_ycbcrsubsampling[0]*td->td_ycbcrsubsampling[1];
		nrows = roundup(nrows, td->td_ycbcrsubsampling[1]);
		/* NB: don't need howmany here 'cuz everything is rounded */
		tilesize = nrows*rowsize + 2*(nrows*rowsize / samplingarea);
	} else
#endif
		tilesize = nrows * TIFFTileRowSize(tif);
	return (tilesize * td->td_tiledepth);
}

/*
 * Compute the # bytes in a row-aligned tile.
 */
tsize_t
SECTiff::TIFFTileSize(TIFF* tif)
{
	return (TIFFVTileSize(tif, tif->tif_dir.td_tilelength));
}
