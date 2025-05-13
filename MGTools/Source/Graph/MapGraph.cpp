// MapGraph.cpp : implementation file
//

#include "..\..\stdafx.h"
#include "..\..\Include\Graph\MapGraph.h"
#include "..\..\Include\Graph\def.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMapGraph

IMPLEMENT_DYNCREATE(CMapGraph, C2DGraph)

CMapGraph::CMapGraph(LPCTSTR _iniFile/*NULL*/)
                   :C2DGraph(_iniFile)
{
	pMatr = NULL;
	pMatrAct = NULL;
	pMatrLog = NULL;
    nx = ny = 0;
	pX = pY = NULL;
	hPal = NULL;
    bRender = FALSE;
  isEnableCoor = FALSE;
}

CMapGraph::~CMapGraph()
{
}

void CMapGraph::OnDestroy()
{
	Delete();
    CScrollView::OnDestroy();
}

void CMapGraph::Delete()
{
	if(pMatrAct)
	  delete[] pMatrAct;
	if(pMatrLog)
	  delete[] pMatrLog;

	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
	if(pX)
		delete[] pX;
	if(pY)
		delete[] pY;
}

CRuntimeClass* CMapGraph::GetRunTime()
{
	return RUNTIME_CLASS(CMapGraph);
}

void CMapGraph::OnInitialUpdate()
{
	C2DGraph::OnInitialUpdate();
	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
	CClientDC dc(this);
    hPal = CreateColorScalePalette(dc.m_hDC, COLOR_MESH_SCALE);
}

void CMapGraph::CreatePalette()
{
	C2DGraph::OnInitialUpdate();
	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
	CClientDC dc(this);
    hPal = CreateColorScalePalette(dc.m_hDC, COLOR_MESH_SCALE);
}

void CMapGraph::Set3DMatrix(float* _pMatr, int _nx, int _ny)
{
	int i =0;
   minZAct = 1.e10;
   maxZAct = -1.e10;
   minZLog = 1.e10;
   maxZLog = -1.e10;
	   
   nx = _nx;
   ny = _ny;
   if(pX)
	   delete[] pX;
   if(pY)
	   delete[] pY;
   pX = new double[nx];
   pY = new double[ny];
   int n = nx*ny;
   if(pMatrAct)
	   delete[] pMatrAct;
   if(pMatrLog)
	   delete[] pMatrLog;
   pMatrAct = new double[n];
   pMatrLog = new double[n];
   double val;
   for(i=0; i < n; i++){
	  val = _pMatr[i];
	  if(val == min_level)
		  continue;
      minZAct = __min(val, minZAct);
      maxZAct = __max(val, maxZAct);
   }
   
   Coef_z_Log= (maxZAct-minZAct)/log((maxZAct-minZAct+LogDelta)/LogDelta);
   LogShift= minZAct-Coef_z_Log*log(LogDelta);

   for(i=0; i < n; i++){
	  if(_pMatr[i] == min_level)
		  continue;
	  val = Coef_z_Log*log(_pMatr[i]-minZAct+LogDelta)+LogShift;
      minZLog = __min(val, minZLog);
      maxZLog = __max(val, maxZLog);
   }

   for(i=0; i < n; i++){
	  if(_pMatr[i] == min_level)
		 val = 0.;
	  else
  	     val = _pMatr[i]-minZAct;
      pMatrAct[i] = val; 
	  pMatrLog[i] = (Coef_z_Log*log(val-minZAct+LogDelta)+LogShift)-minZLog;
   }

   if(EnableLogarithmScale){
	   pMatr = pMatrLog;
	   minZ = 0.;
       maxZ = maxZLog - minZLog;
	   view_minZ = minZLog;
       view_maxZ = maxZLog;
   }
   else{
	   pMatr = pMatrAct;
	   minZ = 0.;//minZAct;
       maxZ = maxZAct-minZAct;
	   view_minZ = minZAct;
       view_maxZ = maxZAct;
   }
}

void CMapGraph::OnWriteMetafile()
{
	WriteMetafile(true);
}

