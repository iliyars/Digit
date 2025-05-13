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
//  Description:	Implementation of SECImage DSP algorithms
//  Created:		Tue Aug 29 15:19:52 1995
//
//  CHANGELOG:
//
//    MSW     9/30/95   Code Review

/*

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%
%
%
%     C o n t r a s t I m a g e
%
%
%
%
%
%
%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
%  Function ContrastImage enhances the intensity differences between the
%  lighter and darker elements of the image.
%
%  The format of the ContrastImage routine is:
%
%      ContrastImage(image,sharpen)
%
%  A description of each parameter follows:
%
%    o image: The address of a structure of type Image;  returned from
%      ReadImage.
%
%    o sharpen: If True, the intensity is increased otherwise it is
%      decreased.
%
%
*/

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifndef _INC_WINDOWSX
#include <windowsx.h>
#endif

#ifdef _CMNDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_CMNDLL

#ifndef __SECIMAGE_H__
#include "..\..\Include\Image\secimage.h"
#endif

#ifndef _INC_MATH
#include <math.h>
#endif

#ifdef _CMNDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_CMNDLL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#define MaxRGB 255

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define ISODD(x) (x&0x01)


//
//	Rotate90
//
//	Rotate the image 90 degrees.
//	This is a resource intense routine since we allocate a brand new
//	image array to copy the existing image array into.
//@doc SECImage
//@mfunc Rotates the image 90 degrees and swaps the width and height parameters.
//@rdesc Nonzero if successful; otherwise 0.
//@comm Rotates the image 90 degrees and updates the image width and height parameters
// to reflect the rotation.  All pixels will swap their row position with their
// column position.  The image’s width and height parameters will also be swapped.
//@xref <c SECImage> <mf SECImage::ConvertImage> <mf SECImage::FlipHorz>
// <mf SECImage::Rotate90> <mf SECImage::Crop>
BOOL SECImage::Rotate90(BOOL bClockwise)
{
    ASSERT_VALID(this);

    DWORD       srcWidth, srcHeight;
    DWORD       padWidth;
    DWORD       x, y, z;
    WORD        wBitCount;
    BYTE        srcByte, destByte;
#ifdef WIN16
    char _huge  *lpSrcBits;
    char _huge  *lpDestBits;
    char _huge  *lpRotated;
    char _huge  *lpTmp;
    lpSrcBits  = (char _huge  *)m_lpSrcBits;
#else
    LPSTR       lpSrcBits, lpDestBits;
    LPSTR       lpRotated;
    LPSTR       lpTmp;
    lpSrcBits  = (LPSTR)m_lpSrcBits;
#endif

    srcWidth   = (unsigned)m_dwWidth;
    srcHeight  = (unsigned)m_dwHeight;
    wBitCount  = (WORD)m_nSrcBitsPerPixel;


    // Calculate the padding required to align on DWORDs
    // since DIB scanlines are aligned on DWORD boundaries
    padWidth  = CalcPadding(wBitCount, srcWidth);

    // Create a temp array to hold the new rotated data in..
    DWORD size = CalcBytesPerLine(wBitCount, srcHeight) * srcWidth;
#ifdef WIN32
    lpRotated = (LPSTR)GlobalAllocPtr(GHND, size);
#else
    lpRotated = (char _huge  *)GlobalAllocPtr(GHND, size);
#endif
    if (!lpRotated)
    {
        TRACE(_T("SECImage::Rotate90 could not allocate memory for rotation\n"));
        return FALSE;
    }
    lpDestBits = lpRotated;

    // Copy the source bits to the destination while rotating each scanline
    DWORD scanline = CalcBytesPerLine(wBitCount, srcHeight);
    switch (wBitCount)
    {
        case 1: // monochrome DIBs
        {
            BYTE BitMask = 0x80;
            long destPos;
            BYTE destBit;
            memset((void *)lpDestBits, 0, (size_t)(scanline*srcWidth) );

            if(bClockwise) { // clockwise
                 for (y = 0; y < srcHeight; y++) {
                     destPos = (y % 8);
                     destBit = (char)(BitMask >> destPos);
                     for (x = 0; x < srcWidth; x++)
                     {
                         if(*lpSrcBits & (BitMask >> (x % 8))) {
                               // Get the destination byte
                               lpTmp = lpDestBits + ((srcWidth - x - 1) * scanline);
                               // Set the destination bit
                               *lpTmp |= (BitMask >> destPos);                                                                                 }
                               if (((x + 1) % 8) == 0)
                                  // Point to next source byte
                                  lpSrcBits++;
                      }

                      // Skip bytes in the source to pad to a DWORD
                      lpSrcBits += padWidth;

                      if (((y + 1) % 8) == 0)
                          // Point to next column in the destination
                          lpDestBits++;
                 }
            } else {  // counterclockwise
                 lpDestBits += (LastByte(wBitCount, srcHeight) - 1);
                 for (y = 0; y < srcHeight; y++)
                 {
                     destPos = ((srcHeight-y-1) % 8);
                     destBit = (unsigned char)(BitMask >> destPos);
                     for (x = 0; x < srcWidth; x++)
                     {
                         // Shift src bit to dest bit's position
                         if(*lpSrcBits & (BitMask >> (x % 8))) {
                              // Get the destination byte
                              lpTmp = lpDestBits + (x * scanline);

                              // Set the destination bit
                              *lpTmp |= (BitMask >> destPos);
                         }
                         if (((x + 1) % 8) == 0)
                             // Point to next source byte
                             lpSrcBits++;
                     }
                     // Skip bytes in the source to pad to a DWORD
                     lpSrcBits += padWidth;

                     if (destPos == 0)
                          // Point to next column in the destination
                          lpDestBits--;

                 }
            }
        }
        break;

        case 4: // 16-color DIBs
        {
            BYTE LoMask    = 0x0F;
            BYTE HiMask    = 0xF0;
            BOOL bLoNibble = FALSE;

            if(bClockwise) { // clockwise
                 for (y = 0; y < srcHeight; y++)
                {
                    for (x = 0; x < srcWidth; x++)
                    {
                       // Get the source byte
                       srcByte = *lpSrcBits;

                       // Mask off the nibble we want
                       if (x % 2)
                       {
                            if (bLoNibble)
                                srcByte &= LoMask;
                            else
                                srcByte = (unsigned char)((srcByte << wBitCount) & HiMask);
                       }
                       else
                       {
                           if (bLoNibble)
                                srcByte = (unsigned char)((srcByte >> wBitCount) & LoMask);
                           else
                                srcByte &= HiMask;
                       }

                       // Get the destination byte
                       lpTmp = lpDestBits + ((srcWidth - x - 1) * scanline);
                       destByte = *lpTmp;

                       // Mask off the unneeded nibble
                       if (bLoNibble)
                           destByte &= HiMask;
                       else
                           destByte = 0x00;

                        // Set the dest nibble
                        *lpTmp = (char)(srcByte | destByte);

                        if (x % 2)
                            // Point to next source byte
                            lpSrcBits++;
                   }

                   // Skip bytes in the source to pad to a DWORD
                   lpSrcBits += padWidth;

                   if (y % 2)
                        // Point to next column in the destination
                        lpDestBits++;

                   // Toggle LoNibble flag
                   bLoNibble = !bLoNibble;
               }
           } else {  // counterclockwise
              lpDestBits += (LastByte(wBitCount, srcHeight) - 1);
              if((srcHeight % 2)==0)
                  bLoNibble = TRUE;

              for (y = 0; y < srcHeight; y++)
              {
                  for (x = 0; x < srcWidth; x++)
                  {
                      // Get the source byte
                      srcByte = *lpSrcBits;

                      // Mask off the nibble we want
                      if (x % 2)
                      {
                           if (bLoNibble)
                               srcByte &= LoMask;   // 2
                           else
                               srcByte = (unsigned char)((srcByte << wBitCount) & HiMask);  // 4
                       }
                       else
                       {
                           if (bLoNibble)            // 3
                               srcByte = (unsigned char)((srcByte >> wBitCount) & LoMask);
                           else
                               srcByte &= HiMask;    // 1
                       }

                       // Get the destination byte
                       lpTmp = lpDestBits + (x * scanline);
                       destByte = *lpTmp;

                       // Mask off the unneeded nibble
                       if (bLoNibble)
                           destByte = 0x00;
                       else
                           destByte &= LoMask;

                       // Set the dest nibble
                       *lpTmp = (char)(srcByte | destByte);

                       if (x % 2)
                          // Point to next source byte
                          lpSrcBits++;
                       }

                       // Skip bytes in the source to pad to a DWORD
                       lpSrcBits += padWidth;

                       if ((srcHeight-y) % 2)
                            // Point to next column in the destination
                            lpDestBits--;

                       // Toggle LoNibble flag
                       bLoNibble = !bLoNibble;
                   }
               }
           }
           break;

        case 8: // 256-color DIBs
           if(bClockwise) { // clockwise
               for (y = 0; y < srcHeight; y++)
               {
                   for (x = 0; x < srcWidth; x++)
                   {
                      // Get the source byte
                      srcByte = *lpSrcBits++;

                      // Set the destination byte
                      lpTmp  = lpDestBits + ((srcWidth - x - 1) * scanline);
                      *lpTmp = srcByte;
                   }

                   // Skip bytes in the source to pad to a DWORD.
                   lpSrcBits += padWidth;

                   // Next column in the destination. Only for 90 degree rotations.
                   lpDestBits++;
               }
           } else { // counter clockwise
               lpDestBits += (LastByte(wBitCount, srcHeight) - 1);
               for (y = 0; y < srcHeight; y++)
               {
                  for (x = 0; x < srcWidth; x++)
                  {
                     // Get the source byte
                      srcByte = *lpSrcBits++;

                      // Set the destination byte
                      lpTmp  = lpDestBits + (x * scanline);
                      *lpTmp = srcByte;
                   }

                   // Skip bytes in the source to pad to a DWORD.
                   lpSrcBits += padWidth;

                   // Next column in the destination. Only for 90 degree rotations.
                   lpDestBits--;
               }
           }
           break;

		case 16:
		case 24:
		case 32:
			if (bClockwise) {
               for (y = 0; y < srcHeight; y++)
               {
                   for (x = 0; x < srcWidth; x++)
                   {
                      // Get the source byte
						// srcByte = *lpSrcBits++;

						// Set the destination byte
						lpTmp  = lpDestBits + ((srcWidth - x - 1) * scanline);
						for (z=0; z<3; z++)
						{
							srcByte = *lpSrcBits++;
							*lpTmp++ = srcByte;
						}


                   }

                   // Skip bytes in the source to pad to a DWORD.
                   lpSrcBits += padWidth;

                   // Next column in the destination. Only for 90 degree rotations.
                   lpDestBits+=3;
               }
			} else {
				lpDestBits += (LastByte(wBitCount, srcHeight) - 3);
				for (y=0; y< srcHeight; y++)
				{
					for (x = 0; x < srcWidth; x++)
					{
						lpTmp = lpDestBits + (x*scanline);
						for (z=0; z<3; z++) {
							srcByte = *lpSrcBits++;
							*lpTmp++ = srcByte;
						}
					}
					lpSrcBits += padWidth;
					lpDestBits -= 3;
				}
			}
			break;

        default: // unsupported format
           GlobalFreePtr(lpDestBits);
		   CString strDum;
//		   strDum.LoadString(IDS_CMN_SECIMAGE_MSG1);
           ::AfxMessageBox(strDum, MB_OK);
           return FALSE;
    }
    // When we get here, a rotated image is available.


    // Change only those member which are affected by image rotation
    DWORD tmpVal = m_dwWidth;
    m_dwWidth = m_dwHeight;
    m_dwHeight = tmpVal;
    m_dwPadWidth = scanline;  //NB: this is now bytes!!

    m_lpBMI->bmiHeader.biWidth = (long)m_dwWidth;
    m_lpBMI->bmiHeader.biHeight = (long)m_dwHeight;

    // Point to rotated image and Free memory for the original image
    LPBYTE lpTemp = m_lpSrcBits;
    m_lpSrcBits = (unsigned char *)lpRotated;
    GlobalFreePtr(lpTemp);

    return TRUE;

}

