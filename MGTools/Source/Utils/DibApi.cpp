#include "..\..\stdafx.h"
#include <io.h>
#include <errno.h>
#include <assert.h>
#include "..\..\Include\Utils\dibapi.h"
#include "..\..\Include\Utils\utils.h"

#define COLOR_SCALE_RED 0
#define COLOR_SCALE_GREEN 1
#define COLOR_SCALE_BLUE 2
#define COLOR_SCALE_GRAY 3
/*
  Вывод структуры файла BMP в слиентную часть окна
*/
BOOL WINAPI PaintDIB(HDC     hDC, int ShiftX, int ShiftY,
					LPRECT  lpDCRect,
					HDIB    hDIB,
					LPRECT  lpDIBRect,
					CPalette* pPal)
{
	LPSTR    lpDIBHdr;            // Pointer to BITMAPINFOHEADER
	LPSTR    lpDIBBits;           // Pointer to DIB bits
	BOOL     bSuccess=FALSE;      // Success/fail flag
	HPALETTE hPal=NULL;           // Our DIB's palette
	HPALETTE hOldPal=NULL;        // Previous palette


	/* Check for valid DIB handle */
	if (hDIB == NULL)
		return FALSE;

	/* Lock down the DIB, and get a pointer to the beginning of the bit
	 *  buffer
	 */
	lpDIBHdr  = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);
	lpDIBBits = ::FindDIBBits(lpDIBHdr);

	// Get the DIB's palette, then select it into DC
	if (pPal != NULL)
	{
		hPal = (HPALETTE) pPal->m_hObject;

		// Select as background since we have
		// already realized in forground if needed
		hOldPal = ::SelectPalette(hDC, hPal, TRUE);
	}

	/* Make sure to use the stretching mode best for color pictures */
	::SetStretchBltMode(hDC, COLORONCOLOR);

	/* Determine whether to call StretchDIBits() or SetDIBitsToDevice() */
	if ((RECTWIDTH(lpDCRect)  == RECTWIDTH(lpDIBRect)) &&
	   (RECTHEIGHT(lpDCRect) == RECTHEIGHT(lpDIBRect)))
		bSuccess = ::SetDIBitsToDevice(hDC,                    // hDC
								   lpDCRect->left,             // DestX
								   lpDCRect->top,              // DestY
								   RECTWIDTH(lpDCRect)-ShiftX, // nDestWidth
								   RECTHEIGHT(lpDCRect)-ShiftY,// nDestHeight
								   lpDIBRect->left+ShiftX,    // SrcX
								   (int)DIBHeight(lpDIBHdr) -
									  lpDIBRect->top+ShiftY -
									  RECTHEIGHT(lpDIBRect),   // SrcY
								   ShiftY,                          // nStartScan
								   (WORD)DIBHeight(lpDIBHdr),  // nNumScans
								   lpDIBBits,                  // lpBits
                   (LPBITMAPINFO)lpDIBHdr,     // lpBitsInfo
								   DIB_RGB_COLORS);            // wUsage
   else
	  bSuccess = ::StretchDIBits(hDC,                          // hDC
							   lpDCRect->left-ShiftX,                 // DestX
							   lpDCRect->top-ShiftY,                  // DestY
							   RECTWIDTH(lpDCRect),            // nDestWidth
							   RECTHEIGHT(lpDCRect),           // nDestHeight
							   lpDIBRect->left,         // SrcX
							   lpDIBRect->top,          // SrcY
							   RECTWIDTH(lpDIBRect),    // wSrcWidth
							   RECTHEIGHT(lpDIBRect),   // wSrcHeight
							   lpDIBBits,                      // lpBits
							   (LPBITMAPINFO)lpDIBHdr,         // lpBitsInfo
							   DIB_RGB_COLORS,                 // wUsage
//                               SRCPAINT);                       // dwROP
                               SRCCOPY);                       // dwROP

   ::GlobalUnlock((HGLOBAL) hDIB);

	/* Reselect old palette */
	if (hOldPal != NULL)
	{
		::SelectPalette(hDC, hOldPal, TRUE);
	}

   return bSuccess;
}

