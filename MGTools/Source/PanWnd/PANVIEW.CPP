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
//  Description:	Implementation for SECPanView : SECZoomView
//  Created:		Fri Jun 16 14:01:33 1995
//
//  CHANGELOG:
//    MSW    9/30/95   Code Review
//    MSW    12/10/95  DoScroll logic fixes.
//
//
// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL


#ifndef __SECZMVW_H__
#include "..\..\Include\PanWnd\seczmvw.h"
#endif

#ifndef __SECPANVW_H__
#include "..\..\Include\PanWnd\secpanvw.h"
#endif

#ifndef __SECPANWN_H__
#include "..\..\Include\PanWnd\secpanwn.h"
#endif

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

IMPLEMENT_DYNAMIC(SECPanView, SECZoomView)

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(SECPanView, SECZoomView)
    //{{AFX_MSG_MAP(SECPanView)
    //}}AFX_MSG_MAP
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_SIZE()
END_MESSAGE_MAP()

//	
//	SECPanView constructor
//
//@doc SECPanView
//@mfunc If you derive your class from SECPanView instead of CScrollView, construction will take place automatically,
// and you do not need to explicitly call the SECPanView constructor.<nl>SECPanView is created with the default zoom
// mode set to SEC_PANDELAY.
//@rdesc void
//@xref<c SECPanView>
SECPanView::SECPanView() : SECZoomView()
{
    m_nCursorID   = 0;
    m_panMode     = SEC_PANDELAY; //Default
    m_bPanning    = FALSE;
    m_hOrigCursor = 0;
    m_hDragCursor = 0;
    m_pOverview   = NULL;
} 

//	
//	~SECPanView
//	
//      Take care of the overview window if it's up.
//
SECPanView::~SECPanView()
{
    if (m_hDragCursor)
	DestroyCursor(m_hDragCursor);
    
    if (m_pOverview){
	m_pOverview->DestroyWindow();
	m_pOverview = NULL;
    }

	// clean up any overview allocations
	CWnd* pWnd;
	POSITION pos=m_listAllocOverview.GetHeadPosition();
	while(pos) {
		pWnd=(CWnd *)m_listAllocOverview.GetNext(pos);
		delete pWnd;
		}
}

#ifdef _DEBUG
void SECPanView::Dump(CDumpContext& dc) const
{
    //Zooming info is all that's of interest for now.
    SECZoomView::Dump(dc);
}

void SECPanView::AssertValid() const
{
    // Bypass CScrollView because of MM_ANISOTROPIC map mode
    CView::AssertValid();
} // AssertValid

#endif //_DEBUG

//Operations

//	
//	SetPanMode
//	
//	Just change the mode
//	

//@doc SECPanView
//@mfunc Changes the pan mode to either SEC_PANINSTANT or SEC_PANDELAY.
//@rdesc void 
//@parm SECPanMode | newPanMode | SEC_PANINSTANT or SEC_PANDELAY
//@xref<mf SECPanView::GetPanMode>
//@xref<c SECPanView>
void SECPanView::SetPanMode(SECPanMode newPanMode)
{
    ASSERT(newPanMode != SEC_PANDELAY ||
	   newPanMode != SEC_PANINSTANT);
    
    m_panMode = newPanMode;
}

//	
//	StartPan
//
//      Start the panning process.  Store the starting
//      point so that when we're done, we know what to do.
//
//	ptPan is in logical points, up to caller to
//	translate from device to logical coords.
//
//@doc SECPanView
//@mfunc Call StartPan to signal SECPanView that the user has started panning the view.  SECPanView displays the pan 
// cursor if one is available.  This member function can be overridden if you want to change some aspect of the panning behavior.
//@rdesc void 
//@parm CPoint * | ptPan | The start of the pan, used by SECPanView to calculate the amount of panning that takes place.
//@parm UINT | nCursorID | The resource ID for a cursor you would like to be displayed when the user pans.
//@xref<mf SECPanView::ContinuePan>
//@xref<mf SECPanView::EndPan>
//@xref<c SECPanView>
//@end
void SECPanView::StartPan(CPoint * ptPan,
			  UINT nCursorID /* = 0 */)
{
    ASSERT(ptPan != NULL);
    ASSERT_VALID(this);
    
    m_bPanning = TRUE;
    
    //Change the cursor
    m_nCursorID   = nCursorID;    
    m_hDragCursor = AfxGetApp()->LoadCursor(m_nCursorID);
    
	// SEC User's::>
	// If you ASSERT here, there's a problem			 
	// with your resource setup in your DLL/LIB.
	// Be sure to follow what the SEC Samples										 
	// are doing..
	
    ASSERT (m_hDragCursor != 0);


    OnSetCursor(NULL,HTCLIENT,0);
    
    m_ptLogStartDrag = *ptPan;
    
    SetCapture();
}

