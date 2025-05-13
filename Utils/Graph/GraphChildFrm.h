#if !defined(AFX_GraphCHILDFRM_H__11C3F68C_BEA4_4E9B_9DC0_CE751BE1E05E__INCLUDED_)
#define AFX_GraphCHILDFRM_H__11C3F68C_BEA4_4E9B_9DC0_CE751BE1E05E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphChildFrm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphChildFrm frame

class CGraphChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGraphChildFrm)
protected:
	CGraphChildFrm();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
   void SavePlacement();
   void SetIniName(LPCTSTR _IniName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphChildFrm)
	public:
	virtual void ActivateFrame(int nCmdShow = -1);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CGraphChildFrm();
	CString IniName;

	// Generated message map functions
	//{{AFX_MSG(CGraphChildFrm)
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GraphCHILDFRM_H__11C3F68C_BEA4_4E9B_9DC0_CE751BE1E05E__INCLUDED_)
