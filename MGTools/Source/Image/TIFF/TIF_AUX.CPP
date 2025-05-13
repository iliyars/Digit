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
//  Description:	tif_aux.cpp
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
 *
 * Auxiliary Support Routines.
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

#ifdef COLORIMETRY_SUPPORT

#ifndef _INC_MATH
#include <math.h>
#endif

void
SECTiff::TIFFDefaultTransferFunction(TIFFDirectory* td)
{
	//AAB WORD **tf = td->td_transferfunction;
	LPPWORD tf = td->td_transferfunction;
	long i, n = 1<<td->td_bitspersample;

	//AAB tf[0] = (WORD *)_TIFFmalloc((size_t)(n * sizeof (WORD)));
	tf[0] = (LPWORD)_TIFFmalloc((size_t)(n * sizeof (WORD)));
	tf[0][0] = 0;
	for (i = 1; i < n; i++)
		tf[0][i] = (WORD)floor(65535.*pow(i/(n-1.), 2.2) + .5);
	if (td->td_samplesperpixel - td->td_extrasamples > 1) {
		//AAB tf[1] = (WORD *)_TIFFmalloc((size_t)(n * sizeof (WORD)));
		tf[1] = (LPWORD)_TIFFmalloc((size_t)(n * sizeof (WORD)));
#ifdef WIN32
		memcpy(tf[1], tf[0], (size_t)(n * sizeof (WORD)));
#else
		_fmemcpy(tf[1], tf[0], (size_t)(n * sizeof (WORD)));
#endif
		//AAB tf[2] = (WORD *)_TIFFmalloc((size_t)(n * sizeof (WORD)));
		tf[2] = (LPWORD)_TIFFmalloc((size_t)(n * sizeof (WORD)));
#ifdef WIN32
		memcpy(tf[2], tf[0], (size_t)(n * sizeof (WORD)));
#else
		_fmemcpy(tf[2], tf[0], (size_t)(n * sizeof (WORD)));
#endif
	}
}

void
SECTiff::TIFFDefaultRefBlackWhite(TIFFDirectory* td)
{
	DWORD i;

	//AAB td->td_refblackwhite = (float *)_TIFFmalloc((size_t)(6*sizeof (float)));
	td->td_refblackwhite = (LPFLOAT)_TIFFmalloc((size_t)(6*sizeof (float)));
	for (i = 0; i < 3; i++) {
	    td->td_refblackwhite[2*i+0] = (float) 0;
	    td->td_refblackwhite[2*i+1] = (float)(1L<<td->td_bitspersample);
	}
}
#endif

/*
 * Like TIFFGetField, but return any default
 * value if the tag is not present in the directory.
 *
 * NB:	We use the value in the directory, rather than
 *	explcit values so that defaults exist only one
 *	place in the library -- in TIFFDefaultDirectory.
 */
