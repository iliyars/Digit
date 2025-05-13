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
//  Description:	Implementation of SECTarga
//  Created:		Tue Aug 29 15:11:10 1995
//
//  CHANGELOG:
//
//    MSW     9/30/95   Code Review



// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __SECTARGA_H__
#include "..\..\Include\Image\sectarga.h"
#endif

#ifndef _INC_WINDOWSX
#include <windowsx.h> 
#endif

//TODO
#ifdef AFX_COLL_SEG
#pragma code_seg(AFX_COLL_SEG)
#endif


#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL


IMPLEMENT_SERIAL(SECTarga, CObject,1)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


static const BYTE c5to8bits[32] = {
    0,   8,  16,  25,  33,  41,  49,  58,
   66,  74,  82,  90,  99, 107, 115, 123,
  132, 140, 148, 156, 165, 173, 181, 189,
  197, 206, 214, 222, 230, 239, 247, 255
};


//@doc SECTarga
//@mfunc Constructs a SECTarga object
//@xref <c SECTarga>
SECTarga::SECTarga()
{
	m_TgaHead.Cmap.IdField = NULL;
	m_TgaHead.Cmap.CmapData = NULL;
	m_TgaHead.Extension.ScanLineTable = NULL;

}

SECTarga::~SECTarga()
{
        
	if (m_TgaHead.Cmap.IdField != NULL)
		free (m_TgaHead.Cmap.IdField);
	if (m_TgaHead.Cmap.CmapData != NULL)
		free (m_TgaHead.Cmap.CmapData);
	if (m_TgaHead.Extension.ScanLineTable != NULL)
		free (m_TgaHead.Extension.ScanLineTable);

}

void SECTarga::MakeBMI()
{
    if (m_lpBMI)
	GlobalFreePtr(m_lpBMI);
    
    m_nSrcBitsPerPixel = 24; //AAB: Targa always unrolled to 24
    
    switch (m_nSrcBitsPerPixel) {
    case 24:
    default:
	m_wColors = 0;
	break;
    }
    m_dwWidth = m_TgaHead.Head.ImageWidth;
	m_dwPadWidth = PADWIDTH(m_dwWidth);
    m_dwHeight = m_TgaHead.Head.ImageHeight;
    
    
    // now allocate the bitmap info structures since you know color
    m_lpBMI = (LPBITMAPINFO)GlobalAllocPtr(GHND, sizeof(BITMAPINFOHEADER) + m_wColors*sizeof(RGBQUAD));
    if (!m_lpBMI)
	{
		TRACE(_T("SECTga::MakeBMI could not allocate colormap\n"));
		return;
	}
    
    // Now we can fill out some of the header info
    m_lpBMI->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_lpBMI->bmiHeader.biWidth = (long)m_dwWidth;
    m_lpBMI->bmiHeader.biHeight = (long)m_dwHeight;
    m_lpBMI->bmiHeader.biPlanes = 1;
    m_lpBMI->bmiHeader.biBitCount = 24; //AAB Targa always unrolled to 24
    m_lpBMI->bmiHeader.biCompression = BI_RGB;
    m_lpBMI->bmiHeader.biSizeImage = 0;
    m_lpBMI->bmiHeader.biXPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biYPelsPerMeter = 0;
    m_lpBMI->bmiHeader.biClrUsed = 0;
    m_lpBMI->bmiHeader.biClrImportant = 0; 
    m_lpRGB = (LPRGBQUAD)(m_lpBMI->bmiColors);    
}


