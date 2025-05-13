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
//		directory write support routines
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

#if HAVE_IEEEFP
#define	TIFFCvtNativeToIEEEFloat(tif, n, fp)
#endif


#define	WriteRationalPair(type, tag1, v1, tag2, v2) {		\
	if (!TIFFWriteRational(tif, type, tag1, dir, v1))	\
		goto bad;					\
	if (!TIFFWriteRational(tif, type, tag2, dir+1, v2))	\
		goto bad;					\
	dir++;							\
}

/*
 * Write the contents of the current directory
 * to the specified file.  This routine doesn't
 * handle overwriting a directory with auxiliary
 * storage that's been changed.
 */
long
SECTiff::TIFFWriteDirectory(TIFF* tif)
{
	//AAB short dircount, tag;
	WORD dircount, tag;
	long nfields, dirsize;
	LPBYTE data;
	const TIFFFieldInfo *fip;
	TIFFDirEntry FAR* dir;
	TIFFDirectory *td;
	DWORD b, fields[FIELD_SETLONGS];

	if (tif->tif_mode == O_RDONLY)
		return (1);
	/*
	 * Clear write state so that subsequent images with
	 * different characteristics get the right buffers
	 * setup for them.
	 */
	if (tif->tif_flags & TIFF_POSTENCODE) {
		tif->tif_flags &= ~TIFF_POSTENCODE;
		// if (tif->tif_postencode && !(*tif->tif_postencode)(tif)) {
		m_fpBool = tif->tif_postencode;
		if (m_fpBool && !(this->*m_fpBool)(tif)) {
			TIFFError(tif->tif_name,
			    _T("Error post-encoding before directory write"));
			return (0);
		}
	}
	if (tif->tif_close)
	{
		// (*tif->tif_close)(tif);
		m_fpVoid = tif->tif_close;
		(this->*m_fpVoid)(tif);
	}
	if (tif->tif_cleanup)
	{
		// (*tif->tif_cleanup)(tif);
		m_fpVoid = tif->tif_cleanup;
		(this->*m_fpVoid)(tif);
	}

	/*
	 * Flush any data that might have been written
	 * by the compression close+cleanup routines.
	 */
	if (tif->tif_rawcc > 0 && !TIFFFlushData1(tif)) {
		TIFFError(tif->tif_name,
		    _T("Error flushing data before directory write"));
		return (0);
	}
	if ((tif->tif_flags & TIFF_MYBUFFER) && tif->tif_rawdata) {
		_TIFFfree(tif->tif_rawdata);
		tif->tif_rawdata = NULL;
		tif->tif_rawcc = 0;
	}
	tif->tif_flags &= ~(TIFF_BEENWRITING|TIFF_BUFFERSETUP);

	td = &tif->tif_dir;
	/*
	 * Size the directory so that we can calculate
	 * offsets for the data items that aren't kept
	 * in-place in each field.
	 */
	nfields = 0;
	for (b = 0; b <= FIELD_LAST; b++)
		if (TIFFFieldSet(tif, b))
			nfields += (b < FIELD_SUBFILETYPE ? 2 : 1);
	dirsize = nfields * sizeof (TIFFDirEntry);
	data = (LPBYTE)_TIFFmalloc((size_t)dirsize);
	if (data == NULL) {
		TIFFError(tif->tif_name,
		    _T("Cannot write directory, out of space"));
		return (0);
	}
	/*
	 * Directory hasn't been placed yet, put
	 * it at the end of the file and link it
	 * into the existing directory structure.
	 */
	if (tif->tif_diroff == 0 && !TIFFLinkDirectory(tif))
		goto bad;
	tif->tif_dataoff =
	    tif->tif_diroff + sizeof (short) + dirsize + sizeof (long);
	if (tif->tif_dataoff & 1)
		tif->tif_dataoff++;
	(void) TIFFSeekFile(tif, tif->tif_dataoff, L_SET);
	tif->tif_curdir++;
	dir = (TIFFDirEntry FAR*)data;
	/*
	 * Setup external form of directory
	 * entries and write data items.
	 */
	memcpy(fields, td->td_fieldsset, sizeof (fields));
	/*
	 * Write out ExtraSamples tag only if
	 * extra samples are present in the data.
	 */
	if (FieldSet(fields, FIELD_EXTRASAMPLES) && td->td_extrasamples) {
		ResetFieldBit(fields, FIELD_EXTRASAMPLES);
		nfields--;
		dirsize -= sizeof (TIFFDirEntry);
	}								/*XXX*/
	for (fip = tiffFieldInfo; fip->field_tag; fip++) {
		if (fip->field_bit == FIELD_IGNORE ||
		    !FieldSet(fields, fip->field_bit))
			continue;
		switch (fip->field_bit) {
		case FIELD_STRIPOFFSETS:
			/*
			 * We use one field bit for both strip and tile
			 * offsets, and so must be careful in selecting
			 * the appropriate field descriptor (so that tags
			 * are written in sorted order).
			 */
			tag = (WORD)(isTiled(tif) ?
			    TIFFTAG_TILEOFFSETS : TIFFTAG_STRIPOFFSETS);
			if (tag != fip->field_tag)
				continue;
			if (!TIFFWriteLongArray(tif, TIFF_LONG, tag, dir,
			    (DWORD) td->td_nstrips, td->td_stripoffset))
				goto bad;
			break;
		case FIELD_STRIPBYTECOUNTS:
			/*
			 * We use one field bit for both strip and tile
			 * byte counts, and so must be careful in selecting
			 * the appropriate field descriptor (so that tags
			 * are written in sorted order).
			 */
			tag = (WORD)(isTiled(tif) ?
			    TIFFTAG_TILEBYTECOUNTS : TIFFTAG_STRIPBYTECOUNTS);
			if (tag != fip->field_tag)
				continue;
			if (!TIFFWriteLongArray(tif, TIFF_LONG, tag, dir,
			    (DWORD) td->td_nstrips, td->td_stripbytecount))
				goto bad;
			break;
		case FIELD_ROWSPERSTRIP:
			TIFFSetupShortLong(tif, TIFFTAG_ROWSPERSTRIP,
			    dir, td->td_rowsperstrip);
			break;
		case FIELD_COLORMAP:
			if (!TIFFWriteShortTable(tif, TIFFTAG_COLORMAP, dir,
			    3, td->td_colormap))
				goto bad;
			break;
		case FIELD_IMAGEDIMENSIONS:
			TIFFSetupShortLong(tif, TIFFTAG_IMAGEWIDTH,
			    dir++, td->td_imagewidth);
			TIFFSetupShortLong(tif, TIFFTAG_IMAGELENGTH,
			    dir, td->td_imagelength);
			break;
		case FIELD_TILEDIMENSIONS:
			TIFFSetupShortLong(tif, TIFFTAG_TILEWIDTH,
			    dir++, td->td_tilewidth);
			TIFFSetupShortLong(tif, TIFFTAG_TILELENGTH,
			    dir, td->td_tilelength);
			break;
		case FIELD_POSITION:
			WriteRationalPair(TIFF_RATIONAL,
			    TIFFTAG_XPOSITION, td->td_xposition,
			    TIFFTAG_YPOSITION, td->td_yposition);
			break;
		case FIELD_RESOLUTION:
			WriteRationalPair(TIFF_RATIONAL,
			    TIFFTAG_XRESOLUTION, td->td_xresolution,
			    TIFFTAG_YRESOLUTION, td->td_yresolution);
			break;
		case FIELD_BITSPERSAMPLE:
		case FIELD_MINSAMPLEVALUE:
		case FIELD_MAXSAMPLEVALUE:
		case FIELD_SAMPLEFORMAT:
			if (!TIFFWritePerSampleShorts(tif, fip->field_tag, dir))
				goto bad;
			break;
		case FIELD_PAGENUMBER:
		case FIELD_HALFTONEHINTS:
#ifdef YCBCR_SUPPORT
		case FIELD_YCBCRSUBSAMPLING:
#endif
#ifdef CMYK_SUPPORT
		case FIELD_DOTRANGE:
#endif
			if (!TIFFSetupShortPair(tif, fip->field_tag, dir))
				goto bad;
			break;
#ifdef JPEG_SUPPORT
		case FIELD_JPEGQTABLES:
			if (!TIFFWriteJPEGQTables(tif, dir))
				goto bad;
			break;
		case FIELD_JPEGDCTABLES:
			if (!TIFFWriteJPEGCTables(tif,
			    TIFFTAG_JPEGDCTABLES, dir, td->td_dctab))
				goto bad;
			break;
		case FIELD_JPEGACTABLES:
			if (!TIFFWriteJPEGCTables(tif,
			    TIFFTAG_JPEGACTABLES, dir, td->td_actab))
				goto bad;
			break;
#endif
#ifdef COLORIMETRY_SUPPORT
		case FIELD_TRANSFERFUNCTION:
			if (!TIFFWriteTransferFunction(tif, dir))
				goto bad;
			break;
#endif
		default:
			if (!TIFFWriteNormalTag(tif, dir, fip))
				goto bad;
			break;
		}
		dir++;
		ResetFieldBit(fields, fip->field_bit);
	}
	/*
	 * Write directory.
	 */
	(void) TIFFSeekFile(tif, tif->tif_diroff, L_SET);
	dircount = (WORD)nfields;
	if (!WriteOK(tif, &dircount, sizeof (short))) {
		TIFFError(tif->tif_name, _T("Error writing directory count"));
		goto bad;
	}
	if (!WriteOK(tif, data, dirsize)) {
		TIFFError(tif->tif_name, _T("Error writing directory contents"));
		goto bad;
	}
	if (!WriteOK(tif, &tif->tif_nextdiroff, sizeof (long))) {
		TIFFError(tif->tif_name, _T("Error writing directory link"));
		goto bad;
	}
	TIFFFreeDirectory(tif);
	_TIFFfree(data);
	tif->tif_flags &= ~TIFF_DIRTYDIRECT;

	/*
	 * Reset directory-related state for subsequent
	 * directories.
	 */
	TIFFDefaultDirectory(tif);
	tif->tif_diroff = 0;
	tif->tif_curoff = 0;
	tif->tif_row = -1;
	tif->tif_curstrip = -1;
	return (1);
bad:
	_TIFFfree(data);
	return (0);
}
#undef WriteRationalPair

