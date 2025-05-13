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
//  Description:	SECTiff Implementation file
//  Created:		Tue Aug 29 15:18:22 1995
//
//	ChangeLog
//	11/10/95 AAB : Removed globals, combined all into SECTiff class
//	01/03/96 AAB : Fixed global free of uninit'ed mem
//
// This is a part of the Stingray Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All rights reserved.
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __SECTIFF_H__
#include "..\..\Include\Image\sectiff.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

IMPLEMENT_SERIAL(SECTiff, CObject, 1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW






//@doc SECTiff
//@mfunc Constructs a SECTiff object.
//@xref <c SECTiff>
SECTiff::SECTiff()
{
    m_nStopOnErr = 0;
    m_lpMap = NULL;
    m_nCompression = COMPRESSION_NONE;
	lppPALmap = NULL;
	lppBWmap = NULL;
}


SECTiff::~SECTiff()
{
    // AAB nothing for now
}

BOOL SECTiff::DoLoadImage(CFile* pFile)
{
    // Since TIFF uses a special file type,
    // we can't use this function.
    pFile; // UNUSED
    return FALSE;
}

BOOL SECTiff::LoadImage(LPCTSTR lpszFileName)
{
    ASSERT(lpszFileName != 0);

    if (!PreLoadImage())
	return FALSE;

    m_lpTif = TIFFOpen(lpszFileName, _T("r"));
    if (!m_lpTif) 
	{
		TRACE(_T("SECTiff::SECLoadImage() failed to open %s\n"), lpszFileName);
		return FALSE;
	}
    
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_ORIENTATION, &m_wOrientation);
    switch (m_wOrientation) 
	{
	case ORIENTATION_TOPLEFT:
	case ORIENTATION_TOPRIGHT:
	case ORIENTATION_LEFTTOP:
	case ORIENTATION_RIGHTTOP:
	    {
		m_wOrientation = ORIENTATION_BOTLEFT;
		break;
	    }
	case ORIENTATION_BOTRIGHT:
	case ORIENTATION_BOTLEFT:
	case ORIENTATION_RIGHTBOT:
	case ORIENTATION_LEFTBOT:
	    {
			m_wOrientation = ORIENTATION_TOPLEFT;
			break;
		    }
	}
    
    TIFFSetField(m_lpTif, TIFFTAG_ORIENTATION, m_wOrientation);
    TIFFGetField(m_lpTif, TIFFTAG_IMAGEWIDTH, &m_dwWidth);
    TIFFGetField(m_lpTif, TIFFTAG_IMAGELENGTH, &m_dwHeight);
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_BITSPERSAMPLE, &m_wBPS);
    TIFFGetField(m_lpTif, TIFFTAG_PHOTOMETRIC, &m_wPhoto);
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_SAMPLESPERPIXEL, &m_wSPP);
    m_nSrcBitsPerPixel = m_wBPS*m_wSPP;
	// Fix for monochrome images until we store mono images
	// in true one bit per pixel
	/*AAB
	if (m_nSrcBitsPerPixel==1)
		m_nSrcBitsPerPixel = 8;
	*/
    switch(m_nSrcBitsPerPixel) {
    case 1:
    case 2:
    case 4:
    case 8:
    default:
	m_lpBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER)+
					       256*sizeof(RGBQUAD));
	if (m_lpBMI == NULL)
	{
		TRACE(_T("SECTiff::SECLoadImage could not allocate m_lpBMI\n"));
	    return NULL;
	}
	m_lpRGB = (LPRGBQUAD)((LPBYTE)m_lpBMI + sizeof(BITMAPINFOHEADER));
	
	break;
	//AAB Don't support 16 bit images (yet)! 
	case 16:
		TIFFClose(m_lpTif);
		return FALSE;
	break;

    case 24:
	m_lpRGB = 0;
	m_lpBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER));
	if (m_lpBMI == NULL)
	{
		TRACE(_T("SECTiff::SECLoadImage could not allocate m_lpBMI for 24 bit\n"));
	    return NULL;
	}
	break;
    }
    
    if (m_wSPP == 1)
	loadPalette();
    else
	loadColors();
    
    
    loadImageData();
    TIFFClose(m_lpTif);
    
    // Now fill out the BITMAPINFOHEADER info...
    m_lpBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_lpBMI->bmiHeader.biWidth = m_dwWidth;
#ifdef WIN32FLIPIMAGE
    // Store negative height so that rendering DIB APIs will "flip" the
    // image automatically
    m_lpBMI->bmiHeader.biHeight = (long)(m_dwHeight) * -1L;   
#else
    // Regular WIN16 still has to have its bits flipped manually (luckily
    // we can just use our member fxn 'FlipVert')
    m_lpBMI->bmiHeader.biHeight = m_dwHeight;
#endif
    m_lpBMI->bmiHeader.biPlanes = (WORD)m_nBitPlanes;
    m_lpBMI->bmiHeader.biBitCount =  (WORD)m_nSrcBitsPerPixel;
    m_lpBMI->bmiHeader.biCompression = BI_RGB;
    m_lpBMI->bmiHeader.biSizeImage = m_dwPadWidth *  m_dwHeight;
    
    // Arrange the image data so that it is compatible with our intermediate DIB 
    // format
    FlipVert();
	//AAB ANOTHER TEST
	// m_bIsPadded = TRUE;

    if (!PostLoadImage())
	return FALSE;

    return (TRUE);
}


#define F_FULLCOLOR 1
#define F_BWDITHER 2
#define F_GREYSCALE 3

BOOL SECTiff::loadPalette()
{
    switch(m_wPhoto)
	{
	case PHOTOMETRIC_PALETTE:
	    m_wColType = F_FULLCOLOR;
	    break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
	    m_wColType = (WORD)((m_wBPS==1)?F_BWDITHER:F_GREYSCALE);
	    break;
	default:
	    m_wColType = 0;
	    break;
	}
    //AAB m_dwPadWidth = PADWIDTH(m_dwWidth);
	m_dwPadWidth = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);
    m_lpSrcBits = (LPBYTE)GlobalAllocPtr(GHND, m_dwPadWidth * m_dwHeight);
    if (!m_lpSrcBits)
	{
		TRACE(_T("SECTiff::loadPalette could not allocate m_lpSrcBits\n"));
		return (FALSE); 
	}
    return(TRUE);
}

