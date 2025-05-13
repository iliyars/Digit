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
//	Author:      Dean Hallman
//	Description: Implementation of SEC3DTabWnd
//	Created:     July 20, 1996
//
//	Modified:	 Feb. 1997 (Kim Starks)
//	Description: Added tab disable feature			
//	

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __TABWND3_H__
#include "..\..\Include\WndTabs\tabwnd3.h"
#endif

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SEC3DTabWnd, SECTabWndBase)

#define new DEBUG_NEW

BEGIN_MESSAGE_MAP(SEC3DTabWnd, SECTabWndBase)
	//{{AFX_MSG_MAP(SEC3DTabWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(TCM_TABSEL, OnTabSelect)
	ON_WM_NCCREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// color constants
const COLORREF rgbBlack = RGB(0,0,0);
const COLORREF rgbWhite = RGB(255,255,255);

//Must define these as Macros because if the System colors change they need to update
//dynamically - JR ( 04/11/97 )
#define rgbShadow (::GetSysColor(COLOR_BTNSHADOW))
#define rgbFace (::GetSysColor(COLOR_BTNFACE))


/////////////////////////////////////////////////////////////////////////////
// SEC3DTabWnd construction/destruction

//@doc SEC3DTabWnd
//@mfunc You construct an SEC3DTabWnd object in two steps.  First call the constructor SEC3DTabWnd, then call Create,
// which initializes the Windows window and attaches it to the SEC3DTabWnd object.
//@xref<mf SEC3DTabWnd::Create>
//@xref<c SEC3DTabWnd>
SEC3DTabWnd::SEC3DTabWnd()
{
	//m_cyTabCtrl = 30;
    m_cyTabCtrl = m_cxTabCtrl = 30;
	m_dwTabCtrlStyle = 0;
}

SEC3DTabWnd::~SEC3DTabWnd()
{
	
}

//@doc SEC3DTabWnd
//@mfunc You construct an SEC3DTabWnd object in two steps.  First call the constructor SEC3DTabWnd, then call Create,
// which initializes the Windows window, attaches it to the SEC3DTabWnd object, and creates the fonts used to draw tab
// labels.<nl><nl>
//Apply the following window styles to an SEC3DTabWnd object:<nl><nl>
//WS_CHILD - Always.<nl>
//WS_VISIBLE - Usually.<nl>
//TWS_TABS_ON_BOTTOM - Creates the window with tabs on the bottom.<nl>
//TWS_TABS_ON_TOP - Creates the window with tabs on the top.<nl>
//TWS_TABS_ON_LEFT - Creates the window with tabs on the left.<nl>
//TWS_TABS_ON_RIGHT - Creates the window with tabs on the right.<nl>
//TWS_NOACTIVE_TAB_ENLARGED - When set, the active tab will not be drawn with emphasis.<nl><nl>
//@rdesc Returns TRUE on success; FALSE otherwise.
//@parm CWnd* | pParentWnd | Specifies the tabbed window’s parent window.
//@parm DWORD | dwStyle | Specifies the style of the tabbed window.
//@parm UINT | nID | Specifies the tabbed window’s window ID.
//@xref<mf SEC3DTabWnd::SEC3DTabWnd>
//@xref<c SEC3DTabWnd>
BOOL SEC3DTabWnd::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
	// The 3D tab window doesn't support a "merged" scrollbar
	// like the 2D tab window does.  If you want to merge the
	// scroll bars of the embedded scroll views with the 3D
	// tab window, create a derived class and override this
	// member.
    ASSERT((dwStyle & (WS_HSCROLL | WS_VSCROLL)) == 0);
	ASSERT((dwStyle & TWS_TAB_ORIENTATIONS)||
		   (dwStyle & TWS_TAB_DRAW_STYLES));

    if (dwStyle & TWS_FULLSCROLL)
		m_dwTabCtrlStyle = TCS_FULLSCROLL;
    else if (dwStyle & TWS_LEFTRIGHTSCROLL)
		m_dwTabCtrlStyle = TCS_LEFTRIGHTSCROLL;
	else
		m_dwTabCtrlStyle = 0;

	m_dwTabCtrlStyle |= (dwStyle & TWS_TAB_ORIENTATIONS);
	m_dwTabCtrlStyle |= (dwStyle & TWS_TAB_DRAW_STYLES);

	if(dwStyle & TWS_NOACTIVE_TAB_ENLARGED) 
		m_dwTabCtrlStyle|=TWS_NOACTIVE_TAB_ENLARGED;

    dwStyle &= ~(TWS_FULLSCROLL | TWS_LEFTRIGHTSCROLL | TWS_TAB_ORIENTATIONS | TWS_TAB_DRAW_STYLES);

	return SECTabWndBase::Create(pParentWnd, dwStyle, nID);
}