//
//	Crop
//
//
//	Crops the image to the specifications passed in
//
//
//
//@doc SECImage
//@mfunc Crops the image to the dimensions specified.
//@comm The image data is automatically updated to reflect the cropping area.
// Any image data outside the cropping area is lost.
//@parm long | nLeft | The left side of the cropping rectangle.
//@parm long | nTop | The top side of the cropping rectangle.
//@parm long | nWidth | The width of the cropping rectangle.
//@parm long | nHeight | The height of the cropping rectangle.
//@xref <c SECImage> <mf SECImage::ConvertImage> <mf SECImage::ContrastImage>
// <mf SECImage::FlipVert> <mf SECImage::Rotate90>
BOOL SECImage::Crop(long nLeft, long nTop, long nWidth, long nHeight)
{
#ifdef ORIG
    ASSERT_VALID(this);
	ASSERT(nLeft>0);
	ASSERT(nTop>0);
	ASSERT(nWidth>0);
	ASSERT(nHeight>0);

    long nSrcRow, nDestRow, nColAdjust=0, nBPP=1, nUsePad;
	DWORD dwUsePad;

    if (m_nSrcBitsPerPixel>=24)
	nBPP=3;

    dwUsePad = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);

    if (ISODD(nWidth))
	nColAdjust = 1;

    LPBYTE lptemp = (LPBYTE)GlobalAllocPtr(GHND, nUsePad);
    if(!lptemp)
	{
		TRACE(_T("SECImage::Crop() could not allocate memory for image manipulation\n"));
		return FALSE;
	}

    FlipVert();
    // enough space has already been allocated for the bit array to
    // include the padding, so we just need to shift rows around.
    for (nSrcRow=nTop, nDestRow=0; (nSrcRow<nHeight && nDestRow<(long)m_dwHeight); nSrcRow++, nDestRow++)
	{
	    DWORD offset = nSrcRow*m_dwPadWidth + nLeft*nBPP;
	    DWORD dest = nDestRow*nUsePad;
	    _fmemcpy(lptemp, (m_lpSrcBits + ((DWORD)nSrcRow*m_dwPadWidth*nBPP) + nLeft*nBPP),
		     (size_t)nUsePad);
	    _fmemcpy(m_lpSrcBits + nDestRow*nUsePad,
		     lptemp,
		     (size_t)nUsePad);
	}


    GlobalFreePtr(lptemp);
    // Reset the image member height and width to cropped values.
    if (nHeight<(long)m_dwHeight)
		m_dwHeight = nHeight;
    if (nWidth<(long)m_dwWidth)
		m_dwWidth = nWidth;
    m_dwPadWidth = PADWIDTH(m_dwWidth);
    FlipVert();

    // adjust m_lpBMI parameters
    m_lpBMI->bmiHeader.biWidth = nWidth;
    m_lpBMI->bmiHeader.biHeight = nHeight;
    GlobalReAllocPtr(m_lpSrcBits, m_dwPadWidth*m_dwHeight*nBPP, GMEM_MOVEABLE);
    return (TRUE);
