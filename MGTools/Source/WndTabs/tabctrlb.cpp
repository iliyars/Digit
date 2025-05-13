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
//  Description:  Implementation of SECTab/SECTabControlBase
//

#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __TABCTRL_H__
#include "..\..\Include\WndTabs\tabctrlb.h"
#endif //__TABCTRLB_H__

#ifndef __TABCTRL3_H__
#include "..\..\Include\WndTabs\tabctrl3.h"
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

/////////////////////////////////////////////////////////////////////////////
// SECTab Construction/Destruction

IMPLEMENT_DYNAMIC(SECTab, CObject)

SECTab::SECTab()
{
    m_nWidth = 0;
	m_nHeight = 0;
    m_bSelected = FALSE;
    m_pClient = NULL;
}

SECTab::~SECTab()
{
}

/////////////////////////////////////////////////////////////////////////////
// SECTab diagnostics

#ifdef _DEBUG
void SECTab::AssertValid() const
{
    CObject::AssertValid();
    ASSERT(m_nWidth >= 0);
    ASSERT(m_bSelected == TRUE || m_bSelected == FALSE);
}

void SECTab::Dump(CDumpContext& dc) const
{
    dc << "Tab \"" << m_csLabel << "\"";
    dc << "\nm_bWidth = " << m_nWidth;
    dc << "\nm_bSelected = " << m_bSelected;
    dc << "\nm_pClient = " << (void*)m_pClient;
    dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase

IMPLEMENT_DYNAMIC(SECTabControlBase, CWnd)

BEGIN_MESSAGE_MAP(SECTabControlBase, CWnd)
    //{{AFX_MSG_MAP(SECTabControlBase)
    ON_WM_DESTROY()
	ON_WM_SETCURSOR()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase Construction/Destruction

//@doc SECTabControlBase
//@mfunc SECTabControlBase
//@rdesc void
//@xref<mf SECTabControlBase::Create>
//@xref<c SECTabControlBase>
SECTabControlBase::SECTabControlBase()
{
    m_pTabArray = NULL;
    m_nActiveTab = 0;
    m_bActiveTab = FALSE;
}

SECTabControlBase::~SECTabControlBase()
{
}

//@doc SECTabControlBase
//@mfunc Creates a tab control and attaches it to the SECTabControlBase object.<nl>
//Apply the following styles to an SECTabControlBase object:<nl>
//@flag TCS_TABS_ON_BOTTOM | Creates the control with tabs on the bottom.
//@flag TCS_TABS_ON_TOP | Creates the control with tabs on the top.
//@flag TCS_TABS_ON_LEFT | Creates the control with tabs on the left.
//@flag TCS_TABS_ON_RIGHT | Creates the control with tabs on the right.
//@rdesc Returns TRUE on success; otherwise FALSE.
//@parm DWORD | dwStyle | Specifies the tab control's style.
//@parm  const RECT& | rect | Reference to the bounding rectangle of the tab control.
//@parm  CWnd* | pParentWnd | The parent window of the tab control.
//@parm  UINT | nID | The ID of the tab control.
//@xref<mf SECTabControlBase::SECTabControlBase>
//@xref<c SECTabControlBase>
BOOL SECTabControlBase::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    if ( CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS),
		      NULL, dwStyle | WS_CHILD, rect, pParentWnd, nID) ) {
		m_pTabArray = new CObArray();
		ASSERT_VALID(m_pTabArray);

		
		m_hcurArrow  = ::LoadCursor(NULL, IDC_ARROW);
		ASSERT(m_hcurArrow != NULL);
		Initialize(dwStyle);
		return TRUE;
    }
	
    TRACE0("Failed to create tab control.\n");
	
    return FALSE;
}

void SECTabControlBase::OnDestroy()
{
    int nTab;
	
    // Delete tabs    
    for (nTab=0; nTab<GetTabCount(); nTab++)
		delete m_pTabArray->GetAt(nTab);
    delete m_pTabArray;
}

/////////////////////////////////////////////////////////////////////////////
// Overridables

