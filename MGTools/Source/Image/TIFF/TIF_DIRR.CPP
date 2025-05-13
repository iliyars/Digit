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
//		directory read support routines
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

#ifndef _INC_MALLOC
#include <malloc.h>
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

#define	IGNORE	0		/* tag placeholder used below */

#if HAVE_IEEEFP
#define	TIFFCvtIEEEFloatToNative(tif, n, fp)
#endif

//AAB#ifdef AFX_COLL_SEG
//#pragma code_seg(AFX_COLL_SEG)
//#endif


LPBYTE
SECTiff::CheckMalloc(TIFF* tif, size_t n, const char* what)
{
	LPBYTE cp = (LPBYTE)_TIFFmalloc(n);

what; //UNUSED
tif; 
	//AAB if (cp == NULL)
	//	TIFFError(tif->tif_name, "No space %s", what);
	return (cp);
}

/*
 * Read the next TIFF directory from a file
 * and convert it to the internal format.
 * We read directories sequentially.
 */
long
SECTiff::TIFFReadDirectory(TIFF* tif)
{
	//AAB register TIFFDirEntry *dp;
	register TIFFDirEntry FAR* dp;
	register long n;
	register TIFFDirectory FAR* td;
	TIFFDirEntry FAR* dir; 
	//AAB TIFFDirEntry *dir;
	long iv;
	WORD v;
	const TIFFFieldInfo *fip;
	WORD dircount;
	DWORD nextdiroff;
	//AAB char *cp;
	LPBYTE cp;
	long diroutoforderwarning = 0;

	tif->tif_diroff = tif->tif_nextdiroff;
	if (tif->tif_diroff == 0)		/* no more directories */
		return (0);
	/*
	 * Cleanup any previous compression state.
	 */
	if (tif->tif_cleanup)
	{
		// (*tif->tif_cleanup)(tif);
		m_fpVoid = tif->tif_cleanup;
		(this->*m_fpVoid)(tif);
	}
	tif->tif_curdir++;
	nextdiroff = 0;
	if (!isMapped(tif)) {
		if (!SeekOK(tif, tif->tif_diroff)) {
			TIFFError(tif->tif_name,
			    _T("Seek error accessing TIFF directory"));
			return (0);
		}
		if (!ReadOK(tif, &dircount, sizeof (WORD))) {
			TIFFError(tif->tif_name,
			    _T("Can not read TIFF directory count"));
			return (0);
		}
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabShort(&dircount);
		dir = (TIFFDirEntry FAR*)CheckMalloc(tif,
		    (size_t)(dircount * sizeof (TIFFDirEntry)), 
		    "to read TIFF directory");
		if (dir == NULL)
			return (0);
		if (!ReadOK(tif, dir, (tsize_t)(dircount*sizeof (TIFFDirEntry)))) {
			TIFFError(tif->tif_name, _T("Can not read TIFF directory"));
			goto bad;
		}
		/*
		 * Read offset to next directory for sequential scans.
		 */
		(void) ReadOK(tif, &nextdiroff, sizeof (DWORD));
	} else {
		toff_t off = tif->tif_diroff;

		if ((tsize_t)(off + sizeof (short)) > tif->tif_size) {
			TIFFError(tif->tif_name,
			    _T("Can not read TIFF directory count"));
			return (0);
		} else
#ifdef WIN32
			memcpy(&dircount, tif->tif_base+off, sizeof(WORD));
#else
			_fmemcpy(&dircount, tif->tif_base + off, sizeof (WORD));
#endif
		off += sizeof (WORD);
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabShort(&dircount);
		dir = (TIFFDirEntry FAR*)CheckMalloc(tif,
		    (size_t)(dircount * sizeof (TIFFDirEntry)), "to read TIFF directory");
		if (dir == NULL)
			return (0);
		if ((tsize_t)(off + dircount*sizeof (TIFFDirEntry)) > tif->tif_size) {
			TIFFError(tif->tif_name, _T("Can not read TIFF directory"));
			goto bad;
		} else
#ifdef WIN32
			memcpy(dir, tif->tif_base + off,
				dircount*sizeof(TIFFDirEntry));
#else
			_fmemcpy(dir, tif->tif_base + off,
			    dircount*sizeof (TIFFDirEntry));
#endif
		off += dircount* sizeof (TIFFDirEntry);
		if ((tsize_t)(off + sizeof (DWORD)) < tif->tif_size)
#ifdef WIN32
			memcpy(&nextdiroff, tif->tif_base+off, sizeof(DWORD));
#else
			_fmemcpy(&nextdiroff, tif->tif_base+off, sizeof (DWORD));
#endif
	}
	if (tif->tif_flags & TIFF_SWAB)
		TIFFSwabLong(&nextdiroff);
	tif->tif_nextdiroff = nextdiroff;

	tif->tif_flags &= ~TIFF_BEENWRITING;	/* reset before new dir */
	/*
	 * Setup default value and then make a pass over
	 * the fields to check type and tag information,
	 * and to extract info required to size data
	 * structures.  A second pass is made afterwards
	 * to read in everthing not taken in the first pass.
	 */
	td = &tif->tif_dir;
	/* free any old stuff and reinit */
	TIFFFreeDirectory(tif);
	TIFFDefaultDirectory(tif);
	tif->tif_postdecode = &SECTiff::TIFFNoPostDecode;
	/*
	 * Electronic Arts writes gray-scale TIFF files
	 * without a PlanarConfiguration directory entry.
	 * Thus we setup a default value here, even though
	 * the TIFF spec says there is no default value.
	 */
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	for (fip = tiffFieldInfo, dp = dir, n = dircount; n > 0; n--, dp++) {
		if (tif->tif_flags & TIFF_SWAB) {
			TIFFSwabArrayOfShort(&dp->tdir_tag, 2);
			TIFFSwabArrayOfLong(&dp->tdir_count, 2);
		}
		/*
		 * Find the field information entry for this tag.
		 */
		/*
		 * Silicon Beach (at least) writes unordered
		 * directory tags (violating the spec).  Handle
		 * it here, but be obnoxious (maybe they'll fix it?).
		 */
		if (dp->tdir_tag < fip->field_tag) {
			if (!diroutoforderwarning) {
				TIFFWarning(tif->tif_name,
	_T("invalid TIFF directory; tags are not sorted in ascending order"));
				diroutoforderwarning = 1;
			}
			fip = tiffFieldInfo;	/* O(n^2) */
		}
		while (fip->field_tag && fip->field_tag < dp->tdir_tag)
			fip++;
		if (!fip->field_tag || fip->field_tag != dp->tdir_tag) {
			TIFFWarning(tif->tif_name,
			    _T("unknown field with tag %d (0x%x) ignored"),
			    dp->tdir_tag,  dp->tdir_tag);
			dp->tdir_tag = IGNORE;
			fip = tiffFieldInfo;	/* restart search */
			continue;
		}
		/*
		 * Null out old tags that we ignore.
		 */
		if (fip->field_bit == FIELD_IGNORE) {
	ignore:
			dp->tdir_tag = IGNORE;
			continue;
		}
		/*
		 * Check data type.
		 */
		while (dp->tdir_type != (WORD)fip->field_type) {
			if (fip->field_type == TIFF_ANY)	/* wildcard */
				break;
			fip++;
			if (!fip->field_tag || fip->field_tag != dp->tdir_tag) {
				TIFFWarning(tif->tif_name,
				   _T("wrong data type %d for \"%s\"; tag ignored"),
				    dp->tdir_type, fip[-1].field_name);
				goto ignore;
			}
		}
		/*
		 * Check count if known in advance.
		 */
		if (fip->field_readcount != TIFF_VARIABLE) {
			DWORD expected = (fip->field_readcount == TIFF_SPP) ?
			    (DWORD) td->td_samplesperpixel :
			    (DWORD) fip->field_readcount;
			if (!CheckDirCount(tif, dp, expected))
				goto ignore;
		}

		switch (dp->tdir_tag) {
		case TIFFTAG_STRIPOFFSETS:
		case TIFFTAG_STRIPBYTECOUNTS:
		case TIFFTAG_TILEOFFSETS:
		case TIFFTAG_TILEBYTECOUNTS:
			TIFFSetFieldBit(tif, fip->field_bit);
			break;
		case TIFFTAG_IMAGEWIDTH:
		case TIFFTAG_IMAGELENGTH:
		case TIFFTAG_IMAGEDEPTH:
		case TIFFTAG_TILELENGTH:
		case TIFFTAG_TILEWIDTH:
		case TIFFTAG_TILEDEPTH:
		case TIFFTAG_PLANARCONFIG:
		case TIFFTAG_SAMPLESPERPIXEL:
		case TIFFTAG_ROWSPERSTRIP:
			if (!TIFFFetchNormalTag(tif, dp))
				goto bad;
			dp->tdir_tag = IGNORE;
			break;
		case TIFFTAG_EXTRASAMPLES:
			(void) TIFFFetchExtraSamples(tif, dp);
			dp->tdir_tag = IGNORE;
			break;
		}
	}

	/*
	 * Allocate directory structure and setup defaults.
	 */
	if (!TIFFFieldSet(tif, FIELD_IMAGEDIMENSIONS)) {
		MissingRequired(tif, "ImageLength");
		goto bad;
	}
	if (!TIFFFieldSet(tif, FIELD_PLANARCONFIG)) {
		MissingRequired(tif, "PlanarConfiguration");
		goto bad;
	}
	/* 
 	 * Setup appropriate structures (by strip or by tile)
	 */
	if (!TIFFFieldSet(tif, FIELD_TILEDIMENSIONS)) {
		td->td_stripsperimage = (td->td_rowsperstrip == 0xffffffff ?
		     (td->td_imagelength != 0 ? 1 : 0) :
		     howmany(td->td_imagelength, td->td_rowsperstrip));
		td->td_tilewidth = td->td_imagewidth;
		td->td_tilelength = td->td_rowsperstrip;
		td->td_tiledepth = td->td_imagedepth;
		tif->tif_flags &= ~TIFF_ISTILED;
	} else {
		td->td_stripsperimage = TIFFNumberOfTiles(tif);
		tif->tif_flags |= TIFF_ISTILED;
	}
	td->td_nstrips = td->td_stripsperimage;
	if (td->td_planarconfig == PLANARCONFIG_SEPARATE)
		td->td_nstrips *= td->td_samplesperpixel;
	if (!TIFFFieldSet(tif, FIELD_STRIPOFFSETS)) {
		MissingRequired(tif,
		    isTiled(tif) ? "TileOffsets" : "StripOffsets");
		goto bad;
	}

	/*
	 * Second pass: extract other information.
	 */
	for (dp = dir, n = dircount; n > 0; n--, dp++) {
		if (dp->tdir_tag == IGNORE)
			continue;
		switch (dp->tdir_tag) {
		case TIFFTAG_COMPRESSION:
		case TIFFTAG_MINSAMPLEVALUE:
		case TIFFTAG_MAXSAMPLEVALUE:
		case TIFFTAG_BITSPERSAMPLE:
			/*
			 * The 5.0 spec says the Compression tag has
			 * one value, while earlier specs say it has
			 * one value per sample.  Because of this, we
			 * accept the tag if one value is supplied.
			 *
			 * The MinSampleValue, MaxSampleValue and
			 * BitsPerSample tags are supposed to be written
			 * as one value/sample, but some vendors incorrectly
			 * write one value only -- so we accept that
			 * as well (yech).
			 */
			if (dp->tdir_count == 1) {
				v = (WORD) TIFFExtractData(tif,
				    dp->tdir_type, dp->tdir_offset);
				if (!TIFFSetField(tif, dp->tdir_tag, (long)v))
					goto bad;
				break;
			}
			/* fall thru... */
		case TIFFTAG_DATATYPE:
		case TIFFTAG_SAMPLEFORMAT:
			if (!TIFFFetchPerSampleShorts(tif, dp, &iv) ||
			    !TIFFSetField(tif, dp->tdir_tag, iv))
				goto bad;
			break;
		case TIFFTAG_STRIPOFFSETS:
		case TIFFTAG_TILEOFFSETS:
			if (!TIFFFetchStripThing(tif, dp,
			    td->td_nstrips, &td->td_stripoffset))
				goto bad;
			break;
		case TIFFTAG_STRIPBYTECOUNTS:
		case TIFFTAG_TILEBYTECOUNTS:
			if (!TIFFFetchStripThing(tif, dp,
			    td->td_nstrips, &td->td_stripbytecount))
				goto bad;
			break;
		case TIFFTAG_COLORMAP:
		case TIFFTAG_TRANSFERFUNCTION:
			/*
			 * TransferFunction can have either 1x or 3x data
			 * values; Colormap can have only 3x items.
			 */
			v = (WORD)(1<<td->td_bitspersample);
			if (dp->tdir_tag == TIFFTAG_COLORMAP ||
			    dp->tdir_count != v) {
				if (!CheckDirCount(tif, dp, 3*v))
					break;
			}
			v *= sizeof (WORD);
			cp = CheckMalloc(tif, (size_t)(dp->tdir_count * sizeof (WORD)),
			    "to read \"TransferFunction\" tag");
			if (cp != NULL) {
				if (TIFFFetchData(tif, dp, cp)) {
					/*
					 * This deals with there being only
					 * one array to apply to all samples.
					 */
					if (dp->tdir_count == (DWORD)(1L<<td->td_bitspersample))
						v = 0;
					TIFFSetField(tif, dp->tdir_tag,
					    cp, cp+v, cp+2*v);
				}
				_TIFFfree(cp);
			}
			break;
		case TIFFTAG_PAGENUMBER:
		case TIFFTAG_HALFTONEHINTS:
		case TIFFTAG_YCBCRSUBSAMPLING:
		case TIFFTAG_DOTRANGE:
			(void) TIFFFetchShortPair(tif, dp);
			break;
#ifdef COLORIMETRY_SUPPORT
		case TIFFTAG_REFERENCEBLACKWHITE:
			(void) TIFFFetchRefBlackWhite(tif, dp);
			break;
#endif
#ifdef JPEG_SUPPORT
		case TIFFTAG_JPEGQTABLES:
			if (TIFFFetchJPEGQTables(tif, dp))
				TIFFSetFieldBit(tif, FIELD_JPEGQTABLES);
			break;
		case TIFFTAG_JPEGDCTABLES:
			if (TIFFFetchJPEGCTables(tif, dp, &td->td_dctab))
				TIFFSetFieldBit(tif, FIELD_JPEGDCTABLES);
			break;
		case TIFFTAG_JPEGACTABLES:
			if (TIFFFetchJPEGCTables(tif, dp, &td->td_actab))
				TIFFSetFieldBit(tif, FIELD_JPEGACTABLES);
			break;
#endif
/* BEGIN REV 4.0 COMPATIBILITY */
		case TIFFTAG_OSUBFILETYPE:
			v = 0;
			switch (TIFFExtractData(tif, dp->tdir_type,
			    dp->tdir_offset)) {
			case OFILETYPE_REDUCEDIMAGE:
				v = FILETYPE_REDUCEDIMAGE;
				break;
			case OFILETYPE_PAGE:
				v = FILETYPE_PAGE;
				break;
			}
			if (v)
				(void) TIFFSetField(tif,
				    TIFFTAG_SUBFILETYPE, (long)v);
			break;
/* END REV 4.0 COMPATIBILITY */
		default:
			(void) TIFFFetchNormalTag(tif, dp);
			break;
		}
	}
	/*
	 * Verify Palette image has a Colormap.
	 */
	if (td->td_photometric == PHOTOMETRIC_PALETTE &&
	    !TIFFFieldSet(tif, FIELD_COLORMAP)) {
		MissingRequired(tif, "Colormap");
		goto bad;
	}
	/*
	 * Attempt to deal with a missing StripByteCounts tag.
	 */
	if (!TIFFFieldSet(tif, FIELD_STRIPBYTECOUNTS)) {
		/*
		 * Some manufacturers violate the spec by not giving
		 * the size of the strips.  In this case, assume there
		 * is one uncompressed strip of data.
		 */
		if (td->td_nstrips > 1) {
		    MissingRequired(tif, "StripByteCounts");
		    goto bad;
		}
		TIFFWarning(tif->tif_name,
_T("TIFF directory is missing required \"%s\" field, calculating from imagelength"),
		    TIFFFieldWithTag(TIFFTAG_STRIPBYTECOUNTS)->field_name);
		EstimateStripByteCounts(tif, dir, dircount);
#define	BYTECOUNTLOOKSBAD \
    (td->td_stripbytecount[0] == 0 || \
    (td->td_compression == COMPRESSION_NONE && \
     td->td_stripbytecount[0] > TIFFGetFileSize(tif) - td->td_stripoffset[0]))
	} else if (td->td_nstrips == 1 && BYTECOUNTLOOKSBAD) {
		/*
		 * Plexus (and others) sometimes give a value
		 * of zero for a tag when they don't know what
		 * the correct value is!  Try and handle the
		 * simple case of estimating the size of a one
		 * strip image.
		 */
		TIFFWarning(tif->tif_name,
	    _T("Bogus \"%s\" field, ignoring and calculating from imagelength"),
		    TIFFFieldWithTag(TIFFTAG_STRIPBYTECOUNTS)->field_name);
		EstimateStripByteCounts(tif, dir, dircount);
	}
	if (dir)
		_TIFFfree((LPBYTE)dir);
	if (!TIFFFieldSet(tif, FIELD_MAXSAMPLEVALUE))
		td->td_maxsamplevalue = (1L<<td->td_bitspersample)-1;
	/*
	 * Setup default compression scheme.
	 */
	if (!TIFFFieldSet(tif, FIELD_COMPRESSION))
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
#ifdef STRIPCHOP_SUPPORT
        /*
         * Some manufacturers make life difficult by writing
	 * large amounts of uncompressed data as a single strip.
	 * This is contrary to the recommendations of the spec.
         * The following makes an attempt at breaking such images
	 * into strips closer to the recommended 8k bytes.  A
	 * side effect, however, is that the RowsPerStrip tag
	 * value may be changed.
         */
        if (td->td_nstrips == 1 && td->td_compression == COMPRESSION_NONE &&
	    td->td_tilewidth == td->td_imagewidth)
		ChopUpSingleUncompressedStrip(tif);
#endif
	/*
	 * Reinitialize i/o since we are starting on a new directory.
	 */
	tif->tif_row = -1;
	tif->tif_curstrip = -1;
	tif->tif_col = -1;
	tif->tif_curtile = (DWORD)-1;  	//AAB: Yes, original code treats this member as an
									// unsigned as well as a signed value!!!
	tif->tif_tilesize = TIFFTileSize(tif);
	tif->tif_scanlinesize = TIFFScanlineSize(tif);
	return (1);
bad:
	if (dir)
		_TIFFfree(dir);
	return (0);
}

