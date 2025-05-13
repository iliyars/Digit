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
//  Author:			Dean Hallman
//  Description:	Implementation of SECTabWnd
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __TABWND_H__
#include "..\..\Include\WndTabs\tabwnd.h"
#endif

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(SECTabWnd, SECTabWndBase)
IMPLEMENT_DYNCREATE(SEC2DTabWnd, SECTabWnd)

#define new DEBUG_NEW

#define CX_BORDER	1
#define CY_BORDER	1

BEGIN_MESSAGE_MAP(SECTabWnd, SECTabWndBase)
	//{{AFX_MSG_MAP(SECTabWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(TCM_TABSEL, OnTabSelect)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_NCCREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Constants
const int nMinHScrollWidth = 60;

/////////////////////////////////////////////////////////////////////////////
// SECTabWnd construction/destruction

//@doc SECTabWnd
//@mfunc You construct an SECTabWnd object in two steps.  First call the constructor SECTabWnd, then call Create,
// which initializes the Windows window and attaches it to the SECTabWnd object.
//@xref<mf SECTabWnd::Create>
//@xref<c SECTabWnd>
SECTabWnd::SECTabWnd()
{
    m_cxVScroll = 0;
    m_cyHScroll = 0;
    m_cxSplitter = 5;
    m_bTracking = FALSE;
    m_hcurSplit = NULL;
    m_hcurArrow = NULL;
    m_bSplitterHit = FALSE;
    m_nTrackOffset = 0;
    m_bFirstSize = TRUE;
}

SEC2DTabWnd::SEC2DTabWnd()
{
}

SECTabWnd::~SECTabWnd()
{
#ifdef WIN32
	// Delete scroll info structs
	SCROLLINFO* pSBI;
	int i;
	for (i=m_arrHScrollInfo.GetSize(); i>0; i--) {
		pSBI = (SCROLLINFO*)m_arrHScrollInfo[i-1];
		ASSERT(pSBI != NULL);
		delete pSBI;
	}
	m_arrHScrollInfo.RemoveAll();
	for (i=m_arrVScrollInfo.GetSize(); i>0; i--) {
		pSBI = (SCROLLINFO*)m_arrVScrollInfo[i-1];
		ASSERT(pSBI != NULL);
		delete pSBI;
	}
	m_arrVScrollInfo.RemoveAll();
#endif
}

SEC2DTabWnd::~SEC2DTabWnd()
{
}


//@doc SECTabWnd
//@mfunc You construct an SECTabWnd object in two steps. First call the constructor SECTabWnd, then call Create, which 
// initializes the Windows window, attaches it to the SECTabWnd object, and creates the fonts used to draw tab labels.
//<nl><nl>Apply the following window styles to an SECTabWnd object:<nl><nl>
//WS_CHILD - Always <nl>
//WS_VISIBLE - Usually<nl>
//TWS_LEFTRIGHTSCROLL - The tabbed window is created with two buttons used to scroll the tabs right and left.<nl>
//TWS_FULLSCROLL - The tabbed window will have full scrolling support.  This is a superset of the TWS_LEFTRIGHTSCROLL
// style, adding two additional buttons.  The additional buttons cause the tabbed window to jump to the first or last 
// tab when pressed.<nl>
//TWS_TBS_TABS_ON_BOTTOM - The tabbed window is created with tabs on the bottom.<nl>
//If neither TWS_LEFTRIGHTSCROLL nor TWS_FULLSCROLL style is specified, no buttons will be displayed for scrolling the 
// tabs left and right.  Other TWS_ window style entries exist; see tabwnd.h.
//@rdesc Returns TRUE on success; FALSE otherwise.
//@parm CWnd* | pParentWnd | Specifies the tabbed window’s parent window.
//@parm  DWORD | dwStyle | Specifies the style of the tabbed window.
//@parm  UINT | nID | Specifies the tabbed window’s window ID.
//@xref<mf SECTabWnd::Create>
//@xref<c SECTabWnd>
BOOL SECTabWnd::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
    ASSERT(pParentWnd != NULL);
    ASSERT(nID != 0);
    ASSERT(dwStyle & WS_CHILD);

    if (dwStyle & TWS_FULLSCROLL)
		m_dwTabCtrlStyle = TCS_FULLSCROLL;
    else if (dwStyle & TWS_LEFTRIGHTSCROLL)
		m_dwTabCtrlStyle = TCS_LEFTRIGHTSCROLL;
	else
		m_dwTabCtrlStyle = 0;

    dwStyle &= ~(TWS_FULLSCROLL | TWS_LEFTRIGHTSCROLL);

	SECTabWndBase::Create(pParentWnd, dwStyle, nID);
		
