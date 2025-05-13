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
//  Author:       Dean Hallman
//  Description:  Implementation of SECTab/SECTabControl
//

#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __TABCTRL_H__
#include "..\..\Include\WndTabs\tabctrl.h"
#endif //__TABCTRL_H__

#ifdef _SECDLL
#undef AFXAPP_DATA
#define AFXAPP_DATA AFXAPI_DATA
#endif //_SECDLL

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#define new DEBUG_NEW
#define ASSERT_VALID_BUTTON(nButton) \
		ASSERT(nButton == TCBM_NONE || nButton == TCBM_FIRST || \
		    nButton == TCBM_LEFT || nButton == TCBM_RIGHT || nButton == TCBM_LAST)

#define ASSERT_VALID_BUTTON_MASK(nMask) \
		ASSERT( (~(TCBM_FIRST|TCBM_LEFT|TCBM_RIGHT|TCBM_LAST) & nMask) == 0)


/////////////////////////////////////////////////////////////////////////////
// SECTabControl

IMPLEMENT_DYNAMIC(SECTabControl, SECTabControlBase)

BEGIN_MESSAGE_MAP(SECTabControl, SECTabControlBase)
    //{{AFX_MSG_MAP(SECTabControl)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_SIZE()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDBLCLK()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Colors for Arrows and Buttons
const COLORREF rgbBlack = RGB(0,0,0);

// must define the following as macros in case the system colors change
// JR ( 04/11/97 )
#define rgbFace (::GetSysColor(COLOR_BTNFACE))
#define rgbShadow (::GetSysColor(COLOR_BTNSHADOW))
#define rgbHilite (::GetSysColor(COLOR_BTNHIGHLIGHT))
#define rgbFrame (::GetSysColor(COLOR_WINDOWFRAME))

// Constants
const int nTabSpacing = -5;
const int nTabFold = 6;
const int nScrollIncr = 10;
const int nLabelPadding = 20;
const int nNextTabShowing = 20;

/////////////////////////////////////////////////////////////////////////////
// SECTabControl Construction/Destruction

//@doc SECTabControl
//@mfunc You construct an SECTabControl object in two steps.  First call the constructor SECTabControl, then call Create,
// which initializes the Windows window and attaches it to the SECTabControl object.
//@xref<c SECTabControlBase>
//@xref<c SECTabControl> 
SECTabControl::SECTabControl()
{
    m_nButtons = TCBM_NONE;
    m_nButtonPressed = TCBM_NONE;
    m_nDesensedButtons = TCBM_NONE;
    m_nScrollOffset = 0;
	m_bMultiSelect = TRUE;
}

SECTabControl::~SECTabControl()
{
}

//@doc SECTabControl
//@mfunc Initializes tab control with the style specified.
//@parm DWORD | dwStyle | Initializes the tab control with the specified style.
//@xref<c SECTabControl>
void SECTabControl::Initialize(DWORD dwStyle)
{
	if (dwStyle & TCS_LEFTRIGHTSCROLL)
		m_nButtons = TCBM_LEFT | TCBM_RIGHT;
	else if (dwStyle & TCS_FULLSCROLL)
		m_nButtons = TCBM_FIRST | TCBM_LAST | TCBM_LEFT | TCBM_RIGHT;
	else
		m_nButtons = TCBM_NONE;
	
    m_nHeight = ::GetSystemMetrics(SM_CYHSCROLL) + 1;
    m_sizeBtn.cy = m_nHeight;
    m_sizeBtn.cx = m_sizeBtn.cy;
	
#ifdef _SEC_INTL_
	BYTE nCharSet = DEFAULT_CHARSET;
#else
	BYTE nCharSet = ANSI_CHARSET;
#endif
    // Fonts
    m_fonts[SECTAB_ACTIVE].CreateFont(m_nHeight - 2, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, _T("Arial Narrow"));
    m_fonts[SECTAB_SELECTED].CreateFont(m_nHeight - 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, _T("Arial Narrow"));
    m_fonts[SECTAB_UNSELECTED].CreateFont(m_nHeight - 2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, 0,
		nCharSet, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS, _T("Arial Narrow"));
	
    // Brushes
    m_brushes[SECTAB_ACTIVE].CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
    m_brushes[SECTAB_SELECTED].CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
    m_brushes[SECTAB_UNSELECTED].CreateSolidBrush(rgbFace);
	
    // Pens
    m_pens[SECTAB_ACTIVE].CreatePen(PS_SOLID, 1, rgbBlack);
    m_pens[SECTAB_SELECTED].CreatePen(PS_SOLID, 1, rgbBlack);
    m_pens[SECTAB_UNSELECTED].CreatePen(PS_SOLID, 1, rgbBlack);
    m_penShadow.CreatePen(PS_SOLID, 1, rgbShadow);
	
    // Text color
    m_rgbText[SECTAB_ACTIVE] = rgbBlack;
    m_rgbText[SECTAB_SELECTED] = rgbBlack;
    m_rgbText[SECTAB_UNSELECTED] = rgbBlack;
}

