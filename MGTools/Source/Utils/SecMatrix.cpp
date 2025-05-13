#include "..\..\stdafx.h"
#include "..\..\Include\Utils\SecMatrix.h"
#include "..\..\Include\Utils\Utils.h"

CMatrixSection::CMatrixSection()
{
   pMatr = NULL;
   Nx = Ny = 0;
   dX = dY = 0.;
   minX = maxX = minY = maxY;
   grd_rd = 0.01745329;
   rd_grd = 57.2957795;
}

CMatrixSection::~CMatrixSection()
{
	delete[] pMatr;
}

void CMatrixSection::SetMatrix(float* _pMatr, int _nx, int _ny)
{
	if(pMatr)
		delete[] pMatr;
	
	Nx = _nx;
	Ny = _ny;
     
	int n = Nx*Ny;
	pMatr = new double[n];
	for(int i=0; i < n; i++){
	   pMatr[i] = _pMatr[i];
	}
}

void CMatrixSection::SetMatrix(double* _pMatr, int _nx, int _ny)
{
	if(pMatr)
		delete[] pMatr;
	
	Nx = _nx;
	Ny = _ny;
     
	int n = Nx*Ny;
	pMatr = new double[n];
	for(int i=0; i < n; i++){
	   pMatr[i] = _pMatr[i];
	}
}

void CMatrixSection::SetLimits(double _minX, double _maxX, double _minY, double _maxY)
{
	minX = _minX;
	maxX = _maxX;
	minY = _minY;
	maxY = _maxY;

	dX = (maxX - minX)/(Nx-1);
	dY = (maxY - minY)/(Ny-1);
}

bool CMatrixSection::CanCalc()
{
	if(pMatr==NULL || Nx==0 || Ny==0)
		return false;
	else
		return true;
}

bool CMatrixSection::GetSymmetricSection(double Angle, int nStepSec, double* pR, double* pF)
{
	if(!CanCalc())
	  return false;
	
   double x, y, RAng;
   double Nc;
   double Step = (maxX - minX)/(nStepSec-1);
   Nc = double(nStepSec-1) / 2.;
   RAng = Angle * grd_rd;
   for (int i=0; i < nStepSec; i++){
       pR[i] = double(i - Nc)*Step;
       x = -sin(RAng) * pR[i];
       y = cos(RAng) * pR[i];
       pF[i] = Parinv4P18(x, y);
   }
	return true;
}

bool CMatrixSection::GetLogSymmetricSection(double Angle, int nStepSec, double* pR, double* pF)
{
	if(!CanCalc())
	  return false;
	
   double x, y, RAng;
   double Nc;
   double Step = (maxX - minX)/(nStepSec-1);
   Nc = double(nStepSec-1) / 2.;
   RAng = Angle * grd_rd;
   for (int i=0; i < nStepSec; i++){
       pR[i] = double(i - Nc)*Step;
       x = -sin(RAng) * pR[i];
       y = cos(RAng) * pR[i];
       pF[i] = log10(Parinv4P18(x, y));
   }
   return true;
}

