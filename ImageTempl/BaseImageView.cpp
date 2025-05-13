// BaseImageView.cpp : implementation file
//
/***********************************************************************************
 ������: BaseImageView.cpp
 ����������:
         ������� ������������� ����������� ��������-�������������      
************************************************************************************/

#include "..\stdafx.h"
#include "..\AppDef.h"
#include "BaseImageDoc.h"
#include "BaseImageView.h"
#include "..\Utils\mutils.h"

#include "D:\Numbering\MGTools\Include\Graph\GraphTools.h"
#include "D:\Numbering\MGTools\Include\Utils\Utils.h"
#include "D:\Numbering\MGTools\Include\Utils\DibApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBaseImageView

IMPLEMENT_DYNCREATE(CBaseImageView, SECZoomView)

CBaseImageView::CBaseImageView()
{
    SetScrollSizes(MM_TEXT, CSize(0, 0));
    //Turn on zooming, default is zooming off.
    SetZoomMode(SEC_ZOOMNORMAL); //SECZoomView
    m_cvwMode = NORMAL;
    m_bCaptured = FALSE;
    m_hZoomInPointCursor  = AfxGetApp()->LoadCursor(IDC_ZOOMIN);
    m_hZoomOutPointCursor = AfxGetApp()->LoadCursor(IDC_ZOOMOUT);
    m_hZoomRectCursor     = AfxGetApp()->LoadCursor(IDC_ZOOMRECT);    
    m_hZoomRectDragCursor = AfxGetApp()->LoadCursor(IDC_ZOOMRECTDRAG);    
}

CBaseImageView::~CBaseImageView()
{
    //Free cursors
    if (m_hZoomInPointCursor)  DestroyCursor(m_hZoomInPointCursor);
    if (m_hZoomOutPointCursor) DestroyCursor(m_hZoomOutPointCursor);
    if (m_hZoomRectCursor)     DestroyCursor(m_hZoomRectCursor);
    if (m_hZoomRectDragCursor) DestroyCursor(m_hZoomRectDragCursor);
}

//����������� ���������� ����� (point) �� ��������� ���������� � 
//���������� ���������� 
void CBaseImageView::ClientToDoc(CPoint& point)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.DPtoLP(&point);
}

void CBaseImageView::ClientToDoc(CSize& size)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.DPtoLP(&size);
}

//����������� ���������� ������������ (rect) �� ��������� ���������� � 
//���������� ���������� 
void CBaseImageView::ClientToDoc(CRect& rect)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.DPtoLP(rect);
//  ASSERT(rect.left <= rect.right);
//  ASSERT(rect.bottom <= rect.top);
}

//����������� ���������� ����� (point) �� ���������� ��������� �  
// ���������� ����������
void CBaseImageView::DocToClient(CPoint& point)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.LPtoDP(&point);
}

void CBaseImageView::DocToClient(CSize& size)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.LPtoDP(&size);
}

//����������� ���������� ������������ (rect) �� ���������� ��������� �  
// ���������� ����������
void CBaseImageView::DocToClient(CRect& rect)
{
    CClientDC dc(this);
    OnPrepareDC(&dc, NULL);
    dc.LPtoDP(rect);
    rect.NormalizeRect();
}

void CBaseImageView::GetZoomCoefficent(double& kZ)
{
    CRect clientR; GetClientRect(clientR);
    CImageCtrls* pImCtrls = GetImageCtrls(this);
    CRect rcDIB(pImCtrls->GetDIBRect());
	double kZoom = GetZoomLevel();
	kZ = kZoom;
}

CRect CBaseImageView::GetImageRegion(bool absReg/*false*/)
{
    CImageCtrls* pImCtrls = GetImageCtrls(this);
    CRect rcDIB(pImCtrls->GetDIBRect());
    if(rcDIB == CRect(0,0,0,0))
        return rcDIB;

    CRect clientR; GetClientRect(clientR);
  
    CRect rcDest;
    int scrW, scrH;
    scrW = GetScrollPos(SB_HORZ);
    scrH = GetScrollPos(SB_VERT);
    double kZ;  
    GetZoomCoefficent(kZ);
    rcDIB.right = rcDIB.left + (int)(rcDIB.Width()*kZ);
    rcDIB.bottom = rcDIB.top + (int)(rcDIB.Height()*kZ);
    if((rcDIB.Width() <= clientR.Width() && rcDIB.Height() <= clientR.Height())){
      rcDest.left = (clientR.Width() - rcDIB.Width())/2;
      rcDest.right = rcDest.left + rcDIB.Width();
      rcDest.top = (clientR.Height() - rcDIB.Height())/2;
      rcDest.bottom = rcDest.top + rcDIB.Height();
    }
    else if(rcDIB.Width() >= clientR.Width() && rcDIB.Height() >= clientR.Height()){
      rcDest.left = clientR.left;
      rcDest.top = clientR.top;
      rcDest.right = rcDest.left + rcDIB.Width();
      rcDest.bottom = rcDest.top + rcDIB.Height();
    }
    else if(rcDIB.Width() <= clientR.Width() && rcDIB.Height() >= clientR.Height()){
      rcDest.left = (clientR.Width() - rcDIB.Width())/2;
      rcDest.right = rcDest.left + rcDIB.Width();
      rcDest.top = clientR.top;
      rcDest.bottom = rcDest.top+rcDIB.Height();
    }
    else if(rcDIB.Width() >= clientR.Width() && rcDIB.Height() <= clientR.Height()){
      rcDest.left = clientR.left;
      rcDest.right = rcDest.left+rcDIB.Width();
      rcDest.top = (clientR.Height() - rcDIB.Height())/2;
      rcDest.bottom = rcDest.top + rcDIB.Height();
    }
    if(absReg){
        rcDest.left -= scrW;
        rcDest.right -= scrW;
        rcDest.top -= scrH;
        rcDest.bottom -= scrH;
    }
    return rcDest;
}