BOOL SECTarga::DoLoadImage(CFile* pFile)
{
    long nImageSize=0, nBytes;
	long tmp;
    WORD wRow;
    
    if (ReadTgaHeader())
	return FALSE;
    MakeBMI();
    // Otherwise we've read in a spanking new Targa file, let's setup
    // our internal support
#ifdef WIN32
    m_lpSrcBits = (LPBYTE)GlobalAllocPtr(GHND,
		m_dwPadWidth*m_dwHeight*3);//AAB: Targa always unrolled to 24 bits
#else
    DWORD dwSpace = m_dwPadWidth*m_dwHeight*(DWORD)3;//AAB Targa unrolls to 24
    m_lpSrcBits = (BYTE huge *)GlobalAllocPtr(GHND, dwSpace);
#endif
    if (!m_lpSrcBits) 
	{
		TRACE(_T("SECTga::LoadImage could not allocate space for m_lpSrcBits\n"));
		return FALSE;
	}
    // move file pointer to data portion
    pFile->Seek(m_TgaHead.TgaDataOffset, CFile::begin);
    
    InitCache(60000, SEC_READ_CACHE);
    // Read in a row at a time
    if (m_TgaHead.Head.ImageType==9 || m_TgaHead.Head.ImageType == 10 || m_TgaHead.Head.ImageType==11) {
	nImageSize = 0;  
	
	long nBufSize =  m_TgaHead.Head.ImageWidth + m_TgaHead.Head.ImageWidth*(long)TGA_BPP;
#ifdef WIN32
	LPBYTE pTempBuf = (LPBYTE)GlobalAllocPtr(GHND, nBufSize);
#else
	BYTE huge *pTempBuf = (BYTE huge *)GlobalAllocPtr(GHND, nBufSize);
#endif
	if (!pTempBuf)
	    goto targa_err;	



	for (wRow=0; wRow<(m_TgaHead.Head.ImageHeight); wRow++) {
		nBytes = TgaDecodeScanLine(pTempBuf, m_TgaHead.Head.ImageWidth, (WORD)TGA_BPP, 0);
		_fmemcpy(m_lpSrcBits+((DWORD)m_TgaHead.Head.ImageWidth*(DWORD)TGA_BPP * (DWORD)wRow),
			pTempBuf, (size_t)nBytes);
	    if (nBytes<0)
		goto targa_err;
	    nImageSize += nBytes;
	}
    }
    // otherwise image is unencoded
    else if (m_TgaHead.Head.ImageType==1 || m_TgaHead.Head.ImageType == 2 || m_TgaHead.Head.ImageType==3) 
    {
	
	// In this case we allocate a temporary buffer big enough to include packet info,
	// and will fill our m_lpSrcBits structure after it rolls out the data
	long nBufSize =  m_TgaHead.Head.ImageWidth + m_TgaHead.Head.ImageWidth*(long)TGA_BPP;
#ifdef WIN32
	LPBYTE pTempBuf = (LPBYTE)GlobalAllocPtr(GHND, nBufSize);
#else
	BYTE huge *pTempBuf = (BYTE huge *)GlobalAllocPtr(GHND, nBufSize);
#endif
	if (!pTempBuf)
	    goto targa_err;

	// iterate through all the rows of pixel data filling up the buffer row by row
	LPBYTE pBuf = m_lpSrcBits;
	BYTE b1,b2;
	int i;
	for (wRow=0; wRow<m_TgaHead.Head.ImageHeight; wRow++) {
		switch(TGA_BPP)
		{
		case 3:
			for (i=0;i<(int)(TGA_BPP*m_TgaHead.Head.ImageWidth);i++) 
			{
				if ((GetCache(&b1,1))!=1)
					goto targa_err;
				*pBuf++ = b1;
			}
			break;
		case 2:
			for (i=0;i<(int)(m_TgaHead.Head.ImageWidth);i++) 
			{
				if ((GetCache(&b1,1))!=1)
					goto targa_err;
				if ((GetCache(&b2,1))!=1)
					goto targa_err;

				short n = (short)(b1 + (b2<<8));
				*pBuf++ = c5to8bits[n&0x1F];
				n >>=5; 
				*pBuf++ = c5to8bits[n&0x1F];
				n >>=5;
				*pBuf++ = c5to8bits[n&0x1F];
			}
			break;
		}


#ifdef ORIG
	    nBytes = (long)TgaEncodeScanLine(pTempBuf, (WORD)nBufSize, m_TgaHead.Head.ImageWidth,(WORD)TGA_BPP, 0);
	    if (nBytes<0)
		goto targa_err;
	    nImageSize += nBytes;
	    long nCol=0;
	    // AAB TODO: Logic for 32 and 16 bit formats; comment this de-roller 

	    for (long nPlace=0; nPlace<nBytes; ) {
		char cRunMax = pTempBuf[nPlace];
		if (cRunMax & 0x80) {
		    cRunMax = (BYTE)(cRunMax & 0x7f);// screen out MSB
		    nPlace++;
		    for (long nRunCount=0; nRunCount<cRunMax+1; nRunCount++) 
			for (long nColor=0; nColor < TGA_BPP; nColor++) 
			{ 
				long check = (long)wRow*(long)TGA_BPP*(long)m_TgaHead.Head.ImageWidth + (long)nCol++;
			    *(m_lpSrcBits + check) = pTempBuf[nPlace+nColor];
		    }
		    nPlace += TGA_BPP;
		}
		else {
		    nPlace++;
		    long nLimit = cRunMax+1;
		    for (long nRunCount=0; nRunCount<nLimit; nRunCount++) {
			for (long nColor=0; nColor < TGA_BPP; nColor++) { 
			    long check = (long)wRow*(long)TGA_BPP*(long)m_TgaHead.Head.ImageWidth + (long)nCol;
			    *(m_lpSrcBits + check) = (BYTE)pTempBuf[nPlace+nColor]; 
			    nCol++;
			}
			nPlace+= TGA_BPP;
		    }
		}
	    }

#endif //ORIG
	}

	// cleanup
	GlobalFreePtr(pTempBuf);
    } 
    PadBits();
    FreeCache();
    
	// Make sure the read-in image is compatible with the intermediate DIB format
	tmp = (long)m_TgaHead.Head.ImagDesc;
	if (tmp & 0x20)
    	FlipVert();
	if (tmp  & 0x10)
		FlipHorz();

	  
    return TRUE;

 targa_err:
    TRACE(_T("Could not load in TGA file due to lack of memory\n"));
    if (m_lpSrcBits)	
	{
	GlobalFreePtr(m_lpSrcBits);
	m_lpSrcBits = NULL;
	}
    return FALSE;
}


BOOL SECTarga::DoSaveImage(CFile* pFile)
{
    pFile; // UNUSED

    ASSERT_VALID(this);
	    
    // Initialize TGAHeader info based on information stored in the base
    // SECImage class
    
    m_TgaHead.Head.IdLength = 0;
    // We always use 24 bit...
    m_TgaHead.Head.CmapType = 0;
    m_TgaHead.Head.ImageType = 2;
    
    m_TgaHead.Head.CmapIndex = 0;
    m_TgaHead.Head.CmapLength = 0;
    m_TgaHead.Head.CmapEntrySize = 0;
    
    m_TgaHead.Head.X_Origin = 0;
    m_TgaHead.Head.Y_Origin = 0;
    m_TgaHead.Head.ImageWidth = (WORD)m_dwWidth;
    m_TgaHead.Head.ImageHeight = (WORD)m_dwHeight;
    
    // We always use 24 bits per pixel...
    m_TgaHead.Head.PixelDepth = 24;
    
    // TODO: Fill out bits 4&5 which specify the origin 
    // location of the image.  Default of 0 specifies lower left;
    // this should eventually be stored/specified via a data member
    m_TgaHead.Head.ImagDesc = 0;
    
    // Unpad bits for a continuous image stream
    UnPadBits();
    
    // TODO: Provide member elements for setting origin of image;
	// ie, to do a FlipVert or FlipHorz before writing out.
 
    WriteTgaFile();
    
    // Reset image back to intermediate DIB formate
    // AAB TODO: implement future origin overrideables
    PadBits();
    return TRUE;
}