void
SECTiff::EstimateStripByteCounts(TIFF* tif, TIFFDirEntry FAR* dir, WORD dircount)
{
	register TIFFDirEntry FAR* dp;
	register TIFFDirectory *td = &tif->tif_dir;

	td->td_stripbytecount = (LPDWORD)
	    CheckMalloc(tif, (size_t)(sizeof (DWORD)), "for \"StripByteCounts\" array");
	if (td->td_compression != COMPRESSION_NONE) {
		DWORD space = sizeof (TIFFHeader)
		    + sizeof (WORD)
		    + (dircount * sizeof (TIFFDirEntry))
		    + sizeof (DWORD);
		toff_t filesize = TIFFGetFileSize(tif);
		WORD n;

		/* calculate amount of space used by indirect values */
		for (dp = dir, n = dircount; n > 0; n--, dp++) {
			long cc = dp->tdir_count * tiffDataWidth[dp->tdir_type];
			if (cc > sizeof (DWORD))
				space += cc;
		}
		td->td_stripbytecount[0] = filesize - space;
		/*
		 * This handles the case were the offset to
		 * the strip is past the place where we think the strip
		 * should begin.  Since a strip of data must be contiguous,
		 * it's safe to assume that we've overestimated the amount
		 * of data in the strip and trim this number back accordingly.
		 */ 
		if ((long)(td->td_stripoffset[0] + td->td_stripbytecount[0]) > filesize)
			td->td_stripbytecount[0] =
			    filesize - td->td_stripoffset[0];
	} else {
		DWORD rowbytes = howmany(td->td_bitspersample *
		    td->td_samplesperpixel * td->td_imagewidth, 8);
		td->td_stripbytecount[0] = td->td_imagelength * rowbytes;
	}
	TIFFSetFieldBit(tif, FIELD_STRIPBYTECOUNTS);
	if (!TIFFFieldSet(tif, FIELD_ROWSPERSTRIP))
		td->td_rowsperstrip = td->td_imagelength;
}