/*************************************************************************
 *
 * CreateDIBPalette()
 *
 * Parameter:
 *
 * HDIB hDIB        - specifies the DIB
 *
 * Return Value:
 *
 * HPALETTE         - specifies the palette
 *
 * Description:
 *
 * This function creates a palette from a DIB by allocating memory for the
 * logical palette, reading and storing the colors from the DIB's color table
 * into the logical palette, creating a palette from this logical palette,
 * and then returning the palette's handle. This allows the DIB to be
 * displayed using the best possible colors (important for DIBs with 256 or
 * more colors).
 *
 ************************************************************************/


BOOL WINAPI CreateDIBPalette(HDIB hDIB, CPalette* pPal)
{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	HPALETTE hPal = NULL;    // handle to a palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;

	/* if handle to DIB is invalid, return FALSE */

	if (hDIB == NULL)
	  return FALSE;

   lpbi = (LPSTR) ::GlobalLock((HGLOBAL) hDIB);

   /* get pointer to BITMAPINFO (Win 3.0) */
   lpbmi = (LPBITMAPINFO)lpbi;

   /* get pointer to BITMAPCOREINFO (old 1.x) */
   lpbmc = (LPBITMAPCOREINFO)lpbi;

   /* get the number of colors in the DIB */
   wNumColors = ::DIBNumColors(lpbi);

   if (wNumColors != 0)
   {
		/* allocate memory block for logical palette */
		hLogPal = ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOGPALETTE)
									+ sizeof(PALETTEENTRY)
									* wNumColors);

		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
		{
			::GlobalUnlock((HGLOBAL) hDIB);
			return FALSE;
		}

		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);

		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;

		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
		{
			if (bWinStyleDIB)
			{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
			else
			{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
			}
		}

		/* create the palette and get handle to it */
		bResult = pPal->CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
//		::GlobalFree((HGLOBAL) hLogPal);
		HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hLogPal);

	}

	::GlobalUnlock((HGLOBAL) hDIB);

	return bResult;
}

/*************************************************************************
 *
 * FindDIBBits()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * LPSTR            - pointer to the DIB bits
 *
 * Description:
 *
 * This function calculates the address of the DIB's bits and returns a
 * pointer to the DIB bits.
 *
 ************************************************************************/


LPSTR WINAPI FindDIBBits(LPSTR lpbi)
{
	return (lpbi + *(LPDWORD)lpbi + ::PaletteSize(lpbi));
}


/*************************************************************************
 *
 * DIBWidth()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - width of the DIB
 *
 * Description:
 *
 * This function gets the width of the DIB from the BITMAPINFOHEADER
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * width field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD WINAPI DIBWidth(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether Win 3.0 and old) */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB width if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its width */
		return (DWORD)lpbmc->bcWidth;
}


/*************************************************************************
 *
 * DIBHeight()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * DWORD            - height of the DIB
 *
 * Description:
 *
 * This function gets the height of the DIB from the BITMAPINFOHEADER
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER
 * height field if it is an other-style DIB.
 *
 ************************************************************************/


DWORD WINAPI DIBHeight(LPSTR lpDIB)
{
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		return lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		return (DWORD)lpbmc->bcHeight;
}


/*************************************************************************
 *
 * PaletteSize()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - size of the color palette of the DIB
 *
 * Description:
 *
 * This function gets the size required to store the DIB's palette by
 * multiplying the number of colors by the size of an RGBQUAD (for a
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an other-
 * style DIB).
 *
 ************************************************************************/


WORD WINAPI PaletteSize(LPSTR lpbi)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpbi))
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBQUAD));
   else
	  return (WORD)(::DIBNumColors(lpbi) * sizeof(RGBTRIPLE));
}


