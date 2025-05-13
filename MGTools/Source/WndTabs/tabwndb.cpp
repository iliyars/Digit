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
//  Description:	Implementation of SECTabWndBase
//

// stdafx.h for SEC includes secall.h
#include "..\..\stdafx.h"

#ifdef _SECDLL
#undef AFXAPI_DATA
#define AFXAPI_DATA __based(__segname("_DATA"))
#endif //_SECDLL

#ifndef __TABWNDB_H__
#include "..\..\Include\WndTabs\tabwndb.h"
#endif // __TABWNDB_H__

#ifndef __TABCTRL_H__
#include "..\..\Include\WndTabs\tabctrl.h"
#endif

#ifdef OTPRO
	#ifndef	__SDOCMULT_H__
	#include "..\..\Include\Image\sdocmult.h"
	#endif
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

IMPLEMENT_DYNAMIC(SECTabWndBase, CWnd)

BEGIN_MESSAGE_MAP(SECTabWndBase, CWnd)
	//{{AFX_MSG_MAP(SECTabWndBase)
	ON_WM_DESTROY()
	ON_WM_CHILDACTIVATE()
	ON_MESSAGE(TCM_TABSEL, OnTabSelect)
	ON_MESSAGE(TCM_TABREACTIVATE, OnTabReActivate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase construction/destruction

SECTabWndBase::SECTabWndBase()
{
    m_pTabCtrl = (SECTabControl*)NULL;
    m_pActiveWnd = (CWnd*)NULL;
	m_pWndCreating = NULL;
	m_pNotifyWnd=NULL;
}

SECTabWndBase::~SECTabWndBase()
{
}

BOOL SECTabWndBase::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName,
						   DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
						   UINT nID, CCreateContext* pContext)
{
	lpszClassName; // UNUSED
	lpszWindowName; // UNUSED
	rect; // UNUSED
	pParentWnd; // UNUSED
	pContext; // UNUSED
	return Create(pParentWnd, dwStyle, nID);
}

  
BOOL SECTabWndBase::Create(CWnd* pParentWnd, DWORD dwStyle, UINT nID)
{
    ASSERT(pParentWnd != NULL);
    ASSERT(nID != 0);
    ASSERT(dwStyle & WS_CHILD);

#ifdef OTPRO
#ifdef _DEBUG

	// The following code checks for a condition that can result in problems
	// related to view activation when tabbed window is used in conjunction 
	// with the docking views component of Objective Toolkit.
	//
	// If the following code produces an assertion, this means that the tab
	// window is being added as a child of a docking view that already has
	// an ID of AFX_ID_PANE_FIRST.  A tab window can have this ID (and, in fact,
	// is *expected* to have this ID) if the tab window consumes the client
	// area of an MDI child frame (e.g., if it is to act as a container for one
	// or more views which would normally otherwise have this ID).  The same
	// is true if the tab window consumes the client area in the SDI scenario.
	// When calculating the layout of the client area, MFC looks for a window with 
	// this ID.  In the normal MDI situation, the view window is the one with
	// this id.  If multiple windows with this ID exist, view activation behavior
	// can be very erratic.
	//
	// To fix this, create a unique ID for the tab window and specify
	// this during the call to Create().  For example:
	//
	//	VERIFY (m_wndTab.Create(this, 
	//							WS_CHILD|WS_VISIBLE|TWS_TABS_ON_BOTTOM, 
	//							ID_MYTAB));		// some unique id
	//

	if(pParentWnd->IsKindOf(RUNTIME_CLASS(CView))) 
	{
		CDocument *pDoc = ((CView*)pParentWnd)->GetDocument();
		if (pDoc) 
		{
			CDocTemplate *pDocTemplate = pDoc->GetDocTemplate();
			if (pDocTemplate->IsKindOf(RUNTIME_CLASS(SECMultiDocTemplate)))
			{
				// see the comments above for a description of the likely 
				// condition that can result in this assertion
				ASSERT(nID != AFX_IDW_PANE_FIRST);
			}
		}
	}

#endif // _DEBUG
#endif // OTPRO

	// Default notification window to parent
	m_pNotifyWnd=pParentWnd;
    
    // the Windows scroll bar styles bits turn on the scrollbars
    dwStyle &= ~(WS_HSCROLL | WS_VSCROLL);

    if (m_hWnd == 0)
	{
		if (!CreateEx(0, AfxRegisterWndClass(CS_DBLCLKS), NULL, dwStyle, 0, 0, 0, 0,
			pParentWnd->m_hWnd, (HMENU)nID, NULL))
		{
			TRACE0("SECTabWndBase window create failed.\n");
			return FALSE;       // create invisible
		}
	}
    
    // attach the initial scrollbar and tab control parts
    TRY
	{
		// Create tab control
		if (!CreateTabCtrl(dwStyle, SEC_IDW_TABCTRL))
			AfxThrowResourceException();  // Can't instantiate this class
	}
    CATCH_ALL(e)
	{
	    TRACE0("Failure caught in SECTabWndBase::CreateCommon()\n");
	    DestroyWindow(); // will clean up child windows
	    return FALSE;
	}
    END_CATCH_ALL

    return TRUE;
}