#endif
    ASSERT_VALID(this);
	ASSERT(nWidth>0);
	ASSERT(nHeight>0);

    long nSrcRow, nDestRow, nColAdjust=0, nBPP=1, nBitSrc=0,nBitDest=0;
	DWORD dwSrcUsePad, dwDestUsePad, dwSrcOffset, dwDestOffset;
	BYTE cSrc = 0, cDest = 0, cTmp;

    if (m_nSrcBitsPerPixel>=24)
	nBPP=3;


	dwSrcUsePad = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);
	dwDestUsePad = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, nWidth);

    if (ISODD(nWidth))
	nColAdjust = 1;

    LPBYTE lptemp = (LPBYTE)GlobalAllocPtr(GHND, dwSrcUsePad);
    if(!lptemp)
	{
		TRACE(_T("SECImage::Crop() could not allocate memory for image manipulation\n"));
		return FALSE;
	}

    FlipVert();
    // enough space has already been allocated for the bit array to
    // include the padding, so we just need to shift rows around.
    // for (nSrcRow=nTop, nDestRow=0; (nSrcRow<nHeight && nDestRow<(long)m_dwHeight); nSrcRow++, nDestRow++)
    for (nSrcRow=nTop, nDestRow=0; (nSrcRow<nTop+nHeight && nDestRow<(long)m_dwHeight); nSrcRow++, nDestRow++)
	{
		// Calculate the starting byte
		dwSrcOffset = dwSrcUsePad*nSrcRow;
		dwDestOffset = 	dwDestUsePad*nDestRow;
		switch(m_nSrcBitsPerPixel) {
		case 1:
			dwSrcOffset += nLeft/8;
			nBitSrc = nLeft%8;
			cSrc = m_lpSrcBits[dwSrcOffset];
			cDest=0x0;
			break;
		case 4:
			dwSrcOffset += nLeft/2;
			nBitSrc = nLeft%2;
			cSrc = m_lpSrcBits[dwSrcOffset];
			cDest=0x0;
			break;
		case 8:
			dwSrcOffset += nLeft;
			break;
		default:
			dwSrcOffset += nLeft*3;
			break;
		}

		// dwSrcOffset now contains the starting BYTE, nBitSrc the starting bit or nibble
		// (for 1bpp and 4bpp cases)

		// next loop through the width of this row
		for(int i=0;i<nWidth;i++) {
			switch(m_nSrcBitsPerPixel) {
			case 1:
				if ((cSrc>>(7-nBitSrc))&0x01) {
					cDest |= 1<<(7-nBitDest);
				}

				// figure out if our source has gone beyond its limit
				if(nBitSrc>=7) {
					nBitSrc=0;
					dwSrcOffset++;
					cSrc = m_lpSrcBits[dwSrcOffset];
				} else {
					nBitSrc++;
				}
				// finish up writing the last data to the destination
				if (i+1==nWidth || nBitDest>=7) {
					// finish up writing the last data to dest
					m_lpSrcBits[dwDestOffset++]=cDest;
					nBitDest=0;
					cDest=0x0;
				} else {
					nBitDest++;
				}
				break;
			case 4:
				// Problem copying wrong bits in a 4bpp image while cropping.
				// Fix: 4/7/99
				if (nBitSrc==0)
				//	cTmp = (UCHAR)(0x0f&cSrc);
					cTmp = (UCHAR)(0x0f & cSrc >> 4);	// Fix for cropping problem
				else
				//	cTmp = (UCHAR)(0x0f & cSrc>>4);
					cTmp = (UCHAR)(0x0f & cSrc);		// Fix for cropping problem
				// cDest |= (UCHAR)(cTmp<<(4*nBitDest));
				cDest |= (nBitDest ? (UCHAR)cTmp : (UCHAR)(cTmp<<4));	// Fix for cropping problem
				if (nBitSrc>=1) {
					nBitSrc=0;
					dwSrcOffset++;
					cSrc=m_lpSrcBits[dwSrcOffset];
				} else {
					nBitSrc++;
				}

				if (nBitDest>=1) {
					m_lpSrcBits[dwDestOffset++]=cDest;
					nBitDest=0;
					cDest=0x0;
				} else {
					nBitDest++;
				}

				break;
			case 8:
				m_lpSrcBits[dwDestOffset++] = m_lpSrcBits[dwSrcOffset++];
				break;
			default:
				m_lpSrcBits[dwDestOffset++] = m_lpSrcBits[dwSrcOffset++];
				m_lpSrcBits[dwDestOffset++] = m_lpSrcBits[dwSrcOffset++];
				m_lpSrcBits[dwDestOffset++] = m_lpSrcBits[dwSrcOffset++];
				break;
			}
		}
		// Fix for 4bpp cropping problem
		if(m_nSrcBitsPerPixel == 4)
		{
			if(nBitDest)
			{
				m_lpSrcBits[dwDestOffset++]=cDest;
				nBitDest=0;
				cDest=0x0;
			}
		}

	}


    GlobalFreePtr(lptemp);
    // Reset the image member height and width to cropped values.
    if (nHeight<(long)m_dwHeight)
		m_dwHeight = nHeight;
    if (nWidth<(long)m_dwWidth)
		m_dwWidth = nWidth;
    m_dwPadWidth = dwDestUsePad;

	// Fix for 8bpp cropping problem
	// The resulatant bmp will be padded, so set the flag TRUE.
	m_bIsPadded = TRUE;
    FlipVert();
    // adjust m_lpBMI parameters
    m_lpBMI->bmiHeader.biWidth = nWidth;
    m_lpBMI->bmiHeader.biHeight = nHeight;
    m_lpSrcBits = (LPBYTE)GlobalReAllocPtr(m_lpSrcBits, m_dwPadWidth*m_dwHeight*nBPP, GMEM_MOVEABLE);
	return TRUE;
}