/*************************************************************************
 *
 * DIBNumColors()
 *
 * Parameter:
 *
 * LPSTR lpbi       - pointer to packed-DIB memory block
 *
 * Return Value:
 *
 * WORD             - number of colors in the color table
 *
 * Description:
 *
 * This function calculates the number of colors in the DIB's color table
 * by finding the bits per pixel for the DIB (whether Win3.0 or other-style
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256,
 * if 24, no colors in color table.
 *
 ************************************************************************/


WORD WINAPI DIBNumColors(LPSTR lpbi)
{
	WORD wBitCount;  // DIB bit count

	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */

	if (IS_WIN30_DIB(lpbi))
	{
		DWORD dwClrUsed;

		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
	}

	/*  Calculate the number of colors in the color table based on
	 *  the number of bits per pixel for the DIB.
	 */
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;

	/* return number of colors based on bits per pixel */
	switch (wBitCount)
	{
		case 1:
			return 2;

		case 4:
			return 16;

		case 8:
			return 256;

		default:
			return 0;
	}
}


//////////////////////////////////////////////////////////////////////////
//// Clipboard support

//---------------------------------------------------------------------
//
// Function:   CopyHandle (from SDK IntView sample clipbrd.c)
//
// Purpose:    Makes a copy of the given global memory block.  Returns
//             a handle to the new memory block (NULL on error).
//
//             Routine stolen verbatim out of ShowDIB.
//
// Parms:      h == Handle to global memory to duplicate.
//
// Returns:    Handle to new global memory block.
//
//---------------------------------------------------------------------

HGLOBAL WINAPI CopyHandle (HGLOBAL h)
{
	if (h == NULL)
		return NULL;

	DWORD dwLen = ::GlobalSize((HGLOBAL)h);
	HGLOBAL hCopy = ::GlobalAlloc(GHND, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) h);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock(hCopy);
		::GlobalUnlock(h);
	}

	return hCopy;
}

HGLOBAL CopyHandle32(LPCVOID lpMem, DWORD dwLen)
{
	if (lpMem == NULL)
		return NULL;

	LPVOID hCopy =  ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLen);

	if (hCopy != NULL)
	{
		void* lpCopy = ::GlobalLock((HGLOBAL) hCopy);
		void* lp     = ::GlobalLock((HGLOBAL) lpMem);
		memcpy(lpCopy, lp, dwLen);
		::GlobalUnlock((HGLOBAL)hCopy);
		::GlobalUnlock((HGLOBAL)lpMem);
	}

	return hCopy;
}

/*************************************************************************
 *
 * BitmapToDIB()
 *
 * Parameters:
 *
 * HBITMAP hBitmap  - specifies the bitmap to convert
 *
 * HPALETTE hPal    - specifies the palette to use with the bitmap
 *
 * Return Value:
 *
 * HDIB             - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function creates a DIB from a bitmap using the specified palette.
 *
 ************************************************************************/

HDIB BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal)
{
    BITMAP              bm;         // bitmap structure
    BITMAPINFOHEADER    bi;         // bitmap header
    LPBITMAPINFOHEADER  lpbi;       // pointer to BITMAPINFOHEADER
    DWORD               dwLen;      // size of memory block
    HANDLE              hDIB, h;    // handle to DIB, temp handle
    HDC                 hDC;        // handle to DC
    WORD                biBits;     // bits per pixel

    // check if bitmap handle is valid

    if (!hBitmap)
        return NULL;

    // fill in BITMAP structure, return NULL if it didn't work

    if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm))
        return NULL;

    // if no palette is specified, use default palette

    if (hPal == NULL)
        hPal = (HPALETTE)GetStockObject(DEFAULT_PALETTE);

    // calculate bits per pixel

    biBits = bm.bmPlanes * bm.bmBitsPixel;

    // make sure bits per pixel is valid

    if (biBits <= 1)
        biBits = 1;
    else if (biBits <= 4)
        biBits = 4;
    else if (biBits <= 8)
        biBits = 8;
    else // if greater than 8-bit, force to 24-bit
        biBits = 24;

    // initialize BITMAPINFOHEADER

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = biBits;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // calculate size of memory block required to store BITMAPINFO

    dwLen = bi.biSize + PaletteSize((LPSTR)&bi);

    // get a DC

    hDC = GetDC(NULL);

    // select and realize our palette

    hPal = SelectPalette(hDC, hPal, FALSE);
    RealizePalette(hDC);

    // alloc memory block to store our bitmap

