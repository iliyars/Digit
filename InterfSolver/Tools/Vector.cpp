#include "Vector.h"

//============================================================================
Vector::Vector(void)
  {
  }
//============================================================================
Vector::Vector(int Mv)
  {
  Vect.RemoveAll();
  Vect.SetSize(Mv);
  }
//============================================================================
Vector::~Vector()
  {
  Vect.RemoveAll();
  }
//============================================================================
void Vector::SetSize(int Mv)
  {
  Vect.RemoveAll();
  Vect.SetSize(Mv);
  }
//============================================================================
void Vector::Init(double R)
  {
  int Mv = Vect.GetSize();
  for (int i = 0; i < Mv; i++) 
    Vect[i] = R;
  }
//============================================================================
void Vector::Init(double  A0,...)
  {
	va_list ap;
  double  Ai;

	va_start(ap, A0);

  Vect[0] = A0;
  int Nv = Vect.GetSize(); 
  for (int i = 1; i < Nv; i++) 
    {
    Ai= va_arg(ap,double );
    Vect[i]= Ai;
    }

	va_end(ap);
  }
//============================================================================
Vector& Vector::operator= (const Vector &A)
  {
  if(this == &A) 
    return *this;
  int Nv = Vect.GetSize(); 
  if (Nv == A.GetSize()) 
    {
    for (int i = 0; i < Nv; i++) 
      Vect[i] = A[i];
    }
  else 
    {
    Nv= A.GetSize();
    Vect.SetSize(Nv);
    for (int i = 0; i < Nv; i++) 
      Vect[i]=A[i];
    }
  return *this;
  }
//============================================================================
Vector operator+ (const Vector &A, const Vector &B)
  {
  int Mv=A.GetSize();
  Vector C(Mv);
  for (int i = 0; i < Mv; i++) 
    C[i] = A[i] + B[i];
	return C;
 }
//============================================================================
Vector operator- (const Vector &A, const Vector &B)
  {
  int Mv = A.GetSize();
  Vector C(Mv);
  for (int i = 0; i < Mv; i++) 
    C[i] = A[i] - B[i];
	return C;
  }
//============================================================================
Vector operator* (double b, const Vector &A)
  {
  int Mv=A.GetSize();
  Vector C(Mv);
  for (int i = 0; i < Mv; i++) 
  C[i] = b * A[i];
	return C;
  }
//============================================================================
Vector operator* (const Vector &A, double  b)
  {
  int Mv=A.GetSize();
  Vector C(Mv);
  for (int i = 0; i < Mv; i++) 
    C[i] = b * A[i];
	return C;
  }
//============================================================================
Vector operator/ (const Vector &A, double  b)
  {
  int Mv=A.GetSize();
    Vector C(Mv);
  for (int i = 0; i < Mv; i++) 
    C[i] =A[i] / b;
	return C;
  }
//============================================================================
double  operator* (const Vector &A, const Vector &B)
  {
  int Mv=A.GetSize();
  double S = 0.;
  for (int i = 0; i < Mv; i++) 
  S += A[i] * B[i];
  return S;
  }
//============================================================================
Vector operator^ (const Vector &A, const Vector &B)
  {
  int Mv=A.GetSize();
  Vector C(Mv);
  if (Mv != 3) 
    {
    AfxMessageBox("The Cross product is defined for N=3\n\r              <<Vector>>", 
                                                       MB_OK | MB_ICONEXCLAMATION);
    return C;
    }
  C[0] = A[1] * B[2] - A[2] * B[1];
  C[1] = A[2] * B[0] - A[0] * B[2];
  C[2] = A[0] * B[1] - A[1] * B[0];
	return C;
  }
//============================================================================
double  Mod(const Vector &A)
  {
  int Mv=A.GetSize();
  double R;
  double S = 0.;
  for (int i = 0; i < Mv; i++) 
    S += A[i] * A[i];
  R = sqrt(S);
  return R;
  }
//============================================================================
Vector UniMod(const Vector& A)
  {
  int Mv=A.GetSize();
  Vector C(Mv);
    double  D = Mod(A);
  C = A / D;
	return C;
  }
//=====================================================================================================================