double CMatrixSection::Parinv4P18(double X, double Y)
  {
  float E18=0.00005f;
  double E2 = 1.e-2;
  double R1x, R2x;
  double R1y, R2y;
  int k;
  bool KeyOutMatr = false;
  double F;
//------------------------------------------------------------------------
  int NxC = Nx / 2;
  int NyC = Ny / 2;
  double X1 = (X + NxC * dX) / dX;
  int Kx = int(X1);
  double Y1 = (Y + NyC * dY) / dY;
  int Ky = int(Y1);
//------------------------------------------------------------------------
  if (Kx < 0 && Ky < 0)
    {
    R1x = X1;
    R1y = Y1;
    k = 0;
    KeyOutMatr = true;;
    }
  else if (Kx > Nx-1 && Ky > Ny-1)
    {
    R1x = X1 - (Nx-1);
    R1y = Y1 - (Ny-1);
    k = Nx * Ny - 1;
    KeyOutMatr = true;;
    }
  else if (Kx < 0 && Ky > Ny-1)
    {
    R1x = X1;
    R1y = Y1 - (Ny-1);
    k = (Ny-1) * Nx;
    KeyOutMatr = true;;
    }
  else if (Kx > Nx-1 && Ky < 0)
    {
    R1x = X1 - (Nx-1);
    R1y = Y1;
    k = Nx - 1;
    KeyOutMatr = true;;
    }
  if (KeyOutMatr)
    {
    if (fabs(sqrt(R1x*R1x+R1y*R1y)) < E2)
      F = pMatr[k];
    else
      F = E18;
    return F;
    }
//------------------------------------------------------------------------
  if (Kx < 0)
    {
    R1x = X1;
    k = Ky * Nx;
    KeyOutMatr = true;;
    }
  else if (Kx >= Nx-1)
    {
    R1x = X1 - (Nx-1);
    k = Ky * Nx + (Nx-1);
    KeyOutMatr = true;;
    }
  if (KeyOutMatr)
    {
    R1y = Y1 - Ky;
    R2y = 1. - R1y;
    if (fabs(sqrt(R1x*R1x+R1y*R1y)) < E2)
      F = pMatr[k];
    else if (fabs(sqrt(R1x*R1x+R2y*R2y)) < E2)
      F = pMatr[k+Nx];
    else
      F = E18;
    return F;
    }
//------------------------------------------------------------------------
  if (Ky < 0)
    {
    R1y = Y1;
    k = Kx;
    KeyOutMatr = true;;
    }
  else if (Ky >= Ny-1)
    {
    R1y = Y1 - (Ny-1);
    k = (Ny-1) * Nx + Kx;
    KeyOutMatr = true;;
    }
  if (KeyOutMatr)
    {
    R1x = X1 - Kx;
    R2x = 1.- R1x;
    if (fabs(sqrt(R1x*R1x+R1y*R1y)) < E2)
      F = pMatr[k];
    else if (fabs(sqrt(R2x*R2x+R1y*R1y)) < E2)
      F = pMatr[k+1];
    else
      F = E18;
    return F;
    }
//------------------------------------------------------------------------
  R1x = X1 - Kx;
  R2x = 1.- R1x;
  R1y = Y1 - Ky;
  R2y = 1.- R1y;
  k = Ky * Nx + Kx;
  if (pMatr[k] != E18 && pMatr[k+Nx] != E18 && pMatr[k+1] != E18 && pMatr[k+Nx+1] != E18)
    {  
    double Z1 = pMatr[k] * R2y + pMatr[k+Nx] * R1y;
    double Z2 = pMatr[k+1] * R2y + pMatr[k+Nx+1] * R1y;
    F = Z1 * R2x + Z2 * R1x;
    }
  else  if (fabs(sqrt(R1x*R1x+R1y*R1y)) < E2)
    F = pMatr[k];
  else if (fabs(sqrt(R2x*R2x+R1y*R1y)) < E2)
    F = pMatr[k+1];
  else if (fabs(sqrt(R1x*R1x+R2y*R2y)) < E2)
    F = pMatr[k+Nx];
  else if (fabs(sqrt(R2x*R2x+R2y*R2y)) < E2)
    F = pMatr[k+Nx+1];
  else
    F = E18;
  return F;
  }          
/*
double CMatrixSection::Parinv4P18(double X, double Y)
{
  double E17=-1.e17;
  float E18=0.00005f;
  int NxC = Nx / 2;
  int NyC = Ny / 2;
  double X1 = (X + NxC * dX) / dX;
  int Kx = int(X1);
  if (Kx < 0)
    Kx = 0;
  if (Kx > Nx-2)
    Kx = Nx - 2;
  double R1x = X1 - Kx;
  double R2x=1. - R1x;
  double Y1 = (Y + NyC * dY) / dY;
  int Ky = int(Y1);
  if (Ky < 0)
    Ky = 0;
  if (Ky > Ny-2)
    Ky = Ny - 2;
  double R1y = Y1 - Ky;
  double R2y = 1. - R1y;
  double F;
  int l = Ky * Nx + Kx;
  if (pMatr[l] != E18 && pMatr[l+Nx] != E18 && pMatr[l+1] != E18 && pMatr[l+Nx+1] != E18)
    {  
    double Z1 = pMatr[l] * R2y + pMatr[l+Nx] * R1y;
    double Z2 = pMatr[l+1] * R2y + pMatr[l+Nx+1] * R1y;
    F = Z1 * R2x + Z2 * R1x;
    }
  else
    F = E18;
  return F;
}					 
*/
