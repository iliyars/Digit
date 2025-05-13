// InterfSolver.h : main header file for the INTERFSOLVER DLL
//

#if !defined(AFX_INTERFSOLVER_H__25581E6B_2CA3_4A1D_A79D_4A7D78BF61E6__INCLUDED_)
#define AFX_INTERFSOLVER_H__25581E6B_2CA3_4A1D_A79D_4A7D78BF61E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#ifdef INTERFSOLVER_EXPORTS
#define INTERF_API __declspec(dllexport)
#else
#define INTERF_API __declspec(dllimport)
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CInterfSolverApp
// See InterfSolver.cpp for the implementation of this class
//

class CInterfSolverApp : public CWinApp
{
public:
	CInterfSolverApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterfSolverApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CInterfSolverApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERFSOLVER_H__25581E6B_2CA3_4A1D_A79D_4A7D78BF61E6__INCLUDED_)