void SECTabControl::OnDestroy()
{
    SECTabControlBase::OnDestroy();

    // Destroy GDI Objects
    m_fonts[SECTAB_ACTIVE].DeleteObject();
    m_fonts[SECTAB_SELECTED].DeleteObject();
    m_fonts[SECTAB_UNSELECTED].DeleteObject();
    m_brushes[SECTAB_ACTIVE].DeleteObject();
    m_brushes[SECTAB_SELECTED].DeleteObject();
    m_brushes[SECTAB_UNSELECTED].DeleteObject();
    m_pens[SECTAB_ACTIVE].DeleteObject();
    m_pens[SECTAB_SELECTED].DeleteObject();
    m_pens[SECTAB_UNSELECTED].DeleteObject();
    m_penShadow.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControl Overrides

BOOL SECTabControl::SetFontTab(CFont* pFont,int nTabType,BOOL bRedraw) {

#ifndef WIN32
	// this function not supported for WIN16!
	pFont;
	nTabType;
	bRedraw;
	return FALSE;
#else	// WIN32

	if(!pFont) return FALSE;

	ASSERT((nTabType==SECTAB_SELECTED)||(nTabType==SECTAB_UNSELECTED)||
		   (nTabType==SECTAB_ACTIVE));
	if((nTabType!=SECTAB_SELECTED)&&(nTabType!=SECTAB_UNSELECTED)&&
	   (nTabType!=SECTAB_ACTIVE)) 
		return FALSE;

	// clone font passed in as new tab font
	// Note: cloning used instead of straight ptr manipulation
	// to preserve backward compatibility.
	LOGFONT lfNew,lfOld;
	if(!pFont->GetLogFont(&lfNew)) return FALSE;
	if(!m_fonts[nTabType].GetLogFont(&lfOld)) return FALSE;
	m_fonts[nTabType].DeleteObject();
	if(!m_fonts[nTabType].CreateFontIndirect(&lfNew)) {
		// if failure, revert to old font
		m_fonts[nTabType].CreateFontIndirect(&lfOld);
		return FALSE;
		}

	// success, should we invalidate?
	if(bRedraw) {
		if(nTabType==SECTAB_ACTIVE) {
			InvalidateTab(m_nActiveTab);
			}
		else {
			for (int nTab = 0; nTab < GetTabCount(); nTab++) 
				InvalidateTab(nTab);
			}
		}

	return TRUE;

#endif	// WIN32
}

//@doc SECTabControl
//@mfunc Invalidates specified tab for repainting.
//@rdesc void 
//@parm int | nTab | Index of tab.
//@xref<c SECTabControl>
void SECTabControl::InvalidateTab(int nTab)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nTab));
	
    CRgn tab_rgn;
	GetTabRgn(nTab, tab_rgn);
	InvalidateRgn(&tab_rgn, TRUE);
	tab_rgn.DeleteObject();
}

//Override needed to update 3D tab data members
SECTab* SECTabControl::InsertTab(int nIndex,
								 LPCTSTR lpszLabel,
								 CObject* pClient,
								 HMENU hMenu,
								 void* pExtra)
{
	ASSERT_VALID(this);
	
	SECTab* pTab = SECTabControlBase::InsertTab(nIndex, lpszLabel, pClient, hMenu, pExtra);
	
	RecalcLayout();

	// Invalidate the tabs to the right of the new one
	int i;
	for (i=nIndex; i<GetTabCount(); i++)
		InvalidateTab(i);

	return pTab;
}