//
//	FlipHorz
//
//
//	Flip the image horizontally.
//
//@doc SECImage
//@mfunc Flips the image horizontally.
//@rdesc Nonzero if successful; otherwise 0.
//@comm All pixels in the image will be "mirrored" over horizontally on the same row.
// For example, if an image is 200 pixels wide and FlipHorz is called, the pixel which
// was in the first column will be swapped with the pixel in the last column,
// the pixel in the second column will be swapped with the pixel second to the
// last column, etc.
//@xref <c SECImage> <mf SECImage::ConvertImage> <mf SECImage::FlipVert>
// <mf SECImage::Rotate90> <mf SECImage::Crop>
BOOL SECImage::FlipHorz()
{
    ASSERT_VALID(this);

    DWORD dwLeft, dwRight, dwRow, dwColAdjust=0, dwBPP=0, dwUsePad, dwBit;
    BYTE cHolder;

    // dwBPP is used as an adjustor for how many bytes we should
    // copy.
	dwBPP = (int)(m_nSrcBitsPerPixel / 8);

    // dwUsePad used if the image data is padded or not
	dwUsePad = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);
    ASSERT(dwUsePad>0);

    // Cycle through a row at a time, moving the bytes on the right
	// side of the line to the left side, and also reversing the
	// order of the bits in each byte.
	if (m_nSrcBitsPerPixel==1) {
		LPBYTE lptemp = (LPBYTE)GlobalAllocPtr(GHND, dwUsePad);

		DWORD dwBytesUsed = (DWORD)LastByte(m_nSrcBitsPerPixel, m_dwWidth);
		int nBitRight,nBitLeft,nCount,nRight;
		BYTE cSrc,cDest;
		for (dwRow=0; dwRow<m_dwHeight; dwRow++) {

			// Take care of the "leftover" bits
			nBitRight = (int)(dwBytesUsed*8-m_dwWidth);
			nBitLeft = 0;
			cDest = 0x0;
			DWORD dwDestByteOffset = 0;
			for (nRight=(int)dwBytesUsed-1;nRight>=0; nRight--) {
				DWORD dwOffset = dwRow*dwUsePad + (DWORD)nRight;
				cSrc=m_lpSrcBits[dwOffset];
				for(nCount=nBitRight; nCount<8; nCount++) {
					if ((cSrc>>(nCount)) & 0x01)
						cDest |= 1<<(7-nBitLeft);
					if (nBitLeft>=7) {
						lptemp[dwDestByteOffset++]=cDest;
						nBitLeft=0;
						cDest=0x0;
					} else {
						nBitLeft++;
					}
				}
				nBitRight=0;
			}
			// transfer the last byte
			if (nBitLeft >0)
				lptemp[dwDestByteOffset]=cDest;

			// copy back the whole line
			_fmemcpy(m_lpSrcBits + dwRow*dwUsePad,
				lptemp,
				(size_t)dwUsePad);


		}
		if (lptemp) GlobalFreePtr(lptemp);

	} else if (m_nSrcBitsPerPixel==4) {
		LPBYTE lptemp = (LPBYTE)GlobalAllocPtr(GHND, dwUsePad);

		DWORD dwBytesUsed = (DWORD)LastByte(m_nSrcBitsPerPixel, m_dwWidth);
		int nNibbleRight,nNibbleLeft,nCount,nRight,nLeft=0;
		BYTE cSrc,cDest,cTmp;
		for (dwRow=0; dwRow<m_dwHeight; dwRow++) {
			// Take care of the "leftover" bits
			nNibbleRight = (int)(dwBytesUsed*2-m_dwWidth);
			nLeft= nNibbleLeft = 0;
			cDest = 0x0;
			for (nRight=(int)dwBytesUsed-1;nRight>=0; nRight--) {
				cSrc=m_lpSrcBits[dwRow*dwUsePad + (DWORD)nRight];
				for(nCount=nNibbleRight; nCount<2; nCount++) {
					if (nCount==0)
						cTmp = (UCHAR)(0x0f & cSrc);
					else
						cTmp = (UCHAR)(0x0f & cSrc>>4);
					cDest |= cTmp<<(4-4*nNibbleLeft);
					if (nNibbleLeft>=1) {
						lptemp[nLeft++]=cDest;
						nNibbleLeft=0;
						cDest=0x0;
					} else {
						nNibbleLeft++;
					}
				}
				nNibbleRight=0;
			}

			// transfer last byte
			if (nNibbleLeft > 0)
				lptemp[nLeft]=cDest;


			// copy back the whole line
			_fmemcpy(m_lpSrcBits + dwRow*dwUsePad,
				lptemp,
				(size_t)dwUsePad);
		}
		if (lptemp) GlobalFreePtr(lptemp);

	} else {
    for (dwRow = 0; dwRow<m_dwHeight; dwRow++) {
	for (dwLeft = 0, dwRight=m_dwWidth*dwBPP+dwColAdjust-dwBPP; dwLeft < dwRight; dwLeft+=dwBPP, dwRight-=dwBPP) {
	    for (dwBit=0; dwBit<dwBPP; dwBit++) {
		cHolder = m_lpSrcBits[dwColAdjust*dwRow + dwRow*dwUsePad+ dwLeft + dwBit];
		m_lpSrcBits[dwColAdjust*dwRow + dwRow*dwUsePad+ dwLeft + dwBit] = m_lpSrcBits[dwColAdjust*dwRow + dwRow*dwUsePad+ dwRight + dwBit];
		m_lpSrcBits[dwColAdjust*dwRow + dwRow*dwUsePad+ dwRight + dwBit] = cHolder;
	    }
	}
    }
	}
    return (TRUE);

}