void
SECTiff::MissingRequired(TIFF* tif, const char* tagname)
{
	TIFFError(tif->tif_name,
	    _T("TIFF directory is missing required \"%s\" field"), tagname);
}

/*
 * Check the count field of a directory
 * entry against a known value.  The caller
 * is expected to skip/ignore the tag if
 * there is a mismatch.
 */
long
SECTiff::CheckDirCount(TIFF* tif, TIFFDirEntry FAR* dir, DWORD count)
{
	if (count != dir->tdir_count) {
		TIFFWarning(tif->tif_name,
	_T("incorrect count for field \"%s\" (%lu, expecting %lu); tag ignored"),
		    TIFFFieldWithTag(dir->tdir_tag)->field_name,
		    dir->tdir_count, count);
		return (0);
	}
	return (1);
}

/*
 * Fetch a contiguous directory item.
 */
tsize_t
SECTiff::TIFFFetchData(TIFF* tif, TIFFDirEntry FAR* dir, LPBYTE cp)
{
	long w = tiffDataWidth[dir->tdir_type];
	tsize_t cc = dir->tdir_count * w;

	if (!isMapped(tif)) {
		if (!SeekOK(tif, dir->tdir_offset))
			goto bad;
		if (!ReadOK(tif, cp, (tsize_t)cc))
			goto bad;
	} else {
		if ((long)(dir->tdir_offset + cc) > tif->tif_size)
			goto bad;
#ifdef WIN32
		memcpy(cp, tif->tif_base + dir->tdir_offset, (size_t)cc);
#else
		_fmemcpy(cp, tif->tif_base + dir->tdir_offset, (size_t)cc);
#endif
	}
	if (tif->tif_flags & TIFF_SWAB) {
		switch (dir->tdir_type) {
		case TIFF_SHORT:
		case TIFF_SSHORT:
			TIFFSwabArrayOfShort((LPWORD)cp, dir->tdir_count);
			break;
		case TIFF_LONG:
		case TIFF_SLONG:
		case TIFF_FLOAT:
			TIFFSwabArrayOfLong((LPDWORD)cp, dir->tdir_count);
			break;
		case TIFF_RATIONAL:
		case TIFF_SRATIONAL:
			TIFFSwabArrayOfLong((LPDWORD)cp, 2*dir->tdir_count);
			break;
		}
	}
	return (cc);
bad:
	TIFFError(tif->tif_name, _T("Error fetching data for field \"%s\""),
	    TIFFFieldWithTag(dir->tdir_tag)->field_name);
	return ((tsize_t) 0);
}