// ��������� ����������� � ��������� ���������� CDC
void CBaseImageView::DrawImage(CDC* pDC)
{
    CImageCtrls* pImCtrls = GetImageCtrls(this);
    SECDib* pImage = (SECDib*)pImCtrls->GetImage();
    CControls* pCtrls = GetControls();
       
    if(pImage && (pCtrls->ViewState & V_INTERFEROGRAM)){
        CRect clientR; 
		GetClientRect(clientR);
	    ClientToDoc(clientR);
        CPalette* pOldPalette = NULL;
        CRect rcDIB(pImCtrls->GetDIBRect());
        CRect rcDest(rcDIB);
        CRect rcSourse(rcDIB);
           if(pImage && pImage->m_pPalette)
            pOldPalette = pDC->SelectPalette(pImage->m_pPalette, TRUE);
		    pImage->m_bUseHalftone = TRUE;
            pImage->StretchDIBits(pDC,
                rcDest.left, rcDest.top, rcDest.Width(), rcDest.Height(),
                rcSourse.left, rcSourse.top, rcSourse.Width(), rcSourse.Height(),
                pImage->m_lpSrcBits,
                pImage->m_lpBMI, DIB_RGB_COLORS,
                SRCCOPY);
            
            if(pOldPalette)
                pDC->SelectPalette(pOldPalette, TRUE);
     }
	else{
		CRect clRect; GetClientRect(clRect);
		pDC->FillRect(&clRect, &CBrush(RGB(0,0,0)));
	}
}

void CBaseImageView::DrawBackGround(CDC* pDC)
{
    CImageCtrls* pImCtrls = GetImageCtrls(this);
    SECDib* pImage = (SECDib*)pImCtrls->GetImage();

	CRect clientRect;
	GetClientRect(clientRect);
    ClientToDoc(clientRect);
    if(pImage){
        CRect rcDIB(pImCtrls->GetDIBRect());
        pDC->ExcludeClipRect(&rcDIB);
	}
	else{
	}
	pDC->FillRect(&clientRect, &CBrush(RGB(0,0,0)));
	pDC->SelectClipRgn(NULL);
}
// ��������� ������ � ��������� ���������� CDC
void CBaseImageView::DrawBounds(CDC* pDC)
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    COLORREF Color = RGB(0,255,0);
    CPen pen;
    pen.CreatePen(PS_COSMETIC, 0, Color);
    CPen* open = pDC->SelectObject(&pen);

	int xDIB, yDIB;
	if(pDoc->imageCtrls.m_pDIB == 0){
       xDIB = pDoc->imageCtrls.ImageSize.cx;
       yDIB = pDoc->imageCtrls.ImageSize.cy;
	}
	else{
       xDIB = pDoc->imageCtrls.m_pDIB->m_dwPadWidth;
       yDIB = pDoc->imageCtrls.m_pDIB->m_dwHeight;
	}
	CRect Bound;
	CArray<CPoint, CPoint> PlgPoints;
	BOOL res;
    int BoundType = pDoc->boundCtrls.ExtBoundType;
	if(BoundType != -1){
		res = pDoc->boundCtrls.GetExtRealBound(BoundType, xDIB, yDIB, Bound, PlgPoints);
		if(res){
			if(BoundType == BOUND_ROUND || BoundType == BOUND_ELLIPSE){
				pDC->Arc(Bound, CPoint(Bound.right, Bound.CenterPoint().y), CPoint(Bound.CenterPoint().x, Bound.right));
				pDC->Arc(Bound, CPoint(Bound.CenterPoint().x, Bound.right), CPoint(Bound.right, Bound.CenterPoint().y));
			}
			else if(BoundType == BOUND_RECT){
				pDC->MoveTo(Bound.left, Bound.top);
				pDC->LineTo(Bound.right, Bound.top);
				pDC->LineTo(Bound.right, Bound.bottom);
				pDC->LineTo(Bound.left, Bound.bottom);
				pDC->LineTo(Bound.left, Bound.top);
			}
			else{
				for(int i=0; i < PlgPoints.GetSize(); i++){
					if(i==0)pDC->MoveTo(PlgPoints[i]);
					else pDC->LineTo(PlgPoints[i]);
				}
			}
		}
	}

    BoundType = pDoc->boundCtrls.InsBoundType;
	if(BoundType != -1){
		int idx = 1;
		res = pDoc->boundCtrls.GetInsRealBound(BoundType, xDIB, yDIB, idx, Bound, PlgPoints);
		if(res){
			if(BoundType == BOUND_ROUND || BoundType == BOUND_ELLIPSE){
				pDC->Arc(Bound, CPoint(Bound.right, Bound.CenterPoint().y), CPoint(Bound.CenterPoint().x, Bound.right));
				pDC->Arc(Bound, CPoint(Bound.CenterPoint().x, Bound.right), CPoint(Bound.right, Bound.CenterPoint().y));
			}
			else if(BoundType == BOUND_RECT){
				pDC->MoveTo(Bound.left, Bound.top);
				pDC->LineTo(Bound.right, Bound.top);
				pDC->LineTo(Bound.right, Bound.bottom);
				pDC->LineTo(Bound.left, Bound.bottom);
				pDC->LineTo(Bound.left, Bound.top);
			}
			else{
				while(res){
					for(int i=0; i < PlgPoints.GetSize(); i++){
						if(i==0)pDC->MoveTo(PlgPoints[i]);
						else pDC->LineTo(PlgPoints[i]);
					}
					idx++;
					res = pDoc->boundCtrls.GetInsRealBound(BoundType, xDIB, yDIB, idx, Bound, PlgPoints);
				}
			}
		}
	}

    if(open){
      CPen* pRetPen = pDC->SelectObject(open);
      if(pRetPen) pRetPen->DeleteObject();
    }
}