//
//  Read the information from a TGA file info a TGAHEADER structure.
//
//  The TGAHEADER structure (defined in the file TGA.H) contains all of the
//  information found in both version 1.0 and 2.0 of the TGA format except
//  for the actual image data.  Image data is not stored because of its
//  typically large size.
//
//  Returns: A negative value if a memory allocation error occured,
//           otherwise 0 if no errors occur.
//
short
SECTarga::ReadTgaHeader()
// TGA  *TgaHead;              /* Pointer to TGA header structure              */
// FILE *FpTga;                /* TGA image file input FILE stream             */
{
    WORD    i, j;           /* Loop counters                                */
    WORD    cmapsize;       /* Size of the color map in bytes               */
    WORD    stampsize;      /* Size of the postage stamp image in bytes     */
    TGATAG *tag;            /* TGA tag structure                            */
    TGATAG *head;           /* Head of TGA tag linked list                  */
    //TGATAG *current_tag=NULL;    /* Current node of linked list                  */

    if (m_TgaHead.Cmap.IdField != NULL)
		free (m_TgaHead.Cmap.IdField);
	if (m_TgaHead.Cmap.CmapData != NULL)
		free (m_TgaHead.Cmap.CmapData);
	if (m_TgaHead.Extension.ScanLineTable != NULL)
		free (m_TgaHead.Extension.ScanLineTable);

    /*
    ** Read the TGA header (Version 1.0 and 2.0).
    */
    if ((m_pFile->Read(&m_TgaHead.Head.IdLength, 1)) != 1)
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.CmapType, 1)) != 1)
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.ImageType, 1)) != 1)
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.CmapIndex, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.CmapLength, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.CmapEntrySize, 1)) != 1)
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.X_Origin, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.Y_Origin, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.ImageWidth, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.ImageHeight, sizeof(WORD))) != sizeof(WORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.PixelDepth, 1)) != 1)
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Head.ImagDesc, 1)) != 1)
	return -1;
    
    /* Read the Image ID field. */
    if (m_TgaHead.Head.IdLength)
	{
	    /* Allocate memory for the Id Field data */
	    if ((m_TgaHead.Cmap.IdField =
		 (char *) calloc(m_TgaHead.Head.IdLength, sizeof(char))) ==
		(char *) NULL)
		{
		    return(-1);     /* Failed to allocate memory */
		}
	    
	    /* Read the Image ID data */
	    /* Do not assume that it is a NULL-terminated string */
	    for (i = 0; i < m_TgaHead.Head.IdLength; i++) {
		if ((m_pFile->Read(&m_TgaHead.Cmap.IdField[i], 1)) != 1)
		    return -1;
	    }
	}
    
    /*
    ** Read the color map data (Version 1.0 and 2.0).
    */
    if (m_TgaHead.Head.CmapType)
	{
	    /* Determine the size of the color map */
	    cmapsize = (WORD)(((m_TgaHead.Head.CmapEntrySize + 7) >> 3) *
			      m_TgaHead.Head.CmapLength);
	    
	    // AAB TODO : convert this to m_lpBMI stuff
	    /* Allocate memory for the color map data */
	    if ((m_TgaHead.Cmap.CmapData =
		 (BYTE *) calloc(cmapsize, sizeof(BYTE))) == (BYTE *) NULL)
		{
		    return(-2);     /* Failed to allocate memory */
		}
	    
	    /* Read the color map data */
	    for (i = 0; i < cmapsize; i++) {
		if ((m_pFile->Read(&m_TgaHead.Cmap.CmapData[i], 1)) != 1)
		    return -1;
	    }
	}
    
    /*
    ** Store the offset of the image data.  This field is not part of
    ** the TGA format, but it helps the software locate the data when
    ** reading/writing TGA files.
    */
    m_TgaHead.TgaDataOffset = m_pFile->GetPosition();
    
    /*
    ** Check the version of the TGA file (Versions 1.0 and 2.0).
    */
    // fseek(FpTga, -26, SEEK_END);  /* Seek to the (possible) TGA footer */
    m_pFile->Seek(-26, CFile::end);
    
    /* Read in the (possible) offset values */
    if ((m_pFile->Read(&m_TgaHead.Foot.ExtensionOffset, sizeof(DWORD))) != sizeof(DWORD))
	return -1;
    if ((m_pFile->Read(&m_TgaHead.Foot.DeveloperOffset, sizeof(DWORD))) != sizeof(DWORD))
	return -1;
    
    /* Read in the (possible) signature */
    for (i = 0; i < sizeof(m_TgaHead.Foot.Signature); i++) {
	if ((m_pFile->Read(&m_TgaHead.Foot.Signature[i], 1)) != 1)
	    return -1;
    }

    /* Check if the data read is a TGA signature string */
    if (!strcmp(m_TgaHead.Foot.Signature, TGASIGNATURE))
        m_TgaHead.NewTgaFormat = TRUE;   /* Yes.  Version 2.0 */
    else
        m_TgaHead.NewTgaFormat = FALSE;  /* No.  Version 1.0 */
    
    /*
    ** If version is 2.0 then check for developer and extension areas.
    */
    if (m_TgaHead.NewTgaFormat == TRUE)
    {
        /* Check for the presence of a developers area */
        if (m_TgaHead.Foot.DeveloperOffset)
	    {
		/* Seek to the developer area */
		m_pFile->Seek(m_TgaHead.Foot.DeveloperOffset, CFile::begin);

		/* Get the number of tags in the directory */
		if ((m_pFile->Read(&m_TgaHead.Developer.NumberOfTags, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		
		/* Read the tags and store as a singly-linked list */
		head = (TGATAG *) NULL;
		for (i = 0; i < m_TgaHead.Developer.NumberOfTags; i++)
		    {
			/* Allocate the tag */
			if ((tag =
			     (TGATAG *) calloc(1,sizeof(TGATAG))) == (TGATAG *) NULL)
			    return(-3);     /* Failed to allocate memory */

			/* Read the tag information */
			if ((m_pFile->Read(&tag->TagNumber, sizeof(WORD))) != sizeof(WORD))
					return -1;
			if ((m_pFile->Read(&tag->TagOffset, sizeof(DWORD))) != sizeof(DWORD))
			    return -1;
			if ((m_pFile->Read(&tag->TagSize, sizeof(DWORD))) != sizeof(DWORD))
			    return -1;
			
			/*
			** Read the tag data
			*/
			/* Seek to the tag data */
			m_pFile->Seek(tag->TagOffset, CFile::begin);
			
			/* Read the tag data */
			for (j = 0; j < tag->TagSize; j++){
				BYTE cHold;
			    // if ((m_pFile->Read(&tag->TagData[j], 1)) != 1)
				if ((m_pFile->Read(&cHold, 1)) != 1)
				return -1;
					}
			
			/* Link the tag to the list */
#ifdef AAB_NOT_SUPPORTING_TYPE_20
			if (head == (TGATAG *) NULL)
			    {
				head = tag;
				current_tag = head;
			    }
			else
			    {
				current_tag->Next = tag;
				tag->Next = (TGATAG *) NULL;
				current_tag = tag;
			    }
#endif
				// if (tag)
				//	free(tag);	//AAB: Until we implement freely, just keep null.
		    }
		m_TgaHead.Developer.TagList = head;  /* Assign list to structure */
		m_TgaHead.Developer.NumberOfTags = 0; //AAB: IN case we try to save...
	    }
	
        /* Check for the presence of an extension area */
        if (m_TgaHead.Foot.ExtensionOffset)
	    {
		/* Seek to the extension area */
		m_pFile->Seek(m_TgaHead.Foot.ExtensionOffset, CFile::begin);
		
		/* Read the extension area size information */
		if ((m_pFile->Read(&m_TgaHead.Extension.Size, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		
		/* Read the extension information defined in TGA version 2.0 */
		for (i = 0; i < sizeof(m_TgaHead.Extension.AuthorName); i++) {
		    if ((m_pFile->Read(&m_TgaHead.Extension.AuthorName[i], 1)) != 1)
			return -1;
		}
		
		for (i = 0; i < sizeof(m_TgaHead.Extension.AuthorComment); i++) {
		    if ((m_pFile->Read(&m_TgaHead.Extension.AuthorComment[i], 1)) != 1)
			return -1;
		}
		
		if ((m_pFile->Read(&m_TgaHead.Extension.StampMonth, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampDay, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampYear, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampHour, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampMinute, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampSecond, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		
		for (i = 0; i < sizeof(m_TgaHead.Extension.JobName); i++) {
		    if ((m_pFile->Read(&m_TgaHead.Extension.JobName[i], 1)) != 1)
			return -1;
		}
		
		if ((m_pFile->Read(&m_TgaHead.Extension.JobHour, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.JobMinute, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.JobSecond, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		
		for (i = 0; i < sizeof(m_TgaHead.Extension.SoftwareId); i++) {
		    if ((m_pFile->Read(&m_TgaHead.Extension.SoftwareId[i], 1)) != 1)
			return -1;
		}
		
		if ((m_pFile->Read(&m_TgaHead.Extension.VersionNumber, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.VersionLetter, 1)) != 1)
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.KeyColor, sizeof(DWORD))) != sizeof(DWORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.PixelNumerator, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.PixelDenominator, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.GammaNumerator, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.GammaDenominator, sizeof(WORD))) != sizeof(WORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.ColorOffset, sizeof(DWORD))) != sizeof(DWORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.StampOffset, sizeof(DWORD))) != sizeof(DWORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.ScanOffset, sizeof(DWORD))) != sizeof(DWORD))
		    return -1;
		if ((m_pFile->Read(&m_TgaHead.Extension.AttributesType, 1)) != 1)
		    return -1;
		
		/* Check for the presence of a scan line table */
		if (m_TgaHead.Extension.ScanOffset)
		    {
			/* Seek to the postage scan line table */
			m_pFile->Seek(m_TgaHead.Extension.ScanOffset, CFile::begin);
			
			/* Allocate memory.  One DWORD per line in image */
			if ((m_TgaHead.Extension.ScanLineTable =
			     (DWORD *) calloc(m_TgaHead.Head.ImageHeight, sizeof(DWORD))) ==
			    (DWORD *) NULL)
			    {
				return(-4);     /* Failed to allocate memory */
			    }
			
			/* Read in scan line offset values */
			for (i = 0; i < m_TgaHead.Head.ImageHeight; i++) {
			    if ((m_pFile->Read(&m_TgaHead.Extension.ScanLineTable[i], sizeof(DWORD))) != sizeof(DWORD))
				return -1;
			}
		    }
		
		/* Check for the presence of a postage stamp image*/
		if (m_TgaHead.Extension.StampOffset)
		    {
			/* Seek to the postage stamp image */
			m_pFile->Seek(m_TgaHead.Extension.StampOffset, CFile::begin);
			
			/* Read the size of the stamp in pixels */
			if ((m_pFile->Read(&m_TgaHead.Extension.StampWidth, 1)) != 1)
					return -1;
			if ((m_pFile->Read(&m_TgaHead.Extension.StampHeight, 1)) != 1)
			    return -1;
			
			/* Calculate the size of the stamp in bytes */
			stampsize = (WORD)(m_TgaHead.Extension.StampWidth *
					   m_TgaHead.Extension.StampHeight *
					   ((m_TgaHead.Head.PixelDepth + 7) >> 3));
			
			/* Allocate memory for the postage stamp image */
			if ((m_TgaHead.Extension.StampImage =
			     (BYTE *) calloc(stampsize, sizeof(BYTE))) ==
			    (BYTE *) NULL)
			    {
				return(-5);     /* Failed to allocate memory */
			    }
			
			/* Read the stamp data one byte at a time */
			for (i = 0; i < stampsize; i++) {
			    if ((m_pFile->Read(&m_TgaHead.Extension.StampImage[i], 1)) != 1)
				return -1;
			}
		    }
		
		/* Check for the presence of a color correction table */
		if (m_TgaHead.Extension.ColorOffset)
		    {
			/* Seek to the color correction table */
			m_pFile->Seek(m_TgaHead.Extension.ColorOffset, CFile::begin);
			
			/* Read in the entire 1024 WORD (2048 byte) table */
			for (i = 0; i < sizeof(m_TgaHead.Extension.ColorTable); i++){
			    if ((m_pFile->Read(&m_TgaHead.Extension.ColorTable[i], sizeof(WORD))) != sizeof(WORD))
				return -1;
			}
		    }
	    }
    }
    return(0);
}


/*
**  Write the information from a TGAHEADER structure to a TGA file.
**
**  Returns: Nothing.
*/
VOID
SECTarga::WriteTgaFile()
// DWORD    ImageSize;         /* Size of the image data in bytes              */
// FILE    *FpTgaData;         /* Pointer to TGA image data                    */
// FILE    *FpTga;             /* TGA image file output FILE stream            */
{                                                                        
    WORD    i;              /* Loop counters                                */
    WORD    cmapsize;       /* Size of the color map in bytes               */
    DWORD   stampsize;      /* Size of the postage stamp in bytes           */
    DWORD   currpos;        /* Current offset position in TGA file          */
    TGATAG *tag;            /* TGA tag structure                            */
    
    BYTE *pLineBuffer;
    BYTE cIndex;
    DWORD dwCol;
#ifdef WIN32
		LPBYTE pByte;
#else
		BYTE huge *pByte;
#endif
		int nDivider;

		switch(m_nSrcBitsPerPixel) {
		case 1:
			nDivider = 8;
			break;
		case 4:
			nDivider = 2;
			break;
		default:
			nDivider = 1;
			break;
		}


	ASSERT_VALID(m_pFile);    
    /*
    ** Write the TGA header (Version 1.0 and 2.0).
    */
    m_pFile->Write(&(m_TgaHead.Head.IdLength), 1);
    m_pFile->Write(&m_TgaHead.Head.CmapType, 1);
    m_pFile->Write(&m_TgaHead.Head.ImageType, 1);
    m_pFile->Write(&m_TgaHead.Head.CmapIndex, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.CmapLength, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.CmapEntrySize, 1);
    m_pFile->Write(&m_TgaHead.Head.X_Origin, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.Y_Origin, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.ImageWidth, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.ImageHeight, sizeof(WORD));
    m_pFile->Write(&m_TgaHead.Head.PixelDepth, 1);
    m_pFile->Write(&m_TgaHead.Head.ImagDesc, 1);
    
    /* Write the Image ID field. */
    if (m_TgaHead.Head.IdLength)
	{
	    /* Write the Image ID data */
	    for (i = 0; i < m_TgaHead.Head.IdLength; i++)
		m_pFile->Write(&(m_TgaHead.Cmap.IdField[i]), 1);
	}
    
    /*
    ** Write the color map data (Version 1.0 and 2.0).
    */
    if (m_TgaHead.Head.CmapType)
	{
	    /* Determine the size of the color map */
	    cmapsize = (WORD)(((m_TgaHead.Head.CmapEntrySize + 7) >> 3) *
			      m_TgaHead.Head.CmapLength);

	    /* Write the color map data */
        for (i = 0; i < cmapsize; i++)
	    m_pFile->Write(&m_TgaHead.Cmap.CmapData[i], 1);
	}
    
    /*
    ** Write the image data (Versions 1.0 and 2.0).
    */
    
    /* Write the image data (slowly) to the TGA file one byte at a time */
    long nBufSize =  m_TgaHead.Head.ImageWidth + m_TgaHead.Head.ImageWidth*(long)TGA_BPP;
#ifdef WIN32
    LPBYTE pTempBuf = (LPBYTE)GlobalAllocPtr(GHND, nBufSize);
#else
    BYTE huge *pTempBuf = (BYTE huge *)GlobalAllocPtr(GHND, nBufSize);
#endif
	if (!pTempBuf)
	{
		TRACE(_T("SECTga:: could not allocate temp buffer in WriteTGAfile\n"));
		return;
	}
    
    // Now iterate through each row, encoding a full scanline at a time..
    if (m_nSrcBitsPerPixel <= 8) 
	pLineBuffer = new BYTE[m_dwWidth*3];
    else
	pLineBuffer = 0;
    
    for (DWORD dwRow=0; dwRow<m_dwHeight; dwRow++) 
	{
	    //TODO: Implement encoding option as class member; right now just write unencoded
#ifdef TODO
	    WORD nBytes = TgaEncodeScanLine(pTempBuf, nBufSize, m_dwWidth-1, TGA_BPP, m_lpSrcBits+wRow*(m_dwWidth-1)*TGA_BPP);
	    for (long i=0; i<nBytes; i++)
		m_pFile->Write(&pTempBuf[i], 1);
#endif
	    if (m_nSrcBitsPerPixel > 8) {
#ifdef WIN32
		m_pFile->Write((LPBYTE)(m_lpSrcBits+dwRow*m_dwWidth*(long)TGA_BPP), m_dwWidth*(long)TGA_BPP);
#else
		m_pFile->WriteHuge((m_lpSrcBits+dwRow*m_dwWidth*(long)TGA_BPP), m_dwWidth*(long)TGA_BPP);
#endif
	    }
	    
	    // Otherwise we need to expand a color mapped image out to 24 bits by
	    // explicitly writing out the color table entry for each pixel.
	    else {
		m_nBitHolder = 0;
		pByte = m_lpSrcBits + (dwRow*m_dwWidth)/nDivider;
		for (dwCol=0; dwCol<m_dwWidth; dwCol++) {
			NEXT_IMAGE_PIXEL(cIndex,pByte);
		    pLineBuffer[dwCol*3] = m_lpRGB[cIndex].rgbBlue;
		    pLineBuffer[dwCol*3 + 1] = m_lpRGB[cIndex].rgbGreen;
		    pLineBuffer[dwCol*3 + 2] = m_lpRGB[cIndex].rgbRed;
		}
		m_pFile->Write(pLineBuffer, (UINT)(m_dwWidth*3));
	    }
	    
	    // PutCache(&pTempBuf[i], 1);
	    // PutCache(pTempBuf, nBytes);
	}
    
    // FlushCache();
    // FreeCache(); 
    GlobalFreePtr(pTempBuf);
    if (pLineBuffer)
	delete pLineBuffer;
    
    /*
    ** Check if we are writing a version 2.0 TGA file.  If not, then we are
    ** done.  If so, check if an extension and/or developer area is to be
    ** written and then write the footer.  Then we are done.
    */
    if (m_TgaHead.NewTgaFormat == TRUE)
	{
	    /* Check for the presence of a developers area */
	    if (m_TgaHead.Foot.DeveloperOffset)
		{
		    /* Save the current offset as the new Developer Area offset */
		    m_TgaHead.Foot.DeveloperOffset = m_pFile->GetPosition();
		    
		    /* Write the number of tags in the directory */
			m_pFile->Write(&m_TgaHead.Developer.NumberOfTags, sizeof(WORD));
		    // PutWord(TgaHead->Developer.NumberOfTags, FpTga);
		    
            /*
            ** Write the directory to the TGA file.
            */
		    tag = m_TgaHead.Developer.TagList;
		    for (i = 0; i < m_TgaHead.Developer.NumberOfTags; i++)
			{
			    currpos = m_pFile->GetPosition();

			    /* Calculate a new offset value for the tag data */
			    currpos += (DWORD) (sizeof(tag->TagNumber) +
						sizeof(tag->TagOffset) +
						sizeof(tag->TagSize));
			    
                /* Assign the new offset value */
			    tag->TagOffset = currpos;
			    
                /* Write the tag values */
			    m_pFile->Write(&tag->TagNumber, sizeof(WORD));
			    m_pFile->Write(&tag->TagOffset, sizeof(DWORD));
			    m_pFile->Write(&tag->TagSize, sizeof(DWORD));
			    
			    
			    /* Write the tag data */
			    for (WORD j = 0; j < tag->TagSize; j++)
				m_pFile->Write(&tag->TagData[j], 1);
			    
			    /* Advance to the next tag */
			    tag = tag->Next;
			}
		}
	    
	    /* Check for the presence of an extension area */
	    if (m_TgaHead.Foot.ExtensionOffset)
		{
            /* Save the current offset as the new Extension Area offset */
            m_TgaHead.Foot.ExtensionOffset = m_pFile->GetPosition();

            /* Write the extension area size information */
            // PutWord(TgaHead->Extension.Size, FpTga);
			m_pFile->Write(&m_TgaHead.Extension.Size, sizeof(WORD));

            /* Write the extension information defined in TGA version 2.0 */
            for (i = 0; i < sizeof(m_TgaHead.Extension.AuthorName); i++)
                // PutByte(TgaHead->Extension.AuthorName[i], FpTga);
				m_pFile->Write(&m_TgaHead.Extension.AuthorName[i], 1);

            for (i = 0; i < sizeof(m_TgaHead.Extension.AuthorComment); i++)
                // PutByte(TgaHead->Extension.AuthorComment[i], FpTga);
				m_pFile->Write(&m_TgaHead.Extension.AuthorComment[i], 1);

			m_pFile->Write(&m_TgaHead.Extension.StampMonth, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.StampDay, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.StampYear, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.StampHour, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.StampMinute, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.StampSecond, sizeof(WORD));

            for (i = 0; i < sizeof(m_TgaHead.Extension.JobName); i++)
				m_pFile->Write(&m_TgaHead.Extension.JobName[i], 1);

			m_pFile->Write(&m_TgaHead.Extension.JobHour, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.JobMinute, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.JobSecond, sizeof(WORD));

            for (i = 0; i < sizeof(m_TgaHead.Extension.SoftwareId); i++)
				m_pFile->Write(&m_TgaHead.Extension.SoftwareId[i], 1);

			m_pFile->Write(&m_TgaHead.Extension.VersionNumber, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.VersionLetter, 1);
			m_pFile->Write(&m_TgaHead.Extension.KeyColor, sizeof(DWORD));
			m_pFile->Write(&m_TgaHead.Extension.PixelNumerator, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.PixelDenominator, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.GammaNumerator, sizeof(WORD));
			m_pFile->Write(&m_TgaHead.Extension.GammaDenominator, sizeof(WORD));

            /*
            ** Here is where things get a bit tricky.  We can't assume that
            ** the offset values present in the extension area are still
            ** valid since WriteTgaHeader() cannot tell if the size of any
            ** information previous to the Exntension Area has changed.
            ** We must therefore calculate the size in bytes of any scan
            ** line table, postage stamp image, and the color correction
            ** table and derive their offset values.
            */
            currpos = m_pFile->GetPosition(); //ftell(FpTga); /* Start with the current offset */

            /* Advance to the end of the Extension Area */
            currpos += (DWORD) (sizeof(m_TgaHead.Extension.ScanOffset)  +
                                sizeof(m_TgaHead.Extension.StampOffset) +
                                sizeof(m_TgaHead.Extension.ColorOffset) +
                                sizeof(m_TgaHead.Extension.AttributesType));

            /* Is there a scan line table? */
            if (m_TgaHead.Extension.ScanOffset)
            {
                m_TgaHead.Extension.ScanOffset = currpos;    /* New offset */

                /* Calculate the size of the table in bytes */
                currpos += (DWORD) (m_TgaHead.Head.ImageHeight * sizeof(DWORD));
            }

            /* Is there a postage stamp image? */
            if (m_TgaHead.Extension.StampOffset)
            {
                m_TgaHead.Extension.StampOffset = currpos;   /* New offset */

                /* Calculate the size of the stamp in bytes */
                currpos += (DWORD) m_TgaHead.Extension.StampWidth *
                                   m_TgaHead.Extension.StampHeight *
                                   ((m_TgaHead.Head.PixelDepth + 7) >> 3) +
                                   sizeof(m_TgaHead.Extension.StampWidth) +
                                   sizeof(m_TgaHead.Extension.StampHeight);
            }

            /* Is there a color offset table? */
            if (m_TgaHead.Extension.ColorOffset)
            {
                m_TgaHead.Extension.ColorOffset = currpos;   /* New offset */
            }

            /* Write the offset values and the attribute type */
			m_pFile->Write(&m_TgaHead.Extension.ColorOffset, sizeof(DWORD));
			m_pFile->Write(&m_TgaHead.Extension.StampOffset, sizeof(DWORD));
			m_pFile->Write(&m_TgaHead.Extension.ScanOffset, sizeof(DWORD));
			m_pFile->Write(&m_TgaHead.Extension.AttributesType, 1);

            /* Check for the presence of a scan line table */
            if (m_TgaHead.Extension.ScanOffset)
            {
                /* Write scan line offset values */
                for (i = 0; i < m_TgaHead.Head.ImageHeight; i++)
					m_pFile->Write(&m_TgaHead.Extension.ScanLineTable[i], sizeof(DWORD));
            }

            /* Check for the presence of a postage stamp image*/
            if (m_TgaHead.Extension.StampOffset)
            {
                /* Write the size of the stamp in pixels */
				m_pFile->Write(&m_TgaHead.Extension.StampWidth, 1);
				m_pFile->Write(&m_TgaHead.Extension.StampHeight, 1);

                /* Calculate the size of the stamp in bytes */
                stampsize = m_TgaHead.Extension.StampWidth *
                            m_TgaHead.Extension.StampHeight *
                            ((m_TgaHead.Head.PixelDepth + 7) >> 3);

                /* Write the stamp data one byte at a time */
                for (i = 0; i < stampsize; i++)
					m_pFile->Write(&m_TgaHead.Extension.StampImage[i], 1);
            }

            /* Check for the presence of a color correction table */
            if (m_TgaHead.Extension.ColorOffset)
            {
                /* Write the entire 1024 WORD (2048 byte) table */
                for (i = 0; i < sizeof(m_TgaHead.Extension.ColorTable); i++)
					m_pFile->Write(&m_TgaHead.Extension.ColorTable[i], sizeof(WORD));
            }
        }

        /*
        ** Write the version 2.0 footer.
        */

        /* Write the developer area and extension area offset values */
		m_pFile->Write(&m_TgaHead.Foot.ExtensionOffset, sizeof(DWORD));
		m_pFile->Write(&m_TgaHead.Foot.DeveloperOffset, sizeof(DWORD));
        
        /* Write the TGA signature */
        for (i = 0; i < sizeof(m_TgaHead.Foot.Signature); i++)
            m_pFile->Write(&m_TgaHead.Foot.Signature[i], 1);
    }
}


/*
**  Read raw image data from a FILE stream and write TGA RLE data to a
**  storage buffer.
**
**  This function always produces raw packets that are a run of the same
**  pixel values.
**
**  We assume that when TgaEncodeScanLine() is called TgaFp is pointing
**  to the first byte of unencoded image data in the FILE stream.
**
**  Returns: A negative value if a buffer overflow would occur, otherwise 
**           the number of RLE packets written to the buffer.
*/
short
#ifdef WIN32
SECTarga::TgaEncodeScanLine( BYTE *EncodedBuffer, WORD BufferSize, WORD LineLength, WORD PixelSize, LPBYTE pImageData)
#else
SECTarga::TgaEncodeScanLine( BYTE *EncodedBuffer, WORD BufferSize, WORD LineLength, WORD PixelSize, BYTE huge *pImageData)
#endif
{
    WORD    i;              /* Loop Counter                                 */
    BYTE    runCount;       /* The number of pixels in the current run      */
    WORD    pixelCount;     /* The number of pixels read from the scan line */
    short	bufIndex;       /* The index of DecodedBuffer                   */
    BYTE    pixelValue1[4]; /* Pixel value read from the scan line (4-byte max) */
    BYTE    pixelValue2[4]; /* Pixel value read from the scan line          */
    BOOL    pixelsMatch;    /* TRUE if pixelValue1 and pixelValue2 are same */

    bufIndex   = 0;
    runCount   = 1;
    pixelCount = 1; 

    /* Read in first pixel value */
    for (i = 0; i < PixelSize; i++)
    {
		if (pImageData)
			pixelValue1[i] = (BYTE)*pImageData++;
		else if ((GetCache(&pixelValue1[i], 1))!=1)
			return -1;
    }

    /* Main encoding loop */
    for (;;)
    {
        pixelsMatch = TRUE;     /* Assume we have a match */


        /* Read in another pixel value */
        for (i = 0; i < PixelSize; i++)
        {
			if (pImageData)
				pixelValue2[i] = (BYTE)*pImageData++;
			else if ((GetCache(&pixelValue2[i], 1))!=1)
				return -1;
            if (pixelValue1[i] != pixelValue2[i])   /* Compare pixels */
                pixelsMatch = FALSE;
        }
        pixelCount++;                /* Count number of pixels read so far */

        /* If the pixels are the same then start or continue a run */
        if (pixelsMatch == TRUE)
        {
            if (runCount < 128)     /* Maximum run-length is 128 pixels     */
            {
				runCount++; 
                if (pixelCount < LineLength)  /* Don't run past end of scan line */
                    continue;       /* Continue reading the run             */
            }
        }
  
        /* Pixel values are different, so stop the run */
        if (runCount < 8)     /* Write a raw packet */
        {
            /* Make sure writing this next run will not overflow the buffer */
            if (bufIndex + (runCount * PixelSize) >= BufferSize)
                return(-1);

            /* Write runCount - 1 to buffer.  MSB is off. */
            EncodedBuffer[bufIndex++] = (BYTE)((runCount - 1) & ~0x80);

            /* Write the pixel data run */
            do
            {
                for (i = 0; i < PixelSize; i++)
                {
                    EncodedBuffer[bufIndex++] = pixelValue1[i];
                    // pixelValue1[i] = pixelValue2[i];
                }
				--runCount;
            }
            while (runCount); 
            for(i=0; i<PixelSize; i++)
            	pixelValue1[i]=pixelValue2[i];
        }
        else                    /* Write an encoded packet */
        {
        	
            /* Make sure writing this next run will not overflow the buffer */
            if (bufIndex + PixelSize + 1 >= BufferSize)
            {
                return(-2);
            }

            /* Write the Pixel Count to the buffer.  MSB is on. */
            EncodedBuffer[bufIndex++] = (BYTE)((runCount - 1) | 0x80);

            /* Write the pixel data run value */
            for (i = 0; i < PixelSize; i++)
            {
                EncodedBuffer[bufIndex++] = pixelValue1[i];
                pixelValue1[i] = pixelValue2[i];
            }
        }
        /* If we've encoded the entire line then break out of the loop */
		runCount = 1;
        if (pixelCount == LineLength)
		{
			if (pixelsMatch)
				return (bufIndex);

             EncodedBuffer[bufIndex++] = (BYTE)(0x0);

            /* Write the pixel data run value */
            for (i = 0; i < PixelSize; i++)
                EncodedBuffer[bufIndex++] = pixelValue2[i];
            return(bufIndex);
		}
        runCount = 1;                /* Start a new pixel run count  */
    }
    return(bufIndex);	      
}




/*
**  Read encoded data from a TGA image file, decode it, and write it to
**  a buffer.  Assume that the FILE pointer is pointing to the first byte
**  of encoded image data.
**
**  Returns: A negative value if a buffer overflow would occur, otherwise 0
**           if no errors occur.
*/
short
#ifdef WIN32
SECTarga::TgaDecodeScanLine(BYTE *DecodedBuffer, WORD LineLength, WORD PixelSize, LPBYTE pImageData)
#else
SECTarga::TgaDecodeScanLine(BYTE *DecodedBuffer, WORD LineLength, WORD PixelSize, BYTE huge *pImageData)
#endif
// BYTE *DecodedBuffer;      /* Pointer to buffer to hold decoded data         */
// WORD  LineLength;         /* The length of a scan line in pixels            */
// WORD  PixelSize;          /* The number of bytes in a pixel                 */
// FILE *FpTga;              /* FILE pointer to the open input TGA image file  */
{
    WORD    i;              /* Loop counter                                 */
    short   byteCount;      /* Number of bytes written to the buffer        */
    BYTE    runCount;       /* The pixel run count                          */
    WORD    bufIndex;       /* The index of DecodedBuffer                   */
    WORD    bufMark;        /* Index marker of DecodedBuffer                */
    WORD    pixelCount;     /* The number of pixels read from the scan line */

    bufIndex   = 0;         /* Initialize buffer index  */
    byteCount  = 0;         /* Initialize byte count    */
    pixelCount = 0; 		/* Initialize pixel counter */
	BYTE b1, b2, red, green, blue;

    /* Main decoding loop */
    while (pixelCount < LineLength)
    {
		if (pImageData)
			runCount = (BYTE)(*pImageData++);
		else if ((GetCache(&runCount, 1))!=1)
			return -1;
     
        /* Make sure writing this next run will not overflow the buffer */       
        if (pixelCount + (runCount & 0x7f) + 1 > LineLength)
            return(-1);     /* Pixel run will overflow buffer */

        /* If the run is encoded... */
        if (runCount & 0x80)
        {
            runCount &= ~0x80;              /* Mask off the upper bit       */
            bufMark = bufIndex;             /* Save the start-of-run index  */

            /* Update total pixel count */
            pixelCount = (WORD)(pixelCount + runCount + 1);


            /* Write the first pixel of the run to the buffer */
			if (PixelSize != 2)
			{
				byteCount = (short)(byteCount + (runCount + 1) * PixelSize);
				for (i = 0; i < PixelSize; i++) 
				{
					if (pImageData)
						DecodedBuffer[bufIndex++] = (BYTE)*(pImageData++);
					else if ((GetCache(&DecodedBuffer[bufIndex++], 1))!=1)
						return -1;
				}
			
				/* Write remainder of pixel run to buffer 'runCount' times */
				while (runCount--)
				{
					for (i = 0; i < PixelSize; i++)
				        DecodedBuffer[bufIndex++] = DecodedBuffer[bufMark + i];
				}

			}
			else
			{
			byteCount = (short)(byteCount + (runCount + 1) * 3);
				if (pImageData)
					// DecodedBuffer[bufIndex++] = (BYTE)*(pImageData++);
					b1 = (BYTE)*pImageData++;
				// if ((GetCache(&DecodedBuffer[bufIndex++], 1))!=1)
				else if ((GetCache(&b1, 1))!=1)
					return -1;
				if (pImageData)
					// DecodedBuffer[bufIndex++] = (BYTE)*(pImageData++);
					b2 = (BYTE)*pImageData++;
				// if ((GetCache(&DecodedBuffer[bufIndex++], 1))!=1)
				else if ((GetCache(&b2, 1))!=1)
					return -1;
				short n = (short)(b1 + (b2<<8));
				red = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];
				n >>=5;
				green = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];
				n >>=5;
				blue = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];

				while (runCount--)
				{
					DecodedBuffer[bufIndex++] = red;
					DecodedBuffer[bufIndex++] = green;
					DecodedBuffer[bufIndex++] = blue;
				}

			}

        }
        else    /* ...the run is unencoded (raw) */
        {
            /* Update total pixel count */
            pixelCount = (WORD)(pixelCount + (WORD)(runCount + 1));
            

            /* Write runCount pixels */
			if (PixelSize != 2)
			{
			/* Update the buffer byte count */
            byteCount  = (short)(byteCount + (runCount + 1) * PixelSize);
            do
            {
                for (i = 0; i < PixelSize; i++) {
					if (pImageData)
						DecodedBuffer[bufIndex++] = (BYTE)*(pImageData++);
					else if ((GetCache(&DecodedBuffer[bufIndex++], 1))!=1)
						return -1;
					}
            } while (runCount--);
			}
			else
			{
				byteCount = (short)(byteCount + (runCount + 1) * 3);
				do
				{
				if (pImageData)
					b1 = (BYTE)*pImageData++;
				else if ((GetCache(&b1, 1))!=1)
					return -1;
				if (pImageData)
					b2 = (BYTE)*pImageData++;
				else if ((GetCache(&b2, 1))!=1)
					return -1;
				short n = (short)(b1 + (b2<<8));
				red = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];
				n >>=5;
				green = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];
				n >>=5;
				blue = DecodedBuffer[bufIndex++] = c5to8bits[n&0x1F];

				} while (runCount--);

			}
        }
    }

    return(byteCount);
}
	 	 

void SECTarga::Serialize(CArchive& ar)
{
    SECImage::Serialize(ar);
}

#ifdef _DEBUG
void SECTarga::AssertValid() const
{
    CObject::AssertValid();
	SECImage::AssertValid();
}
#endif /* _DEBUG */

