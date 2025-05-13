#include "Sort.h"

//=========================================================================
void SortIncrease (CArrayDouble &ArrX)
  {
  int i, j;
  int NArr = ArrX.GetSize();
  if (NArr == 0)
    return;
  double XMin, XTmp;
  int jMin;
  for (i = 0; i < NArr-1; i++)
    {
    XMin = ArrX[i];
    jMin = i;
    for (j = i + 1; j < NArr; j++)
      {
      if (ArrX[j] < XMin)
        {
        XMin = ArrX[j];
        jMin = j;
        }
      }
    if (jMin != i)
      {
      XTmp = ArrX[i];
      ArrX[i] = ArrX[jMin];
      ArrX[jMin] = XTmp;
      }
    }
  }
//=========================================================================