// ��������� �������  � �������� �������������� � ��������� ���������� CDC
void CBaseImageView::DrawCurBound(CDC* pDC)
{
    COLORREF Color = RGB(0,255,255);
    CPen pen;
    pen.CreatePen(PS_COSMETIC, 0 , Color);
    CPen* open = pDC->SelectObject(&pen);

    CControls* pCtrls = GetControls();
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    
	if(pDoc->boundCtrls.IsCurArea()){
        CRect dR(pDoc->boundCtrls.CurBound);
        CRect wR;
		wR.left = dR.left;
		wR.right = dR.right;
		wR.top = dR.top;
		wR.bottom = dR.bottom;
        wR.NormalizeRect();
        if(pCtrls->CurTypeBound == BOUND_ROUND || pCtrls->CurTypeBound == BOUND_ELLIPSE){
          pDC->Arc(wR, CPoint(wR.right, wR.CenterPoint().y), CPoint(wR.CenterPoint().x, wR.right));
          pDC->Arc(wR, CPoint(wR.CenterPoint().x, wR.right), CPoint(wR.right, wR.CenterPoint().y));
        }
        else if(pCtrls->CurTypeBound == BOUND_RECT){
			pDC->MoveTo(wR.left, wR.top);
			pDC->LineTo(wR.right, wR.top);
			pDC->LineTo(wR.right, wR.bottom);
			pDC->LineTo(wR.left, wR.bottom);
			pDC->LineTo(wR.left, wR.top);
        }
        else if(pCtrls->CurTypeBound == BOUND_POLYGON){
			for(int i=0; i < pDoc->boundCtrls.CurPlg.GetSize(); i++){
				if(i==0) pDC->MoveTo(pDoc->boundCtrls.CurPlg[i]);
				else pDC->LineTo(pDoc->boundCtrls.CurPlg[i]);
			}
		}
	}
    if(open){
      CPen* pRetPen = pDC->SelectObject(open);
      if(pRetPen) pRetPen->DeleteObject();
    }
}
// ��������� ������� ��������� ������
// cP - ����� �������
// zoomMark - ����� ���������� ����������� �������
// Color - ���� �������
void CBaseImageView::DrawMarker(CDC* pDC, CPoint cP, double zoomMark, COLORREF Color)
{
    CPen* open=NULL;
    if(Color){
        CPen pen;
        pen.CreatePen(PS_COSMETIC, 0, Color);
        open = pDC->SelectObject(&pen);
    }

	CControls* pCtrls = GetControls();
    int step = (int)(pCtrls->MarkerSide*zoomMark/2);
    int ts = step;
	CSize s_ts(ts, ts);
    pDC->DPtoLP(&s_ts);
	step = s_ts.cx;
    CRect rP(cP.x-step, cP.y-step, cP.x+step, cP.y+step);
    pDC->MoveTo(rP.left, rP.top);
    pDC->LineTo(rP.right, rP.bottom);
    pDC->MoveTo(rP.right, rP.top);
    pDC->LineTo(rP.left, rP.bottom);

    if(open){
      CPen* pRetPen = pDC->SelectObject(open);
      if(pRetPen) pRetPen->DeleteObject();
    }
}
// ��������� �����, ��� ������� ������ ������� � ��������� CDC
void CBaseImageView::DrawCustomDots(CDC* pDC)
{
	CBaseImageDoc* pDoc = (CBaseImageDoc*) GetDocument();
    COLORREF Color = InvColor;
    CBrush br(Color);
    CPen pen;
    pen.CreatePen(PS_COSMETIC, 0, Color);
    CPen* open = pDC->SelectObject(&pen);
    int orop = pDC->SetROP2(R2_XORPEN);
      
    for(int i=0; i < pDoc->boundCtrls.CustomDots.GetSize(); i++){
       CPoint Mr(pDoc->boundCtrls.CustomDots[i]);
       DrawMarker(pDC, Mr);
    }
	
    pDC->SetROP2(orop);
    CPen* retopen = pDC->SelectObject(open);
    if(retopen) retopen->DeleteObject();
}
// ��������� ����� �� ����� ������� ������
//point - ���������� ���������� �����
void CBaseImageView::SetCustomDot(CPoint point)
{
	CControls* pCtrls = GetControls();
	CBaseImageDoc* pDoc = (CBaseImageDoc*) GetDocument();
    if(pCtrls->EnableCustomDots){
        pDoc->boundCtrls.CustomDot = point;
        if(pDoc->boundCtrls.RemoveCustomDot(pDoc->boundCtrls.CustomDot)){
            SetCapture();
        }
        else{
            CImageCtrls* pImCtrls = GetImageCtrls(this);
            CBoundCtrls* pBCtrls = GetBoundCtrls(this);
            pDoc->boundCtrls.AddCustomDot(pDoc->boundCtrls.CustomDot);
			if(pDoc->boundCtrls.CustomDots.GetSize()==1)
              pDoc->boundCtrls.SetCurBound(pCtrls->CurTypeBound);
			else
              pDoc->boundCtrls.SetCurBound(pCtrls->CurTypeBound);
            Invalidate(FALSE);
        }
    }
}
// �������������� ����� ������� ������
// point - ����� ��������� �����
void CBaseImageView::DragCustomDot(CPoint point, bool ReDraw/*true*/)
{
   if(GetCapture() != this)
      return;

  CBaseImageDoc* pDoc = (CBaseImageDoc*) GetDocument();
  CControls* pCtrls = GetControls();
  if(pCtrls->EnableCustomDots){
      CPen penLine;
      COLORREF Color = InvColor;
      penLine.CreatePen(PS_COSMETIC, 0, Color);

      CDC* dc = GetDC();
      OnPrepareDC(dc);
      CPen *pOld = dc->SelectObject(&penLine);
      int orop = dc->SetROP2(R2_XORPEN);
      DrawMarker(dc, pDoc->boundCtrls.CustomDot);
      pDoc->boundCtrls.CustomDot = point;
      DrawMarker(dc, pDoc->boundCtrls.CustomDot);

      dc->SetROP2(orop);
      CPen* pRetPen = dc->SelectObject(pOld);
      if(pRetPen) pRetPen->DeleteObject();
      ReleaseDC(dc);
  }
}