BOOL SECTiff::loadColors()
{
    m_wColType = F_FULLCOLOR;
	m_dwPadWidth = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);
    m_lpSrcBits = (LPBITS)GlobalAllocPtr(GHND, m_dwPadWidth*m_dwHeight);		
    if (!m_lpSrcBits)
	{
		TRACE(_T("SECTiff::loadColors could not allocate m_lpSrcBits\n"));
		return(FALSE);
	}
    
    return (TRUE);
}

WORD SECTiff::loadImageData()
{
    WORD wSPS, wBPS, wPhoto;
    
    // Make sure we support the format..
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_BITSPERSAMPLE, &wBPS);
    switch(wBPS)
	{
	case 1:
	case 2:
	case 4:
	case 8:
	case 16:
	    break;
	default:
	    return(0);	// error message in future for unsupported format
	}

    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_SAMPLESPERPIXEL, &wSPS);
    switch (wSPS) 
	{
	case 1:
		case 3:
	case 4:
	    break;
	default:
	    return(0); // future error for unsupported format
	}
    
    if (!TIFFGetField(m_lpTif, TIFFTAG_PHOTOMETRIC, &wPhoto))
	{
	    switch(wSPS)
		{
		case 1: wPhoto = PHOTOMETRIC_MINISBLACK;
		    break;
		case 3:
		case 4:
		    wPhoto = PHOTOMETRIC_RGB;
		    break;
		default:
		    return 0; //future error msg
		}
	}
    
    
    WORD ok = gt();
    if (lppBWmap)
	GlobalFreePtr(lppBWmap);
	lppBWmap = NULL;
    if (lppPALmap)
	GlobalFreePtr(lppPALmap);
	lppPALmap = NULL;
    return ok;
}


WORD SECTiff::checkcmap(int n, LPWORD r, LPWORD g, LPWORD b)
{
    while (n-- >= 0)
	if ((*r++ >= 256) || (*g++ >= 256) || (*b++ >= 256))
	    return 16;
    return(8);
}


WORD SECTiff::gt()
{
    // LPBYTE lpMap = NULL;
    WORD minsamplevalue, maxsamplevalue, planarconfig;
    LPWORD redcmap=NULL, bluecmap=NULL, greencmap=NULL;
    
    int  e;
    int x, range=0;
    
    m_wBPP = 1;
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_MINSAMPLEVALUE, &minsamplevalue);
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_MAXSAMPLEVALUE, &maxsamplevalue);
    
    switch(m_wPhoto) {
    case PHOTOMETRIC_YCBCR:
	TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_YCBCRCOEFFICIENTS, &m_lpYCbCrCoeffs);
	TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_YCBCRSUBSAMPLING, &m_wYCbCrHorizSampling,
			      &m_wYCbCrVertSampling);
	wYCbCrHorizSampling = m_wYCbCrHorizSampling;
	wYCbCrVertSampling = m_wYCbCrVertSampling;
	TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_REFERENCEBLACKWHITE, &lpRefBlackWhite);
	SECinitYCbCrConversion();
    case PHOTOMETRIC_RGB:
	m_wBPP*=3;
	if (minsamplevalue==0 && maxsamplevalue == 255)
	    break;
    case PHOTOMETRIC_MINISBLACK:
    case PHOTOMETRIC_MINISWHITE:
	range = maxsamplevalue - minsamplevalue;
	m_lpMap = (LPBYTE)GlobalAllocPtr(GHND, (range+1)*sizeof(BYTE));
	if (m_lpMap == NULL)
	{
		TRACE(_T("SECTiff::gt could not allocate m_lpMap with %d colors\n"),range+1);
	    // error message for bad photometric conversion table
	    return 0; 
	}
	if (m_wPhoto == PHOTOMETRIC_MINISWHITE) 
	    {
		for(x=0; x<=range; x++)
		    {
			m_lpRGB[x].rgbRed =
			    m_lpRGB[x].rgbGreen =
			    m_lpRGB[x].rgbBlue =
			    m_lpMap[x] = (BYTE)((255*(range-x))/range);
		    }
	    }
	else
	    {
		for (x=0; x<=range; x++)
		    m_lpRGB[x].rgbRed =
			m_lpRGB[x].rgbGreen =
			m_lpRGB[x].rgbBlue =
			m_lpMap[x] = (BYTE)((255*x)/range);
	    }
	if (m_wPhoto != PHOTOMETRIC_RGB && m_wBPS <=8)
	    {
		// use photometric mapping table to construct unpacking
		// tables for samples <= 8 bits..
		if (!makebwmap())
		    return (0);
		if (m_lpMap)
		    GlobalFreePtr(m_lpMap);
		m_lpMap=NULL;
	    }
	range++; //HOD CHECK
	break;
    case PHOTOMETRIC_PALETTE:
	if(!TIFFGetField(m_lpTif, TIFFTAG_COLORMAP, &redcmap, &greencmap, &bluecmap))
	    return (0);
	range = (1<<m_wBPS)-1;
	if (checkcmap(range, redcmap, greencmap, bluecmap) == 16) 
	    { 
//AAB: Typecast to long for full precision under WIN16
#define CVT(x)	((((long)x)*(long)255)/((1L<<16)-1))
		for(x=range; x>=0; x--) 
		    { 
			m_lpRGB[x].rgbRed = (BYTE)CVT(redcmap[x]);
			m_lpRGB[x].rgbGreen = (BYTE)CVT(greencmap[x]);
			m_lpRGB[x].rgbBlue = (BYTE)CVT(bluecmap[x]);
		    }
	    }
	else
	    {
		for(x=range; x>=0; x--)
		    {
			m_lpRGB[x].rgbRed = (BYTE)(redcmap[x]);
			m_lpRGB[x].rgbGreen = (BYTE)(greencmap[x]);
			m_lpRGB[x].rgbBlue = (BYTE)(bluecmap[x]);
		    }
	    }
	if (m_wBPS <= 8) 
	    {
		if (!makecmap())
		    return (0);
	    }
	break;
    default:
	//error for unknown photometric tag
	return (0);
    }
    m_wColors = (WORD)range; 
    
    TIFFGetField(m_lpTif, TIFFTAG_PLANARCONFIG, &planarconfig);
    if (planarconfig == PLANARCONFIG_SEPARATE && m_wSPP > 1) 
	{
	    e = TIFFIsTiled(m_lpTif)? gtTileSeparate():gtStripContig();
	}
    else 
	{
	    e = TIFFIsTiled(m_lpTif)? gtTileContig() : gtStripContig();
	}
    if (m_lpMap)
	GlobalFreePtr(m_lpMap);
    return ((WORD)e);    
}

