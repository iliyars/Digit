﻿// Digit.cpp : Defines the class behaviors for the application.
//

#include <direct.h>
#include "stdafx.h"
#include "Digit.h"

#include "MainFrame\MainFrm.h"

#include "ImageTempl\ImageChildFrm.h"
#include "ImageTempl\ImageView.h"
#include "ImageTempl\ImageDoc.h"

#include "Utils\mutils.h"
#include "Utils\SpecFileDlg.h"

#include "Utils\Graph\GraphChildFrm.h"
#include "Utils\Graph\GraphDoc.h"
#include "Utils\Graph\3DGraphDoc.h"

#include "Utils\Edit\TextChildFrm.h"
#include "Utils\Edit\BaseTextDoc.h"
#include "Utils\Edit\BaseTextView.h"
//C:\Ilya\Programming\cpp\Numbering\Digit.cpp
#include "MGTools\Include\Utils\Utils.h"
#include "MGTools\Include\Graph\2DGraph.h"
#include "MGTools\Include\Graph\3DGraph.h"
#include "MGTools\MGTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int CDigitApp::m_nOpenMsg = RegisterWindowMessage(_T("DigitOpenMessage"));
UINT WM_CAPTURE_PERIOD = RegisterWindowMessage("CAPTURE_PERIOD");

/////////////////////////////////////////////////////////////////////////////
// CDigitApp

BEGIN_MESSAGE_MAP(CDigitApp, CWinApp)
	//{{AFX_MSG_MAP(CDigitApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
//	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDigitApp construction