//    hDIB = (HDIB)GlobalAlloc(GHND, dwLen);
	hDIB =  HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwLen);

    // if we couldn't get memory block

    if (!hDIB)
    {
      // clean up and return NULL

      SelectPalette(hDC, hPal, TRUE);
      RealizePalette(hDC);
      ReleaseDC(NULL, hDC);
      return NULL;
    }

    // lock memory and get pointer to it

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    /// use our bitmap info. to fill BITMAPINFOHEADER

    *lpbi = bi;

    // call GetDIBits with a NULL lpBits param, so it will calculate the
    // biSizeImage field for us

    GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, NULL, (LPBITMAPINFO)lpbi,
        DIB_RGB_COLORS);

    // get the info. returned by GetDIBits and unlock memory block

    bi = *lpbi;
    GlobalUnlock(hDIB);

    // if the driver did not fill in the biSizeImage field, make one up
    if (bi.biSizeImage == 0)
     bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight;

    // realloc the buffer big enough to hold all the bits

    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage;

//    if (h = GlobalReAlloc(hDIB, dwLen, 0))
    if (h = HeapReAlloc(GetProcessHeap(), 0, hDIB, dwLen))
	     hDIB = h;
    else
    {
        // clean up and return NULL

//        GlobalFree(hDIB);
        HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hDIB); hDIB = NULL;
        hDIB = NULL;
        SelectPalette(hDC, hPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
        return NULL;
    }

    // lock memory block and get pointer to it */

    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB);

    // call GetDIBits with a NON-NULL lpBits param, and actualy get the
    // bits this time

    if (GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, (LPSTR)lpbi +
            (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi,
            DIB_RGB_COLORS) == 0)
    {
        // clean up and return NULL

        GlobalUnlock(hDIB);
        hDIB = NULL;
        SelectPalette(hDC, hPal, TRUE);
        RealizePalette(hDC);
        ReleaseDC(NULL, hDC);
        return NULL;
    }

    bi = *lpbi;

    // clean up
    GlobalUnlock(hDIB);
    SelectPalette(hDC, hPal, TRUE);
    RealizePalette(hDC);
    ReleaseDC(NULL, hDC);

    // return handle to the DIB
    return (HDIB)hDIB;
}

/*************************************************************************
 *
 * CopyScreenToDIB()
 *
 * Parameter:
 *
 * LPRECT lpRect    - specifies the window
 *
 * Return Value:
 *
 * HDIB             - identifies the device-independent bitmap
 *
 * Description:
 *
 * This function copies the specified part of the screen to a device-
 * independent bitmap.
 *
 ************************************************************************/
HDIB CopyScreenToDIB(LPRECT lpRect)
{
    HBITMAP     hBitmap;        // handle to device-dependent bitmap
    HPALETTE    hPalette;       // handle to palette
    HDIB        hDIB = NULL;    // handle to DIB

    // get the device-dependent bitmap in lpRect by calling
    //  CopyScreenToBitmap and passing it the rectangle to grab

    hBitmap = CopyScreenToBitmap(lpRect);

    // check for a valid bitmap handle

    if (!hBitmap)
      return NULL;

    // get the current palette

    hPalette = GetSystemPalette();

    // convert the bitmap to a DIB

    hDIB = BitmapToDIB(hBitmap, hPalette);

    // clean up

    DeleteObject(hPalette);
    DeleteObject(hBitmap);

    // return handle to the packed-DIB
    return hDIB;
}

