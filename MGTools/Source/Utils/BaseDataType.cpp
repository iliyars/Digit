#include "..\..\StdAfx.h"
#include "math.h"
#include "..\..\Include\Utils\BaseDataType.h"

//=========================== CDPoint ============================
CDPoint::CDPoint()
{
}

CDPoint::CDPoint(double _x, double _y)
{
    x = _x;
    y = _y;
}

CDPoint::CDPoint(CPoint wP)
{
	x = wP.x;
	y = wP.y;
}

CDPoint& CDPoint::operator=(const CDPoint& rhs)
{
      if(this==&rhs) return *this;
        x = rhs.x;
        y = rhs.y;
       return *this;
}

BOOL CDPoint::operator==(const CDPoint& rhs) const
{
   if(x == rhs.x && y == rhs.y)
       return TRUE;
   else
       return FALSE;
}

BOOL CDPoint::operator!=(const CDPoint& rhs) const
{
   if(x != rhs.x || y != rhs.y)
       return TRUE;
   else
       return FALSE;
}

//=========================== CDLine ============================
CDLine::CDLine()
{
	Init();
}

CDLine::CDLine(CDPoint _P1, CDPoint _P2)
{
    P1 = _P1;
    P2 = _P2;
}

void CDLine::Init()
{
  P1.x = 0.; P1.y = 0.;
  P2.x = 0.; P2.y = 0.;
}

double CDLine::GetH()
{
	return (P2.y - P1.y);
}

double CDLine::GetW()
{
	return (P2.x - P1.x);
}

double CDLine::GetD()
{
	double H = GetH();
	double W = GetW();
	double D = sqrt(H*H + W*W);
	return D;
}

double CDLine::GetA()
{
	double H = GetH();
	double W = GetW();
//    double radAngle = atan2(-H, W);
    double radAngle = -atan2(W, -H);
    double Angle = (180.*radAngle/pi);
	return Angle;
}

CDLine& CDLine::operator=(const CDLine& rhs)
{
      if(this==&rhs) return *this;
        P1 = rhs.P1;
        P2 = rhs.P2;
       return *this;
}

BOOL CDLine::operator>(const CDLine& rhs)
{
	if(P1.y > rhs.P1.y && P2.y > rhs.P2.y)
		return TRUE;
	else
		return FALSE;
}

BOOL CDLine::operator<(const CDLine& rhs)
{
	if(P1.y < rhs.P1.y && P2.y < rhs.P2.y)
		return TRUE;
	else
		return FALSE;
}

BOOL CDLine::operator==(const CDLine& rhs) const
{
   if(P1 == rhs.P1 && P2 == rhs.P2)
       return TRUE;
   else
       return FALSE;
}

//=========================== CDPolyLine ============================
CDPolyLine::CDPolyLine()
{
    Init();
}

void CDPolyLine::Init()
{
   Line.RemoveAll();
}

CDPolyLine& CDPolyLine::operator=(const CDPolyLine& rhs)
{
      if(this==&rhs) return *this;
        Line.RemoveAll();
        for(int i=0; i < rhs.Line.GetSize(); i++){
           Line.Add(rhs.Line[i]);
        }
       return *this;
}

//=========================== CDRect ============================
CDRect::CDRect()
{
}

CDRect::CDRect(double _left, double _top, double _right, double _bottom)
{
    left = _left;
    top = _top;
    right = _right;
    bottom = _bottom;
}

CDRect::CDRect(CRect wR)
{
    left = wR.left;
    top = wR.top;
    right = wR.right;
    bottom = wR.bottom;
}

double CDRect::Width()
{
   double W = fabs(right - left);
   return W;
}

double CDRect::Height()
{
   double H = fabs(top-bottom);
   return H;
}

CDPoint CDRect::TopLeft()
{
	CDPoint P(left,top);
	return P;
}

CDPoint CDRect::BottomRight()
{
	CDPoint P(right,bottom);
	return P;
}

CDPoint CDRect::GetCenter(bool TopIsMax/*false*/)
{
	if(TopIsMax){
		CDPoint P(left+(right-left)/2, top+(bottom-top)/2);
		return P;
	}
	else{
		CDPoint P(left+(right-left)/2, bottom+(top-bottom)/2);
		return P;
	}
}

BOOL CDRect::IsPointInside(CDPoint& rhs, bool TopIsMax/*true*/)
{
	if(TopIsMax && left <= rhs.x && rhs.x <= right && bottom <= rhs.y && rhs.y <= top)
		return TRUE;
	else if(!TopIsMax && left <= rhs.x && rhs.x <= right && top <= rhs.y && rhs.y <= bottom)
		return TRUE;

	return FALSE;
}

