// 
// Stingray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All Rights Reserved
// 
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// Based originally on the imagick package by John Christy
// 		Copyright 1995 E. I. du Pont de Nemours and Company
//		
// See the SEC help files for detailed information
// regarding using SEC classes.
// 

//  Author:		AAB
//  Description:	SECPcx Implementation file
//  Created:		Tue Aug 29 15:17:15 1995
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __SECPCX_H__
#include "..\..\Include\Image\secpcx.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif


#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL


IMPLEMENT_SERIAL(SECPcx, CObject, 1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

//@doc SECPcx
//@mfunc Constructs a SECPcx object.
//@xref <c SECPcx>
SECPcx::SECPcx()
{
 	// m_bEGA = FALSE;  // flag for EGA palette format
}


SECPcx::~SECPcx()
{
	// AAB nothing for now
}


BOOL SECPcx::DoLoadImage(CFile* pFile)
{
	BOOL bEGA = FALSE; //AAB TODO: make a member

    if (pFile->Read(&m_PcxHdr, sizeof(PCXHDR)) != sizeof(PCXHDR))
	return FALSE;
    
    if (m_PcxHdr.cIdentifier != 0x0a)
	goto err_Pcx;
    if (m_PcxHdr.cVersion > 5)
	goto err_Pcx;
    
    m_dwWidth = m_PcxHdr.wXEnd - m_PcxHdr.wXStart + 1;
    m_dwPadWidth = PADWIDTH(m_dwWidth);
    m_dwHeight = m_PcxHdr.wYEnd - m_PcxHdr.wYStart + 1;
    m_nSrcBitsPerPixel = m_PcxHdr.cBitsPerPixel;

    m_wColors = (WORD)(1 << m_PcxHdr.cBitsPerPixel * m_PcxHdr.cNumBitPlanes); 

    if (m_wColors > 256)
	goto err_Pcx;
    
    // we treat monochrome images as 8 bit 
    if (m_PcxHdr.cBitsPerPixel == 1)
	{
		// MSS 1/15/99 include 2 bit plane images in bEGA processing
		if ((m_PcxHdr.cNumBitPlanes <= 4) && (m_PcxHdr.cNumBitPlanes >= 2))
		{
			bEGA = TRUE;
//			m_wColors = 16;
		}
		else
    		m_wColors = 256;
	}
    
    if (m_lpBMI)
	GlobalFreePtr(m_lpBMI);
    
    // now allocate the bitmap info structures since you know color
    m_lpBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + m_wColors*sizeof(RGBQUAD));
    if (!m_lpBMI)
	goto err_Pcx;
    
    // Now we can fill out some of the header info
    m_lpBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_lpBMI->bmiHeader.biWidth = (long)m_dwWidth;
    m_lpBMI->bmiHeader.biHeight = (long)m_dwHeight;
    m_lpBMI->bmiHeader.biPlanes = 1; // m_PcxHdr.cNumBitPlanes;
    m_lpBMI->bmiHeader.biBitCount = m_PcxHdr.cBitsPerPixel;
    m_lpBMI->bmiHeader.biCompression = BI_RGB;
    m_lpBMI->bmiHeader.biSizeImage = 0;
    m_lpBMI->bmiHeader.biXPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biYPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biClrUsed = 0;
    m_lpBMI->bmiHeader.biClrImportant = 0; 
    m_lpRGB = (LPRGBQUAD)(m_lpBMI->bmiColors);
    
    if (m_PcxHdr.cEncoding != 1)
	goto err_Pcx;
    
    if (m_PcxHdr.cBitsPerPixel != 1 && m_PcxHdr.cBitsPerPixel != 8)
	goto err_Pcx;
    
#ifdef WIN32
    m_lpSrcBits = (LPBYTE)GlobalAllocPtr(GHND, m_PcxHdr.cNumBitPlanes*(m_dwPadWidth*m_dwHeight) + 32); 
#else
    m_lpSrcBits = (BYTE huge *)GlobalAllocPtr(GHND, m_PcxHdr.cNumBitPlanes*(m_dwPadWidth*m_dwHeight) + 32);
#endif
    if (!m_lpSrcBits)
	goto err_Pcx;
    
    InitCache(SEC_PCX_CACHE_SIZE, SEC_READ_CACHE);
    LoadImageBits();
    
	m_lpBMI->bmiHeader.biBitCount = (WORD)(m_lpBMI->bmiHeader.biBitCount *m_PcxHdr.cNumBitPlanes);
	m_nSrcBitsPerPixel = (int)m_lpBMI->bmiHeader.biBitCount;
    if (m_lpBMI->bmiHeader.biBitCount < 8 )
    {
    	m_lpBMI->bmiHeader.biBitCount = 8;
		if (bEGA)
		{
			//  MSS 1/15/99  use m_wColors
    		m_lpBMI->bmiHeader.biClrUsed = 
    		m_lpBMI->bmiHeader.biClrImportant = m_wColors /* 16 */; // (1<<m_lpBMI->bmiHeader.biBitCount);
			m_nSrcBitsPerPixel = 8;
		}
		else
		{
      		m_lpBMI->bmiHeader.biClrUsed = 
    		m_lpBMI->bmiHeader.biClrImportant = (1<<m_lpBMI->bmiHeader.biBitCount);
			m_nSrcBitsPerPixel = 8;
		}
    }

    	
    LoadColorMap();
    PadBits(); 

    FlipVert();
    FreeCache();
    
    return (TRUE);
    
 err_Pcx:
    FreeCache();
    return FALSE;
}