void CBaseImageView::DropCustomDot(CPoint point)
{
    CBoundCtrls* pBCtrls =  GetBoundCtrls(this);
	CControls* pCtrls = GetControls();
    ReleaseCapture();
    if(pCtrls->EnableCustomDots){
        pBCtrls->CustomDot = point;
        pBCtrls->AddCustomDot(pBCtrls->CustomDot);
        pBCtrls->SetCurBound(pCtrls->CurTypeBound);
        Invalidate(FALSE);
    }
}

// returns logical coords of center of handle
CPoint CBaseImageView::GetHandle(int nHandle)
{
    ASSERT_VALID(this);
    int x, y, xCenter, yCenter;

    // this gets the center regardless of left/right and top/bottom ordering
    CBoundCtrls* pBCtrls =  GetBoundCtrls(this);
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    CRect m_rect;
	pDoc->Tracker.GetBoundRect(m_rect);
    xCenter = m_rect.left + m_rect.Width() / 2;
    yCenter = m_rect.top + m_rect.Height() / 2;

    switch (nHandle)
    {
    default:
        ASSERT(FALSE);

    case 1:
        x = m_rect.left;
        y = m_rect.top;
        break;

    case 2:
        x = xCenter;
        y = m_rect.top;
        break;

    case 3:
        x = m_rect.right;
        y = m_rect.top;
        break;

    case 4:
        x = m_rect.right;
        y = yCenter;
        break;

    case 5:
        x = m_rect.right;
        y = m_rect.bottom;
        break;

    case 6:
        x = xCenter;
        y = m_rect.bottom;
        break;

    case 7:
        x = m_rect.left;
        y = m_rect.bottom;
        break;

    case 8:
        x = m_rect.left;
        y = yCenter;
        break;
    }

    return CPoint(x, y);
}