/*************************************************************************
 *
 * CopyWindowToBitmap()
 *
 * Parameters:
 *
 * HWND hWnd        - specifies the window
 *
 * WORD fPrintArea  - specifies the window area to copy into the device-
 *                    dependent bitmap
 *
 * Return Value:
 *
 * HDIB         - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function copies the specified part(s) of the window to a device-
 * dependent bitmap.
 *
 *
 ************************************************************************/

HBITMAP CopyWindowToBitmap(HWND hWnd, WORD fPrintArea)
{
    HBITMAP     hBitmap = NULL;  // handle to device-dependent bitmap

    // check for a valid window handle

    if (!hWnd)
        return NULL;

    switch (fPrintArea)
    {
        case PW_WINDOW: // copy entire window
        {
            RECT    rectWnd;

            // get the window rectangle

            GetWindowRect(hWnd, &rectWnd);

            // get the bitmap of that window by calling
            // CopyScreenToBitmap and passing it the window rect
       
            hBitmap = CopyScreenToBitmap(&rectWnd);
            break;
        }
        

        case PW_CLIENT: // copy client area
        {
            RECT rectClient;
            POINT pt1, pt2;

            // get client dimensions

            GetClientRect(hWnd, &rectClient);

            // convert client coords to screen coords

            pt1.x = rectClient.left;
            pt1.y = rectClient.top;
            pt2.x = rectClient.right;
            pt2.y = rectClient.bottom;
            ClientToScreen(hWnd, &pt1);
            ClientToScreen(hWnd, &pt2);
            rectClient.left = pt1.x;
            rectClient.top = pt1.y;
            rectClient.right = pt2.x;
            rectClient.bottom = pt2.y;

            // get the bitmap of the client area by calling
            // CopyScreenToBitmap and passing it the client rect
            
            hBitmap = CopyScreenToBitmap(&rectClient);
            break;
        }
    
        default:    // invalid print area
            return NULL;
    }

    // return handle to the bitmap

   return hBitmap;
}

HBITMAP CopyScreenToBitmap(LPRECT lpRect)
{
    HDC         hScrDC, hMemDC;         // screen DC and memory DC
    HBITMAP     hBitmap, hOldBitmap;    // handles to deice-dependent bitmaps
    int         nX, nY, nX2, nY2;       // coordinates of rectangle to grab
    int         nWidth, nHeight;        // DIB width and height
    int         xScrn, yScrn;           // screen resolution

    // check for an empty rectangle

    if (IsRectEmpty(lpRect))
      return NULL;

    // create a DC for the screen and create
    // a memory DC compatible to screen DC
    
    hScrDC = CreateDC("DISPLAY", NULL, NULL, NULL);
    hMemDC = CreateCompatibleDC(hScrDC);

    // get points of rectangle to grab

    nX = lpRect->left;
    nY = lpRect->top;
    nX2 = lpRect->right;
    nY2 = lpRect->bottom;

    // get screen resolution

    xScrn = GetDeviceCaps(hScrDC, HORZRES);
    yScrn = GetDeviceCaps(hScrDC, VERTRES);

    //make sure bitmap rectangle is visible

    if (nX < 0)
        nX = 0;
    if (nY < 0)
        nY = 0;
    if (nX2 > xScrn)
        nX2 = xScrn;
    if (nY2 > yScrn)
        nY2 = yScrn;

    nWidth = nX2 - nX;
    nHeight = nY2 - nY;

    // create a bitmap compatible with the screen DC
    hBitmap = CreateCompatibleBitmap(hScrDC, nWidth, nHeight);

    // select new bitmap into memory DC
    hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

    // bitblt screen DC to memory DC
    BitBlt(hMemDC, 0, 0, nWidth, nHeight, hScrDC, nX, nY, SRCCOPY);

    // select old bitmap back into memory DC and get handle to
    // bitmap of the screen
    
    hBitmap = (HBITMAP)SelectObject(hMemDC, hOldBitmap);

    // clean up

    DeleteDC(hScrDC);
    DeleteDC(hMemDC);

    // return handle to the bitmap

    return hBitmap;
}