DWORD SECTiff::setorientation(DWORD h)
{
    DWORD y;
    
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_ORIENTATION, &m_wOrientation);
    switch (m_wOrientation) {
    case ORIENTATION_BOTRIGHT:
    case ORIENTATION_RIGHTBOT:
    case ORIENTATION_LEFTBOT:
	m_wOrientation = ORIENTATION_BOTLEFT;
	// fall thru
    case ORIENTATION_BOTLEFT:
	y = 0;
	break;
    case ORIENTATION_TOPRIGHT:
    case ORIENTATION_RIGHTTOP:
    case ORIENTATION_LEFTTOP:
    default:
	m_wOrientation = ORIENTATION_TOPLEFT;
    case ORIENTATION_TOPLEFT:
	y = h-1;
	break;
    }
    return (y);
}



int SECTiff::gtTileContig()
{
    DWORD col, row, y;
    DWORD tw, th;
    LPBYTE lpBuf;
    int fromskew, toskew;
    DWORD nrow;

    LPBITS raster = m_lpSrcBits;
    m_nStopOnErr = 0;
    
    m_fpContigRoutine = SECpickTileContigCase(m_lpMap);
    if (m_fpContigRoutine == 0) 
	return (0);
    lpBuf = (LPBYTE)GlobalAllocPtr(GHND, TIFFTileSize(m_lpTif));
    if (!lpBuf)
	{
	    // error for mem allocation of tile buffer
	    return (0);
	}
    
	// DWORD dwPad = CalcBytesPerLine(m_nSrcBitsPerPixel, m_dwWidth);
	DWORD dwLastByte = LastByte(m_nSrcBitsPerPixel, m_dwWidth);
    TIFFGetField(m_lpTif, TIFFTAG_TILEWIDTH, &tw);
    TIFFGetField(m_lpTif, TIFFTAG_TILELENGTH, &th);
    y = setorientation(m_dwHeight);
    toskew = (m_wOrientation==ORIENTATION_TOPLEFT? (int)(-1*(tw+m_dwWidth)) : (int)(-1*tw + m_dwWidth));
    
    for (row=0; row<m_dwHeight; row+=th) {
	nrow = (row+th > m_dwHeight ? m_dwHeight-row : th);
	for (col=0; col<m_dwWidth; col+=tw)
	    {
		if (TIFFReadTile(m_lpTif, lpBuf, col, row, 0, 0) < 0 && m_nStopOnErr)
		    break;
		if (col+tw > m_dwWidth) 
		    {
			DWORD npix = m_dwWidth - col;
			fromskew = (int)(tw - npix);
			(this->*m_fpContigRoutine)((LPBITS)(raster + (y*dwLastByte + LastByte(m_nSrcBitsPerPixel, col))), (LPBYTE)lpBuf, (LPBYTE)m_lpMap, (DWORD)npix, (DWORD)nrow, 
			       (int)fromskew, (int)((toskew + fromskew)*m_wBPP));
		    }
		else
		    (this->*m_fpContigRoutine)(raster + (y*dwLastByte + LastByte(m_nSrcBitsPerPixel, col)), lpBuf, m_lpMap, tw, nrow, 0, toskew*m_wBPP);
	    }
	y += (m_wOrientation == ORIENTATION_TOPLEFT ? -1*nrow : nrow);
    }
    if (lpBuf)
	GlobalFreePtr(lpBuf);
    return(1);
}



int SECTiff::gtTileSeparate()
{
    DWORD col, row, y;
    DWORD tw, th;
    LPBYTE lpBuf=NULL;
    LPBYTE r, g, b;
    int tilesize;
    int fromskew, toskew;
    DWORD nrow;
    LPBITS raster = m_lpSrcBits;
    
    m_fpTileSeparate = SECpickTileSeparateCase(m_lpMap);
    if (m_fpTileSeparate == 0)
	{
		TRACE(_T("SECTiff::gtTileSeparate pickTileSeparateCase returns a 0\n"));
		return (0); 
	}
    tilesize = (int)TIFFTileSize(m_lpTif);
    lpBuf = (LPBYTE)GlobalAllocPtr(GHND, 3*tilesize);
    if (lpBuf==0)
	{
		TRACE(_T("SECTiff::gtTileSeparate could not allocate lpBuf of size %d\n"), 3*tilesize);
		return 0;
	}
    
    r = lpBuf;
    g = r + tilesize;
    b = g + tilesize;
    TIFFGetField(m_lpTif, TIFFTAG_TILEWIDTH, &tw);
    TIFFGetField(m_lpTif, TIFFTAG_TILELENGTH, &th);
    y = setorientation(m_dwHeight);
    toskew = (m_wOrientation == ORIENTATION_TOPLEFT? (int)(-1*(tw + m_dwWidth)) : (int)(-1*(tw) + m_dwWidth));
    for (row=0; row<m_dwHeight; row+=th)
	{
	    nrow = (row+th > m_dwHeight ? m_dwHeight-row : th);
	    for (col = 0; col<m_dwWidth; col += tw)
		{
		    if (TIFFReadTile(m_lpTif, r, col, row, 0, 0) < 0 && m_nStopOnErr)
			break;
		    if (TIFFReadTile(m_lpTif, g, col, row, 0, 1) < 0 && m_nStopOnErr)
			break;
		    if (TIFFReadTile(m_lpTif, b, col, row, 0, 2) < 0 && m_nStopOnErr)
			break;
		    if (col + tw > m_dwWidth )
			{
			    DWORD npix = m_dwWidth - col;
			    fromskew = (int)(tw - npix);
			    (this->*m_fpTileSeparate)(raster + (y*m_dwWidth + col)*m_wBPP, r,g,b,m_lpMap, npix, nrow,
				   fromskew, (toskew+fromskew)*m_wBPP);
			}
		    else
			(this->*m_fpTileSeparate)(raster + (y*m_dwWidth + col)*m_wBPP, r,g,b, m_lpMap,
			       tw, nrow, 0, toskew*m_wBPP);
		}
	    y += (m_wOrientation == ORIENTATION_TOPLEFT ? -1*nrow : nrow);
	}
    if (lpBuf)
	GlobalFreePtr(lpBuf);
    return (1);
}

DWORD CalcBytesPerLine2(WORD dwBitsPerPixel, DWORD dwWidth) {
    DWORD dwBits, dwPadBits;

    dwBits    = dwBitsPerPixel * dwWidth;

    if ((dwBits % 32) == 0)
        return (dwBits/8);  // already DWORD aligned, no padding needed
    else
        dwPadBits = 32 - (dwBits % 32);

    return (dwBits/8 + dwPadBits/8 + (((dwPadBits % 8) > 0) ? 1 : 0));
}