/*
 * Process tags that are not special cased.
 */
long
SECTiff::TIFFWriteNormalTag(TIFF* tif, TIFFDirEntry FAR* dir, const TIFFFieldInfo* fip)
{
	TIFFDirectory* td = &tif->tif_dir;
	WORD wc = (WORD) fip->field_writecount;

	dir->tdir_tag = (WORD)fip->field_tag;
	dir->tdir_type = (WORD)fip->field_type;
	dir->tdir_count = wc;
#define	WRITEDIRW(x,y)	x(tif, fip->field_type, fip->field_tag, dir, wc, y)
	switch (fip->field_type) {
	case TIFF_SHORT:
	case TIFF_SSHORT:
		if (wc > 1) {
			LPWORD wp;
			if (wc == (WORD) TIFF_VARIABLE) {
				_TIFFgetfield(td, fip->field_tag, &wc, &wp);
				dir->tdir_count = wc;
			} else
				_TIFFgetfield(td, fip->field_tag, &wp);
			if (!WRITEDIRW(TIFFWriteShortArray, wp))
				return (0);
		} else {
			WORD sv;
			_TIFFgetfield(td, fip->field_tag, &sv);
			dir->tdir_offset =
			    TIFFInsertData(tif, dir->tdir_type, sv);
		}
		break;
	case TIFF_LONG:
	case TIFF_SLONG:
		if (wc > 1) {
			LPDWORD lp;
			if (wc == (WORD) TIFF_VARIABLE) {
				_TIFFgetfield(td, fip->field_tag, &wc, &lp);
				dir->tdir_count = wc;
			} else
				_TIFFgetfield(td, fip->field_tag, &lp);
			if (!WRITEDIRW(TIFFWriteLongArray, lp))
				return (0);
		} else {
			/* XXX handle LONG->SHORT conversion */
			_TIFFgetfield(td, fip->field_tag, &dir->tdir_offset);
		}
		break;
	case TIFF_RATIONAL:
	case TIFF_SRATIONAL:
		if (wc > 1) {
			LPFLOAT fp;
			if (wc == (WORD) TIFF_VARIABLE) {
				_TIFFgetfield(td, fip->field_tag, &wc, &fp);
				dir->tdir_count = wc;
			} else
				_TIFFgetfield(td, fip->field_tag, &fp);
			if (!WRITEDIRW(TIFFWriteRationalArray, fp))
				return (0);
		} else {
			float fv;
			_TIFFgetfield(td, fip->field_tag, &fv);
			if (!TIFFWriteRational(tif, fip->field_type, fip->field_tag, dir, fv))
				return (0);
		}
		break;
	case TIFF_FLOAT:
		if (wc > 1) {
			LPFLOAT fp;
			if (wc == (WORD) TIFF_VARIABLE) {
				_TIFFgetfield(td, fip->field_tag, &wc, &fp);
				dir->tdir_count = wc;
			} else
				_TIFFgetfield(td, fip->field_tag, &fp);
			if (!WRITEDIRW(TIFFWriteFloatArray, fp))
				return (0);
		} else {
			float fv;
			_TIFFgetfield(td, fip->field_tag, &fv);
			TIFFCvtNativeToIEEEFloat(tif, 1, &fv);
			/* XXX assumes sizeof (long) == sizeof (float) */
			dir->tdir_offset = *(DWORD*) &fv;	/* XXX */
		}
		break;
	case TIFF_ASCII: {
		TCHAR *cp;
		_TIFFgetfield(td, fip->field_tag, &cp);
		if (!TIFFWriteString(tif, fip->field_tag, dir, cp))
			return (0);
		break;
	}
	}
	return (1);
}
#undef WRITEDIRW