//@doc SECTabControlBase
//@mfunc This member is an advanced overridable that allows you to change the default appearance of the tab control. 
// By overriding this member function, you can customize the fonts, colors and pens used in rendering the tabs. About the 
// only characteristic you cannot alter by overriding Initialize is the shape of the tab.
//@rdesc void 
//@parm DWORD | dwStyle | The Windows style of the tab control.
//@xref<c SECTabControlBase>
void SECTabControlBase::Initialize(DWORD dwStyle)
{
	dwStyle; // UNUSED
}

//@doc SECTabControlBase
//@mfunc As discussed in the class introduction, each tab is represented by an instance of SECTab or a class derived from
// SECTab.  If you need to associate domain-specific data with each tab, you can derive your own class from SECTab and 
// insert those members there.  CreateNewTab is the member you have to override in order to instantiate your derived class
// instead of the default SECTab. 
//@rdesc A pointer to the tab object which was created.
//@xref<c SECTabControlBase>
SECTab* SECTabControlBase::CreateNewTab() const
{
	return new SECTab();
}

//@doc SECTabControlBase
//@mfunc Causes the tab control to recompute the size and positions of all tabs and any other graphical subparts it 
// contains.  If you retrieve a tab (via GetTab or GetTabPtr) and modify its attributes directly, you should call 
// RecalcLayout and InvalidateTab after you’re done.  This guarantees that all layout changes resulting from your 
// modifications are reflected on screen.
//@rdesc void 
//@xref<mf SECTabControlBase::InvalidateTab>
//@xref<c SECTabControlBase>
void SECTabControlBase::RecalcLayout()
{
}