int SECTiff::gtStripContig()
{
    DWORD row, y, nrow;
    LPBYTE lpBuf;
    DWORD rowsperstrip;
    DWORD imagewidth;
    int scanline;
    int fromskew, toskew;
    LPBITS raster = m_lpSrcBits;
    
    m_fpContigRoutine = SECpickTileContigCase(m_lpMap);
    if (m_fpContigRoutine == 0)
	return (0);
    lpBuf = (LPBYTE)GlobalAllocPtr(GHND, TIFFStripSize(m_lpTif));
    if (lpBuf == 0)
	return (0);
    
    
    y = setorientation(m_dwHeight);
    toskew = (m_wOrientation == ORIENTATION_TOPLEFT? (int)(-2*m_dwWidth) : 0);
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    TIFFGetField(m_lpTif, TIFFTAG_IMAGEWIDTH, &imagewidth);
    scanline = (int)TIFFScanlineSize(m_lpTif);
    fromskew = (m_dwWidth<imagewidth?(int)(imagewidth - m_dwWidth) : 0);
	DWORD dwLastByte = LastByte(m_nSrcBitsPerPixel, m_dwWidth);

    for (row = 0; row < m_dwHeight; row += rowsperstrip)
	{
	    nrow = (row + rowsperstrip > m_dwHeight ? m_dwHeight - row : rowsperstrip);
	    if (TIFFReadEncodedStrip(m_lpTif, TIFFComputeStrip(m_lpTif, row, 0),
				     lpBuf, nrow*scanline) < 0 && m_nStopOnErr)
		break;
	    (this->*m_fpContigRoutine)(raster + y*dwLastByte, lpBuf, m_lpMap, m_dwWidth, nrow, fromskew, toskew*m_wBPP);

	    y += (m_wOrientation == ORIENTATION_TOPLEFT ? -1*nrow : nrow);
	}
    if (lpBuf)
	GlobalFreePtr(lpBuf);
    return (1);
}


int SECTiff::gtStripSeparate()
{
    LPBYTE lpBuf;
    LPBYTE r, g, b;
    DWORD row, y, nrow;
    int scanline;
    DWORD rowsperstrip;
    DWORD imagewidth;
    DWORD stripsize;
    int fromskew, toskew;
    LPBITS raster = m_lpSrcBits;
    
    stripsize = TIFFStripSize(m_lpTif);
    r = lpBuf = (LPBYTE)GlobalAllocPtr(GHND, 3*stripsize);
    if (lpBuf == 0)
	{
		TRACE(_T("SECTiff::gtStripSeparate could not allocate stripsize of %d..\n"), 3*stripsize);
		return 0;
	}
    g = r + stripsize;
    b = g + stripsize;
    m_fpTileSeparate = SECpickTileSeparateCase(m_lpMap);
    if (m_fpTileSeparate == 0) 
	{
		TRACE(_T("SECTiff::gtStripSeparate pickTileSeparateCase returns a 0\n"));
		return 0;
	}
    y = setorientation (m_dwHeight);
    toskew = (m_wOrientation == ORIENTATION_TOPLEFT ? (int)(-2*m_dwWidth) : 0);
    TIFFGetFieldDefaulted(m_lpTif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
    TIFFGetField(m_lpTif, TIFFTAG_IMAGEWIDTH, &imagewidth);
    scanline = (int)TIFFScanlineSize(m_lpTif);
    fromskew = (m_dwWidth<imagewidth ? (int)(imagewidth -1*m_dwWidth) : 0);
    for (row=0; row<m_dwHeight; row+= rowsperstrip)
	{
	    nrow = (row + rowsperstrip > m_dwHeight ? m_dwHeight-row : rowsperstrip);
	    if (TIFFReadEncodedStrip(m_lpTif, TIFFComputeStrip (m_lpTif, row, 0), r, nrow*scanline) < 0 && m_nStopOnErr)
		break;
	    if (TIFFReadEncodedStrip(m_lpTif, TIFFComputeStrip (m_lpTif, row, 1), g, nrow*scanline) < 0 && m_nStopOnErr)
		break;
	    if (TIFFReadEncodedStrip(m_lpTif, TIFFComputeStrip (m_lpTif, row, 2), b, nrow*scanline) < 0 && m_nStopOnErr)
		break;
	    (this->*m_fpTileSeparate)(raster + y*m_dwWidth*m_wBPP, r, g, b, m_lpMap, m_dwWidth, nrow, fromskew, toskew*m_wBPP);
	    y += (m_wOrientation == ORIENTATION_TOPLEFT ? -1*nrow : nrow);
	}
    if (lpBuf)
	GlobalFreePtr(lpBuf);
    return (1);
}

/*
 * Greyscale images with less than 8 bits/sample are handled
 * with a table to avoid lots of shifts and masks.  The table
 * is setup so that put*bwtile (below) can retrieve 8/bitspersample
 * pixel values simply by indexing into the table with one
 * number.
 */
int SECTiff::makebwmap()
{
    int i;
    int nsamples = 8 / m_wBPS;
    LPBYTE p;
    
    lppBWmap = (LPPBYTE)GlobalAllocPtr(GHND,
				       256*sizeof (BYTE *)+(256*nsamples));
    if (lppBWmap == NULL) 
    {
		TRACE(_T("SECTiff::makebwmap could not allocate colormap of size %d\n"), 256*sizeof(BYTE *)*nsamples);
		return (0);
    }
    p = (LPBYTE)(lppBWmap + 256);
    for (i = 0; i < 256; i++) {
	lppBWmap[i] = p;
	switch (m_wBPS) {
#define	GREY(x)	*p++ = (BYTE)(x);
	case 1:
	    GREY(i>>7);
	    GREY((i>>6)&1);
	    GREY((i>>5)&1);
	    GREY((i>>4)&1);
	    GREY((i>>3)&1);
	    GREY((i>>2)&1);
	    GREY((i>>1)&1);
	    GREY(i&1);
	    break;
	case 2:
	    GREY(i>>6);
	    GREY((i>>4)&3);
	    GREY((i>>2)&3);
	    GREY(i&3);
	    break;
	case 4:
	    GREY(i>>4);
	    GREY(i&0xf);
	    break;
	case 8:
	    GREY(i);
	    break;
	}
#undef	GREY
    }
    return (1);
}

/*
 * Palette images with <= 8 bits/sample are handled
 * with a table to avoid lots of shifts and masks.  The table
 * is setup so that put*cmaptile (below) can retrieve 8/bitspersample
 * pixel values simply by indexing into the table with one
 * number.
 */
int SECTiff::makecmap()
{
    int i;
    int nsamples = 8 / m_wBPS;
    LPBYTE p;
    
    lppPALmap = (LPPBYTE)GlobalAllocPtr( GHND,
					 256*sizeof (LPBYTE)+(256*nsamples));
    if (lppPALmap == NULL) 
    {
		TRACE(_T("SECTiff::makecmap could not allocate colormap for nsamples %d\n"), nsamples);
		return (0);  
    }
    p = (LPBYTE)(lppPALmap + 256);
    for (i = 0; i < 256; i++) {
	lppPALmap[i] = p;
#define	CMAP(x)	*p++ = (BYTE)(x);
	switch (m_wBPS) {
	case 1:
	    CMAP(i>>7);
	    CMAP((i>>6)&1);
	    CMAP((i>>5)&1);
	    CMAP((i>>4)&1);
	    CMAP((i>>3)&1);
	    CMAP((i>>2)&1);
	    CMAP((i>>1)&1);
	    CMAP(i&1);
	    break;
	case 2:
	    CMAP(i>>6);
	    CMAP((i>>4)&3);
	    CMAP((i>>2)&3);
	    CMAP(i&3);
	    break;
	case 4:
	    CMAP(i>>4);
	    CMAP(i&0xf);
	    break;
	case 8:
	    CMAP(i);
	    break;
		}
#undef CMAP
    }
    return (1);
}


/*
 * The following routines move decoded data returned
 * from the TIFF library into rasters filled with packed
 * ABGR pixels (i.e. suitable for passing to lrecwrite.)
 *
 * The routines have been created according to the most
 * important cases and optimized.  pickTileContigCase and
 * pickTileSeparateCase analyze the parameters and select
 * the appropriate "put" routine to use.
 */
#define	REPEAT8(op)	REPEAT4(op); REPEAT4(op)
#define	REPEAT4(op)	REPEAT2(op); REPEAT2(op)
#define	REPEAT2(op)	(op); (op)
#define	CASE8(x,op)				\
switch (x) {				\
 case 7: (op); case 6: (op); case 5: (op);	\
 case 4: (op); case 3: (op); case 2: (op);	\
 case 1: (op);				\
				    }
#define	CASE4(x,op)	switch (x) { case 3: op; case 2: op; case 1: op; }

#define	UNROLL8(w, op1, op2) {		\
					DWORD x;		\
					for (x = w; x >= 8; x -= 8) {	\
									(op1);			\
									REPEAT8((op2));		\
								    }				\
					if (x > 0) {			\
									op1;			\
									CASE8(x,(op2));		\
								    }				\
				    }
