// 
// Stingray Software Extension Classes
// Copyright (C) 1995 Stingray Software Inc.
// All Rights Reserved
// 
// This source code is only intended as a supplement to the
// Stingray Extension Class product.
// See the SEC help files for detailed information
// regarding using SEC classes.
// 
//  Author:		MSW
//  Description:	Implementation of SECPanWnd -
//                      the panning overview window
//  Created:		Tue Jun 27 15:12:52 1995
//
//  CHANGELOG:
//
//   MSW     9/30/95   Code Review
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __SECPANWN_H__
#include "..\..\Include\PanWnd\secpanwn.h"
#endif

#ifndef __SECPANVW_H__
#include "..\..\Include\PanWnd\secpanvw.h"
#endif

#ifndef __SECRES_H__
#include "..\..\Include\PanWnd\secres.h" //for IDC_PAN_OVERVI
#endif


#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW


BEGIN_MESSAGE_MAP(SECPanWnd, CWnd)
    //{{AFX_MSG_MAP(SECPanWnd)
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_SETCURSOR()
    ON_WM_PAINT()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	
//	SECPanWnd constructor
//	
//	Load our cursor, setup the back pointer to
//	the view.
//	
//@doc SECPanWnd
//@mfunc Constructs an SECPanWnd overview window object by referencing a back pointer to the SECPanView object.
//@rdesc void
//@parm SECPanView * | pPanView | Pointer to the SECPanView object.
//@xref<mf SECPanWnd::Create>
//@xref<c SECPanWnd> 
SECPanWnd::SECPanWnd(SECPanView * pPanView)
{
    m_pPanView  = pPanView;
    m_bCaptured = FALSE;
    m_hOverviewCursor = AfxGetApp()->LoadCursor(IDC_PAN_OVERVIEW);
}

//	
//	~SECPanWnd
//	
//	Nuke the cursor
//
//      tell the view we've been destroyed.
//
SECPanWnd::~SECPanWnd()
{
    if (m_pPanView)
	m_pPanView->OverviewDestroyed();
    
    if (m_hOverviewCursor)
	DestroyCursor(m_hOverviewCursor);
}

//	
//	Create
//	
//	Create the window, set WS_EX_TOPMOST
//	and also make the framework think it's a 
//	view so that it gets the correct background and
//	handles onerasebackgrnd for us.
//
//      rectCreate specifies the size of the overview window
//      It looks best when it has the same dimensions as your
//      document. e.g. 2/3 ratio, etc..  If not you will
//      have a dead area of the overview window.
//
//@doc SECPanWnd
//@mfunc Creates the overview window and specifies its initial position and size.
//@rdesc Returns non-zero if successful; zero otherwise.
//@parm CRect | rectCreate | The rectangle to be created.
//@parm  LPCTSTR | lpszTitle | The title for the overview window. Default is NULL.
//@xref<mf SECPanWnd::SECPanWnd>
//@xref<mf SECPanWnd::PreCreateWindow>
//@xref<c SECPanWnd>
BOOL SECPanWnd::Create(CRect rectCreate, LPCTSTR lpszTitle)
{
    CString strDum;
	strDum.LoadString(IDS_SEC_PWND_TITL);
	BOOL bRetVal =
	CWnd::CreateEx(WS_EX_TOPMOST,
		       AfxRegisterWndClass(CS_CLASSDC,
					   NULL,
					   (HBRUSH)(COLOR_WINDOW + 1),
					   NULL),
					   lpszTitle != NULL ? lpszTitle : strDum,
		       WS_POPUP | WS_VISIBLE| WS_CLIPSIBLINGS|
		       WS_DLGFRAME|WS_SYSMENU|WS_BORDER,
		       rectCreate.left, rectCreate.top, rectCreate.Width(),
		       rectCreate.Height(), NULL, NULL);
    return bRetVal;
}

//	
//	PreCreateWindow
//	
//	Make sure that the framework thinks we're a view
//	
//@doc SECPanWnd
//@mfunc Implements the overview window’s style settings at creation.
//@rdesc Default: Returns TRUE.
//@parm CREATESTRUCT& | cs | The overview window’s style settings.
//@xref<mf SECPanWnd::SECPanWnd>
//@xref<mf SECPanWnd::Create>
//@xref<c SECPanWnd>
BOOL SECPanWnd::PreCreateWindow(CREATESTRUCT& cs)
{
    if (cs.lpszClass == NULL){
	cs.lpszClass = _T("SECPanWnd");
	
    }
    
    return TRUE;
}