BOOL CDRect::IsRelRectInside(CDRect& rhs)
{
   double W = fabs(right - left);
   double H = fabs(top-bottom);
   if(rhs.Width() <= W && rhs.Height() <= H){
       return TRUE;
   }
   else{
       return FALSE;
   }
}

BOOL CDRect::IsAbsRectInside(CDRect& rhs)
{
   if(left < rhs.left && rhs.left < right &&
	  left < rhs.right && rhs.right < right &&
	  bottom < rhs.top && rhs.top < top &&
	  bottom < rhs.bottom && rhs.bottom < top){
       return TRUE;
   }
   else{
       return FALSE;
   }
}

void CDRect::NormalizeRect()
{
	double t;
	if(left > right){
		t = left; left = right; right = t;
	}
	if(bottom > top){
		t = top; top = bottom; bottom = t;
	}
}

void CDRect::MultiplyRect(double cst)
{
	left *= cst;
	top *= cst;
	right *= cst;
	bottom *= cst;
}

void CDRect::InflateRect(double infX, double infY)
{
	left -= infX;
	right += infX;
	top += infY;
	bottom -= infY;
}

CDRect& CDRect::operator=(const CDRect& rhs)
{
      if(this==&rhs) return *this;
        left = rhs.left;
        top = rhs.top;
        right = rhs.right;
        bottom = rhs.bottom;
       return *this;
}

BOOL CDRect::operator==(const CDRect& rhs) const
{
   if(left == rhs.left && top == rhs.top && right == rhs.right && bottom == rhs.bottom)
       return TRUE;
   else
       return FALSE;
}
//=========================== CDPolygon ============================
CDPolygon::~CDPolygon()
{
}

CDPolygon::CDPolygon()
{
	Init();
}

void CDPolygon::Init()
{
	Dots.RemoveAll();
	InclineType = INCLINE_NONE;
}

void CDPolygon::SetBoundRect(CDRect _dRect)
{
	Dots.SetSize(4);

	CDPoint P;
	P.x = _dRect.left; P.y = _dRect.top; Dots[0] = P;
	P.x = _dRect.right; P.y = _dRect.top; Dots[1] = P;
	P.x = _dRect.right; P.y = _dRect.bottom; Dots[2] = P;
	P.x = _dRect.left; P.y = _dRect.bottom; Dots[3] = P;
//	SetPrecision();
}

void CDPolygon::SetDots(CArray<CDPoint, CDPoint>& _Dots)
{
	int nS = _Dots.GetSize();
	Dots.SetSize(nS);
	for(int i=0; i < nS; i++){
		Dots[i] = _Dots[i];
	}
//	SetPrecision();
}

double CDPolygon::Width()
{
	double W = GetRight() - GetLeft();
	W = DtoD(W, DIGIT_PRECISION);
	return W;
}

double CDPolygon::Height()
{
	double H = GetBottom() - GetTop();
	H = DtoD(H, DIGIT_PRECISION);
	return H;
}

void CDPolygon::ApplyAngle(double radAngle)
{
	CDRect dR;
	dR.left = Dots[0].x;
	dR.top = Dots[0].y;
	dR.right = Dots[2].x;
	dR.bottom = Dots[2].y;

    double ShX =  dR.Height() * tan(radAngle);
    Dots[0].x += ShX;
    Dots[1].x += ShX;
	SetPrecision();
}

BOOL CDPolygon::IsEmpty()
{
	if(Dots.GetSize()==0)
		return TRUE;
	else
		return FALSE;
}

BOOL CDPolygon::IsPointInside(CDPoint& P)
{
	double left = GetLeft();
	double top = GetTop();
	double right = GetRight();
	double bottom = GetBottom();
	if(left <= P.x && P.x <= right && top <= P.y && P.y <= bottom)
		return TRUE;
	else
		return FALSE;
}

double CDPolygon::GetLeft()
{
	double d = 1.e10;
	for(int i=0; i < Dots.GetSize(); i++){
		d = __min(d, Dots[i].x);
	}
	d = DtoD(d, DIGIT_PRECISION);
	return d;
}

double CDPolygon::GetBottom()
{
	double d = -1.e10;
	for(int i=0; i < Dots.GetSize(); i++){
		d = __max(d, Dots[i].y);
	}
	d = DtoD(d, DIGIT_PRECISION);
	return d;
}

double CDPolygon::GetRight()
{
	double d = -1.e10;
	for(int i=0; i < Dots.GetSize(); i++){
		d = __max(d, Dots[i].x);
	}
	d = DtoD(d, DIGIT_PRECISION);
	return d;
}