#define	UNROLL4(w, op1, op2) {		\
					DWORD x;		\
					for (x = w; x >= 4; x -= 4) {	\
									op1;			\
									REPEAT4(op2);		\
								    }				\
					if (x > 0) {			\
									op1;			\
									CASE4(x,op2);		\
								    }				\
				    }
#define	UNROLL2(w, op1, op2) {		\
					DWORD x;		\
					for (x = w; x >= 2; x -= 2) {	\
									op1;			\
									REPEAT2(op2);		\
								    }				\
					if (x) {			\
									op1;			\
									op2;			\
								    }				\
				    }


#define	SKEW(r,g,b,skew)	{ r += skew; g += skew; b += skew; }

/*
 * 8-bit palette => colormap/RGB
 */
void SECTiff::SECput8bitcmaptile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp);
	ASSERT(pp);

    while (h-- > 0) {
	UNROLL8(w,0, (*cp++ = lppPALmap[*pp++][0]));
	cp += toskew;
	pp += fromskew;
    }
    Map; //UNUSED
}

/*
 * 4-bit palette => colormap/RGB
 */
//static void
void SECTiff::SECput4bitcmaptile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp);
	ASSERT(pp);

    LPBYTE bw;
    
    fromskew /= 2;
    while (h-- > 0) {
	UNROLL2(w, bw = lppPALmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }        
    Map; //UNUSED
}

/*
 * 2-bit palette => colormap/RGB
 */
// static void
void SECTiff::SECput2bitcmaptile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp);
	ASSERT(pp);

    LPBYTE bw;
    
    fromskew /= 4;
    while (h-- > 0) {
	UNROLL4(w, bw = lppPALmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }            
    Map; //UNUSED
}


/*
 * 1-bit palette => colormap/RGB
 */
// static void
void SECTiff::SECput1bitcmaptile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);
    LPBYTE bw;
    
    fromskew /= 8;
    while (h-- > 0) {
	UNROLL8(w, bw = lppPALmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }            
    Map; //UNUSED
}

/*
 * 8-bit greyscale => colormap/RGB
 */
// static void
void SECTiff::SECputgreytile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);
    while (h-- > 0) {
	DWORD x;
	for (x = w; x-- > 0;)
	    *cp++ = lppBWmap[*pp++][0];
	cp += toskew;
	pp += fromskew;
    }            
    Map; //UNUSED
}

void SECTiff::pack1(int *pnByte, DWORD *pdwWidth, DWORD *pdwTotalBytes, LPBITS *cp, LPBYTE *bw, DWORD wMax, DWORD dwBytesInLine) 
{
	BYTE b=1;
	(*pdwWidth)++;
	if (**bw) {
		**cp = (BYTE)(**cp | (b<<(7-*pnByte)));
	}
	(*bw)++;
	(*pnByte)++;

	if (*pnByte>7) {
		(*cp)++;   
		(*pdwTotalBytes)++;
		*pnByte=0;
	}
	if ((*pdwWidth)==wMax) {
		*pdwWidth=0;
		for (DWORD i=0; i< (dwBytesInLine - *pdwTotalBytes); i++)
			(*cp)++;
		*pnByte=0;
		*pdwTotalBytes=0;
	}
	
}
/*
 * 1-bit bilevel => colormap/RGB
 */
// static void
void SECTiff::SECput1bitbwtile(LPBITS cp, LPBYTE pp, LPBYTE pMap, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);
	int nByte=0;
	DWORD dwWidth=0;
	DWORD dwTotalBytes = 0;

    LPBYTE bw;
    pMap;
    
    fromskew /= 8;
	toskew = (int)LastByte(m_nSrcBitsPerPixel, toskew);
	//AAB?? toskew /= 8;
	DWORD dwBytesLine = LastByte(m_nSrcBitsPerPixel, m_dwWidth);
    while (h-- > 0) {
	UNROLL8(w, bw = lppBWmap[*pp++], pack1(&nByte, &dwWidth, &dwTotalBytes, &cp,&bw, m_dwWidth, dwBytesLine));
	cp += toskew;
	pp += fromskew;
    }
}

