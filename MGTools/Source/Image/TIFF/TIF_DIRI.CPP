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
//  Description:	tif_diri.cpp
//		known directory tag support
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

#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif


//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif

#ifndef TRUE
#define	TRUE	1
#define	FALSE	0
#endif

/*
 * NB: THIS ARRAY IS ASSUMED TO BE SORTED BY TAG.
 *     Also, if a tag can have both LONG and SHORT types
 *     then the LONG must be placed before the SHORT for
 *     writing to work properly.
 */
const TIFFFieldInfo tiffFieldInfo[100] = {
    { TIFFTAG_SUBFILETYPE,	 1, 1, TIFF_LONG,	FIELD_SUBFILETYPE,
      TRUE,	_T("SubfileType") },
/* XXX SHORT for compatibility w/ old versions of the library */
    { TIFFTAG_SUBFILETYPE,	 1, 1, TIFF_SHORT,	FIELD_SUBFILETYPE,
      TRUE,	_T("SubfileType") },
    { TIFFTAG_OSUBFILETYPE,	 1, 1, TIFF_SHORT,	FIELD_SUBFILETYPE,
      TRUE,	_T("OldSubfileType") },
    { TIFFTAG_IMAGEWIDTH,	 1, 1, TIFF_LONG,	FIELD_IMAGEDIMENSIONS,
      FALSE,	_T("ImageWidth") },
    { TIFFTAG_IMAGEWIDTH,	 1, 1, TIFF_SHORT,	FIELD_IMAGEDIMENSIONS,
      FALSE,	_T("ImageWidth") },
    { TIFFTAG_IMAGELENGTH,	 1, 1, TIFF_LONG,	FIELD_IMAGEDIMENSIONS,
      TRUE,	_T("ImageLength") },
    { TIFFTAG_IMAGELENGTH,	 1, 1, TIFF_SHORT,	FIELD_IMAGEDIMENSIONS,
      TRUE,	_T("ImageLength") },
    { TIFFTAG_BITSPERSAMPLE,	-1,-1, TIFF_SHORT,	FIELD_BITSPERSAMPLE,
      FALSE,	_T("BitsPerSample") },
    { TIFFTAG_COMPRESSION,	-1, 1, TIFF_SHORT,	FIELD_COMPRESSION,
      FALSE,	_T("Compression") },
    { TIFFTAG_PHOTOMETRIC,	 1, 1, TIFF_SHORT,	FIELD_PHOTOMETRIC,
      FALSE,	_T("PhotometricInterpretation") },
    { TIFFTAG_THRESHHOLDING,	 1, 1, TIFF_SHORT,	FIELD_THRESHHOLDING,
      TRUE,	_T("Threshholding") },
    { TIFFTAG_CELLWIDTH,	 1, 1, TIFF_SHORT,	FIELD_IGNORE,
      TRUE,	_T("CellWidth") },
    { TIFFTAG_CELLLENGTH,	 1, 1, TIFF_SHORT,	FIELD_IGNORE,
      TRUE,	_T("CellLength") },
    { TIFFTAG_FILLORDER,	 1, 1, TIFF_SHORT,	FIELD_FILLORDER,
      FALSE,	_T("FillOrder") },
    { TIFFTAG_DOCUMENTNAME,	-1,-1, TIFF_ASCII,	FIELD_DOCUMENTNAME,
      TRUE,	_T("DocumentName") },
    { TIFFTAG_IMAGEDESCRIPTION,	-1,-1, TIFF_ASCII,	FIELD_IMAGEDESCRIPTION,
      TRUE,	_T("ImageDescription") },
    { TIFFTAG_MAKE,		-1,-1, TIFF_ASCII,	FIELD_MAKE,
      TRUE,	_T("Make") },
    { TIFFTAG_MODEL,		-1,-1, TIFF_ASCII,	FIELD_MODEL,
      TRUE,	_T("Model") },
    { TIFFTAG_STRIPOFFSETS,	-1,-1, TIFF_LONG,	FIELD_STRIPOFFSETS,
      FALSE,	_T("StripOffsets") },
    { TIFFTAG_STRIPOFFSETS,	-1,-1, TIFF_SHORT,	FIELD_STRIPOFFSETS,
      FALSE,	_T("StripOffsets") },
    { TIFFTAG_ORIENTATION,	 1, 1, TIFF_SHORT,	FIELD_ORIENTATION,
      FALSE,	_T("Orientation") },
    { TIFFTAG_SAMPLESPERPIXEL,	 1, 1, TIFF_SHORT,	FIELD_SAMPLESPERPIXEL,
      FALSE,	_T("SamplesPerPixel") },
    { TIFFTAG_ROWSPERSTRIP,	 1, 1, TIFF_LONG,	FIELD_ROWSPERSTRIP,
      FALSE,	_T("RowsPerStrip") },
    { TIFFTAG_ROWSPERSTRIP,	 1, 1, TIFF_SHORT,	FIELD_ROWSPERSTRIP,
      FALSE,	_T("RowsPerStrip") },
    { TIFFTAG_STRIPBYTECOUNTS,	-1,-1, TIFF_LONG,	FIELD_STRIPBYTECOUNTS,
      FALSE,	_T("StripByteCounts") },
    { TIFFTAG_STRIPBYTECOUNTS,	-1,-1, TIFF_SHORT,	FIELD_STRIPBYTECOUNTS,
      FALSE,	_T("StripByteCounts") },
    { TIFFTAG_MINSAMPLEVALUE,	-2,-1, TIFF_SHORT,	FIELD_MINSAMPLEVALUE,
      TRUE,	_T("MinSampleValue") },
    { TIFFTAG_MAXSAMPLEVALUE,	-2,-1, TIFF_SHORT,	FIELD_MAXSAMPLEVALUE,
      TRUE,	_T("MaxSampleValue") },
    { TIFFTAG_XRESOLUTION,	 1, 1, TIFF_RATIONAL,	FIELD_RESOLUTION,
      FALSE,	_T("XResolution") },
    { TIFFTAG_YRESOLUTION,	 1, 1, TIFF_RATIONAL,	FIELD_RESOLUTION,
      FALSE,	_T("YResolution") },
    { TIFFTAG_PLANARCONFIG,	 1, 1, TIFF_SHORT,	FIELD_PLANARCONFIG,
      FALSE,	_T("PlanarConfiguration") },
    { TIFFTAG_PAGENAME,		-1,-1, TIFF_ASCII,	FIELD_PAGENAME,
      TRUE,	_T("PageName") },
    { TIFFTAG_XPOSITION,	 1, 1, TIFF_RATIONAL,	FIELD_POSITION,
      TRUE,	_T("XPosition") },
    { TIFFTAG_YPOSITION,	 1, 1, TIFF_RATIONAL,	FIELD_POSITION,
      TRUE,	_T("YPosition") },
    { TIFFTAG_FREEOFFSETS,	-1,-1, TIFF_LONG,	FIELD_IGNORE,
      FALSE,	_T("FreeOffsets") },
    { TIFFTAG_FREEBYTECOUNTS,	-1,-1, TIFF_LONG,	FIELD_IGNORE,
      FALSE,	_T("FreeByteCounts") },
    { TIFFTAG_GRAYRESPONSEUNIT,	 1, 1, TIFF_SHORT,	FIELD_IGNORE,
      TRUE,	_T("GrayResponseUnit") },
    { TIFFTAG_GRAYRESPONSECURVE,-1,-1, TIFF_SHORT,	FIELD_IGNORE,
      TRUE,	_T("GrayResponseCurve") },
    { TIFFTAG_GROUP3OPTIONS,	 1, 1, TIFF_LONG,	FIELD_GROUP3OPTIONS,
      FALSE,	_T("Group3Options") },
    { TIFFTAG_GROUP4OPTIONS,	 1, 1, TIFF_LONG,	FIELD_GROUP4OPTIONS,
      FALSE,	_T("Group4Options") },
    { TIFFTAG_RESOLUTIONUNIT,	 1, 1, TIFF_SHORT,	FIELD_RESOLUTIONUNIT,
      FALSE,	_T("ResolutionUnit") },
    { TIFFTAG_PAGENUMBER,	 2, 2, TIFF_SHORT,	FIELD_PAGENUMBER,
      TRUE,	_T("PageNumber") },
    { TIFFTAG_COLORRESPONSEUNIT, 1, 1, TIFF_SHORT,	FIELD_IGNORE,
      TRUE,	_T("ColorResponseUnit") },
#ifdef COLORIMETRY_SUPPORT
    { TIFFTAG_TRANSFERFUNCTION,	-1,-1, TIFF_SHORT,	FIELD_TRANSFERFUNCTION,
      TRUE,	_T("TransferFunction") },
#endif
    { TIFFTAG_SOFTWARE,		-1,-1, TIFF_ASCII,	FIELD_SOFTWARE,
      TRUE,	_T("Software") },
    { TIFFTAG_DATETIME,		20,20, TIFF_ASCII,	FIELD_DATETIME,
      TRUE,	_T("DateTime") },
    { TIFFTAG_ARTIST,		-1,-1, TIFF_ASCII,	FIELD_ARTIST,
      TRUE,	_T("Artist") },
    { TIFFTAG_HOSTCOMPUTER,	-1,-1, TIFF_ASCII,	FIELD_HOSTCOMPUTER,
      TRUE,	_T("HostComputer") },
    { TIFFTAG_PREDICTOR,	 1, 1, TIFF_SHORT,	FIELD_PREDICTOR,
      FALSE,	_T("Predictor") },
#ifdef COLORIMETRY_SUPPORT
    { TIFFTAG_WHITEPOINT,	 2, 2, TIFF_RATIONAL,FIELD_WHITEPOINT,
      TRUE,	_T("WhitePoint") },
    { TIFFTAG_PRIMARYCHROMATICITIES,6,6,TIFF_RATIONAL,FIELD_PRIMARYCHROMAS,
      TRUE,	_T("PrimaryChromaticities") },
#endif
    { TIFFTAG_COLORMAP,		-1,-1, TIFF_SHORT,	FIELD_COLORMAP,
      TRUE,	_T("ColorMap") },
    { TIFFTAG_HALFTONEHINTS,	 2, 2, TIFF_SHORT,	FIELD_HALFTONEHINTS,
      TRUE,	_T("HalftoneHints") },
    { TIFFTAG_TILEWIDTH,	 1, 1, TIFF_LONG,	FIELD_TILEDIMENSIONS,
      FALSE,	_T("TileWidth") },
    { TIFFTAG_TILEWIDTH,	 1, 1, TIFF_SHORT,	FIELD_TILEDIMENSIONS,
      FALSE,	_T("TileWidth") },
    { TIFFTAG_TILELENGTH,	 1, 1, TIFF_LONG,	FIELD_TILEDIMENSIONS,
      FALSE,	_T("TileLength") },
    { TIFFTAG_TILELENGTH,	 1, 1, TIFF_SHORT,	FIELD_TILEDIMENSIONS,
      FALSE,	_T("TileLength") },
    { TIFFTAG_TILEOFFSETS,	-1, 1, TIFF_LONG,	FIELD_STRIPOFFSETS,
      FALSE,	_T("TileOffsets") },
    { TIFFTAG_TILEBYTECOUNTS,	-1, 1, TIFF_LONG,	FIELD_STRIPBYTECOUNTS,
      FALSE,	_T("TileByteCounts") },
    { TIFFTAG_TILEBYTECOUNTS,	-1, 1, TIFF_SHORT,	FIELD_STRIPBYTECOUNTS,
      FALSE,	_T("TileByteCounts") },
    { TIFFTAG_BADFAXLINES,	 1, 1, TIFF_LONG,	FIELD_BADFAXLINES,
      TRUE,	_T("BadFaxLines") },
    { TIFFTAG_BADFAXLINES,	 1, 1, TIFF_SHORT,	FIELD_BADFAXLINES,
      TRUE,	_T("BadFaxLines") },
    { TIFFTAG_CLEANFAXDATA,	 1, 1, TIFF_SHORT,	FIELD_CLEANFAXDATA,
      TRUE,	_T("CleanFaxData") },
    { TIFFTAG_CONSECUTIVEBADFAXLINES,1,1, TIFF_LONG,FIELD_BADFAXRUN,
      TRUE,	_T("ConsecutiveBadFaxLines") },
    { TIFFTAG_CONSECUTIVEBADFAXLINES,1,1, TIFF_SHORT,FIELD_BADFAXRUN,
      TRUE,	_T("ConsecutiveBadFaxLines") },
#ifdef CMYK_SUPPORT		/* 6.0 CMYK tags */
    { TIFFTAG_INKSET,		 1, 1, TIFF_SHORT,	FIELD_INKSET,
      FALSE,	_T("InkSet") },
    { TIFFTAG_INKNAMES,		-1,-1, TIFF_ASCII,	FIELD_INKNAMES,
      TRUE,	_T("InkNames") },
    { TIFFTAG_DOTRANGE,		 2, 2, TIFF_SHORT,	FIELD_DOTRANGE,
      FALSE,	_T("DotRange") },
    { TIFFTAG_DOTRANGE,		 2, 2, TIFF_BYTE,	FIELD_DOTRANGE,
      FALSE,	_T("DotRange") },
    { TIFFTAG_TARGETPRINTER,	-1,-1, TIFF_ASCII,	FIELD_TARGETPRINTER,
      TRUE,	_T("TargetPrinter") },
#endif
    { TIFFTAG_EXTRASAMPLES,	-1,-1, TIFF_SHORT,	FIELD_EXTRASAMPLES,
      FALSE,	_T("ExtraSamples") },
/* XXX for bogus Adobe Photoshop v2.5 files */
    { TIFFTAG_EXTRASAMPLES,	-1,-1, TIFF_BYTE,	FIELD_EXTRASAMPLES,
      FALSE,	_T("ExtraSamples") },
    { TIFFTAG_SAMPLEFORMAT,	-1,-1, TIFF_SHORT,	FIELD_SAMPLEFORMAT,
      FALSE,	_T("SampleFormat") },
#ifdef notdef
    { TIFFTAG_SMINSAMPLEVALUE,	-2,-1, TIFF_ANY,	FIELD_SMINSAMPLEVALUE,
      TRUE,	_T("SMinSampleValue") },
    { TIFFTAG_SMAXSAMPLEVALUE,	-2,-1, TIFF_ANY,	FIELD_SMAXSAMPLEVALUE,
      TRUE,	_T("SMaxSampleValue") },
#endif
#ifdef JPEG_SUPPORT		/* 6.0 JPEG tags */
    { TIFFTAG_JPEGPROC,		 1, 1, TIFF_SHORT,	FIELD_JPEGPROC,
      FALSE,	_T("JPEGProc") },
    { TIFFTAG_JPEGIFOFFSET,	 1, 1, TIFF_LONG,	FIELD_IGNORE,
      FALSE,	_T("JPEGInterchangeFormat") },
    { TIFFTAG_JPEGIFBYTECOUNT,	 1, 1, TIFF_LONG,	FIELD_IGNORE,
      FALSE,	_T("JPEGInterchangeFormatLength") },
    { TIFFTAG_JPEGRESTARTINTERVAL,1,1, TIFF_SHORT,FIELD_JPEGRESTARTINTERVAL,
      FALSE,	_T("JPEGRestartInterval") },
    { TIFFTAG_JPEGQTABLES,	-2,-1, TIFF_LONG,	FIELD_JPEGQTABLES,
      FALSE,	_T("JPEGQTables") },
    { TIFFTAG_JPEGDCTABLES,	-2,-1, TIFF_LONG,	FIELD_JPEGDCTABLES,
      FALSE,	_T("JPEGDCTables") },
    { TIFFTAG_JPEGACTABLES,	-2,-1, TIFF_LONG,	FIELD_JPEGACTABLES,
      FALSE,	_T("JPEGACTables") },
#endif
#ifdef YCBCR_SUPPORT		/* 6.0 YCbCr tags */
    { TIFFTAG_YCBCRCOEFFICIENTS, 3, 3, TIFF_RATIONAL,	FIELD_YCBCRCOEFFICIENTS,
      FALSE,	_T("YCbCrCoefficients") },
    { TIFFTAG_YCBCRSUBSAMPLING,	 2, 2, TIFF_SHORT,	FIELD_YCBCRSUBSAMPLING,
      FALSE,	_T("YCbCrSubsampling") },
    { TIFFTAG_YCBCRPOSITIONING,	 1, 1, TIFF_SHORT,	FIELD_YCBCRPOSITIONING,
      FALSE,	_T("YCbCrPositioning") },
#endif
#ifdef COLORIMETRY_SUPPORT
    { TIFFTAG_REFERENCEBLACKWHITE,6,6,TIFF_RATIONAL,	FIELD_REFBLACKWHITE,
      FALSE,	_T("ReferenceBlackWhite") },
/* XXX temporarily accept LONG for backwards compatibility */
    { TIFFTAG_REFERENCEBLACKWHITE,6,6,TIFF_LONG,	FIELD_REFBLACKWHITE,
      FALSE,	_T("ReferenceBlackWhite") },
#endif
/* begin SGI tags */
    { TIFFTAG_MATTEING,		 1, 1, TIFF_SHORT,	FIELD_EXTRASAMPLES,
      FALSE,	_T("Matteing") },
    { TIFFTAG_DATATYPE,		-2,-1, TIFF_SHORT,	FIELD_SAMPLEFORMAT,
      FALSE,	_T("DataType") },
    { TIFFTAG_IMAGEDEPTH,	 1, 1, TIFF_LONG,	FIELD_IMAGEDEPTH,
      FALSE,	_T("ImageDepth") },
    { TIFFTAG_IMAGEDEPTH,	 1, 1, TIFF_SHORT,	FIELD_IMAGEDEPTH,
      FALSE,	_T("ImageDepth") },
    { TIFFTAG_TILEDEPTH,	 1, 1, TIFF_LONG,	FIELD_TILEDEPTH,
      FALSE,	_T("TileDepth") },
    { TIFFTAG_TILEDEPTH,	 1, 1, TIFF_SHORT,	FIELD_TILEDEPTH,
      FALSE,	_T("TileDepth") },
/* end SGI tags */
    { 0 }
};