//@doc SECTabControlBase
//@mfunc Called when a tab is being activated.
//@rdesc void 
//@parm int | nTab | Index of tab.
//@xref<c SECTabControlBase>
void SECTabControlBase::OnActivateTab(int nTab)
{
	nTab; // UNUSED
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase Operation

//@doc SECTabControlBase
//@mfunc A new tab is created and inserted immediately before the tab with the index nIndex.  The display positions of all
// tabs to the right of the new tab are recomputed and a repaint occurs.<nl>The label specified by lpszLabel becomes the
// new tab’s label.<nl>The third parameter is treated as client data which is associated with the tab. Usually, this will 
// be a pointer to a CWnd - derived class which should be shown when its associated tab is active.  However, the pClient
// parameter doesn’t have to be a CWnd-derivative since the SECTabControlBase (and its implementations) only store this 
// data.<nl>The hMenu parameter is an optional menu which should be shown when the tab is active.  The pExtra parameter is
// an optional opaque pointer to user-defined data which the caller needs to associate with the new tab.<nl>As long as you
// have a tab index, the user can call GetTabInfo to retrieve any of the data members (pClient, hMenu, or pExtra)
// associated with the new tab.<nl>Upon successful completion, this function returns a pointer to the new tab.  You can
// use this pointer to initialize or change any tab attributes immediately following the call to InsertTab. 
//@rdesc A pointer to the new SECTab object; NULL if a failure occurred.
//@parm int | nIndex | Index of tab to insert before.
//@parm LPCTSTR | lpszLabel | Label to be shown on the new tab.
//@parm CObject* | pClient | A pointer to the client to associate with the new tab.
//@parm HMENU | hMenu | The menu to show when the tab is active.
//@parm void* | pExtra | A pointer to user-defined data.
//@xref<mf SECTabControlBase::AddTab>
//@xref<mf SECTabControlBase::DeleteTab>
//@xref<c SECTabControlBase>
SECTab* SECTabControlBase::InsertTab(int nIndex,
								 LPCTSTR lpszLabel,
								 CObject* pClient,
								 HMENU hMenu,
								 void* pExtra)
{
    ASSERT_VALID(this);
    ASSERT(nIndex >= 0 && nIndex < GetTabCount());
    ASSERT(AfxIsValidString(lpszLabel));

	// Create and insert the tab
    SECTab* pTab = CreateNewTab();
	m_pTabArray->InsertAt(nIndex, pTab, 1);
	
	pTab->m_csLabel = lpszLabel;
    pTab->m_bSelected = FALSE;
    pTab->m_pClient = pClient;
    pTab->m_hMenu = hMenu;
    pTab->m_pExtra = pExtra;

	return pTab;
}

//@doc SECTabControlBase
//@mfunc A new tab is created and appended after the last tab.  The label specified by lpszLabel becomes the new tab’s 
// label.<nl>The second parameter is treated as client data which is associated with the tab. Usually, this will be a 
// pointer to a CWnd-derived class which should be shown when its associated tab is active.  However, the pClient
// parameter doesn’t have to be a CWnd-derivative since the SECTabControlBase (and its implementations) only store this
// data.<nl>The hMenu parameter is an optional menu which should be shown when the tab is active.  The pExtra parameter is
// an optional opaque pointer to user-defined data which the caller needs to associate with the new tab.<nl>As long as you
// have a tab index, the user can call GetTabInfo to retrieve any of the data members (pClient, hMenu, or pExtra) 
// associated with the new tab.<nl>Upon successful completion, this function returns a pointer to the new tab.  You can
// use this pointer to initialize or change any tab attributes immediately following the call to AddTab. 
//@rdesc A pointer to the new tab or NULL if the call failed. 
//@parm LPCTSTR | lpszLabel | Label to be shown on the tab.
//@parm CObject* | pClient | The object to associate with the tab (Usually CWnd-derived).
//@parm HMENU | hMenu | Optional menu to show when this tab is selected.
//@parm void* | pExtra | Opaque pointer to user-defined data to associate with this tab.
//@xref <mf SECTabControlBase::InsertTab>
//@xref<mf SECTabControlBase::DeleteTab>
//@xref<c SECTabControlBase>
SECTab* SECTabControlBase::AddTab(LPCTSTR lpszLabel,
							  CObject* pClient,
							  HMENU hMenu,
							  void* pExtra)
{
    ASSERT_VALID(this);
    ASSERT(AfxIsValidString(lpszLabel));

	// Create and insert the tab
    SECTab* pTab = CreateNewTab();
    int nIndex = m_pTabArray->Add(pTab);

    pTab->m_csLabel = lpszLabel;
    pTab->m_bSelected = FALSE;
    pTab->m_pClient = pClient;
    pTab->m_hMenu = hMenu;
    pTab->m_pExtra = pExtra;
	RecalcLayout();

	InvalidateTab(nIndex);

	return pTab;
}

//@doc SECTabControlBase
//@mfunc The SetTabIcon member allows you to set an icon to be shown on the tab specified by nIndex.  Some tab control 
// implementations may not support icons in their look and feel (i.e., the SECTabWnd class doesn’t).  In these cases, 
// calls to this member are ignored.  Three variants of this member exist, so you can call this member if you have a 
// handle, resource ID or resource name of the icon.
//@syntax SetTabIcon(int nIndex, HICON hIcon)
//@syntax SetTabIcon(int nIndex, UINT nIDIcon,int cx,int cy)
//@syntax SetTabIcon(int nIndex, LPCTSTR lpszIconName,int cx,int cy)
//@rdesc void 
//@parm int | nIndex | Index of tab to specify a tab for.
//@parm HICON | hIcon | The handle to the icon.
//@parm UINT | nIDIcon | The resource ID of the icon.
//@parm LPCTSTR | lpszIconName | The resource name of the icon. 
//@parm int | cx | Not used.
//@parm int | cy | Not used.
//@xref<c SECTabControlBase>
void SECTabControlBase::SetTabIcon(int nIndex, HICON hIcon)
{
	nIndex; // UNUSED
	hIcon; // UNUSED

	TRACE(_T("SetTabIcon() not implemented by this tab control variant.\n"));
}

void SECTabControlBase::SetTabIcon(int nIndex, UINT nIDIcon,int cx,int cy)
{
	nIndex; // UNUSED
	nIDIcon; // UNUSED
	cx; cy;	// unused

	TRACE(_T("SetTabIcon() not implemented by this tab control variant.\n"));
}

void SECTabControlBase::SetTabIcon(int nIndex, LPCTSTR lpszIconName,int cx,int cy)
{
	nIndex; // UNUSED
	lpszIconName; // UNUSED
	cx; cy;	// unused

	TRACE(_T("SetTabIcon() not implemented by this tab control variant.\n"));
}

//@doc SECTabControlBase
//@mfunc The specified tab is given the new label and updated to reflect the change on screen.
//@rdesc void 
//@parm int | nIndex | Index of tab to rename.
//@parm  LPCTSTR | lpszLabel | New label to be shown on the tab.
//@xref<mf SECTabControlBase::AddTab>
//@xref<mf SECTabControlBase::DeleteTab>
//@xref<c SECTabControlBase>
void SECTabControlBase::RenameTab(int nIndex, LPCTSTR lpszLabel)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	SECTab* pTab = GetTabPtr(nIndex);

	// Invalidate the tabs to the right of the new one
	int i;
	for (i=nIndex; i<GetTabCount(); i++)
		InvalidateTab(i);

	// Change the label
    pTab->m_csLabel = lpszLabel;
	RecalcLayout();

	// Invalidate the tabs to the right of the new one
	for (i=nIndex; i<GetTabCount(); i++)
		InvalidateTab(i);
}