/*
 * 2-bit greyscale => colormap/RGB
 */
// static void
void SECTiff::SECput2bitbwtile(LPBITS cp, LPBYTE pp, LPBYTE pMap, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);

    LPBYTE bw;
    pMap;
    
    fromskew /= 4;
    while (h-- > 0) {
	UNROLL4(w, bw = lppBWmap[*pp++], *cp++ = *bw++);
	cp += toskew;
	pp += fromskew;
    }
}

/*
 * 4-bit greyscale => colormap/RGB
 */
// static void
void SECTiff::SECput4bitbwtile(LPBITS cp, LPBYTE pp, LPBYTE pMap, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);

	LPBYTE bw;
	pMap;

	fromskew /= 2;
	while (h-- > 0) {
		UNROLL2(w, bw = lppBWmap[*pp++], *cp++ = *bw++);
		cp += toskew;
		pp += fromskew;
	}
}

/*
 * 8-bit packed samples => RGB
 */
// static void
void SECTiff::SECputRGBcontig8bittile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);

	fromskew *= m_wSPP;
	if (Map) {
		while (h-- > 0) {
			DWORD x;
			for (x = w; x-- > 0;) {
				*cp++ = Map[pp[0]];
				*cp++ = Map[pp[1]];
				*cp++ = Map[pp[2]];
				pp += m_wSPP;
			}
			pp += fromskew;
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			UNROLL8(w,0,
			    (*cp++ = pp[2],	 // AAB reverse!!!
			    *cp++ = pp[1],
			    *cp++ = pp[0],
			    pp += m_wSPP));
   
			cp += toskew;
			pp += fromskew;
		}
	}
}

/*
 * 16-bit packed samples => RGB
 */
// static void
void SECTiff::SECputRGBcontig16bittile(LPBITS cp, LPBYTE tpp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(tpp!=0);

	DWORD x;
	LPWORD pp = (LPWORD)tpp;

	fromskew *= m_wSPP;
	if (Map) {
		while (h-- > 0) {
			for (x = w; x-- > 0;) {
				*cp++ = Map[pp[0]];
				*cp++ = Map[pp[1]];
				*cp++ = Map[pp[2]];
				pp += m_wSPP;
			}
			cp += toskew;
			pp += fromskew;
		}
	} else {
		while (h-- > 0) {
			for (x = w; x-- > 0;) {
				*cp++ = (BYTE)pp[0];
				*cp++ = (BYTE)pp[1];
				*cp++ = (BYTE)pp[2];
				pp += m_wSPP;
			}
			cp += toskew;
			pp += fromskew;
		}
	}
}

/*
 * 8-bit unpacked samples => RGB
 */
// static void
void SECTiff::SECputRGBseparate8bittile(LPBYTE cp, LPBYTE r, LPBYTE g, LPBYTE b, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(r!=0);
	ASSERT(g!=0);
	ASSERT(b!=0);
	if (Map) {
		while (h-- > 0) {
			DWORD x;
			for (x = w; x > 0; x--) {
				*cp++ = Map[*r++];
				*cp++ = Map[*g++];
				*cp++ = Map[*b++];
			}
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			UNROLL8(w,0,
			    (*cp++ = *r++,
			    *cp++ = *g++,
			    *cp++ = *b++ )
			);
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	}
}

/*
 * 16-bit unpacked samples => RGB
 */
// static void
void SECTiff::SECputRGBseparate16bittile(LPBYTE cp, LPBYTE pr, LPBYTE pg, LPBYTE pb, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pr!=0);
	ASSERT(pg!=0);
	ASSERT(pb!=0);

	DWORD x;
	LPWORD r=(LPWORD)pr;
	LPWORD g=(LPWORD)pg;
	LPWORD b=(LPWORD)pb;

	if (Map) {
		while (h-- > 0) {
			for (x = w; x > 0; x--) {
				*cp++ = Map[*r++];
				*cp++ = Map[*g++];
				*cp++ = Map[*b++];
			}
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	} else {
		while (h-- > 0) {
			for (x = 0; x < w; x++) {
				*cp++ = (BYTE)*r++;
				*cp++ = (BYTE)*g++;
				*cp++ = (BYTE)*b++;
			}
			SKEW(r, g, b, fromskew);
			cp += toskew;
		}
	}
}

/* #define Code2V(c, RB, RW, CR)  ((((c)-(int)RB)*(float)CR)/(float)(RW-RB)) */
#define Code2V(c, RB, RW, CR)     ((((c)-RB)*(float)CR)/(float)(RW-RB))

#define	CLAMP(f,min,max) \
    (BYTE)((f)+.5 < (min) ? (min) : (f)+.5 > (max) ? (max) : (f)+.5)

#define	LumaRed		m_lpYCbCrCoeffs[0]
#define	LumaGreen	m_lpYCbCrCoeffs[1]
#define	LumaBlue	m_lpYCbCrCoeffs[2]

static	float D1, D2;
static	float D3, D4;

void 
SECTiff::SECinitYCbCrConversion()
{
    D1 = 2 - 2*LumaRed;
    D2 = D1*LumaRed / LumaGreen;
    D3 = 2 - 2*LumaBlue;
    D4 = D2*LumaBlue / LumaGreen;
}

void SECTiff::SECputRGBContigYCbCrClump(LPBYTE cp, LPBYTE pp, WORD cw, WORD ch, DWORD w, int n, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);
	float Cb, Cr;
	WORD j, k; //int j, k;

	Cb = Code2V(pp[n],   lpRefBlackWhite[2], lpRefBlackWhite[3], 127);
	Cr = Code2V(pp[n+1], lpRefBlackWhite[4], lpRefBlackWhite[5], 127);
	for (j = 0; j < ch; j++) {
		for (k = 0; k < cw; k++) {
			float Y, R, G, B;
			Y = Code2V(*pp++,
			    lpRefBlackWhite[0], lpRefBlackWhite[1], 255);
			R = Y + Cr*D1;
			B = Y + Cb*D3;
			G = Y - Cb*D4 - Cr*D2;
			cp[3*k+0] = CLAMP(R,0,255);
			cp[3*k+1] = CLAMP(G,0,255);
			cp[3*k+2] = CLAMP(B,0,255);
		}
		cp += 3*w+toskew;
		pp += fromskew;
	}
}
#undef LumaBlue
#undef LumaGreen
#undef LumaRed
#undef CLAMP
#undef Code2V

/*
 * 8-bit packed YCbCr samples => RGB
 */
