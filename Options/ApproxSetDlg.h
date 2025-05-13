#if !defined(AFX_APPROXSETDLG_H__A4F53898_628A_4422_8692_7FA2240CEE1B__INCLUDED_)
#define AFX_APPROXSETDLG_H__A4F53898_628A_4422_8692_7FA2240CEE1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApproxSetDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApproxSetDlg dialog

class CApproxSetDlg : public CDialog
{
// Construction
public:
	CApproxSetDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CApproxSetDlg)
	enum { IDD = IDD_APPROXSET_DLG };
	double	Eps;
	int		Pow;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CApproxSetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CApproxSetDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPROXSETDLG_H__A4F53898_628A_4422_8692_7FA2240CEE1B__INCLUDED_)