void SECPcx::LoadColorMap()
{
    WORD i;
    BYTE cTmp;
    
    // Load in colormap
    if (m_wColors > 16)
	{
	    while((GetCache(&cTmp,1)) && (cTmp!=PCX_COLORMAP));
	    for (i=0; i<m_wColors; i++)
		{
		    if (!GetCache(&(m_lpRGB[i].rgbRed),1))
			break;
		    if (!GetCache(&(m_lpRGB[i].rgbGreen),1))
			break;
		    if (!GetCache(&(m_lpRGB[i].rgbBlue),1))
			break;
		    m_lpRGB[i].rgbReserved = 0;
		}
	    if (i<m_wColors)
		{
		    for(i=0; i<m_wColors; i++)
			{
			    m_lpRGB[i].rgbRed =
				m_lpRGB[i].rgbGreen =
				m_lpRGB[i].rgbBlue = (BYTE)i;
			}
		}
	}
    // otherwise map to the header color map directly
    else if (m_wColors > 0)
	{
	    for(i=0; i<m_wColors; i++) 
		{
		    m_lpRGB[i].rgbRed = m_PcxHdr.Palette[i*3];
		    m_lpRGB[i].rgbGreen = m_PcxHdr.Palette[i*3 + 1];
		    m_lpRGB[i].rgbBlue = m_PcxHdr.Palette[i*3 + 2];
		}
		// extra check for monochrome bitmap where colormap
		// is not filled by some pcx files.. 
		if (m_wColors==2 
			&& m_lpRGB[0].rgbRed==0
			&& m_lpRGB[0].rgbGreen==0
			&& m_lpRGB[0].rgbBlue == 0
			&& m_lpRGB[1].rgbRed == 0
			&& m_lpRGB[1].rgbGreen == 0
			&& m_lpRGB[1].rgbBlue == 0 
			)
		{
			m_lpRGB[1].rgbRed =
			m_lpRGB[1].rgbGreen =
			m_lpRGB[1].rgbBlue = 0xff;
		}
	}
}


void SECPcx::WriteColorMap()
{
    WORD i;
    BYTE cTmp;
	WORD wColorsNeeded = (WORD)(1 << m_PcxHdr.cBitsPerPixel);
    
    // Load in colormap
	if (wColorsNeeded > 32)
	{
	    cTmp=PCX_COLORMAP;
	    PutCache(&cTmp, 1);
	    for (i=0; i<m_wColors; i++)
		{
		    PutCache(&(m_lpRGB[i].rgbRed),1);
		    PutCache(&(m_lpRGB[i].rgbGreen),1);
		    PutCache(&(m_lpRGB[i].rgbBlue),1);
		}

		//Fill out remaining colors needed
		for (;i< 3*wColorsNeeded; i++)
		{
			PutCache(&i, 1);
		}
	}
    // Otherwise, the data has already been stored in the PcxHdr.Palette array
}