BOOL SEC3DTabWnd::CreateTabCtrl(DWORD dwStyle, UINT nID)
{
    ASSERT_VALID(this);

	m_pTabCtrl = new SEC3DTabControl();
    ASSERT_VALID(this);
	VERIFY(m_pTabCtrl->Create(WS_VISIBLE | m_dwTabCtrlStyle,
		CRect(0, 0, 0, 0), this, nID));

	dwStyle; // UNUSED
	return (m_pTabCtrl != NULL);
}

BOOL SEC3DTabWnd::OnNcCreate(LPCREATESTRUCT lpcs)
{
	if (!SECTabWndBase::OnNcCreate(lpcs))
		return FALSE;

#ifdef WIN32
	// remove WS_EX_CLIENTEDGE style from parent window
	//  (the tabwnd itself will provide the 3d look)
	CWnd* pParent = GetParent();
	ASSERT_VALID(pParent);
	pParent->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
#endif

	return TRUE;
}

void SEC3DTabWnd::OnSysColorChange()
{
	DWORD dwStyle = m_pTabCtrl->GetStyle();

	m_pTabCtrl->DestroyGDIObjects();
	m_pTabCtrl->Initialize(dwStyle);

	Invalidate();
}


//@doc SEC3DTabWnd
//@mfunc Sets the tab style.
//@rdesc Returns the old(existing) style.
//@parm DWORD | dwTabStyle | The tab style
//@xref<mf SEC3DTabWnd::GetTabStyle>
//@xref<c SEC3DTabWnd>
DWORD SEC3DTabWnd::SetTabStyle(DWORD dwTabStyle)
{
	// return old style
	DWORD dwOldStyle = m_dwTabCtrlStyle;

	// needs to be an orientation style
	ASSERT((dwTabStyle & TWS_TAB_ORIENTATIONS)||
		   (dwTabStyle & TWS_TAB_DRAW_STYLES));

	if (dwTabStyle == m_dwTabCtrlStyle)
		// no change
		return dwOldStyle;

	// turn off old style
	m_dwTabCtrlStyle &= ~dwOldStyle;

	// turn on new style
	m_dwTabCtrlStyle |= dwTabStyle;

	// layout this window
	RecalcLayout();

#ifdef WIN32
	// redo the style of the tab control and recalc the layout
	((SEC3DTabControl*)m_pTabCtrl)->ModifyStyle(
		dwOldStyle & (TWS_TAB_ORIENTATIONS|TWS_TAB_DRAW_STYLES),
		dwTabStyle & (TWS_TAB_ORIENTATIONS|TWS_TAB_DRAW_STYLES));
#endif

	Invalidate();

	return dwOldStyle;
}


//@doc SEC3DTabWnd
//@mfunc Enables/disables tab.
//@syntax EnableTab(Cwnd* pWnd, BOOL bEnable = TRUE);
//@syntax EnableTab(int nIndex, BOOL bEnable = TRUE);
//@rdesc void 
//@parm CWnd* | pWnd | Points to a child window.
//@parm int | nIndex | Index of tab.
//@parm BOOL | bEnable | Flag indicating whether tab enabled or disabled; default is enabled.
//@xref<mf SEC3DTabWnd::IsTabEnabled>
//@xref<c SEC3DTabWnd>
void SEC3DTabWnd::EnableTab(CWnd* pWnd, BOOL bEnable)
{
    int nIndex;

    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));
    		
    VERIFY(FindTab(pWnd, nIndex));
    EnableTab(nIndex, bEnable);
}