void SECTiff::SECputcontig8bitYCbCrtile(LPBITS cp, LPBYTE pp, LPBYTE Map, DWORD w, DWORD h, int fromskew, int toskew)
{
	ASSERT(cp!=0);
	ASSERT(pp!=0);

	WORD Coff = (WORD)(wYCbCrVertSampling * wYCbCrHorizSampling);
	LPBYTE tp;
	DWORD x;

	/* XXX adjust fromskew */
	while (h >= wYCbCrVertSampling) {
		tp = cp;
		for (x = w; x >= (DWORD)wYCbCrHorizSampling; x -= (DWORD)wYCbCrHorizSampling) {
			SECputRGBContigYCbCrClump(tp, pp,
			    wYCbCrHorizSampling, wYCbCrVertSampling,
			    w, Coff, 0, toskew);
			tp += 3*wYCbCrHorizSampling;
			pp += Coff+2;
		}
		if (x > 0) {
			SECputRGBContigYCbCrClump(tp, pp,
			    (WORD)x, wYCbCrVertSampling,
			    (DWORD)w, Coff, (int)(wYCbCrHorizSampling - x), toskew);
			pp += Coff+2;
		}
		cp += wYCbCrVertSampling*(3*w + toskew);
		pp += fromskew;
		h -= wYCbCrVertSampling;
	}
	if (h > 0) {
		tp = cp;
		for (x = w; x >= wYCbCrHorizSampling; x -= wYCbCrHorizSampling) {
//void putRGBContigYCbCrClump(LPBYTE cp, LPBYTE pp, WORD cw, WORD ch, DWORD w, int n, int fromskew, int toskew)
			SECputRGBContigYCbCrClump(tp, pp, wYCbCrHorizSampling, (WORD)h,
			    (DWORD)w, (int)Coff, 0, toskew);
			tp += 3*wYCbCrHorizSampling;
			pp += Coff+2;
		}
		if (x > 0)
			SECputRGBContigYCbCrClump(tp, pp, (WORD)x, (WORD)h,
			    (DWORD)w, (int)Coff, (int)(wYCbCrHorizSampling - x), toskew);
	}            
	Map; //UNUSED
}

/*
 * Select the appropriate conversion routine for packed data.
 */


// static tileContigRoutine
tileContigRoutine SECTiff::SECpickTileContigCase(LPBYTE Map)
{
	tileContigRoutine put = 0;
	// void (*put)(LPBYTE, LPBYTE, LPBYTE, DWORD, DWORD, int, int);

	switch (m_wPhoto) {
	case PHOTOMETRIC_RGB:
		put = (m_wBPS == 8 ?
		    (&SECTiff::SECputRGBcontig8bittile) : (&SECTiff::SECputRGBcontig16bittile));
		break;
	case PHOTOMETRIC_PALETTE:
		switch (m_wBPS) {
		case 8:	put = &SECTiff::SECput8bitcmaptile; break;
		case 4: put = &SECTiff::SECput4bitcmaptile; break;
		case 2: put = &SECTiff::SECput2bitcmaptile; break;
		case 1: put = &SECTiff::SECput1bitcmaptile; break;
		}
		break;
	case PHOTOMETRIC_MINISWHITE:
	case PHOTOMETRIC_MINISBLACK:
		switch (m_wBPS) {
		case 8:	put = &SECTiff::SECputgreytile; break;
		case 4: put = &SECTiff::SECput4bitbwtile; break;
		case 2: put = &SECTiff::SECput2bitbwtile; break;
		case 1: put = &SECTiff::SECput1bitbwtile; break;
		}
		break;
	case PHOTOMETRIC_YCBCR:
		switch (m_wBPS) {
		case 8: put = &SECTiff::SECputcontig8bitYCbCrtile; break;
		}
		break;
	}            
	Map; //UNUSED
	//if (put == 0)
	//	TIFFError(filename, "Can not handle format");
	return (put);
}

/*
 * Select the appropriate conversion routine for unpacked data.
 *
 * NB: we assume that unpacked single channel data is directed
 *	 to the "packed routines.
 */
// static tileSeparateRoutine
tileSeparateRoutine SECTiff::SECpickTileSeparateCase(LPBYTE Map)
{
	tileSeparateRoutine put = 0;

	switch (m_wPhoto) {
	case PHOTOMETRIC_RGB:
		put = (m_wBPS == 8 ?
//		    this->SECputRGBseparate8bittile : this->SECputRGBseparate16bittile);
		    tileSeparateRoutine(&SECTiff::SECputRGBcontig8bittile) : tileSeparateRoutine(&SECTiff::SECputRGBcontig16bittile));

		break;
	}            
	Map; //UNUSED
	//if (put == 0)
	//	TIFFError(filename, "Can not handle format");
	return (put);
}


#define ALLOW_JPEG 0  /* set to '1' to allow 'JPEG' choice in dialog box */


void SECTiff::setupColormap(TIFF *tif, LPBYTE rmap, LPBYTE gmap, LPBYTE bmap)
{
  ASSERT(tif!=0);
  WORD red[256], green[256], blue[256];
  WORD i;
  
  /* convert 8-bit colormap to 16-bit */
  for (i=0; i<m_wColors; i++) {
#define	SCALE(x)	(((x)*((1L<<16)-1))/255)
      red[i] = (WORD)SCALE(m_lpRGB[i].rgbRed);
      green[i] = (WORD)SCALE(m_lpRGB[i].rgbGreen);
      blue[i] = (WORD)SCALE(m_lpRGB[i].rgbBlue);
  }            
  bmap; //UNUSED
  gmap;
  rmap;
  TIFFSetField(tif, TIFFTAG_COLORMAP, red, green, blue);
}