#if _MFC_VER >= 0x0300
		HINSTANCE hInst = AfxFindResourceHandle(
		MAKEINTRESOURCE(AFX_IDC_HSPLITBAR), RT_GROUP_CURSOR);
#else
    HINSTANCE hInst = AfxGetInstanceHandle();
#endif
	
    m_hcurSplit = ::LoadCursor(hInst, MAKEINTRESOURCE(AFX_IDC_HSPLITBAR));
    m_hcurArrow  = ::LoadCursor(NULL, IDC_ARROW);

	//If you hit this ASSERT, comment out the 
	//#define _AFX_NO_SPLITTER_RESOURCES directive in
	//your resource includes. ASSERT(m_hcurSplit != NULL)
    ASSERT(m_hcurSplit != NULL);

	// Create the scroll bars
	if (!CreateScrollBarCtrl(SBS_HORZ, SEC_IDW_HORZ_SCROLLBAR))
		AfxThrowResourceException();
	if (!CreateScrollBarCtrl(SBS_VERT, SEC_IDW_VERT_SCROLLBAR))
		AfxThrowResourceException();
	
	// Initialize dimensions
	m_cxVScroll = ::GetSystemMetrics(SM_CXVSCROLL);
	m_cyHScroll = ::GetSystemMetrics(SM_CYHSCROLL);
	m_cyTabCtrl = m_cyHScroll;

    return TRUE;
}

//@doc SECTabWnd
//@mfunc Overridable member function to create tab control in a specific style.
//@rdesc Nonzero if tab control successfully created; otherwise 0.
//@parm DWORD | dwStyle | Specifies style attributes.
//@parm  UINT | nID | Tab control’s resource identifier.
//@xref<c SECTabWnd>
BOOL SECTabWnd::CreateTabCtrl(DWORD dwStyle, UINT nID)
{
    ASSERT_VALID(this);

	m_pTabCtrl = new SECTabControl();
    ASSERT_VALID(this);
	VERIFY(m_pTabCtrl->Create(WS_VISIBLE | m_dwTabCtrlStyle,
		CRect(0, 0, 0, 0), this, nID));

	dwStyle; // UNUSED
	return (m_pTabCtrl != NULL);
}

