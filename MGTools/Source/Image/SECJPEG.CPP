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
//  Description:	Implementation of SECJpeg
//  Created:		7/25/95
//
//  CHANGELOG
//
//     MSW     9/30/95    Code Review


// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _CMNDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_CMNDLL

#ifndef __SECJPEG_H__
#include "..\..\Include\Image\secjpeg.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif

#ifdef _CMNDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_CMNDLL


IMPLEMENT_SERIAL(SECJpeg, CObject, 1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

#ifdef WIN32
typedef LPBYTE* LPPBYTE;
typedef LPBYTE LPBITS;
#else
typedef LPBYTE FAR* LPPBYTE;
typedef BYTE huge * LPBITS;
#endif

//@doc SECJpeg
//@mfunc Constructs a SECJpeg object.
//@xref <c SECJpeg>
SECJpeg::SECJpeg()
{
	if (m_wSysColors > 0 )
		m_bQuantize = TRUE; 
	m_lpQuant = 0;
	m_nQuality = 75;
	m_nSmoothing = 0;
	m_bOptimizeEntropyCoding = FALSE;
	m_bArithmeticCoding = FALSE; // To use arithmetic or Huffman
	m_dwSizeSrcBits=0;
}


SECJpeg::~SECJpeg()
{

}  

void SECJpeg::SECoutput_init(j_decompress_ptr cinfo)
{
	ASSERT(cinfo != 0);    
    m_dwWidth = cinfo->image_width;
    m_dwHeight = cinfo->image_height;
    long lOffset = 1;

	if (m_wSysColors > 0)
		m_nSrcBitsPerPixel = 8;
	else
		m_nSrcBitsPerPixel = 24; 

	if (cinfo->jpeg_color_space == JCS_GRAYSCALE && m_nSrcBitsPerPixel == 24)
		lOffset = 3;

    // m_wColors = (WORD)cinfo->desired_number_of_colors;
    m_wColors = m_wSysColors;
	//AAB 
	if (m_wColors < 256 && m_wColors > 0)
		m_wColors = 256;
    // now allocate the bitmap info structures since you know color
    m_lpBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + m_wColors*sizeof(RGBQUAD));
    if (!m_lpBMI)
	return;
    m_lpBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_lpBMI->bmiHeader.biWidth = (long)m_dwWidth;
    m_lpBMI->bmiHeader.biHeight = (long)m_dwHeight;
    m_lpBMI->bmiHeader.biBitCount = (WORD)m_nSrcBitsPerPixel; // cinfo->data_precision;
    m_lpBMI->bmiHeader.biCompression = BI_RGB;
    m_lpBMI->bmiHeader.biPlanes = 1;
    m_lpBMI->bmiHeader.biSizeImage = 0;
    m_lpBMI->bmiHeader.biXPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biYPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biClrUsed = 0 /*AAB*/;
    m_lpBMI->bmiHeader.biClrImportant = 0 /*AAB*/;
    m_lpRGB = (LPRGBQUAD)(m_lpBMI->bmiColors);
	//m_dwPadWidth = CalcBytesPerLine((DWORD)m_nSrcBitsPerPixel, m_dwWidth);
	m_dwPadWidth = PADWIDTH(m_dwWidth);
    
#ifdef WIN32
//	m_dwSizeSrcBits = lOffset*m_dwPadWidth*m_dwHeight;
	m_dwSizeSrcBits = lOffset*((m_dwPadWidth * m_dwHeight* m_lpBMI->bmiHeader.biBitCount) / 8);
    m_lpSrcBits = (LPBYTE)GlobalAllocPtr(GHND,m_dwSizeSrcBits);  
#else 
	m_dwSizeSrcBits = lOffset*(long)cinfo->num_components*(long)m_dwPadWidth*(long)m_dwHeight;
    m_lpSrcBits = (BYTE huge *)GlobalAllocPtr(GHND,m_dwSizeSrcBits);
#endif
	if (!m_lpSrcBits)
	{
		GlobalFreePtr(m_lpRGB);
		return;
	}
        
}

BOOL SECJpeg::DoLoadImage(CFile* pFile)
{
	struct jpeg_decompress_struct cinfo;

	JSAMPARRAY buffer;
	int row_stride;
	int nResult = 0, i=0;

	struct jpeg_error_mgr pub;

	cinfo.err = jpeg_std_error(&pub);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, pFile);

	// read in header info
	nResult = jpeg_read_header(&cinfo, TRUE);
	if (!nResult)
		return FALSE;
	//initialize and allocate memory
	SECoutput_init(&cinfo);

	if (m_wSysColors > 0 && m_bQuantize == TRUE)
	{
		cinfo.desired_number_of_colors = 256;
		cinfo.quantize_colors = TRUE;
	}
	jpeg_start_decompress(&cinfo);
	row_stride = cinfo.output_width * cinfo.output_components;
	buffer = (this->*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);


#ifdef WIN32
	BYTE *pBits = m_lpSrcBits;