//
//	SetPanRect
// 
//	The viewport for the view has changed
//	so store the new overview panning rectangle
//	coords in m_rectLogPan and draw the new
//	rectangle, erasing the old one first.
//
//      rectNewPan is in logical coordinates
//
//@doc SECPanWnd
//@mfunc Sets the rectangle representing the overview window.
//@rdesc void 
//@parm CRect | rectNewLogPan | The rectangle representing the newly created or updated overview window.
//@xref<mf SECPanWnd::DrawPanRect>
//@xref<c SECPanWnd>
void SECPanWnd::SetPanRect(CRect rectNewLogPan)
{
    ASSERT_VALID(this);
    CClientDC dc(this);
    
    m_pPanView->PreOverview();
    m_pPanView->OnPrepareDC(&dc);

    DrawPanRect(&dc);

    
//
//  Code to fix pan rectangle problem
//
    CRect rectDevClient;
    //Device coords
    GetClientRect(&rectDevClient);
    ClientToScreen(&rectDevClient); //Device screen coords

    CPoint tmpPoint;
    CRect rectDevPan = rectNewLogPan;
	
    tmpPoint.x = rectDevPan.left;
    tmpPoint.y = rectDevPan.top;
    dc.LPtoDP(&tmpPoint);
    rectDevPan.left = tmpPoint.x;
    rectDevPan.top = tmpPoint.y;
	
    tmpPoint.x = rectDevPan.right;
    tmpPoint.y = rectDevPan.bottom;
    dc.LPtoDP(&tmpPoint);
    rectDevPan.right = tmpPoint.x;
    rectDevPan.bottom = tmpPoint.y;

    if (rectDevPan.Width() > rectDevClient.Width())
    {
	rectNewLogPan.right -= rectNewLogPan.left;
	rectNewLogPan.left = 0;
    }
    if (rectDevPan.Height() > rectDevClient.Height())
    {
	rectNewLogPan.bottom -= rectNewLogPan.top;
	rectNewLogPan.top = 0;
    }
//
    m_rectLogPan = rectNewLogPan;
    DrawPanRect(&dc);
    m_pPanView->PostOverview();    
}

//	
//	OnPaint
//	
//	Put the view into overview window mode
//	and then call OnDraw so that the view
//	does all the drawing for us.
//	
void SECPanWnd::OnPaint()
{
    ASSERT_VALID(this);
    CPaintDC dc(this);
    BOOL bDirty = FALSE;
    CRect rectClip;
	CRect rectTest = m_rectLogPan;
    dc.GetClipBox(&rectClip);
    rectClip.InflateRect(2,2);
    if (rectTest.IntersectRect(&rectTest,&rectClip)){
	bDirty = TRUE;
    }
    
    m_pPanView->PreOverview();
    m_pPanView->OnPrepareDC(&dc);
    m_pPanView->OnDraw(&dc);
    m_pPanView->PostOverview();

    
    if (bDirty)
	DrawPanRect(&dc);
}

//	
//	DrawPanRect
//	
//      Draws a dotted rectangle around the currently
//	visible viewing area for panning.
// 
//      Assumes that m_rectLogPan is in log coords
//
//@doc SECPanWnd
//@mfunc Draws the rectangle representing the overview window.
//@rdesc void 
//@parm CDC * | pDC | Pointer to the device context used to draw the rectangle.
//@xref<mf SECPanWnd::SetPanRect>
//@xref<c SECPanWnd>
void SECPanWnd::DrawPanRect(CDC * pDC)
{
    ASSERT_VALID(this);
    ASSERT(pDC != NULL);
    
    CPen pen;
    
    pDC->SaveDC();
    pDC->SetROP2(R2_NOTXORPEN);
    pen.CreatePen(PS_DOT,  0, RGB(0, 0, 0));
    
    pDC->SelectObject(&pen);
    
    pDC->Rectangle(m_rectLogPan);
    pDC->RestoreDC(-1);
    return;    
}