const long tiffDataWidth[] = {
    1,	/* nothing */
    1,	/* TIFF_BYTE */
    1,	/* TIFF_ASCII */
    2,	/* TIFF_SHORT */
    4,	/* TIFF_LONG */
    8,	/* TIFF_RATIONAL */
    1,	/* TIFF_SBYTE */
    1,	/* TIFF_UNDEFINED */
    2,	/* TIFF_SSHORT */
    4,	/* TIFF_SLONG */
    8,	/* TIFF_SRATIONAL */
    4,	/* TIFF_FLOAT */
    8,	/* TIFF_DOUBLE */
};

const TIFFFieldInfo *
SECTiff::TIFFFindFieldInfo(ttag_t tag, TIFFDataType dt)
{
	static const TIFFFieldInfo *last = NULL;
	register const TIFFFieldInfo *fip;

	if (last && last->field_tag == tag &&
	    (dt == TIFF_ANY || dt == last->field_type))
		return (last);
	/* NB: if table gets big, use sorted search (e.g. binary search) */
	for (fip = tiffFieldInfo; fip->field_tag; fip++)
		if (fip->field_tag == tag &&
		    (dt == TIFF_ANY || fip->field_type == dt))
			return (last = fip);
	return ((const TIFFFieldInfo *)0);
}

#include <assert.h>

#ifndef _INC_STDIO
#include <stdio.h>
#endif

const TIFFFieldInfo *
SECTiff::TIFFFieldWithTag(ttag_t tag)
{
	const TIFFFieldInfo *fip = TIFFFindFieldInfo(tag, TIFF_ANY);
	if (!fip) {
		TIFFError(_T("TIFFFieldWithTag"),
		    _T("Internal error, unknown tag 0x%x"), (DWORD) tag);
		assert(fip != NULL);
		/*NOTREACHED*/
	}
	return (fip);
}