//
//	FlipVert
//
//
//	Flip the image vertically.
//
//@doc SECImage
//@mfunc Flips the image vertically.
//@rdesc Nonzero if successful; otherwise 0.
//@comm All pixels in the image will be "mirrored" over vertically on the same column.
// The image rows will be swapped using the middle row as the axis to flip around.
//@xref <c SECImage> <mf SECImage::ConvertImage> <mf SECImage::FlipHorz>
// <mf SECImage::Rotate90> <mf SECImage::Crop>
BOOL SECImage::FlipVert()
{
    ASSERT_VALID(this);

    DWORD dwTop, dwBottom, dwUsePad;

	if (m_bIsPadded == FALSE)
		PadBits();

	dwUsePad = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);

    LPBYTE lptemp = (LPBYTE)GlobalAllocPtr(GHND, sizeof(BYTE)*dwUsePad);
    if(!lptemp)
	{
		TRACE(_T("SECImage::FlipVert could not allocate memory for flip\n"));
		return FALSE;
	}

    // enough space has already been allocated for the bit array to
    // include the padding, so we just need to shift rows around.
    for (dwTop=0, dwBottom=m_dwHeight-1; dwTop<dwBottom; dwTop++, dwBottom--)
	{
	    _fmemcpy(lptemp, (LPBYTE)(m_lpSrcBits + (dwTop*dwUsePad)), (size_t)(dwUsePad));
	    _fmemcpy((LPBYTE)(m_lpSrcBits+(dwTop*dwUsePad)),
		     (LPBYTE)(m_lpSrcBits+(dwBottom*dwUsePad)),
		     (size_t)(dwUsePad));
	    _fmemcpy((LPBYTE)(m_lpSrcBits + (dwBottom*dwUsePad)), lptemp, (size_t)(dwUsePad));
	}
    GlobalFreePtr(lptemp);

    return (TRUE);
}