//	
//	OnUpdate
//	
//	Called to invalidate the correct regions
//	of the overview wnd when needed.
//	
//@doc SECPanWnd
//@mfunc Updates the overview window when the user scrolls or updates the panning view.  Invalidates the overview
// window’s client area to force a repaint.
//@rdesc void 
//@parm CView * | pSender | Identifies the panning view being scrolled or updated by the user.
//@parm  LPARAM | lHint | Specifies what part of the overview window needs updating.
//@parm  CObject * |  pHint | Address of overview window being updated.
//@xref<c SECPanWnd>
void SECPanWnd::OnUpdate(CView * pSender, LPARAM lHint, CObject * pHint)
{
    ASSERT(pSender != NULL);
    ASSERT_VALID(this);
    
    CClientDC dc(this);
    m_pPanView->PreOverview();
    m_pPanView->OnPrepareDC(&dc);
    m_pPanView->OnUpdateOverview(pSender,lHint,pHint,&dc,this);
    m_pPanView->PostOverview();
}

//	
//	ClipOverviewCursor
//	
//	Uses the size of the panning rectangle and
//	the point where the user clicked in the rectangle
//	plust the client window rect to calculate a rectangle
//	to clip the cursor too so that the user can't drag
//	the pan rectangle out of the visible part of the view.
//
void SECPanWnd::ClipOverviewCursor()
{
    //Don't let them move the rect out of the window!
    CRect rectDevClip;
    CRect rectDevClient;
    CSize szDevDrag  = m_szLogDrag;
    CRect rectDevPan = m_rectLogPan;
    
    // Convert m_szLogDrag and m_rectLogPan
    // to device coords for clipping rectangle calculations.
    {
	CClientDC dc(this);
	m_pPanView->PreOverview();
	m_pPanView->OnPrepareDC(&dc);
	dc.LPtoDP(&szDevDrag);
	
	CPoint tmpPoint;
	
	tmpPoint.x = rectDevPan.left;
	tmpPoint.y = rectDevPan.top;
	dc.LPtoDP(&tmpPoint);
	rectDevPan.left = tmpPoint.x;
	rectDevPan.top = tmpPoint.y;
	
	tmpPoint.x = rectDevPan.right;
	tmpPoint.y = rectDevPan.bottom;
	dc.LPtoDP(&tmpPoint);
	rectDevPan.right = tmpPoint.x;
	rectDevPan.bottom = tmpPoint.y;
	
	m_pPanView->PostOverview();
    } //BLOCK DC off
    
    //Device coords
    GetClientRect(&rectDevClient);
    ClientToScreen(&rectDevClient); //Device screen coords
    
    //Now calculate the cursor clipping rectangle in device coords.
    rectDevClip.left  = rectDevClient.left + szDevDrag.cx;
    rectDevClip.top   = rectDevClient.top + szDevDrag.cy;
    rectDevClip.right = rectDevClient.right -
	(rectDevPan.Width() - szDevDrag.cx);
    rectDevClip.bottom = rectDevClient.bottom -
	(rectDevPan.Height() - szDevDrag.cy);

//
//  Code to fix clip rectangle problem:
//  When the viewport is larger than the actual diagram, the pan rectangle
//  is larger than the pan window, so the clip rectangle is invalid.
//  Adjust the clip rectangle accordingly.
//
    if (rectDevClip.right < rectDevClip.left)
	rectDevClip.right = rectDevClip.left + rectDevPan.left;
    if (rectDevClip.bottom < rectDevClip.top)
	rectDevClip.bottom = rectDevClip.top + rectDevPan.top;
        
    //Clip the cursor in device coords.
    ::ClipCursor(&rectDevClip);
}

//	
//	OverviewDPtoLP
//	
//	Takes a point in device coords and does
//	the overview window magic to convert it to
//	logical coordinates.
//	
void SECPanWnd::OverviewDPtoLP(CPoint * pointConvert)
{    
    ASSERT_VALID(this);
    ASSERT(pointConvert != NULL);
    
    m_pPanView->PreOverview();    
    {
	CClientDC dc(this);
	m_pPanView->OnPrepareDC(&dc);
	dc.DPtoLP(pointConvert);
    }//block of DC
    
    m_pPanView->PostOverview();
    
}

//
// SECPanWnd message handlers
//


