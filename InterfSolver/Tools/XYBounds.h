#ifndef _XYBOUNDS_H_
#define _XYBOUNDS_H_

#include <math.h>

struct XYBounds
  {
  double XLeft;
  double YTop;
  double XRight;
  double YBottom;
  XYBounds()
    {
    XLeft = 0.;
    YTop = 0.;
    XRight = 0.;
    YBottom = 0.;
    }
  XYBounds(double _XLeft, double _YTop, double _XRight, double _YBottom)
    {
    XLeft = _XLeft;
    YTop = _YTop;
    XRight = _XRight;
    YBottom = _YBottom;
    }
  XYBounds(const XYBounds &A)
    {
    XLeft = A.XLeft;
    YTop = A.YTop;
    XRight = A.XRight;
    YBottom = A.YBottom;
    }
  ~XYBounds()
    {
    }
  XYBounds &operator = (const XYBounds &A)
    {
    XLeft = A.XLeft;
    YTop = A.YTop;
    XRight = A.XRight;
    YBottom = A.YBottom;
    return *this;
    }
  bool isEmpty()
    {
    if (XLeft == 0. && YTop == 0. && XRight == 0. && YBottom == 0.)
      return true;
    return false;
    }
  void Clear()
    {
    XLeft = 0.;
    YTop = 0.;
    XRight = 0.;
    YBottom = 0.;
    }
  void InverseY(double YcInv)
    {
    YTop = YcInv - YTop;
    YBottom = YcInv - YBottom;
    }
  void ShiftX(double dX)
    {
    XLeft += dX;
    XRight += dX;
    }
  void ShiftY(double dY)
    {
    YTop += dY;
    YBottom += dY;
    }
  };
#endif
