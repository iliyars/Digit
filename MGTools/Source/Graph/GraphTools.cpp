#include "..\..\Include\Graph\GraphTools.h"

void GetDCRegion(CDC* pDC, double rangeX, double rangeY, int& regX, int& regY, double& scale)
{
   CSize reg(regX, regY);
   pDC->DPtoHIMETRIC(&reg);

   double Sx = reg.cx/rangeX;
   double Sy = reg.cy/rangeY;

   double S = __min(Sx, Sy);
   if(S == Sx){
       int cy = (int)(S * rangeY);
       CSize t(0, cy);
       pDC->HIMETRICtoDP(&t);
       regY = t.cy;
   }
   else if(S == Sy){
       int cx = (int)(S * rangeX);
       CSize t(cx, 0);
       pDC->HIMETRICtoDP(&t);
       regX = t.cx;
   }
   scale = S;
}