/*************************************************************************
 *
 * GetSystemPalette()
 *
 * Parameters:
 *
 * None
 *
 * Return Value:
 *
 * HPALETTE         - handle to a copy of the current system palette
 *
 * Description:
 *
 * This function returns a handle to a palette which represents the system
 * palette.  The system RGB values are copied into our logical palette using
 * the GetSystemPaletteEntries function.  
 *
 ************************************************************************/
HPALETTE GetSystemPalette(void)
{
    HDC hDC;                // handle to a DC
    static HPALETTE hPal = NULL;   // handle to a palette
    HANDLE hLogPal;         // handle to a logical palette
    LPLOGPALETTE lpLogPal;  // pointer to a logical palette
    int nColors;            // number of colors

    // Find out how many palette entries we want.

    hDC = GetDC(NULL);

    if (!hDC)
        return NULL;

    nColors = PalEntriesOnDevice(hDC);   // Number of palette entries

    // Allocate room for the palette and lock it.

//    hLogPal = GlobalAlloc(GHND, sizeof(LOGPALETTE) + nColors *
//            sizeof(PALETTEENTRY));
	hLogPal =  HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOGPALETTE) + nColors *sizeof(PALETTEENTRY)); 

    // if we didn't get a logical palette, return NULL

    if (!hLogPal)
        return NULL;

    // get a pointer to the logical palette

    lpLogPal = (LPLOGPALETTE)GlobalLock(hLogPal);

    // set some important fields

    lpLogPal->palVersion = PALVERSION;
    lpLogPal->palNumEntries = nColors;

    // Copy the current system palette into our logical palette

    GetSystemPaletteEntries(hDC, 0, nColors,
            (LPPALETTEENTRY)(lpLogPal->palPalEntry));

    // Go ahead and create the palette.  Once it's created,
    // we no longer need the LOGPALETTE, so free it.    

    hPal = CreatePalette(lpLogPal);

    // clean up

    GlobalUnlock(hLogPal);
//    GlobalFree(hLogPal);
	HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hLogPal);
    ReleaseDC(NULL, hDC);

    return hPal;
}

/*************************************************************************
 *
 * PalEntriesOnDevice()
 *
 * Parameter:
 *
 * HDC hDC          - device context
 *
 * Return Value:
 *
 * int              - number of palette entries on device
 *
 * Description:
 *
 * This function gets the number of palette entries on the specified device
 *
 ************************************************************************/

int PalEntriesOnDevice(HDC hDC)
{
    int nColors;  // number of colors

    // Find out the number of colors on this device.
    
    nColors = (1 << (GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES)));

    assert(nColors);
    return nColors;
}

