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
//  Description:	tif_dir.cpp
//		directory tag get&set routines + misc. additions
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//


#include "..\..\..\stdafx.h"

#ifndef _WINDOWS_
#include <windows.h>
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> //AAB
#endif // _INC_WINDOWSX

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif


//AAB
#ifndef _INC_MALLOC
#include <malloc.h>
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

static void
setString(TCHAR **cpp, TCHAR *cp)
{
	if (*cpp)
		free(*cpp), *cpp = 0;
	if (cp) {
		size_t len = (_tcslen(cp)+1) * sizeof(TCHAR);
		//AAB *cpp = (char *)malloc(len);
		*cpp = (TCHAR *)GlobalAllocPtr(GHND, len);
		if (*cpp)
			memcpy(*cpp, cp, len);
	}
}

void
SECTiff::setShortArray(LPPWORD wpp, LPWORD wp, long n)
{
	if (*wpp)
		_TIFFfree((LPBYTE)*wpp), *wpp = 0;
	n *= sizeof (WORD);
	if (wp ) 
	{
	   *wpp = (LPWORD)_TIFFmalloc((size_t)n);
	   if (*wpp) 
	   {
#ifdef WIN32
		memcpy(*wpp, wp, (size_t)n);
#else
		_fmemcpy(*wpp, wp, (size_t)n);
#endif
       }
	}
}

void
SECTiff::setLongArray(LPPDWORD wpp, LPDWORD wp, long n)
{
	if (*wpp)
		_TIFFfree((LPBYTE)*wpp), *wpp = 0;
	n *= sizeof (DWORD);
	*wpp = (LPDWORD)_TIFFmalloc((size_t)n);
	if (wp && *wpp)
#ifdef WIN32
		memcpy(*wpp, wp, (size_t)n);
#else
		_fmemcpy(*wpp, wp, (size_t)n);
#endif
}

void
SECTiff::setFloatArray(LPPFLOAT wpp, LPFLOAT wp, long n)
{
	if (*wpp)
		_TIFFfree((LPBYTE)*wpp), *wpp = 0;
	n *= sizeof (float);
	*wpp = (LPFLOAT)_TIFFmalloc((size_t)n);
	if (wp && *wpp)
#ifdef WIN32
		memcpy(*wpp, wp, (size_t)n);
#else
		_fmemcpy(*wpp, wp, (size_t)n);
#endif
}

#ifdef JPEG_SUPPORT
/*
 * Install a JPEG Quantization table.
 * Note that we reorder the elements
 * of the array in the zig-zag order
 * that is expected by the compression code
 * and that is to be stored in the file.
 */
void
SECTiff::setJPEGQTable(LPPPBYTE wpp, LPPBYTE wp, long nc)
{
	static BYTE zigzag[64] = {
	    0,  1,  5,  6, 14, 15, 27, 28,
	    2,  4,  7, 13, 16, 26, 29, 42,
	    3,  8, 12, 17, 25, 30, 41, 43,
	    9, 11, 18, 24, 31, 40, 44, 53,
	   10, 19, 23, 32, 39, 45, 52, 54,
	   20, 22, 33, 38, 46, 51, 55, 60,
	   21, 34, 37, 47, 50, 56, 59, 61,
	   35, 36, 48, 49, 57, 58, 62, 63
	};
	LPBYTE tab;
	long i, j;

	if (*wpp)
		_TIFFfree((LPBYTE)*wpp), *wpp = 0;
	*wpp = (LPPBYTE)
	    _TIFFmalloc((size_t)(nc * (sizeof (LPBYTE) + 64*sizeof (BYTE))));
	tab = (((LPBYTE)*wpp) + nc*sizeof (LPBYTE));
	for (i = 0; i < nc; i++) {
		(*wpp)[i] = (LPBYTE)tab;
		for (j = 0; j < 64; j++)
			tab[zigzag[j]] = wp[i][j];
		tab += 64*sizeof (BYTE);
	}
}

/*
 * Install a JPEG Coefficient table.
 */
void
SECTiff::setJPEGCTable(LPPPBYTE cpp, LPPBYTE cp, long nc)
{
	LPBYTE tab;
	long i, j, nw;

	if (*cpp)
		_TIFFfree(*cpp), *cpp = 0;
	/*
	 * Calculate the size of the table by counting
	 * the number of codes specified in the bits array.
	 */
	nw = 0;
	for (i = 0; i < nc; i++) {
		nw += 16;		/* 16 bytes for bits array */
		for (j = 0; j < 16; j++)/* sum up count of codes */
			nw += cp[i][j];
	}
	*cpp = (LPPBYTE)_TIFFmalloc((size_t)(nc*sizeof (LPBYTE) + nw));
	tab = ((LPBYTE)*cpp) + nc*sizeof (LPBYTE);
	/*
	 * Setup internal array and copy user data.
	 */
	for (i = 0; i < nc; i++) {
		(*cpp)[i] = tab;
		for (nw = 16, j = 0; j < 16; j++)
			nw += cp[i][j];
#ifdef WIN32
		memcpy(tab, cp[i], (size_t)nw);
#else
		_fmemcpy(tab, cp[i], (size_t)nw);
#endif
		tab += nw;
	}
}
#endif

