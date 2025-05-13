#if !defined(AFX_BASETEXTDOC_H__04A7B7C6_A0A3_407F_953A_84345206CA55__INCLUDED_)
#define AFX_BASETEXTDOC_H__04A7B7C6_A0A3_407F_953A_84345206CA55__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BaseTextDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBaseTextDoc document

class CBaseTextDoc : public CDocument
{
protected:
	DECLARE_DYNCREATE(CBaseTextDoc)

// Attributes
public:
	CBaseTextDoc();
    void SetContents(LPCTSTR Title, LPCTSTR Contents);

// Operations
public:
    BOOL ReadOnly;
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBaseTextDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
    virtual void DeleteContents();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void OnCloseDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBaseTextDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CBaseTextDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASETEXTDOC_H__04A7B7C6_A0A3_407F_953A_84345206CA55__INCLUDED_)
