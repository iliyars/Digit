/*
 * jmemansi.c
 *
 * Copyright (C) 1992-1994, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides a simple generic implementation of the system-
 * dependent portion of the JPEG memory manager.  This implementation
 * assumes that you have the ANSI-standard library routine tmpfile().
 * Also, the problem of determining the amount of memory available
 * is shoved onto the user.
 */

#define JPEG_INTERNALS
#include "..\..\..\stdafx.h"

#ifndef __SECJPEG_H__
#include "..\..\..\Include\Image\secjpeg.h"
#endif

#ifndef __JMEMSYS_H__
#include "..\..\..\Include\Image\jmemsys.h"		/* import the system-dependent declarations */
#endif

#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare malloc(),free() */
extern void * malloc JPP((size_t size));
extern void free JPP((void *ptr));
#endif

#ifndef SEEK_SET		/* pre-ANSI systems may not define this; */
#define SEEK_SET  0		/* if not, assume 0 is correct */
#endif

#include  "windowsx.h"


/*
 * Memory allocation and freeing are controlled by the regular library
 * routines malloc() and free().
 */

// GLOBAL void *
void * SECJpeg::jpeg_get_small (j_common_ptr cinfo, size_t sizeofobject)
{
  return (void *) malloc(sizeofobject);
  cinfo; //unused
}

// GLOBAL void
void SECJpeg::jpeg_free_small (j_common_ptr cinfo, void * object, size_t sizeofobject)
{
  free(object);
  cinfo; //unused
  sizeofobject; //unused
}


/*
 * "Large" objects are treated the same as "small" ones.
 * NB: although we include FAR keywords in the routine declarations,
 * this file won't actually work in 80x86 small/medium model; at least,
 * you probably won't be able to process useful-size images in only 64KB.
 */

// GLOBAL void FAR *
#ifdef WIN32
void * SECJpeg::jpeg_get_large (j_common_ptr cinfo, long sizeofobject)
#else
void huge * SECJpeg::jpeg_get_large (j_common_ptr cinfo, long sizeofobject)
#endif
{
  //AAB return (void FAR *) malloc(sizeofobject);
#ifdef WIN32
	return (void *)GlobalAllocPtr(GHND, sizeofobject);
#else
	return (void huge *)GlobalAllocPtr(GHND, sizeofobject);
#endif
  cinfo; //unused
}

// GLOBAL void
#ifdef WIN32
void SECJpeg::jpeg_free_large (j_common_ptr cinfo, void * object, size_t sizeofobject)
#else
void SECJpeg::jpeg_free_large (j_common_ptr cinfo, void huge * object, size_t sizeofobject)
#endif
{
  //AAB free(object);
	if (object)
		GlobalFreePtr(object);
  cinfo; //unused
  sizeofobject; //unused
}


/*
 * This routine computes the total memory space available for allocation.
 * It's impossible to do this in a portable way; our current solution is
 * to make the user tell us (with a default value set at compile time).
 * If you can actually get the available space, it's a good idea to subtract
 * a slop factor of 5% or so.
 */

#ifndef DEFAULT_MAX_MEM		/* so can override from makefile */
#define DEFAULT_MAX_MEM		1000000L /* default: one megabyte */
#endif

// GLOBAL long
long SECJpeg::jpeg_mem_available (j_common_ptr cinfo, long min_bytes_needed,
		    long max_bytes_needed, long already_allocated)
{
  return cinfo->mem->max_memory_to_use - already_allocated;
  max_bytes_needed; //unused
  min_bytes_needed; //unused
}


/*
 * Backing store (temporary file) management.
 * Backing store objects are only used when the value returned by
 * jpeg_mem_available is less than the total space needed.  You can dispense
 * with these routines if you have plenty of virtual memory; see jmemnobs.c.
 */