//@doc SECTabControlBase
//@mfunc The tab specified by nTab is destroyed and the positions of all tabs to its right are recomputed.
//@rdesc void 
//@parm int | nTab | Index of the tab to delete.
//@xref<mf SECTabControlBase::AddTab>
//@xref<mf SECTabControlBase::RenameTab >
//@xref<c SECTabControlBase>
void SECTabControlBase::DeleteTab(int nTab)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nTab));

	// Invalidate the tabs to the right of the deleted one
	int i;
	for (i=nTab; i<GetTabCount(); i++)
		InvalidateTab(i);

	// Do the deletion	
    delete m_pTabArray->GetAt(nTab);
    m_pTabArray->RemoveAt(nTab);
	
    // Make sure the active tab index is still within range
   if (m_bActiveTab) {
        if (GetTabCount() == 0)
			m_bActiveTab = FALSE;
		else if (m_nActiveTab >= GetTabCount()) {
			m_bActiveTab = FALSE;
			ActivateTab(GetTabCount() - 1);
		}
		else if (m_nActiveTab == nTab) {
			m_bActiveTab = FALSE;
			ActivateTab(nTab); // Deleted the active tab; active next one
		}
    }

	// Invalidate the tabs to the right of the deleted one
	for (i=nTab; i<GetTabCount(); i++)
		InvalidateTab(i);

	RecalcLayout();
}

void SECTabControlBase::ReActivateTab()
{
	GetParent()->SendMessage( TCM_TABREACTIVATE, (WPARAM)m_nActiveTab );
}

//@doc SECTabControlBase
//@mfunc The tab at the specified index is activated and scrolled into view.  The active tab is the tab appearing on top 
// of all other tabs (including selected tabs).  There can be at most one active tab at any given moment.  On the other
// hand, any number of tabs can be concurrently selected.
//@rdesc void 
//@parm int | nTab | Index of the tab to activate.
//@xref<mf SECTabControlBase::GetActiveTab>
//@xref<mf SECTabControlBase::SelectTab>
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
void SECTabControlBase::ActivateTab(int nTab)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nTab));

	if (m_bActiveTab && nTab == m_nActiveTab)
		return;
	else if (m_bActiveTab)
	{
		// Invalidate and deactivate the previously active tab
		GetTab(m_nActiveTab).m_bSelected = FALSE;
		InvalidateTab(m_nActiveTab);
	}

    // Invalidate and mark as selected
    m_nActiveTab = nTab;
    m_bActiveTab = TRUE;
    GetTab(m_nActiveTab).m_bSelected = TRUE;
	OnActivateTab(nTab);
	InvalidateTab(m_nActiveTab);
    
    // Send a message to notify parent of tab selection change
    ScrollToTab(m_nActiveTab);
    GetParent()->SendMessage(TCM_TABSEL, (WPARAM)m_nActiveTab);
}

//@doc SECTabControlBase
//@mfunc The tab with the specified index is selected.  Any number of tabs can be selected but only one can be the active
// tab.  The active tab appears on top of all other tabs (including selected tabs).  After marking the tab as selected, 
// the tab control sends a TCM_TABSEL message to its parent.  The parent’s handler for this message will need to call 
// GetTabInfo for all tabs to determine which tabs are selected and retrieve the client of selected tabs.
//@rdesc void 
//@parm int | nTab |Index of tab to select.
//@xref<mf SECTabControlBase::ActivateTab>
//@xref<mf SECTabControlBase::ClearSelection>
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
void SECTabControlBase::SelectTab(int nTab)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nTab));
	
    GetTab(nTab).m_bSelected = TRUE;
	InvalidateTab(nTab);
    GetParent()->SendMessage(TCM_TABSEL, (WPARAM)m_nActiveTab);
}