BOOL SECTabWnd::OnNcCreate(LPCREATESTRUCT lpcs)
{
	if (!SECTabWndBase::OnNcCreate(lpcs))
		return FALSE;

#ifdef WIN32
	// remove WS_EX_CLIENTEDGE style from parent window
	//  (the tabwnd itself will provide the 3d look)
	CWnd* pParent = GetParent();
	ASSERT_VALID(pParent);
	pParent->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
#endif

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase operations

//
// Since each tab will contain a different window
// and each window may be at a different scroll
// position, we must maintain separate scroll bars
// for each tab.  Therefore, the pWnd parameter
// is now necessary.
// 
//@doc SECTabWnd
//@mfunc Returns a pointer to the CScrollBar object used by a particular tab.
//@rdesc A pointer to the CScrollBar object.
//@parm int | nBar | Specifies the type of scroll bar.
//@parm  const CWnd* const | pWnd | CWnd pointer of tab for which to retrieve the scrollbar.
//@xref<c SECTabWnd>
CScrollBar* SECTabWnd::GetScrollBar(int nBar, const CWnd* const pWnd) const
{
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

	CScrollBar *pScrollBar = NULL;
	int nTab;
	CWnd* pWndTmp = (CWnd*)pWnd;

	if (pWnd == NULL)
		pWndTmp = m_pActiveWnd;

	if (pWndTmp != NULL && FindTab(pWndTmp, nTab)) {
		ASSERT(nTab < m_arrScrollStyles.GetSize());
		DWORD dwScrollStyle = m_arrScrollStyles[nTab];
		if (nBar == SB_HORZ && (dwScrollStyle & WS_HSCROLL) != 0)
			pScrollBar = (CScrollBar*)GetDlgItem(SEC_IDW_HORZ_SCROLLBAR);
		else if (nBar == SB_VERT && (dwScrollStyle & WS_VSCROLL) != 0)
			pScrollBar = (CScrollBar*)GetDlgItem(SEC_IDW_VERT_SCROLLBAR);
	}
	return pScrollBar;
}

//@doc SECTabWnd
//@mfunc When the tab window contains a CScrollView derivative, the view can be scrolled through
// the scroll bars in the tab window. Call this function to set the scrolling style for the tab
// at index nTab.  Available styles:<nl><nl>
//	WS_HSCROLL<nl>
//	WS_VSCROLL<nl>
//@rdesc void 
//@parm int | nIndex | Index of tab.
//@parm  DWORD | dwStyle | Specifies style attributes.
//@xref<c SECTabWnd>
void SECTabWnd::SetScrollStyle(int nIndex, DWORD dwStyle)
{
	// Save scroll bar style and thumb position info
	if (nIndex < m_arrScrollStyles.GetSize())
		m_arrScrollStyles.SetAt(nIndex, dwStyle);
	else {
		m_arrScrollStyles.Add(dwStyle);
#ifdef WIN32
		SCROLLINFO* pSBI;
		pSBI = new SCROLLINFO;
		memset(pSBI, 0, sizeof(SCROLLINFO));
		pSBI->cbSize = sizeof(SCROLLINFO);
		pSBI->fMask = SIF_ALL;
		m_arrHScrollInfo.Add(pSBI);
		pSBI = new SCROLLINFO;
		memset(pSBI, 0, sizeof(SCROLLINFO));
		pSBI->cbSize = sizeof(SCROLLINFO);
		pSBI->fMask = SIF_ALL;
		m_arrVScrollInfo.Add(pSBI);
#endif // WIN32
	}
}

//@doc SECTabWndBase
//@mfunc Inserts a new tab into the tabbed window.
//@syntax InsertTab(CWnd* pWnd, int nIndex, LPCTSTR lpszLabel);
//@syntax InsertTab(CRuntimeClass* pViewClass, int nIndex, LPCTSTR lpszLabel, CCreateContext* pContext = NULL, UINT nID = -1);
//@rdesc Pointer to an SECTab object representing the new tab.
//@parm CWnd* | pWnd | Pointer to window to be inserted.
//@parm int | nIndex | Position to insert into.
//@parm LPCTSTR | lpszLabel | Tab label for new tab.
//@parm CRuntimeClass* | pViewClass | Runtime class of new tab to create.
//@parm CCreateContext* | pContext | CCreateContext in which to create new instance of pViewClass.
//@parm UINT | nID | ID of child view created by InsertTab (i.e., ID of the new tab).
//@xref<c SECTabWnd>
SECTab* SECTabWnd::InsertTab(CWnd* pWnd,
							 int nIndex,
							 LPCTSTR lpszLabel)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	m_arrScrollStyles.InsertAt(nIndex, (DWORD)0);
#ifdef WIN32
	SCROLLINFO* pSBI;
	pSBI = new SCROLLINFO;
	memset(pSBI, 0, sizeof(SCROLLINFO));
	m_arrHScrollInfo.Add(pSBI);
	pSBI = new SCROLLINFO;
	memset(pSBI, 0, sizeof(SCROLLINFO));
	m_arrVScrollInfo.Add(pSBI);
#endif // WIN32

	return SECTabWndBase::InsertTab(pWnd, nIndex, lpszLabel);
}