//@doc SECTabControl
//@mfunc Recalculates layout of tab control.
//@rdesc void 
//@xref<c SECTabControl>
void SECTabControl::RecalcLayout()
{
	SECTab* pTab;
	int nTab;

	for (nTab = 0; nTab < GetTabCount(); nTab++) {
		pTab = GetTabPtr(nTab);
		pTab->m_nWidth = GetLabelWidth(nTab) + nLabelPadding;
	}
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControl Operation

//@doc SECTabControl
//@mfunc Scrolls tabs so that the index passed in is now visible.
//@rdesc void 
//@parm int | nTab | Index of tab to scroll to.
//@xref<c SECTabControl>
void SECTabControl::ScrollToTab(int nTab)
{
    ASSERT_VALID(this);
	
	CRect rectClient;
	GetClientRect(rectClient);
	if (!rectClient.IsRectNull()) {
		CRgn tabRgn;
		CRect rect, rectTabs;
		int nPrevScrollOffset;
		int nOffset;

		nPrevScrollOffset = m_nScrollOffset;
		GetTabsRect(rectTabs);
		GetTabRgn(nTab, tabRgn);
		tabRgn.GetRgnBox(&rect);
    
		if (rect.Width() + nNextTabShowing < rectTabs.Width())
			nOffset = nNextTabShowing;
		else
			nOffset = 0;
    
		if (rect.right > rectTabs.right)
			m_nScrollOffset -= (rect.right - rectTabs.right + nOffset);
		
		tabRgn.DeleteObject();
		GetTabRgn(nTab, tabRgn);
		tabRgn.GetRgnBox(&rect);
    
		if (rect.left < rectTabs.left)
			m_nScrollOffset += (rectTabs.left - rect.left + nOffset);
		
    
		RecalcButtonSensitivity();		
		if ((m_nScrollOffset - nPrevScrollOffset) != 0) {
			ScrollWindow(m_nScrollOffset - nPrevScrollOffset, 0, rectTabs, rectTabs);
			UpdateWindow();
		}
		
		// cleanup
		tabRgn.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControl Queries

//@doc SECTabControl
//@mfunc Determines if point passed in resides inside tab at index nTab.
//@rdesc Nonzero if the point is inside the tab specified.
//@parm int | nTab | Tab index to test.
//@parm  CPoint& | point | Point to test.
//@xref<c SECTabControl>
BOOL SECTabControl::TabHit(int nTab, CPoint& point) const
{
    CRgn tab_rgn;
	BOOL bHit = FALSE;

	GetTabRgn(nTab, tab_rgn);
	if (tab_rgn.PtInRegion(point))
		bHit = TRUE;

	tab_rgn.DeleteObject();

	return bHit;
}

int SECTabControl::GetLabelWidth(int nTab)
{
    ASSERT_VALID(this);

	SECTab* pTab = GetTabPtr(nTab);

    ASSERT(AfxIsValidString(pTab->m_csLabel));

    CClientDC dc(this);

    ASSERT_VALID(&dc);

    CFont *pOldFont;
    CSize labelExtent;
    
    // Select the appropriate font and compute size of label (in pixels)
    ASSERT_VALID(&m_fonts[SECTAB_ACTIVE]);
    pOldFont = dc.SelectObject(&m_fonts[SECTAB_ACTIVE]);
#ifdef WIN32
    labelExtent = dc.GetTextExtent(pTab->m_csLabel, _tcslen(pTab->m_csLabel));
#else
    labelExtent = dc.GetTextExtent(pTab->m_csLabel, _fstrlen(pTab->m_csLabel));
#endif
    dc.SelectObject(pOldFont);

	return labelExtent.cx;
}


void SECTabControl::GetTabRgn(int nTab, CRgn &tabRgn) const
{
    ASSERT(nTab >= 0 && nTab < GetTabCount());

    CPoint pts[4];
    CPoint tab_offset;
    CRect tabsRect;
    int tab;
    
    GetTabsRect(tabsRect);
    for (tab = 0, tab_offset.x = tabsRect.left + m_nScrollOffset; tab < nTab;
	 tab_offset.x += (GetTab(tab).m_nWidth + nTabSpacing), tab++);
    
    // Initialize pts in the tab polygon
    pts[0].x = tab_offset.x;
    pts[0].y = tabsRect.top;
    pts[1].x = pts[0].x + nTabFold;
    pts[1].y = tabsRect.bottom;
    pts[2].x = pts[1].x + GetTab(nTab).m_nWidth - nTabFold*2;
    pts[2].y = pts[1].y;
    pts[3].x = pts[2].x + nTabFold;
    pts[3].y = pts[0].y;
    
    tabRgn.CreatePolygonRgn(pts, 4, ALTERNATE);
}

void SECTabControl::OnPaint() 
{
    ASSERT_VALID(this);
    CPaintDC dc(this); // device context for painting
    CRect rectInvalid;
    CRect rect;
    CRect intersection;
    int oldBkMode;
    CPen *pOldPen;
    CBrush *pOldBrush;
    CFont *pOldFont;
    COLORREF oldTextColor;
    CRgn tabRgn;
    int nTab;
    CRect btnRect, intersect;
    RECT rectTab;
    CRect tabsRect;
    CRect rectBtn;
    
    dc.GetClipBox(&rectInvalid);
    
    oldBkMode = dc.SetBkMode(TRANSPARENT);
    
    // Draw invalidated buttons
    GetButtonRect(TCBM_FIRST, rectBtn);
    //rectBtn.OffsetRect(1, 0);
    if (m_nButtons & TCBM_FIRST && intersect.IntersectRect(rectInvalid, rectBtn))
		DrawButton(&dc, TCBM_FIRST);
    
    GetButtonRect(TCBM_LEFT, rectBtn);
    //rectBtn.OffsetRect(1, 0);
    if (m_nButtons & TCBM_LEFT && intersect.IntersectRect(rectInvalid, rectBtn))
		DrawButton(&dc, TCBM_LEFT);
    
    GetButtonRect(TCBM_RIGHT, rectBtn);
    //rectBtn.OffsetRect(1, 0);
    if (m_nButtons & TCBM_RIGHT && intersect.IntersectRect(rectInvalid, rectBtn))
		DrawButton(&dc, TCBM_RIGHT);
    
    GetButtonRect(TCBM_LAST, rectBtn);
    //rectBtn.OffsetRect(1, 0);
    if (m_nButtons & TCBM_LAST && intersect.IntersectRect(rectInvalid, rectBtn))
		DrawButton(&dc, TCBM_LAST);
    
    GetTabsRect(tabsRect);		
    dc.IntersectClipRect(tabsRect);
    
    // Draw invalidated, unselected tabs
    oldBkMode = dc.SetBkMode(TRANSPARENT);
    oldTextColor = dc.SetTextColor(m_rgbText[SECTAB_UNSELECTED]);
    pOldBrush = dc.SelectObject(&m_brushes[SECTAB_UNSELECTED]);
    pOldFont = dc.SelectObject(&m_fonts[SECTAB_UNSELECTED]);
    pOldPen = dc.SelectObject(&m_pens[SECTAB_UNSELECTED]);
    for (nTab = 0; nTab < GetTabCount(); nTab++) {
		GetTabRgn(nTab, tabRgn);
		if ((!m_bActiveTab || nTab != m_nActiveTab) &&
			!GetTab(nTab).m_bSelected && tabRgn.RectInRegion(rectInvalid))
			DrawTab(&dc, nTab, tabRgn);
		tabRgn.DeleteObject();
    }
    
    // Draw invalidated, selected tabs
    dc.SetTextColor(m_rgbText[SECTAB_SELECTED]);
    dc.SelectObject(&m_brushes[SECTAB_SELECTED]);
    dc.SelectObject(&m_fonts[SECTAB_SELECTED]);
    dc.SelectObject(&m_pens[SECTAB_SELECTED]);
    for (nTab = 0; nTab < GetTabCount(); nTab++) {
		GetTabRgn(nTab, tabRgn);
		if ((!m_bActiveTab || nTab != m_nActiveTab) &&
			GetTab(nTab).m_bSelected && tabRgn.RectInRegion(rectInvalid))
			DrawTab(&dc, nTab, tabRgn);
		tabRgn.DeleteObject();
    }
    
    // Draw the active tab last so it appears on top
    dc.SetTextColor(m_rgbText[SECTAB_ACTIVE]);
    dc.SelectObject(&m_brushes[SECTAB_ACTIVE]);
    dc.SelectObject(&m_fonts[SECTAB_ACTIVE]);
    dc.SelectObject(&m_pens[SECTAB_ACTIVE]);
    if (GetTabCount() > 0 && m_bActiveTab) {
		CRect btnArea;
		
		GetTabRgn(m_nActiveTab, tabRgn);
		if (tabRgn.RectInRegion(rectInvalid))
			DrawTab(&dc, m_nActiveTab, tabRgn);
		
		// Draw line at top of tab
		tabRgn.GetRgnBox(&rectTab);
		GetButtonsRect(btnArea);
		if (rectTab.left > btnArea.right) {
			dc.MoveTo(tabsRect.left, tabsRect.top);
			dc.LineTo(rectTab.left, tabsRect.top);
		}
		dc.MoveTo(rectTab.right - 1, tabsRect.top);
		dc.LineTo(tabsRect.right, tabsRect.top);
		
		tabRgn.DeleteObject();
    }
    else {
		// Draw line at top of tab
		dc.MoveTo(tabsRect.left, tabsRect.top);
		dc.LineTo(tabsRect.right, tabsRect.top);
    }
	
    // Cleanup
    dc.SetBkMode(oldBkMode);
    dc.SelectObject(pOldFont);
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
    dc.SetTextColor(oldTextColor);
}

//@doc SECTabControl
//@mfunc Draws specified tab by reference to a CRgn region with clipping as determined by a pointer to a CDC.
//@rdesc void 
//@parm CDC * | pDC | Pointer to a device context.
//@parm int | nTab | Index of tab.
//@parm  CRgn & | tabRgn | Region to be drawn.
//@xref<c SECTabControl>
void SECTabControl::DrawTab(CDC *pDC, int nTab, CRgn &tabRgn)
{
    CPen *pOldPen;
    CPoint pts[4];
    CPoint tab_offset;
    CRect rectClient;
    RECT rectTab;
    CSize text_size;
    UINT oldTextAlign;
    CPoint refPt;
	CRgn fillRgn;
	
    ASSERT(nTab >= 0 && nTab < GetTabCount());
	
    // Fill tab	using currently selected brush
    CRect tabsRect;
    int tab;
    
    GetTabsRect(tabsRect);
    for (tab = 0, tab_offset.x = tabsRect.left + m_nScrollOffset; tab < nTab;
	 tab_offset.x += (GetTab(tab).m_nWidth + nTabSpacing), tab++);
    
    // Initialize pts in the tab polygon
    pts[0].x = tab_offset.x;
    pts[0].y = tabsRect.top;
    pts[1].x = pts[0].x + nTabFold;
    pts[1].y = tabsRect.bottom;
    pts[2].x = pts[1].x + GetTab(nTab).m_nWidth - nTabFold*2 - 1;
    pts[2].y = pts[1].y;
    pts[3].x = pts[2].x + nTabFold - 1;
    pts[3].y = pts[0].y;
    
    fillRgn.CreatePolygonRgn(pts, 4, ALTERNATE);
    pDC->PaintRgn(&fillRgn);
	fillRgn.DeleteObject();
    
    // Compute size of label (in pixels)
    text_size = pDC->GetTextExtent(GetTab(nTab).m_csLabel,
		_tcslen(GetTab(nTab).m_csLabel));
    
    // Compute tab points
    // (Unfortunately, we can't access the points comprising the tabRgn
    //  so we have to recompute them)
    tabRgn.GetRgnBox(&rectTab);
    
    pts[0].x = rectTab.left;
    pts[0].y = rectTab.top;
    pts[1].x = pts[0].x + nTabFold;
    pts[1].y = rectTab.bottom - 1;   
    pts[2].x = pts[1].x + GetTab(nTab).m_nWidth - nTabFold*2;
    pts[2].y = pts[1].y;
    pts[3].x = pts[2].x + nTabFold;
    pts[3].y = pts[0].y;
    refPt.x = pts[1].x + (pts[2].x - pts[1].x - text_size.cx)/2;
    refPt.y = pts[1].y + (pts[0].y - pts[1].y + text_size.cy)/2;
    
    // Draw tab lines
    pDC->MoveTo(pts[0].x, pts[0].y);
    pDC->LineTo(pts[1].x, pts[1].y);
    pOldPen = pDC->SelectObject(&m_penShadow);
    pDC->LineTo(pts[2].x-2, pts[2].y);
    pDC->LineTo(pts[3].x-2, pts[3].y);
    pDC->SelectObject(pOldPen);
    pDC->MoveTo(pts[2].x-1, pts[2].y);
    pDC->LineTo(pts[3].x-1, pts[3].y);
	
    // Draw tab text
    oldTextAlign = pDC->SetTextAlign( TA_LEFT | TA_BOTTOM | TA_NOUPDATECP );
    pDC->TextOut(refPt.x, refPt.y, GetTab(nTab).m_csLabel,
    				_tcslen(GetTab(nTab).m_csLabel));
}

BOOL SECTabControl::OnEraseBkgnd(CDC* pDC) 
{
    ASSERT_VALID(this);
    ASSERT_VALID(pDC);

    CRect rect;
    GetClientRect(rect);
    pDC->FillRect(rect, &m_brushes[SECTAB_UNSELECTED]);
    
    return TRUE;
}

void SECTabControl::GetButtonRect(int nButton, CRect& rect)
{
    ASSERT_VALID_BUTTON(nButton);
	
    int b;
    // Compute rectangle of specified button
    rect.left = 0;
    for (b = TCBM_FIRST; b != nButton; b <<= 1)
        if (m_nButtons & b)
		rect.left += m_sizeBtn.cx;
    
    rect.top = 0;
    rect.right = rect.left + m_sizeBtn.cx;
    rect.bottom = rect.top + m_sizeBtn.cy;
}

void SECTabControl::DrawButton(CDC* pDC, int nButton)
{
    ASSERT_VALID_BUTTON(nButton);
    ASSERT(pDC);
	
    int x, y, dx, dy, icon_cx, icon_cy;
    CRect rect;
    
    GetButtonRect(nButton, rect);
	
    x = rect.left;
    y = rect.top + 1;
    dx = rect.Width() - 1;
    dy = rect.Height() - 1;
	icon_cx = (int)(rect.Width() * (float)0.30);
	icon_cy = (int)(rect.Height() * (float)0.55);
    
    // border around button
    rgbFill(pDC, x-1,  y-1,    dx+1, 1,  rgbFrame);
    rgbFill(pDC, x,    y+dy,   dx, 1,  rgbFrame);
    rgbFill(pDC, x-1,  y,      1,  dy, rgbFrame);
    rgbFill(pDC, x+dx, y-1,    1,  dy, rgbFrame);
	
    // interior grey
    rgbFill(pDC, x+1, y, dx-1, dy, rgbFace);
    
    if (m_nButtonPressed == nButton)
	{
		// pressed in or down or down disabled
		rgbFill(pDC, x, y, 2, dy, rgbShadow);
		rgbFill(pDC, x, y, dx, 2, rgbShadow);
		
		rgbFill(pDC, x + dx - 1, y, 1, dy, rgbShadow);
		rgbFill(pDC, x, y + dy-1, dx, 1,   rgbShadow);
	}
    else
	{
		// regular button look
		rgbFill(pDC, x, y, 1, dy - 1, rgbHilite);
		rgbFill(pDC, x, y, dx - 1, 1, rgbHilite);
		
		rgbFill(pDC, x + dx - 1, y, 1, dy, rgbShadow);
		rgbFill(pDC, x, y + dy-2, dx, 1,   rgbShadow);
		
		rgbFill(pDC, x + 1 + dx - 3, y + 1, 1, dy-2, rgbShadow);
		rgbFill(pDC, x + 1, y + dy - 3, dx - 2, 1, rgbShadow);
	}
    
    // if pressed, image gets moved
    if (m_nButtonPressed == nButton) {
		rect.left++;
		rect.top++;
		rect.bottom++;
		rect.right++;
    }

    // Center the icon image
    x = rect.left + rect.Width()/2;
    y = rect.top + (rect.Height()-1)/2;
    
    COLORREF rgb;
    
    if (m_nDesensedButtons & nButton)
		rgb = rgbShadow;
    else
		rgb = rgbBlack;
    
	CPoint pts[3];
	CBrush bgBrush(rgb);
	CBrush* pOldBrush;
	CPen pen(PS_SOLID, 1, rgb);
	CPen* pOldPen;
	pOldBrush = pDC->SelectObject(&bgBrush);
	pOldPen = pDC->SelectObject(&pen);

    switch (nButton) {
    case TCBM_FIRST:
		// Draw Left Arrow
		pts[0].x = x - icon_cx/2;
		pts[0].y = y;
		pts[1].x = pts[0].x + icon_cx;
		pts[1].y = pts[0].y + icon_cy/2;
		pts[2].x = pts[1].x;
		pts[2].y = pts[1].y - icon_cy;
		pDC->Polygon(pts, 3);
		// Draw Line
		pDC->MoveTo(pts[0].x - 2, pts[1].y);
		pDC->LineTo(pts[0].x - 2, pts[2].y-1);
		break;
   case TCBM_LEFT:
		x -= 2;
		// Draw Left Arrow
		pts[0].x = x - icon_cx/2;
		pts[0].y = y;
		pts[1].x = pts[0].x + icon_cx;
		pts[1].y = pts[0].y + icon_cy/2;
		pts[2].x = pts[1].x;
		pts[2].y = pts[1].y - icon_cy;
		pDC->Polygon(pts, 3);
		break;
    case TCBM_RIGHT:
		// Draw Right Arrow
		pts[0].x = x + icon_cx/2;
		pts[0].y = y;
		pts[1].x = pts[0].x - icon_cx;
		pts[1].y = pts[0].y + icon_cy/2;
		pts[2].x = pts[1].x;
		pts[2].y = pts[1].y - icon_cy;
		pDC->Polygon(pts, 3);
		break;
    case TCBM_LAST:
		x -= 2;
		// Draw Right Arrow
		pts[0].x = x + icon_cx/2;
		pts[0].y = y;
		pts[1].x = pts[0].x - icon_cx;
		pts[1].y = pts[0].y + icon_cy/2;
		pts[2].x = pts[1].x;
		pts[2].y = pts[1].y - icon_cy;
		pDC->Polygon(pts, 3);
		// Draw Line
		pDC->MoveTo(pts[0].x + 2, pts[1].y);
		pDC->LineTo(pts[0].x + 2, pts[2].y-1);
		break;
    }

	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	bgBrush.DeleteObject();
	pen.DeleteObject();
}

void SECTabControl::rgbFill(CDC* pDC,int x,int y,int cx,int cy, COLORREF rgb)
{
    RECT rect;
    COLORREF rgbOld;
    
    rect.left   = x;
    rect.top    = y;
    rect.right  = x + cx;
    rect.bottom = (y + cy);
    rgbOld = pDC->GetBkColor();
    pDC->SetBkColor(rgb);
    pDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
    pDC->SetBkColor(rgbOld);
}

void SECTabControl::RecalcButtonSensitivity()
{
    ASSERT_VALID(this);
	
    CRgn rightmostTabRgn;
    RECT rightmostTabRect;
    RECT rectClient;
    int maxScrollRight;
    CRect rect;
	
    if (GetTabCount() > 0) {    
		GetClientRect(&rectClient);
		GetTabRgn(GetTabCount()-1, rightmostTabRgn);
		rightmostTabRgn.GetRgnBox(&rightmostTabRect);
		maxScrollRight = m_nScrollOffset - (rightmostTabRect.right - rectClient.right);
		
		if (m_nScrollOffset >= 0) {
			m_nScrollOffset = 0;
			if (!(m_nDesensedButtons & TCBM_LEFT)) {
				if (m_nButtons & TCBM_FIRST) {
					m_nDesensedButtons |= TCBM_FIRST;
					GetButtonRect(TCBM_FIRST, rect);
					InvalidateRect(rect, FALSE);
				}
				if (m_nButtons & TCBM_LEFT) {
					m_nDesensedButtons |= TCBM_LEFT;
					GetButtonRect(TCBM_LEFT, rect);
					InvalidateRect(rect, FALSE);
				}
			}
		}
		else if (m_nDesensedButtons & TCBM_LEFT) {
			if (m_nButtons & TCBM_FIRST) {
				m_nDesensedButtons &= ~TCBM_FIRST;
				GetButtonRect(TCBM_FIRST, rect);
				InvalidateRect(rect, FALSE);
			}
			if (m_nButtons & TCBM_LEFT) {
				m_nDesensedButtons &= ~TCBM_LEFT;
				GetButtonRect(TCBM_LEFT, rect);
				InvalidateRect(rect, FALSE);
			}
		}
		
		if (m_nScrollOffset <= maxScrollRight) {
			m_nScrollOffset = (maxScrollRight <= 0) ? maxScrollRight : 0;
			if (!(m_nDesensedButtons & TCBM_RIGHT)) {
				if (m_nButtons & TCBM_RIGHT) {
					m_nDesensedButtons |= TCBM_RIGHT;
					GetButtonRect(TCBM_RIGHT, rect);
					InvalidateRect(rect, FALSE);
				}
				if (m_nButtons & TCBM_LAST) {
					m_nDesensedButtons |= TCBM_LAST;
					GetButtonRect(TCBM_LAST, rect);
					InvalidateRect(rect, FALSE);
				}
			}
		}
		else if (m_nDesensedButtons & TCBM_RIGHT) {
			if (m_nButtons & TCBM_RIGHT) {
				m_nDesensedButtons &= ~TCBM_RIGHT;
				GetButtonRect(TCBM_RIGHT, rect);
				InvalidateRect(rect, FALSE);
			}
			if (m_nButtons & TCBM_LAST) {
				m_nDesensedButtons &= ~TCBM_LAST;
				GetButtonRect(TCBM_LAST, rect);
				InvalidateRect(rect, FALSE);
			}
		}
    } else {
		m_nDesensedButtons = TCBM_FIRST | TCBM_LEFT | TCBM_RIGHT | TCBM_LAST;
		GetButtonsRect(rect);
		InvalidateRect(rect, FALSE);
    }
    
    // cleanup
    rightmostTabRgn.DeleteObject();
}

// Compute rectangle within client area inwhich
// the tabs are drawn.
void SECTabControl::GetTabsRect(CRect& tabsRect) const
{
    RECT rectClient;
    int nButton;
    
    // Compute left edge of tabs rect
    tabsRect.left = 0;
    for (nButton = TCBM_FIRST; nButton <= TCBM_LAST; nButton <<= 1) {
		if (m_nButtons & nButton)
			tabsRect.left += m_sizeBtn.cx;
	}    
    
    GetClientRect(&rectClient);
    tabsRect.top = rectClient.top;
	tabsRect.right = rectClient.right;
    tabsRect.bottom = rectClient.bottom;
}

void SECTabControl::GetButtonsRect(CRect& btnArea)
{
    ASSERT_VALID(this);

    RECT rectClient;
    int nButton;
    
    // Compute left edge of tabs rect
    btnArea.right = 1;
    for (nButton = TCBM_FIRST; nButton <= TCBM_LAST; nButton <<= 1)
	if (m_nButtons & nButton)
	    btnArea.right += m_sizeBtn.cx;
    
    GetClientRect(&rectClient);
    btnArea.top = rectClient.top;
    btnArea.left = rectClient.left;
    btnArea.bottom = rectClient.bottom;
}

void SECTabControl::OnSize(UINT nType, int cx, int cy) 
{
    CRect tabsRect;
    int nPrevScrollOffset;
    
	GetTabsRect(tabsRect);
	nPrevScrollOffset = m_nScrollOffset;
	RecalcButtonSensitivity();
	if ((m_nScrollOffset - nPrevScrollOffset) != 0)	{
	    ScrollWindow(m_nScrollOffset - nPrevScrollOffset, 0, tabsRect, tabsRect);
	    UpdateWindow();
	}
    
    SECTabControlBase::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControl mouse input handlers

void SECTabControl::OnButtonPress(int nButton)
{
    ASSERT_VALID(this);
    ASSERT_VALID_BUTTON(nButton);
	
    CRgn rightmostTabRgn;
    RECT rightmostTabRect;
    MSG   msgLoop;
    int nPrevScrollOffset;
    int maxScrollRight;
    CRect tabsRect;
    
    if ((m_nDesensedButtons & nButton) == nButton)
		return;
    
    nPrevScrollOffset = m_nScrollOffset;
    GetTabsRect(tabsRect);
    GetTabRgn(GetTabCount()-1, rightmostTabRgn);
    rightmostTabRgn.GetRgnBox(&rightmostTabRect);
    maxScrollRight = m_nScrollOffset - (rightmostTabRect.right - tabsRect.right);
    
    switch (nButton) {
    case TCBM_LEFT:
		m_nScrollOffset += nScrollIncr;
		break;
    case TCBM_RIGHT:
		m_nScrollOffset -= nScrollIncr;
		break;
    case TCBM_FIRST:
		m_nScrollOffset = 0;
		break;
    case TCBM_LAST:
		m_nScrollOffset -= rightmostTabRect.right - tabsRect.right;
		break;
    default:
		ASSERT(FALSE);
		break;
    }
    
    RecalcButtonSensitivity();		
    if ((m_nScrollOffset - nPrevScrollOffset) != 0) {
		ScrollWindow(m_nScrollOffset - nPrevScrollOffset, 0, tabsRect, tabsRect);
		UpdateWindow();
    }
    
    msgLoop.message = 0;
    
    SetCapture();
    
    DWORD nLastTicks = GetTickCount();
    DWORD nTicksNeeded = 300;
    
    while (msgLoop.message != WM_LBUTTONUP)	{
		if (PeekMessage(&msgLoop, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msgLoop);
			DispatchMessage(&msgLoop);
			continue;
		}
		
		if (GetTickCount() - nLastTicks > nTicksNeeded) {
			
			nPrevScrollOffset = m_nScrollOffset;
			
			switch (nButton) {
			case TCBM_LEFT:
				rightmostTabRect.right += nScrollIncr;
				m_nScrollOffset += nScrollIncr;
				break;
			case TCBM_RIGHT:
				rightmostTabRect.right -= nScrollIncr;
				m_nScrollOffset -= nScrollIncr;
				break;
			}
			
			RecalcButtonSensitivity();
			if ((m_nScrollOffset - nPrevScrollOffset) != 0) {
				ScrollWindow(m_nScrollOffset - nPrevScrollOffset, 0, tabsRect, tabsRect);
				UpdateWindow();
			}
			
			nTicksNeeded = 20;
			nLastTicks = GetTickCount();
		}
    }
	
    // cleanup    
    ReleaseCapture();
    rightmostTabRgn.DeleteObject();
}

void SECTabControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    ASSERT_VALID(this);
	
    CPaintDC dc(this);
    int tab;
    int nTabHit = 0;
    BOOL bTabHit = FALSE;
    CRect rectBtn;
    int nButton;
	
    ASSERT_VALID(&dc);
    
    // Determine if a button was hit.
    m_nButtonPressed = TCBM_NONE;
    for (nButton = TCBM_FIRST; nButton <= TCBM_LAST; nButton <<= 1) {
		if (m_nButtons & nButton) {
			GetButtonRect(nButton, rectBtn);
			if (rectBtn.PtInRect(point)) {
				m_nButtonPressed = nButton;
				InvalidateRect(rectBtn, FALSE);
				OnButtonPress(nButton);
				return;
			}
		}
    }
    
    // Determine if a tab was hit.
    for (tab=0, bTabHit = FALSE; tab<GetTabCount() && !bTabHit; tab++) {
		if (TabHit(tab, point)) {
			nTabHit = tab;
			bTabHit = TRUE;
		}
    }
    
    if (bTabHit) {
		if ( m_bMultiSelect && ( nFlags & MK_CONTROL )) {
			GetTab(nTabHit).m_bSelected = !GetTab(nTabHit).m_bSelected;
			if (m_bActiveTab && nTabHit == m_nActiveTab)
				m_bActiveTab = FALSE;
			InvalidateTab(nTabHit);
		}
		else if ( m_bMultiSelect && (nFlags & MK_SHIFT))	{
			int left_tab, right_tab, i;
			
			left_tab = ( m_nActiveTab < nTabHit ) ? m_nActiveTab : nTabHit;
			right_tab = ( m_nActiveTab < nTabHit ) ? nTabHit : m_nActiveTab;
			
			m_bActiveTab = FALSE;
			for (i=0; i<left_tab; i++) {
				if (GetTab(i).m_bSelected) {
					GetTab(i).m_bSelected = FALSE;
					InvalidateTab(i);
				}
			}
			
			for (i=left_tab; i<=right_tab; i++)	{
				GetTab(i).m_bSelected = TRUE;
				InvalidateTab(i);
			}
			
			for (i=right_tab+1; i<GetTabCount(); i++) {
				if (GetTab(i).m_bSelected) {
					GetTab(i).m_bSelected = FALSE;
					InvalidateTab(i);
				}
			}
			
			ActivateTab(nTabHit);
		}
		else if (nTabHit != m_nActiveTab || !m_bActiveTab)
		{
			int i;
			
			for (i=0; i<GetTabCount(); i++)	
			{
				if (GetTab(i).m_bSelected || i == nTabHit)
				{
					GetTab(i).m_bSelected = FALSE;
					InvalidateTab(i);
				}
			}
			
			ActivateTab(nTabHit);
		}
		else if (nTabHit == m_nActiveTab)
		{
			ReActivateTab();
		}
    }
	
    SECTabControlBase::OnLButtonDown(nFlags, point);
}

void SECTabControl::OnLButtonUp(UINT nFlags, CPoint point) 
{
    ASSERT_VALID(this);
	
    CRect rectBtn;
    
    if ( (m_nButtons != TCBM_NONE) && ((m_nButtons & m_nButtonPressed) != TCBM_NONE)) {
		GetButtonRect(m_nButtonPressed, rectBtn);
		InvalidateRect(rectBtn, FALSE);
    }
    m_nButtonPressed = TCBM_NONE;
    
    SECTabControlBase::OnLButtonUp(nFlags, point);
}

void SECTabControl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    ASSERT_VALID(this);
	
    int tab;
    int nTabHit = 0;
    BOOL bTabHit = FALSE;
    CRect rectBtn;
    int nButton;
	
    // Determine if a button was hit.
    m_nButtonPressed = TCBM_NONE;
    for (nButton = TCBM_FIRST; nButton <= TCBM_LAST; nButton <<= 1) {
		if (m_nButtons & nButton) {
			GetButtonRect(nButton, rectBtn);
			if (rectBtn.PtInRect(point)) {
				m_nButtonPressed = nButton;
				InvalidateRect(rectBtn, FALSE);
				OnButtonPress(nButton);
				return;
			}
		}	
    }
    
    // Determine if a tab was hit.
    for (tab=0, bTabHit = FALSE; tab<GetTabCount() && !bTabHit; tab++) {
		if (TabHit(tab, point)) {
			nTabHit = tab;
			bTabHit = TRUE;
		}
    }
    
    SECTabControlBase::OnLButtonDblClk(nFlags, point);
    GetParent()->SendMessage(TCM_TABDBLCLK, (WPARAM)nTabHit);
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControl diagnostics

#ifdef _DEBUG
void SECTabControl::AssertValid() const
{
    ASSERT_VALID_BUTTON_MASK(m_nButtons);
    ASSERT_VALID_BUTTON_MASK(m_nButtonPressed);
    ASSERT_VALID_BUTTON_MASK(m_nDesensedButtons);
    SECTabControlBase::AssertValid();
}

void SECTabControl::Dump(CDumpContext& dc) const
{
	SECTabControlBase::Dump(dc);

	dc << "\nTab control buttons = ";
	if (m_nButtons == TCBM_NONE)
		dc << "No buttons.";
	else {
		if (m_nButtons & TCBM_FIRST)
			dc << "[FIRST]  ";
		if (m_nButtons & TCBM_LEFT)
			dc << "[LEFT]  ";
		if (m_nButtons & TCBM_RIGHT)
			dc << "[RIGHT]  ";
		if (m_nButtons & TCBM_LAST)
			dc << "[LAST]";
	}
	dc << "\nPressed buttons = ";
	switch (m_nButtonPressed) {
	case TCBM_NONE:
		dc << "No button.";
		break;
	case TCBM_FIRST:
		dc << "[FIRST]";
		break;
	case TCBM_LEFT:
		dc << "[LEFT]";
		break;
	case TCBM_RIGHT:
		dc << "[RIGHT]";
		break;
	case TCBM_LAST:
		dc << "[LAST]";
		break;
	default:
		ASSERT(FALSE);
	}
	dc << "\nDesensed buttons = ";
	if (m_nDesensedButtons == TCBM_NONE)
		dc << "No buttons.";
	else {
		if (m_nDesensedButtons & TCBM_FIRST)
			dc << "[FIRST]  ";
		if (m_nDesensedButtons & TCBM_LEFT)
			dc << "[LEFT]  ";
		if (m_nDesensedButtons & TCBM_RIGHT)
			dc << "[RIGHT]  ";
		if (m_nDesensedButtons & TCBM_LAST)
			dc << "[LAST]";
	}
	dc << "\nButton Size = " << m_sizeBtn;
	dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////
