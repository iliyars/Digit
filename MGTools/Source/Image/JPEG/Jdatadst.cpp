/*
 * jdatadst.c
 *
 * Copyright (C) 1994, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains compression data destination routines for the case of
 * emitting JPEG data to a file (or any stdio stream).  While these routines
 * are sufficient for most applications, some will want to use a different
 * destination manager.
 * IMPORTANT: we assume that fwrite() will correctly transcribe an array of
 * JOCTETs into 8-bit-wide elements on external storage.  If char is wider
 * than 8 bits on your machine, you may need to do some tweaking.
 */

/* this is not a core library module, so it doesn't define JPEG_INTERNALS */
#include "..\..\..\stdafx.h"

#ifndef __SECJPEG_H__
#include "..\..\..\Include\Image\secjpeg.h"
#endif

#include "..\..\..\Include\Image\jerror.h"


/* Expanded data destination object for stdio output */

typedef struct {
  struct jpeg_destination_mgr pub; /* public fields */

  //AAB FILE * outfile;		/* target stream */
  CFile *outfile;
  JOCTET * buffer;		/* start of buffer */
} my_destination_mgr;

typedef my_destination_mgr * my_dest_ptr;

#define OUTPUT_BUF_SIZE  4096	/* choose an efficiently fwrite'able size */


/*
 * Initialize destination --- called by jpeg_start_compress
 * before any data is actually written.
 */

// METHODDEF void
void SECJpeg::init_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  /* Allocate the output buffer --- it will be released when done with image */
  dest->buffer = (JOCTET *)
      (this->*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
				  OUTPUT_BUF_SIZE * SIZEOF(JOCTET));

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;
}


/*
 * Empty the output buffer --- called whenever buffer fills up.
 *
 * In typical applications, this should write the entire output buffer
 * (ignoring the current state of next_output_byte & free_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been dumped.
 *
 * In applications that need to be able to suspend compression due to output
 * overrun, a FALSE return indicates that the buffer cannot be emptied now.
 * In this situation, the compressor will return to its caller (possibly with
 * an indication that it has not accepted all the supplied scanlines).  The
 * application should resume compression after it has made more room in the
 * output buffer.  Note that there are substantial restrictions on the use of
 * suspension --- see the documentation.
 *
 * When suspending, the compressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_output_byte & free_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point will be regenerated after resumption, so do not
 * write it out when emptying the buffer externally.
 */

// METHODDEF boolean
boolean SECJpeg::empty_output_buffer (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;

  //AAB if (JFWRITE(dest->outfile, dest->buffer, OUTPUT_BUF_SIZE) !=
  //AAB    (size_t) OUTPUT_BUF_SIZE)
  //AAB  SEC_ERREXIT(cinfo, JERR_FILE_WRITE);
  JFWRITE(dest->outfile, dest->buffer, OUTPUT_BUF_SIZE);

  dest->pub.next_output_byte = dest->buffer;
  dest->pub.free_in_buffer = OUTPUT_BUF_SIZE;

  return TRUE;
}


/*
 * Terminate destination --- called by jpeg_finish_compress
 * after all data has been written.  Usually needs to flush buffer.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

// METHODDEF void
void SECJpeg::term_destination (j_compress_ptr cinfo)
{
  my_dest_ptr dest = (my_dest_ptr) cinfo->dest;
  size_t datacount = OUTPUT_BUF_SIZE - dest->pub.free_in_buffer;

  /* Write any data remaining in the buffer */
  if (datacount > 0) {
    //AAB if (JFWRITE(dest->outfile, dest->buffer, datacount) != datacount)
    //AAB  SEC_ERREXIT(cinfo, JERR_FILE_WRITE);
	JFWRITE(dest->outfile, dest->buffer, datacount);
  }
  //AAB fflush(dest->outfile);
  dest->outfile->Flush();
  /* Make sure we wrote the output file OK */
  //AAB if (ferror(dest->outfile))
  //AAB   SEC_ERREXIT(cinfo, JERR_FILE_WRITE);
}


/*
 * Prepare for output to a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing compression.
 */

// GLOBAL void
void SECJpeg::jpeg_stdio_dest (j_compress_ptr cinfo, CFile * outfile)
{
  my_dest_ptr dest;

  /* The destination object is made permanent so that multiple JPEG images
   * can be written to the same file without re-executing jpeg_stdio_dest.
   * This makes it dangerous to use this manager and a different destination
   * manager serially with the same JPEG object, because their private object
   * sizes may be different.  Caveat programmer.
   */
  if (cinfo->dest == NULL) {	/* first time for this JPEG object? */
    cinfo->dest = (struct jpeg_destination_mgr *)
      (this->*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  SIZEOF(my_destination_mgr));
  }

  dest = (my_dest_ptr) cinfo->dest;
  dest->pub.init_destination = &SECJpeg::init_destination;
  dest->pub.empty_output_buffer = &SECJpeg::empty_output_buffer;
  dest->pub.term_destination = &SECJpeg::term_destination;
  dest->outfile = outfile;
}