void SECPcx::LoadImageBits()
{
	ASSERT (m_lpSrcBits != 0);
    BYTE cTmp, cByteCount, cPlane=0, cMask = 1;
    WORD wPad, wByteCount=0, wRow=0;
#ifdef WIN32
    LPBYTE lpImageData, lpOldImageData;
#else
    BYTE huge * lpImageData;
    BYTE huge * lpOldImageData;
#endif
    int i,j;
    
    
    // Calculate the number of "pad" bytes at the end of each scan line
    // that's part of the PCX format     
    //int nPad = ((m_PcxHdr.wBytesPerLine * (WORD)m_PcxHdr.cNumBitPlanes) * (WORD)(8/m_PcxHdr.cBitsPerPixel)) - ((m_PcxHdr.wXEnd - m_PcxHdr.wXStart) + 1);

    if (m_PcxHdr.cBitsPerPixel == 1)
	wPad = (WORD)(m_PcxHdr.wBytesPerLine*8 - m_dwWidth);
    else
	wPad = (WORD)(m_PcxHdr.wBytesPerLine - (WORD)m_dwWidth);
	BYTE offset = m_PcxHdr.cNumBitPlanes;
	BOOL bEGA = FALSE;
	// MSS 1/15/99 include 2 bit plane images in bEGA processing
	if( (m_PcxHdr.cNumBitPlanes<=4 && m_PcxHdr.cNumBitPlanes>=2) && m_PcxHdr.cBitsPerPixel == 1)
	{
		bEGA = TRUE;
		offset = 1;
	}

	lpImageData = lpOldImageData = m_lpSrcBits + (offset-1);
    
    
    // Run through the cache until you've read all the image data, or
    // you hit an error condition
    while((GetCache(&cTmp, 1)) == 1)	
	{
	    // Check for repeater code 
	    if ((cTmp&0xC0) == 0xC0)
		{
		    // find out number of repeater bytes
		    cByteCount = (BYTE)(cTmp & 0x3f);
		    // read in the data byte to be repeated
		    if ((GetCache(&cTmp, 1)) != 1)	
			return;           
		}
	    else
		// otherwise you just "repeat" once
		cByteCount = 1;
	    
	    // Cycle through repeater times and fill in the data array
	    for (i=0; i<cByteCount; i++)
		{   
		    // Special case for 1 BitsPerPixel ; take mask into consideration
		    if (m_PcxHdr.cBitsPerPixel == 1)
			{
				if (bEGA)
				{
					for (j=7; j>=0; j--)
					{
						if (cTmp&(0x01<<j))
							*lpImageData++ |= 0x01<<(cPlane);
						else
							*lpImageData++;
					}
				}
				else
				{
					for (j=7; j>=0; j--)  
						*lpImageData++ = (UCHAR)(((cTmp)&(0x01<<j)?(BYTE)(m_wColors-1):0x00));
				}

			
			}
		    // otherwise just do a straightwise copy
		    else 
			{
			    *lpImageData = (BYTE)cTmp;
				lpImageData += offset;
			    // lpImageData += sizeof(BYTE);
			}
		    wByteCount++;
		    
		    // Check if a single row's bytes have all been read
		    if (wByteCount == m_PcxHdr.wBytesPerLine)
			{
			    wByteCount = 0;
			    cPlane++;
			    
				// Need to determine whether format is saved in "plane"
				// format; with scan line data stored by individual
				// color (ie, all "red" bitmap data is stored contiguously,
				// as is all "green" and "blue" data).
			    if (cPlane >= m_PcxHdr.cNumBitPlanes) 
				{
				    cPlane = 0;
					// decrement data array pointer by pad amount
				    lpImageData -= wPad;
					lpImageData += (offset-1);
				    lpOldImageData = lpImageData;
				    wRow++;
				    if ((DWORD)wRow >= m_dwHeight)
					return;
				}
			    else
				{
					if (bEGA)
						lpImageData = lpOldImageData;
					else
						lpImageData = lpOldImageData-cPlane;
				}
			    cMask = (BYTE)(1 << cPlane);
			}
		}
	} 
    
}

