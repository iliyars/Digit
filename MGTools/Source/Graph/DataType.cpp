#include "..\..\Include\Graph\DataType.h"
#include "..\..\Include\Graph\Def.h"
#include "..\..\Include\Utils\Utils.h"

//=======================  Limits  ===============================
REGION_LIMITS::REGION_LIMITS()
{
	Init();
}

REGION_LIMITS::~REGION_LIMITS()
{
}

void REGION_LIMITS::Init()
{
	xMin = D_EMPTY;
	xMax = D_EMPTY;
	yMin = D_EMPTY;
	yMax = D_EMPTY;
}

REGION_LIMITS& REGION_LIMITS::operator=(const REGION_LIMITS& rhs)
{
   if(this==&rhs) return *this;
	xMin = rhs.xMin;
	xMax = rhs.xMax;
	yMin = rhs.yMin;
	yMax = rhs.yMax;
    return *this;
}
//==========================================================
DRAW_LIMITS::DRAW_LIMITS()
{
	Init();
}

DRAW_LIMITS::~DRAW_LIMITS()
{
  if(pPoints)
    delete[] pPoints;
  pPoints = NULL;
}

void DRAW_LIMITS::Init()
{
   limR = CDRect(-1., -1., -1., -1.);
   limType = -1;
   nPoints = 51;
   pPoints = NULL;
   Angle = 0.;
   EllType = -1;
}

DRAW_LIMITS& DRAW_LIMITS::operator=(const DRAW_LIMITS& rhs)
{
   if(this==&rhs) return *this;
	limR = rhs.limR;
	limType = rhs.limType;
	nPoints = rhs.nPoints;
	pPoints = rhs.pPoints;
    Angle = rhs.Angle;
    EllType = rhs.EllType;
    return *this;
}

CPoint* DRAW_LIMITS::GetPolyline(CRect clipReg, int& nP)
{
    if(pPoints==NULL)
		pPoints = new CPoint[nPoints*4];

	CPoint cP = clipReg.CenterPoint();
	if(limType == LIM_RECT){
	  nP = 5;
	  pPoints[0] = CPoint(clipReg.left, clipReg.top);
	  pPoints[1] = CPoint(clipReg.right, clipReg.top);
	  pPoints[2] = CPoint(clipReg.right, clipReg.bottom);
	  pPoints[3] = CPoint(clipReg.left, clipReg.bottom);
	  pPoints[4] = CPoint(clipReg.left, clipReg.top);
	}
	else{
	  nP = nPoints*4;
      CreateEllipseArray(&clipReg, pPoints, nPoints);
	  if(Angle){
          double grd_rd = 0.01745329;
		  double fi = Angle*grd_rd;
          double cosFi = cos(fi);
          double sinFi = sin(fi);
		  for(int i=0; i < nP; i++){
			  double x = pPoints[i].x-cP.x;
			  double y = pPoints[i].y-cP.y;
			  pPoints[i].x = (int)(x*cosFi + y*sinFi);
			  pPoints[i].y = (int)(-x*sinFi + y*cosFi);
			  pPoints[i].x +=  cP.x;
			  pPoints[i].y +=  cP.y;
		  }
	  }
	}
	return pPoints;
}

//=======================  Regions  ===============================
REGION_VIEW::REGION_VIEW()
{
	Init();
}

REGION_VIEW::~REGION_VIEW()
{
}

void REGION_VIEW::Init()
{
	left = 0.1;
	top = 0.1;
	right = 0.9;
	bottom = 0.9;
}

REGION_VIEW& REGION_VIEW::operator=(const REGION_VIEW& rhs)
{
   if(this==&rhs) return *this;
	left = rhs.left;
	top = rhs.top;
	right = rhs.right;
	bottom = rhs.bottom;
    return *this;
}

//=======================  Grid  ===============================
GRID_VIEW::GRID_VIEW()
{
	Init();
}

GRID_VIEW::~GRID_VIEW()
{
	if(pFont){
		delete pFont;
	}
}

void GRID_VIEW::Init()
{
  XtextColor = RGB(255,255,255);
  XpenColor = RGB(255,255,255);
  YtextColor = RGB(255,255,255);
  YpenColor = RGB(255,255,255);
  penWidth = 0;
  penType = PS_SOLID;

  isDotGrid = TRUE; 
  isXAxis = TRUE;
  isYAxis = TRUE;

  Ybeg = D_EMPTY;
  Xbeg = D_EMPTY;
  Ux = D_EMPTY;
  Uy = D_EMPTY;
  pFont = 0;
  nx = 0;
  ny = 0;
  XAlign = TA_RIGHT|TA_BOTTOM;
  YAlign = TA_RIGHT|TA_TOP;
  XName.Empty();
  YName.Empty();
}

GRID_VIEW& GRID_VIEW::operator=(const GRID_VIEW& rhs)
{
   if(this==&rhs) return *this;
      XtextColor = rhs.XtextColor;
      XpenColor = rhs.XpenColor;
      YtextColor = rhs.YtextColor;
      YpenColor = rhs.YpenColor;
      penWidth = rhs.penWidth;
      penType = rhs.penType;
      isDotGrid = rhs.isDotGrid; 
      isXAxis = rhs.isXAxis;
      isYAxis = rhs.isYAxis;
	  Ybeg = rhs.Ybeg;
	  Xbeg = rhs.Xbeg;
	  Ux = rhs.Ux;
	  Uy = rhs.Uy;
	  nx = rhs.nx;
	  ny = rhs.ny;
	  pFont = rhs.pFont;
	  XAlign = rhs.XAlign;
	  YAlign = rhs.YAlign;
	  XName = rhs.XName;
	  YName = rhs.YName;
    return *this;
}