long 
SECTiff::TIFFVGetFieldDefaulted(TIFF* tif, ttag_t tag, va_list ap)
{
	TIFFDirectory *td = &tif->tif_dir;

	if (TIFFVGetField(tif, tag, ap))
		return (1);
	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
		*va_arg(ap, WORD *) = td->td_subfiletype;
		return (1);
	case TIFFTAG_BITSPERSAMPLE:
		*va_arg(ap, WORD *) = td->td_bitspersample;
		return (1);
	case TIFFTAG_THRESHHOLDING:
		*va_arg(ap, WORD *) = td->td_threshholding;
		return (1);
	case TIFFTAG_FILLORDER:
		*va_arg(ap, WORD *) = td->td_fillorder;
		return (1);
	case TIFFTAG_ORIENTATION:
		*va_arg(ap, WORD *) = td->td_orientation;
		return (1);
	case TIFFTAG_SAMPLESPERPIXEL:
		*va_arg(ap, WORD *) = td->td_samplesperpixel;
		return (1);
	case TIFFTAG_ROWSPERSTRIP:
		*va_arg(ap, DWORD *) = td->td_rowsperstrip;
		return (1);
	case TIFFTAG_MINSAMPLEVALUE:
	//AAB warning here for next 2 cases; original code did the mistype match..
		*va_arg(ap, WORD *) = (WORD)td->td_minsamplevalue;
		return (1);
	case TIFFTAG_MAXSAMPLEVALUE:
		*va_arg(ap, WORD *) = (WORD)td->td_maxsamplevalue;
		return (1);
	case TIFFTAG_PLANARCONFIG:
		*va_arg(ap, WORD *) = td->td_planarconfig;
		return (1);
	case TIFFTAG_GROUP4OPTIONS:
		*va_arg(ap, DWORD *) = td->td_group4options;
		return (1);
	case TIFFTAG_RESOLUTIONUNIT:
		*va_arg(ap, WORD *) = td->td_resolutionunit;
		return (1);
	case TIFFTAG_PREDICTOR:
		*va_arg(ap, WORD *) = td->td_predictor;
		return (1);
#ifdef CMYK_SUPPORT
	case TIFFTAG_DOTRANGE:
		*va_arg(ap, WORD *) = 0;
		*va_arg(ap, WORD *) = (WORD)((1<<td->td_bitspersample)-1);
		return (1);
	case TIFFTAG_INKSET:
		*va_arg(ap, WORD *) = td->td_inkset;
		return (1);
#endif
	case TIFFTAG_EXTRASAMPLES:
		*va_arg(ap, WORD *) = td->td_extrasamples;
		*va_arg(ap, WORD **) = td->td_sampleinfo;
		return (1);
	case TIFFTAG_MATTEING:
		*va_arg(ap, WORD *) =
		    (WORD)(td->td_extrasamples == 1 &&
		     td->td_sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
		return (1);
	case TIFFTAG_TILEDEPTH:
		*va_arg(ap, DWORD *) = td->td_tiledepth;
		return (1);
	case TIFFTAG_DATATYPE:
		*va_arg(ap, WORD *) = (WORD)(td->td_sampleformat-1);
		return (1);
	case TIFFTAG_IMAGEDEPTH:
	//AAB cast needed to null warning for original code..
		*va_arg(ap, WORD *) = (WORD)td->td_imagedepth;
		return (1);
#ifdef YCBCR_SUPPORT
	case TIFFTAG_YCBCRCOEFFICIENTS:
		if (!td->td_ycbcrcoeffs) {
			//AAB td->td_ycbcrcoeffs = (float *)
			td->td_ycbcrcoeffs = (LPFLOAT)
			    _TIFFmalloc((size_t)(3*sizeof (float)));
			/* defaults are from CCIR Recommendation 601-1 */
			td->td_ycbcrcoeffs[0] = (float).299;
			td->td_ycbcrcoeffs[1] = (float).587;
			td->td_ycbcrcoeffs[2] = (float).114;
		}
		*va_arg(ap, float **) = td->td_ycbcrcoeffs;
		return (1);
	case TIFFTAG_YCBCRSUBSAMPLING:
		*va_arg(ap, WORD *) = td->td_ycbcrsubsampling[0];
		*va_arg(ap, WORD *) = td->td_ycbcrsubsampling[1];
		return (1);
	case TIFFTAG_YCBCRPOSITIONING:
		*va_arg(ap, WORD *) = td->td_ycbcrpositioning;
		return (1);
#endif
#ifdef COLORIMETRY_SUPPORT
	case TIFFTAG_TRANSFERFUNCTION:
		if (!td->td_transferfunction[0])
			TIFFDefaultTransferFunction(td);
		*va_arg(ap, WORD **) = td->td_transferfunction[0];
		if (td->td_samplesperpixel - td->td_extrasamples > 1) {
			*va_arg(ap, WORD **) = td->td_transferfunction[1];
			*va_arg(ap, WORD **) = td->td_transferfunction[2];
		}
		return (1);
	case TIFFTAG_REFERENCEBLACKWHITE:
		if (!td->td_refblackwhite)
			TIFFDefaultRefBlackWhite(td);
		*va_arg(ap, float **) = td->td_refblackwhite;
		return (1);
#endif
	}
	return (0);
}

/*
 * Like TIFFGetField, but return any default
 * value if the tag is not present in the directory.
 */
long 
SECTiff::TIFFGetFieldDefaulted(TIFF* tif, ttag_t tag, ...)
{
	long ok;
	va_list ap;

	va_start(ap, tag);
	ok =  TIFFVGetFieldDefaulted(tif, tag, ap);
	va_end(ap);
	return (ok);
}