void SEC3DTabWnd::EnableTab(int nIndex, BOOL bEnable)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

    ((SEC3DTabControl*)m_pTabCtrl)->EnableTab(nIndex, bEnable);
}


//@doc SEC3DTabWnd
//@mfunc Determines if tab is enabled/disabled.
//@syntax  IsTabEnabled(Cwnd* pWnd);
//@syntax  IsTabEnabled(int nIndex);
//@rdesc Nonzero if tab enabled; otherwise 0.
//@parm CWnd* | pWnd | Points to a child window.
//@parm int | nIndex | Index of tab.
//@xref<mf SEC3DTabWnd::EnableTab>
//@xref<c SEC3DTabWnd>
BOOL SEC3DTabWnd::IsTabEnabled(CWnd* pWnd)
{
    int nIndex;
    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));    		
    VERIFY(FindTab(pWnd, nIndex));
    return this->IsTabEnabled(nIndex);
}

BOOL SEC3DTabWnd::IsTabEnabled(int nIndex)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));
    return ((SEC3DTabControl*)m_pTabCtrl)->IsTabEnabled(nIndex);
}


//@doc SEC3DTabWnd
//@mfunc Sets an active tab's current font to the specified font.  If bRedraw is TRUE, the tab will also be redrawn.<nl>
//The CFont object passed by pointer will be copied to duplicate storage, so the caller need not hang onto the pointer.
//@rdesc Returns TRUE on success; FALSE otherwise.
//@parm CFont* | pFont | Specifies the new font.
//@parm BOOL | bRedraw | If TRUE, redraw the SEC3DTabWnd object.
//@xref<mf SEC3DTabWnd::GetFontActiveTab>
//@xref<c SEC3DTabWnd>
BOOL SEC3DTabWnd::SetFontActiveTab(CFont* pFont,BOOL bRedraw) {
    ASSERT_VALID(this);
    BOOL bRet=((SEC3DTabControl*)m_pTabCtrl)->SetFontActiveTab(pFont,bRedraw);
	if((bRedraw)&&(bRet)) Invalidate();
	return bRet;
}


//@doc SEC3DTabWnd
//@mfunc Sets an inactive tab's current font to the specified font.  If bRedraw is TRUE, the tab will also be redrawn.<nl>
//The CFont object passed by pointer will be copied to duplicate storage, so the caller need not hang onto the pointer.
//@rdesc Returns TRUE on success; FALSE otherwise.
//@parm CFont* | pFont | Specifies the new font.
//@parm BOOL | bRedraw | If TRUE, redraw the SEC3DTabWnd object.
//@xref<mf SEC3DTabWnd::GetFontInactiveTab>
//@xref<c SEC3DTabWnd>
BOOL SEC3DTabWnd::SetFontInactiveTab(CFont* pFont,BOOL bRedraw) {
    ASSERT_VALID(this);
    BOOL bRet=((SEC3DTabControl*)m_pTabCtrl)->SetFontInactiveTab(pFont,bRedraw);
	if((bRedraw)&&(bRet)) Invalidate();
	return bRet;
}


//@doc SEC3DTabWnd
//@mfunc Gets the current font of an active tab.  The CFont pointer passed back is temporary and should not be stored for
// later use.
//@rdesc A pointer to a CFont that contains the current font.
//@xref<mf SEC3DTabWnd::SetFontActiveTab>
//@xref<c SEC3DTabWnd>
CFont* SEC3DTabWnd::GetFontActiveTab() {
    ASSERT_VALID(this);
    return ((SEC3DTabControl*)m_pTabCtrl)->GetFontActiveTab();
}