/*
 * Setup a directory entry with either a SHORT
 * or LONG type according to the value.
 */
void
SECTiff::TIFFSetupShortLong(TIFF* tif, ttag_t tag, TIFFDirEntry FAR* dir, DWORD v)
{
	dir->tdir_tag = (WORD)tag;
	dir->tdir_count = 1;
	if (v > 0xffffL) {
		dir->tdir_type = (short)TIFF_LONG;
		dir->tdir_offset = v;
	} else {
		dir->tdir_type = (short)TIFF_SHORT;
		dir->tdir_offset = TIFFInsertData(tif, (long)TIFF_SHORT, v);
	}
}
#undef MakeShortDirent

/*
 * Setup a RATIONAL directory entry and
 * write the associated indirect value.
 */
long
SECTiff::TIFFWriteRational(TIFF* tif,
    TIFFDataType type, ttag_t tag, TIFFDirEntry FAR* dir, float v)
{
	return (TIFFWriteRationalArray(tif, type, tag, dir, 1, &v));
}

#define	NITEMS(x)	(sizeof (x) / sizeof (x[0]))
/*
 * Setup a directory entry that references a
 * samples/pixel array of SHORT values and
 * (potentially) write the associated indirect
 * values.
 */
long
SECTiff::TIFFWritePerSampleShorts(TIFF* tif, ttag_t tag, TIFFDirEntry FAR* dir)
{
	WORD buf[10], v;
	LPWORD w = buf;
	long i, status, samples = tif->tif_dir.td_samplesperpixel;

	if (samples > NITEMS(buf))
		w = (LPWORD)_TIFFmalloc((size_t)(samples * sizeof (WORD)));
	_TIFFgetfield(&tif->tif_dir, tag, &v);
	for (i = 0; i < samples; i++)
		w[i] = v;
	status = TIFFWriteShortArray(tif, TIFF_SHORT, tag, dir, samples, w);
	if (w != buf)
		_TIFFfree(w);
	return (status);
}
#undef NITEMS