/*
 * Fetch an ASCII item from the file.
 */
tsize_t
//AAB SECTiff::TIFFFetchString(TIFF* tif, TIFFDirEntry FAR* dir, char* cp)
SECTiff::TIFFFetchString(TIFF* tif, TIFFDirEntry FAR* dir, LPBYTE cp)
{
	if (dir->tdir_count <= 4) {
		DWORD l = dir->tdir_offset;
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabLong(&l);
#ifdef WIN32
		memcpy(cp, &l, (size_t)dir->tdir_count);
#else
		_fmemcpy(cp, &l, (size_t)dir->tdir_count);
#endif
		return (1);
	}
	//AAB typecase should be ok short->far (FPSEG==0)
	return (TIFFFetchData(tif, dir, (LPBYTE)cp));
}

/*
 * Convert numerator+denominator to float.
 */
long
SECTiff::cvtRational(TIFF* tif, TIFFDirEntry FAR* dir, DWORD num, DWORD denom, LPFLOAT rv)
{
	if (denom == 0) {
		TIFFError(tif->tif_name,
		    _T("%s: Rational with zero denominator (num = %lu)"),
		    TIFFFieldWithTag(dir->tdir_tag)->field_name, num);
		return (0);
	} else {
		if (dir->tdir_type == TIFF_RATIONAL)
			*rv = ((float)num / (float)denom);
		else
			*rv = ((float)(long)num / (float)(long)denom);
		return (1);
	}
}