CWnd* SECTabWndBase::CreateView(CRuntimeClass* pViewClass,
			    CCreateContext* pContext, UINT nID)
{
    ASSERT_VALID(this);
    ASSERT(pViewClass != NULL);
#ifdef WIN32
    ASSERT(pViewClass->IsDerivedFrom(RUNTIME_CLASS(CWnd)));
#endif

#if _MFC_VER < 0x042 //4.2 B1 not happy with this at all!
    ASSERT(AfxIsValidAddress(pViewClass, sizeof(CRuntimeClass)));
#endif 
    CWnd* pWnd = NULL;
    TRY
	{
	    pWnd = (CWnd*)pViewClass->CreateObject();
	    if (pWnd == NULL)
		AfxThrowMemoryException();
	}
    CATCH_ALL(e)
	{
	    TRACE0("Out of memory creating a tab window view\n");
	    return pWnd;
	}
    END_CATCH_ALL
	
    ASSERT(pWnd->IsKindOf(RUNTIME_CLASS(CWnd)));
    ASSERT(pWnd->m_hWnd == NULL);       // not yet created
    
	// Remember the window being created.
	m_pWndCreating = pWnd;

    if (!pWnd->Create(NULL, NULL, SEC_WS_DEFAULT_TABCLIENT,
		CRect(0,0,0,0), this, nID, pContext))
	{
	    TRACE0("Warning: couldn't create the tab window view\n");
	    // pWnd will be cleaned up by PostNcDestroy
	    return NULL;
	}

	m_pWndCreating = NULL;

    return pWnd;
}

BOOL SECTabWndBase::CreateScrollBarCtrl(DWORD dwStyle, UINT nID)
{
    ASSERT_VALID(this);
    ASSERT(m_hWnd != NULL);

	ASSERT((dwStyle & WS_VISIBLE) == 0);
    HWND hSB = ::CreateWindow(_T("SCROLLBAR"), NULL, dwStyle | WS_CHILD,
			   0, 0, 1, 1, m_hWnd, (HMENU)nID,
			   AfxGetInstanceHandle(), NULL);
    ASSERT(hSB != NULL);
    return (hSB != NULL);
}


BOOL SECTabWndBase::CreateTabCtrl(DWORD dwStyle, UINT nID)
{
	dwStyle; // UNUSED
	nID; // UNUSED
    return FALSE;
}

void SECTabWndBase::OnDestroy()
{
    ASSERT_VALID(m_pTabCtrl);
    m_pTabCtrl->DestroyWindow();
    delete m_pTabCtrl;
}

void SECTabWndBase::OnChildActivate()
{
	OnReActivateTab();
}


/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase operations