double CDPolygon::GetTop()
{
	double d = 1.e10;
	for(int i=0; i < Dots.GetSize(); i++){
		d = __min(d, Dots[i].y);
	}
	d = DtoD(d, DIGIT_PRECISION);
	return d;
}

CDPoint CDPolygon::GetCenter()
{
	CDPoint cP;
	double left = GetLeft();
	double right = GetRight();
	double top = GetTop();
	double bottom = GetBottom();
	cP.x = left + (right-left)/2;
	cP.y = bottom + (top-bottom)/2;
	cP.x = DtoD(cP.x, DIGIT_PRECISION);
	cP.y = DtoD(cP.y, DIGIT_PRECISION);
	return cP;
}


bool CDPolygon::SetNewCenter(double x, double y)
{
	ShiftToX0Y0();
    CDPoint cP = GetCenter();
	double difX = x - cP.x;
	double difY = y - cP.y;
	ShiftCenter(difX, difY);
	SetPrecision();
	return true;
}

void CDPolygon::ShiftToX0Y0()
{
	double left, bottom;
	left = bottom = 1.e10;
	CDPoint cP = GetCenter();
	left = cP.x;
	bottom = cP.y;
    for(int i=0; i < Dots.GetSize(); i++){
	  Dots[i].x -= left;
	  Dots[i].y -= bottom;
    }
	SetPrecision();
}

bool CDPolygon::ShiftCenter(double x, double y)
{
	for(int i=0; i < Dots.GetSize(); i++){
		Dots[i].x += x;
		Dots[i].y += y;
	}
	SetPrecision();
	return true;
}

void CDPolygon::Inflate(double tolX, double tolY)
{
	if(Dots.GetSize()){
		Dots[0].x -= tolX;
		Dots[0].y -= tolY;
		Dots[1].x += tolX;
		Dots[1].y -= tolY;
		Dots[2].x += tolX;
		Dots[2].y += tolY;
		Dots[3].x -= tolX;
		Dots[3].y += tolY;
	}
}

void CDPolygon::Multiplay(double mp)
{
	for(int i=0; i < Dots.GetSize(); i++){
		Dots[i].x *= mp;
		Dots[i].y *= mp;
	}
}

void CDPolygon::MakeMirror()
{
	if(Dots.GetSize()==0)
		return;
/*	CDRect dR;
	dR.left = GetLeft();
	dR.right = GetRight();
	dR.top = GetTop();
	dR.bottom = GetBottom();
	dR.NormalizeRect();
	SetBoundRect(dR);*/
	CArray<CDPoint, CDPoint> swDots;
	swDots.SetSize(4);
	swDots[0] = Dots[2];
	swDots[1] = Dots[3];
	swDots[2] = Dots[0];
	swDots[3] = Dots[1];
}

void CDPolygon::SetPrecision(int didgit)
{
	double d;
	int nd,  i;
	double prec = 1.;
	double toler = 0.49;
	for(i=0; i < didgit; i++){
	   prec *= 10.;
	}
	toler /= prec;
	for(i=0; i < Dots.GetSize(); i++){
	 	d = Dots[i].x;
		nd = (int)(d*prec);
		d = nd/prec;
	    if(Dots[i].x - d > toler)
		   d = d + 1/prec;
	 	Dots[i].x = d;
	 	d = Dots[i].y;
		nd = (int)(d*prec);
		d = nd/prec;
	    if(Dots[i].y - d > toler)
		   d = d + 1/prec;
	 	Dots[i].y = d;
	}
}

CDPolygon& CDPolygon::operator=(const CDPolygon& rhs)
{
      if(this==&rhs) return *this;
         Dots.RemoveAll();
         InclineType = rhs.InclineType;
         for(int i=0; i < rhs.Dots.GetSize(); i++){
          Dots.Add(rhs.Dots[i]);
         }
       return *this;
}

BOOL CDPolygon::operator==(const CDPolygon& rhs) const
{
  if(Dots.GetSize() != rhs.Dots.GetSize())
	  return FALSE;

  for(int i=0; i < Dots.GetSize(); i++){
	  if(Dots[i] != rhs.Dots[i])
         return FALSE;
  }

  return TRUE;
}

double DtoD(double _d, int iDigit, bool needTol/*true*/)
{
    double d = _d + 0.00000001;
    double prec = 1.;
    double toler = 0.49;
    for(int i=0; i < iDigit; i++){
       prec *= 10.;
    }
    int nd = (int)(d*prec);
    d = nd/prec;
    toler /= prec;
    if(needTol && _d - d > toler)
        d = d + 1./prec;
    return d;
}