//@doc SEC3DTabWnd
//@mfunc Gets the current font of an inactive tab.  The CFont pointer passed back is temporary and should not be stored for
// later use.
//@rdesc A pointer to a CFont that contains the current font.
//@xref<mf SEC3DTabWnd::SetFontInactiveTab>
//@xref<c SEC3DTabWnd>
CFont* SEC3DTabWnd::GetFontInactiveTab() {
    ASSERT_VALID(this);
    return ((SEC3DTabControl*)m_pTabCtrl)->GetFontInactiveTab();
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabWnd drawing and layout

// like GetClientRect but inset by tab control
void SEC3DTabWnd::GetInsideRect(CRect& rect) const
{
    ASSERT_VALID(this);
    
    GetClientRect(rect);
    ASSERT(rect.left == 0 && rect.top == 0);

	// subtract tabctrl clearance

	if (m_dwTabCtrlStyle & TCS_TABS_ON_BOTTOM)
	{
		rect.top    += 4;
		rect.bottom -= (m_cyTabCtrl+1);
		rect.left   += 6;
		rect.right  -= 7;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_TOP)
	{
		rect.top    += (m_cyTabCtrl+1);
		rect.bottom -= 4;
		rect.left   += 6;
		rect.right  -= 7;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_LEFT)
	{
		rect.top    += 4;
		rect.bottom -= 6;
		rect.left   += (m_cxTabCtrl+1);
		rect.right  -= 7;
	}
	else // if (m_dwTabCtrlStyle & TCS_TABS_ON_RIGHT)
	{
		rect.top    += 4;
		rect.bottom -= 6;
		rect.left   += 6;
		rect.right  -= (m_cxTabCtrl+1);
	}
}

// repositions client area of specified window
// assumes everything has WS_BORDER or is inset like it does
//  (includes scroll bars)
static HDWP NEAR PASCAL DeferClientPos(HDWP hDWP, CWnd* pWnd,
				       int x, int y, int cx, int cy,
				       BOOL bScrollBar, UINT dwExtraFlags)
{
    ASSERT(pWnd != NULL);
    ASSERT(pWnd->m_hWnd != NULL);
    
    // first check if the new rectangle is the same as the current
    CRect rectOld;
    pWnd->GetWindowRect(rectOld);
    pWnd->GetParent()->ScreenToClient(&rectOld);
    if (rectOld.left == x && rectOld.top == y &&
		rectOld.Width() == cx && rectOld.Height() == cy)
	{
		return hDWP;        // nothing to do
	}
	
	bScrollBar; // UNUSED
	
    return ::DeferWindowPos(hDWP, pWnd->m_hWnd, NULL,
		x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | dwExtraFlags);
}

void SEC3DTabWnd::RecalcLayout()
{
    ASSERT_VALID(this);
    
    CRect rectInside;
    CRect rectClient;
    CPoint ptTabCtrl;
	CSize sizeTabCtrl;

	if (m_dwTabCtrlStyle & TCS_TABS_ON_BOTTOM)
	{
		m_cyTabCtrl = 30;
		m_cxTabCtrl = 0;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_TOP)
	{
		m_cyTabCtrl = 30;
		m_cxTabCtrl = 0;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_LEFT)
	{
		m_cyTabCtrl = 0;
		m_cxTabCtrl = 30;
	}
	else // if (m_dwTabCtrlStyle & TCS_TABS_ON_RIGHT)
	{
		m_cyTabCtrl = 0;
		m_cxTabCtrl = 30;
	}

    GetInsideRect(rectInside);
    GetClientRect(rectClient);

	// figure the tab control rectangle
	if (m_dwTabCtrlStyle & TCS_TABS_ON_BOTTOM)
	{
		ptTabCtrl.x    = rectInside.left;
		ptTabCtrl.y    = rectInside.bottom + 1;
		sizeTabCtrl.cx = rectInside.Width();
		sizeTabCtrl.cy = rectClient.bottom - rectInside.bottom;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_TOP)
	{
		ptTabCtrl.x    = rectInside.left;
		ptTabCtrl.y    = rectClient.top;
		sizeTabCtrl.cx = rectInside.Width();
		sizeTabCtrl.cy = rectInside.top - rectClient.top;
	}
	else if (m_dwTabCtrlStyle & TCS_TABS_ON_LEFT)
	{
		ptTabCtrl.x    = rectClient.left;
		ptTabCtrl.y    = rectInside.top;
		sizeTabCtrl.cx = rectInside.left - rectClient.left;
		sizeTabCtrl.cy = rectInside.Height();
	}
	else // (m_dwTabCtrlStyle & TCS_TABS_ON_RIGHT)
	{
		sizeTabCtrl.cx = rectClient.right - rectInside.right;
		sizeTabCtrl.cy = rectInside.Height();
		ptTabCtrl.x    = rectClient.right - sizeTabCtrl.cx;
		ptTabCtrl.y    = rectClient.top;
	}

    HDWP hDWP = ::BeginDeferWindowPos(4);

    // Resize tab control
    // Note, the tab control draws it's own border, so
    // no need to subtract border pixels
    CWnd* pTabCtrl = GetDlgItem(SEC_IDW_TABCTRL);     
    ASSERT(pTabCtrl != NULL);
    hDWP = DeferClientPos(hDWP, pTabCtrl,
		ptTabCtrl.x, ptTabCtrl.y, sizeTabCtrl.cx, sizeTabCtrl.cy,
		FALSE, SWP_NOREDRAW);

	if (m_pActiveWnd)
		hDWP = DeferClientPos(hDWP, m_pActiveWnd, 
		rectInside.left, rectInside.top,
		rectInside.Width()-1,
		rectInside.Height(), FALSE, 0);
    
    ::EndDeferWindowPos(hDWP);
}

void SEC3DTabWnd::OnSize(UINT nType, int cx, int cy) 
{
    ASSERT_VALID(this);
    
    if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
	{
	    RecalcLayout();

	    // We must completely redraw the tab control after a resize
	    CWnd* pTabCtrl = GetDlgItem(SEC_IDW_TABCTRL);
	    ASSERT(pTabCtrl != NULL);
	    pTabCtrl->Invalidate(TRUE);
		Invalidate(FALSE);
	}
    
	SECTabWndBase::OnSize(nType, cx, cy);
}

// Private drawing helpers for drawing 3D lines and fills.
// Note, draw Vert lines before Horz lines.
// This is necessary so the joining of a horz
// and vert line is drawn properly.

void SEC3DTabWnd::VLine3D(CDC* pDC, int x, int y, int nHeight)
{
	rgbFill(pDC, x,   y, 1, nHeight, rgbWhite);
	rgbFill(pDC, x+1, y, 1, nHeight, rgbFace);
	rgbFill(pDC, x+2, y, 1, nHeight, rgbShadow);
	rgbFill(pDC, x+3, y, 1, nHeight, rgbBlack);
}

void SEC3DTabWnd::HLine3D(CDC* pDC, int x, int y, int nWidth, BOOL bFlip)
{
	if (bFlip)
	{
		rgbFill(pDC, x+2, y,   nWidth-5, 1, rgbWhite);
		rgbFill(pDC, x+1, y+1, nWidth-3, 1, rgbFace);
		rgbFill(pDC, x+1, y+2, nWidth-2, 1, rgbShadow);
		rgbFill(pDC, x,   y+3, nWidth,   1, rgbBlack);
	}
	else
	{
		rgbFill(pDC, x,   y,   nWidth-1, 1, rgbWhite);
		rgbFill(pDC, x+1, y+1, nWidth-3, 1, rgbFace);
		rgbFill(pDC, x+2, y+2, nWidth-6, 1, rgbShadow);
		rgbFill(pDC, x+3, y+3, nWidth-8, 1, rgbBlack);
	}
}

void SEC3DTabWnd::rgbFill(CDC* pDC,int x,int y,int cx,int cy, COLORREF rgb)
{
    RECT    rc;
    
    rc.left   = x;
    rc.top    = y;
    rc.right  = x + cx;
    rc.bottom = (y + cy);
    
    COLORREF rgbOld = pDC->GetBkColor();
    pDC->SetBkColor(rgb);
    pDC->ExtTextOut(0,0,ETO_OPAQUE,&rc,NULL,0,NULL);
    pDC->SetBkColor(rgbOld);
}

void SEC3DTabWnd::OnPaint() 
{
    ASSERT_VALID(this);
    CPaintDC dc(this); // device context for painting

	CRect rectInside;
	GetInsideRect(rectInside);

	CRect rectInvalid;
    dc.GetClipBox(&rectInvalid);
    MapWindowPoints(m_pTabCtrl, rectInvalid);
	m_pTabCtrl->RedrawWindow(rectInvalid, NULL, RDW_INVALIDATE);

	// Draw 3D borders
	VLine3D(&dc, rectInside.left-4, rectInside.top-4, rectInside.Height()+8);
	VLine3D(&dc, rectInside.right, rectInside.top-4, rectInside.Height()+8);
	HLine3D(&dc, rectInside.left-4, rectInside.top-4, rectInside.Width()+8);
	HLine3D(&dc, rectInside.left-4, rectInside.bottom+1, rectInside.Width()+8, TRUE);
    // Do not call SECTabWndBase::OnPaint() for painting messages
}

BOOL SEC3DTabWnd::OnEraseBkgnd(CDC* pDC) 
{
    ASSERT_VALID(this);
    ASSERT_VALID(pDC);

	CRect rectInside, rectClient;
	GetInsideRect(rectInside);
	GetClientRect(rectClient);

	// exclude the tab control from the paintable area
	CRect rectTab;
	m_pTabCtrl->GetWindowRect(rectTab);
	ScreenToClient(rectTab);
	pDC->ExcludeClipRect(rectTab);

	// fill the outer edges NOT covered by the tab control or the client
	// windows with the background color (rgbFace)
	// this makes the repainting (and indirectly the resizing) of windows
	// appear much cleaner

	// fill the outer edges excluding the borders
	rgbFill(pDC, rectClient.left, rectClient.top, rectClient.Width(),rectInside.top - rectClient.top, rgbFace);
	rgbFill(pDC, rectClient.left, rectClient.top, rectInside.left - rectClient.left, rectClient.Height(), rgbFace);
	rgbFill(pDC, rectInside.right-1, rectClient.top, rectClient.right - rectInside.right + 1, rectClient.Height(), rgbFace);
	rgbFill(pDC, rectClient.left, rectInside.bottom, rectClient.Width(), rectClient.bottom - rectInside.bottom, rgbFace);

	// fill the inside area excluding the borders
	rgbFill(pDC, rectInside.left-1, rectInside.bottom, rectInside.Width()+1, 1, rgbFace);

    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabWnd message handlers

LRESULT SEC3DTabWnd::OnTabSelect(WPARAM wParam, LPARAM /*lParam*/)
{
    ASSERT_VALID(m_pTabCtrl);

    LPCTSTR lpszLabel;
    BOOL bSelected;
    CObject* pClient;
    HMENU hMenu;
    void* pExtra;
    
    m_pTabCtrl->GetTabInfo((int)wParam, lpszLabel, bSelected,
    		pClient, hMenu, pExtra);
    ASSERT_VALID((CWnd*)pClient);
    ActivateTab((CWnd*)pClient, (int)wParam);

	CRect rectInside;
	GetClientRect(rectInside);
	InvalidateRect(rectInside, TRUE);

	ASSERT_VALID(m_pNotifyWnd);
	m_pNotifyWnd->SendMessage( TCM_TABSEL, wParam, 0 );

    return 0;
}

