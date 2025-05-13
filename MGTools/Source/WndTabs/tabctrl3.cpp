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
//  Description:  Implementation of SECTab/SEC2DTabControl
//  Created:      July 20, 1996
//
//	Modified:	 Feb. 1997 (Kim Starks)
//	Description: Added tab disable feature			
//	

#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#define ROP_PSDPxax	0xB8074AL

#ifndef __TABCTRL3_H__
#include "..\..\Include\WndTabs\tabctrl3.h"
#endif //__TABCTRL3_H__

#ifndef __SECAUX_H__
#include "..\..\Include\WndTabs\secaux.h"
#endif

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

#define new DEBUG_NEW

extern SEC_AUX_DATA secData;

// Colors for Arrows and Buttons
const COLORREF rgbBlack = RGB(0,0,0);
const COLORREF rgbWhite = RGB(255,255,255);

// Must define the following as macros in case the System colors change
// JR ( 04/11/97 )
#define rgbFace (::GetSysColor(COLOR_BTNFACE))
#define rgbShadow (::GetSysColor(COLOR_BTNSHADOW))
#define rgbHilite (::GetSysColor(COLOR_BTNHIGHLIGHT))
#define rgbFrame (::GetSysColor(COLOR_WINDOWFRAME))

    
// Constants
const int nMaxTabWidth = 90;
const int nIconWidth = 16;
const int nIconHeight = 16;
const int nPadding = 3;
const int nLeftOffset = 7;
const int nTopOffset = 7;
const int nDefaultHeight = 22;
const int nDefaultWidth = 22;
const int nSizingStart = 12;

// inline for swap integer
void inline swap(long& x, long& y) { long nTmp = x; x = y; y = nTmp; }

/////////////////////////////////////////////////////////////////////////////
// SEC3DTab Construction/Destruction

IMPLEMENT_DYNAMIC(SEC3DTab, SECTab)

SEC3DTab::SEC3DTab()
{
	m_hIcon = NULL;
	m_nWidth = nMaxTabWidth;
	m_nHeight = 0;
	m_bEnabled = TRUE;
	m_bToolTip = FALSE;
}

SEC3DTab::~SEC3DTab()
{
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTab diagnostics

#ifdef _DEBUG
void SEC3DTab::AssertValid() const
{
	SECTab::AssertValid();
}

void SEC3DTab::Dump(CDumpContext& dc) const
{
	SECTab::Dump(dc);
	dc << "\nm_hIcon = " << (void*)m_hIcon;
    dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl

IMPLEMENT_DYNAMIC(SEC3DTabControl, SECTabControlBase)

BEGIN_MESSAGE_MAP(SEC3DTabControl, SECTabControlBase)
    //{{AFX_MSG_MAP(SEC3DTabControl)
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_SIZE()
    ON_WM_LBUTTONDBLCLK()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl Construction/Destruction

//@doc SEC3DTabControl
//@mfunc You construct a SEC3DTabControl object in two steps.  First call the constructor SEC3DTabControl, then call
// Create, inherited from SECTabControlBase, which initializes the Windows window and attaches it to the SEC3DTabControl 
// object.
//@xref<c SECTabControlBase>
//@xref<c SEC3DTabControl> 
SEC3DTabControl::SEC3DTabControl()
{
	m_bShowTabLabels = TRUE;	
	m_bSizing = TRUE;
#ifdef WIN32
	m_pToolTip = NULL;
#endif
	m_nDefTabHeight=nDefaultHeight;
	m_nDefTabWidth=nDefaultWidth;
	m_nLeftOffset=nLeftOffset;
	m_nTopOffset=nTopOffset;
}

SEC3DTabControl::~SEC3DTabControl()
{
#ifdef WIN32
	ASSERT(m_pToolTip != NULL);
	delete m_pToolTip;
#endif
}

void SEC3DTabControl::OnDestroy()
{
	SECTabControlBase::OnDestroy();

    DestroyGDIObjects();
}

//@doc SEC3DTabControl
//@mfunc Destroys all fonts, pens, and brushes used for tab control drawing.  Override if you are implementing your own
// fonts, pens or brushes.
//@rdesc void 
//@xref<c SEC3DTabControl>
void SEC3DTabControl::DestroyGDIObjects()
{
    m_fonts[SEC3DTAB_ACTIVE].DeleteObject();
    m_fonts[SEC3DTAB_INACTIVE].DeleteObject();
    m_brushes[SEC3DTAB_ACTIVE].DeleteObject();
    m_brushes[SEC3DTAB_INACTIVE].DeleteObject();
    m_pens[SEC3DTAB_ACTIVE].DeleteObject();
    m_pens[SEC3DTAB_INACTIVE].DeleteObject();
    m_penShadow.DeleteObject();
	m_penWhite.DeleteObject();
}
#ifdef WIN32
//@doc SEC3DTabControl
//@mfunc Modifies tab style.  Win32 only.
//@rdesc Nonzero if set to modify tab style; otherwise 0.
//@parm DWORD | dwRemove | Specifies the style attributes to remove.
//@parm  DWORD | dwAdd | Specifies the style attributes to add.
//@parm  UINT | nFlags | Indicates the style flags to set; default is 0.
//@xref<c SEC3DTabControl>
BOOL SEC3DTabControl::ModifyStyle(DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	BOOL bRet = SECTabControlBase::ModifyStyle(dwRemove, dwAdd, nFlags);
	
	if (dwAdd & TCS_TAB_ORIENTATIONS)
	{
		// adjust for latest escapement value
		int nNewEscapement=0;
		if (dwAdd & TCS_TABS_ON_LEFT)
			 nNewEscapement=900;
		else if(dwAdd & TCS_TABS_ON_RIGHT)
			nNewEscapement=-900;

		LOGFONT lf;
		// active font
		m_fonts[SEC3DTAB_ACTIVE].GetLogFont(&lf);
		lf.lfEscapement=nNewEscapement;
		m_fonts[SEC3DTAB_ACTIVE].DeleteObject();
		m_fonts[SEC3DTAB_ACTIVE].CreateFontIndirect(&lf);

		// inactive font
		m_fonts[SEC3DTAB_INACTIVE].GetLogFont(&lf);
		lf.lfEscapement=nNewEscapement;
		m_fonts[SEC3DTAB_INACTIVE].DeleteObject();
		m_fonts[SEC3DTAB_INACTIVE].CreateFontIndirect(&lf);

		RecalcLayout();
	}

	// must have at least one window style for orientation
	ASSERT(GetStyle() & TCS_TAB_ORIENTATIONS);

	return bRet;
}
#endif

//@doc SEC3DTabControl
//@mfunc Enables or disables the tab.
//@rdesc void 
//@parm int | nIndex | Index of tab.
//@parm  BOOL | bEnable | Flag indicating whether tab enabled; default is TRUE.
//@xref<mf SEC3DTabControl::IsTabEnabled>
//@xref<c SEC3DTabControl>
void SEC3DTabControl::EnableTab(int nIndex, BOOL bEnable)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	SEC3DTab* pTab = GetTabPtr(nIndex);

	if (pTab->m_bEnabled == bEnable)
		return;

	pTab->m_bEnabled = bEnable;
	InvalidateTab(nIndex);
}

//@doc SEC3DTabControl
//@mfunc Specifies whether the tab is enabled or disabled.
//@rdesc Flag indicating whether tab enabled. TRUE - Tab enabled.
//@parm int |nIndex | Index of tab.
//@xref<mf SEC3DTabControl::EnableTab>
//@xref<c SEC3DTabControl>
BOOL SEC3DTabControl::IsTabEnabled(int nIndex)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));
	SEC3DTab* pTab = GetTabPtr(nIndex);
	return pTab->m_bEnabled;
}