/*
 * Fetch a rational item from the file
 * at offset off and return the value
 * as a floating point number.
 */
float
SECTiff::TIFFFetchRational(TIFF* tif, TIFFDirEntry FAR* dir)
{
	DWORD l[2];
	float v;

	//AAB return ((float)!TIFFFetchData(tif, dir, (char *)l) ||
	return ((float)!TIFFFetchData(tif, dir, (LPBYTE)l) ||
	    !cvtRational(tif, dir, l[0], l[1], &v) ? (float)1.0 : v);
}

/*
 * Fetch a single floating point value
 * from the offset field and return it
 * as a native float.
 */
float
SECTiff::TIFFFetchFloat(TIFF* tif, TIFFDirEntry FAR* dir)
{
	float v = (float)
	    TIFFExtractData(tif, dir->tdir_type, dir->tdir_offset);
	TIFFCvtIEEEFloatToNative(tif, 1, &v);
	return (v);
}

/*
 * Fetch an array of BYTE or SBYTE values.
 */
long
SECTiff::TIFFFetchByteArray(TIFF* tif, TIFFDirEntry FAR* dir, LPWORD v)
{
	if (dir->tdir_count <= 4) {
		/*
		 * Extract data from offset field.
		 */
		if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
			switch (dir->tdir_count) {
			case 4: v[3] = (WORD)(dir->tdir_offset & 0xff);
			case 3: v[2] = (WORD)((dir->tdir_offset >> 8) & 0xff);
			case 2: v[1] = (WORD)((dir->tdir_offset >> 16) & 0xff);
			case 1: v[0] = (WORD)(dir->tdir_offset >> 24);
			}
		} else {
			switch (dir->tdir_count) {
			case 4: v[3] = (WORD)(dir->tdir_offset >> 24);
			case 3: v[2] = (WORD)((dir->tdir_offset >> 16) & 0xff);
			case 2: v[1] = (WORD)((dir->tdir_offset >> 8) & 0xff);
			case 1: v[0] = (WORD)(dir->tdir_offset & 0xff);
			}
		}
		return (1);
	} else
		return (TIFFFetchData(tif, dir, (LPBYTE)v));	/* XXX */
}

/*
 * Fetch an array of SHORT or SSHORT values.
 */
long
SECTiff::TIFFFetchShortArray(TIFF* tif, TIFFDirEntry FAR* dir, LPWORD v)
{
	if (dir->tdir_count <= 2) {
		if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
			switch (dir->tdir_count) {
			case 2: v[1] = (WORD)(dir->tdir_offset & 0xffff);
			case 1: v[0] = (WORD)(dir->tdir_offset >> 16);
			}
		} else {
			switch (dir->tdir_count) {
			case 2: v[1] = (WORD)(dir->tdir_offset >> 16);
			case 1: v[0] = (WORD)(dir->tdir_offset & 0xffff);
			}
		}
		return (1);
	} else
		return (TIFFFetchData(tif, dir, (LPBYTE)v));
}

/*
 * Fetch a pair of SHORT or BYTE values.
 */
long
SECTiff::TIFFFetchShortPair(TIFF* tif, TIFFDirEntry FAR* dir)
{
	WORD v[2];
	long ok = 0;

	switch (dir->tdir_type) {
	case TIFF_SHORT:
	case TIFF_SSHORT:
		ok = TIFFFetchShortArray(tif, dir, v);
		break;
	case TIFF_BYTE:
	case TIFF_SBYTE:
		ok  = TIFFFetchByteArray(tif, dir, v);
		break;
	}
	if (ok)
		TIFFSetField(tif, dir->tdir_tag, v[0], v[1]);
	return (ok);
}

/*
 * Fetch an array of LONG or SLONG values.
 */
long
SECTiff::TIFFFetchLongArray(TIFF* tif, TIFFDirEntry FAR* dir, LPDWORD v)
{
	if (dir->tdir_count == 1) {
		v[0] = dir->tdir_offset;
		return (1);
	} else
		return (TIFFFetchData(tif, dir, (LPBYTE)v));
}

/*
 * Fetch an array of RATIONAL or SRATIONAL values.
 */