//@doc SECTabWnd
//@mfunc Removes a tab.
//@rdesc void 
//@parm int | nIndex | Index of tab to remove.
//@xref<c SECTabWnd>
void SECTabWnd::RemoveTab(int nIndex)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	// Delete the scroll bar info
#ifdef WIN32
	SCROLLINFO* pSBI;
	ASSERT(nIndex < m_arrHScrollInfo.GetSize());
	ASSERT(nIndex < m_arrHScrollInfo.GetSize());
	pSBI = (SCROLLINFO*)m_arrHScrollInfo.GetAt(nIndex);
	m_arrHScrollInfo.RemoveAt(nIndex);
	ASSERT(pSBI != NULL);
	delete pSBI;
	pSBI = (SCROLLINFO*)m_arrVScrollInfo.GetAt(nIndex);
	m_arrVScrollInfo.RemoveAt(nIndex);
	ASSERT(pSBI != NULL);
	delete pSBI;
	m_arrScrollStyles.RemoveAt(nIndex);
#endif // WIN32

	// Delete the tab from the tab control
	SECTabWndBase::RemoveTab(nIndex);
}


//@doc SECTabWnd
//@mfunc Activates a specific tab.
//@syntax ActivateTab(CWnd* pWnd, int nIndex);
//@syntax ActivateTab(CWnd* pWnd);
//@syntax ActivateTab(int nIndex);
//@rdesc void 
//@parm CWnd* | pWnd | CWnd pointer to tab to activate.
//@parm  int | nIndex | Index of tab to activate.
//@xref<c SECTabWnd>
void SECTabWnd::ActivateTab(CWnd* pWnd, int nIndex)
{
    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));


	if (m_pActiveWnd != pWnd) {

		CScrollBar* pSB;
		
#ifdef WIN32
		SCROLLINFO* pSBI;

		// Save away scroll bar info so thumb size and position
		// can be restored upon a future activation of this tab.
		if (m_pActiveWnd != NULL) {
			int nTab;
			pSB = GetScrollBar(SB_HORZ);
			VERIFY(FindTab(m_pActiveWnd, nTab));
			if (pSB) {
				pSBI = (SCROLLINFO*)m_arrHScrollInfo[nTab];
				pSB->GetScrollInfo(pSBI, SIF_ALL);
			}
			pSB = GetScrollBar(SB_VERT);
			if (pSB) {
				pSBI = (SCROLLINFO*)m_arrVScrollInfo[nTab];
				pSB->GetScrollInfo(pSBI, SIF_ALL);
			}
		}
#endif // WIN32
		
		// Restore scroll bar info to previous values
		if (pWnd != NULL) {
			pSB = GetScrollBar(SB_HORZ, pWnd);
			if (pSB) {
#ifdef WIN32
				pSBI = (SCROLLINFO*)m_arrHScrollInfo[nIndex];
				pSB->SetScrollInfo(pSBI, TRUE);
#endif
				pSB->ShowWindow(SW_SHOW);
			}
			else {
				pSB = (CScrollBar*)GetDlgItem(SEC_IDW_HORZ_SCROLLBAR);
				ASSERT(pSB != NULL);
				pSB->ShowWindow(SW_HIDE);
			}
			pSB = GetScrollBar(SB_VERT, pWnd);
			if (pSB) {
#ifdef WIN32
				pSBI = (SCROLLINFO*)m_arrVScrollInfo[nIndex];
				pSB->SetScrollInfo(pSBI, TRUE);
#endif
				pSB->ShowWindow(SW_SHOW);
			}
			else {
				pSB = (CScrollBar*)GetDlgItem(SEC_IDW_VERT_SCROLLBAR);
				ASSERT(pSB != NULL);
				pSB->ShowWindow(SW_HIDE);
			}
		}
	}

	SECTabWndBase::ActivateTab(pWnd, nIndex);
}