// METHODDEF void
void SECJpeg::read_backing_store (j_common_ptr cinfo, backing_store_ptr info,
#ifdef WIN32
		    void * buffer_address,
#else
			void huge *buffer_address,
#endif
		    long file_offset, long byte_count)
{
//AAB  if (fseek(info->temp_file, file_offset, SEEK_SET))
  long lresult = info->pTempFile->Seek(file_offset,CFile::begin);
  if (lresult != file_offset)
    SEC_ERREXIT(cinfo, JERR_TFILE_SEEK);
  info->pTempFile->Flush(); //AAB TEST
  lresult = JFREAD(info->pTempFile, buffer_address, byte_count);
  if (lresult != (long) byte_count)
    SEC_ERREXIT(cinfo, JERR_TFILE_READ);
}


// METHODDEF void
void SECJpeg::write_backing_store (j_common_ptr cinfo, backing_store_ptr info,
		     void FAR * buffer_address,
		     long file_offset, long byte_count)
{
//   if (fseek(info->temp_file, file_offset, SEEK_SET))
  long lresult = info->pTempFile->Seek(file_offset,CFile::begin);
  if (lresult != file_offset)
    SEC_ERREXIT(cinfo, JERR_TFILE_SEEK);
  JFWRITE(info->pTempFile, buffer_address, byte_count);
  //AAB if (lresult != (long) byte_count)
  //AAB  SEC_ERREXIT(cinfo, JERR_TFILE_WRITE);
}


// METHODDEF void
void SECJpeg::close_backing_store (j_common_ptr cinfo, backing_store_ptr info)
{
  //AAB fclose(info->temp_file);
	//JOR Must keep a local copy of the file name so we can delete the file
	//   TMP files should be deleted after closing.
#ifdef WIN32
	CString str = info->pTempFile->GetFilePath();
#endif
	info->pTempFile->Close();
	delete info->pTempFile;
	info->pTempFile = NULL;

#ifdef WIN32
	//JOR Now we delete the temporary file...
	TRY
	{
		CFile::Remove( str );
	}
	CATCH( CFileException, e )
	{
		e->Delete();
	}
	END_CATCH
#endif

  /* Since this implementation uses tmpfile() to create the file,
   * no explicit file deletion is needed.
   */
  cinfo; //unused
}


/*
 * Initial opening of a backing-store object.
 *
 * This version uses tmpfile(), which constructs a suitable file name
 * behind the scenes.  We don't have to use info->temp_name[] at all;
 * indeed, we can't even find out the actual name of the temp file.
 */

// GLOBAL void
void SECJpeg::jpeg_open_backing_store (j_common_ptr cinfo, backing_store_ptr info,
			 long total_bytes_needed)
{

  TCHAR tempname[512];
  // char *pTest = _ttmpnam(temp);
  info->pTempFile = new CFile();
#ifdef WIN32
  TCHAR temppath[512];
  GetTempPath(512, temppath);
  GetTempFileName(temppath, _T("sec"), 0, tempname);
#else
  GetTempFileName(0, "sec", 0, tempname);
#endif
  BOOL bTest = info->pTempFile->Open(tempname, CFile::modeCreate|CFile::modeReadWrite);
  if (!bTest)
    SEC_ERREXITS(cinfo, JERR_TFILE_CREATE, "");
  info->read_backing_store = &SECJpeg::read_backing_store;
  info->write_backing_store = &SECJpeg::write_backing_store;
  info->close_backing_store = &SECJpeg::close_backing_store;
  total_bytes_needed; //unused
}


/*
 * These routines take care of any system-dependent initialization and
 * cleanup required.
 */

// GLOBAL long
long SECJpeg::jpeg_mem_init (j_common_ptr cinfo)
{
  return DEFAULT_MAX_MEM;	/* default for max_memory_to_use */
  cinfo; //unused
}

// GLOBAL void
void SECJpeg::jpeg_mem_term (j_common_ptr cinfo)
{
  /* no work */
	cinfo; //unused
}