long
SECTiff::TIFFFetchRationalArray(TIFF* tif, TIFFDirEntry FAR* dir, LPFLOAT v)
{
	long ok = 0;
	LPDWORD l;

	l = (LPDWORD)CheckMalloc(tif,
	    (size_t)(dir->tdir_count*tiffDataWidth[dir->tdir_type]),
	    "to fetch array of rationals");
	if (l) {
		if (TIFFFetchData(tif, dir, (LPBYTE)l)) {
			DWORD i;
			for (i = 0; i < dir->tdir_count; i++) {
				ok = cvtRational(tif, dir,
				    l[2*i+0], l[2*i+1], &v[i]);
				if (!ok)
					break;
			}
		}
		_TIFFfree(l);
	}
	return (ok);
}

/*
 * Fetch an array of FLOAT values.
 */
long
SECTiff::TIFFFetchFloatArray(TIFF* tif, TIFFDirEntry FAR* dir, LPFLOAT v)
{
	if (TIFFFetchData(tif, dir, (LPBYTE)v)) {
		TIFFCvtIEEEFloatToNative(tif, dir->tdir_count, v);
		return (1);
	} else
		return (0);
}

/*
 * Fetch a tag that is not handled by special case code.
 *
 * NB: DOUBLE and UNDEFINED types are not handled.
 */
long
SECTiff::TIFFFetchNormalTag(TIFF* tif, TIFFDirEntry FAR* dp)
{
	static char mesg[] = "to fetch tag value";
	long ok = 0;

	if (dp->tdir_count > 1) {		/* array of values */
		//AAB char* cp = NULL;
		LPBYTE cp = NULL;

		switch (dp->tdir_type) {
		case TIFF_BYTE:
		case TIFF_SBYTE:
			/* NB: always expand BYTE values to shorts */
			cp = CheckMalloc(tif,
			    (size_t)(dp->tdir_count * sizeof (WORD)), mesg);
			ok = cp && TIFFFetchByteArray(tif, dp, (LPWORD)cp);
			break;
		case TIFF_SHORT:
		case TIFF_SSHORT:
			cp = CheckMalloc(tif,
			    (size_t)(dp->tdir_count * sizeof (WORD)), mesg);
			ok = cp && TIFFFetchShortArray(tif, dp, (LPWORD)cp);
			break;
		case TIFF_LONG:
		case TIFF_SLONG:
			cp = CheckMalloc(tif,
			    (size_t)(dp->tdir_count * sizeof (DWORD)), mesg);
			ok = cp && TIFFFetchLongArray(tif, dp, (LPDWORD)cp);
			break;
		case TIFF_RATIONAL:
		case TIFF_SRATIONAL:
			cp = CheckMalloc(tif,
			    (size_t)(dp->tdir_count * sizeof (float)), mesg);
			ok = cp && TIFFFetchRationalArray(tif, dp, (LPFLOAT)cp);
			break;
		case TIFF_FLOAT:
			cp = CheckMalloc(tif,
			    (size_t)(dp->tdir_count * sizeof (float)), mesg);
			ok = cp && TIFFFetchFloatArray(tif, dp, (LPFLOAT)cp);
			break;
		case TIFF_ASCII:
			/*
			 * Some vendors write strings w/o the trailing
			 * NULL byte, so always append one just in case.
			 */
			cp = CheckMalloc(tif, (size_t)(dp->tdir_count+1), mesg);
			ok = (cp && TIFFFetchString(tif, dp, cp));
			if (ok)
				cp[dp->tdir_count] = '\0';	/* XXX */
			break;
		}
		if (ok)
			ok = TIFFSetField(tif, dp->tdir_tag, cp);
		if (cp != NULL)
			_TIFFfree(cp);
	} else if (CheckDirCount(tif, dp, 1)) {	/* singleton value */
		char c[2];
		TIFFDataType type;
		switch (dp->tdir_type) {
		case TIFF_BYTE:
		case TIFF_SBYTE:
		case TIFF_SHORT:
		case TIFF_SSHORT:
			/*
			 * If the tag is also acceptable as a LONG or SLONG
			 * then TIFFSetField will expect an DWORD parameter
			 * passed to it (through varargs).  Thus, for machines
			 * where sizeof (int) != sizeof (DWORD) we must do
			 * a careful check here.  It's hard to say if this
			 * is worth optimizing.
			 *
			 * NB: We use TIFFFieldWithTag here knowing that
			 *     it returns us the first entry in the table
			 *     for the tag and that that entry is for the
			 *     widest potential data type the tag may have.
			 */
			type = TIFFFieldWithTag(dp->tdir_tag)->field_type;
			if (type != TIFF_LONG && type != TIFF_SLONG) {
				ok = TIFFSetField(tif, dp->tdir_tag, (long)
			  TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset));
				break;
			}
			/* fall thru... */
		case TIFF_LONG:
		case TIFF_SLONG:
			ok = TIFFSetField(tif, dp->tdir_tag, (DWORD)
		  TIFFExtractData(tif, dp->tdir_type, dp->tdir_offset));
			break;
		case TIFF_RATIONAL:
		case TIFF_SRATIONAL:
			ok = TIFFSetField(tif, dp->tdir_tag,
			    TIFFFetchRational(tif, dp));
			break;
		case TIFF_FLOAT:
			ok = TIFFSetField(tif, dp->tdir_tag,
			    TIFFFetchFloat(tif, dp));
			break;
		case TIFF_ASCII:
			ok = (TIFFFetchString(tif, dp, (LPBYTE)(&c)));
			if (ok) {
				c[1] = '\0';		/* XXX paranoid */
				ok = TIFFSetField(tif, dp->tdir_tag, c);
			}
			break;
		}
	}
	return (ok);
}

#define	NITEMS(x)	(sizeof (x) / sizeof (x[0]))
/*
 * Fetch samples/pixel short values for 
 * the specified tag and verify that
 * all values are the same.
 */