void CMapGraph::OnLogarithmScale()
{
	if(EnableLogarithmScale) EnableLogarithmScale = FALSE;
	else  EnableLogarithmScale = TRUE;

   if(EnableLogarithmScale){
	   pMatr = pMatrLog;
	   minZ = minZLog;
       maxZ = maxZLog;		   
   }
   else{
	   pMatr = pMatrAct;
	   minZ = minZAct;
       maxZ = maxZAct;		   
   }
   RefreshPicture();
}

void CMapGraph::SetLimits(int iArea, double xMin, double xMax, double yMin, double yMax)
{
	int i =0;
	C2DGraph::SetLimits(iArea, xMin, xMax, yMin, yMax);
	double dx = (xMax - xMin)/(nx-1);
	double dy = (yMax - yMin)/(ny-1);
	for(i=0; i < nx; i++){
		pX[i] = xMin + i*dx;
	}
	for(i=0; i < ny; i++){
		pY[i] = yMin + i*dy;
	}
}

void CMapGraph::AddDrawLimits(int iArea, CDRect limR, int limType, 
							  double Anfle, int EllType)
{
	C2DGraph::AddDrawLimits(iArea, limR, limType, Anfle, EllType);
}

void CMapGraph::AddDrawLimits(int iArea, CArray<CDPolyLine, CDPolyLine>& PolyLines)
{
	C2DGraph::AddDrawLimits(iArea, PolyLines);
}

void CMapGraph::DrawSmoothingLegend(CDC* pDC)
{
   CRect clientR;
   if(isPrinting)
	  clientR = TotalPrintArea;
   else if(isWMF){
      GetClientRect(clientR);
      ConvertRectToWMFCoord(clientR, pDC);
   }
   else
      GetClientRect(clientR);

  CRect regR;
  regR = GetRegion(0);
  CRect legR(regR.right, regR.top, clientR.right, regR.bottom); 

  int Xc = legR.left + legR.Width()/2;
  int Yc = legR.top + legR.Height()/2;
  int left = Xc - int(legR.Width()*0.05);
  int right = Xc + int(legR.Width()*0.05);
  int top = Yc - int(legR.Height() / 3);
  int bottom = Yc + int(legR.Height() / 3);

  COLORREF Color;
  if(bkColor == RGB(255,255,255))
    Color = RGB(0,0,0);
  else
    Color = RGB(255,255,255);

  CPen pen;
  pen.CreatePen(PS_SOLID, 0, Color);
  CPen* open = pDC->SelectObject(&pen);
  if(open) open->DeleteObject();
  if(isWMF)
    pDC->Rectangle(&CRect(left,-top,right,-bottom));
  else	  
    pDC->Rectangle(&CRect(left,top,right,bottom));
  if(isPrinting || isWMF)
     pDC->SetTextColor(RGB(0,0,0));
  else
     pDC->SetTextColor(Color);
  CString s;
  s.Format("%0.3f", view_maxZ);
  if(isWMF)
    WriteText(pDC, left, -top, LPCTSTR(s), TA_LEFT|TA_BOTTOM);
  else	  
    WriteText(pDC, left, top, LPCTSTR(s), TA_LEFT|TA_BOTTOM);
  s.Format("%0.3f", view_minZ);
  if(isWMF)
    WriteText(pDC, left, -bottom, LPCTSTR(s), TA_LEFT|TA_TOP);
  else	  
    WriteText(pDC, left, bottom, LPCTSTR(s), TA_LEFT|TA_TOP);

  int id=PAL_COLORS-1;
  float f_id;
  f_id = (float)id;
  float s_id = (float)PAL_COLORS/(bottom-top);
  int pCol[3];

  for(int i=top+1; i < bottom-1; i++){
    GetPaletteColor(hPal, id, 0, pCol);
	f_id -= s_id;
	id = (int)f_id;
    CPen pen1;
    pen1.CreatePen(PS_SOLID, 0, RGB(pCol[0], pCol[1], pCol[2]));
    CPen* open1 = pDC->SelectObject(&pen1);
	if(open1)open1->DeleteObject();
	int iP;
	if(isWMF)
	 iP = -i;
	else
	 iP = i;	
	pDC->MoveTo(left+1, iP);
	pDC->LineTo(right-1, iP);
  }
  CPen* retPen = pDC->SelectObject(open);
  if(retPen)
     retPen->DeleteObject();
}

