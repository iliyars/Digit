// IsolineGraph.cpp : implementation file
//

#include "..\..\stdafx.h"
#include "..\..\Include\Graph\IsolineGraph.h"
#include "..\..\Include\Graph\Def.h"
#include "..\..\Include\Utils\Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CIsolineGraph *gr = NULL;

void Polyline(int pn, ISO_POINT *plist, double level, int ilevel)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
  gr->SingleIsoline(pn, plist, level, ilevel);
}

/////////////////////////////////////////////////////////////////////////////
// CIsolineGraph

IMPLEMENT_DYNCREATE(CIsolineGraph, C2DGraph)

CIsolineGraph::CIsolineGraph(LPCTSTR _iniFile/*NULL*/)
         :C2DGraph(_iniFile)
{
	pMatr = NULL;
	pMatrAct = NULL;
	pMatrLog = NULL;
    nx = ny = 0;
	nLevel = 17;
	pLevelAct = new double[nLevel];
	pLevelLog = new double[nLevel];
	pX = pY = NULL;
	hPal = NULL;
    bRender = FALSE;
	maxNLevels = 20;
    isIsolineNumbers = TRUE;
  isEnableCoor = FALSE;
}

CIsolineGraph::~CIsolineGraph()
{
}

void CIsolineGraph::OnDestroy()
{
	Delete();
    CScrollView::OnDestroy();
}

void CIsolineGraph::Delete()
{
	if(pMatrAct)
	  delete[] pMatrAct;
	if(pMatrLog)
	  delete[] pMatrLog;
	if(pLevelAct)
	  delete[] pLevelAct;
	if(pLevelLog)
	  delete[] pLevelLog;

	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
   if(pX)
	   delete[] pX;
   if(pY)
	   delete[] pY;
}

void CIsolineGraph::OnInitialUpdate()
{
	C2DGraph::OnInitialUpdate();
	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
	CClientDC dc(this);
    hPal = CreateColorScalePalette(dc.m_hDC, COLOR_MESH_SCALE);
}

void CIsolineGraph::CreatePalette()
{
	if(hPal)
	  ::DeleteObject(HGDIOBJ(hPal));
	CClientDC dc(this);
    hPal = CreateColorScalePalette(dc.m_hDC, COLOR_MESH_SCALE);
}

CRuntimeClass* CIsolineGraph::GetRunTime()
{
	return RUNTIME_CLASS(CIsolineGraph);
}

void CIsolineGraph::SetLevelsCount(int _nLevel)
{
	nLevel = _nLevel+1;

	if(pLevelAct)
	  delete[] pLevelAct;	
	pLevelAct = new double[nLevel];

	if(pLevelLog)
	  delete[] pLevelLog;	
	pLevelLog = new double[nLevel];
	SetLevelsRange(minZAct, maxZAct);
}

void CIsolineGraph::RestoreLevelsRange()
{
	SetLevelsRange(InitialminZ, InitialmaxZ);
}

void CIsolineGraph::SetLevelsRange(double minLevel, double maxLevel)
{
   minZAct = minLevel;
   maxZAct = maxLevel;
   minZLog = log(minZAct-minZAct+LogDelta);
   maxZLog = log(maxZAct-minZAct+LogDelta);
   minZLog = Coef_z_Log*minZLog+LogShift;
   maxZLog = Coef_z_Log*maxZLog+LogShift;

   double dzAct = (maxZAct - minZAct)/(nLevel-2);
   double dzLog = (maxZLog - minZLog)/(nLevel-2);
   for(int i=0; i < nLevel; i++){
	   pLevelAct[i] = minZAct + i*dzAct;
	   pLevelLog[i] = minZLog + i*dzLog;
   }

   if(EnableLogarithmScale){
	   pMatr = pMatrLog;
	   pLevel = pLevelLog;
	   minZ = minZLog;
       maxZ = maxZLog;		   
   }
   else{
	   pMatr = pMatrAct;
	   pLevel = pLevelAct;
	   minZ = minZAct;
       maxZ = maxZAct;		   
   }
}

void CIsolineGraph::SetLimits(int iArea, double xMin, double xMax, double yMin, double yMax)
{
	int i =0;
	C2DGraph::SetLimits(iArea, xMin, xMax, yMin, yMax);
	double dx = (xMax - xMin)/(nx-1);
	double dy = (yMax - yMin)/(ny-1);
	for( i=0; i < nx; i++){
		pX[i] = xMin + i*dx;
	}
	for(i=0; i < ny; i++){
		pY[i] = yMin + i*dy;
	}
}

void CIsolineGraph::AddDrawLimits(int iArea, CDRect limR, int limType,double Anfle, int EllType)
{
	C2DGraph::AddDrawLimits(iArea, limR, limType, Anfle, EllType);
}

void CIsolineGraph::AddDrawLimits(int iArea, CArray<CDPolyLine, CDPolyLine>& PolyLines)
{
	C2DGraph::AddDrawLimits(iArea, PolyLines);
}