long
SECTiff::TIFFFetchPerSampleShorts(TIFF* tif, TIFFDirEntry FAR* dir, long far* pl)
{
	long samples = tif->tif_dir.td_samplesperpixel;
	long status = 0;

	if (CheckDirCount(tif, dir, (DWORD) samples)) {
		WORD buf[10];
		LPWORD v = buf;

		if (samples > NITEMS(buf))
			v = (LPWORD)_TIFFmalloc((size_t)(samples * sizeof (WORD)));
		if (TIFFFetchShortArray(tif, dir, v)) {
			long i;
			for (i = 1; i < samples; i++)
				if (v[i] != v[0]) {
					TIFFError(tif->tif_name,
		_T("Cannot handle different per-sample values for field \"%s\""),
				   TIFFFieldWithTag(dir->tdir_tag)->field_name);
					goto bad;
				}
			*pl = v[0];
			status = 1;
		}
	bad:
		if (v != buf)
			_TIFFfree(v);
	}
	return (status);
}
#undef NITEMS

/*
 * Fetch a set of offsets or lengths.
 * While this routine says "strips",
 * in fact it's also used for tiles.
 */
long
SECTiff::TIFFFetchStripThing(TIFF* tif, TIFFDirEntry FAR* dir, long nstrips, LPPDWORD lpp)
{
	register LPDWORD lp;
	long status;

	if (!CheckDirCount(tif, dir, nstrips))
		return (0);
	/*
	 * Allocate space for strip information.
	 */
	if (*lpp == NULL &&
	    (*lpp = (LPDWORD)CheckMalloc(tif,
	      (size_t)(nstrips * sizeof (DWORD)), "for strip array")) == NULL)
		return (0);
	lp = *lpp;
	if (dir->tdir_type == (long)TIFF_SHORT) {
		/*
		 * Handle WORD->DWORD expansion.
		 */
		LPWORD dp = (LPWORD)CheckMalloc(tif,
		    (size_t)(dir->tdir_count* sizeof (WORD)), "to fetch strip tag");
		if (dp == NULL)
			return (0);
		status = TIFFFetchShortArray(tif, dir, dp);
		if (status) {
			register LPWORD wp = dp;
			while (nstrips-- > 0)
				*lp++ = *wp++;
		}
		_TIFFfree(dp);
	} else
		status = TIFFFetchLongArray(tif, dir, lp);
	return (status);
}

#define	NITEMS(x)	(sizeof (x) / sizeof (x[0]))
/*
 * Fetch and set the ExtraSamples tag.
 */
long
SECTiff::TIFFFetchExtraSamples(TIFF* tif, TIFFDirEntry FAR* dir)
{
	WORD buf[10], *v = buf;
	long status;

	if (dir->tdir_count > NITEMS(buf))
		v = (LPWORD)_TIFFmalloc((size_t)(dir->tdir_count * sizeof (WORD)));
	if (dir->tdir_type == TIFF_BYTE)
		status = TIFFFetchByteArray(tif, dir, v);
	else
		status = TIFFFetchShortArray(tif, dir, v);
	if (status)
		status = TIFFSetField(tif, dir->tdir_tag, dir->tdir_count, v);
	if (v != buf)
		_TIFFfree((char *)v);
	return (status);
}
#undef NITEMS

#ifdef COLORIMETRY_SUPPORT
/*
 * Fetch and set the RefBlackWhite tag.
 */
long
SECTiff::TIFFFetchRefBlackWhite(TIFF* tif, TIFFDirEntry FAR* dir)
{
	static char mesg[] = "for \"ReferenceBlackWhite\" array";
	LPBYTE cp;
	long ok;

	if (dir->tdir_type == TIFF_RATIONAL)
		return (TIFFFetchNormalTag(tif, dir));
	/*
	 * Handle LONG's for backward compatibility.
	 */
	cp = CheckMalloc(tif, (size_t)(dir->tdir_count * sizeof (DWORD)), mesg);
	ok = (cp && TIFFFetchLongArray(tif, dir, (LPDWORD)cp));
	if (ok) {
		LPFLOAT fp = (LPFLOAT)
		    CheckMalloc(tif, (size_t)(dir->tdir_count * sizeof (float)), mesg);
		ok = (fp != NULL);
		if (ok ) {
			DWORD i;
			for (i = 0; i < dir->tdir_count; i++)
				fp[i] = (float)((LPDWORD)cp)[i];
			ok = TIFFSetField(tif, dir->tdir_tag, fp);
			_TIFFfree((LPBYTE)fp);
		}
	}
	if (cp)
		_TIFFfree(cp);
	return (ok);
}
#endif

#ifdef JPEG_SUPPORT
/*
 * Fetch the JPEG Quantization tables
 * for the specified directory entry.
 * Storage for the td_qtab array is
 * allocated as a side effect.
 */
long
SECTiff::TIFFFetchJPEGQTables(TIFF* tif, TIFFDirEntry FAR* dir)
{
	TIFFDirectory *td = &tif->tif_dir;
	DWORD off[4];
	DWORD i;
	TIFFDirEntry tdir;
	LPBYTE qmat;

	if (dir->tdir_count > 1) {
		/* XXX verify count <= 4 */
		//AAB if (!TIFFFetchData(tif, dir, (char *)off))
		if (!TIFFFetchData(tif, dir, (LPBYTE)(&off)))
			return (0);
	} else
		off[0] = dir->tdir_offset;
	/*
	 * We don't share per-component q matrices because
	 * (besides complicating this logic even more), it
	 * would make it very painful if the user does a ``set''.
	 */
	td->td_qtab = (LPPBYTE)CheckMalloc(tif,
	    (size_t)(dir->tdir_count*(sizeof (LPBYTE) + 64*sizeof (BYTE))),
	    "for JPEG Q table");
	if (td->td_qtab == NULL)
		return (0);
	tdir.tdir_type = TIFF_BYTE;
	tdir.tdir_count = 64;
	qmat = (((LPBYTE)td->td_qtab) + dir->tdir_count*sizeof (LPBYTE));
	for (i = 0; i < dir->tdir_count; i++) {
		td->td_qtab[i] = (LPBYTE)qmat;
		tdir.tdir_offset = off[i];
		if (!TIFFFetchData(tif, &tdir, qmat))
			return (0);
		qmat += 64*sizeof (BYTE);
	}
	return (1);
}

