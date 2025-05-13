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
//  Description:	SECGif stubbed implementation file
//  Created:		Tue Aug 29 15:15:44 1995
//
//  CHANGELOG:
//
//   MSW     9/30/95    Code Review
//
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __SECIMAGE_H__
#include "..\..\Include\Image\secimage.h"
#endif

#ifndef __SECGIF_H__
#include "..\..\Include\Image\secgif.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif


#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

IMPLEMENT_SERIAL(SECGif, CObject, 1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#define TRACE_GIF	TRACE(_T("GIF/LZW decoding module not enabled!  Contact Stingray Software for details on unlocking the GIF module\n"));

//
//	constructor for GIF class
//
//
//@doc SECGif
//@mfunc Constructs a SECGif object.
//@xref <c SECGif>
SECGif::SECGif()
{
    m_lpRawGIF = NULL;
    m_lBitOffset = 0;
    m_wXC = 0;
    m_wYC = 0;
    m_lPass = 0;
    m_bInterlaced = 0;
}


SECGif::~SECGif()
{
    // AAB nothing for now
}  
 

// virtual classes fleshed out
BOOL SECGif::DoSaveImage(CFile* pFile)
{
    pFile; // UNUSED
    TRACE_GIF
    return FALSE;	
}

BOOL SECGif::DoLoadImage(CFile* pFile)
{
   pFile; // UNUSED
   TRACE_GIF
   return FALSE;
}

// Create a palette based on GIF color table (global or local)
BOOL SECGif::SECCreateColorTable(int nColors)
{
	nColors;
	TRACE_GIF
    return FALSE;
}

BOOL SECGif::ReadImageHeader()
{
    TRACE_GIF
	return FALSE;
}


BOOL SECGif::ReadImage()
{
	TRACE_GIF
    return FALSE;  
}

int SECGif::ShiftRasterBytes(long nShift)
{
	nShift;	
    TRACE_GIF
    return FALSE;         
}


int SECGif::ReadCode(BOOL bInit)
{
	bInit;

    TRACE_GIF
	return FALSE;
     
}

BOOL SECGif::ReadTrailer()
{
    return TRUE;
}

void SECGif::DoInterlace(int Index)
{
	Index;
}


BOOL SECGif::ReadExtension()
{
   return FALSE;
}





DWORD
SECGif::LZWEncodeImage(DWORD data_size)
{
	data_size;
	TRACE_GIF
	return FALSE;
}         
 
void SECGif::Serialize(CArchive& ar)
{
	ar;
}
                 
#ifdef _DEBUG
void SECGif::AssertValid() const
{

}
#endif /* _DEBUG */