//@doc SECTabControlBase
//@mfunc All tabs are marked as unselected and non-active.  Then, a TCM_TABSELCLR message is sent to the parent of the 
// tab control.
//@rdesc void 
//@xref<mf SECTabControlBase::SelectTab>
//@xref<mf SECTabControlBase::ActivateTab>
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
void SECTabControlBase::ClearSelection()
{
    ASSERT_VALID(this);
	
    int i;
    for (i=0; i<GetTabCount(); i++) {
		if (GetTab(i).m_bSelected)
			InvalidateTab(i);
		GetTab(i).m_bSelected = FALSE;
    }
    
    m_bActiveTab = FALSE;
    
    GetParent()->SendMessage(TCM_TABSELCLR);
}

//@doc SECTabControlBase
//@mfunc The tab with the specified index is scrolled into view.  The tabs are scrolled until the specified tab is
// completely in view.
//@rdesc void 
//@parm int | nTab | Index of tab to scroll into view.
//@xref<c SECTabControlBase>
void SECTabControlBase::ScrollToTab(int nTab)
{
	nTab; // UNUSED
}

//@doc SECTabControlBase
//@mfunc Causes a redraw of the area occupied by the specified tab.
//@rdesc void 
//@parm int | nTab | Index of tab to redraw.
//@xref<mf SECTabControlBase::AddTab>
//@xref<mf SECTabControlBase::DeleteTab>
//@xref<c SECTabControlBase>
void SECTabControlBase::InvalidateTab(int nTab)
{
	nTab; // UNUSED
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase Queries

//@doc SECTabControlBase
//@mfunc Call this function to check the existence of a tab.
//@syntax TabExists(CObject* pClient)
//@syntax TabExists(int nTab)
//@rdesc Nonzero if the specified tab exists; otherwise 0.
//@parm CObject* | pClient | Test for existence of tab with this client.
//@parm int | nTab | Test for existence of tab with this index.
//@xref<c SECTabControlbase>
BOOL SECTabControlBase::TabExists(CObject* pClient) const
{
    int nTab;
    if (FindTab(pClient, nTab))
		return TRUE;
    else
		return FALSE;
}

BOOL SECTabControlBase::TabExists(int nTab)	const
{
    if (nTab >= 0 && nTab < GetTabCount())
		return TRUE;
    else
		return FALSE;
}

//@doc SECTabControlBase
//@mfunc The label, client, and selection status, etc. of the tab with the given index are written to the corresponding 
// parameters.  Alternatively, you may prefer to retrieve the tab (using GetTab) and dereference only the attributes you
// need.
//@rdesc Nonzero if a tab exists with the specified index; otherwise 0.
//@parm int | nTab | Index of tab to retrieve info from.
//@parm  LPCTSTR& | lpszLabel | Contains the label shown on the specified tab after returning.
//@parm  BOOL& | bSelected | If a tab with the specified index exists, its selection state is returned through bSelected.
//@parm  CObject*& | pClient | If a tab with the specified index exists, a pointer to its associated client is returned
// through pClient (may be NULL).
//@parm  HMENU& | hMenu | If a tab with the specified index exists, its associated menu is returned via hMenu (may be NULL).
//@parm  void*& | pExtra | If a tab with the specified index exists, a pointer to user-defined data is returned in pExtra
// (may be NULL).
//@xref<mf SECTabControlBase::GetTab>
//@xref<c SECTabControlBase>
BOOL SECTabControlBase::GetTabInfo(int nTab, LPCTSTR& lpszLabel,
	BOOL& bSelected, CObject*& pClient, HMENU& hMenu, void*& pExtra) const
{
    ASSERT(nTab >= 0 && nTab < GetTabCount());

    if (nTab < GetTabCount()) {
		if((&GetTab(nTab))->IsKindOf(RUNTIME_CLASS(SEC3DTab)))
			lpszLabel = ((SEC3DTab*)&GetTab(nTab))->m_csLong;
		else
			lpszLabel = GetTab(nTab).m_csLabel;
		bSelected = GetTab(nTab).m_bSelected;
		pClient = GetTab(nTab).m_pClient;
		hMenu = GetTab(nTab).m_hMenu;
		pExtra = GetTab(nTab).m_pExtra;
		return TRUE;
    }
	
    TRACE0("Failed in SECTabControlBase::GetTabInfo()\n");
    return FALSE;
}

//@doc SECTabControlBase
//@mfunc Returns the number of tabs currently in the tab control.
//@rdesc The number of tabs displayed by the tab control.
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
int SECTabControlBase::GetTabCount() const
{
    return (m_pTabArray != NULL) ? m_pTabArray->GetSize() : 0;
}

//@doc SECTabControlBase
//@mfunc If you need to read or write tab attributes directly, you can retrieve a pointer to the SECTab-derived object
// and dereference its members at will.  You may prefer to use this member instead of GetTab if you need to do a 
// type-safe cast of the tab object to one of the SECTab-derived classes.
//@rdesc A pointer to the specified tab.
//@parm int | nTab | Index of tab to return a pointer to.
//@xref<mf SECTabControlBase::GetTab>
//@xref<c SECTabControlBase>
SECTab* SECTabControlBase::GetTabPtr(int nTab) const
{
    ASSERT(TabExists(nTab));
	ASSERT((SECTab*)m_pTabArray->GetAt(nTab) != NULL);
	return (SECTab*)m_pTabArray->GetAt(nTab);
}

//@doc SECTabControlBase
//@mfunc If you need to read or write tab attributes directly, you can retrieve a reference to the SECTab-derived object
// and dereference its members at will. 
//@rdesc A reference to the specified tab.
//@parm int | nTab | Index of tab to return.
//@xref<mf SECTabControlBase::GetTabPtr>
//@xref<c SECTabControlBase>
SECTab& SECTabControlBase::GetTab(int nTab) const
{
	SECTab* pTab = GetTabPtr(nTab);
    return *pTab;
}

//@doc SECTabControlBase
//@mfunc The array of tabs is searched for a tab with the client specified by pClient.  If such a tab is found, its index
// is returned in the nTab parameter.
//@rdesc Nonzero if a tab with the given client was found; otherwise 0.
//@parm const CObject* const | pClient | Find the tab with this client.
//@parm  int& | nTab | If a tab is found, its index is returned via this parameter.
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
BOOL SECTabControlBase::FindTab(const CObject* const pClient, int& nTab) const
{
    BOOL bFound;
    int i;
    
    bFound = FALSE;
    for (i=0; i<GetTabCount(); i++)
	if (GetTab(i).m_pClient == pClient) {
	    nTab = i;
	    bFound = TRUE;
	}
    
    return bFound;
}

//@doc SECTabControlBase
//@mfunc The index of the currently active tab is written to the nTab parameter.
//@rdesc Nonzero if there is an active tab; otherwise 0.
//@parm int& | nTab | If a tab is currently active, its index is returned via nTab.
//@xref<mf SECTabControlBase::GetTabInfo>
//@xref<c SECTabControlBase>
BOOL SECTabControlBase::GetActiveTab(int& nTab) const
{
    nTab = m_nActiveTab;
    return m_bActiveTab;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase Message Handlers

BOOL SECTabControlBase::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   	::SetCursor(m_hcurArrow);

    message; //UNUSED
	pWnd; // UNUSED
	nHitTest; // UNUSED
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabControlBase diagnostics

#ifdef _DEBUG
void SECTabControlBase::AssertValid() const
{
    ASSERT(!m_bActiveTab || (m_bActiveTab && m_nActiveTab >= 0 &&
    	m_nActiveTab < m_pTabArray->GetSize()));
    CWnd::AssertValid();
}

void SECTabControlBase::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);

	dc << "\nNumber of tabs = " << m_pTabArray->GetSize();
	if (m_bActiveTab)
		dc << "\nActive Tab = " << m_nActiveTab;
	dc << "\n";
}
#endif

/////////////////////////////////////////////////////////////////////////////
