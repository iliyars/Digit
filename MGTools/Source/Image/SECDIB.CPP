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
//
//  Author:		AAB
//  Description:	Implementation of SECDib
//  Created:		Tue Aug 29 15:05:21 1995
//
//  CHANGELOG:
//
//    MSW     9/30/95    Code Review
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _CMNDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif

//applicable includes go here

#ifndef __SECIMAGE_H__
#include "..\..\Include\Image\secimage.h"
#endif

#ifndef __SECDIB_H__
#include "..\..\Include\Image\secdib.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif


#ifdef _CMNDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif


IMPLEMENT_SERIAL(SECDib, CObject, 1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


//@doc SECDib
//@mfunc Constructs a SECDib object.
//@xref <c SECImage> <c SECDib>
SECDib::SECDib()
{
	m_lpSrcBits = NULL;
}

SECDib::~SECDib()
{

}

BOOL SECDib::DoLoadImage(CFile* pFile)
{
    return SECLoadDib(pFile);
}

// Does not open or close pFile.  Assumes
// caller will do it.  Function has the
// following side effect: m_pFile is set.
BOOL SECDib::DoSaveImage(CFile* pFile)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pFile);
    
    BITMAPFILEHEADER bmfHdr;
    
    // Make sure bitmap data is in padded format
    PadBits();
    
    bmfHdr.bfType = DIB_HEADER_MARKER;
    // initialize to BitmapInfo size
    DWORD dwImageSize= m_lpBMI->bmiHeader.biSize;
	// Add in palette size
    WORD wColors = NumColors();
    WORD wPaletteSize = (WORD)(wColors*sizeof(RGBQUAD));
    dwImageSize+=wPaletteSize;
    
    // Add in size of actual bit array
    dwImageSize+=PADWIDTH((m_dwWidth)*(DWORD)(m_lpBMI->bmiHeader.biBitCount)/8) * m_dwHeight;
    m_lpBMI->bmiHeader.biSizeImage = 0; //AAB CHECK dwImageSize;
    bmfHdr.bfSize = dwImageSize + sizeof(BITMAPFILEHEADER);
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + m_lpBMI->bmiHeader.biSize + wPaletteSize;
    pFile->Write(&bmfHdr, sizeof(BITMAPFILEHEADER));
    
    pFile->Write(m_lpBMI, sizeof(BITMAPINFO) + (wColors-1)*sizeof(RGBQUAD));

	/*AAB
    pFile->WriteHuge(m_lpSrcBits, 
			(DWORD)((m_dwPadWidth*(DWORD)m_lpBMI->bmiHeader.biBitCount
				*m_dwHeight)/8) );
	*/
	DWORD dwTest = CalcBytesPerLine((DWORD)m_lpBMI->bmiHeader.biBitCount, m_dwWidth);
    pFile->Write(m_lpSrcBits, 
			(DWORD)((dwTest*m_dwHeight)) );
  
    return TRUE;
}

void SECDib::Serialize(CArchive& ar)
{
    SECImage::Serialize(ar);
}


#ifdef _DEBUG
void SECDib::AssertValid() const
{
    CObject::AssertValid();
	SECImage::AssertValid();
}
#endif /* _DEBUG */