/*
 * Setup a pair of shorts that are returned by
 * value, rather than as a reference to an array.
 */
long
SECTiff::TIFFSetupShortPair(TIFF* tif, ttag_t tag, TIFFDirEntry FAR* dir)
{
	WORD v[2];

	_TIFFgetfield(&tif->tif_dir, tag, &v[0], &v[1]);
	return (TIFFWriteShortArray(tif, TIFF_SHORT, tag, dir, 2, v));
}

/*
 * Setup a directory entry for an NxM table of shorts,
 * where M is known to be 2**bitspersample, and write
 * the associated indirect data.
 */
long
SECTiff::TIFFWriteShortTable(TIFF* tif,
    ttag_t tag, TIFFDirEntry FAR* dir, DWORD n, LPPWORD table)
{
	DWORD i, off;

	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)TIFF_SHORT;
	/* XXX -- yech, fool TIFFWriteData */
	dir->tdir_count = 1L<<tif->tif_dir.td_bitspersample;
	off = tif->tif_dataoff;
	for (i = 0; i < n; i++)
		if (!TIFFWriteData(tif, dir, (LPBYTE)table[i]))
			return (0);
	dir->tdir_count *= n;
	dir->tdir_offset = off;
	return (1);
}

/*
 * Setup a directory entry of an ASCII string
 * and write any associated indirect value.
 */