#else
	BYTE huge *pBits = m_lpSrcBits;
#endif
	if (m_nSrcBitsPerPixel == 24 && cinfo.jpeg_color_space == JCS_GRAYSCALE)
	{
		int j;
		BYTE *pTmp;

		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);
			pTmp = (BYTE *)buffer[0];
			//copy buffer into image memory
			for (j=0; j<row_stride; j++)
			{
				pBits[j*3] =
				pBits[j*3 +1] =
				pBits[j*3 +2] = (BYTE)pTmp[j];
			}
			pBits += 3*row_stride;
		}
	}
	else {

		DWORD dwBitsLeft=m_dwSizeSrcBits;
		while(cinfo.output_scanline < cinfo.output_height)
			{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			//copy buffer into image memory
			if((UINT)row_stride > dwBitsLeft) {
				// handle error condition gracefully
#ifdef WIN32
				_fmemcpy(pBits, buffer[0], dwBitsLeft);
#else
				_fmemcpy(pBits, buffer[0],(size_t)dwBitsLeft);
#endif
				pBits+=dwBitsLeft;
				break;
				}
			else {
				_fmemcpy(pBits, buffer[0], row_stride);
				pBits += row_stride;
				dwBitsLeft-=row_stride;
				}
			}
		}

	for (i=0; i<cinfo.actual_number_of_colors; i++)
	{
		if (cinfo.num_components == 3)
		{
			m_lpRGB[i].rgbRed = cinfo.colormap[2][i];
			m_lpRGB[i].rgbGreen = cinfo.colormap[1][i];
			m_lpRGB[i].rgbBlue = cinfo.colormap[0][i];
		}
		else
		{
			m_lpRGB[i].rgbRed =
			m_lpRGB[i].rgbGreen =
			m_lpRGB[i].rgbBlue = cinfo.colormap[0][i];
		}
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

    FlipVert();
    PadBits();
	return TRUE;

	
}


BOOL SECJpeg::DoSaveImage(CFile* pFile)
{
    struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
  /* More stuff */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride=0;		/* physical row width in image buffer */




	ASSERT_VALID(this);   
	FlipVert();
	UnPadBits();

	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);
	jpeg_stdio_dest(&cinfo, pFile);

	cinfo.image_width = (int)m_dwWidth; 	/* image width and height, in pixels */
	cinfo.image_height = (int)m_dwHeight;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
	jpeg_set_defaults(&cinfo);
	jpeg_start_compress(&cinfo, TRUE);

	// Must convert image formats with <= 8 bits per pixel to
	// 24 bpp before writing...
	if (m_nSrcBitsPerPixel <= 8)
	{
		char *pRow = (char *)GlobalAllocPtr(GHND, m_dwWidth*3);
		BYTE *pHolder = m_lpSrcBits;

		int nPixel;
		BYTE cPixel;
		while (cinfo.next_scanline < cinfo.image_height) 
		{
			for(nPixel=0; nPixel<(int)m_dwWidth; nPixel++)
			{
				NEXT_IMAGE_PIXEL(cPixel, pHolder);
				pRow[3*nPixel] = m_lpRGB[cPixel].rgbBlue;
				pRow[3*nPixel+1] = m_lpRGB[cPixel].rgbGreen;
				pRow[3*nPixel+2] = m_lpRGB[cPixel].rgbRed;
			}
			row_pointer[0] = pRow;
			(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
		}
		if (pRow)
			GlobalFreePtr(pRow);
	}
	else
	{
		int j=0;
		char *pOrig, *pWrite;
		
		char *pRow = (char *)GlobalAllocPtr(GHND, m_dwWidth*3);
		row_stride = (int)m_dwWidth*3;

		while (cinfo.next_scanline < cinfo.image_height) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		* Here the array is only one element long, but you could pass
		* more than one scanline at a time if that's more convenient.
		*/
			if (m_nSrcBitsPerPixel == 24) 
			{
				pOrig = (char *)(&m_lpSrcBits[cinfo.next_scanline*row_stride]);
				pWrite = row_pointer[0];
				for (j=0; j<row_stride; j++) 
					pRow[j] = *pOrig++;
				row_pointer[0] = pRow;
				(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

			} else
			{
				row_pointer[0] = (char *)(& m_lpSrcBits[cinfo.next_scanline * row_stride]);
				(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}
		}
		if (pRow)
			GlobalFreePtr(pRow);

	}


  jpeg_finish_compress(&cinfo);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);




    // Make sure we are storing as a continuous stream of data
  PadBits();
  FlipVert(); 


	return TRUE;
}

 
 
 


void SECJpeg::Serialize(CArchive& ar)
{     
    SECImage::Serialize(ar);
}           

#ifdef _DEBUG
void SECJpeg::AssertValid() const
{
    CObject::AssertValid();
	SECImage::AssertValid();
	// don't need to verify specific elements of SECJpeg class

}
#endif /* _DEBUG */