void CIsolineGraph::Set3DMatrix(float* _pMatr, int _nx, int _ny)
{
   minZAct = 1.e10;
   maxZAct = -1.e10;
   minZLog = 1.e10;
   maxZLog = -1.e10;
	int i =0;   
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

   for(i=0; i < n; i++){
	  val = _pMatr[i];
	  if(val == min_level)
         pMatrAct[i] = minZAct-min_level*2;
	  else
       pMatrAct[i] = val;
   }
   
   Coef_z_Log= (maxZAct-minZAct)/log((maxZAct-minZAct+LogDelta)/LogDelta);
   LogShift= minZAct-Coef_z_Log*log(LogDelta);

   for(i=0; i < n; i++){
	  val = pMatrAct[i]; 
	  pMatrLog[i] = Coef_z_Log*log(val-minZAct+LogDelta)+LogShift;
   }
   InitialminZ = minZAct; 
   InitialmaxZ = maxZAct;	   
   SetLevelsRange(minZAct, maxZAct);   
}

void CIsolineGraph::OnLogarithmScale()
{
	if(EnableLogarithmScale) EnableLogarithmScale = FALSE;
	else  EnableLogarithmScale = TRUE;

   if(EnableLogarithmScale){
	   pMatr = pMatrLog;
	   pLevel = pLevelLog;
	   minZ = minZLog;
       maxZ = maxZLog;		   
   }
   else{
	   pMatr = pMatrAct;
	   pLevel = pLevelAct;
	   minZ = minZAct;
       maxZ = maxZAct;		   
   }
   RefreshPicture();
}

void CIsolineGraph::OnIsolineNumbers()
{
	if(EnableIsolineNumbers) EnableIsolineNumbers = FALSE;
	else  EnableIsolineNumbers = TRUE;
    RefreshPicture();
}

void CIsolineGraph::DrawIsolines(CDC* pDC)
{

//  InitPath(pDC);
	InitPathLimitPolylines(pDC);

  gr = this;
  hDC = pDC->GetSafeHdc();
  if (nLevel > 0)
    pLevel[nLevel - 1] *= 0.999999999;

  Contour(pMatr, ny, nx, pY, pX, nLevel, pLevel, Polyline);

//  DeInitPath(pDC);
//  StrokeDrawPath(pDC);
	DeInitPathLimitPolylines(pDC);
    StrokeLimitPolylines(pDC);
}


void CIsolineGraph::SingleIsoline(int pn, ISO_POINT *plist, double level, int ilevel)
{
  if (pn < 2)
    return;

  CDC* pDC = CDC::FromHandle(hDC);
  int n = pn;
  ISO_POINT *list = plist;
  long int xC;
  long int yC;
  double z = level;

    int id;
    int ICol[3];

    z = (z-minZ)/(maxZ-minZ);
    id = int(z*PAL_COLORS);
    if ( id < 0 )
      id = 1;
    if ( id >= PAL_COLORS)
      id = PAL_COLORS - 1;
    GetPaletteColor(hPal, id, 0, ICol);

    CPen pen;
	pen.CreatePen(PS_SOLID, 0, RGB(ICol[0],ICol[1],ICol[2]));
	CPen* open = pDC->SelectObject(&pen);

    MatToWndCoor(0, list->x, list->y, xC, yC);
    pDC->MoveTo(xC, yC);
    list++;

    while (--n){
      MatToWndCoor(0, list->x, list->y, xC, yC);
      pDC->LineTo(xC, yC);
      list++;
    }

    // Add text label to contour line.
    //  if (pn > MINCELLS)
      {
      srand((unsigned)time(NULL));

      static char s[80];
      sprintf (s, "%d", ilevel + 1);

      int mincell = MINCELLS; //(int)((double)pn / 4.);
      int index = (int)(((double)rand() / (double)RAND_MAX) * (double)mincell);

	  if(EnableIsolineNumbers){
		  for (int i = index; i < pn; i += mincell){
			  MatToWndCoor(0, plist[i].x, plist[i].y, xC, yC);
			  CPoint numP(xC,yC);
			  pDC->SetTextColor(RGB(ICol[0],ICol[1],ICol[2]));
			  WriteText(pDC, numP.x+2, numP.y+2, s, TA_CENTER|TA_BOTTOM);
		  }
	  } 
     }
   CPen* retPen = pDC->SelectObject(open);
   if(retPen) retPen->DeleteObject();
}

