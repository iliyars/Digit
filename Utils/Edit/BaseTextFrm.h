#if !defined(AFX_BASETEXTFRM_H__83C8CC36_C35A_4C4F_83C6_5392A436B4C2__INCLUDED_)
#define AFX_BASETEXTFRM_H__83C8CC36_C35A_4C4F_83C6_5392A436B4C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTextFrm.h : header file
//

#include "BaseTextView.h"
#include "BaseTextDoc.h"

/////////////////////////////////////////////////////////////////////////////
// CBaseTextFrm frame

class CBaseTextFrm : public CFrameWnd
{
	DECLARE_DYNCREATE(CBaseTextFrm)
protected:

// Attributes
public:
     static HWND hwndParent;

// Operations
public:
	CBaseTextFrm();           // protected constructor used by dynamic creation

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTextFrm)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
  CBaseTextView* pEditView;
  
protected:
	virtual ~CBaseTextFrm();
    void SavePlacement();
	
	// Generated message map functions
	//{{AFX_MSG(CBaseTextFrm)
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETEXTFRM_H__83C8CC36_C35A_4C4F_83C6_5392A436B4C2__INCLUDED_)