//@doc SECTabWndBase
//@mfunc A new tab is created and inserted immediately before the tab with the index nIndex and the tabbed window is
// redrawn.<nl><nl>There are two versions of this member function: the first adds an existing window to the tabbed window,
// and the second dynamically creates the window before adding it.<nl><nl>Create a new instance of the object with the 
// runtime class specified by pViewClass.  Also, a new tab is added and associated with the child window just created.<nl><nl>
//Upon successful completion, this function returns a pointer to the new tab.  You can use this pointer to initialize or
// change any tab attributes immediately following the call to InsertTab.
//@syntax InsertTab(CWnd* pWnd, int nIndex, LPCTSTR lpszLabel)
//@syntax InsertTab(CRuntimeClass* pViewClass, int nIndex, LPCTSTR lpszLabel, CCreateContext* pContext, UINT nID)
//@rdesc A pointer to the new SECTab object; NULL if a failure occurred. 
//@parm CWnd* | pWnd | A pointer to the client window to be associated with the new tab.
//@parm int | nIndex | Index of tab to insert before.
//@parm LPCTSTR | lpszLabel | Label to be shown on the new tab.
//@parm CRuntimeClass* | pViewClass | Specifies the CRuntimeClass of the new view.
//@parm CCreateContext* | pContext | A pointer to a creation context used to create the view (usually the pContext passed 
// into the parent frame’s overridden CFrameWnd::OnCreateClient member function in which the tabbed window is being created).
//@parm UINT | nID | Window ID to assign to the new view.
//@xref<mf SECTabWndBase::AddTab>
//@xref<mf SECTabWndBase::RemoveTab>
//@xref<c SECTabWndBase>
SECTab* SECTabWndBase::InsertTab(CWnd* pWnd,
							 int nIndex,
							 LPCTSTR lpszLabel)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

    SECTab* pTab = m_pTabCtrl->InsertTab(nIndex, lpszLabel, pWnd);

	// Initialize the scroll style for the new tab
	SetScrollStyle(nIndex, 0);

	return pTab;
}

								       
SECTab* SECTabWndBase::InsertTab(CRuntimeClass* pViewClass,
							 int nIndex,
							 LPCTSTR lpszLabel,
							 CCreateContext* pContext,
							 UINT nID)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

	// If nID unspecified, generate a unique ID automatically
	if (nID == -1)
		nID = SEC_IDW_TABCLIENT_FIRST + GetTabCount();

    CWnd* pWnd = CreateView(pViewClass, pContext, nID);

	return InsertTab(pWnd, nIndex, lpszLabel);
}


//@doc SECTabWndBase
//@mfunc A new tab is created and appended after the last tab.  The label specified by lpszLabel becomes the new tab’s 
// label.<nl><nl>The second parameter is treated as client data which is associated with the tab. Usually, this will be a 
// pointer to a CWnd - derived class which should be shown when its associated tab is active.  However, the pClient 
// parameter doesn’t have to be a CWnd-derivative since the SECTabControl (and its implementations) only store this data.<nl><nl>
//Upon successful completion, this function returns a pointer to the new tab.  You can use this pointer to initialize or 
// change any tab attributes immediately following the call to AddTab. 
//@syntax AddTab(CWnd* pWnd, LPCTSTR lpszLabel)
//@syntax AddTab(CRuntimeClass* pViewClass, LPCTSTR lpszLabel, CCreateContext* pContext, UINT nID)
//@rdesc  A pointer to the new tab or NULL if the call failed.
//@parm CWnd* | pWnd | The window to associate with the tab.
//@parm LPCTSTR | lpszLabel | Label to be shown on the tab.
//@parm CRuntimeClass* | pViewClass | Specifies the CRuntimeClass of the new view.
//@parm CCreateContext* | pContext | A pointer to a creation context used to create the view (usually the pContext passed
// into the parent frame’s overridden CFrameWnd::OnCreateClient member function in which the tabbed window is being created).
//@xref<mf SECTabWndBase::InsertTab>
//@xref<c SECTabWndBase>
SECTab* SECTabWndBase::AddTab(CWnd* pWnd,
						  LPCTSTR lpszLabel)
{
    ASSERT_VALID(this);

    SECTab* pTab = m_pTabCtrl->AddTab(lpszLabel, pWnd);

	// Initialize the scroll style for the new tab
	SetScrollStyle(m_pTabCtrl->GetTabCount()-1, 0);

	return pTab;
}