/*
 * Install extra samples information.
 */
long
SECTiff::setExtraSamples(TIFFDirectory *td, va_list ap, LPWORD v)
{
	LPWORD va;
	WORD i;

	*v = va_arg(ap, WORD);
	if (*v > td->td_samplesperpixel)
		return (0);
	va = va_arg(ap, LPWORD);
	if (va == NULL)			/* typically missing param */
		return (0);
	for (i = 0; i < *v; i++)
		if (va[i] > EXTRASAMPLE_UNASSALPHA)
			return (0);
	td->td_extrasamples = *v;
	setShortArray(&td->td_sampleinfo, va, td->td_extrasamples);
	return (1);
}

long 
SECTiff::TIFFSetField1(TIFF* tif, ttag_t tag, va_list ap)
{
	TIFFDirectory *td = &tif->tif_dir;
	long status = 1;
	DWORD v32;
	WORD i, v;

	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
	//AAB cast needed for original
		td->td_subfiletype = (WORD)va_arg(ap, DWORD);
		break;
	case TIFFTAG_IMAGEWIDTH:
		td->td_imagewidth = va_arg(ap, DWORD);
		break;
	case TIFFTAG_IMAGELENGTH:
		td->td_imagelength = va_arg(ap, DWORD);
		break;
	case TIFFTAG_BITSPERSAMPLE:
		td->td_bitspersample = (WORD)va_arg(ap, long);	//AAB BIG CHECK
		/*
		 * If the data require post-decoding processing
		 * to byte-swap samples, set it up here.  Note
		 * that since tags are required to be ordered,
		 * compression code can override this behaviour
		 * in the setup method if it wants to roll the
		 * post decoding work in with its normal work.
		 */
		if (tif->tif_flags & TIFF_SWAB) {
			if (td->td_bitspersample == 16)
				tif->tif_postdecode = &SECTiff::TIFFSwab16BitData;
			else if (td->td_bitspersample == 32)
				tif->tif_postdecode = &SECTiff::TIFFSwab32BitData;
		}
		break;
	case TIFFTAG_COMPRESSION:
		v = (WORD)(va_arg(ap, long) & 0xffff);
		/*
		 * If we're changing the compression scheme,
		 * the notify the previous module so that it
		 * can cleanup any state it's setup.
		 */
		if (TIFFFieldSet(tif, FIELD_COMPRESSION)) {
			if (td->td_compression == v)
				break;
			if (tif->tif_cleanup)
			{
				// (*tif->tif_cleanup)(tif);
				m_fpVoid=tif->tif_cleanup;
				(this->*m_fpVoid)(tif);
			}
		}
		/*
		 * Setup new compression routine state.
		 */
		status = TIFFSetCompressionScheme(tif, v);
		if (status)
			td->td_compression = v;
		break;
	case TIFFTAG_PHOTOMETRIC:
		td->td_photometric = va_arg(ap, WORD);
		break;
	case TIFFTAG_THRESHHOLDING:
		td->td_threshholding = va_arg(ap, WORD);
		break;
	case TIFFTAG_FILLORDER:
		v = va_arg(ap, WORD);
		if (v != FILLORDER_LSB2MSB && v != FILLORDER_MSB2LSB)
			goto badvalue;
		td->td_fillorder = v;
		break;
	case TIFFTAG_DOCUMENTNAME:
		setString(&td->td_documentname, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_ARTIST:
		setString(&td->td_artist, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_DATETIME:
		setString(&td->td_datetime, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_HOSTCOMPUTER:
		setString(&td->td_hostcomputer, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_IMAGEDESCRIPTION:
		setString(&td->td_imagedescription, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_MAKE:
		setString(&td->td_make, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_MODEL:
		setString(&td->td_model, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_SOFTWARE:
		setString(&td->td_software, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_ORIENTATION:
		v = va_arg(ap, WORD);
		if (v < ORIENTATION_TOPLEFT || ORIENTATION_LEFTBOT < v) {
			TIFFWarning(tif->tif_name,
			    _T("Bad value %ld for \"%s\" tag ignored"),
			    v, TIFFFieldWithTag(tag)->field_name);
		} else
			td->td_orientation = v;
		break;
	case TIFFTAG_SAMPLESPERPIXEL:
		/* XXX should cross check -- e.g. if pallette, then 1 */
		v = va_arg(ap, WORD);
		if (v == 0)
			goto badvalue;
		td->td_samplesperpixel = v;
		break;
	case TIFFTAG_ROWSPERSTRIP:
		v32 = va_arg(ap, DWORD);
		if (v32 == 0)
			goto badvalue32;
		td->td_rowsperstrip = v32;
		if (!TIFFFieldSet(tif, FIELD_TILEDIMENSIONS)) {
			td->td_tilelength = v32;
			td->td_tilewidth = td->td_imagewidth;
		}
		break;
	case TIFFTAG_MINSAMPLEVALUE:
		td->td_minsamplevalue = va_arg(ap, long) & 0xffff;
		break;
	case TIFFTAG_MAXSAMPLEVALUE:
		td->td_maxsamplevalue = va_arg(ap, long) & 0xffff;
		break;
	case TIFFTAG_XRESOLUTION:
		td->td_xresolution = (float)va_arg(ap, dblparam_t);
		break;
	case TIFFTAG_YRESOLUTION:
		td->td_yresolution = (float)va_arg(ap, dblparam_t);
		break;
	case TIFFTAG_PLANARCONFIG:
		v = (WORD)va_arg(ap, long);	//AAB TEST!  Used to be WORD...
		if (v != PLANARCONFIG_CONTIG && v != PLANARCONFIG_SEPARATE)
			goto badvalue;
		td->td_planarconfig = (WORD)v;
		break;
	case TIFFTAG_PAGENAME:
		setString(&td->td_pagename, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_XPOSITION:
		td->td_xposition = (float)va_arg(ap, dblparam_t);
		break;
	case TIFFTAG_YPOSITION:
		td->td_yposition = (float)va_arg(ap, dblparam_t);
		break;
	case TIFFTAG_GROUP3OPTIONS:
		td->td_group3options = va_arg(ap, DWORD);
		break;
	case TIFFTAG_GROUP4OPTIONS:
		td->td_group4options = va_arg(ap, DWORD);
		break;
	case TIFFTAG_RESOLUTIONUNIT:
		v = va_arg(ap, WORD);
		if (v < RESUNIT_NONE || RESUNIT_CENTIMETER < v)
			goto badvalue;
		td->td_resolutionunit = v;
		break;
	case TIFFTAG_PAGENUMBER:
		td->td_pagenumber[0] = va_arg(ap, WORD);
		td->td_pagenumber[1] = va_arg(ap, WORD);
		break;
	case TIFFTAG_HALFTONEHINTS:
		td->td_halftonehints[0] = va_arg(ap, WORD);
		td->td_halftonehints[1] = va_arg(ap, WORD);
		break;
	case TIFFTAG_COLORMAP:
		v = (WORD)(1<<td->td_bitspersample);
		setShortArray(&td->td_colormap[0], va_arg(ap, WORD *), v);
		setShortArray(&td->td_colormap[1], va_arg(ap, WORD *), v);
		setShortArray(&td->td_colormap[2], va_arg(ap, WORD *), v);
		break;
	case TIFFTAG_PREDICTOR:
		td->td_predictor = va_arg(ap, WORD);
		break;
	case TIFFTAG_EXTRASAMPLES:
		if (!setExtraSamples(td, ap, &v))
			goto badvalue;
		break;
	case TIFFTAG_MATTEING:
		td->td_extrasamples = (WORD)(va_arg(ap, long) != 0);
		if (td->td_extrasamples) {
			WORD sv = EXTRASAMPLE_ASSOCALPHA;
			setShortArray(&td->td_sampleinfo, &sv, 1);
		}
		break;
	case TIFFTAG_BADFAXLINES:
		td->td_badfaxlines = va_arg(ap, DWORD);
		break;
	case TIFFTAG_CLEANFAXDATA:
		td->td_cleanfaxdata = va_arg(ap, WORD);
		break;
	case TIFFTAG_CONSECUTIVEBADFAXLINES:
	//AAB: mismatch from orig..
		td->td_badfaxrun = (WORD)va_arg(ap, DWORD);
		break;
	case TIFFTAG_TILEWIDTH:
		v32 = va_arg(ap, DWORD);
		if (v32 % 16)
			goto badvalue32;
		td->td_tilewidth = v32;
		tif->tif_flags |= TIFF_ISTILED;
		break;
	case TIFFTAG_TILELENGTH:
		v32 = va_arg(ap, DWORD);
		if (v32 % 16)
			goto badvalue32;
		td->td_tilelength = v32;
		tif->tif_flags |= TIFF_ISTILED;
		break;
	case TIFFTAG_TILEDEPTH:
		v32 = va_arg(ap, DWORD);
		if (v32 == 0)
			goto badvalue32;
		td->td_tiledepth = v32;
		break;
	case TIFFTAG_DATATYPE:
	case TIFFTAG_SAMPLEFORMAT:
		v = va_arg(ap, WORD);
		if (tag == TIFFTAG_DATATYPE && v == 0)
			v = SAMPLEFORMAT_VOID;
		if (v < SAMPLEFORMAT_UINT || SAMPLEFORMAT_VOID < v)
			goto badvalue;
		td->td_sampleformat = v;
		break;
	case TIFFTAG_IMAGEDEPTH:
		td->td_imagedepth = va_arg(ap, DWORD);
		break;
#ifdef YCBCR_SUPPORT
	case TIFFTAG_YCBCRCOEFFICIENTS:
		setFloatArray(&td->td_ycbcrcoeffs, va_arg(ap, float *), 3);
		break;
	case TIFFTAG_YCBCRPOSITIONING:
		td->td_ycbcrpositioning = va_arg(ap, WORD);
		break;
	case TIFFTAG_YCBCRSUBSAMPLING:
		td->td_ycbcrsubsampling[0] = va_arg(ap, WORD);
		td->td_ycbcrsubsampling[1] = va_arg(ap, WORD);
		break;
#endif
#ifdef JPEG_SUPPORT
	case TIFFTAG_JPEGPROC:
		td->td_jpegproc = va_arg(ap, WORD);
		break;
	case TIFFTAG_JPEGRESTARTINTERVAL:
		td->td_jpegrestartinterval = va_arg(ap, WORD);
		break;
	case TIFFTAG_JPEGQTABLES:
		setJPEGQTable(&td->td_qtab, va_arg(ap, LPPBYTE),
		    td->td_samplesperpixel);
		break;
	case TIFFTAG_JPEGDCTABLES:
		setJPEGCTable(&td->td_dctab, va_arg(ap, LPPBYTE),
		    td->td_samplesperpixel);
		break;
	case TIFFTAG_JPEGACTABLES:
		setJPEGCTable(&td->td_actab, va_arg(ap, LPPBYTE),
		    td->td_samplesperpixel);
		break;
#endif
#ifdef COLORIMETRY_SUPPORT
	case TIFFTAG_WHITEPOINT:
		setFloatArray(&td->td_whitepoint, va_arg(ap, float *), 2);
		break;
	case TIFFTAG_PRIMARYCHROMATICITIES:
		setFloatArray(&td->td_primarychromas, va_arg(ap, float *), 6);
		break;
	case TIFFTAG_TRANSFERFUNCTION:
		v = (WORD)((td->td_samplesperpixel - td->td_extrasamples) > 1 ? 3 : 1);
		for (i = 0; i < v; i++)
			setShortArray(&td->td_transferfunction[i],
			    va_arg(ap, WORD *), 1L<<td->td_bitspersample);
		break;
	case TIFFTAG_REFERENCEBLACKWHITE:
		/* XXX should check for null range */
		setFloatArray(&td->td_refblackwhite, va_arg(ap, float *), 6);
		break;
#endif
#ifdef CMYK_SUPPORT
	case TIFFTAG_INKSET:
		td->td_inkset = va_arg(ap, WORD);
		break;
	case TIFFTAG_DOTRANGE:
		/* XXX should check for null range */
		td->td_dotrange[0] = va_arg(ap, WORD);
		td->td_dotrange[1] = va_arg(ap, WORD);
		break;
	case TIFFTAG_INKNAMES:
		setString(&td->td_inknames, va_arg(ap, TCHAR *));
		break;
	case TIFFTAG_TARGETPRINTER:
		setString(&td->td_targetprinter, va_arg(ap, TCHAR *));
		break;
#endif
	default:
		TIFFError(tif->tif_name,
		    _T("Internal error, tag value botch, tag \"%s\""),
		    TIFFFieldWithTag(tag)->field_name);
		status = 0;
		break;
	}
	if (status) {
		TIFFSetFieldBit(tif, TIFFFieldWithTag(tag)->field_bit);
		tif->tif_flags |= TIFF_DIRTYDIRECT;
	}
	va_end(ap);
	return (status);
badvalue:
	TIFFError(tif->tif_name, _T("%d: Bad value for \"%s\""), v,
	    TIFFFieldWithTag(tag)->field_name);
	va_end(ap);
	return (0);
badvalue32:
	TIFFError(tif->tif_name, _T("%ld: Bad value for \"%s\""), v32,
	    TIFFFieldWithTag(tag)->field_name);
	va_end(ap);
	return (0);
}

/*
 * Return 1/0 according to whether or not
 * it is permissible to set the tag's value.
 * Note that we allow ImageLength to be changed
 * so that we can append and extend to images.
 * Any other tag may not be altered once writing
 * has commenced, unless its value has no effect
 * on the format of the data that is written.
 */
long 
SECTiff::OkToChangeTag(TIFF* tif, ttag_t tag)
{
	if (tag != TIFFTAG_IMAGELENGTH &&
	    (tif->tif_flags & TIFF_BEENWRITING)) {
		const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);
		/*
		 * Consult info table to see if tag can be changed
		 * after we've started writing.  We only allow changes
		 * to those tags that don't/shouldn't affect the
		 * compression and/or format of the data.
		 */
		if (fip && !fip->field_oktochange)
			return (0);
	}
	return (1);
}

/*
 * Record the value of a field in the
 * internal directory structure.  The
 * field will be written to the file
 * when/if the directory structure is
 * updated.
 */
long
SECTiff::TIFFSetField(TIFF* tif, ttag_t tag, ...)
{
	long status = 0;

	if (OkToChangeTag(tif, tag)) {
		va_list ap;

		va_start(ap, tag);
		status = TIFFSetField1(tif, tag, ap);
		va_end(ap);
	} else {
		const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);
		if (fip)
			TIFFError(_T("TIFFSetField"),
			    _T("%s: Cannot modify tag \"%s\" while writing"),
			    tif->tif_name, fip->field_name);
	}
	return (status);
}

/*
 * Like TIFFSetField, but taking a varargs
 * parameter list.  This routine is useful
 * for building higher-level interfaces on
 * top of the library.
 */
long
SECTiff::TIFFVSetField(TIFF* tif, ttag_t tag, va_list ap)
{
	long status = 0;

	if (!OkToChangeTag(tif, tag)) {
		const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);
		if (fip)
			TIFFError(_T("TIFFVSetField"),
			    _T("%s: Cannot modify tag \"%s\" while writing"),
			    tif->tif_name, fip->field_name);
	} else
		status = TIFFSetField1(tif, tag, ap);
	return (status);
}

void
SECTiff::TIFFGetField1(TIFFDirectory* td, ttag_t tag, va_list ap)
{
	switch (tag) {
	case TIFFTAG_SUBFILETYPE:
		*va_arg(ap, DWORD *) = td->td_subfiletype;
		break;
	case TIFFTAG_IMAGEWIDTH:
		*va_arg(ap, DWORD*) = td->td_imagewidth;
		break;
	case TIFFTAG_IMAGELENGTH:
		*va_arg(ap, DWORD*) = td->td_imagelength;
		break;
	case TIFFTAG_BITSPERSAMPLE:
		*va_arg(ap, WORD*) = td->td_bitspersample;
		break;
	case TIFFTAG_COMPRESSION:
		*va_arg(ap, WORD*) = td->td_compression;
		break;
	case TIFFTAG_PHOTOMETRIC:
		*va_arg(ap, WORD*) = td->td_photometric;
		break;
	case TIFFTAG_THRESHHOLDING:
		*va_arg(ap, WORD*) = td->td_threshholding;
		break;
	case TIFFTAG_FILLORDER:
		*va_arg(ap, WORD*) = td->td_fillorder;
		break;
	case TIFFTAG_DOCUMENTNAME:
		*va_arg(ap, TCHAR **) = td->td_documentname;
		break;
	case TIFFTAG_ARTIST:
		*va_arg(ap, TCHAR **) = td->td_artist;
		break;
	case TIFFTAG_DATETIME:
		*va_arg(ap, TCHAR **) = td->td_datetime;
		break;
	case TIFFTAG_HOSTCOMPUTER:
		*va_arg(ap, TCHAR **) = td->td_hostcomputer;
		break;
	case TIFFTAG_IMAGEDESCRIPTION:
		*va_arg(ap, TCHAR **) = td->td_imagedescription;
		break;
	case TIFFTAG_MAKE:
		*va_arg(ap, TCHAR **) = td->td_make;
		break;
	case TIFFTAG_MODEL:
		*va_arg(ap, TCHAR **) = td->td_model;
		break;
	case TIFFTAG_SOFTWARE:
		*va_arg(ap, TCHAR **) = td->td_software;
		break;
	case TIFFTAG_ORIENTATION:
		*va_arg(ap, WORD*) = td->td_orientation;
		break;
	case TIFFTAG_SAMPLESPERPIXEL:
		*va_arg(ap, WORD*) = td->td_samplesperpixel;
		break;
	case TIFFTAG_ROWSPERSTRIP:
		*va_arg(ap, DWORD*) = td->td_rowsperstrip;
		break;
	case TIFFTAG_MINSAMPLEVALUE:
		*va_arg(ap, WORD*) = (WORD)td->td_minsamplevalue;
		break;
	case TIFFTAG_MAXSAMPLEVALUE:
		*va_arg(ap, WORD*) = (WORD)td->td_maxsamplevalue;
		break;
	case TIFFTAG_XRESOLUTION:
		*va_arg(ap, float *) = td->td_xresolution;
		break;
	case TIFFTAG_YRESOLUTION:
		*va_arg(ap, float *) = td->td_yresolution;
		break;
	case TIFFTAG_PLANARCONFIG:
		*va_arg(ap, WORD*) = td->td_planarconfig;
		break;
	case TIFFTAG_XPOSITION:
		*va_arg(ap, float *) = td->td_xposition;
		break;
	case TIFFTAG_YPOSITION:
		*va_arg(ap, float *) = td->td_yposition;
		break;
	case TIFFTAG_PAGENAME:
		*va_arg(ap, TCHAR **) = td->td_pagename;
		break;
	case TIFFTAG_GROUP3OPTIONS:
		*va_arg(ap, DWORD*) = td->td_group3options;
		break;
	case TIFFTAG_GROUP4OPTIONS:
		*va_arg(ap, DWORD*) = td->td_group4options;
		break;
	case TIFFTAG_RESOLUTIONUNIT:
		*va_arg(ap, WORD*) = td->td_resolutionunit;
		break;
	case TIFFTAG_PAGENUMBER:
		*va_arg(ap, WORD*) = td->td_pagenumber[0];
		*va_arg(ap, WORD*) = td->td_pagenumber[1];
		break;
	case TIFFTAG_HALFTONEHINTS:
		*va_arg(ap, WORD*) = td->td_halftonehints[0];
		*va_arg(ap, WORD*) = td->td_halftonehints[1];
		break;
	case TIFFTAG_COLORMAP:
		*va_arg(ap, WORD**) = td->td_colormap[0];
		*va_arg(ap, WORD**) = td->td_colormap[1];
		*va_arg(ap, WORD**) = td->td_colormap[2];
		break;
	case TIFFTAG_PREDICTOR:
		*va_arg(ap, WORD*) = td->td_predictor;
		break;
	case TIFFTAG_STRIPOFFSETS:
	case TIFFTAG_TILEOFFSETS:
		*va_arg(ap, DWORD**) = td->td_stripoffset;
		break;
	case TIFFTAG_STRIPBYTECOUNTS:
	case TIFFTAG_TILEBYTECOUNTS:
		*va_arg(ap, DWORD**) = td->td_stripbytecount;
		break;
	case TIFFTAG_MATTEING:
		*va_arg(ap, WORD*) =
		    (WORD)(td->td_extrasamples == 1 &&
		     td->td_sampleinfo[0] == EXTRASAMPLE_ASSOCALPHA);
		break;
	case TIFFTAG_EXTRASAMPLES:
		*va_arg(ap, WORD*) = td->td_extrasamples;
		*va_arg(ap, WORD**) = td->td_sampleinfo;
		break;
	case TIFFTAG_BADFAXLINES:
		*va_arg(ap, DWORD*) = td->td_badfaxlines;
		break;
	case TIFFTAG_CLEANFAXDATA:
		*va_arg(ap, WORD*) = td->td_cleanfaxdata;
		break;
	case TIFFTAG_CONSECUTIVEBADFAXLINES:
		*va_arg(ap, DWORD*) = td->td_badfaxrun;
		break;
	case TIFFTAG_TILEWIDTH:
		*va_arg(ap, DWORD*) = td->td_tilewidth;
		break;
	case TIFFTAG_TILELENGTH:
		*va_arg(ap, DWORD*) = td->td_tilelength;
		break;
	case TIFFTAG_TILEDEPTH:
		*va_arg(ap, DWORD*) = td->td_tiledepth;
		break;
	case TIFFTAG_DATATYPE:
		*va_arg(ap, WORD*) = (USHORT)(
		    (td->td_sampleformat == SAMPLEFORMAT_VOID ?
			0 : td->td_sampleformat));
		break;
	case TIFFTAG_SAMPLEFORMAT:
		*va_arg(ap, WORD*) = td->td_sampleformat;
		break;
	case TIFFTAG_IMAGEDEPTH:
		*va_arg(ap, DWORD*) = td->td_imagedepth;
		break;
#ifdef YCBCR_SUPPORT
	case TIFFTAG_YCBCRCOEFFICIENTS:
		*va_arg(ap, float **) = td->td_ycbcrcoeffs;
		break;
	case TIFFTAG_YCBCRPOSITIONING:
		*va_arg(ap, WORD*) = td->td_ycbcrpositioning;
		break;
	case TIFFTAG_YCBCRSUBSAMPLING:
		*va_arg(ap, WORD*) = td->td_ycbcrsubsampling[0];
		*va_arg(ap, WORD*) = td->td_ycbcrsubsampling[1];
		break;
#endif
#ifdef JPEG_SUPPORT
	case TIFFTAG_JPEGPROC:
		*va_arg(ap, WORD*) = td->td_jpegproc;
		break;
	case TIFFTAG_JPEGRESTARTINTERVAL:
		*va_arg(ap, WORD*) = td->td_jpegrestartinterval;
		break;
	case TIFFTAG_JPEGQTABLES:
		*va_arg(ap, LPPPBYTE) = td->td_qtab;
		break;
	case TIFFTAG_JPEGDCTABLES:
		*va_arg(ap, LPPPBYTE) = td->td_dctab;
		break;
	case TIFFTAG_JPEGACTABLES:
		*va_arg(ap, LPPPBYTE) = td->td_actab;
		break;
#endif
#ifdef COLORIMETRY_SUPPORT
	case TIFFTAG_WHITEPOINT:
		*va_arg(ap, float **) = td->td_whitepoint;
		break;
	case TIFFTAG_PRIMARYCHROMATICITIES:
		*va_arg(ap, float **) = td->td_primarychromas;
		break;
	case TIFFTAG_TRANSFERFUNCTION:
		*va_arg(ap, WORD**) = td->td_transferfunction[0];
		if (td->td_samplesperpixel - td->td_extrasamples > 1) {
			*va_arg(ap, WORD**) = td->td_transferfunction[1];
			*va_arg(ap, WORD**) = td->td_transferfunction[2];
		}
		break;
	case TIFFTAG_REFERENCEBLACKWHITE:
		*va_arg(ap, float **) = td->td_refblackwhite;
		break;
#endif
#ifdef CMYK_SUPPORT
	case TIFFTAG_INKSET:
		*va_arg(ap, WORD*) = td->td_inkset;
		break;
	case TIFFTAG_DOTRANGE:
		*va_arg(ap, WORD*) = td->td_dotrange[0];
		*va_arg(ap, WORD*) = td->td_dotrange[1];
		break;
	case TIFFTAG_INKNAMES:
		*va_arg(ap, TCHAR **) = td->td_inknames;
		break;
	case TIFFTAG_TARGETPRINTER:
		*va_arg(ap, TCHAR **) = td->td_targetprinter;
		break;
#endif
	default:
		TIFFError(_T("TIFFGetField1"),
		    _T("Internal error, no value returned for tag \"%s\""),
		    TIFFFieldWithTag(tag)->field_name);
		break;
	}
	va_end(ap);
}

/*
 * Return the value of a field in the
 * internal directory structure.
 */
long
SECTiff::TIFFGetField(TIFF* tif, ttag_t tag, ...)
{
	const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);

	if (fip) {
		WORD bit = fip->field_bit;
		if (bit != FIELD_IGNORE && TIFFFieldSet(tif, bit)) {
			va_list ap;
			va_start(ap, tag);
			TIFFGetField1(&tif->tif_dir, tag, ap);
			va_end(ap);
			return (1);
		}
	} else
		TIFFError(_T("TIFFGetField"), _T("Unknown field, tag 0x%x"), tag);
	return (0);
}

/*
 * Like TIFFGetField, but taking a varargs
 * parameter list.  This routine is useful
 * for building higher-level interfaces on
 * top of the library.
 */
long
SECTiff::TIFFVGetField(TIFF* tif, ttag_t tag, va_list ap)
{
	const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);

	if (fip) {
		WORD bit = fip->field_bit;
		if (bit != FIELD_IGNORE && TIFFFieldSet(tif, bit)) {
			TIFFGetField1(&tif->tif_dir, tag, ap);
			return (1);
		}
	} else
		TIFFError(_T("TIFFGetField"), _T("Unknown field, tag 0x%x"), tag);
	return (0);
}

/*
 * Internal interface to TIFFGetField...
 */
void
SECTiff::_TIFFgetfield(TIFFDirectory* td, ttag_t tag, ...)
{
	va_list ap;
	va_start(ap, tag);
	TIFFGetField1(td, tag, ap);
	va_end(ap);
}

#define	CleanupField(member) {		\
    if (td->member) {			\
	_TIFFfree((LPBYTE)td->member);	\
	td->member = 0;			\
    }					\
}

/*
 * Release storage associated with a directory.
 */
void
SECTiff::TIFFFreeDirectory(TIFF* tif)
{
	register TIFFDirectory *td = &tif->tif_dir;

	CleanupField(td_colormap[0]);
	CleanupField(td_colormap[1]);
	CleanupField(td_colormap[2]);
	CleanupField(td_documentname);
	CleanupField(td_artist);
	CleanupField(td_datetime);
	CleanupField(td_hostcomputer);
	CleanupField(td_imagedescription);
	CleanupField(td_make);
	CleanupField(td_model);
	CleanupField(td_software);
	// if(td->td_software)
	//	free(td->td_software);
	CleanupField(td_pagename);
	CleanupField(td_sampleinfo);
#ifdef YCBCR_SUPPORT
	CleanupField(td_ycbcrcoeffs);
#endif
#ifdef JPEG_SUPPORT
	CleanupField(td_qtab);
	CleanupField(td_dctab);
	CleanupField(td_actab);
#endif
#ifdef CMYK_SUPPORT
	CleanupField(td_inknames);
	CleanupField(td_targetprinter);
#endif
#ifdef COLORIMETRY_SUPPORT
	CleanupField(td_whitepoint);
	CleanupField(td_primarychromas);
	CleanupField(td_refblackwhite);
	CleanupField(td_transferfunction[0]);
	CleanupField(td_transferfunction[1]);
	CleanupField(td_transferfunction[2]);
#endif
	CleanupField(td_stripoffset);
	CleanupField(td_stripbytecount);
}
#undef CleanupField

/*
 * Setup a default directory structure.
 */
long
SECTiff::TIFFDefaultDirectory(TIFF* tif)
{
	register TIFFDirectory *td = &tif->tif_dir;

	memset(td, 0, sizeof (*td));
	td->td_fillorder = FILLORDER_MSB2LSB;
	td->td_bitspersample = 1;
	td->td_threshholding = THRESHHOLD_BILEVEL;
	td->td_orientation = ORIENTATION_TOPLEFT;
	td->td_samplesperpixel = 1;
	td->td_predictor = 1;
	td->td_rowsperstrip = 0xffffffff;
	td->td_tilewidth = 0xffffffff;
	td->td_tilelength = 0xffffffff;
	td->td_tiledepth = 1;
	td->td_resolutionunit = RESUNIT_INCH;
	td->td_sampleformat = SAMPLEFORMAT_VOID;
	td->td_imagedepth = 1;
#ifdef YCBCR_SUPPORT
	td->td_ycbcrsubsampling[0] = 2;
	td->td_ycbcrsubsampling[1] = 2;
	td->td_ycbcrpositioning = YCBCRPOSITION_CENTERED;
#endif
#ifdef CMYK_SUPPORT
	td->td_inkset = INKSET_CMYK;
#endif
	(void) TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
	/*
	 * NB: The directory is marked dirty as a result of setting
	 * up the default compression scheme.  However, this really
	 * isn't correct -- we want TIFF_DIRTYDIRECT to be set only
	 * if the user does something.  We could just do the setup
	 * by hand, but it seems better to use the normal mechanism
	 * (i.e. TIFFSetField).
	 */
	tif->tif_flags &= ~TIFF_DIRTYDIRECT;
	return (1);
}

/*
 * Set the n-th directory as the current directory.
 * NB: Directories are numbered starting at 0.
 */
long
SECTiff::TIFFSetDirectory(TIFF* tif, tdir_t dirn)
{
	static const TCHAR module[] = _T("TIFFSetDirectory");
	WORD dircount;
	DWORD nextdir;
	tdir_t n;

	nextdir = tif->tif_header.tiff_diroff;
	for (n = dirn; n > 0 && nextdir != 0; n--) {
		if (!SeekOK(tif, nextdir) ||
		    !ReadOK(tif, &dircount, sizeof (WORD))) {
			TIFFError(module, _T("%s: Error fetching directory count"),
			    tif->tif_name);
			return (0);
		}
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabShort(&dircount);
		TIFFSeekFile(tif, dircount*sizeof (TIFFDirEntry), L_INCR);
		if (!ReadOK(tif, &nextdir, sizeof (DWORD))) {
			TIFFError(module, _T("%s: Error fetching directory link"),
			    tif->tif_name);
			return (0);
		}
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabLong(&nextdir);
	}
	tif->tif_nextdiroff = nextdir;
	/*
	 * Set curdir to the actual directory index.  The
	 * -1 is because TIFFReadDirectory will increment
	 * tif_curdir after successfully reading the directory.
	 */
	tif->tif_curdir = (tdir_t)((dirn - n) - 1);
	return (TIFFReadDirectory(tif));
}

/*
 * Return an indication of whether or not we are
 * at the last directory in the file.
 */
long
SECTiff::TIFFLastDirectory(TIFF* tif)
{
	return (tif->tif_nextdiroff == 0);
}