// Font manipulation
//@doc SECTabWnd
//@mfunc Sets a selected tab’s current font to the specified font.  If bRedraw is TRUE, the tab will also be redrawn.<nl>
//The CFont object passed by pointer will be copied to duplicate storage, so the caller need not hang onto the pointer.
//@parm CFont* | pFont | Specifies the new font.
//@parm BOOL | bRedraw | If TRUE, redraw the SECTabWnd object.
//@xref<mf SECTabWnd::GetFontSelectedTab>
//@xref<c SECTabWnd>
BOOL SECTabWnd::SetFontSelectedTab(CFont* pFont,BOOL bRedraw) {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->SetFontSelectedTab(pFont,bRedraw);
}

//@doc SECTabWnd
//@mfunc Sets an unselected tab’s current font to the specified font.  If bRedraw is TRUE, the tab will also be redrawn.<nl>
//The CFont object passed by pointer will be copied to duplicate storage, so the caller need not hang onto the pointer.
//@parm CFont* | pFont | Specifies the new font.
//@parm BOOL | bRedraw | If TRUE, redraw the SECTabWnd object.
//@xref<mf SECTabWnd::GetFontUnSelectedTab>
//@xref<c SECTabWnd>
BOOL SECTabWnd::SetFontUnselectedTab(CFont* pFont,BOOL bRedraw) {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->SetFontUnselectedTab(pFont,bRedraw);
}

//@doc SECTabWnd
//@mfunc Sets an active tab’s current font to the specified font.  If bRedraw is TRUE, the tab will also be redrawn.<nl>
//The CFont object passed by pointer will be copied to duplicate storage, so the caller need not hang onto the pointer.
//@parm CFont* | pFont | Specifies the new font.
//@parm BOOL | bRedraw | If TRUE, redraw the SECTabWnd object.
//@xref<mf SECTabWnd::GetFontActiveTab>
//@xref<c SECTabWnd>
BOOL SECTabWnd::SetFontActiveTab(CFont* pFont,BOOL bRedraw) {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->SetFontActiveTab(pFont,bRedraw);	
}

//@doc SECTabWnd
//@mfunc Gets the current font of a selected tab.  The CFont pointer passed back is temporary and should not be stored for
// later use.
//@rdesc A pointer to a CFont that contains the current font.
//@xref<mf SECTabWnd::SetFontSelectedTab>
//@xref<c SECTabWnd>
CFont* SECTabWnd::GetFontSelectedTab() {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->GetFontSelectedTab();
}

//@doc SECTabWnd
//@mfunc Gets the current font of an unselected tab.  The CFont pointer passed back is temporary and should not be stored for
// later use.
//@rdesc A pointer to a CFont that contains the current font.
//@xref<mf SECTabWnd::SetFontUnselectedTab>
//@xref<c SECTabWnd>
CFont* SECTabWnd::GetFontUnselectedTab() {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->GetFontUnselectedTab();
}