long
SECTiff::TIFFWriteString(TIFF* tif, ttag_t tag, TIFFDirEntry FAR* dir, TCHAR *cp)
{
	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)TIFF_ASCII;
	dir->tdir_count = (_tcslen(cp) + 1) * sizeof(TCHAR);	/* includes \0 byte */
	if (dir->tdir_count > 4) {
		//AAB if (!TIFFWriteData(tif, dir, cp))
		//AAB typecast ok here going from near to far...
		if (!TIFFWriteData(tif, dir, (LPBYTE)cp))
			return (0);
	} else
#ifdef WIN32
		memcpy(&dir->tdir_offset, cp, (size_t)dir->tdir_count);
#else
		_fmemcpy(&dir->tdir_offset, cp, (size_t)dir->tdir_count);
#endif
	return (1);
}

/*
 * Setup a directory entry of an array of SHORT
 * or SSHORT and write the associated indirect values.
 */
long
SECTiff::TIFFWriteShortArray(TIFF* tif,
    TIFFDataType type, ttag_t tag, TIFFDirEntry FAR* dir, DWORD n, LPWORD v)
{
	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)type;
	dir->tdir_count = n;
	if (n <= 2) {
		if (tif->tif_header.tiff_magic == TIFF_BIGENDIAN) {
			dir->tdir_offset = (long)v[0] << 16;
			if (n == 2)
				dir->tdir_offset |= v[1] & 0xffff;
		} else {
			dir->tdir_offset = v[0] & 0xffff;
			if (n == 2)
				dir->tdir_offset |= (long)v[1] << 16;
		}
		return (1);
	} else
		return (TIFFWriteData(tif, dir, (LPBYTE)v));
}

/*
 * Setup a directory entry of an array of LONG
 * or SLONG and write the associated indirect values.
 */
long
SECTiff::TIFFWriteLongArray(TIFF* tif,
    TIFFDataType type, ttag_t tag, TIFFDirEntry FAR* dir, DWORD n, LPDWORD v)
{
	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)type;
	dir->tdir_count = n;
	if (n == 1) {
		dir->tdir_offset = v[0];
		return (1);
	} else
		return (TIFFWriteData(tif, dir, (LPBYTE)v));
}

/*
 * Setup a directory entry of an array of RATIONAL
 * or SRATIONAL and write the associated indirect values.
 */
