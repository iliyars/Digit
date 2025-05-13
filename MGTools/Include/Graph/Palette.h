#ifndef _PALETTE_H_
#define _PALETTE_H_

#include <math.h>
#include "..\..\stdafx.h"

class CColorPalette   
  {
  public:
  protected:
    RGBQUAD PaletteTab[256];  // Палитра псевдоцветов

  public:
    CColorPalette();
    ~CColorPalette();
    COLORREF GetColor(BYTE Color);
  protected:
    void CreatePalette();
  };
#endif