//	
//	OnLButtonDown
//	
//	If we get a button down inside the pan rect,
//	grab the cursor, clip it and start dragging
//	the pan rectangle
//	
void SECPanWnd::OnLButtonDown(UINT nFlags, CPoint pointDev) 
{
    nFlags;     //UNUSED
    CPoint pointLog = pointDev;
    OverviewDPtoLP(&pointLog);
    
    //Did the user press in the panner rectangle in the overview wnd?
    if (m_rectLogPan.PtInRect(pointLog)){
	
	m_bCaptured = TRUE;
	SetCapture();
	::SetCursor(m_hOverviewCursor);
	OnSetCursor(NULL,HTCLIENT,0); //Force update of cursor
	
	//Record where they are from the upper left corner of the rect.
	m_szLogDrag.cx = pointLog.x - m_rectLogPan.left;
	m_szLogDrag.cy = pointLog.y - m_rectLogPan.top;
	ClipOverviewCursor();

	//Tell the view where we started
	m_pPanView->m_ptLogStartDrag = pointLog;
	
    } // end if - it's not in the panner rect, ignore.    
}

//	
//	OnMouseMove
//	
//	If the user is dragging the panner rectangle,
//	calculate the new location, erase the old rect,
//	draw the new one and update everything.
//	
void SECPanWnd::OnMouseMove(UINT nFlags, CPoint pointDev) 
{
    nFlags; //UNUSED
    CPoint pointLog = pointDev;

    if (m_bCaptured){
	OverviewDPtoLP(&pointLog);	
	CRect rectNewLogPan;
	rectNewLogPan.left   = pointLog.x - m_szLogDrag.cx;
	rectNewLogPan.top    = pointLog.y - m_szLogDrag.cy;
	rectNewLogPan.right  = rectNewLogPan.left + m_rectLogPan.Width();
	rectNewLogPan.bottom = rectNewLogPan.top  + m_rectLogPan.Height();
	SetPanRect(rectNewLogPan);
	
	if (m_pPanView->m_panMode == SEC_PANINSTANT){
	    m_pPanView->UpdateViewport(&pointLog);
	    m_pPanView->m_ptLogStartDrag = pointLog;
	}
    }
    //nothing going on, ignore this mousemove.
}

//	
//	OnLButtonUp
//	
//	User is done dragging the panning rectangle
//	undo the capture and the clipped cursor, then
//	update everything and change the cursor back too.
//	
void SECPanWnd::OnLButtonUp(UINT nFlags, CPoint pointDev) 
{
    nFlags; //UNUSED
    if (m_bCaptured){
	CPoint pointLog = pointDev;
	OverviewDPtoLP(&pointLog);
	
	ReleaseCapture();
	m_bCaptured = FALSE;
	::ClipCursor(NULL); //Release the cursor
	
	CRect rectNewLogPan;
	rectNewLogPan.left   = pointLog.x - m_szLogDrag.cx;
	rectNewLogPan.top    = pointLog.y - m_szLogDrag.cy;
	rectNewLogPan.right  = rectNewLogPan.left + m_rectLogPan.Width();
	rectNewLogPan.bottom = rectNewLogPan.top  + m_rectLogPan.Height();
	SetPanRect(rectNewLogPan);
	
	m_pPanView->UpdateViewport(&pointLog);
    }    
    //Nothing going on, ignore onlbuttonup
}

//	
//	OnSetCursor
//	
//	Set the cursor to the cool hand with dotted rect
//	if the user is dragging the pan rectangle.
//	
BOOL SECPanWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   // ASSERT(pWnd != NULL);
    ASSERT_VALID(this);
    if (nHitTest != HTCLIENT)
	return CWnd::OnSetCursor(pWnd,nHitTest,message);
    
    if (m_bCaptured == TRUE)
	::SetCursor(m_hOverviewCursor);
    else
	::SetCursor(::LoadCursor(NULL,IDC_ARROW));
    
    return TRUE;
}

void SECPanWnd::OnClose() 
{
    if (m_pPanView)
	m_pPanView->OverviewDestroyed();
    CWnd::OnClose();
}


#ifdef _DEBUG

void SECPanWnd::Dump(CDumpContext & dc)
{
    CWnd::Dump(dc);

    dc << "panview pointer is\n ";
    m_pPanView->Dump(dc);

    dc << "overview cursor = "<< (UINT)m_hOverviewCursor << "\n";
    dc << "viewport  = " << m_rectLogPan << "\n";
    
}

void SECPanWnd::AssertValid()
{
    CWnd::AssertValid();
    ASSERT (m_pPanView != NULL);
}



#endif