/*******************************************/
int SECTiff::WriteTIFF(FILE *fp, LPBYTE pic, int ptype, LPBYTE rmap,
		       LPBYTE gmap,LPBYTE bmap,int numcols,int colorstyle,
		       const TCHAR *fname, TCHAR * comment)
{
    ASSERT(fp!=0);
	ASSERT(pic!=0);
    TIFF *tif;
    LPBYTE pix;
    // int   i,j;
    DWORD i,j;
    
    tif = TIFFOpen(fname, _T("w"));
    
    if (!tif)   
    {
		TRACE(_T("SECTiff::WriteTiff failed to open for writing\n"));
    	return 0; 
	}
    
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, m_dwWidth);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, m_dwHeight);
    TIFFSetField(tif, TIFFTAG_COMPRESSION, m_nCompression);
    
    if (comment && _tcslen(comment)>0) {
	TIFFSetField(tif, TIFFTAG_IMAGEDESCRIPTION, comment);
    }

    if (m_nCompression == COMPRESSION_CCITTFAX3)
      TIFFSetField(tif, TIFFTAG_GROUP3OPTIONS,
		   GROUP3OPT_2DENCODING+GROUP3OPT_FILLBITS);
    
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, m_dwHeight);
    
    /* write the image data */
    
    if (m_nSrcBitsPerPixel == 24) {  /* only have to deal with FULLCOLOR or GREYSCALE */
	colorstyle = F_FULLCOLOR;
	if (colorstyle == F_FULLCOLOR) {
	    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 3);
	    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
	    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_RGB);
	    
	    TIFFWriteEncodedStrip(tif, 0, pic, m_dwWidth*m_dwHeight*3);
	}

	else {  /* colorstyle == F_GREYSCALE */
	    LPBYTE tpic=NULL, tp, sp;
	    
	    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 1);
	    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE,   8);
	    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC,     PHOTOMETRIC_MINISBLACK);
	    
	    tpic = (LPBYTE)GlobalAllocPtr(GHND, m_dwWidth*m_dwHeight);
	    if (tpic == NULL)
	  	return (NULL);
      
#define MONO(rd,gn,bl) (BYTE)(((rd)*11 + (gn)*16 + (bl)*5) >> 5)          

	    for (i=0, tp=tpic, sp=pic; i<m_dwWidth*m_dwHeight; i++, sp+=3) 
		*tp++ = MONO(sp[0],sp[1],sp[2]);
	    
	    TIFFWriteEncodedStrip(tif, 0, tpic, m_dwWidth*m_dwHeight);
	    
	    GlobalFreePtr(tpic);
	}
    }
    
    else {  /* PIC8 */
		//AAB NEW
		if (m_nSrcBitsPerPixel>1)
			colorstyle = F_FULLCOLOR;
		else
			colorstyle = F_BWDITHER;

	//AAB NEW colorstyle = F_FULLCOLOR;
	if (colorstyle == F_FULLCOLOR) {                  /* 8bit Palette RGB */
	    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_PALETTE);
	    setupColormap(tif, rmap, gmap, bmap);
	    TIFFWriteEncodedStrip(tif, 0, pic, m_dwWidth*m_dwHeight);
	}	
	else if (colorstyle == F_GREYSCALE) {             /* 8-bit greyscale */
	    BYTE rgb[256];
	    LPBYTE tpic = (LPBYTE)GlobalAllocPtr(GHND, m_dwWidth * m_dwHeight);
	    LPBYTE tp = tpic;
	    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	    for (i=0; i<(DWORD)numcols; i++) rgb[i] = MONO(rmap[i],gmap[i],bmap[i]);
	    for (i=0, pix=pic; i<m_dwWidth*m_dwHeight; i++,pix++) {
		*tp++ = rgb[*pix];
	    }
	    TIFFWriteEncodedStrip(tif, 0, tpic, m_dwWidth*m_dwHeight);
	    GlobalFreePtr(tpic);
	}	
	else if (colorstyle == F_BWDITHER) {             /* 1-bit B/W stipple */
	    int k,flipbw;
	    LPBYTE tpic, tp;
	    
		flipbw = 1;
	    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 1);
	    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
	    tpic = (LPBYTE)GlobalAllocPtr(GHND, TIFFStripSize(tif));
	    if (!tpic)
		return 0;
	    tp = tpic;
	    for (i=0, pix=pic; i<m_dwHeight; i++) {
			for(j=0;j<m_dwWidth/8;j++) {
				k=*pix++;
				k= ~k;
				*tp++ = (BYTE)(k&0xff);
			}
		}
		
	    TIFFWriteEncodedStrip(tif, 0, tpic, TIFFStripSize(tif));
	    GlobalFreePtr(tpic);
	}
    }
    
    
    TIFFClose(tif);
    ptype; //UNUSED
    fp;
    return 1;
}


/* local variables */
static char *filename;
static int   colorType;


void SECTiff::ConvertRGB()
{
    BYTE r;
#ifdef WIN32
    LPBYTE lpPtr = m_lpSrcBits;
#else 
    BYTE huge *lpPtr = m_lpSrcBits;
#endif
    if (m_nSrcBitsPerPixel < 24)
	return;
    
    // Temporary function to convert images correctly into the
    // format TIFF is expecting...  basically need to reverse R and B
    for (DWORD dwRow=0; dwRow<m_dwHeight; dwRow++)
	for (DWORD dwCol=0; dwCol<m_dwWidth; dwCol++) {
	    r = (BYTE)*lpPtr;
	    *(lpPtr) = *(lpPtr+2);
	    *(lpPtr+2) = r;
	    lpPtr += 3;
	}
}

/*******************************************/
BOOL SECTiff::DoSaveImage(CFile* pFile)
{
    // Since TIFF uses a special file type,
    // we can't use this function.
    pFile; // UNUSED
    return FALSE;
}

BOOL SECTiff::SaveImage(LPCTSTR filename)
{
	ASSERT(filename != 0);
	ASSERT_VALID(this);

    if (!PreSaveImage())
	return FALSE;

    FILE *fp;
    int  nc=0, rv, ptype=0;
    LPBYTE rmap=NULL, gmap=NULL, bmap=NULL;
    LPBYTE thepic;

    
    thepic = m_lpSrcBits;

    
    /* see if we can open the output file before proceeding */
    fp = _tfopen(filename, _T("w"));
    if (!fp)
    {
		TRACE(_T("SECTiff::SECSaveImage could not open file %s ofr writing\n"),(const char *)filename);
    	return FALSE; 
	}
    
    // Normalize the image from intermediate DIB format to a form
    // coherent to TIFF
    FlipVert();
	UnPadBits();
    ConvertRGB();
    
    
    rv = WriteTIFF(fp, thepic, ptype,
		   rmap, gmap, bmap, nc, colorType, filename, NULL);
    
    
    fclose(fp);
    
    // Transfer image data back into intermediate DIB format
    ConvertRGB();
    FlipVert();
	// note: FlipVert will actually call PadBits, so we don't
	// need the redundant call below...
    // PadBits();

    if (!PostSaveImage())
	return FALSE;
    
    return TRUE;
}


void SECTiff::Serialize(CArchive& ar)
{
    SECImage::Serialize(ar);    
}


#ifdef _DEBUG
void SECTiff::AssertValid() const
{
    CObject::AssertValid();
	SECImage::AssertValid();
}
#endif /* _DEBUG */

