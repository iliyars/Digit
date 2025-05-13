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
//  Description:	tif_fax4.cpp
//		CCITT Group 4 Facsimile-compatible compression scheme support
//
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
// 	AAB 7/1/95	Initial implementation
//
//



#include "..\..\..\stdafx.h"

//AAB #ifdef WIN32
#ifndef __TIFFIOP_H__
#include "..\..\..\Include\Image\tiffiop.h"
#endif

#ifndef __TIF_FAX3_H__
#include "..\..\..\Include\Image\tif_fax3.h"
#endif

#ifndef __T4_H__
#include "..\..\..\Include\Image\t4.h"
#endif

#ifndef __SECTIFF_H__
#include "..\..\..\Include\Image\sectiff.h"
#endif

/*
 * Decode the requested amount of data.
 */
long
SECTiff::Fax4Decode(TIFF* tif, tidata_t buf, tsize_t occ, tsample_t s)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;
	long status;

	memset(buf, 0, (size_t)occ);		/* decoding only sets non-zero bits */
	while ((long)occ > 0) {
		status = Fax3Decode2DRow(tif, (BYTE *)buf, (DWORD)sp->rowpixels);
		if (status < 0)
			return (status == G3CODE_EOF);
		memcpy(sp->refline, buf, (size_t)sp->rowbytes);
		buf += sp->rowbytes;
		occ -= sp->rowbytes;
		if (occ != 0)
			tif->tif_row++;
	}			  
	s; //UNUSED
	return (1);
}

/*
 * Encode the requested amount of data.
 */
 long
SECTiff::Fax4Encode(TIFF* tif, tidata_t bp, tsize_t cc, tsample_t s)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	while ((long)cc > 0) {
		long test = Fax3Encode2DRow(tif, (BYTE *)bp, (BYTE *)sp->refline, (DWORD)sp->rowpixels);
		if (!test)
			return (0);
		memcpy(sp->refline, bp, (size_t)sp->rowbytes);
		bp += sp->rowbytes;
		cc -= sp->rowbytes;
		if (cc != 0)
			tif->tif_row++;
	}
s; //UNUSED
	return (1);
}

long
SECTiff::Fax4PostEncode(TIFF* tif)
{
	Fax3BaseState *sp = (Fax3BaseState *)tif->tif_data;

	/* terminate strip w/ EOFB */
	Fax3PutBits(tif, EOL, 12);
	Fax3PutBits(tif, EOL, 12);
	if (sp->bit != 8)
		Fax3FlushBits(tif, sp);
	return (1);
}

long
SECTiff::TIFFInitCCITTFax4(TIFF* tif)
{
	TIFFInitCCITTFax3(tif);		/* reuse G3 compression */
	tif->tif_decoderow = &SECTiff::Fax4Decode;
	tif->tif_decodestrip = &SECTiff::Fax4Decode;
	tif->tif_decodetile = &SECTiff::Fax4Decode;
	tif->tif_encoderow = &SECTiff::Fax4Encode;
	tif->tif_encodestrip = &SECTiff::Fax4Encode;
	tif->tif_encodetile = &SECTiff::Fax4Encode;
	tif->tif_postencode = &SECTiff::Fax4PostEncode;
	/*
	 * FAX3_NOEOL causes the regular G3 decompression
	 * code to not skip to the EOL mark at the end of
	 * a row (during normal decoding).  FAX3_CLASSF
	 * suppresses RTC generation at the end of an image.
	 */
	tif->tif_options = FAX3_NOEOL|FAX3_CLASSF;
	return (1);
}
//AAB #endif /*WIN32*/