/////////////////////////////////////////////////////////////////////////////
// Overrides

// VC6.0 change
#if _MFC_VER < 0x0423
	extern BOOL AFXAPI AfxEndDeferRegisterClass(short fClass);
#else
	extern BOOL AFXAPI AfxEndDeferRegisterClass(long fClass);
#endif

#define SecDeferRegisterClass(fClass) \
	((afxRegisteredClasses & fClass) ? TRUE : AfxEndDeferRegisterClass(fClass))
#define AFX_WNDCOMMCTLS_REG     (0x0010)


//@doc SEC3DTabControl
//@mfunc Initializes the tab font.
//@rdesc void 
//@parm DWORD | dwStyle | Specifies style of tab font to initialize.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::InitializeFonts(DWORD dwStyle)
{
	// incase they already exist
	m_fonts[SEC3DTAB_ACTIVE].DeleteObject();
	m_fonts[SEC3DTAB_INACTIVE].DeleteObject();

	// Initialize the LOGFONT structure for font creation
	LOGFONT lf;
	lf.lfHeight = m_nDefTabHeight - 7;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;	// default, for top and bottom tabs
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = 0;
#ifdef _SEC_INTL_
	lf.lfCharSet = RUSSIAN_CHARSET;//DEFAULT_CHARSET;
#else
	lf.lfCharSet = RUSSIAN_CHARSET;//ANSI_CHARSET;
#endif
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH /*VARIABLE_PITCH*/ | FF_SWISS;
//	_tcsncpy(lf.lfFaceName, _T("Arial Narrow"), LF_FACESIZE);
	_tcsncpy(lf.lfFaceName, _T("MS Sans Serif"), LF_FACESIZE);

	// rotated fonts
	if (dwStyle & TCS_TABS_ON_LEFT)
		lf.lfEscapement = 900;
	else if (dwStyle & TCS_TABS_ON_RIGHT)
		lf.lfEscapement = -900; 
	
    // Create Fonts
	m_fonts[SEC3DTAB_INACTIVE].CreateFontIndirect(&lf);

	// Make font of active tab bold
	lf.lfWeight += 200;
	m_fonts[SEC3DTAB_ACTIVE].CreateFontIndirect(&lf);
}

//@doc SEC3DTabControl
//@mfunc Initializes tab in style specified.
//@rdesc void 
//@parm DWORD | dwStyle | Specifies tab style.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::Initialize(DWORD dwStyle)
{
	InitializeFonts(dwStyle);

	// Brushes
	m_brushes[SEC3DTAB_ACTIVE].CreateSolidBrush(::GetSysColor(COLOR_WINDOW));
	m_brushes[SEC3DTAB_INACTIVE].CreateSolidBrush(rgbFace);
	
    // Pens
	m_pens[SEC3DTAB_ACTIVE].CreatePen(PS_SOLID, 1, rgbBlack);
	m_pens[SEC3DTAB_INACTIVE].CreatePen(PS_SOLID, 1, rgbBlack);
	m_penShadow.CreatePen(PS_SOLID, 1, rgbShadow);
	m_penWhite.CreatePen(PS_SOLID, 1, rgbWhite);
	
    // Text color
	m_rgbText[SEC3DTAB_ACTIVE] = rgbBlack;
	m_rgbText[SEC3DTAB_INACTIVE] = rgbBlack;

	

	// initialize common controls
#ifdef WIN32
	if( !m_pToolTip )
	{

		VERIFY(SecDeferRegisterClass(AFX_WNDCOMMCTLS_REG));

		// Create and activate the tooltip control
		m_pToolTip = new CToolTipCtrl;
		if (!m_pToolTip->Create(this))
		{
			TRACE(_T("Unable to create ToolTip Control.\n"));
			delete m_pToolTip;
			m_pToolTip = NULL;
			return;
		}
		m_pToolTip->Activate(TRUE);
		EnableToolTips(TRUE);
	}
#endif
}



//@doc SEC3DTabControl
//@mfunc Creates a new tab.
//@rdesc Returns a pointer to the new SECTab object.
SECTab* SEC3DTabControl::CreateNewTab() const
{

	return new SEC3DTab();
	
}


//@doc SEC3DTabControl
//@mfunc Call this member to enable or disable the tab sizing feature. 
//@parm BOOL | bVal | A FALSE value for 'bVal' will disable the tab sizing. Default is set to TRUE.
//@rdesc void
void SEC3DTabControl::EnableTabSizing(BOOL bVal)
{
	m_bSizing = bVal;
	for(int nTab = 0; nTab < GetTabCount(); nTab++) 
	{
		SEC3DTab* pTab = GetTabPtr(nTab);
		pTab->m_csLabel = pTab->m_csLong;
	}
}


//override needed to update the m_csLong member of SEC3DTab
void SEC3DTabControl::RenameTab(int nIndex, LPCTSTR lpszLabel)
{
	ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	//update the m_csLong member
	SEC3DTab* pTab = GetTabPtr(nIndex);
	pTab->m_csLong = lpszLabel;
	
	//invoke the base class
	SECTabControlBase::RenameTab(nIndex, lpszLabel);
}


//@doc SEC3DTabControl
//@mfunc Retrieves the width of the tab label.
//@rdesc The width value for the tab label.
//@parm int | nTab | Index of tab.
//@parm BOOL | bLong | If TRUE, retrieves the full label width. Else, uses the current display width. Default is FALSE.
//@xref<c SEC3DTabControl>
int SEC3DTabControl::GetLabelWidth(int nTab, BOOL bLong)
{
	// NOTE: this methods works for both WITHOUT and WITH escapement
    ASSERT_VALID(this);

	SEC3DTab* pTab = GetTabPtr(nTab);

    ASSERT(AfxIsValidString(pTab->m_csLong));
	ASSERT(pTab->IsKindOf(RUNTIME_CLASS(SEC3DTab)));

    CClientDC dc(this);

    ASSERT_VALID(&dc);

    CFont *pOldFont;
    CSize labelExtent;

    // Select the appropriate font and compute size of label (in pixels)
    ASSERT_VALID(&m_fonts[SEC3DTAB_ACTIVE]);
    pOldFont = dc.SelectObject(&m_fonts[SEC3DTAB_ACTIVE]);
#ifdef WIN32
	if(bLong)
		labelExtent = dc.GetTextExtent(pTab->m_csLong, _tcslen(pTab->m_csLong));
	else
		labelExtent = dc.GetTextExtent(pTab->m_csLabel, _tcslen(pTab->m_csLabel));
#else
	if(bLong)
	    labelExtent = dc.GetTextExtent(pTab->m_csLong, _fstrlen(pTab->m_csLong));
	else
		labelExtent = dc.GetTextExtent(pTab->m_csLabel, _fstrlen(pTab->m_csLabel));
#endif
    dc.SelectObject(pOldFont);

	return labelExtent.cx;
}