//	
//	ContinuePan
//	
//	Continue panning, if we're in delay mode
//	just update pan rectangle.  If we're in instant
//	mode, actually perform a pan.
//
//	ptPan should be logical coordinates.
//	
//@doc SECPanView
//@mfunc As the user pans the view, you must call ContinuePan so that SECPanView will know what the user is doing with the
// view.  In SEC_PANINSTANT mode, ContinuePan automatically updates the view to reflect the panning.  This member function
// can be overridden if you want to change some aspect of the panning behavior.
//@rdesc void 
//@parm CPoint * | ptPan | The next point in the pan process.
//@xref<mf SECPanView::StartPan>
//@xref<mf SECPanView::EndPan>
//@xref<c SECPanView>
void SECPanView::ContinuePan(CPoint * ptPan)
{
    ASSERT(m_bPanning != FALSE);
    
    // Move the pan rectangle to reflect the new panning.    
    
    if (m_panMode == SEC_PANINSTANT){
	PerformPan(ptPan);
	UpdateOverviewRect();
    }
}

//	EndPan
//	
//	Turn out the lights, the panning's over!
//    
//      Release the capture, return the
//      cursor to the original value, 
//      do the final pan and update the overview wnd.
//
//      ptPan should be in logical coordinates
//  
//@doc SECPanView
//@mfunc To end the panning process, call EndPan.  If the SECPanView is in SEC_PANDELAY mode, the view will not be updated
// until this point.  Even if you are in SEC_PANINSTANT, you have to call EndPan because the view performs internal calculations
// and frees any panning resources.  This member function can be overridden if you want to change some aspect of the panning behavior.
//@rdesc void 
//@parm CPoint * | ptPan | The final point in the pan process.
//@xref<mf SECPanView::StartPan>
//@xref<mf SECPanView::ContinuePan>
//@xref<c SECPanView>
void SECPanView::EndPan(CPoint * ptPan)
{
    ASSERT(m_bPanning != FALSE);
    ReleaseCapture();
    
    m_bPanning = FALSE;
    OnSetCursor(NULL,HTCLIENT,0);
    
    PerformPan(ptPan);
    UpdateOverviewRect();
}

//	
//	PerformPan
//	
//	Does the actual scrolling of the window and
//	also the updating of scrollbars.
//	
//      ptEndPan must be in logical coords.
//      bReversePan indicates that it's not a drag, but
//      a reverse pan, like the overview does.
//
//      12/10 - broke out scrolling logic into DoScroll()
//   
//
void SECPanView::PerformPan(CPoint * ptEndPan, BOOL bReversePan /* FALSE */)
{
    CClientDC dc(this);

    OnPrepareDC(&dc);
    CSize sizeScroll;
    if (bReversePan){
	sizeScroll.cx = ptEndPan->x - m_ptLogStartDrag.x;
	sizeScroll.cy = ptEndPan->y - m_ptLogStartDrag.y;
    }
    else{
	sizeScroll.cx = m_ptLogStartDrag.x - ptEndPan->x;
	sizeScroll.cy = m_ptLogStartDrag.y - ptEndPan->y;
    }
    dc.LPtoDP(&sizeScroll);
    DoScroll(sizeScroll,TRUE);    
}


//
//   Overview Window Operations 
//


