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
//  Description:	tif_stri.cpp
//		strip-organized image support routines
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
 * Compute which strip a (row,sample) value is in.
 */
tstrip_t
SECTiff::TIFFComputeStrip(TIFF* tif, uint32 row, tsample_t sample)
{
	TIFFDirectory *td = &tif->tif_dir;
	tstrip_t strip;

	strip = row / td->td_rowsperstrip;
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE) {
		if (sample >= td->td_samplesperpixel) {
			TIFFError(tif->tif_name,
			    _T("%u: Sample out of range, max %u"),
			    sample, td->td_samplesperpixel);
			return ((tstrip_t) 0);
		}
		strip += sample*td->td_stripsperimage;
	}
	return (strip);
}

/*
 * Compute how many strips are in an image.
 */
tstrip_t
SECTiff::TIFFNumberOfStrips(TIFF* tif)
{
	TIFFDirectory *td = &tif->tif_dir;
	tstrip_t nstrips;

	nstrips = (td->td_rowsperstrip == 0xffffffff ?
	     (td->td_imagelength != 0 ? 1 : 0) :
	     howmany(td->td_imagelength, td->td_rowsperstrip));
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		nstrips *= td->td_samplesperpixel;
	return (nstrips);
}

/*
 * Compute the # bytes in a variable height, row-aligned strip.
 */
tsize_t
SECTiff::TIFFVStripSize(TIFF* tif, uint32 nrows)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (nrows == (uint32) -1)
		nrows = td->td_imagelength;
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
		    roundup(td->td_imagewidth, td->td_ycbcrsubsampling[0]);
		tsize_t scanline = howmany(w*td->td_bitspersample, 8);
		tsize_t samplingarea =
		    td->td_ycbcrsubsampling[0]*td->td_ycbcrsubsampling[1];
		nrows = roundup(nrows, td->td_ycbcrsubsampling[1]);
		/* NB: don't need howmany here 'cuz everything is rounded */
		return (nrows*scanline + 2*(nrows*scanline / samplingarea));
	} else
#endif
		return (nrows * TIFFScanlineSize(tif));
}

/*
 * Compute the # bytes in a (row-aligned) strip.
 */
tsize_t
SECTiff::TIFFStripSize(TIFF* tif)
{
	return (TIFFVStripSize(tif, tif->tif_dir.td_rowsperstrip));
}
