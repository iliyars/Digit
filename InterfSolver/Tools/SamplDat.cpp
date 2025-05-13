#include "SamplDat.h"

//==============================================================
SAMPLE_DATA :: SAMPLE_DATA()
  {
  }
//==============================================================
SAMPLE_DATA& SAMPLE_DATA :: operator= (const SAMPLE_DATA &A)
  {
  Properties.Copy(A.Properties);
  XPnt.Copy(A.XPnt);
  YPnt.Copy(A.YPnt);
  FPnt.Copy(A.FPnt);
  return *this;
  }
//==============================================================
int SAMPLE_DATA :: GetSize()
  {
  return XPnt.GetSize();
  }
//==============================================================
void SAMPLE_DATA :: SetSize(int NewSize)
  {
  Properties.SetSize(NewSize);
  XPnt.SetSize(NewSize);      
  YPnt.SetSize(NewSize);      
  FPnt.SetSize(NewSize);      
  }
//==============================================================
void SAMPLE_DATA :: Append(const SAMPLE_DATA &A)
  {
  Properties.Append(A.Properties);
  XPnt.Append(A.XPnt);
  YPnt.Append(A.YPnt);
  FPnt.Append(A.FPnt);
  }
//==============================================================
void SAMPLE_DATA :: Add(double X, double Y, double F, double P)
  {
  Properties.Add(P);
  XPnt.Add(X);
  YPnt.Add(Y);
  FPnt.Add(F);
  }
//==============================================================
void SAMPLE_DATA :: SortIncreaseX()
  {
  int i, j, iM;
  double Xm, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Xm = XPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (XPnt[j] < Xm)
        {
        Xm = XPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: SortDecreaseX()
  {
  int i, j, iM;
  double Xm, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Xm = XPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (XPnt[j] > Xm)
        {
        Xm = XPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: SortIncreaseY()
  {
  int i, j, iM;
  double Ym, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Ym = YPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (YPnt[j] < Ym)
        {
        Ym = YPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: SortDecreaseY()
  {
  int i, j, iM;
  double Ym, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Ym = YPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (YPnt[j] > Ym)
        {
        Ym = YPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: SortIncreaseF()
  {
  int i, j, iM;
  double Fm, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Fm = FPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (FPnt[j] < Fm)
        {
        Fm = FPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: SortDecreaseF()
  {
  int i, j, iM;
  double Fm, Pv, Xv, Yv, Fv;
  int NPnt = XPnt.GetSize();
  if (NPnt == 0)
    return;
  for (i = 0; i < NPnt-1; i++)
    {
    Fm = FPnt[i];
    iM = i;
    for (j = i+1; j < NPnt; j++)
      {
      if (FPnt[j] > Fm)
        {
        Fm = FPnt[j];
        iM = j;
        }
      }
    if (iM == i)
      continue;
    Pv = Properties[i];
    Properties[i] = Properties[iM];
    Properties[iM] = Pv;
    Xv = XPnt[i];
    XPnt[i] = XPnt[iM];
    XPnt[iM] = Xv;  
    Yv = YPnt[i]; 
    YPnt[i] = YPnt[iM];
    YPnt[iM] = Yv;  
    Fv = FPnt[i]; 
    FPnt[i] = FPnt[iM];
    FPnt[iM] = Fv;  
    }  
  }
//==============================================================
void SAMPLE_DATA :: InverseY(double YcInv)
  {
  int i;
  int NPnt = GetSize();
  for (i = 0; i < NPnt; i++)
    YPnt[i] = YcInv - YPnt[i];
  }
//==============================================================
void SAMPLE_DATA :: ShiftX(double dX)
  {
  int i;
  int NPnt = GetSize();
  for (i = 0; i < NPnt; i++)
    XPnt[i] += dX;
  }
//==============================================================
void SAMPLE_DATA :: ShiftY(double dY)
  {
  int i;
  int NPnt = GetSize();
  for (i = 0; i < NPnt; i++)
    YPnt[i] += dY;
  }
//==============================================================
void SAMPLE_DATA :: Normalization(double Xc, double Yc, double Rad, double ScaleFactor)
  {
  int i;
  int NPnt = GetSize();
  for (i = 0; i < NPnt; i++)
    {
    XPnt[i] =  (XPnt[i] - Xc) / Rad;
    YPnt[i] =  (YPnt[i] - Yc) / Rad;
    FPnt[i] /= ScaleFactor;
    }
  }
//==============================================================
void SAMPLE_DATA :: Clear()
  {
  Properties.RemoveAll();
  XPnt.RemoveAll();      
  YPnt.RemoveAll();      
  FPnt.RemoveAll();      
  }
//==============================================================
