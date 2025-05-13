#ifndef _POLAPPROXIMATION_H_
#define _POLAPPROXIMATION_H_
#include "XYPolynom.h"
#include "SamplDat.h"
#include "Matrix.h"
#include "Vector.h"

//==============================================================
class PolApproximation
  {
  public:
    Matrix AMtr;

  protected:
    int MaxPow;
    double Epst;
    double Fish;
    SAMPLE_DATA Sampl;
    int PP;
    int PP1;
    int PH;
    int PH1;
    int PH2;
    CArrayDouble SumXY;
    CArrayDouble SumFXY;
    XYPolynom CurPol;

  public:
    PolApproximation();
    ~PolApproximation();
    void Set(int _MaxPow, double _Epst, double _Fish, const SAMPLE_DATA &_Sampl);
    bool Calc(XYPolynom &Pol, CArrayDouble &ArrRMS, CArrayDouble &ArrDisp);


  protected:
    void CreateMaxMatr();
    bool CalcCurMatr();
    void CalcRMSDisp(double &RMS, double &Disp);
    void Clear();
  };
#endif