long
SECTiff::TIFFWriteRationalArray(TIFF* tif,
    TIFFDataType type, ttag_t tag, TIFFDirEntry FAR* dir, DWORD n, LPFLOAT v)
{
	DWORD i;
	LPDWORD t;
	long status;

	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)type;
	dir->tdir_count = n;
	t = (LPDWORD)_TIFFmalloc((size_t)(2*n * sizeof (DWORD)));
	for (i = 0; i < n; i++) {
		float fv = v[i];
		long sign = 1;
		DWORD den;

		if (fv < 0) {
			if (type == TIFF_RATIONAL) {
				TIFFWarning(tif->tif_name,
	_T("\"%s\": Information lost writing value (%g) as (unsigned) RATIONAL"),
					TIFFFieldWithTag(tag)->field_name, v);
				fv = (float)0.;
			} else
				fv = -fv, sign = -1;
		}
		den = 1L;
		if (fv > 0) {
			while (fv < 1L<<(31-3) && den < 1L<<(31-3))
				fv *= 1<<3, den *= 1L<<3;
		}
		t[2*i+0] = (DWORD)(sign * (fv + 0.5));
		t[2*i+1] = den;
	}
	status = TIFFWriteData(tif, dir, (LPBYTE)t);
	_TIFFfree(t);
	return (status);
}

long
SECTiff::TIFFWriteFloatArray(TIFF* tif,
    TIFFDataType type, ttag_t tag, TIFFDirEntry FAR* dir, DWORD n, LPFLOAT v)
{
	dir->tdir_tag = (WORD)tag;
	dir->tdir_type = (short)type;
	dir->tdir_count = n;
	TIFFCvtNativeToIEEEFloat(tif, n, v);
	if (n == 1) {
		dir->tdir_offset = *(LPDWORD)&v[0];
		return (1);
	} else
		return (TIFFWriteData(tif, dir, (LPBYTE)v));
}

#ifdef JPEG_SUPPORT
#define	NITEMS(x)	(sizeof (x) / sizeof (x[0]))
/*
 * Setup a directory entry for JPEG Quantization
 * tables and write the associated indirect values.
 */
long
SECTiff::TIFFWriteJPEGQTables(TIFF* tif, TIFFDirEntry FAR* dir)
{
	long i, status = 0, samples = tif->tif_dir.td_samplesperpixel;
	DWORD buf[10];
	LPDWORD off = buf;
	TIFFDirEntry tdir;

	tdir.tdir_tag = TIFFTAG_JPEGQTABLES;	/* for diagnostics */
	tdir.tdir_type = (short)TIFF_BYTE;
	tdir.tdir_count = 64;
	if (samples > NITEMS(buf))
		off = (LPDWORD)_TIFFmalloc((size_t)(samples * sizeof (DWORD)));
	for (i = 0; i < samples; i++) {
		if (!TIFFWriteData(tif, &tdir, (LPBYTE)tif->tif_dir.td_qtab[i]))
			goto bad;
		off[i] = tdir.tdir_offset;
	}
	status = TIFFWriteLongArray(tif, TIFF_LONG,
	    TIFFTAG_JPEGQTABLES, dir, samples, off);
bad:
	if (off != buf)
		_TIFFfree(off);
	return (status);
}

/*
 * Setup a directory entry for JPEG Coefficient
 * tables and write the associated indirect values.
 */
long
SECTiff::TIFFWriteJPEGCTables(TIFF* tif, ttag_t tag, TIFFDirEntry FAR* dir, LPPBYTE tab)
{
	long status = 0, samples = tif->tif_dir.td_samplesperpixel;
	DWORD buf[10];
	LPDWORD off = buf;
	TIFFDirEntry tdir;
	long i, j, ncodes;

	tdir.tdir_tag = (WORD)tag;		/* for diagnostics */
	tdir.tdir_type = (short)TIFF_BYTE;
	if (samples > NITEMS(buf))
		off = (LPDWORD)_TIFFmalloc((size_t)(samples * sizeof (DWORD)));
	for (i = 0; i < samples; i++) {
		for (ncodes = 0, j = 0; j < 16; j++)
			ncodes += tab[i][j];
		tdir.tdir_count = 16+ncodes;
		if (!TIFFWriteData(tif, &tdir, (LPBYTE)tab[i]))
			goto bad;
		off[i] = tdir.tdir_offset;
	}
	status = TIFFWriteLongArray(tif, TIFF_LONG, tag, dir, samples, off);
bad:
	if (off != buf)
		_TIFFfree(off);
	return (status);
}
#undef NITEMS
#endif