//	
//	ShowOverviewWnd
//	
//	Display the overview window.
//	Make sure it is initialized with the right
//	rectangle to begin with.
//
//@doc SECPanView
//@mfunc Calling ShowOverviewWnd causes the PanWnd overview window to be displayed.  Once displayed the overview window
// will automatically be updated to show the current visible portion of the view.  The user can also start panning using 
// the overview window once it is shown.
//@rdesc Nonzero if the window could be created, otherwise 0.
//@parm CRect | rectLocation | Rectangle specifying the size of the overview window.
//@xref<mf SECPanView::HideOverviewWnd>
//@xref<mf SECPanView::IsOverviewShown>
//@xref<c SECPanView>
BOOL SECPanView::ShowOverviewWnd(CRect rectLocation)
{    
    ASSERT_VALID(this);
    if (m_pOverview == NULL) {
        m_pOverview = new SECPanWnd(this);
	
        ASSERT(m_pOverview != NULL);
	
        m_pOverview->Create(rectLocation);
        m_pOverview->ShowWindow(SW_SHOW);
		m_listAllocOverview.AddTail(m_pOverview);
    }
    else {
	m_pOverview->MoveWindow(rectLocation);
	m_pOverview->OnUpdate(this,0L,NULL);
    }
    m_pOverview->UpdateWindow();
    
    UpdateOverviewRect();
    
    if (m_pOverview)
	return TRUE;
    return FALSE;
}

//	
//	HideOverviewWnd
//	
//	
//	Nuke the overview window.
//	
//@doc SECPanView
//@mfunc Hides the overview window.  Overview windows are automatically destroyed.
//@rdesc Returns TRUE on success.
//@xref<mf SECPanView::ShowOverviewWnd>
//@xref<mf SECPanView::IsOverviewShown>
//@xref<c SECPanView>
BOOL SECPanView::HideOverviewWnd()
{
    ASSERT_VALID(this);
    ASSERT(m_pOverview != NULL);
    
    if (m_pOverview){
	m_pOverview->ShowWindow(SW_HIDE);
	m_pOverview->DestroyWindow();
	m_pOverview = NULL;
    }
    return TRUE;
}

//	
//	PreOverview
//	
//	We're about to call preparedc/ondraw
//	with the values necessary to draw the
//	overview window, so save the current state
//	so that our view doesn't get messed up by having
//	a new zoom level.
//
void SECPanView::PreOverview()
{
    ASSERT(m_pOverview != NULL);

    CPoint pt;
    CRect  clientRect;
    
    //Store state
    m_orig_szTotalDev = m_totalDev;
    m_orig_bCenter    = m_bCenter;
    m_orig_zoomMode   = m_zoomMode;
    
    //Setup a special zoom mode for the pan overview window
    //So that the view's OnDraw will be tricked into drawing
    //our overview window for us.
    m_bCenter  = FALSE;
    m_zoomMode = SEC_ZOOMPANOVER;

    //The overview window shows the entire view
    //in it's borders, so setup the device size to
    //the client size.
    m_pOverview->GetClientRect(&clientRect);
    
    ASSERT(clientRect.top == 0 && clientRect.left == 0);
    m_totalDev.cx = clientRect.right;
    m_totalDev.cy = clientRect.bottom;

    // Make sure that the overview looks just like the
    // panview derivative.
    PersistRatio(m_totalLog,m_totalDev,pt);
}

//	
//	PostOverview
//	
//	The overview has been painted, so now revert
//	back to our original state as preserved in 
//	PreOverview() so that when the view draws, it
//      doesn't get messed up by drawing using the
//      stuff we setup for the overview window
//
void SECPanView::PostOverview()
{
    //ReStore state
    m_totalDev   = m_orig_szTotalDev;
    m_bCenter    = m_orig_bCenter;
    m_zoomMode   = m_orig_zoomMode;
}

//	
//	UpdateOverviewRect
//
//      The viewport has changed, so we need to
//      let the overview window know.
//
//      Calculate the currently visible rectangle      
//      in logical coordinates and let it know.
//
void SECPanView::UpdateOverviewRect()
{
    ASSERT_VALID(this);

    //Ok for m_pOverview to be NULL.
    if (m_pOverview == NULL) //protect here, so callers don't have to
	return;
    
    CRect rectClient;
    GetClientRect(&rectClient);
    {
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&rectClient);
    } //BLOCK for DC

    m_pOverview->SetPanRect(rectClient);
}