void
Contrast(int sign, RGBQUAD *pRGB)
{
    ASSERT(pRGB !=0);
    double brightness, hue, minimum, saturation, theta;

    // register double b, g, r;
    double b, g, r;

    // Convert RGB to HSV colorspace.

    hue=(-1.0);
    saturation=0.0;
    r=(double) (pRGB->rgbRed)/(double) MaxRGB;
    g=(double) (pRGB->rgbGreen)/(double) MaxRGB;
    b=(double) (pRGB->rgbBlue)/(double) MaxRGB;
    if (r >= g)
    {
	if (r >= b)
	    brightness=r;
	else
	    brightness=b;
    }
  else
      if (g >= b)
	  brightness=g;
      else
	  brightness=b;
    if (r <= g)
    {
	    if (r <= b)
		minimum=r;
	    else
		minimum=b;
    }
    else
	if (g <= b)
	    minimum=g;
	else
	    minimum=b;
    if (brightness != 0.0)
	saturation=(brightness-minimum)/brightness;
    if (saturation != 0.0)
    {
	if (r == brightness)
	    hue=(g-b)/(brightness-minimum);
	else
	    if (g == brightness)
		hue=2.0+(b-r)/(brightness-minimum);
	    else
		if (b == brightness)
		    hue=4.0+(r-g)/(brightness-minimum);
	hue=hue*60.0;
	if (hue < 0.0)
	    hue+=360.0;
    }
    // Enhance contrast: dark color become darker, light color become lighter.
    theta=(brightness-0.5)*M_PI;
    brightness+=(((((sin(theta)+1.0))*0.5)-brightness)*sign)*0.5;
    if (brightness > 1.0)
	brightness=1.0;
    else
	if (brightness < 0)
	    brightness=0.0;
    // Convert HSV to RGB

    r=brightness;
    g=brightness;
    b=brightness;
    if ((hue != -1.0) && (saturation != 0.0))
    {
	double f, j, k, l, v;

	long i;

	if (hue == 360.0)
	    hue=0.0;
	hue=hue/60.0;
	i=(long)floor(hue);
	if (i < 0)
	    i=0;
	f=hue-i;
	j=brightness*(1.0-saturation);
	k=brightness*(1.0-(saturation*f));
	l=brightness*(1.0-(saturation*(1.0-f)));
	v=brightness;
	switch (i)
	    {
	    case 0:  r=v;  g=l;  b=j;  break;
	    case 1:  r=k;  g=v;  b=j;  break;
	    case 2:  r=j;  g=v;  b=l;  break;
	    case 3:  r=j;  g=k;  b=v;  break;
	    case 4:  r=l;  g=j;  b=v;  break;
	    case 5:  r=v;  g=j;  b=k;  break;
	    }
    }
    pRGB->rgbRed = (BYTE)floor((r* (double)MaxRGB)+0.5);
    pRGB->rgbGreen=(BYTE) floor((g* (double)MaxRGB)+0.5);
    pRGB->rgbBlue =(BYTE) floor((b* (double)MaxRGB)+0.5);
}