//@doc SECTabWnd
//@mfunc Gets the current font of an active tab.  The CFont pointer passed back is temporary and should not be stored for
// later use.
//@rdesc A pointer to a CFont that contains the current font.
//@xref<mf SECTabWnd::SetFontActiveTab>
//@xref<c SECTabWnd>
CFont* SECTabWnd::GetFontActiveTab() {
    ASSERT_VALID(m_pTabCtrl);
	return ((SECTabControl *)m_pTabCtrl)->GetFontActiveTab();
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWnd drawing and layout

// like GetClientRect but inset by shared scrollbars
void SECTabWnd::GetInsideRect(CRect& rect) const
{
    ASSERT_VALID(this);
    
    GetClientRect(rect);
    ASSERT(rect.left == 0 && rect.top == 0);
    
    // subtract tabctrl/scrollbar clearance
    rect.bottom -= m_cyTabCtrl;

    // note:  vertical's width is same as beam's height
	if (GetScrollBar(SB_VERT, m_pActiveWnd))
		rect.right -= m_cxVScroll;
}

void SECTabWnd::GetSplitterRect(CRect& rectSplit)
{
    CRect rectInside;
    GetInsideRect(rectInside);
    
    rectSplit.SetRect(rectInside.left + m_cxTabCtrl,
		rectInside.bottom,
		rectInside.left + m_cxTabCtrl+m_cxSplitter,
		rectInside.bottom + m_cyTabCtrl);
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

    if (bScrollBar)
	{
	    DWORD dwStyle = pWnd->GetStyle();                            
	    
	    // if there is enough room, draw scroll bar without border
	    // if there is not enough room, set the WS_BORDER bit so that
	    //   we will at least get a proper border drawn
	    BOOL bNeedBorder = (cx <= CX_BORDER || cy <= CY_BORDER);
	    if (bNeedBorder)
		dwStyle |= WS_BORDER;
	    else
		dwStyle &= ~WS_BORDER;
	    ::SetWindowLong(pWnd->m_hWnd, GWL_STYLE, dwStyle);
	}
    
    // first check if the new rectangle is the same as the current
    CRect rectOld;
    pWnd->GetWindowRect(rectOld);
    pWnd->GetParent()->ScreenToClient(&rectOld);
    if (rectOld.left == x && rectOld.top == y &&
	rectOld.Width() == cx && rectOld.Height() == cy)
	{
	    return hDWP;        // nothing to do
	}
    return ::DeferWindowPos(hDWP, pWnd->m_hWnd, NULL,
			    x, y, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE
				| SWP_NOCOPYBITS | dwExtraFlags);
}

void SECTabWnd::RecalcLayout()
{
    ASSERT_VALID(this);
    
    CRect rectInside;
    CRect rectClient;
	int cxTabCtrl;

    GetInsideRect(rectInside);
    GetClientRect(rectClient);

    HDWP hDWP = ::BeginDeferWindowPos(2 + 2*(m_bTracking==FALSE));
    
    // Resize horizontal scroll bar
	CWnd* pScrollBar = GetScrollBar(SB_HORZ);
	if (pScrollBar) {
	    CRect rectSplit;
		// Comput tab control width
    	m_cxTabCtrl = max(m_cxTabCtrl, 0);
    	m_cxTabCtrl = min(m_cxTabCtrl, rectInside.right - nMinHScrollWidth);
    	GetSplitterRect(rectSplit); 
		hDWP = DeferClientPos(hDWP, pScrollBar,
			rectSplit.right,
			rectInside.bottom,
			rectInside.right - rectSplit.right + 1,
			m_cyHScroll, TRUE, 0);
		cxTabCtrl = m_cxTabCtrl;
	}
    else
        cxTabCtrl = rectInside.Width();
	    
    // Resize tab control
    // Note, the tab control draws it's own border, so
    // no need to subtract border pixels
    CWnd* pTabCtrl = GetDlgItem(SEC_IDW_TABCTRL);     
    ASSERT(pTabCtrl != NULL);
    hDWP = DeferClientPos(hDWP, pTabCtrl, rectInside.left, rectInside.bottom,
    		cxTabCtrl, m_cyTabCtrl, FALSE, SWP_NOREDRAW);

    if (!m_bTracking)
	{
	    CWnd* pScrollBar = GetScrollBar(SB_VERT);
		if (pScrollBar) {
		    hDWP = DeferClientPos(hDWP, pScrollBar, rectInside.right, rectInside.top,
				m_cxVScroll, rectInside.Height(), TRUE, 0);
		}
    
	    if (m_pActiveWnd)
		    hDWP = DeferClientPos(hDWP, m_pActiveWnd, rectInside.left, rectInside.top,
				rectInside.Width(), rectInside.Height(), FALSE, 0);
	}
    
    ::EndDeferWindowPos(hDWP);
}

void SECTabWnd::OnSize(UINT nType, int cx, int cy) 
{
    ASSERT_VALID(this);
    
    if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
	{
		if (GetScrollBar(SB_HORZ)) {
		    if (m_bFirstSize)
			{
			    m_cxTabCtrl = max(cx - nMinHScrollWidth, 0);
			    m_bFirstSize = FALSE;
			}
		    else if ( m_cxTabCtrl > cx - nMinHScrollWidth )
			m_cxTabCtrl = max(cx - nMinHScrollWidth, 0);
		}
	    else if (m_bFirstSize) {
		    CRect rectInside;
		    GetInsideRect(rectInside);
		    m_cxTabCtrl = rectInside.Width();
		    m_bFirstSize = FALSE;
		}
	    
	    RecalcLayout();

	    // We must completely redraw the tab control after a resize
	    CWnd* pTabCtrl = GetDlgItem(SEC_IDW_TABCTRL);
	    ASSERT(pTabCtrl != NULL);
	    pTabCtrl->Invalidate(TRUE);

		if (GetScrollBar(SB_HORZ)) {
			// Redraw the splitter
			CRect rectInvalid;
			GetSplitterRect(rectInvalid);
			InvalidateRect(&rectInvalid, TRUE);
		}
	}
    
    SECTabWndBase::OnSize(nType, cx, cy);
}


void SECTabWnd::OnDrawSplitter(CDC* pDC, const CRect& rect)
    // if pDC == NULL then just invalidate
{
    ASSERT_VALID(this);
    ASSERT_VALID(pDC);

    COLORREF rgbFace	= GetSysColor(COLOR_BTNFACE);
    COLORREF rgbShadow	= GetSysColor(COLOR_BTNSHADOW);
    COLORREF rgbHilite	= GetSysColor(COLOR_BTNHIGHLIGHT);
    //COLORREF rgbFrame	= GetSysColor(COLOR_WINDOWFRAME);
    
    rgbFill(pDC, rect.left, rect.top, 1, rect.Height(), RGB(0,0,0));
    rgbFill(pDC, rect.left+1, rect.top, 1, rect.Height(), rgbHilite);
    rgbFill(pDC, rect.left+2, rect.top, 1, rect.Height(), rgbFace);
    rgbFill(pDC, rect.left+3, rect.top, 1, rect.Height(), rgbShadow);
    rgbFill(pDC, rect.left+4, rect.top, 1, rect.Height(), RGB(0,0,0));
}


void SECTabWnd::OnPaint() 
{
    ASSERT_VALID(this);
    CPaintDC dc(this); // device context for painting

	CRect rectInvalid;
    dc.GetClipBox(&rectInvalid);
    MapWindowPoints(m_pTabCtrl, rectInvalid);
	m_pTabCtrl->RedrawWindow(rectInvalid, NULL, RDW_INVALIDATE);

    if (GetScrollBar(SB_HORZ))
	{
	    CRect rect;
	    GetSplitterRect(rect);
	    OnDrawSplitter(&dc, rect);
	}
   
    // Do not call SECTabnWnd::OnPaint() for painting messages
}

void SECTabWnd::rgbFill(CDC* pDC,int x,int y,int cx,int cy, COLORREF rgb)
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

BOOL SECTabWnd::OnEraseBkgnd(CDC* pDC) 
{
    ASSERT_VALID(this);
    ASSERT_VALID(pDC);

	CWnd* pWnd = (CWnd*)NULL;
	this->GetActiveTab(pWnd);

	// if no active tab, bail out
	if(!pWnd) return FALSE;

    if (GetScrollBar(SB_HORZ,pWnd) || GetScrollBar(SB_VERT,pWnd))
	{    
	    CBrush brErase;
	    CBrush* brOld;
	    brErase.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
	    brOld = (CBrush*) pDC->SelectObject(&brErase);
	    CRect rect;
	    GetClientRect(rect);
	    rect.left = rect.right - m_cxVScroll; //+ 1;
	    pDC->FillRect(rect, &brErase);
	    pDC->SelectObject(brOld);
	    brErase.DeleteObject();
	}
    
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWnd tracking support

void SECTabWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
    ASSERT_VALID(this);

    CRect rectClient;
    GetClientRect(&rectClient);
    
    if (GetScrollBar(SB_HORZ))
	{
	    CRect rect;
	    GetSplitterRect(rect);
	    OnMouseMove(nFlags, point);
	    if (rect.PtInRect(point))
		{
		    SetCapture();
		    m_bTracking = TRUE;
		    m_nTrackOffset = point.x - rect.left;
		    return;
		}
	}
    
    SECTabWndBase::OnLButtonDown(nFlags, point);
}

void SECTabWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
    ASSERT_VALID(this);

    if (m_bTracking)
	{
	    ReleaseCapture();
	    m_bTracking = FALSE;
	}
    
    SECTabWndBase::OnLButtonUp(nFlags, point);
}