//	
//	OverviewDestroyed
//	
//	Called from the overview wnd to notify
//	the view that it has been destroyed.
//	
void SECPanView::OverviewDestroyed()
{
    ASSERT_VALID(this);
    if (m_pOverview){
		m_pOverview = NULL;
    }
}

//	
//	UpdateViewport
//	
//	The user has changed our viewport
//	using the overview window dragging rect.
//	Update our viewport to reflect the changes
//	
//      rectNewLog is the new rectangle in log coords.
//      Similar to onscrollby.
//
//
void SECPanView::UpdateViewport(CPoint * lpPtNewLog)
{
    ASSERT_VALID(this);
    ASSERT(lpPtNewLog != NULL);

    PerformPan(lpPtNewLog,TRUE);
}

//	
//	UpdateOverviewWnd
//	
//	Similar behavior of CDoc::UpdateAllViews
//	
//	PanView user must call this after
//	UpdateAllViews with same args
//      to get overview window behavior
//

//@doc SECPanView
//@mfunc Updates the overview window.
//@rdesc void 
//@parm CView * | pSender | View initiating the update.
//@parm LPARAM | lHint | Contains information about the modifications.
//@parm CObject * | pHint | Points to an object containing information about the modifications.
//@xref<mf SECPanView::ShowOverviewWnd>
//@xref<mf SECPanView::HideOverviewWnd>
//@xref<mf SECPanView::OnUpdateOverview>
//@xref<c SECPanView>
//@end
void SECPanView::UpdateOverviewWnd(CView * pSender,
				   LPARAM lHint /* = 0L */,
				   CObject * pHint /* = NULL */)
{
    if (m_pOverview)
	m_pOverview->OnUpdate(pSender,lHint,pHint);
}

//	
//	OnUpdateOverview
//	
//	SECPanView user should override this
//	and treat it just like CView::OnUpdate
//	but be sure to use the CDC * provided
//	and the CWnd * for any invalidates.
//
//      Default implementation invalidates everything,
//      but this will cause flashing unless
//      user overrides this and calls his invalidate
//      logic instead.
//

//@doc SECPanView
//@mfunc When overview window is updated, invalidates all of the window or, using hints, only some regions for repainting.
// Override this function if you want to update only those regions that map to the modified portions of the document. To
// do this you must pass information about the modifications using the hint parameters.
//@rdesc void 
//@parm CView * | pSender | View initiating the update.
//@parm LPARAM | lHint | Contains information about the modifications.
//@parm CObject * | pHint | Points to an object containing information about the modifications.
//@parm CDC * | pOverviewDC | Points to a device context for repainting the overview window.
//@parm  CWnd * | pWndOverview | Points to the overview window to be repainted.
//@xref<mf SECPanView::ShowOverviewWnd>
//@xref<mf SECPanView::HideOverviewWnd>
//@xref<mf SECPanView::UpdateOverviewWnd>
//@xref<c SECPanView>
void SECPanView::OnUpdateOverview(CView * pSender,
			      LPARAM lHint,CObject * pHint,
			      CDC * pOverviewDC, CWnd * pWndOverview)
{
//UNUSED
 pOverviewDC;
 pHint;
 lHint;
 pSender;
    pWndOverview->InvalidateRect(NULL);
}

//
//  SECPanView Message handlers
//


//	
//	OnSetCursor
//	
//	If we're in pan mode set the cursor to a hand, else
//	default to the original cursor.
//	
BOOL SECPanView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    if (nHitTest != HTCLIENT)
	return CScrollView::OnSetCursor(pWnd,nHitTest,message);

    if (m_bPanning){
	if (!m_hOrigCursor)
	    m_hOrigCursor = ::SetCursor(m_hDragCursor);
	else
	    ::SetCursor(m_hDragCursor);
	return TRUE;
    }

    //If we make it here, we're not panning, so set back.
    if (m_hOrigCursor){
	::SetCursor(m_hOrigCursor);
	m_hOrigCursor = 0;
	return TRUE;
    }
    return CScrollView::OnSetCursor(pWnd,nHitTest,message);
    
}