SECTab* SECTabWndBase::AddTab(CRuntimeClass* pViewClass,
						  LPCTSTR lpszLabel,
						  CCreateContext* pContext,
						  UINT nID)
{
    ASSERT_VALID(this);

	// If nID unspecified, generate a unique ID automatically
	if (nID == -1)
		nID = SEC_IDW_TABCLIENT_FIRST + GetTabCount();

    CWnd* pWnd = CreateView(pViewClass, pContext, nID);

	return AddTab(pWnd, lpszLabel);
}

//@doc SECTabWndBase
//@mfunc The SetTabIcon member allows you to set an icon to be shown on the tab specified by nIndex.  Some tab control
// implementations may not support icons in their look and feel (i.e., the SECTabWnd class doesn’t).  In these cases,
// calls to this member are ignored.  Three variants of this member exist, so you can call this member if you have a 
// handle, resource ID or resource name of the icon.
//@syntax SetTabIcon(int nIndex, HICON hIcon)
//@syntax SetTabIcon(int nIndex, UINT nIDIcon, int cx, int cy)
//@syntax SetTabIcon(int nIndex, LPCTSTR lpszIconName, int cx, int cy)
//@rdesc void 
//@parm int | nIndex | Index of tab to specify a tab for.
//@parm  HICON | hIcon | The handle to the icon.
//@parm UINT | nIDIcon | The resource ID of the icon.
//@parm LPCTSTR | lpszIconName | The resource name of the icon. 
//@xref<c SECTabWndBase>
void SECTabWndBase::SetTabIcon(int nIndex, HICON hIcon)
{
	m_pTabCtrl->SetTabIcon(nIndex, hIcon);
}

void SECTabWndBase::SetTabIcon(int nIndex, UINT nIDIcon, int cx, int cy)
{
	m_pTabCtrl->SetTabIcon(nIndex, nIDIcon, cx, cy);
}

void SECTabWndBase::SetTabIcon(int nIndex, LPCTSTR lpszIconName, int cx, int cy)
{
	m_pTabCtrl->SetTabIcon(nIndex, lpszIconName, cx, cy);
}

void SECTabWndBase::SetScrollStyle(int nIndex, DWORD dwStyle)
{
	nIndex; // UNUSED
	dwStyle; // UNUSED
}

//@doc SECTabWndBase
//@mfunc The tab specified by nIndex or the associated window object is removed from the tabbed window.
//@syntax RemoveTab(CWnd* pWnd)
//@syntax RemoveTab(int nIndex)
//@rdesc void 
//@parm CWnd* | pWnd | Window of tab to remove.
//@parm int | nIndex | Index of the tab to delete.
//@xref<mf SECTabWndBase::AddTab>
//@xref<mf SECTabWndBase::RenameTab >
//@xref<c SECTabWndBase>
void SECTabWndBase::RemoveTab(CWnd* pWnd)
{
    int nIndex;

    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));
    		
    VERIFY(FindTab(pWnd, nIndex));
    RemoveTab(nIndex);
}

void SECTabWndBase::RemoveTab(int nIndex)
{
	LPCTSTR lpszLabel;
	BOOL bSelected;
	CWnd* pWnd;
	void* pExtra;

    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

    if (GetTabInfo(nIndex, lpszLabel, bSelected, pWnd, pExtra) && pWnd == m_pActiveWnd)
		m_pActiveWnd = NULL;

    m_pTabCtrl->DeleteTab(nIndex);
}

//@doc SECTabWndBase
//@mfunc The specified tab is given the new label and updated to reflect the change on screen.
//@syntax RenameTab(int nIndex, LPCTSTR lpszLabel)
//@syntax RenameTab(CWnd* pWnd, LPCTSTR lpszLabel)
//@rdesc void 
//@parm int | nIndex | Index of tab to rename.
//@parm CWnd* | pWnd | Window of tab to rename.
//@parm  LPCTSTR | lpszLabel | New label to be shown on the tab.
//@xref<mf SECTabWndBase::AddTab>
//@xref<mf SECTabWndBase::RemoveTab>
//@xref<c SECTabWndBase>
void SECTabWndBase::RenameTab(int nIndex, LPCTSTR lpszLabel)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));
    m_pTabCtrl->RenameTab(nIndex, lpszLabel);
}

