#include "XYBrokenLine.h"
//=========================================================================
XYBrokenLine :: XYBrokenLine()
  {
  }
//=========================================================================
XYBrokenLine :: XYBrokenLine(const XYBrokenLine &A)
   {
   ArrPnt.Copy(A.ArrPnt);
   }
//=========================================================================
XYBrokenLine :: XYBrokenLine(int NPnt, double *pArrX, double *pArrY)
  {
  ArrPnt.SetSize(NPnt);
  for (int i = 0; i < NPnt; i++)
    {
    ArrPnt[i].X = pArrX[i];
    ArrPnt[i].Y = pArrY[i];
    }
  }
//=========================================================================
XYBrokenLine :: XYBrokenLine(int NPnt, int *pArrX, int *pArrY)
  {
  ArrPnt.SetSize(NPnt);
  for (int i = 0; i < NPnt; i++)
    {
    ArrPnt[i].X = double(pArrX[i]);
    ArrPnt[i].Y = double(pArrY[i]);
    }
  }
//=========================================================================
XYBrokenLine :: XYBrokenLine(const CArrayDouble &ArrX, const CArrayDouble &ArrY)
  {
  int NPnt = ArrX.GetSize();
  ArrPnt.SetSize(NPnt);
  for (int i = 0; i < NPnt; i++)
    {
    ArrPnt[i].X = ArrX[i];
    ArrPnt[i].Y = ArrY[i];
    }
  }
//=========================================================================
XYBrokenLine :: XYBrokenLine(const CArrayDouble &ArrXY)
  {
  int NPnt = ArrXY.GetSize();
  NPnt /= 2;
  ArrPnt.SetSize(NPnt);
  for (int i = 0; i < NPnt; i++)
    {
    ArrPnt[i].X = ArrXY[2*i];
    ArrPnt[i].Y = ArrXY[2*i+1];
    }
  }
//=========================================================================
XYBrokenLine :: ~XYBrokenLine()
  {
  }
//=========================================================================
XYBrokenLine& XYBrokenLine :: operator= (const XYBrokenLine &A)
  {
  ArrPnt.Copy(A.ArrPnt);
  return *this;
  }
//=========================================================================
XYBrokenLine& XYBrokenLine :: operator= (const CArrayXYPoint &A)
  {
  int i;
  int Na = A.GetSize();
  ArrPnt.SetSize(Na);
  for (i = 0; i < Na; i++)
    ArrPnt[i] = A[i];
  return *this;
  }
//=========================================================================
void XYBrokenLine :: GetArrX(CArrayDouble &ArrX)
  {
  int i;
  int NPnt = GetSize();
  ArrX.SetSize(NPnt);
  for (i = 0; i < NPnt; i++)
    ArrX[i] = ArrPnt[i].X;
  }
//=========================================================================
void XYBrokenLine :: GetArrY(CArrayDouble &ArrY)
  {
  int i;
  int NPnt = GetSize();
  ArrY.SetSize(NPnt);
  for (i = 0; i < NPnt; i++)
    ArrY[i] = ArrPnt[i].Y;
  }
//=========================================================================
void XYBrokenLine :: Inverse() 
  {
  int NPnt = GetSize();
  int NInv = NPnt / 2;
  int i, i1, i2;
  XYPoint P;
  for (i = 0; i < NInv; i++)
    {
    i1 = i;
    i2 = NPnt -1 -i;
    P = ArrPnt[i1];
    ArrPnt[i1] = ArrPnt[i2];
    ArrPnt[i2] = P;
    }
  }
//=========================================================================
bool XYBrokenLine :: FindNearPoint(const XYPoint P, int &iNear, double &Distn) 
  {
  int i;
  int NPnt = GetSize();
  if (NPnt == 0)
    return false;
  double CurD;
  iNear = 0;
  Distn = Distance(P, ArrPnt[0]);
  for (i = 1; i < NPnt; i++)
    {
    CurD = Distance(P, ArrPnt[i]);
    if (CurD < Distn)
      {
      Distn = CurD;
      iNear = i;
      }
    }
  return true;
  }
//=========================================================================
bool FindNearPoint(const XYPoint P, const XYBrokenLine &BLine, int &iNear, double &Distn)
  {
  int i;
  int NPnt = BLine.GetSize();
  if (NPnt == 0)
    return false;
  double CurD;
  iNear = 0;
  Distn = Distance(P, BLine[0]);
  for (i = 1; i < NPnt; i++)
    {
    CurD = Distance(P, BLine[i]);
    if (CurD < Distn)
      {
      Distn = CurD;
      iNear = i;
      }
    }
  return true;
  }
//=========================================================================
void XYBrokenLine :: Arrange()
  {
  XYPoint P0, P;
  int iNear, iPnt;
  double Distn;
  int NPnt = GetSize();
  if (NPnt < 3)
    return;
  CArray<XYPoint,XYPoint> VArr;
  P0 = ArrPnt[0];
  VArr.Add(P0);
  ArrPnt.RemoveAt(0);
  FindNearPoint(P0, iNear, Distn);
  P = ArrPnt[iNear];
  VArr.Add(P);
  ArrPnt.RemoveAt(iNear);
  while (FindNearPoint(P, iNear, Distn))
    {
    P = ArrPnt[iNear];
    if (Distance(P0, P) < Distn)
      break;
    VArr.Add(P);
    ArrPnt.RemoveAt(iNear);
    }

  if (ArrPnt.GetSize() == 0)
    {
    ArrPnt.SetSize(NPnt);
    for (iPnt = 0; iPnt < NPnt; iPnt++)
      ArrPnt[iPnt] = VArr[iPnt];
    return;
    }

  VArr.InsertAt(0, P);
  ArrPnt.RemoveAt(iNear);
  while (FindNearPoint(P, iNear, Distn))
    {
    P = ArrPnt[iNear];
    VArr.InsertAt(0, P);
    ArrPnt.RemoveAt(iNear);
    }

  ArrPnt.SetSize(NPnt);
  for (iPnt = 0; iPnt < NPnt; iPnt++)
    ArrPnt[iPnt] = VArr[iPnt];
  }
//=========================================================================
void Arrange(XYBrokenLine &BLine)
  {
  XYPoint P0, P;
  int iNear, iPnt;
  double Distn;
  int NPnt = BLine.GetSize();
  if (NPnt < 3)
    return;
  CArray<XYPoint,XYPoint> VArr;
  P0 = BLine[0];
  VArr.Add(P0);
  BLine.RemoveAt(0);
  FindNearPoint(P0, BLine, iNear, Distn);
  P = BLine[iNear];
  VArr.Add(P);
  BLine.RemoveAt(iNear);
  while (FindNearPoint(P, BLine, iNear, Distn))
    {
    P = BLine[iNear];
    if (Distance(P0, P) < Distn)
      break;
    VArr.Add(P);
    BLine.RemoveAt(iNear);
    }

  if (BLine.GetSize() == 0)
    {
    BLine.SetSize(NPnt);
    for (iPnt = 0; iPnt < NPnt; iPnt++)
      BLine[iPnt] = VArr[iPnt];
    return;
    }

  VArr.InsertAt(0, P);
  BLine.RemoveAt(iNear);
  while (FindNearPoint(P, BLine, iNear, Distn))
    {
    P = BLine[iNear];
    VArr.InsertAt(0, P);
    BLine.RemoveAt(iNear);
    }

  BLine.SetSize(NPnt);
  for (iPnt = 0; iPnt < NPnt; iPnt++)
    BLine[iPnt] = VArr[iPnt];
  }
//=========================================================================
