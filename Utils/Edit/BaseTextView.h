#if !defined(AFX_BASETEXTVIEW_H__22A3CBED_38F4_4A51_A323_1AD0674EF6DF__INCLUDED_)
#define AFX_BASETEXTVIEW_H__22A3CBED_38F4_4A51_A323_1AD0674EF6DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTextView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseTextView view

class CBaseTextView : public CEditView
{
protected:
	DECLARE_DYNCREATE(CBaseTextView)

// Attributes
public:
    CString TextInfo;

// Operations
public:
	CBaseTextView();
    void WriteTextStr();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTextView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
    CFont* mF;
	virtual ~CBaseTextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseTextView)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETEXTVIEW_H__22A3CBED_38F4_4A51_A323_1AD0674EF6DF__INCLUDED_)