void CIsolineGraph::DrawLegend(CDC* pDC)
{
  int stepDown = 18; // !!!!
  int id;
  int ICol[3];
  char buf[26];
  int y=0;
  
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
  CRect legR(regR.right, regR.top, (int)(regR.right+regR.Width()*0.3), regR.bottom); 

  int left = int(regR.right+legR.Width() * 0.15); 
  int right = int(left + legR.Width()*0.25); 
  stepDown = int(legR.Height() / maxNLevels);
  double minLevel;
  double maxLevel;
  if(EnableLogarithmScale){
	  minLevel = pLevelLog[0];
	  maxLevel = pLevelLog[nLevel-1];
  }
  else{
	  minLevel = pLevel[0];
	  maxLevel = pLevel[nLevel-1];
  }
  y = regR.top - stepDown;

  CPen pen0;
  pen0.CreatePen(PS_SOLID, 0, RGB(0,0,0));
  CPen* open = pDC->SelectObject(&pen0);

  int idy = nLevel;
  for(int i=nLevel-2; i > -1; i--){
      double z;
      if(EnableLogarithmScale)
	    z= pLevelLog[i];
	  else
	    z= pLevel[i];
        if (z < minLevel || z > maxLevel)
          continue;   
      y += stepDown;
      z = (z-minLevel) / (maxLevel-minLevel);
      id = int(z*PAL_COLORS);
      if ( id < 0 ) id = 1;
      if ( id >= PAL_COLORS) id = PAL_COLORS - 1;
      GetPaletteColor(hPal, id, 0, ICol);
      
      CPoint LP(left, y); 
      CPoint RP(right, y);
      CPoint textP(int(left + legR.Width()*0.25), y);
      CPoint numP(left+(right - left)/2, y);
//      textP = Translate(textP.x, textP.y);
//      numP = Translate(numP.x, numP.y);
      CPen pen;
  	  pen.CreatePen(PS_SOLID, 0, RGB(ICol[0],ICol[1],ICol[2]));
	  CPen* pRetPen = pDC->SelectObject(&pen);
	  if(pRetPen) pRetPen->DeleteObject();

	  if(isWMF){
		  LP.y = -LP.y;
		  RP.y = -RP.y;
	  }
      pDC->MoveTo(LP.x, LP.y);
      pDC->LineTo(RP.x, RP.y);
      sprintf(buf, "%0.3f", pLevel[i]);
	  if(bkColor == RGB(255,255,255))
        pDC->SetTextColor(RGB(0,0,0));
	  else
        pDC->SetTextColor(RGB(255,255,255));
	  if(isPrinting || isWMF)
        pDC->SetTextColor(RGB(0,0,0));
	  if(isWMF)
		  textP.y = -textP.y;
      WriteText(pDC, textP.x, textP.y, buf, TA_LEFT|TA_BASELINE);
	  if(EnableIsolineNumbers){
        pDC->SetTextColor(RGB(ICol[0],ICol[1],ICol[2]));
        sprintf(buf, "%d", --idy);
	  if(isWMF)
		  numP.y = -numP.y;
        WriteText(pDC, numP.x, numP.y, buf, TA_CENTER|TA_BOTTOM);
	  }
    }
  CPen* pRetPen = pDC->SelectObject(open);
  if(pRetPen) pRetPen->DeleteObject();
}

void CIsolineGraph::RefreshPicture(BOOL key/*FALSE*/)
{
    bRender = TRUE;
	Invalidate(key);
}

BEGIN_MESSAGE_MAP(CIsolineGraph, C2DGraph)
	//{{AFX_MSG_MAP(CIsolineGraph)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_COMMAND(IDMR_LogarithmScale, OnLogarithmScale)
	ON_COMMAND(IDMR_IsolineNumbers, OnIsolineNumbers)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIsolineGraph drawing

BOOL CIsolineGraph::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    return C2DGraph::OnSetCursor(pWnd, nHitTest, message);
}

void CIsolineGraph::OnDraw(CDC* pDC)
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
    CDC* pDrawDC=NULL;
    CPen* oldPen;
    CFont* oldFont;
    CBitmap *pBitmap=NULL;

    CBrush BrushWnd(bkColor);
    CPoint AxisNamePos;
    int orop;
    
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
    
    pDrawDC->FillRect(clientR, &BrushWnd);
  int obkMode;
  CFont* pFont=NULL;     
  if(Areas.GetSize()){
    InitPlotMode(pDrawDC, 0);
    obkMode = pDrawDC->SetBkMode(TRANSPARENT);
    
      GRID_VIEW grid = Areas[0].Grid;
      if(grid.pFont==0){
          pFont = new CFont;
          if(isWMF){
              LOGFONT lf;
              memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
              lf.lfHeight = 400;                      // request a 12-pixel-height font
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

      DrawIsolines(pDrawDC);

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
      DrawLegend(pDrawDC);
  }   
    pDrawDC->SetBkMode(obkMode);
    CPen* retPen = pDrawDC->SelectObject(oldPen);
    if(retPen) retPen->DeleteObject();
    CFont* retFont = pDrawDC->SelectObject(oldFont);
    if(retFont) retFont->DeleteObject();

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

/////////////////////////////////////////////////////////////////////////////
// CIsolineGraph diagnostics

#ifdef _DEBUG
void CIsolineGraph::AssertValid() const
{
	CScrollView::AssertValid();
}

void CIsolineGraph::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CIsolineGraph message handlers

void CIsolineGraph::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);

}