void CMapGraph::DrawSteppedLegend(CDC* pDC)
{
/*   CRect clientR;
   if(isPrinting)
	  clientR = TotalPrintArea;
   else if(isWMF){
      GetClientRect(clientR);
      ConvertRectToWMFCoord(clientR, pDC);
   }
   else
      GetClientRect(clientR);

  CRect regR;
  regR = GetRegion(0);
  CRect legR(regR.right, regR.top, clientR.right, regR.bottom); 

  int Xc = legR.left + legR.Width()/2;
  int Yc = legR.top + legR.Height()/2;
  int left = Xc - int(legR.Width()*0.05);
  int right = Xc + int(legR.Width()*0.05);
  int top = Yc - int(legR.Height() / 3);
  int bottom = Yc + int(legR.Height() / 3);

  COLORREF Color;
  if(bkColor == RGB(255,255,255))
    Color = RGB(0,0,0);
  else
    Color = RGB(255,255,255);

  CPen pen;
  pen.CreatePen(PS_SOLID, 0, Color);
  CPen* open = pDC->SelectObject(&pen);
  if(open) open->DeleteObject();
  if(isWMF)
    pDC->Rectangle(&CRect(left,-top,right,-bottom));
  else	  
    pDC->Rectangle(&CRect(left,top,right,bottom));
  if(isPrinting || isWMF)
     pDC->SetTextColor(RGB(0,0,0));
  else
     pDC->SetTextColor(Color);
  CString s;
  s.Format("%0.2g", view_maxZ);
  if(isWMF)
    WriteText(pDC, left, -top, LPCTSTR(s), TA_LEFT|TA_BOTTOM);
  else	  
    WriteText(pDC, left, top, LPCTSTR(s), TA_LEFT|TA_BOTTOM);
  s.Format("%0.2g", view_minZ);
  if(isWMF)
    WriteText(pDC, left, -bottom, LPCTSTR(s), TA_LEFT|TA_TOP);
  else	  
    WriteText(pDC, left, bottom, LPCTSTR(s), TA_LEFT|TA_TOP);

  int id=PAL_COLORS-1;
  float f_id;
  f_id = (float)id;
  float s_id = (float)PAL_COLORS/(bottom-top);
  int pCol[3];

  for(int i=top+1; i < bottom-1; i++){
    GetPaletteColor(hPal, id, 0, pCol);
	f_id -= s_id;
	id = (int)f_id;
    CPen pen1;
    pen1.CreatePen(PS_SOLID, 0, RGB(pCol[0], pCol[1], pCol[2]));
    CPen* open1 = pDC->SelectObject(&pen1);
	if(open1)open1->DeleteObject();
	int iP;
	if(isWMF)
	 iP = -i;
	else
	 iP = i;	
	pDC->MoveTo(left+1, iP);
	pDC->LineTo(right-1, iP);
  }
  CPen* retPen = pDC->SelectObject(open);
  if(retPen)
     retPen->DeleteObject();*/
}

void CMapGraph::RefreshPicture(BOOL key/*FALSE*/)
{
	bRender = TRUE;
	Invalidate(key);
}


BEGIN_MESSAGE_MAP(CMapGraph, C2DGraph)
	//{{AFX_MSG_MAP(CMapGraph)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDMR_LogarithmScale, OnLogarithmScale)
	ON_COMMAND(IDMR_WriteMetaFile, OnWriteMetafile)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapGraph drawing

BOOL CMapGraph::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return C2DGraph::OnSetCursor(pWnd, nHitTest, message);
}