void SECTabWnd::OnMouseMove(UINT nFlags, CPoint point)
{
    ASSERT_VALID(this);

    CRect rectSplit;

    if (m_bTracking && point.x != m_cxTabCtrl)
	{
	    int old_cx;
	    
	    old_cx = m_cxTabCtrl;
	    m_cxTabCtrl = point.x - m_nTrackOffset - 1;
	    RecalcLayout();

	    // Redraw the part of the tab control that was exposed
	    CRect rectInvalid;
	    CWnd* pTabCtrl = GetDlgItem(SEC_IDW_TABCTRL);
	    ASSERT(pTabCtrl != NULL);
	    rectInvalid.SetRect(old_cx, 0, m_cxTabCtrl + m_cxSplitter, m_cyTabCtrl);
	    pTabCtrl->InvalidateRect(&rectInvalid, TRUE);

	    // Redraw the splitter
	    GetSplitterRect(rectInvalid);
	    InvalidateRect(&rectInvalid, TRUE);
	    
	    UpdateWindow();
	    return;
	}
    else if (GetScrollBar(SB_HORZ))
	{
	    GetSplitterRect(rectSplit);
	    if (rectSplit.PtInRect(point))
	        m_bSplitterHit = TRUE;
 	    else
		m_bSplitterHit = FALSE;
	}
    
    SECTabWndBase::OnMouseMove(nFlags, point);
}

BOOL SECTabWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest == HTCLIENT && pWnd == this && m_bSplitterHit)
		::SetCursor(m_hcurSplit);
		
	return SECTabWndBase::OnSetCursor(pWnd, nHitTest, message);
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWnd message handlers

LRESULT SECTabWnd::OnTabSelect(WPARAM wParam, LPARAM /*lParam*/)
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
    
	ASSERT_VALID(m_pNotifyWnd);
	m_pNotifyWnd->SendMessage( TCM_TABSEL, wParam, 0 );

    return 0;
}

void SECTabWnd::OnVScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    // Forward message to active window
    MSG msg = *GetCurrentMessage();
    if (m_pActiveWnd)
	m_pActiveWnd->SendMessage(msg.message, msg.wParam, msg.lParam);
}

void SECTabWnd::OnHScroll(UINT /*nSBCode*/, UINT /*nPos*/, CScrollBar* /*pScrollBar*/)
{
    // Forward message to active window
    MSG msg = *GetCurrentMessage();
    if (m_pActiveWnd)
	m_pActiveWnd->SendMessage(msg.message, msg.wParam, msg.lParam);
}