void SECTabWndBase::RenameTab(CWnd* pWnd, LPCTSTR lpszLabel)
{
    int nIndex;

    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));

    FindTab(pWnd, nIndex);
    RenameTab(nIndex, lpszLabel);
}

//@doc SECTabWndBase
//@mfunc The tab at the specified index is activated and scrolled into view.  The active tab is the tab appearing on top 
// of all other tabs (including selected tabs).  There can be at most one active tab at any given moment.  On the other
// hand, any number of tabs can be concurrently selected.
//@syntax ActivateTab(CWnd* pWnd, int nIndex)
//@syntax ActivateTab(CWnd* pWnd)
//@syntax ActivateTab(int nIndex)
//@rdesc void 
//@parm CWnd* | pWnd | Window to activate.
//@parm int | nIndex | Index of the tab to activate.
//@xref<mf SECTabWndBase::GetActiveTab>
//@xref<mf SECTabWndBase::GetTabinfo>
//@xref<c SECTabWndBase>
void SECTabWndBase::ActivateTab(CWnd* pWnd, int nIndex)
{
    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));

    BOOL bActiveTab;
    int nActiveTab;

	
   // if (m_pActiveWnd != pWnd) 			 
	{
		CWnd* pWndTemp = m_pActiveWnd;
		m_pActiveWnd = pWnd;

		bActiveTab = m_pTabCtrl->GetActiveTab(nActiveTab);
		if (!bActiveTab || nActiveTab != nIndex)
    		m_pTabCtrl->ActivateTab(nIndex);

		// Hide the old window
		if (pWndTemp)
			pWndTemp->ShowWindow(SW_HIDE);
		
		// Show the new one
			m_pActiveWnd->ShowWindow(SW_SHOW);
       

		// IMPORTANT NOTE: if embedding the tab window inside a floating
		// controlbar, it is possible the tab window will restrict
		// access the the mainframe menubar and toolbar. If you 
		// encounter this scenario, it is important that you
		// override CView::OnActivateView for all your CView derived
		// classes and *NOT* invoke the base class, as this will
		// result in adverse affects on the tab window focus.

		BOOL bIsView=FALSE;
		if(pWnd->IsKindOf(RUNTIME_CLASS(CView)))
			bIsView=TRUE;

#ifdef OTPRO
		// If OT PRO, special handling required to insure view activation
		// logic peacefully coeexists with Docking Views activation logic.

		if(!bIsView) {
			SetFocus();		
			m_pActiveWnd->SetFocus();
			m_pActiveWnd->SetActiveWindow();
		}
		else {
			CFrameWnd* pParentFrame=GetParentFrame();
			ASSERT(pParentFrame);
			pParentFrame->SetActiveView((CView*) pWnd);

			// If we have a view inside a tab inside a floating controlbar,
			// must make sure the top-level-frame is the foreground window,
			// otherwise we will not have access to this frame's nonclient
			// area (for menu selection, toolbar clicks, etc.).
			CFrameWnd* pTLF=GetTopLevelFrame();
			if(pTLF) 
				::SetWindowPos(pTLF->m_hWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}	
#else
		SetFocus();
		m_pActiveWnd->SetFocus();
		m_pActiveWnd->SetActiveWindow();

		// This is necessary to avoid undue flicker when sizing a tab window
		// with contained views.
		if(bIsView) 
			GetParentFrame()->SetActiveView((CView *)pWnd);
#endif

		RecalcLayout();
		Invalidate(TRUE);
	}
}

void SECTabWndBase::ActivateTab(CWnd* pWnd)
{
    int nIndex;

    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);
    ASSERT(TabExists(pWnd));

    FindTab(pWnd, nIndex);
    ActivateTab(pWnd, nIndex);
}