/////////////////////////////////////////////////////////////////////////////
// Queries

//@doc SEC3DTabControl
//@mfunc If you need to read or write tab attributes directly, you can retrieve a pointer to the SECTab-derived object and
// dereference its members at will.  You may prefer to use this member instead of GetTab if you need to do a type-safe 
// cast of the tab object to one of the SECTab-derived classes.
//@rdesc A pointer to the specified tab.
//@parm int | nTab | Index of tab to return a pointer to.
//@xref<mf SEC3DTabControl::GetTab>
//@xref<c SEC3DTabControl>
SEC3DTab* SEC3DTabControl::GetTabPtr(int nTab) const
{
    ASSERT(TabExists(nTab));
	ASSERT((SECTab*)m_pTabArray->GetAt(nTab) != NULL);
	ASSERT(m_pTabArray->GetAt(nTab)->IsKindOf(RUNTIME_CLASS(SEC3DTab)));
	return (SEC3DTab*)m_pTabArray->GetAt(nTab);
}


//Virtual overridden to facilitate direct tab control creation
SECTab* SEC3DTabControl::AddTab(LPCTSTR lpszLabel,
							  CObject* pClient,
							  HMENU hMenu,
							  void* pExtra)
{	
	ASSERT_VALID(this);
	
	SECTab* pTab = SECTabControlBase::AddTab(lpszLabel, pClient, hMenu, pExtra);
	SEC3DTab* p3Tab = (SEC3DTab*)pTab;	
    p3Tab->m_csLong = lpszLabel;

	CRect rectClient;
	GetClientRect(&rectClient);
 
	return pTab;
}


//Virtual overridden to allow tab resizing upon insertion of new tab
SECTab* SEC3DTabControl::InsertTab(int nIndex,
								 LPCTSTR lpszLabel,
								 CObject* pClient,
								 HMENU hMenu,
								 void* pExtra)
{
    ASSERT_VALID(this);
    
	SECTab* pTab = SECTabControlBase::InsertTab(nIndex, lpszLabel, pClient, hMenu, pExtra);
	((SEC3DTab*)pTab)->m_csLong = lpszLabel;

	RecalcLayout();

	// Invalidate the tabs to the right of the new one
	int i;
	for (i=nIndex; i<GetTabCount(); i++)
		InvalidateTab(i);

	CRect rectClient;
	GetClientRect(&rectClient);

	return pTab;
}


//@doc SEC3DTabControl
//@mfunc If you need to read or write tab attributes directly, you can retrieve a reference to the SECTab-derived object
// and dereference its members at will.  
//@rdesc A reference to the specified tab.
//@parm int | nTab | Index of tab to return.
//@xref<mf SEC3DTabControl::GetTabPtr>
//@xref<c SEC3DTabControl>
SEC3DTab& SEC3DTabControl::GetTab(int nTab)
{
	SEC3DTab* pTab = GetTabPtr(nTab);
    return *pTab;
}

//@doc SEC3DTabControl
//@mfunc Gets the tab rectangle.
//@rdesc void 
//@parm int | nTab | Index of tab.
//@parm  CRect & | rcTab | References the tab rectangle.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::GetTabRect(int nTab, CRect &rcTab) const
{
	SEC3DTab* pTab = GetTabPtr(nTab);
	rcTab = pTab->m_rect;
}

//@doc SEC3DTabControl
//@mfunc Indicates whether a point is within the tab rectangle.
//@rdesc Nonzero if the given point is within the tab rectangle; otherwise 0.
//@parm int | nTab | Index of the tab.
//@parm  CPoint& | point | A point in the tab rectangle.
//@xref<mf SEC3DTabControl::GetTabPtr>
//@xref<c SEC3DTabControl>
BOOL SEC3DTabControl::TabHit(int nTab, CPoint& point) const
{
    CRect tabRect;
	BOOL bHit = FALSE;
	SEC3DTab* pTab = GetTabPtr(nTab);

	GetTabRect(nTab, tabRect);
	if (tabRect.PtInRect(point) && pTab->m_bEnabled)
		bHit = TRUE;

	return bHit;
}

/////////////////////////////////////////////////////////////////////////////
// Operations