void SECPcx::WriteImageBits()
{
	ASSERT_VALID(this);

    BYTE cData, cDataNext, cCounter;
    WORD wPad, wRow=0, wCol=0;
#ifdef WIN32
    LPBYTE lpImageData;
#else
    BYTE huge * lpImageData;
#endif
    
    lpImageData = m_lpSrcBits;


    // Calculate the number of "pad" bytes at the end of each scan line
    // that's part of the PCX format     
    //int nPad = ((m_PcxHdr.wBytesPerLine * (WORD)m_PcxHdr.cNumBitPlanes) *
	//	(WORD)(8/m_PcxHdr.cBitsPerPixel)) -
	//	((m_PcxHdr.wXEnd - m_PcxHdr.wXStart) + 1);

    if (m_PcxHdr.cBitsPerPixel == 1)
	wPad = (WORD)(m_PcxHdr.wBytesPerLine*8 - m_dwWidth);
    else
	wPad = (WORD)(m_PcxHdr.wBytesPerLine - (WORD)m_dwWidth);  
    
	m_nBitHolder = 0;	 
	NEXT_IMAGE_PIXEL(cData, lpImageData);
	cDataNext = cData;


    for (wRow=0; (DWORD)wRow<m_dwHeight  ; wRow++)
	{
	    // Always reset byte counter for beginning of line
	    // cCounter = 1;
	    cCounter = 0;
	    // for (wCol=0; (DWORD)wCol<m_dwWidth; wCol++)
		for (wCol=0; (DWORD)wCol<m_dwWidth; wCol++)
		{
		    cCounter++;
		    
			if ((DWORD)(wCol+1)<m_dwWidth || (DWORD)(wRow+1)<m_dwHeight)
				NEXT_IMAGE_PIXEL(cDataNext, lpImageData);
		    // Write out the data if run length has ended, or you've
		    // reached the 63 RLE limit for this implementation
		    if (cDataNext != cData || cCounter==63)
			{
			    cCounter |= 0xc0;
			    PutCache(&cCounter, 1);
			    PutCache(&cData, 1);
			    cCounter = 0; 
			    cData = cDataNext;
			}  
		}
	    // At end of any row, must write out current byte along
	    // with pad bytes 
	    if (cCounter)
		{
		    cCounter |= 0xc0;
		    PutCache(&cCounter, 1);
		    PutCache(&cData, 1);
		}
	} // end of col
}

BOOL SECPcx::DoSaveImage(CFile* pFile)
{
	ASSERT_VALID(this);

    // First make sure that image can be saved in valid PCX
    // format (ie, 256 colors or less)
    if (m_nSrcBitsPerPixel>8)
		return FALSE;
    
    //convert from intermediate format to unflipped/unpadded state	
    FlipVert();   
    UnPadBits();

   
    // Fill out PcxHdr info
    m_PcxHdr.cIdentifier = 0x0a;
    m_PcxHdr.cVersion = 4;// 5
    m_PcxHdr.cEncoding = 1;	// default RLE encoding
	switch (m_nSrcBitsPerPixel) {
		case 1:
		case 4:
			//AAB m_PcxHdr.cBitsPerPixel = (BYTE)m_nSrcBitsPerPixel;
			m_PcxHdr.cBitsPerPixel = 8; //round up to 256 color format
			break;
		default:
			m_PcxHdr.cBitsPerPixel = (BYTE)m_nSrcBitsPerPixel;
			break;
	}

    // dimensions are stored in pixel format always from origin 0,0
    m_PcxHdr.wXStart	= 0;
    m_PcxHdr.wYStart = 0;
    m_PcxHdr.wXEnd = (WORD)(m_dwWidth-1);
    m_PcxHdr.wYEnd = (WORD)(m_dwHeight-1);
    m_PcxHdr.wHorzRes = 640;
    m_PcxHdr.wVertRes = 480;
    m_PcxHdr.Reserved1 = 0;
    m_PcxHdr.cNumBitPlanes = 1; // we don't separate r,g,b components
    m_PcxHdr.wBytesPerLine = (WORD)m_dwWidth; // AAB: Check for 1bpp image
    if (m_PcxHdr.cBitsPerPixel==1)
	m_PcxHdr.wPaletteType = 1;  // monochrome indicator
    else
	m_PcxHdr.wPaletteType = 2;  // indicates you can use gray scale

    m_PcxHdr.wHorzScreenSize=0;
    m_PcxHdr.wVertScreenSize=0;
    
	// Fill out PcxHdr Palette info if < 16 colors
	if (m_wColors < 16)
	{
	    for(WORD i=0; i<m_wColors; i++) 
		{
		    m_PcxHdr.Palette[i*3] = m_lpRGB[i].rgbRed;
		    m_PcxHdr.Palette[i*3 + 1] = m_lpRGB[i].rgbGreen;
		    m_PcxHdr.Palette[i*3 + 2] = m_lpRGB[i].rgbBlue;
		}
	}

	InitCache(SEC_PCX_CACHE_SIZE, SEC_WRITE_CACHE);

    pFile->Write(&m_PcxHdr, sizeof(PCXHDR));
    WriteImageBits();
    WriteColorMap();
    FlushCache();
    FreeCache();
  
    // restore to intermediate format
    PadBits();
    FlipVert();
    
    return TRUE;
}


void SECPcx::Serialize(CArchive& ar)
{
    SECImage::Serialize(ar);
}


#ifdef _DEBUG
void SECPcx::AssertValid() const
{
    CObject::AssertValid();
	SECImage::AssertValid();
}
#endif /* _DEBUG */