void CMapGraph::OnDraw(CDC* pDC)
{
    CRect clientR;
    if(!bRender){
        GetClientRect(clientR);
        pDC->FillRect(&clientR, &CBrush(RGB(0,0,0)));
        return;
    }

    if(pDC->IsPrinting())
      isPrinting = TRUE;
    else
      isPrinting = FALSE;

  ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
    CDC dcMem;
    CDC* pDrawDC;
    CPen* oldPen;
    CFont* oldFont;
    CBitmap *pBitmap=NULL;

    CBrush BrushWnd(bkColor);
    CPoint AxisNamePos;
    int orop;
    int obkMode = -1;
    if(isPrinting){
       clientR = TotalPrintArea;
       pDrawDC = pDC;
    }
    else if(isWMF){
       clientR = TotalPrintArea;
       pDrawDC = pDC;
    }
    else{
       GetClientRect(clientR);
       pDrawDC = &dcMem;
    }
    if(pDrawDC != pDC){
      pDrawDC->CreateCompatibleDC(pDC);
      GetClientRect(clientR);
      CBitmap bitmap;
      bitmap.CreateCompatibleBitmap(pDC, clientR.right, clientR.bottom);
      pBitmap = pDrawDC->SelectObject(&bitmap);
    }
    else{
      obkMode = pDrawDC->SetBkMode(TRANSPARENT);
    }

    pDrawDC->FillRect(clientR, &BrushWnd);
    
    InitPlotMode(pDrawDC, 0);
    CFont* pFont=NULL;
    GRID_VIEW grid = Areas[0].Grid;
	
    if(grid.pFont==0){
          pFont = new CFont;
          if(isWMF){
              LOGFONT lf;
              memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
              lf.lfHeight = 370;                      // request a 12-pixel-height font
              strcpy(lf.lfFaceName, "Arial");        // request a face name "Arial"
              pFont->CreateFontIndirect(&lf);  // create the font
          }
          else
            pFont->CreatePointFont(70, "Arial", pDrawDC);
    }
    else
	 pFont = grid.pFont; 

    if(pFont)
     oldFont = pDrawDC->SelectObject(pFont);

      DrawSmoothingMap(pDrawDC);
//      DrawSteppedMap(pDrawDC);

      CPen XYgridPen;
          if(bkColor == grid.YpenColor){
            if(bkColor == RGB(0,0,0)){
             XYgridPen.CreatePen(grid.penType, grid.penWidth, RGB(255,255,255));
             pDrawDC->SetTextColor(RGB(255,255,255));
            }
            else{
             XYgridPen.CreatePen(grid.penType, grid.penWidth, RGB(0,0,0));
             pDrawDC->SetTextColor(RGB(0,0,0));
            } 
          }
          else{
            XYgridPen.CreatePen(grid.penType, grid.penWidth, grid.YpenColor);
            pDrawDC->SetTextColor(grid.YtextColor);
          } 
      oldPen = pDrawDC->SelectObject(&XYgridPen);
      
      double Dx = AxesX(pDrawDC, 0, grid.Ybeg, grid.Ux, grid.nx);
      LPCTSTR xName =  LPCTSTR(Areas[0].Grid.XName);
      if(xName){
        MatToWndCoor(0, Areas[0].Limits.xMax, grid.Ybeg, AxisNamePos.x, AxisNamePos.y);
        WriteText(pDrawDC, AxisNamePos.x, AxisNamePos.y, xName, TA_LEFT|TA_TOP);
      }
      double Dy = AxesY(pDrawDC, 0, grid.Xbeg, grid.Uy, grid.ny, grid.YAlign);
      LPCTSTR yName =  LPCTSTR(Areas[0].Grid.YName);
      if(yName){
          MatToWndCoor(0, grid.Xbeg, Areas[0].Limits.yMax, AxisNamePos.x, AxisNamePos.y);
          WriteText(pDrawDC, AxisNamePos.x, AxisNamePos.y, yName, TA_LEFT|TA_BOTTOM);
      }
      if(grid.isDotGrid){
          orop = pDrawDC->SetROP2(R2_NOT);
          AxesGridX(pDrawDC, 0, Dx);
          AxesGridY(pDrawDC, 0, Dy);
          pDrawDC->SetROP2(orop);
      }

      if(isPrinting || isWMF){
          pDrawDC->SetTextColor(RGB(0,0,0));
          CPen XYgridPen1;
          XYgridPen1.CreatePen(grid.penType, grid.penWidth, RGB(0,0,0));
          if(oldPen)oldPen->DeleteObject();
          oldPen = pDrawDC->SelectObject(&XYgridPen1);
          LPCTSTR yName =  LPCTSTR(Areas[0].Grid.YName);
          Dx = AxesX(pDrawDC, 0, grid.Ybeg, grid.Ux, grid.nx);
          Dy = AxesY(pDrawDC, 0, grid.Xbeg, grid.Uy, grid.ny, grid.YAlign);
      }

    DrawSmoothingLegend(pDrawDC);
      
    CPen* retPen = pDrawDC->SelectObject(oldPen);
    if(retPen) retPen->DeleteObject();
    CFont* retFont = pDrawDC->SelectObject(oldFont);
    if(retFont) retFont->DeleteObject();
    
    if(obkMode != -1)pDrawDC->SetBkMode(obkMode);

    if(pDrawDC != pDC){
      pDC->BitBlt(0, 0, clientR.right, clientR.bottom, pDrawDC, 0, 0, SRCCOPY);
    }
    if(pBitmap){
        CBitmap* retBM = pDrawDC->SelectObject(pBitmap);
        if(retBM) retBM->DeleteObject();
    }
    dcMem.DeleteDC();
    
    if(pFont)
		delete pFont;
  ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void CMapGraph::DrawSmoothingMap(CDC* pDC)
{
//    InitPath(pDC);
	InitPathLimitPolylines(pDC);

	double x1, y1, x2, y2;
	long int x1C, y1C, x2C, y2C;
	for(int j=1; j < ny; j++){
		y1 = pY[j-1];
		y2 = pY[j];
		for(int i=1; i < nx; i++){
			x1 = pX[i-1];
			x2 = pX[i];
			MatToWndCoor(0, x1, y1, x1C, y1C);
			MatToWndCoor(0, x2, y2, x2C, y2C);
			CRect R(x1C, y1C, x2C, y2C);
            CBrush brh(GetColor(i-1, j-1, i, j));
			pDC->FillRect(&R, &brh);
		}
	}

//    DeInitPath(pDC);
//    StrokeDrawPath(pDC);
	DeInitPathLimitPolylines(pDC);
    StrokeLimitPolylines(pDC);
}

COLORREF CMapGraph::GetColor(int ix1, int iy1, int ix2, int iy2)
{
	COLORREF Color;
	double z, z1, z2, z3, z4;
	z1 = z2 = z3 = z4 = 0.;
	z1 = pMatr[iy1*nx+ix1];
	z2 = pMatr[iy1*nx+ix2];
	z3 = pMatr[iy2*nx+ix2];
	z3 = pMatr[iy2*nx+ix1];

	z = __max(z1,z2);
	z = __max(z,z3);
	z = __max(z,z4);

    int id;
    int pCol[3];
    z = (z-minZ)/(maxZ-minZ);
	if(z==F_EXCLUDE_VAL || z==D_EXCLUDE_VAL){
        Color = bkColor;
	}
	else{
       id = int(z*PAL_COLORS);
       if(id<0) id = 0;
       if(id >PAL_COLORS-1) id = PAL_COLORS-1;
       GetPaletteColor(hPal, id, 0, pCol);
       Color = RGB(pCol[0], pCol[1], pCol[2]);
	}
	return Color;
}


void CMapGraph::DrawSteppedMap(CDC* pDC)
{
/*//    InitPath(pDC);
	InitPathLimitPolylines(pDC);

	double x1, y1, x2, y2;
	long int x1C, y1C, x2C, y2C;
	for(int j=1; j < ny; j++){
		y1 = pY[j-1];
		y2 = pY[j];
		for(int i=1; i < nx; i++){
			x1 = pX[i-1];
			x2 = pX[i];
			MatToWndCoor(0, x1, y1, x1C, y1C);
			MatToWndCoor(0, x2, y2, x2C, y2C);
			CRect R(x1C, y1C, x2C, y2C);
            CBrush brh(GetColor(i-1, j-1, i, j));
			pDC->FillRect(&R, &brh);
		}
	}

//    DeInitPath(pDC);
//    StrokeDrawPath(pDC);
	DeInitPathLimitPolylines(pDC);
    StrokeLimitPolylines(pDC);*/
}

void CMapGraph::SetLevelsCount(int _nLevel)
{
	nLevel = _nLevel+1;
}
/////////////////////////////////////////////////////////////////////////////
// CMapGraph diagnostics

#ifdef _DEBUG
void CMapGraph::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMapGraph::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMapGraph message handlers

void CMapGraph::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
}
