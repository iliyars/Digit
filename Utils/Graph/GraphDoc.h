#if !defined(AFX_GRAPHDOC_H__C162C262_4180_435C_BB04_D7560464B7A1__INCLUDED_)
#define AFX_GRAPHDOC_H__C162C262_4180_435C_BB04_D7560464B7A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGraphDoc document

class CGraphDoc : public CDocument
{
protected:
	CGraphDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CGraphDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGraphDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHDOC_H__C162C262_4180_435C_BB04_D7560464B7A1__INCLUDED_)