BOOL SEC3DTabControl::SetFontTab(CFont* pFont,int nTabType,BOOL bRedraw) {

#ifndef WIN32
	// this function not supported for WIN16!
	pFont;
	nTabType;
	bRedraw;
	return FALSE;
#else	// WIN32

	if(!pFont) return FALSE;

	ASSERT((nTabType==SEC3DTAB_ACTIVE)||(nTabType==SEC3DTAB_INACTIVE));
	if((nTabType!=SEC3DTAB_ACTIVE)&&(nTabType!=SEC3DTAB_INACTIVE)) 
		return FALSE;

	// account for escapement, in case left or right orientation
	DWORD dwStyle=GetStyle();
	int nEscapement=0;
	if (dwStyle & TCS_TABS_ON_LEFT)
		nEscapement=900;
	else if(dwStyle & TCS_TABS_ON_RIGHT)
		nEscapement=-900;

	// clone font passed in as new active/inactive tab font
	// Note: cloning used instead of straight ptr manipulation
	// to preserve backward compatibility.
	LOGFONT lfNew,lfOld;
	if(!pFont->GetLogFont(&lfNew)) return FALSE;
	if(!m_fonts[nTabType].GetLogFont(&lfOld)) return FALSE;
	m_fonts[nTabType].DeleteObject();
	lfNew.lfEscapement=nEscapement;
	if(!m_fonts[nTabType].CreateFontIndirect(&lfNew)) {
		// if failure, revert to old font
		m_fonts[nTabType].CreateFontIndirect(&lfOld);
		return FALSE;
		}

	// adjust tab size as necessary for new font size
	RecalcLayout();

	// should we invalidate?
	if(bRedraw) {
		if(nTabType==SEC3DTAB_ACTIVE) {
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

//@doc SEC3DTabControl
//@mfunc Invalidates tab for painting.
//@rdesc void 
//@parm int | nTab | Index of the tab to be invalidated.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::InvalidateTab(int nTab)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nTab));

    CRect rcTab;
	GetTabRect(nTab, rcTab);
	rcTab.right += 1;
	rcTab.bottom += 1;
	InvalidateRect(&rcTab, TRUE);
}

//@doc SEC3DTabControl
//@mfunc Sets the tab icon.
//@syntax SetTabIcon(int nIndex, HICON hIcon)
//@syntax SetTabIcon(int nIndex, UINT nIDIcon, int cx, int cy)
//@syntax SetTabIcon(int nIndex, LPCTSTR lpszIconName, int cx, int cy)
//@rdesc void 
//@parm int | nIndex | Index of tab icon.
//@parm  HICON | hIcon | Handle of icon.
//@parm UINT | nIDIcon | ID of icon.
//@parm LPCTSTR | lpszIconName | Icon resource name.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::SetTabIcon(int nIndex, HICON hIcon)
{
	SEC3DTab* pTab = GetTabPtr(nIndex);
	pTab->m_hIcon = hIcon;
	RecalcLayout();

	// Invalidate the tabs to the right of the new one
	for (int i=nIndex; i<GetTabCount(); i++)
		InvalidateTab(i);
}

void SEC3DTabControl::SetTabIcon(int nIndex, UINT nIDIcon, int cx, int cy)
{
#ifdef WIN32
	// Use LoadImage to properly load icon resource of choice (i.e. 16x16)
	HINSTANCE hInstIcon=AfxFindResourceHandle(MAKEINTRESOURCE(nIDIcon),RT_GROUP_ICON);
	HICON hIcon = (HICON)::LoadImage(hInstIcon,
						MAKEINTRESOURCE(nIDIcon),IMAGE_ICON,
						cx,cy,LR_DEFAULTCOLOR);
#else
	cx; cy;	// unreferenced
	HICON hIcon = LoadIcon(AfxGetResourceHandle(),MAKEINTRESOURCE(nIDIcon));
#endif

	SetTabIcon(nIndex, hIcon);
}

void SEC3DTabControl::SetTabIcon(int nIndex, LPCTSTR lpszIconName, int cx, int cy)
{
#ifdef WIN32
	// Use LoadImage to properly load icon resource of choice (i.e. 16x16)
	HINSTANCE hInstIcon=AfxFindResourceHandle(lpszIconName,RT_GROUP_ICON);
	HICON hIcon = (HICON)::LoadImage(hInstIcon,
						lpszIconName,IMAGE_ICON,
						cx,cy,LR_DEFAULTCOLOR);
#else
	cx; cy; // unreferenced
	HICON hIcon = LoadIcon(AfxGetResourceHandle(),lpszIconName);
#endif

	SetTabIcon(nIndex, hIcon);
}

//@doc SEC3DTabControl
//@mfunc Call this function when the specified tab is being activated or deactivated.
//@rdesc void 
//@parm int | nTab | Index of tab.
//@xref<c SEC3DTabControl>
void SEC3DTabControl::OnActivateTab(int nTab)
{
	nTab; // UNUSED
	RecalcLayout();	
	
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl layout

void SEC3DTabControl::SetTabPos(int nTab, int x, int y, DWORD dwStyle)
{
    ASSERT(TabExists(nTab));

	SEC3DTab* pTab = GetTabPtr(nTab);

	if (dwStyle & (TCS_TABS_ON_TOP|TCS_TABS_ON_BOTTOM))
	{
		// Going from left to right
		pTab->m_nWidth = nPadding*2;
		if (pTab->m_hIcon)
			pTab->m_nWidth += nIconWidth + nPadding;
		if (m_bShowTabLabels)
			pTab->m_nWidth += GetLabelWidth(nTab) + nPadding*2;
		pTab->m_nHeight = m_nDefTabHeight;

		// subtract one from width to avoid overpainting
		pTab->m_rect.SetRect(x, y, x+pTab->m_nWidth, y+pTab->m_nHeight);
	}
	else // (dwStyle & (TCS_TABS_ON_RIGHT|TCS_TABS_ON_LEFT))
	{
		// Going from top to bottom
		pTab->m_nHeight = nPadding*2;
		if (pTab->m_hIcon)
			pTab->m_nHeight += nIconWidth + nPadding;
		if (m_bShowTabLabels)
			pTab->m_nHeight += GetLabelWidth(nTab) + nPadding*2;
		pTab->m_nWidth = m_nDefTabWidth;

		// subtract one from height to avoid overpainting
		pTab->m_rect.SetRect(x, y, x+pTab->m_nWidth, y+pTab->m_nHeight);
	}
}

//@doc SEC3DTabControl
//@mfunc Recalculates the tab layout.
//@rdesc void 
//@xref<c SEC3DTabControl>
void SEC3DTabControl::RecalcLayout()
{
	SEC3DTab* pTab;
	CRect rectClient;
	int x, y;
	int nTab;
	DWORD dwStyle = GetStyle();

    GetClientRect(&rectClient);	
		
	// Determine if all tabs would fit in client with text labels
	int nTotal = 0;
	int nTotIcon = 0;
	CalcLabelWidth(nTotal, nTotIcon, TRUE);

	if (dwStyle & (TCS_TABS_ON_TOP|TCS_TABS_ON_BOTTOM))
	{	
		//if m_bSizing is FALSE, do not size tabs.
		if(m_bSizing)
			SizeTabs(nTotal, nTotIcon, rectClient.Width());
		else
			m_bShowTabLabels = rectClient.Width() > nTotal+nSizingStart;
		
		// Now, position and size the tabs individually
		x = rectClient.left + m_nLeftOffset;
		if (dwStyle & TCS_TABS_ON_BOTTOM)
			y = rectClient.top + 2;
		else			
			y = rectClient.bottom - (m_nDefTabHeight + 3);

		for (nTab = 0; nTab < GetTabCount(); nTab++)
		{
			pTab = GetTabPtr(nTab);
			SetTabPos(nTab, x, y, dwStyle);
			x += pTab->m_nWidth;
		}

		// If the tab is active, make it appear to stand out
		if ((m_bActiveTab)&&(!(dwStyle & TWS_NOACTIVE_TAB_ENLARGED)))
		{
			pTab = GetTabPtr(m_nActiveTab);
			pTab->m_rect.left -= 3;
			pTab->m_rect.right += 3;
			if (dwStyle & TCS_TABS_ON_BOTTOM)
				pTab->m_rect.bottom += 3;
			else
				pTab->m_rect.top -= 3;
		}
	
	}
	else // (dwStyle & (TCS_TABS_ON_LEFT|TCS_TABS_ON_RIGHT))
	{
		//Size tabs only if m_bSizing is TRUE
		if(m_bSizing)
			SizeTabs(nTotal, nTotIcon, rectClient.Height());
		else
			m_bShowTabLabels = rectClient.Height() > nTotal+nSizingStart;

		if (dwStyle & TCS_TABS_ON_LEFT)
			x = rectClient.right - (m_nDefTabWidth + 3);
		else
			x = rectClient.left + 2;
		y = rectClient.top + m_nTopOffset;

		for (nTab = 0; nTab < GetTabCount(); nTab++)
		{
			pTab = GetTabPtr(nTab);
			SetTabPos(nTab, x, y, dwStyle);
			y += pTab->m_nHeight;
		}

		// If the tab is active, make it appear to stand out
		if ((m_bActiveTab)&&(!(dwStyle & TWS_NOACTIVE_TAB_ENLARGED)))
		{
			pTab = GetTabPtr(m_nActiveTab);
			pTab->m_rect.top -= 3;
			pTab->m_rect.bottom += 3;
			if (dwStyle & TCS_TABS_ON_LEFT)
				pTab->m_rect.left -= 3;
			else
				pTab->m_rect.right += 3;
		}
	}
}

void SEC3DTabControl::SizeTabs(int nTotal, int nTotIcon, int nDimParam)
{	   	
	int nTab =  0;
	SEC3DTab* pTabCal;
	BOOL bSz1 = TRUE;
	//determine sizing method to use
	for(nTab = 0; nTab < GetTabCount(); nTab++)
	{
		pTabCal = GetTabPtr(nTab);
		if(_tcslen(pTabCal->m_csLong) >= _tcslen(_T("...")))
			pTabCal->m_csLabel = _T("...");
	}

	int nTotalLoop = 0;
	int nTotIconLoop = 0;
	CalcLabelWidth(nTotalLoop, nTotIconLoop, FALSE);

	if(nDimParam < (nTotalLoop+nSizingStart))
		bSz1 = FALSE;
	else
	{
		//reset tab labels and use primary sizing
		for(nTab = 0; nTab < GetTabCount(); nTab++)
		{
			pTabCal = GetTabPtr(nTab);
			pTabCal->m_csLabel = pTabCal->m_csLong;
			//reset tooltips
			pTabCal->m_bToolTip = FALSE;
		}
	}

	if(nDimParam > nTotIcon)
		m_bShowTabLabels = TRUE;
	else
		m_bShowTabLabels = FALSE;

	//primary sizing
	while((nDimParam > 0) && (nTotal+nSizingStart > nDimParam) && bSz1)
	{
		for(int nTab = 0; nTab < GetTabCount(); nTab++)
		{
			SEC3DTab* pTab = GetTabPtr(nTab);
			if(_tcslen(pTab->m_csLabel) >= _tcslen(_T("...")) && (pTab->m_csLabel != _T("...")) )
				bSz1 = TRUE;
			else
				bSz1 = FALSE;
		}
		
		//truncate label and recheck
		SEC3DTab* pTabBig = GetTabPtr(GetTabBig());
		CString strTemp = pTabBig->m_csLabel; 

		if(pTabBig->m_csLabel == pTabBig->m_csLong)
		{
			if(strTemp.Find(_T("...")))
				pTabBig->m_csLabel = strTemp.Mid(0, _tcslen(strTemp)-4);
			else
				pTabBig->m_csLabel = strTemp.Mid(0, _tcslen(strTemp)-3);
		}
		else
		//Remove ellipsis & truncate
		{
			int cutoff = strTemp.Find(_T("..."));
			if(cutoff > 0)
				pTabBig->m_csLabel = strTemp.Left(_tcslen(strTemp) - 3); 
			
			pTabBig->m_csLabel = pTabBig->m_csLabel.Mid(0, _tcslen(pTabBig->m_csLabel)-1);
		}

		if(_tcslen(pTabBig->m_csLong) >= _tcslen(_T("...")))
			pTabBig->m_csLabel += _T("...");
		//set tooltip to be displayed
		pTabBig->m_bToolTip = TRUE;

		int nTotRecal = 0;
		int nTotIconRecal = 0;
		CalcLabelWidth(nTotRecal, nTotIconRecal, FALSE);

		nTotal = nTotRecal;
	}

	//secondary sizing
	while((nDimParam>0) && (nTotal+nSizingStart>nDimParam) && m_bShowTabLabels && (!bSz1))
	{
		for(int nTab = 0; nTab < GetTabCount(); nTab++)
		{
			SEC3DTab* pTab = GetTabPtr(nTab);
			pTab->m_bToolTip = TRUE;
			if(pTab->m_csLabel != _T(""))
				m_bShowTabLabels = TRUE;
			else
				m_bShowTabLabels = FALSE;
		}

		SEC3DTab* pTabBig = GetTabPtr(GetTabBig());
		CString strTemp = pTabBig->m_csLabel; 

		pTabBig->m_csLabel = strTemp.Mid(0, _tcslen(strTemp)-1);

		int nTotRecal = 0;
		int nTotIconRecal = 0;
		CalcLabelWidth(nTotRecal, nTotIconRecal, FALSE);

		nTotal = nTotRecal;
	}
}


void SEC3DTabControl::CalcLabelWidth(int& nTotBuff, int& nTotIconBuff, BOOL bLong)
{
	//reset values
	nTotBuff = 0;
	nTotIconBuff = 0;
	SEC3DTab* pTab;
	for (int nTab = 0; nTab < GetTabCount(); nTab++)
	{
		pTab = GetTabPtr(nTab);
		nTotBuff += nPadding*2;
		if (pTab->m_hIcon)
			nTotBuff += nIconWidth + nPadding;
		    nTotIconBuff += nIconWidth + nPadding;
		if(bLong)
			nTotBuff += GetLabelWidth(nTab, TRUE) + nPadding*2;
		else
			nTotBuff += GetLabelWidth(nTab) + nPadding*2;
	}
}

	
int SEC3DTabControl::GetTabBig()
{
	int nTabBig = 0;
	int nLenBig = GetLabelWidth(0);
	int nLenTemp;
	for(int nTab = 1; nTab < GetTabCount(); nTab++)
	{
		nLenTemp = GetLabelWidth(nTab);
		if(nLenTemp > nLenBig)
		{
			nLenBig = nLenTemp;
			nTabBig = nTab;
		}
	}

	return nTabBig;
}

void SEC3DTabControl::OnSize(UINT nType, int cx, int cy) 
{
	RecalcLayout();		
	SECTabControlBase::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl drawing

void SEC3DTabControl::OnPaint() 
{
    ASSERT_VALID(this);

    CPaintDC dcPaint(this); // device context for painting
    CRect rectInvalid;
	CRect rectClient;
    CRect intersection;
    CRect rcTab;
    int nTab;
    CRect btnRect, intersect;

	dcPaint.GetClipBox(&rectInvalid);
	GetClientRect(rectClient);
    
	// Create a memory DC to do operations in, eliminating flicker
	CDC dc;
	dc.CreateCompatibleDC(&dcPaint);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(&dcPaint, rectClient.Width(), rectClient.Height());
	CBitmap* pBmpOld = dc.SelectObject(&bmpMem);

    int oldBkMode = dc.SetBkMode(TRANSPARENT);
    
    // Draw unselected tabs
    COLORREF oldTextColor = dc.SetTextColor(m_rgbText[SEC3DTAB_INACTIVE]);
    CBrush *pOldBrush = dc.SelectObject(&m_brushes[SEC3DTAB_INACTIVE]);
    CFont *pOldFont = dc.SelectObject(&m_fonts[SEC3DTAB_INACTIVE]);
    CPen *pOldPen = dc.SelectObject(&m_pens[SEC3DTAB_INACTIVE]);

	// fill in the background with the default color
	rgbFill(&dc, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), rgbFace);

	for (nTab = 0; nTab < GetTabCount(); nTab++) 
	    if (!m_bActiveTab || nTab != m_nActiveTab)
			DrawTab(&dc, nTab);

	// Draw the 3D border above the tabs
	int nWidth = rectClient.Width();
	int nHeight = rectClient.Height();

	DWORD dwStyle = GetStyle();
	if (dwStyle & TCS_TABS_ON_BOTTOM)
	{
		rgbFill(&dc, rectClient.left, rectClient.top,   nWidth, 1, rgbWhite);
		rgbFill(&dc, rectClient.left, rectClient.top+1, nWidth, 1, rgbFace);
		rgbFill(&dc, rectClient.left, rectClient.top+2, nWidth, 1, rgbShadow);
		rgbFill(&dc, rectClient.left, rectClient.top+3, nWidth, 1, rgbBlack);
	}
	else if (dwStyle & TCS_TABS_ON_TOP)
	{
		rgbFill(&dc, rectClient.left, rectClient.bottom-4, nWidth, 1, rgbWhite);
		rgbFill(&dc, rectClient.left, rectClient.bottom-3, nWidth, 1, rgbFace);
		rgbFill(&dc, rectClient.left, rectClient.bottom-2, nWidth, 1, rgbShadow);
		rgbFill(&dc, rectClient.left, rectClient.bottom-1, nWidth, 1, rgbBlack);
	}
	else if (dwStyle & TCS_TABS_ON_LEFT)
	{
		rgbFill(&dc, rectClient.right-4, rectClient.top, 1, nHeight, rgbWhite);
		rgbFill(&dc, rectClient.right-3, rectClient.top, 1, nHeight, rgbFace);
		rgbFill(&dc, rectClient.right-2, rectClient.top, 1, nHeight, rgbShadow);
		rgbFill(&dc, rectClient.right-1, rectClient.top, 1, nHeight, rgbBlack);
	}
	else if (dwStyle & TCS_TABS_ON_RIGHT)
	{
		rgbFill(&dc, rectClient.left,   rectClient.top, 1, nHeight, rgbWhite);
		rgbFill(&dc, rectClient.left+1, rectClient.top, 1, nHeight, rgbFace);
		rgbFill(&dc, rectClient.left+2, rectClient.top, 1, nHeight, rgbShadow);
		rgbFill(&dc, rectClient.left+3, rectClient.top, 1, nHeight, rgbBlack);
	}

    // Draw the active tab last so it appears on top
	// of non-active tabs and the 3D border.
	if (GetTabCount() > 0 && m_bActiveTab) 
	{
	    CRect btnArea;
	    GetTabRect(m_nActiveTab, rcTab);
	    if (rcTab.IntersectRect(rcTab, rectInvalid))
		{
			dc.SetTextColor(m_rgbText[SEC3DTAB_ACTIVE]);
			dc.SelectObject(&m_brushes[SEC3DTAB_ACTIVE]);
			dc.SelectObject(&m_brushes[SEC3DTAB_INACTIVE]);
			dc.SelectObject(&m_fonts[SEC3DTAB_ACTIVE]);
			dc.SelectObject(&m_pens[SEC3DTAB_ACTIVE]);
			DrawTab(&dc, m_nActiveTab);
		}
    }

    // Cleanup
    dc.SetBkMode(oldBkMode);
    dc.SelectObject(pOldFont);
    dc.SelectObject(pOldPen);
    dc.SelectObject(pOldBrush);
    dc.SetTextColor(oldTextColor);

	// blit the resulting image back into the paint device context
	dcPaint.BitBlt(rectClient.left, rectClient.top,
				   rectClient.Width(), rectClient.Height(), 
				   &dc, 0, 0, SRCCOPY);

	dc.SelectObject(pBmpOld);
}


void SEC3DTabControl::DrawTab(CDC* pDC, int nTab) 
{
	SEC3DTab* pTab = GetTabPtr(nTab);
	CRect rcTab;

    // Fill tab	using currently selected brush
	GetTabRect(nTab, rcTab);
#ifdef WIN32
	pDC->FillSolidRect(&rcTab, rgbFace);
#else
	CBrush brgbFace(rgbFace);
    pDC->FillRect(&rcTab,&brgbFace);
#endif
	DWORD dwStyle = GetStyle();

	if (dwStyle & TCS_TABS_ON_TOP)
		DrawTabTop(pDC, pTab, TCS_TABS_ON_TOP);
	else if (dwStyle & TCS_TABS_ON_RIGHT)
		DrawTabRight(pDC, pTab, TCS_TABS_ON_RIGHT);
	else if (dwStyle & TCS_TABS_ON_LEFT)
		DrawTabLeft(pDC, pTab, TCS_TABS_ON_LEFT);
	else
		// the old default tabs on bottom
		DrawTabBottom(pDC, pTab, TCS_TABS_ON_BOTTOM);
}


void SEC3DTabControl::DrawDisabled(CDC* pDC, int nWidth, int nHeight)
{
	ASSERT_VALID(this);

	// We take the drawn button bitmap and effectively gray it and add a shadow.
	CDC monoDC;
	CBitmap bmp;

	if(!monoDC.CreateCompatibleDC(pDC) ||
	   !bmp.CreateBitmap(nWidth, nHeight, 1, 1, NULL))
		return;

	// Initialise to 0.
	CBitmap* pOldBmp = monoDC.SelectObject(&bmp);
	monoDC.PatBlt(0, 0, nWidth, nHeight, WHITENESS);
	
	// Create mask based on the button bitmap.
	pDC->SetBkColor(secData.clrBtnFace);
	monoDC.BitBlt(0, 0, nWidth-3, nHeight-3, pDC, 1, 1, SRCCOPY);

	pDC->SetBkColor(secData.clrBtnHilite);
	monoDC.BitBlt(0, 0, nWidth-3, nHeight-3, pDC, 1, 1, SRCPAINT);

	pDC->SetBkColor(RGB(255,255,255));
	pDC->SetTextColor(0L);

	CBrush btnHighLight, btnShadow;
	btnHighLight.CreateSolidBrush(secData.clrBtnHilite);
	btnShadow.CreateSolidBrush(secData.clrBtnShadow);

	// Draw high light shadow where we have zeros in our mask.
	CBrush* pBrOld = pDC->SelectObject(&btnHighLight);
	pDC->BitBlt(2,2,nWidth-4, nHeight-4, &monoDC, 0, 0, ROP_PSDPxax);

	// Draw the shadow where we have zeros in our mask.
	pDC->SelectObject(&btnShadow);
	pDC->BitBlt(1,1,nWidth-3, nHeight-3, &monoDC, 0, 0, ROP_PSDPxax);

	pDC->SelectObject(pBrOld);
	monoDC.SelectObject(pOldBmp);
}


void SEC3DTabControl::DrawTabBottom(CDC* pDC, SEC3DTab* pTab, DWORD dwStyle)
{
	CRect rectItem = pTab->m_rect;

	CPen* pOldPen = pDC->SelectObject(&m_penWhite);

	pDC->MoveTo(rectItem.left, rectItem.top + 1);
	pDC->LineTo(rectItem.left, rectItem.bottom - 2);

	pDC->SelectObject(&m_penShadow);

	pDC->MoveTo(rectItem.left+1,  rectItem.bottom-2);
	pDC->LineTo(rectItem.left+1, rectItem.bottom-1);
	pDC->LineTo(rectItem.right-2, rectItem.bottom-1);

		// Continue to draw shadow line down right side
		pDC->LineTo(rectItem.right-2, rectItem.bottom-2);
		pDC->LineTo(rectItem.right-1, rectItem.bottom-2);
		pDC->LineTo(rectItem.right-1, rectItem.top);
		pDC->LineTo(rectItem.right, rectItem.top);

		// Draw black line down right side and bottom
		pDC->SelectObject(pOldPen);

		pDC->MoveTo(rectItem.right, rectItem.top+2);
		pDC->LineTo(rectItem.right, rectItem.bottom-2);
		pDC->LineTo(rectItem.right-2, rectItem.bottom);

	// Draw black line across bottom
	pDC->LineTo(rectItem.left + 2, rectItem.bottom);

	DrawInterior(pDC, pTab, dwStyle);
}


void SEC3DTabControl::DrawTabTop(CDC* pDC, SEC3DTab* pTab, DWORD dwStyle)
{
	CRect rectItem = pTab->m_rect;

	CPen* pOldPen = pDC->SelectObject(&m_penWhite);

	pDC->MoveTo(rectItem.left, rectItem.bottom - 1);
	pDC->LineTo(rectItem.left, rectItem.top + 2);
	pDC->LineTo(rectItem.left + 2, rectItem.top);
	pDC->LineTo(rectItem.right - 2, rectItem.top);

	pDC->SelectObject(&m_penShadow);

		// Draw dark gray line down right side
		pDC->MoveTo(rectItem.right-2, rectItem.top+1);
		pDC->LineTo(rectItem.right-1, rectItem.top+2);
		pDC->LineTo(rectItem.right-1, rectItem.bottom);

		// Draw black line down right side
		pDC->SelectObject(pOldPen);
		pDC->MoveTo(rectItem.right-1, rectItem.top+1);
		pDC->LineTo(rectItem.right,   rectItem.top+2);
		pDC->LineTo(rectItem.right,   rectItem.bottom);

	DrawInterior(pDC, pTab, dwStyle);
}


void SEC3DTabControl::DrawTabLeft(CDC* pDC, SEC3DTab* pTab, DWORD dwStyle)
{
	CRect rectItem = pTab->m_rect;

	CPen* pOldPen = pDC->SelectObject(&m_penShadow);

	pDC->MoveTo(rectItem.left+1, rectItem.bottom-2);
	pDC->LineTo(rectItem.left+2, rectItem.bottom-1);
	pDC->LineTo(rectItem.right, rectItem.bottom-1);

	pDC->SelectObject(&m_penWhite);
	pDC->MoveTo(rectItem.left, rectItem.bottom-2);
	pDC->LineTo(rectItem.left, rectItem.top+2);

		pDC->LineTo(rectItem.left+2, rectItem.top);
		pDC->LineTo(rectItem.right, rectItem.top);

	pDC->SelectObject(pOldPen);
	pDC->MoveTo(rectItem.left+1, rectItem.bottom-1);
	pDC->LineTo(rectItem.left+2, rectItem.bottom);
	pDC->LineTo(rectItem.right, rectItem.bottom);

	DrawInterior(pDC, pTab, dwStyle);
}


void SEC3DTabControl::DrawTabRight(CDC* pDC, SEC3DTab* pTab, DWORD dwStyle)
{
	CRect rectItem = pTab->m_rect;

	CPen* pOldPen = pDC->SelectObject(&m_penWhite);

	pDC->MoveTo(rectItem.TopLeft());
	pDC->LineTo(rectItem.right-2, rectItem.top);

	pDC->SelectObject(&m_penShadow);
	pDC->LineTo(rectItem.right-1, rectItem.top+1);
	pDC->LineTo(rectItem.right-1, rectItem.bottom-2);

	pDC->SelectObject(pOldPen);
	pDC->MoveTo(rectItem.right, rectItem.top+2);
	pDC->LineTo(rectItem.right, rectItem.bottom-2);

		pDC->SelectObject(&m_penShadow);
		pDC->MoveTo(rectItem.right-1, rectItem.bottom-2);
		pDC->LineTo(rectItem.right-2, rectItem.bottom-1);
		pDC->LineTo(rectItem.left, rectItem.bottom-1);

		pDC->SelectObject(pOldPen);
		pDC->MoveTo(rectItem.right, rectItem.bottom-2);
		pDC->LineTo(rectItem.right-2, rectItem.bottom);
		pDC->LineTo(rectItem.left, rectItem.bottom);

	DrawInterior(pDC, pTab, dwStyle);
}


void SEC3DTabControl::DrawInterior(CDC* pDC, SEC3DTab* pTab, DWORD dwStyle)
{ 
	CRect	rectItem = pTab->m_rect;
	CSize	size;
	int		x = 0;
	int		y = 0;
	CPoint	pt;
	CPoint	pt2;

	if (m_bShowTabLabels)
#ifdef WIN32
		size = pDC->GetTextExtent(pTab->m_csLabel,_tcslen(pTab->m_csLabel));
#else
		size = pDC->GetTextExtent(pTab->m_csLabel,_fstrlen(pTab->m_csLabel));
#endif
	else
		size = CSize(0,0);

	if (dwStyle & (TCS_TABS_ON_LEFT|TCS_TABS_ON_RIGHT))
		// switch the x and y coordinates for sizes, the extent is always x relative
		swap((LONG &)size.cx, (LONG &)size.cy);

	if (dwStyle & (TCS_TABS_ON_TOP|TCS_TABS_ON_BOTTOM))
	{
		// calculate the top left point for drawing text and the bitmap
		pt.x = rectItem.left + nPadding*2;
		pt.y = rectItem.top + rectItem.Height()/2 - size.cy/2;
		pt2.x = rectItem.left + nPadding*2;
		pt2.y = rectItem.top + rectItem.Height()/2 - size.cy/2;
	}
	else if (dwStyle & TCS_TABS_ON_LEFT)
	{
		// calculate the bottom left point for drawing text and the bitmap
		// for the bottom left coordinate, required by ExtTextOut()
		pt.x = rectItem.left + rectItem.Width()/2 - size.cx/2;
		pt.y = rectItem.bottom - nPadding*2;
		pt2.x = rectItem.left + rectItem.Width()/2 - size.cx/2;
		pt2.y = rectItem.bottom - nPadding*2 - size.cy;
	}
	else // (dwStyle & TCS_TABS_ON_RIGHT)
	{
		// calculate the top right point for drawing text and the bitmap
		pt.x = rectItem.left + rectItem.Width()/2 + size.cx/2;
		pt.y = rectItem.top + nPadding*2;
		pt2.x = rectItem.left + rectItem.Width()/2 - size.cx/2;
		pt2.y = rectItem.top + nPadding*2;
	}


	if (pTab->m_hIcon)
	{
		if (dwStyle & (TCS_TABS_ON_TOP|TCS_TABS_ON_BOTTOM))
		{
			x = pt.x;
			y = rectItem.top + rectItem.Height()/2 - nIconWidth/2;
			// move over to suit bitmap
			pt.x += nIconWidth + nPadding;
			pt2.x += nIconWidth + nPadding;
		}
		else if (dwStyle & TCS_TABS_ON_LEFT)
		{
			x = rectItem.left + rectItem.Width()/2 - nIconWidth/2;
			pt.y -= nIconWidth;
			pt2.y -= nIconWidth;
			y = pt.y;
			pt.y -= nPadding;
			pt2.y -= nPadding;
		}
		else // (dwStyle & TCS_TABS_ON_RIGHT)
		{
			x = rectItem.left + rectItem.Width()/2 - nIconWidth/2;
			y = pt.y;
			// move over to suit bitmap
			pt.y += nIconWidth + nPadding;
			pt2.y += nIconWidth + nPadding;
		}
	}

    // Fill tab	using currently selected brush
	CDC			memDC;
	CBitmap		bmp;
	if(	pTab->m_bEnabled || 
	  (!memDC.CreateCompatibleDC(pDC) ||
	   !bmp.CreateCompatibleBitmap(pDC, rectItem.Width()+5, rectItem.Height()+5)))
		{
		// Draw icon
		// center bitmap vertically
#ifdef WIN32
		if (pTab->m_hIcon)
			::DrawIconEx(pDC->GetSafeHdc(), x, y,
				pTab->m_hIcon, nIconWidth, nIconWidth, 0, 0, DI_NORMAL);
#endif

		// Draw the text  (if any)

		
		if (m_bShowTabLabels)
			pDC->ExtTextOut(pt.x, pt.y, ETO_CLIPPED,
				&rectItem, pTab->m_csLabel, pTab->m_csLabel.GetLength(), NULL);
		}
	else
		{
		CBitmap*	pBmpOld = NULL;
		int oldBkMode;
		CPen *pOldPen;
		CBrush *pOldBrush;
		CFont *pOldFont;
		COLORREF oldTextColor;
		CRect	rcTab(rectItem);
		CPoint	ptImage(0,0), ptStart(0,0);
		int		nWidth, nHeight;

		rcTab.OffsetRect(-rectItem.left, -rectItem.top);
#ifdef WIN32
		rcTab.InflateRect(0, 0, 5, 5);
#else
		::InflateRect(rcTab,5,5);
#endif
		nWidth = size.cx;
		nHeight = size.cy;
		if (dwStyle & TCS_TABS_ON_LEFT)
			{
			ptImage.y = rcTab.bottom;
			ptStart.y = rcTab.bottom - size.cy;
			}
		else if (dwStyle & TCS_TABS_ON_RIGHT)
			{
			ptImage.x = rcTab.left + rectItem.Width()/2 + size.cx/2;
			ptStart.x = ptImage.x - size.cx;
			}
		pBmpOld = memDC.SelectObject(&bmp);
		oldBkMode = memDC.SetBkMode(TRANSPARENT);
    
		oldTextColor = memDC.SetTextColor(m_rgbText[SEC3DTAB_INACTIVE]);
		pOldBrush = memDC.SelectObject(&m_brushes[SEC3DTAB_INACTIVE]);
		pOldFont = memDC.SelectObject(&m_fonts[SEC3DTAB_INACTIVE]);
		pOldPen = memDC.SelectObject(&m_pens[SEC3DTAB_INACTIVE]);

		// Draw icon
		// center bitmap vertically
		if (pTab->m_hIcon)
			{
#ifdef WIN32
			memDC.FillSolidRect(&rcTab, rgbFace);
			::DrawIconEx(memDC.GetSafeHdc(), 0, 0,
				pTab->m_hIcon, nIconWidth, nIconWidth, 0, 0, DI_NORMAL);
#else
			CBrush brgbFace(rgbFace);
    		memDC.FillRect(&rcTab,&brgbFace);
#endif
			DrawDisabled(&memDC, rcTab.Width(), rcTab.Height());
			pDC->BitBlt(x, y, nIconWidth, nIconWidth, &memDC, 0, 0, SRCCOPY);

			if (dwStyle & (TCS_TABS_ON_TOP|TCS_TABS_ON_BOTTOM))
				nWidth = min(size.cx, rectItem.right-pt2.x);
			}
		// Draw the text  (if any)
		if (m_bShowTabLabels)
			{
			CSize Avail;

#ifdef WIN32
			memDC.FillSolidRect(&rcTab, rgbFace);
#else
			CBrush brgbFace(rgbFace);
    		memDC.FillRect(&rcTab,&brgbFace);
#endif
			memDC.ExtTextOut(ptImage.x, ptImage.y, ETO_CLIPPED,
				&rcTab, pTab->m_csLabel, pTab->m_csLabel.GetLength(), NULL);
			DrawDisabled(&memDC, rcTab.Width(), rcTab.Height());
			pDC->BitBlt(pt2.x, pt2.y, nWidth, nHeight, &memDC, ptStart.x, ptStart.y, SRCCOPY);
			}
		memDC.SelectObject(pBmpOld);
		memDC.SetBkMode(oldBkMode);
		memDC.SelectObject(pOldFont);
		memDC.SelectObject(pOldPen);
		memDC.SelectObject(pOldBrush);
		memDC.SetTextColor(oldTextColor);
		}  
}	


/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl erase background handler

BOOL SEC3DTabControl::OnEraseBkgnd(CDC* pDC) 
{
    ASSERT_VALID(this);
    ASSERT_VALID(pDC);
	pDC; //UNUSED

    CRect rect;
    GetClientRect(rect);
    //pDC->FillRect(rect, &m_brushes[SEC3DTAB_INACTIVE]);
    
    return TRUE;
}

void SEC3DTabControl::rgbFill(CDC* pDC,int x,int y,int cx,int cy, COLORREF rgb)
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

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl mouse input handlers

void SEC3DTabControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    ASSERT_VALID(this);
	
    // Activate the tab if one was hit.
	int iActiveTab = -1;
	GetActiveTab(iActiveTab);

    for (int tab=0; tab<GetTabCount(); tab++)
	{
		if (TabHit(tab, point))
		{
			if( iActiveTab == tab )
				ReActivateTab();
			else
				ActivateTab(tab);
		}
    }
    
	SECTabControlBase::OnLButtonDown(nFlags, point);
}

void SEC3DTabControl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
    ASSERT_VALID(this);
	
    // Determine which tab was dbl hit and send message
    for (int tab=0; tab<GetTabCount(); tab++)
	{
		if (TabHit(tab, point))
			GetParent()->SendMessage(TCM_TABDBLCLK, (WPARAM)tab);
    }
    
	SECTabControlBase::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl Tool Tip Handlers

#ifdef WIN32
//@doc SEC3DTabControl
//@mfunc Win32 only.  Determines whether a point is in the bounding rectangle of the specified tool.  If the point is in 
// the rectangle, it retrieves information about the tool.  If the area with which the tooltip is associated is not a
// button (i.e., a tab), OnToolHitTest sets the structure flags to TTF_NOTBUTTON and TTF_CENTERTIP. 
//@rdesc Nonzero if the tooltip control is found; otherwise 0.
//@parm CPoint | point | Specifies the x- and y - coordinate of the cursor, relative to the upper lefthand corner of the
// window.
//@parm  TOOLINFO* | pTI | Pointer to a TOOLINFO structure.
//@xref<c SEC3DTabControl>
int SEC3DTabControl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	ASSERT_VALID(this);
	ASSERT(::IsWindow(m_hWnd));

	CRect rcTab;
	int nTab;

	// check child windows first by calling SECTabControlBase
	int nHit = SECTabControlBase::OnToolHitTest(point, pTI);
	if (nHit != -1)
		return nHit;

	for (nTab = 0; nTab < GetTabCount(); nTab++)
	{
		GetTabRect(nTab, rcTab);
		if (rcTab.PtInRect(point) && (GetTabPtr(nTab)->m_bToolTip || (!m_bShowTabLabels & !m_bSizing)))
		{
			if (pTI)
			{
				pTI->cbSize=sizeof(TOOLINFO);
				pTI->hwnd = m_hWnd;
				pTI->rect = rcTab;
				pTI->uId = nTab + 1;
				pTI->lpszText = _tcsdup(GetTabPtr(nTab)->m_csLong);
			}
			return nTab + 1;
		}
	}

	return -1;
}
#endif


/////////////////////////////////////////////////////////////////////////////
// SEC3DTabControl diagnostics

#ifdef _DEBUG
void SEC3DTabControl::AssertValid() const
{
	SECTabControlBase::AssertValid();
}

void SEC3DTabControl::Dump(CDumpContext& dc) const
{
	SECTabControlBase::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////