BEGIN_MESSAGE_MAP(CBaseImageView, SECZoomView)
	//{{AFX_MSG_MAP(CBaseImageView)
    ON_COMMAND(IDD_CLONE, OnCloneDoc)
    ON_COMMAND(IDD_COPY, OnCopy)
    ON_COMMAND(IDD_CLONE, OnCloneDoc)
    ON_COMMAND(IDD_COPY_SCENARIO, OnCopyScn)
    ON_COMMAND(IDD_PAST_SCENARIO, OnPasteScn)
    ON_COMMAND(IDD_ZOOM_IN, OnZoomIn)
    ON_COMMAND(IDD_ZOOM_OUT, OnZoomOut)
    ON_COMMAND(IDD_ZOOM_FIT, OnZoomFit)
    ON_UPDATE_COMMAND_UI(IDD_COPY_SCENARIO, OnUpdateCopyScn)
    ON_UPDATE_COMMAND_UI(IDD_PASTE_SCENARIO, OnUpdatePasteScn)
    ON_COMMAND(IDD_OPEN_SCENARIO, OnLoadScn)
    ON_COMMAND(IDD_SAVE_SCENARIO, OnSaveScn)
    ON_UPDATE_COMMAND_UI(IDD_OPEN_SCENARIO, OnUpdateLoadScn)
    ON_UPDATE_COMMAND_UI(IDD_SAVE_SCENARIO, OnUpdateSaveScn)
    ON_COMMAND(IDD_BOUND_APPLY, OnApplyBound)
    ON_COMMAND(IDD_BOUND_CURREMOVE, OnRemoveCurBound)
    ON_COMMAND(IDD_BOUND_LASTREMOVE, OnRemoveLastBound)
    ON_COMMAND(IDD_BOUND_ALLREMOVE, OnRemoveAllBound)
    ON_COMMAND(IDD_BOUND_SETDOTS, OnSetupDotsBound)
    ON_COMMAND(IDD_BOUND_SETRECT, OnSetupRectBound)
    ON_COMMAND(IDD_BOUND_SCRROUND, OnScrRoundBound)
    ON_COMMAND(IDD_BOUND_SCRELLIPSE, OnScrEllipseBound)
    ON_COMMAND(IDD_BOUND_SCRRECT, OnScrRectBound)
    ON_COMMAND(IDD_BOUND_SCRPLG, OnScrPlgBound)
    ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_SETFOCUS()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaseImageView drawing

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnInitialUpdate()
{
    SetZoomMinMax(0.02f,22.0f);
    CImageCtrls* pImage = GetImageCtrls(this);
    CRect wDIBRect = pImage->GetDIBRect();
	CSize sizeDoc(wDIBRect.Width()+2, wDIBRect.Height()+2);
    SetScrollSizes(MM_TEXT, sizeDoc);    
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CBaseImageView diagnostics

// ������ Microsoft Visual C++ ������������
#ifdef _DEBUG
void CBaseImageView::AssertValid() const
{
	SECZoomView::AssertValid();
}

void CBaseImageView::Dump(CDumpContext& dc) const
{
	SECZoomView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CBaseImageView message handlers
// ������ Microsoft Visual C++ ������������
BOOL CBaseImageView::OnEraseBkgnd(CDC* pDC) 
{
    return TRUE;
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SECZoomView::OnHScroll(nSBCode, nPos, pScrollBar);
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	SECZoomView::OnVScroll(nSBCode, nPos, pScrollBar);
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	SECZoomView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnSetFocus(CWnd* pOldWnd) 
{
	SECZoomView::OnSetFocus(pOldWnd);
	// TODO: Add your message handler code here
}

// �������� ����� �������� �� �����
void CBaseImageView::OnLoadScn()
{
    LPCTSTR title = CRS("������� ���� ��������", "Open scenario file");
    CFileDialog fileDlg(TRUE);
    CString fIndex;

    TCHAR strName[_MAX_PATH];
    strName[0] = (TCHAR)NULL;
    fileDlg.m_ofn.lpstrFile = strName;
    fileDlg.m_ofn.lpstrTitle = title;

    LPCTSTR lan;
	CString str;
    str.Empty();
    lan = CRS("����� (*.scn)","Files (*.scn)");
    str += lan; str += (TCHAR)NULL;
    str += "*.scn"; str += (TCHAR)NULL;
    fileDlg.m_ofn.lpstrFilter = LPCTSTR(str);
    fileDlg.m_ofn.nFilterIndex = 1;

    fileDlg.m_ofn.Flags |= OFN_FILEMUSTEXIST;

    TCHAR Str[_MAX_PATH];
    if(ReadPath(GetWorkPath(), Str, GetIniFile()))
      fileDlg.m_ofn.lpstrInitialDir = Str;
    else
      fileDlg.m_ofn.lpstrInitialDir = NULL;
    if(fileDlg.DoModal() == IDOK){
      CString fName = fileDlg.GetFileName();
//	  LoadScenario(LPCTSTR(fName));
	}
}

//���������� ����� ��������
void CBaseImageView::OnSaveScn()
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    LPCTSTR title = CRS("��������� ���� ��������", "Save scenario file");
    CFileDialog fileDlg(FALSE);
    LPCTSTR lan;
	CString str;
    lan = CRS("����� (SCN)","Files (SCN)");
    str += lan; str += (TCHAR)NULL;
    fileDlg.m_ofn.lpstrFilter = LPCTSTR(str);
    fileDlg.m_ofn.nFilterIndex = 1;
    TCHAR strName[_MAX_PATH];
    strName[0] = (TCHAR)NULL;
	int iP = pDoc->TmpPath.ReverseFind('\\');
	CString s;
	s = pDoc->TmpPath.Mid(iP+1);
	s = s.Left(s.GetLength()-4);
    strcpy(strName, LPCTSTR(s));
    fileDlg.m_ofn.lpstrFile = strName;
    fileDlg.m_ofn.lpstrTitle = title;

    fileDlg.m_ofn.Flags |= (OFN_EXTENSIONDIFFERENT|OFN_OVERWRITEPROMPT);
    TCHAR Str[_MAX_PATH];
    if(ReadPath(GetWorkPath(), Str, GetIniFile()))
      fileDlg.m_ofn.lpstrInitialDir = Str;
    else
      fileDlg.m_ofn.lpstrInitialDir = NULL;
    if(fileDlg.DoModal() == IDOK){
      CString fName = fileDlg.GetFileName();
	  CString Ext = fileDlg.GetFileExt();
	  if(Ext.IsEmpty())
		  fName += ".scn";
//	  SaveScenario(LPCTSTR(fName));
	}
}

void CBaseImageView::OnUpdateLoadScn(CCmdUI* pCmdUI)
{
}
// ����������� ��������� ����
void CBaseImageView::OnUpdateSaveScn(CCmdUI* pCmdUI)
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   BOOL key;
   if(pDoc->Scenario.GetCount()==0)
	   key = FALSE;
   else
	   key = TRUE;
   pCmdUI->Enable(key);
}

void CBaseImageView::OnZoomIn()
{
	ZoomIn(NULL, 1.1f);
	Invalidate(FALSE);
}

void CBaseImageView::OnZoomOut()
{
	ZoomOut(NULL, 1.1f);
	Invalidate(FALSE);
}

void CBaseImageView::OnZoomFit()
{
	m_zoomMode = SEC_ZOOMFIT;
	ZoomFit();
	m_zoomMode = SEC_ZOOMNORMAL;
	Invalidate(FALSE);
}

// ����������� �������� ���������
void CBaseImageView::OnCopyScn()
{
   CControls* pCtrls = GetControls();
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   pCtrls->curScenario.RemoveAll();
  CString s;	
  POSITION pos = pDoc->Scenario.GetHeadPosition();
  for(int i=0;i < pDoc->Scenario.GetCount();i++){
     s = pDoc->Scenario.GetNext(pos);
	 pCtrls->curScenario.AddTail(s);
  }
}

// ����������� ��������� ����
void CBaseImageView::OnUpdateCopyScn(CCmdUI* pCmdUI)
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   BOOL key;
   if(pDoc->Scenario.GetCount()==0)
	   key = FALSE;
   else
	   key = TRUE;
   pCmdUI->Enable(key);
}
//������� �������� ���������
void CBaseImageView::OnPasteScn()
{
    CImageCtrls* pImCtrls = GetImageCtrls(this);
    CControls* pCtrls = GetControls();
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
	
    pDoc->LastOperationType = O_IMAGE_SCENARIO;
    ::CopyFile(LPCTSTR(pDoc->TmpPath), LPCTSTR(pDoc->UndoTmpPath), FALSE);

//	ApplyListScenario(pImCtrls, pCtrls->curScenario, LPCTSTR(pDoc->TmpPath));
	pImCtrls->m_pDIB->SaveImage(LPCTSTR(pDoc->TmpPath));
	pDoc->ReloadDocument(LPCTSTR(pDoc->TmpPath));

	Invalidate(FALSE);
}