/*---------------------------------------------------------------------------*\
| CREATE COLOR SCALE PALETTE
|   This routine creates a palette representing the scale values of a
|   particular RGB color.  A gray-scale palette can also be created.
|
\*---------------------------------------------------------------------------*/
HPALETTE CreateColorScalePalette(HDC hDC, int nColor)
{
    HPALETTE     hPalette;
    GLOBALHANDLE hMem;
    LPLOGPALETTE lpMem;
    int          idx,nReserved,nSize;

    hPalette = NULL;
        nReserved = GetDeviceCaps(hDC,NUMRESERVED);
        nSize     = GetDeviceCaps(hDC,SIZEPALETTE) - nReserved;
        if(nColor == COLOR_MESH_SCALE) nSize = PAL_COLORS;
        if(hMem = GlobalAlloc(GHND,(DWORD)sizeof(LOGPALETTE)+(sizeof(PALETTEENTRY)*nSize)))
        {
            if(lpMem = (LPLOGPALETTE)GlobalLock(hMem))
            {
                lpMem->palNumEntries = (WORD)nSize;
                lpMem->palVersion    = (WORD)0x0300;
                
                int id1, id2, id3;
                id1 = id2 = id3 = 0;
                float s2 = (float)(nSize/3.);
                int col;
                switch(nColor)
                {
                    case COLOR_MESH_SCALE:
                        for(idx=0; idx < nSize; idx++)
                        {
                            if(idx<=nSize*1/3){
                               id1++; 
                               col = (int)(id1/s2 * 255);
                               if(col>255) col = 255;
                               lpMem->palPalEntry[idx].peRed  = 0;
                               lpMem->palPalEntry[idx].peGreen  = (BYTE)col;
                               lpMem->palPalEntry[idx].peBlue  = 255;
//                               lpMem->palPalEntry[idx].peRed  = 0;
//                               lpMem->palPalEntry[idx].peGreen  = (BYTE)col;
//                               lpMem->palPalEntry[idx].peBlue  = (BYTE)col;
                                                }
                            else if(nSize*1/3<=idx && idx<=nSize*2/3){
                               id2++; 
                               col = (int)(id2/s2 * 255);
                               if(col>255) col = 255;
                               lpMem->palPalEntry[idx].peRed  = (BYTE)col;
                               lpMem->palPalEntry[idx].peGreen  = 255;
                               lpMem->palPalEntry[idx].peBlue  = (BYTE)(255-col);
                            }
                            else if(idx>=nSize*2/3){
                               id3++; 
                               col = (int)(id3/s2* 255);
                               if(col>255) col = 255;
                               lpMem->palPalEntry[idx].peRed  = 255;
                               lpMem->palPalEntry[idx].peGreen  = (BYTE)(255-col);
                               lpMem->palPalEntry[idx].peBlue  = 0;
                            }
                        }
                     break;
                    case COLOR_SCALE_RED:
                        for(idx=0; idx < nSize; idx++)
                        {
                            lpMem->palPalEntry[idx].peRed   = (BYTE)idx;
                            lpMem->palPalEntry[idx].peGreen = 0;
                            lpMem->palPalEntry[idx].peBlue  = 0;
                            lpMem->palPalEntry[idx].peFlags = PC_RESERVED;
                        }
                        break;

                    case COLOR_SCALE_GREEN:
                        for(idx=0; idx < nSize; idx++)
                        {
                            lpMem->palPalEntry[idx].peRed   = 0;
                            lpMem->palPalEntry[idx].peGreen = (BYTE)idx;
                            lpMem->palPalEntry[idx].peBlue  = 0;
                            lpMem->palPalEntry[idx].peFlags = PC_RESERVED;
                        }
                        break;

                    case COLOR_SCALE_BLUE:
                        for(idx=0; idx < nSize; idx++)
                        {
                            lpMem->palPalEntry[idx].peRed   = 0;
                            lpMem->palPalEntry[idx].peGreen = 0;
                            lpMem->palPalEntry[idx].peBlue  = (BYTE)idx;
                            lpMem->palPalEntry[idx].peFlags = PC_RESERVED;
                        }
                        break;

                    default:
                    case COLOR_SCALE_GRAY:
                        for(idx=0; idx < nSize; idx++)
                        {
                            lpMem->palPalEntry[idx].peRed   = (BYTE)idx;
                            lpMem->palPalEntry[idx].peGreen = (BYTE)idx;
                            lpMem->palPalEntry[idx].peBlue  = (BYTE)idx;
                            lpMem->palPalEntry[idx].peFlags = PC_RESERVED;
                        }
                        break;
                }

                hPalette = CreatePalette(lpMem);

                GlobalUnlock(hMem);
            }
            GlobalFree(hMem);
        }
    return(hPalette);
}

void GetPaletteColor(HPALETTE hP, int ind, float* pFCol, int* pICol)
{
  PALETTEENTRY pe;
  UINT col = GetPaletteEntries(hP, ind, 1, &pe);
  if(col==1){
   if(pFCol){
    pFCol[0] = (float)(pe.peRed/255.);
    pFCol[1] = (float)(pe.peGreen/255.);
    pFCol[2] = (float)(pe.peBlue/255.);
   }
   else if(pICol){
    pICol[0] = (int)pe.peRed;
    pICol[1] = (int)pe.peGreen;
    pICol[2] = (int)pe.peBlue;
   }
  } 
}