#ifdef COLORIMETRY_SUPPORT
long
SECTiff::TIFFWriteTransferFunction(TIFF* tif, TIFFDirEntry FAR* dir)
{
	TIFFDirectory* td = &tif->tif_dir;
	DWORD n = (1L<<td->td_bitspersample) * sizeof (WORD);
	LPPWORD tf = td->td_transferfunction;
	long ncols;

	/*
	 * Check if the table can be written as a single column,
	 * or if it must be written as 3 columns.  Note that we
	 * write a 3-column tag if there are 2 samples/pixel and
	 * a single column of data won't suffice--hmm.
	 */
	switch (td->td_samplesperpixel - td->td_extrasamples) {
#ifdef WIN32
	default:	if (memcmp(tf[0], tf[2], (size_t)n)) { ncols = 3; break; }
	case 2:		if (memcmp(tf[0], tf[1], (size_t)n)) { ncols = 3; break; }
#else
	default:	if (_fmemcmp(tf[0], tf[2], (size_t)n)) { ncols = 3; break; }
	case 2:		if (_fmemcmp(tf[0], tf[1], (size_t)n)) { ncols = 3; break; }
#endif
	case 1: case 0:	ncols = 1;
	}
	return (TIFFWriteShortTable(tif,
	    TIFFTAG_TRANSFERFUNCTION, dir, ncols, tf));
}
#endif

/*
 * Write a contiguous directory item.
 */
long
SECTiff::TIFFWriteData(TIFF* tif, TIFFDirEntry FAR* dir, LPBYTE cp)
{
	tsize_t cc;

	dir->tdir_offset = tif->tif_dataoff;
	cc = dir->tdir_count * tiffDataWidth[dir->tdir_type];
	if (SeekOK(tif, dir->tdir_offset) &&
	    WriteOK(tif, cp, cc)) {
		tif->tif_dataoff += (cc + 1) & ~1;
		return (1);
	}
	TIFFError(tif->tif_name, _T("Error writing data for field \"%s\""),
	    TIFFFieldWithTag(dir->tdir_tag)->field_name);
	return (0);
}

/*
 * Link the current directory into the
 * directory chain for the file.
 */
long
SECTiff::TIFFLinkDirectory(TIFF* tif)
{
	static const TCHAR module[] = _T("TIFFLinkDirectory");
	WORD dircount;
	DWORD nextdir;

	tif->tif_diroff = (TIFFSeekFile(tif, 0L, L_XTND)+1) &~ 1L;
	if (tif->tif_header.tiff_diroff == 0) {
		/*
		 * First directory, overwrite header.
		 */
		tif->tif_header.tiff_diroff = tif->tif_diroff;
		(void) TIFFSeekFile(tif, 0L, L_SET);
		if (!WriteOK(tif, &tif->tif_header,
		    sizeof (tif->tif_header))) {
			TIFFError(tif->tif_name, _T("Error writing TIFF header"));
			return (0);
		}
		return (1);
	}
	/*
	 * Not the first directory, search to the last and append.
	 */
	nextdir = tif->tif_header.tiff_diroff;
	do {
		if (!SeekOK(tif, nextdir) ||
		    !ReadOK(tif, &dircount, sizeof (WORD))) {
			TIFFError(module, _T("Error fetching directory count"));
			return (0);
		}
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabShort(&dircount);
		TIFFSeekFile(tif, dircount * sizeof (TIFFDirEntry), L_INCR);
		if (!ReadOK(tif, &nextdir, sizeof (DWORD))) {
			TIFFError(module, _T("Error fetching directory link"));
			return (0);
		}
		if (tif->tif_flags & TIFF_SWAB)
			TIFFSwabLong(&nextdir);
	} while (nextdir != 0);
	(void) TIFFSeekFile(tif, (toff_t)-1 * (toff_t)sizeof (nextdir), L_INCR);
	if (!WriteOK(tif, &tif->tif_diroff, sizeof (tif->tif_diroff))) {
		TIFFError(module, _T("Error writing directory link"));
		return (0);
	}
	return (1);
}