//@doc SECImage
//@mfunc Modifies the contrast of an image.
//@comm Contrast of the image is increased or decreased by the percentage indicated by
// nSharpen.  Valid ranges are from -99 to +99.
//@devnote There is no guarantee that contrasted image data can be restored to its
// original state by applying the opposite contrast value.
//@parm short | nSharpen | Percentage of contrast increase/decrrease.
//@xref <c SECImage> <mf SECImage::ConvertImage> <mf SECImage::FlipVert>
// <mf SECImage::Rotate90> <mf SECImage::Crop>
void SECImage::ContrastImage(short nSharpen)
{
	ASSERT_VALID(this);
    WORD wColor, wRow, wCol;
    int	sign;

    RGBQUAD rgbPix;
#ifdef WIN32
    LPBYTE lpPixel;
#else
    BYTE huge *lpPixel;
#endif
    BOOL bPrePadded = m_bIsPadded;

    if (!bPrePadded)
	UnPadBits();

    if (nSharpen>0)
	sign = 1;
    else
	sign = -1;

    switch (m_nSrcBitsPerPixel) {
    case 4:
    case 8:
	// Case 8 only modifies the color map and then regenerates
	// the palette.  User's code will need to make sure that the
	// new palette is selected during the next rendering phase
	for(wColor = 0; wColor<m_wColors; wColor++) {
	    Contrast(sign, &m_lpRGB[wColor]);
	}
	// Now create new palette based on the adjusted vals
	CreatePalette();
	break;
    case 24:
	for (wRow=0; wRow<m_dwHeight; wRow++)
	    for (wCol=0; wCol<m_dwWidth; wCol++)
		{
		    lpPixel = (m_lpSrcBits + (3*(wRow*wCol + wCol)));
		    rgbPix.rgbRed = (BYTE)*lpPixel;
		    rgbPix.rgbGreen= (BYTE)*(lpPixel+1);
		    rgbPix.rgbBlue= (BYTE)*(lpPixel+2);
		    Contrast(sign, &rgbPix);
		    *lpPixel = (BYTE)rgbPix.rgbRed;
		    *(lpPixel+1) = (BYTE)rgbPix.rgbGreen;
		    *(lpPixel+2) = (BYTE)rgbPix.rgbBlue;
		}
	break;
    case 32:
	for (wRow=0; wRow<m_dwHeight; wRow++)
	    for (wCol=0; wCol<m_dwWidth; wCol++)
		{
		    lpPixel = (m_lpSrcBits + (3*(wRow*wCol + wCol)));
		    rgbPix.rgbRed = (BYTE)*lpPixel;
		    rgbPix.rgbGreen= (BYTE)*(lpPixel+1);
		    rgbPix.rgbBlue= (BYTE)*(lpPixel+2);
		    Contrast(sign, &rgbPix);
		    *lpPixel = (BYTE)rgbPix.rgbRed;
		    *(lpPixel+1) = (BYTE)rgbPix.rgbGreen;
		    *(lpPixel+2) = (BYTE)rgbPix.rgbBlue;
		}
	break;
    default:
	break;
    }

    if (bPrePadded)
	PadBits();
}

