#ifndef _CLASSICABERR_H_
#define _CLASSICABERR_H_

#include "XYPolynom.h"

#define  POWER   0
#define  ZERNIKE  1

//============================================================================
struct SPH_COEFF
  {
  public:
    double D;
    double Lx;
    double Ly;
    double C;
  public:
    SPH_COEFF();
    SPH_COEFF(const SPH_COEFF &Sph);
    SPH_COEFF(double _D, double _Lx, double _Ly, double _C);
    ~SPH_COEFF();
    SPH_COEFF& operator= (const SPH_COEFF &Sph);
    double CalcValue(double X, double Y);
    XYPolynom ConvToPolynom () const;
    void Clear();
    void Delete();

  friend XYPolynom operator+ (const XYPolynom &A, const SPH_COEFF &B);
  friend XYPolynom operator- (const XYPolynom &A, const SPH_COEFF &B);
  };
//============================================================================
struct AST_COEFF
  {
  public:
    double A;
    double Fa;
  public:
    AST_COEFF();
    AST_COEFF(const AST_COEFF &Ast);
    AST_COEFF(double _A, double _Fa);
    ~AST_COEFF();
    AST_COEFF& operator= (const AST_COEFF &Ast);
    double CalcValue(double X, double Y);
    XYPolynom ConvToPolynom () const;
    void Clear();
    void Delete();

  friend XYPolynom operator+ (const XYPolynom &A, const AST_COEFF &B);
  friend XYPolynom operator- (const XYPolynom &A, const AST_COEFF &B);
  };
//============================================================================
struct COM_COEFF
  {
  public:
    double C;
    double Fc;
  public:
    COM_COEFF();
    COM_COEFF(const COM_COEFF &Coma);
    COM_COEFF(double _C, double _Fc);
    ~COM_COEFF();
    COM_COEFF& operator= (const COM_COEFF &Coma);
    double CalcValue(double X, double Y);
    XYPolynom ConvToPolynom () const;
    void Clear();
    void Delete();

  friend XYPolynom operator+ (const XYPolynom &A, const COM_COEFF &B);
  friend XYPolynom operator- (const XYPolynom &A, const COM_COEFF &B);
  };
//============================================================================
struct ZON_COEFF
  {
  public:
    int Type;
    CArrayDouble Coeff;

  public:
    ZON_COEFF();
    ZON_COEFF(const ZON_COEFF &A);
    ZON_COEFF(int NCoeff, int _Type = POWER);
    ZON_COEFF(const CArrayDouble &ArrCoeff, int _Type = POWER);
    ~ZON_COEFF();
    int GetSize() const {return Coeff.GetSize();}
    void ChangeSize(int NewSize);
    void ChangeType(int NewType);
    ZON_COEFF& operator= (const ZON_COEFF &A);
    const double operator[] (int iC) const {return Coeff[iC];}
    double &operator[] (int iC) {return Coeff[iC];}
    double CalcValue(double X, double Y);
    XYPolynom ConvToPolynom () const;
    void Delete();
    void Clear();
  private:
    void ConvToPower();
    void ConvToZernike();

  friend ZON_COEFF operator+ (const ZON_COEFF &A, const ZON_COEFF &B);
  friend XYPolynom operator+ (const XYPolynom &A, const ZON_COEFF &B);
  friend XYPolynom operator- (const XYPolynom &A, const ZON_COEFF &B);
  };
//============================================================================
#endif