/*
 * Fetch JPEG Huffman code tables for the
 * specified directory entry.  Storage for
 * the tables are allocated as a side effect.
 */
long
SECTiff::TIFFFetchJPEGCTables(TIFF* tif, TIFFDirEntry FAR* dir, LPPPBYTE ptab)
{
	DWORD off[4];
	DWORD i, j, ncodes;
	TIFFDirEntry tdir;
	LPBYTE tab;

	if (dir->tdir_count > 1) {
		/* XXX verify count <= 4 */
		//AAB if (!TIFFFetchData(tif, dir, (char *)off))
		if (!TIFFFetchData(tif, dir, (LPBYTE)(&off)))
			return (0);
	} else
		off[0] = dir->tdir_offset;
	/*
	 * We don't share per-component tables because
	 * (besides complicating this logic even more), it
	 * would make it very painful if the user does a
	 * ``set''.  Note also that we don't try to optimize
	 * storage of the tables -- we just allocate enough
	 * space to hold the largest possible.  All this
	 * stuff is so complicated 'cuz the tag is defined
	 * to be compatible with the JPEG table format,
	 * rather than something that fits well longo the
	 * structure of TIFF -- argh!
	 */
	*ptab = (LPPBYTE)CheckMalloc(tif, (size_t)(dir->tdir_count*
	    (sizeof (LPBYTE) + (16+256)*sizeof (BYTE))),
	    "for JPEG Huffman table");
	if (*ptab == NULL)
		return (0);
	tdir.tdir_type = TIFF_BYTE;
	tab = (((LPBYTE)*ptab) + dir->tdir_count*sizeof (LPBYTE));
	for (i = 0; i < dir->tdir_count; i++) {
		(*ptab)[i] = (LPBYTE)tab;
		tdir.tdir_offset = off[i];
		tdir.tdir_count = 16;
		/*
		 * We must fetch the array that holds the
		 * count of codes for each bit length first
		 * and the count up the number of codes that
		 * are in the variable length table.  This
		 * information is implicit in the JPEG format
		 * 'cuz it's preceded by a length field.
		 */
		if (!TIFFFetchData(tif, &tdir, tab))	/* count array */
			return (0);
		for (ncodes = 0, j = 0; j < 16; j++)
			ncodes += tab[j];
		/*
		 * Adjust offsets and fetch codes separately.
		 */
		tdir.tdir_offset += 16;
		tdir.tdir_count = ncodes;
		tab += 16;
		if (!TIFFFetchData(tif, &tdir, tab))
			return (0);
		tab += ncodes;
	}
	return (1);
}
#endif

#ifdef STRIPCHOP_SUPPORT
/*
 * Replace a single strip (tile) of uncompressed data by
 * multiple strips (tiles), each approximately 8Kbytes.
 * This is useful for dealing with large images or
 * for dealing with machines with a limited amount
 * memory.
 */
void
SECTiff::ChopUpSingleUncompressedStrip(TIFF* tif)
{
	register TIFFDirectory *td = &tif->tif_dir;
	DWORD bytecount = td->td_stripbytecount[0];
	DWORD offset = td->td_stripoffset[0];
	tsize_t rowbytes = TIFFVTileSize(tif, 1), stripbytes;
	tstrip_t strip, nstrips, rowsperstrip;
	LPDWORD newcounts;
	LPDWORD newoffsets;

	/*
	 * Make the rows hold at least one
	 * scanline, but fill 8k if possible.
	 */
	if (rowbytes > 8192) {
		stripbytes = rowbytes;
		rowsperstrip = 1;
	} else {
		rowsperstrip = 8192 / rowbytes;
		stripbytes = rowbytes * rowsperstrip;
	}
	nstrips = howmany(bytecount, stripbytes);
	newcounts = (LPDWORD) CheckMalloc(tif, (size_t)(nstrips * sizeof (DWORD)),
				"for chopped \"StripByteCounts\" array");
	newoffsets = (LPDWORD) CheckMalloc(tif, (size_t)(nstrips * sizeof (DWORD)),
				"for chopped \"StripOffsets\" array");
	if (newcounts == NULL || newoffsets == NULL) {
	        /*
		 * Unable to allocate new strip information, give
		 * up and use the original one strip information.
		 */
		if (newcounts != NULL)
			_TIFFfree(newcounts);
		if (newoffsets != NULL)
			_TIFFfree(newoffsets);
		return;
	}
	/*
	 * Fill the strip information arrays with
	 * new bytecounts and offsets that reflect
	 * the broken-up format.
	 */
	for (strip = 0; strip < nstrips; strip++) {
		if ((DWORD)stripbytes > bytecount)
			stripbytes = bytecount;
		newcounts[strip] = stripbytes;
		newoffsets[strip] = offset;
		offset += stripbytes;
		bytecount -= stripbytes;
	}
	/*
	 * Replace old single strip info with multi-strip info.
	 */
	td->td_stripsperimage = td->td_nstrips = nstrips;
	td->td_rowsperstrip = rowsperstrip;
	td->td_tilelength = rowsperstrip;

	_TIFFfree(td->td_stripbytecount);
	_TIFFfree(td->td_stripoffset);
	td->td_stripbytecount = newcounts;
	td->td_stripoffset = newoffsets;
}
#endif /* STRIPCHOP_SUPPORT */
