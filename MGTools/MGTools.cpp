// MGTools.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MGTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

UINT WM_CAPTURE_PERIOD = RegisterWindowMessage("CAPTURE_PERIOD");

/////////////////////////////////////////////////////////////////////////////
// CMGToolsApp

BEGIN_MESSAGE_MAP(CMGToolsApp, CWinApp)
	//{{AFX_MSG_MAP(CMGToolsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMGToolsApp construction

CMGToolsApp::CMGToolsApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMGToolsApp object

CMGToolsApp theApp;

BOOL CMGToolsApp::InitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::InitInstance();
}

int CMGToolsApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	return CWinApp::ExitInstance();
}


extern "C" BOOL FAR PASCAL EXPORT FilterDllMsg(LPMSG lpMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRY
	{
/*        if(lpMsg->message == WM_CAPTURE_PERIOD){
		 CString mes;
		 mes = "Unregistered components are found\r\nEmail support@stingsoft.com";
         CFileStatus stF;
         CTime tmF;
         CTime tmC(0x7D3,0x7,0x6,0x14,0x2D,0);
         BOOL resF = CFile::GetStatus(LPCTSTR(lpMsg->wParam), stF);
         if(resF){
			 tmF = stF.m_mtime;
			 CString ts1 = tmF.Format("%c");
			 if(tmF > tmC){
				 int res = AfxMessageBox(LPCTSTR(mes));
				 ::SendMessage(lpMsg->hwnd, WM_CLOSE,0L,0L);
				 return TRUE;
			 }
         }
		}*/
		return AfxGetThread()->PreTranslateMessage(lpMsg);
	}
	END_TRY
	return FALSE;
}

extern "C" void FAR PASCAL EXPORT ProcessDllIdle()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRY
	{
		// flush it all at once
		long lCount = 0;
		while (AfxGetThread()->OnIdle(lCount))
			lCount++;
	}
	END_TRY
}