//=======================  Line  ===============================
DRAW_LINE::DRAW_LINE()
{
	Init();
}

DRAW_LINE::~DRAW_LINE()
{
}

void DRAW_LINE::Init()
{
    penColor = RGB(255,255,255);
    penWidth = 0;
	penType = PS_SOLID;
    LegendText.Empty();

	X.RemoveAll();
	Y.RemoveAll();
	ToolTipsFormat.Empty();
}

DRAW_LINE& DRAW_LINE::operator=(const DRAW_LINE& rhs)
{
	int i =0;
   if(this==&rhs) return *this;
       LegendText = rhs.LegendText;
       penColor = rhs.penColor;
       penWidth = rhs.penWidth;
	   penType = rhs.penType;
	   ToolTipsFormat = rhs.ToolTipsFormat;
        X.RemoveAll();
        for(i=0; i < rhs.X.GetSize(); i++){
          X.Add(rhs.X[i]);
        }
        Y.RemoveAll();
        for(i=0; i < rhs.Y.GetSize(); i++){
          Y.Add(rhs.Y[i]);
        }
    return *this;
}

//=======================  Ellipse  ===============================
DRAW_ELLIPSE::DRAW_ELLIPSE()
{
	Init();
}

DRAW_ELLIPSE::~DRAW_ELLIPSE()
{
}

void DRAW_ELLIPSE::Init()
{
    x1 = 0.;
    y1 = 0.;
    x2 = 0.;
    y2 = 0.;
    penColor = RGB(255,255,255);
    penWidth = 0;
	penType = PS_SOLID;
    LegendText.Empty();
	
	ToolTipsFormat.Empty();
}

DRAW_ELLIPSE& DRAW_ELLIPSE::operator=(const DRAW_ELLIPSE& rhs)
{
	if(this==&rhs) return *this;
	LegendText = rhs.LegendText;
	penColor = rhs.penColor;
	penWidth = rhs.penWidth;
	penType = rhs.penType;
	ToolTipsFormat = rhs.ToolTipsFormat;
	x1 = rhs.x1;
	y1 = rhs.y1;
	x2 = rhs.x2;
	y2 = rhs.y2;
	return *this;
}

//============================ Text Position =====================
TEXT_POSITION::TEXT_POSITION()
{
	Init();
}

TEXT_POSITION::~TEXT_POSITION()
{
}

void TEXT_POSITION::Init()
{
	x = 0.; 
	y = 0.; 
	val = 0.;
	text.Empty();
	Align = TA_LEFT|TA_BOTTOM;
}

TEXT_POSITION& TEXT_POSITION::operator=(const TEXT_POSITION& rhs)
{
   if(this==&rhs) return *this;
     x = rhs.x;
     y = rhs.y;
     val = rhs.val;
	 text = rhs.text;
	 Align = rhs.Align;
   return *this;
}

//=======================  Draw Area  ===============================
DRAW_AREA::DRAW_AREA()
{
	Init();
}

DRAW_AREA::~DRAW_AREA()
{
}

void DRAW_AREA::Init()
{
	bkColor = RGB(0,0,0);
    EnableToolTips = FALSE;
	Region.Init();
	Limits.Init();
	DrawLimits.RemoveAll();
	DrawLimitPolylines.RemoveAll();
	Grid.Init();
    
	arFreq.RemoveAll();
    arRe.RemoveAll();
    arIm.RemoveAll();
}

DRAW_AREA& DRAW_AREA::operator=(const DRAW_AREA& rhs)
{
	int i =0;
   if(this==&rhs) return *this;
     Region = rhs.Region;
     Limits = rhs.Limits;
	 Grid = rhs.Grid;
	 bkColor = rhs.bkColor;
	 EnableToolTips = rhs.EnableToolTips;
     Lines.RemoveAll();
        for(i=0; i < rhs.Lines.GetSize(); i++){
          Lines.Add(rhs.Lines[i]);
        }
     DrawLimits.RemoveAll();
        for(i=0; i < rhs.DrawLimits.GetSize(); i++){
          DrawLimits.Add(rhs.DrawLimits[i]);
        }
	 DrawLimitPolylines.RemoveAll();	
        for(i=0; i < rhs.DrawLimitPolylines.GetSize(); i++){
          DrawLimitPolylines.Add(rhs.DrawLimitPolylines[i]);
        }
     Texts.RemoveAll();
        for(i=0; i < rhs.Texts.GetSize(); i++){
          Texts.Add(rhs.Texts[i]);
        }
     arFreq.RemoveAll();
	 arRe.RemoveAll();
	 arIm.RemoveAll();
        for(i=0; i < rhs.arFreq.GetSize(); i++){
          arFreq.Add(rhs.arFreq[i]);
          arRe.Add(rhs.arRe[i]);
          arIm.Add(rhs.arIm[i]);
        }
    return *this;
}
