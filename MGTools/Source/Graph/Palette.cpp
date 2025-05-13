// BitMapGraph.cpp : implementation file
//

#include <math.h>
#include "..\..\Include\Graph\Palette.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//===========================================================================
 CColorPalette::CColorPalette() 
  {
  CreatePalette();
  }
//===========================================================================
 CColorPalette::~CColorPalette() 
  {
  }
//===========================================================================
void CColorPalette::CreatePalette() 
  {
  int i;
	int x;
  BYTE Blue, Green, Red;

  for (i = 0; i < 256; i++)
    {
	  if (i >= 0 && i < 64)
	    {
		  x = i;
		  Blue = 0;
		  Green = BYTE(-0.00002098*x*x*x*x+0.00331624*x*x*x-0.21533203*x*x+9.68229167*x);
		  Red = 255;
	    }
	  if (i >= 64 && i < 128)
	    {
		  x = 128-i;
		  Blue = 0;;
		  Green = BYTE(-0.9875*i + 318.6);
		  Red = BYTE(-0.00002098*x*x*x*x+0.00331624*x*x*x-0.21533203*x*x+9.68229167*x);
	    }
	  if (i >= 128 && i < 192)
	    {
		  x = i-128;
		  Blue = BYTE(-0.00002098*x*x*x*x+0.00331624*x*x*x-0.21533203*x*x+9.68229167*x);
		  Green = BYTE(0.9875*i + 65.8);
		  Red = 0;
	    }
	  if(i >= 192 && i < 256)
	    {
		  x = 256-i;
		  Blue = 255;
		  Green = BYTE(-0.00002098*x*x*x*x+0.00331624*x*x*x-0.21533203*x*x+9.68229167*x);
		  Red = 0;
	    }
    PaletteTab[i].rgbBlue = Blue;
    PaletteTab[i].rgbGreen = Green;
    PaletteTab[i].rgbRed = Red;
    PaletteTab[i].rgbReserved = 0;
    }
  }
//===========================================================================
COLORREF CColorPalette::GetColor(BYTE Color) 
  {
  COLORREF CR;
  unsigned int i = Color;
    CR = RGB(PaletteTab[i].rgbRed, PaletteTab[i].rgbGreen, PaletteTab[i].rgbBlue);
  return CR;
  }
//===========================================================================