void SECTabWndBase::ActivateTab(int nIndex)
{
    LPCTSTR label;
    CWnd* pWnd;
    BOOL bSelected;
    void* pExtra;

    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));

    GetTabInfo(nIndex, label, bSelected, pWnd, pExtra);
    ActivateTab(pWnd, nIndex);
}

//@doc SECTabWndBase
//@mfunc All tabs are marked as unselected and non-active.
//@rdesc void 
//@xref<mf SECTabWndBase::ActivateTab>
//@xref<mf SECTabWndBase::GetTabInfo>
//@xref<c SECTabWndBase>
void SECTabWndBase::ClearSelection()
{
    ASSERT_VALID(this);

    m_pTabCtrl->ClearSelection();
}

//@doc SECTabWndBase
//@mfunc The tab with the specified index is scrolled into view.
//@syntax ScrollToTab(CWnd* pWnd)
//@syntax ScrollToTab(int nIndex)
//@rdesc void 
//@parm CWnd* | pWnd | Window of tab to scroll into view.
//@parm int | nIndex | Index of tab to scroll into view.
//@xref<c SECTabWndBase>
void SECTabWndBase::ScrollToTab(CWnd* pWnd)
{
    int nIndex;

    ASSERT_VALID(pWnd);
    ASSERT_VALID(this);
    ASSERT(TabExists(pWnd));

    FindTab(pWnd, nIndex);
    m_pTabCtrl->ScrollToTab(nIndex);
}

void SECTabWndBase::ScrollToTab(int nIndex)
{
    ASSERT_VALID(this);
    ASSERT(TabExists(nIndex));
    m_pTabCtrl->ScrollToTab(nIndex);
}