//	
//	OnHScroll
//	
//	User has changed the viewport, update the
//	overview window
//	
void SECPanView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnHScroll(nSBCode,nPos,pScrollBar);
    UpdateOverviewRect();
}
	
//	OnVScroll
//	
//	User has changed the viewport, update the
//	overview window.
//	
void SECPanView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    CScrollView::OnVScroll(nSBCode,nPos,pScrollBar);
    UpdateOverviewRect();
}

//	
//	OnSize handler
//	
//      Let the overview window know the viewport's changed.
//	
void SECPanView::OnSize(UINT nType, int cx, int cy) 
{
    SECZoomView::OnSize(nType, cx, cy);
    
    UpdateOverviewRect();
} 

//	
//	ZoomLevelChanged
//	
//	The user has changed the zoom level
//	update the overview window rectangle
//	because this affects the viewport.
//	
void SECPanView::ZoomLevelChanged(void)
{ 
    UpdateOverviewRect();
}

//	
//	SECPanView::DoScroll()
//	
//	Broke this out of PerformPan to isolate 
//	device/logical coordinate issues.
//	
//	sizeScroll - the amount to scroll in DEVICE coordinates
//	
BOOL SECPanView::DoScroll(CSize sizeScroll, BOOL bDoScroll)
{
    int xOrig, x, xMax;
    int yOrig, y, yMax;

    CScrollBar* pBar;
    DWORD dwStyle = GetStyle();
    //If no scroll bars, don't do anything
    // e.g. the user has a small scale in a zoom, or a large
    //     window and there are no scroll bars.  Everything is
    //     in view, so no panning needed.
    
    pBar = GetScrollBarCtrl(SB_VERT);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
	(pBar == NULL && !(dwStyle & WS_VSCROLL)))
	sizeScroll.cy = 0;
    
    pBar = GetScrollBarCtrl(SB_HORZ);
    if ((pBar != NULL && !pBar->IsWindowEnabled()) ||
	(pBar == NULL && !(dwStyle & WS_HSCROLL)))
	sizeScroll.cx = 0;
    
    // Adjust current x position based on scroll bar constraints.
    xOrig = x = GetScrollPos(SB_HORZ);
    
    // CWnd scrolling is very different in Win32/Win16, call
    // GetScrollRange on Win16 and GetScrollLimit on Win32 to
    // make sure the correct adjusted max is retrieved not something
    // bogus.  We do this for both x and y here.
    
#ifndef WIN32
    int nDummy;
    GetScrollRange(SB_HORZ,&nDummy,&xMax);
#else
    xMax = GetScrollLimit(SB_HORZ);
#endif
    x += sizeScroll.cx;

    if (x < 0)
	x = 0;
    else if (x > xMax)
	x = xMax;

    
    // Adjust current y position based on scroll bar constraints.
    yOrig = y = GetScrollPos(SB_VERT);
#ifndef WIN32
    GetScrollRange(SB_VERT,&nDummy,&yMax);
#else
    yMax = GetScrollLimit(SB_VERT);
#endif
    
    y += sizeScroll.cy;
    if (y < 0)
	y = 0;
    else if (y > yMax)
	y = yMax;
    
    // If nothing changed, just return, no work to do.
    if (x == xOrig && y == yOrig)
	return FALSE;
    
    if (bDoScroll)
    {
	// This order and combination of calls generates the smoothest
	// scrolling.
	// UpdateWindow slows down things, but without it items are
	// 'stretched' around the scrollbar zomes because of multiple
	// ScrollWindow calls - this is especially noticable in realtime
	// panning.
	
	if (x != xOrig)
	    SetScrollPos(SB_HORZ, x);
	if (y != yOrig)
	    SetScrollPos(SB_VERT, y);
	ScrollWindow(-(x-xOrig), -(y-yOrig));
	
	UpdateWindow();
    }
    return TRUE;
}






