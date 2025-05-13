#include "isPupil.h"
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYEllipse &ArrEll, const CArrayXYRect &ArrRect, const CArrayXYPolygon &ArrPlg)
  {
  int i;
  int NEll = ArrEll.GetSize();
  for (i = 0; i < NEll; i++)
    {
    if (!ArrEll[i].isVisible(P))
      return false;
    }

  int NRect = ArrRect.GetSize();
  for (i = 0; i < NRect; i++)
    {
    if (!ArrRect[i].isVisible(P))
      return false;
    }

  int NPlg = ArrPlg.GetSize();
  for (i = 0; i < NPlg; i++)
    {
    if (!ArrPlg[i].isVisible(P))
      return false;
    }

  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYEllipse &ArrEll)
  {
  int NEll = ArrEll.GetSize();
  if (NEll == 0)
    return true;
  int i;
  for (i = 0; i < NEll; i++)
    {
    if (!ArrEll[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYRect &ArrRect)
  {
  int NRect = ArrRect.GetSize();
  if (NRect == 0)
    return true;
  int i;
  for (i = 0; i < NRect; i++)
    {
    if (!ArrRect[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYPolygon &ArrPlg)
  {
  int NPlg = ArrPlg.GetSize();
  if (NPlg == 0)
    return true;
  int i;
  for (i = 0; i < NPlg; i++)
    {
    if (!ArrPlg[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYEllipse &ArrEll, int ExceptElm)
  {
  int NEll = ArrEll.GetSize();
  if (NEll == 0)
    return true;
  int i;
  for (i = 0; i < NEll; i++)
    {
    if (i == ExceptElm)
      continue;
    if (!ArrEll[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYRect &ArrRect, int ExceptElm)
  {
  int NRect = ArrRect.GetSize();
  if (NRect == 0)
    return true;
  int i;
  for (i = 0; i < NRect; i++)
    {
    if (i == ExceptElm)
      continue;
    if (!ArrRect[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
bool isPupil(const XYPoint &P, const CArrayXYPolygon &ArrPlg, int ExceptElm)
  {
  int NPlg = ArrPlg.GetSize();
  if (NPlg == 0)
    return true;
  int i;
  for (i = 0; i < NPlg; i++)
    {
    if (i == ExceptElm)
      continue;
    if (!ArrPlg[i].isVisible(P))
      return false;
    }
  return true;
  }
//=========================================================================