void SECTabWndBase::SetNotifyWnd(CWnd* pNotifyWnd) {
	if(!pNotifyWnd) 
		 m_pNotifyWnd=GetParent();
	else m_pNotifyWnd=pNotifyWnd;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase queries

//@doc SECTabWndBase
//@mfunc Call this function to check for the existence of a specific tab.
//@syntax TabExists(CWnd* pWnd)
//@syntax TabExists(int nIndex)
//@rdesc Nonzero if the specified tab exists; otherwise 0.
//@parm CWnd* | pWnd | Test for existence of tab with this client window.
//@parm int | nIndex | Test for existence of tab with this index.
//@xref<c SECTabWndBase>
BOOL SECTabWndBase::TabExists(CWnd* pWnd)
{
    ASSERT_VALID(this);
    return m_pTabCtrl->TabExists((CObject*)pWnd);
}

BOOL SECTabWndBase::TabExists(int nIndex)
{
    ASSERT_VALID(this);
    return m_pTabCtrl->TabExists(nIndex);
}

//@doc SECTabWndBase
//@mfunc Gets the number of tabs contained in the tabbed window.
//@rdesc The number of tabs displayed by the tabbed window.
//@xref<c SECTabWndBase>
int SECTabWndBase::GetTabCount()
{
    ASSERT_VALID(this);

    return m_pTabCtrl->GetTabCount();
}

//@doc SECTabWndBase
//@mfunc The label, client window, and selection status, etc. of the tab with the given index are written to the 
// corresponding parameters. 
//@rdesc Nonzero if a tab exists with the specified index; otherwise 0.
//@parm int | nIndex | Index of tab to retrieve info of.
//@parm  LPCTSTR& | lpszLabel | Contains the label shown on the specified tab after returning.
//@parm  BOOL& | bSelected | If a tab with the specified index exists, its selection state is returned through bSelected.
//@parm  CWnd*& | pWnd | If a tab with the specified index exists, a pointer to its associated client is returned through
// pWnd (may be NULL).
//@parm  void*& | pExtra | If a tab with the specified index exists, a pointer to user-defined data is returned in pExtra
// (may be NULL).
//@xref<c SECTabWndBase>
BOOL SECTabWndBase::GetTabInfo(int nIndex, LPCTSTR& lpszLabel,
	BOOL& bSelected, CWnd*& pWnd, void*& pExtra)
{
    ASSERT_VALID(this);
    HMENU hMenu;
    
    return m_pTabCtrl->GetTabInfo(nIndex, lpszLabel,
    		bSelected, (CObject*&)pWnd, hMenu, pExtra);
}


//@doc SECTabWndBase
//@mfunc The array of tabs is searched for a tab with the associated window client specified by pWnd.  If a tab is found,
// its index is returned in the nTab parameter.
//@rdesc Nonzero if a tab with the given client was found; otherwise 0.
//@parm const CWnd* const | pWnd | Find the tab with this client.
//@parm  int& | nIndex | If a tab is found, its index is returned via this parameter.
//@xref<mf SECTabWndBase::GetTabInfo>
//@xref<c SECTabWndBase>
BOOL SECTabWndBase::FindTab(const CWnd* const pWnd, int& nIndex) const
{
    ASSERT_VALID(this);
    ASSERT_VALID(pWnd);

    return m_pTabCtrl->FindTab((CObject*)pWnd, nIndex);
}


//@doc SECTabWndBase
//@mfunc The index of the currently active tab is written to the nIndex parameter.
//@syntax GetActiveTab(CWnd*& pWnd)
//@syntax GetActiveTab(int& nIndex)
//@rdesc Nonzero if there is an active tab; otherwise 0.
//@parm CWnd*& | pWnd | Window currently active in tabbed window.
//@parm int& | nIndex | If a tab is currently active, its index is returned via nIndex.
//@xref<mf SECTabWndBase::GetTabInfo>
//@xref<c SECTabWndBase>
BOOL SECTabWndBase::GetActiveTab(CWnd*& pWnd)
{
    LPCTSTR lpszLabel;
    BOOL bSelected;
    int nIndex;
    HMENU hMenu;
    void* pExtra;

    ASSERT_VALID(this);
    if (!m_pTabCtrl->GetActiveTab(nIndex))
	return FALSE;

    return m_pTabCtrl->GetTabInfo(nIndex, lpszLabel,
    		bSelected, (CObject*&)pWnd, hMenu, pExtra);
}

BOOL SECTabWndBase::GetActiveTab(int& nIndex)
{
    ASSERT_VALID(this);

    return m_pTabCtrl->GetActiveTab(nIndex);
}

SECTabWndBase* GetParentTabWnd(const CWnd* const pWnd)
{
	if (pWnd->GetSafeHwnd() == NULL) // no Window attached
		return NULL;

	CWnd* pParentWnd = pWnd->GetParent();  // start with one parent up
	while (pParentWnd != NULL)
	{
		if (pParentWnd->IsKindOf(RUNTIME_CLASS(SECTabWndBase)))
			return (SECTabWndBase*)pParentWnd;
		pParentWnd = pParentWnd->GetParent();
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase drawing and layout

// like GetClientRect but inset by shared scrollbars
void SECTabWndBase::GetInsideRect(CRect& rect) const
{
    ASSERT_VALID(this);
    GetClientRect(rect);
}

void SECTabWndBase::RecalcLayout()
{
}

/////////////////////////////////////////////////////////////////////////////
// SECTabWndBase message handlers

LRESULT SECTabWndBase::OnTabSelect(WPARAM wParam, LPARAM /*lParam*/)
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
    
    return 0;
}

LRESULT SECTabWndBase::OnTabReActivate( WPARAM, LPARAM )
{
	OnReActivateTab();
	return 0L;
}

void SECTabWndBase::OnReActivateTab()
{
	int iTab = -1;
	BOOL bActive = m_pTabCtrl->GetActiveTab(iTab);

	if( bActive )
	{
		ASSERT( m_pActiveWnd );

		BOOL bIsView=FALSE;
		if(m_pActiveWnd->IsKindOf(RUNTIME_CLASS(CView)))
			bIsView=TRUE;
		if(!bIsView) {
			SetFocus();
			m_pActiveWnd->SetFocus();
			m_pActiveWnd->SetActiveWindow();
			}
		else 
			GetParentFrame()->SetActiveView( (CView*)m_pActiveWnd );
	}
}