// ����������� ��������� ����
void CBaseImageView::OnUpdatePasteScn(CCmdUI* pCmdUI)
{
   CControls* pCtrls = GetControls();
   BOOL key;
   if(pCtrls->curScenario.GetCount()==0)
	   key = FALSE;
   else
	   key = TRUE;
   pCmdUI->Enable(key);
}
// ������������ ���������
void CBaseImageView::OnCloneDoc()
{
  CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
  pDoc->CopyImage();
}

// ����������� ��������� � ClipBoard
void CBaseImageView::OnCopy()
{
  OnCopyClipboard();
}
// ����������� ��������� � ClipBoard
void CBaseImageView::OnCopyClipboard()
{
    // Clean clipboard of contents, and copy the DIB.
    MakeLoopMessage();
    if (OpenClipboard())
    {
     BeginWaitCursor();
     EmptyClipboard();
     CDC* pDC = GetDC();
     HBITMAP hBit = CopyWindowToBitmap(m_hWnd, PW_CLIENT);
     HPALETTE hPal = GetSystemPalette();
     HDIB hWDib = BitmapToDIB(hBit, hPal);
     SetClipboardData (CF_DIB, CopyHandle((HANDLE) hWDib));
     ReleaseDC(pDC);
     CloseClipboard();
     HeapFree(GetProcessHeap(), HEAP_NO_SERIALIZE, hWDib);
     EndWaitCursor();
    }
}

// ������ Microsoft Visual C++ ������������
void CBaseImageView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CControls* pCtrls = GetControls();
	if((pCtrls->EnableOptions & I_BOUNDS_EXT) || (pCtrls->EnableOptions & I_BOUNDS_INS))
	  CreateBoundMenu(point);
	else	
	  CreateDefaultMenu(point);
}

// ������������ ���� �� ���������
void CBaseImageView::CreateDefaultMenu(CPoint point)
{
   CControls* pCtrls = GetControls();
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   
   CMenu Main;
   Main.CreatePopupMenu();

   LPCTSTR ItemText;
   UINT CloneDocFlag;
   UINT CopyDocFlag;
   UINT CopyScnFlag;
   UINT PastScnFlag;
   CloneDocFlag = MF_STRING;
   CopyDocFlag = MF_STRING;
   CopyScnFlag = MF_STRING;
   PastScnFlag = MF_STRING;
   if(pDoc->Scenario.GetCount()==0)
	   CopyScnFlag |= MF_GRAYED;
   if(pCtrls->curScenario.GetCount()==0)
	   PastScnFlag |= MF_GRAYED;
   
   ItemText = CRS("��������", "Clone");
   Main.AppendMenu(CopyDocFlag, IDD_CLONE, ItemText);
   ItemText = CRS("����������", "Copy");
   Main.AppendMenu(CopyDocFlag, IDD_COPY, ItemText);
   Main.AppendMenu(MF_SEPARATOR);

   ItemText = CRS("���������� ��������", "Copy scenario");
   Main.AppendMenu(CopyScnFlag, IDD_COPY_SCENARIO, ItemText);
   ItemText = CRS("��������� ��������", "Past scenario");
   Main.AppendMenu(PastScnFlag, IDD_PAST_SCENARIO, ItemText);

   Main.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                              point.x, point.y, this, NULL);
}

