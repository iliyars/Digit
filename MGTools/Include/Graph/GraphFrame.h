#if !defined(AFX_2DGRAPHFRAME_H__71BC488A_864F_42E7_9DFA_15F011929839__INCLUDED_)
#define AFX_2DGRAPHFRAME_H__71BC488A_864F_42E7_9DFA_15F011929839__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphFrame.h : header file
//

#include "2DGraph.h"
#include "3DGraph.h"
#include "IsolineGraph.h"
#include "MapGraph.h"
/////////////////////////////////////////////////////////////////////////////
// CGraphFrame frame

class CGraphFrame : public CFrameWnd
{
	DECLARE_DYNCREATE(CGraphFrame)
protected:

// Attributes
public:
   CGraphFrame(LPCTSTR _Title=NULL, LPCTSTR _iniFile=NULL,
	           bool _ReadOnlyPos=false);
   virtual ~CGraphFrame();

   void EnableRatioWtoH(bool r){isRatioWtoH = r;}
   void SetTypeGraph(int _TypeGraph){TypeGraph = _TypeGraph;}
// Operations
public:
   CString iniFile;
   int TypeGraph;
        C2DGraph* p2DGraph;
        C3DGraph* p3DGraph;
	   CMapGraph* pMapGraph;
   CIsolineGraph* pIsolineGraph;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphFrame)
	public:
	virtual BOOL DestroyWindow();
	protected:
	virtual void PostNcDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnSetPreviewMode( BOOL bPreview, CPrintPreviewState* pModeStuff );
	//}}AFX_VIRTUAL

// Implementation
protected:
    HWND hParent;
    CString Title;
    bool ReadOnlyPos;
	bool isRatioWtoH;

    // Generated message map functions
	//{{AFX_MSG(CGraphFrame)
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_2DGRAPHFRAME_H__71BC488A_864F_42E7_9DFA_15F011929839__INCLUDED_)