CDigitApp::CDigitApp()
{
	rc_hInstance = NULL;
	intrLang = -1;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDigitApp object

CDigitApp theApp;
static BOOL bClassRegistered = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CDigitApp initialization
BOOL CDigitApp::FirstInstance(LPCTSTR CmdLine)
{
  CWnd *pWndPrev, *pWndChild;
  
  // Determine if another window with our class name exists...
  if (pWndPrev = CWnd::FindWindow(_T("DigitClass"), NULL))
    {
    DWORD dw = NULL;
	TCHAR cmd[_MAX_PATH];
	strcpy(cmd, CmdLine);
    ATOM atom = GlobalAddAtom(CmdLine);
    ::SendMessageTimeout(pWndPrev->GetSafeHwnd(), m_nOpenMsg, NULL, (LPARAM)atom,
        SMTO_ABORTIFHUNG, 500, &dw);
    // if so, does it have any popups?
    pWndChild = pWndPrev->GetLastActivePopup();

    // If iconic, restore the main window
    if (pWndPrev->IsIconic())
      pWndPrev->ShowWindow(SW_RESTORE);

    // Bring the main window or its popup to
    // the foreground

    pWndChild->SetForegroundWindow();
	
    // and we are done activating the previous one.
    return FALSE;
    }
  // First instance. Proceed as normal.
  else
    return TRUE;
}

BOOL CDigitApp::InitInstance()
{
    TCHAR dir[_MAX_PATH];
      _getcwd(dir, _MAX_PATH ); 
      fINI = dir;
      fINI += '\\';
      fINI += m_pszProfileName; 
//	  ::BroadcastSystemMessage(BSF_NOHANG, BSM_APPLICATIONS, WM_CAPTURE_PERIOD,0,0);
  CWnd *pWndPrev;
  pWndPrev = NULL;
  CCommandLineInfo cmdInfo;
  ParseCommandLine(cmdInfo);

  CString ss = m_lpCmdLine;
  ss.MakeLower();
  if(ss.Find(".scn") != -1){
//		ScnBackgroundMode(m_lpCmdLine);
		return false;
  }
  
  if(!FirstInstance(LPCTSTR(ss))){
    return FALSE;
  }

  CControls* pCtrls = GetControls();
  pCtrls->LoadINISettings();
  
  CString Text;
  if(LoadKeySection("Options", "Lang", Text, (LPCTSTR)fINI))
     intrLang = atoi(LPCTSTR(Text));
  else
	 intrLang = ENG_LANG;
  
  if(!ChangeIntrLang(intrLang)){
      LPCTSTR Str = "Fail load resource DLL";//CRS("RusLang.dll íå íàéäåíà","EngLang.dll is not available");
      AfxMessageBox(Str, MB_OK | MB_ICONSTOP);
      return FALSE;
   }

  WNDCLASS wndcls;
  memset(&wndcls, 0, sizeof(WNDCLASS));   // start with NULL defaults

  wndcls.style = CS_DBLCLKS;// | CS_HREDRAW | CS_VREDRAW;
  wndcls.lpfnWndProc = ::DefWindowProc;
  wndcls.hInstance = AfxGetInstanceHandle();
  HANDLE han = LoadImage(rc_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 32, 32, LR_SHARED);
  wndcls.hIcon = HICON(han); // or load a different icon
  wndcls.hCursor = LoadCursor( IDC_ARROW );
  wndcls.hbrBackground = (HBRUSH) (COLOR_WINDOW);
  wndcls.lpszMenuName = NULL;

  // Specify our own class name for using FindWindow later
  wndcls.lpszClassName = _T("DigitClass");

  // Register new class and exit if it fails
  if(!AfxRegisterClass(&wndcls))
    {
    TRACE("Class Registration Failed\n");
    return FALSE;
    }
	bClassRegistered = TRUE;
	// Change the registry key under which our settings are stored.
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
      free((void*)m_pszProfileName);
      m_pszProfileName=_tcsdup((LPCTSTR)fINI);

	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register document templates

    CMultiDocTemplate* pDocTemplate = new CMultiDocTemplate(
        IDR_IMAGETYPE,
        RUNTIME_CLASS(CImageDoc),
        RUNTIME_CLASS(CImageChildFrm),
        RUNTIME_CLASS(CImageView));
    AddDocTemplate(pDocTemplate);

    // create main MDI Frame window
    CMainFrame* pMainFrame = new CMainFrame;
    if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
        return FALSE;
    m_pMainWnd = pMainFrame;

	// Advanced Templates
	AddAdvancedTemplates();

    // Enable DDE Execute open
    EnableShellOpen();
    RegisterShellFileTypes();

  	  bool res = TreatCmdLine(m_lpCmdLine);
	  if(!res)
       return FALSE;
	
    // Enable drag/drop open
    m_pMainWnd->DragAcceptFiles();

    // The main window has been initialized, so show and update it.
    pMainFrame->ShowWindow(/*m_nCmdShow*/SW_MAXIMIZE);
    pMainFrame->UpdateWindow();

	MSG Msg;
	Msg.hwnd = GetMainFrame()->GetSafeHwnd();
	Msg.message = WM_CAPTURE_PERIOD;
	Msg.time = 0;
	Msg.wParam = WPARAM(LPCTSTR(fINI));
	Msg.lParam = 0L;
	FilterDllMsg(&Msg);
	
    return TRUE;
}

bool CDigitApp::TreatCmdLine(LPCTSTR CmdLine)
{
    // simple command line parsing
    if (CmdLine[0] != '\0'){
        // open an existing document
		CString ss = CmdLine;
        int iP1 = ss.ReverseFind('\\')+1;
        CString path;
		if(iP1 != 0)
		   path = ss.Left(iP1);
		while(ss.GetLength()){
		  ss.TrimLeft(',');
          int iP2 = ss.Find(',');
          CString name;
		  if(iP2 != -1){
		    name = ss.Mid(iP1, iP2 - iP1);
		    ss = ss.Mid(iP2);
		  }
		  else{
		    name = ss;
			ss.Empty();
		  }
		  iP1 = 0;
//          OpenDocumentFile(LPCTSTR(path+name)); ???
         OpenDocumentFile(LPCTSTR(name));

//============== Load ZAP, FRN ==========================
     CImageDoc* pDoc = (CImageDoc*)GetWIActiveDocument();
	 if(pDoc){
//		pDoc->Digit.Load(LPCTSTR(path+name)); ???
		pDoc->Digit.Load(LPCTSTR(name));
        CMainFrame* pMFr = GetMainFrame();
        pMFr->SetImageInfo(pDoc->Digit.Comments, pDoc->Digit.ScaleFactor, pDoc->Digit.Rotation);
	 }
		}
    }
	return true;
}

void CDigitApp::AddAdvancedTemplates()
{
    CMultiDocTemplate* pDocTemplate1 = new CMultiDocTemplate(
		    IDR_TEXT_WINDOW,
            RUNTIME_CLASS(CBaseTextDoc),
            RUNTIME_CLASS(CTextChildFrm),
            RUNTIME_CLASS(CBaseTextView));
    AddDocTemplate(pDocTemplate1);

    CMultiDocTemplate* pDocTemplate2 = new CMultiDocTemplate(
            IDR_IMAGETYPE,
            RUNTIME_CLASS(CGraphDoc),
            RUNTIME_CLASS(CTextChildFrm),
            C2DGraph::GetRunTime());
    AddDocTemplate(pDocTemplate2);

    CMultiDocTemplate* pDocTemplate3 = new CMultiDocTemplate(
            IDR_IMAGETYPE,
            RUNTIME_CLASS(C3DGraphDoc),
            RUNTIME_CLASS(CTextChildFrm),
            C3DGraph::GetRunTime());
    AddDocTemplate(pDocTemplate3);
}

bool CDigitApp::ChangeIntrLang(int iLang/*-1*/)
{
  if(iLang != -1){
     intrLang = iLang;
  }
  else{
	 if(intrLang == RUS_LANG)
		 intrLang = ENG_LANG;
	 else if(intrLang == ENG_LANG)
		 intrLang = RUS_LANG;
  }

  AFX_MODULE_STATE* pState = AfxGetModuleState();
  if (pState->m_appLangDLL != NULL){
 	 AfxFreeLibrary(pState->m_appLangDLL);
	 pState->m_appLangDLL = NULL;
  }

  if(intrLang == RUS_LANG)
	  rc_hInstance = AfxLoadLibrary("RusLangD.dll");
  else 
	  rc_hInstance = AfxLoadLibrary("EngLangD.dll");

  pState->m_appLangDLL = rc_hInstance;
  
  if(rc_hInstance) {
      AfxSetResourceHandle(rc_hInstance);
      return true;
  }
  else return false;
}

void CDigitApp::OnFileOpen()
{
    LPCTSTR title = CRS("Îòêðûòü ôàéë", "Open file");
    CSpecialFileDialog fileDlg(TRUE);
    CString fIndex;

    TCHAR strName[_MAX_PATH];
    strName[0] = (TCHAR)NULL;
    fileDlg.m_ofn.lpstrFile = strName;
    fileDlg.m_ofn.lpstrTitle = title;

    LPCTSTR lan;
	CString str;
    str.Empty();
    lan = CRS("Ôàéëû (*.bmp;*.jpg;*.pcx;*.tif;*.tga)","Files (*.bmp;*.jpg;*.pcx;*.tif;*.tga)");
    str += lan; str += (TCHAR)NULL;
    str += "*.bmp;*.jpg;*.pcx;*.tif;*.tga"; str += (TCHAR)NULL;

    lan = CRS("Ôàéëû (*.frn;*.zap)","Files (*.frn;*.zap)");
    str += lan; str += (TCHAR)NULL;
    str += "*.frn;*.zap"; str += (TCHAR)NULL;
	
    fileDlg.m_ofn.lpstrFilter = LPCTSTR(str);
    fileDlg.m_ofn.nFilterIndex = 1;

    fileDlg.m_ofn.Flags |= (OFN_FILEMUSTEXIST|OFN_ALLOWMULTISELECT);
    TCHAR Str[_MAX_PATH];
  
    if(ReadPath(GetWorkPath(), Str, GetIniFile()))
      fileDlg.m_ofn.lpstrInitialDir = Str;
    else
      fileDlg.m_ofn.lpstrInitialDir = NULL;

    if(ReadPath("LOAD_INDEX_FILE", Str, GetIniFile())){
       fileDlg.m_ofn.nFilterIndex = atoi(Str);
    }
    else
       fileDlg.m_ofn.nFilterIndex = 1;

    INT_PTR result = fileDlg.DoModal();
    if(result == IDOK){
      POSITION pos = fileDlg.GetStartPosition();
      CString s;
      while(pos){
         s = fileDlg.GetNextPathName(pos);
         AfxGetApp()->OpenDocumentFile(s);
      }
     CString FileName = s;
     int iP = FileName.ReverseFind('\\');
     CString sav = FileName.Left(iP);
     SavePath(GetWorkPath(), sav, GetIniFile());
     sav.Format("%d", fileDlg.m_ofn.nFilterIndex);
     SavePath("LOAD_INDEX_FILE", sav, GetIniFile());
	 
//============== Load ZAP, FRN ==========================
     CImageDoc* pDoc = (CImageDoc*)GetWIActiveDocument();
	 if(pDoc){
		pDoc->Digit.Load(LPCTSTR(FileName));
        CMainFrame* pMFr = GetMainFrame();
        pMFr->SetImageInfo(pDoc->Digit.Comments, pDoc->Digit.ScaleFactor, pDoc->Digit.Rotation);
	 }
	}
    else
    {
        DWORD err = CommDlgExtendedError();
        CString msg;
        msg.Format(_T("DoModal() вернул %d, CommDlgExtendedError() = 0x%08X"), (int)result, err);
        AfxMessageBox(msg);
        return;
    }
}

int CDigitApp::ExitInstance() 
{
  AFX_MODULE_STATE* pState = AfxGetModuleState();
  if (pState->m_appLangDLL != NULL){
 	 ::FreeLibrary(pState->m_appLangDLL);
	 pState->m_appLangDLL = NULL;
  }
  rc_hInstance = NULL;
  CloseAllDocuments(TRUE);

  CString Text;
  Text.Format("%d", intrLang);
  SaveKeySection("Options", "Lang", LPCTSTR(Text), GetIniFile(), FALSE);

  if (bClassRegistered)
    ::UnregisterClass(_T("DigitClass"), AfxGetInstanceHandle());

  return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDigitApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDigitApp message handlers



BOOL CDigitApp::PreTranslateMessage(MSG* pMsg) 
{
    if (CWinApp::PreTranslateMessage(pMsg))
        return TRUE;

	return FilterDllMsg(pMsg);
}

BOOL CDigitApp::OnIdle(LONG lCount) 
{
    if (CWinApp::OnIdle(lCount))
        return TRUE;

    ProcessDllIdle();
    return FALSE;   // no more for me to do
}