// ������������ ���� � ������ ��������� ������ 
void CBaseImageView::CreateBoundMenu(CPoint point)
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CControls* pCtrls = GetControls();
   LPCTSTR ItemText;
   
   CMenu SetUpMenu;
   SetUpMenu.CreatePopupMenu();
   UINT SetDotFlag;
   UINT SetRectFlag;
   SetDotFlag = MF_STRING;
   SetRectFlag = MF_STRING;
   if(pCtrls->EnableCustomDots)
      SetDotFlag |= MF_CHECKED;
   else if(pCtrls->EnableTracker)
      SetRectFlag |= MF_CHECKED;

   if(pDoc->Tracker.GetEnableState() || pBCtrls->CustomDots.GetSize()){
      SetDotFlag |= MF_GRAYED;
      SetRectFlag |= MF_GRAYED;
   }

   if(pCtrls->CurTypeBound == BOUND_POLYGON)
      SetRectFlag |= MF_GRAYED;

   ItemText = CRS("�����", "Dots");
   SetUpMenu.AppendMenu(SetDotFlag, IDD_BOUND_SETDOTS, ItemText);
   ItemText = CRS("������", "Contour");
   SetUpMenu.AppendMenu(SetRectFlag, IDD_BOUND_SETRECT, ItemText);

   CMenu ScreenMenu;
   ScreenMenu.CreatePopupMenu();
   UINT ScreenRoundFlag;
   UINT ScreenEllipseFlag;
   UINT ScreenRectFlag;
   UINT ScreenPlgFlag;
   ScreenRoundFlag = MF_STRING;
   ScreenEllipseFlag = MF_STRING;
   ScreenRectFlag = MF_STRING;
   ScreenPlgFlag = MF_STRING;
   if(pCtrls->CurTypeBound == BOUND_ROUND)
	   ScreenRoundFlag |= MF_CHECKED;
   else if(pCtrls->CurTypeBound == BOUND_ELLIPSE)
	   ScreenEllipseFlag |= MF_CHECKED;
   else if(pCtrls->CurTypeBound == BOUND_RECT)
	   ScreenRectFlag |= MF_CHECKED;
   else if(pCtrls->CurTypeBound == BOUND_POLYGON)
	   ScreenPlgFlag |= MF_CHECKED;

   if(pCtrls->EnableTracker)
      ScreenPlgFlag |= MF_GRAYED;

   ItemText = CRS("����", "Round");
   ScreenMenu.AppendMenu(ScreenRoundFlag, IDD_BOUND_SCRROUND, ItemText);
   ItemText = CRS("������", "Ellpse");
   ScreenMenu.AppendMenu(ScreenEllipseFlag, IDD_BOUND_SCRELLIPSE, ItemText);
   ItemText = CRS("�������������", "Rectangle");
   ScreenMenu.AppendMenu(ScreenRectFlag, IDD_BOUND_SCRRECT, ItemText);
   ItemText = CRS("�������", "Polygon");
   ScreenMenu.AppendMenu(ScreenPlgFlag, IDD_BOUND_SCRPLG, ItemText);

   CMenu Main;
   Main.CreatePopupMenu();

   UINT ApplyFlag;
   UINT RemoveCurFlag;
   UINT RemoveLastFlag;
   UINT RemoveAllFlag;
   UINT SetTypeFlag;
   UINT ScreenFlag;
   
   SetTypeFlag = MF_POPUP;
   ScreenFlag = MF_POPUP;
   RemoveAllFlag = MF_STRING;
   RemoveCurFlag = MF_STRING;
   RemoveLastFlag = MF_STRING;

   if(!pDoc->Tracker.GetEnableState() && pBCtrls->CustomDots.GetSize()==0){
     ApplyFlag = MF_STRING|MF_GRAYED;
     RemoveCurFlag = MF_STRING|MF_GRAYED;
   }
   else{
     ApplyFlag = MF_STRING;
     RemoveAllFlag = MF_STRING;
     RemoveLastFlag = MF_STRING;
   }
   
   ItemText = CRS("��������", "Apply");
   Main.AppendMenu(ApplyFlag, IDD_BOUND_APPLY, ItemText);

   ItemText = CRS("�������� ", "Cancel");
   Main.AppendMenu(RemoveCurFlag, IDD_BOUND_CURREMOVE, ItemText);
   ItemText = CRS("������� ��������� �������", "Remove last bound");
   Main.AppendMenu(RemoveLastFlag, IDD_BOUND_LASTREMOVE, ItemText);
   ItemText = CRS("������� ��� �������", "Remove all bounds");
   Main.AppendMenu(RemoveAllFlag, IDD_BOUND_ALLREMOVE, ItemText);
   
   ItemText = CRS("��� ���������", "Set up type");
   Main.AppendMenu(SetTypeFlag, (UINT)SetUpMenu.m_hMenu, ItemText);
   ItemText = CRS("��� �������", "Bound type");
   Main.AppendMenu(ScreenFlag, (UINT)ScreenMenu.m_hMenu, ItemText);

   Main.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
                              point.x, point.y, this, NULL);
}
//������� ������������� ������� 
void CBaseImageView::OnApplyBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CControls* pCtrls = GetControls();
   CRect R;
   if(pDoc->Tracker.GetEnableState() && pDoc->Tracker.GetBoundRect(R)){
	   pBCtrls->CustomDots.SetSize(4);
	   int W = R.Width()/2;
	   int H = R.Height()/2;
	   pBCtrls->CustomDots[0].x = R.left + W;
	   pBCtrls->CustomDots[0].y = R.top;
	   pBCtrls->CustomDots[1].x = R.right;
	   pBCtrls->CustomDots[1].y = R.top + H;
	   pBCtrls->CustomDots[2].x = R.left + W;
	   pBCtrls->CustomDots[2].y = R.bottom;
	   pBCtrls->CustomDots[3].x = R.left;
	   pBCtrls->CustomDots[3].y = R.top + H;
   }
   if(pBCtrls->CustomDots.GetSize()){
	   if(pCtrls->EnableOptions & I_BOUNDS_EXT)
	      pBCtrls->AddBound(pCtrls->CurTypeBound, 1);
	   else if(pCtrls->EnableOptions & I_BOUNDS_INS)
	      pBCtrls->AddBound(pCtrls->CurTypeBound, 0);
   }
   pDoc->Tracker.SetEnableState(FALSE);
   pBCtrls->RemoveCurBound();
   pBCtrls->CustomDots.RemoveAll();
   Invalidate(FALSE);
}
//������� ��������� �������
void CBaseImageView::OnRemoveCurBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
	pBCtrls->CustomDots.RemoveAll();
	pDoc->Tracker.SetEnableState(FALSE);
    pBCtrls->RemoveCurBound();
	Invalidate(FALSE);
}

void CBaseImageView::OnRemoveLastBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
	pBCtrls->CustomDots.RemoveAll();
	pDoc->Tracker.SetEnableState(FALSE);
    pBCtrls->RemoveLastBound();
	Invalidate(FALSE);
}

// ������� ��� �������
void CBaseImageView::OnRemoveAllBound()
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    CControls* pCtrls = GetControls();
    CBoundCtrls* pBCtrls = GetBoundCtrls(this);
	pDoc->Tracker.SetEnableState(FALSE);
	pBCtrls->CustomDots.RemoveAll();
	if(pCtrls->EnableOptions & I_BOUNDS_EXT)
	   pBCtrls->RemoveAllBound();
	else if(pCtrls->EnableOptions & I_BOUNDS_INS)
	   pBCtrls->RemoveAllBound();
    pBCtrls->RemoveCurBound();
	pCtrls->EnableOptions &= ~I_BOUNDS_INS;
	pCtrls->EnableOptions |= I_BOUNDS_EXT;
	Invalidate(FALSE);
}

// ��������� ������ ����� ������������� �������
void CBaseImageView::OnSetupDotsBound()
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    CBoundCtrls* pBCtrls = GetBoundCtrls(this);
	CControls* pCtrls = GetControls();
	pCtrls->EnableCustomDots = TRUE;
	pCtrls->EnableTracker = FALSE;
	pDoc->Tracker.SetEnableState(FALSE);
}
// ��������� ������������� ������ 
void CBaseImageView::OnSetupRectBound()
{
    CBoundCtrls* pBCtrls = GetBoundCtrls(this);
	CControls* pCtrls = GetControls();
	pCtrls->EnableTracker = TRUE;
	pCtrls->EnableCustomDots = FALSE;
}

