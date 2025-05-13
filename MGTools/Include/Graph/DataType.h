#if !defined(_DATA_TYPE_INFO_)
#define _DATA_TYPE_INFO_

#include "..\..\stdafx.h"
#include <afxtempl.h>
#include "..\..\Include\Utils\BaseDataType.h"

//const int I_EMPTY = -10000;
//const double D_EMPTY = -10000.;

#define GRAPH_NONE    0
#define GRAPH_2D      1
#define GRAPH_3D      2
#define GRAPH_ISOLINE 3
#define GRAPH_MAP     4

#define WND_2D_GRAPH          (WM_USER+1000)
#define WND_3D_GRAPH          (WM_USER+1003)
#define WND_ISOLINE_GRAPH     (WM_USER+1006)
#define WND_MAP_GRAPH         (WM_USER+1009)

#define LIM_ROUND   0
#define LIM_ELLIPSE 1
#define LIM_RECT    2 

#define SIGN(v) ((v >= 0) ? 1. : -1.)

//HPALETTE CreateColorScalePalette(HDC hDC, int nColor);
//void GetPaletteColor(HPALETTE hP, int ind, float* pCol, int* pICol=0);

typedef struct REGION_LIMITS
{
 double xMin;
 double yMax;
 double xMax;
 double yMin;

  REGION_LIMITS();
  ~REGION_LIMITS();
  void Init();

  REGION_LIMITS(const  REGION_LIMITS& rhs){
      { operator=(rhs);}
     }
  REGION_LIMITS& operator=(const  REGION_LIMITS& rhs);

} WLimits;

typedef struct DRAW_LIMITS
{
  CDRect limR;
  int  limType;
  int nPoints;
  CPoint* pPoints;
  double Angle;
  int EllType;

  DRAW_LIMITS();
  ~DRAW_LIMITS();
  void Init();
  CPoint* GetPolyline(CRect clipReg, int& nP);

  DRAW_LIMITS(const  DRAW_LIMITS& rhs){
      { operator=(rhs);}
     }
  DRAW_LIMITS& operator=(const DRAW_LIMITS& rhs);
  
} DLimits;

typedef struct REGION_VIEW
{
	double left;   // max = 1.
	double top;    // max = 1.
	double right;  // max = 1.
	double bottom; // max = 1.

  REGION_VIEW();
  ~REGION_VIEW();
  void Init();

  REGION_VIEW(const  REGION_VIEW& rhs){
      { operator=(rhs);}
     }
  REGION_VIEW& operator=(const  REGION_VIEW& rhs);

} WRegion;

typedef struct GRID_VIEW
{
  COLORREF XtextColor;
  COLORREF XpenColor;
  COLORREF YtextColor;
  COLORREF YpenColor;
  int penWidth;
  UINT penType;

  BOOL isDotGrid;
  BOOL isXAxis;
  BOOL isYAxis;

  double Ybeg;
  double Xbeg;
  double Ux;
  double Uy;
  int nx;
  int ny;
  CFont* pFont;
  UINT XAlign;
  UINT YAlign;
  CString YName;
  CString XName;

  GRID_VIEW();
  ~GRID_VIEW();
  void Init();

  GRID_VIEW(const  GRID_VIEW& rhs){
      { operator=(rhs);}
     }
  GRID_VIEW& operator=(const GRID_VIEW& rhs);

} WGrid;

typedef struct DRAW_LINE
{
  CArray<double, double> X;
  CArray<double, double> Y;
  CString ToolTipsFormat;
  CString LegendText;

  COLORREF penColor;
  int penWidth;
  UINT penType;

  DRAW_LINE();
  ~DRAW_LINE();
  void Init();

  DRAW_LINE(const  DRAW_LINE& rhs){
      { operator=(rhs);}
     }
  DRAW_LINE& operator=(const DRAW_LINE& rhs);

} WLine;

typedef struct DRAW_ELLIPSE
{
    double x1; 
    double y1;
    double x2;
    double y2;
	CString ToolTipsFormat;
	CString LegendText;
	
	COLORREF penColor;
	int penWidth;
	UINT penType;
	
	DRAW_ELLIPSE();
	~DRAW_ELLIPSE();
	void Init();
	
	DRAW_ELLIPSE(const  DRAW_ELLIPSE& rhs){
		{ operator=(rhs);}
	}
	DRAW_ELLIPSE& operator=(const DRAW_ELLIPSE& rhs);
	
} WEllipse;

typedef struct TEXT_POSITION
{
  double x;
  double y;
  double val;
  CString text;
  UINT Align;

  TEXT_POSITION();
  ~TEXT_POSITION();
    void Init();

    TEXT_POSITION(const  TEXT_POSITION& rhs){
      { operator=(rhs);}
      }
    TEXT_POSITION& operator=(const TEXT_POSITION& rhs);

} TextPos;

typedef struct DRAW_AREA
{
    COLORREF bkColor;
	BOOL EnableToolTips;

	GRID_VIEW Grid;
	REGION_VIEW Region;
	REGION_LIMITS Limits;
	CArray<DRAW_LIMITS, DRAW_LIMITS> DrawLimits;
	CArray<CDPolyLine, CDPolyLine> DrawLimitPolylines;
    CArray<DRAW_LINE, DRAW_LINE> Lines;
	CArray<DRAW_ELLIPSE, DRAW_ELLIPSE> Ellipses;
    CArray<TEXT_POSITION, TEXT_POSITION> Texts;

	CArray<double, double> arFreq;
	CArray<double, double> arRe;
	CArray<double, double> arIm;

	DRAW_AREA();
   ~DRAW_AREA();
    void Init();

    DRAW_AREA(const  DRAW_AREA& rhs){
      { operator=(rhs);}
      }
    DRAW_AREA& operator=(const  DRAW_AREA& rhs);

} WArea;


#endif