// ��������� ������ � ���� �����
void CBaseImageView::OnScrRoundBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CImageCtrls* pImCtrls = GetImageCtrls(this);
   CControls* pCtrls = GetControls();
   pCtrls->CurTypeBound = BOUND_ROUND;
   CRect R;
   if(pDoc->Tracker.GetEnableState() && pDoc->Tracker.GetBoundRect(R)){
	   pBCtrls->CustomDots.SetSize(4);
	   int W = R.Width()/2;
	   int H = R.Height()/2;
	   pBCtrls->CustomDots[0].x = R.left + W;
	   pBCtrls->CustomDots[0].y = R.top;
	   pBCtrls->CustomDots[1].x = R.right;
	   pBCtrls->CustomDots[1].y = R.top + H;
	   pBCtrls->CustomDots[2].x = R.left + W;
	   pBCtrls->CustomDots[2].y = R.bottom;
	   pBCtrls->CustomDots[3].x = R.left;
	   pBCtrls->CustomDots[3].y = R.top + H;
   }
   if(pBCtrls->CustomDots.GetSize()){
      pBCtrls->SetCurBound(pCtrls->CurTypeBound);
   }
   Invalidate(FALSE);
}

// ��������� ������ � ���� �������
void CBaseImageView::OnScrEllipseBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CImageCtrls* pImCtrls = GetImageCtrls(this);
   CControls* pCtrls = GetControls();
   pCtrls->CurTypeBound = BOUND_ELLIPSE;
   CRect R;
   if(pDoc->Tracker.GetEnableState() && pDoc->Tracker.GetBoundRect(R)){
	   pBCtrls->CustomDots.SetSize(4);
	   int W = R.Width()/2;
	   int H = R.Height()/2;
	   pBCtrls->CustomDots[0].x = R.left + W;
	   pBCtrls->CustomDots[0].y = R.top;
	   pBCtrls->CustomDots[1].x = R.right;
	   pBCtrls->CustomDots[1].y = R.top + H;
	   pBCtrls->CustomDots[2].x = R.left + W;
	   pBCtrls->CustomDots[2].y = R.bottom;
	   pBCtrls->CustomDots[3].x = R.left;
	   pBCtrls->CustomDots[3].y = R.top + H;
   }
   if(pBCtrls->CustomDots.GetSize()){
      pBCtrls->SetCurBound(pCtrls->CurTypeBound);
   }
   Invalidate(FALSE);
}

// ��������� � ���� �������������� 
void CBaseImageView::OnScrRectBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CImageCtrls* pImCtrls = GetImageCtrls(this);
   CControls* pCtrls = GetControls();
   pCtrls->CurTypeBound = BOUND_RECT;
   CRect R;
   if(pDoc->Tracker.GetEnableState() && pDoc->Tracker.GetBoundRect(R)){
	   pBCtrls->CustomDots.SetSize(4);
	   int W = R.Width()/2;
	   int H = R.Height()/2;
	   pBCtrls->CustomDots[0].x = R.left + W;
	   pBCtrls->CustomDots[0].y = R.top;
	   pBCtrls->CustomDots[1].x = R.right;
	   pBCtrls->CustomDots[1].y = R.top + H;
	   pBCtrls->CustomDots[2].x = R.left + W;
	   pBCtrls->CustomDots[2].y = R.bottom;
	   pBCtrls->CustomDots[3].x = R.left;
	   pBCtrls->CustomDots[3].y = R.top + H;
   }
   if(pBCtrls->CustomDots.GetSize()){
      pBCtrls->SetCurBound(pCtrls->CurTypeBound);
   }
   Invalidate(FALSE);
}

void CBaseImageView::OnScrPlgBound()
{
   CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
   CBoundCtrls* pBCtrls = GetBoundCtrls(this);
   CImageCtrls* pImCtrls = GetImageCtrls(this);
   CControls* pCtrls = GetControls();
   pCtrls->CurTypeBound = BOUND_POLYGON;
   pCtrls->EnableTracker = FALSE;
   pCtrls->EnableCustomDots = TRUE;
   CRect R;
   if(pBCtrls->CustomDots.GetSize()){
      pBCtrls->SetCurBound(pCtrls->CurTypeBound);
   }
   Invalidate(FALSE);
}

//������������� �������������� ��������� ������
void CBaseImageView::BeginTracker(CPoint P1)
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    CControls* pCtrls = GetControls();

    CPoint TL(P1);
    CClientDC dc(this);
    OnPrepareDC(&dc);
	if(pDoc->Tracker.GetEnableState()){
       pDoc->Tracker.Track(this, &dc, TL, true, false);
	}
	else{
	   pDoc->Tracker.SetEnableState(TRUE);
       pDoc->Tracker.Track(this, &dc, TL, true, true);
	}

	pDoc->Tracker.SetDragingState(TRUE);
    Invalidate(FALSE);
}

//�������������� �������������� ��������� ������
void CBaseImageView::DragTracker(CPoint P2)
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
	
    CClientDC dc(this);
    OnPrepareDC(&dc);
    pDoc->Tracker.Track(this, &dc, P2, false);
}

//����� �������������� �������������� ��������� ������
void CBaseImageView::DropTracker(CPoint P2)
{
    CBaseImageDoc* pDoc = (CBaseImageDoc*)GetDocument();
    CControls* pCtrls = GetControls();
	pDoc->Tracker.SetDragingState(FALSE);

    Invalidate(FALSE);
}

