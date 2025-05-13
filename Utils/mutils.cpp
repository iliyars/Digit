#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <ctype.h>
#include <io.h>
#include <time.h>

#include "..\stdafx.h"
#include "..\ImageTempl\ImageDoc.h"
#include "..\ImageTempl\ImageView.h"
#include "Edit\BaseTextDoc.h"
#include "mutils.h"
#include "..\MGTools\Include\Utils\Utils.h"


LPCTSTR GetIniFile()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  return pApp->GetIniFile();
}

CMainFrame* GetMainFrame()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  return (CMainFrame*)pApp->m_pMainWnd;
}
/*
CToolBar* GetImageToolBar()
{
  return GetMainFrame()->GetImageToolBar();
}
*/
CDocument* GetWIActiveDocument()
{
  CMainFrame* pFr = GetMainFrame();
  if(!pFr)
	  return 0;
  CFrameWnd* pAf = pFr->GetActiveFrame();
  return pAf->GetActiveDocument();
}

void GetImageFileName(CString& PathName, CSize& cs)
{
	CString ext = PathName.Right(3);
	ext.MakeLower();
	if(ext == "frn" || ext == "zap"){
     TCHAR text[MAX_PATH];
     if(ReadSection("IMAGE", "Size", text, LPCTSTR(PathName))){
		int Arcs[2]; 
		int n = StringToArray(text, Arcs);
		if(n==2){
		  cs.cx = Arcs[0]; cs.cy = Arcs[1];
		}
	 }
     if(ReadSection("IMAGE", "FileName", text, LPCTSTR(PathName))){
		PathName = text;
		if(!IsFileExist(LPCTSTR(PathName), FALSE))
           PathName.Empty();
     }
	 else{
		PathName.Empty();
	 }
	}
	else{
		PathName.Empty();
	}
}

void UpdateAllImageViews()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CMultiDocTemplate* pDT = (CMultiDocTemplate*)pApp->GetNextDocTemplate(pos);
  pos = pDT->GetFirstDocPosition();
  if(!pos)
	  return;
  CImageDoc* pActDoc = (CImageDoc*)GetWIActiveDocument();
  while(pos){
    CImageDoc* pD = (CImageDoc*)pDT->GetNextDoc(pos);
	pD->GetView()->InvalidateRect(NULL, FALSE);
  }
}

BOOL CheckForSaveAllImageDoc()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CMultiDocTemplate* pDT = (CMultiDocTemplate*)pApp->GetNextDocTemplate(pos);
  pos = pDT->GetFirstDocPosition();
  if(!pos)
	  return TRUE;
  CImageDoc* pActDoc = (CImageDoc*)GetWIActiveDocument();
  while(pos){
    CImageDoc* pD = (CImageDoc*)pDT->GetNextDoc(pos);
	if(!pD->CheckForSave())
	return FALSE;	
  }
  return TRUE;
}

bool IsLastActiveDoc()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CMultiDocTemplate* pDT = (CMultiDocTemplate*)pApp->GetNextDocTemplate(pos);
  pos = pDT->GetFirstDocPosition();
  CImageDoc* pActDoc = (CImageDoc*)GetWIActiveDocument();
  int n = 0;
  while(pos){
	n++;  
    CImageDoc* pD = (CImageDoc*)pDT->GetNextDoc(pos);
  }
  if(n==1)
	  return true;
  else
	  return false;
}

void DeActivateMode(UINT iMode)
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CMultiDocTemplate* pDT = (CMultiDocTemplate*)pApp->GetNextDocTemplate(pos);
  pos = pDT->GetFirstDocPosition();
  CImageDoc* pActDoc = (CImageDoc*)GetWIActiveDocument();
  while(pos){
    CImageDoc* pD = (CImageDoc*)pDT->GetNextDoc(pos);
	if(iMode == I_FOTO_SECTIONS)
	  pD->ActivateFotoSections(FALSE);
	else if(iMode == I_MEASURELINE)
	  pD->ActivateMeasure(FALSE);
	else if(iMode == I_BOUNDS_EXT)
	  pD->ActivateExtBounds(FALSE);
	else if(iMode == I_BOUNDS_INS)
	  pD->ActivateInsBounds(FALSE);
  }
}

void MinimizeUnactiveDocuments(bool minimize)
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CMultiDocTemplate* pDT = (CMultiDocTemplate*)pApp->GetNextDocTemplate(pos);
  pos = pDT->GetFirstDocPosition();
  CImageDoc* pActDoc = (CImageDoc*)GetWIActiveDocument();
  while(pos){
    CImageDoc* pD = (CImageDoc*)pDT->GetNextDoc(pos);
	if(pD != pActDoc){
	   CFrameWnd* pFrm = (CFrameWnd*)pD->GetView()->GetParentFrame();
	   if(minimize){
//		 pFrm->ModifyStyle(WS_OVERLAPPEDWINDOW, 0);
		 pFrm->UpdateWindow();
	     pFrm->ShowWindow(SW_HIDE);
	   }
	   else{
//		 pFrm->ModifyStyle(0, WS_OVERLAPPEDWINDOW);
		 pFrm->UpdateWindow();
	     pFrm->ShowWindow(SW_SHOW);
	   }
	   pFrm->Invalidate(FALSE);
	}
  }
}

CControls* GetControls()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  return pApp->GetControls();
}

CImageCtrls* GetImageCtrls(CView* pView/*NULL*/)
{
  CImageDoc* pD;
  if(pView==NULL)
     pD = (CImageDoc*)GetWIActiveDocument();
  else
	 pD = (CImageDoc*)pView->GetDocument();
  return &pD->imageCtrls;
}

CMeasureCtrls* GetMeasureCtrls(CView* pView/*NULL*/)
{
  CImageDoc* pD;
  if(pView==NULL)
     pD = (CImageDoc*)GetWIActiveDocument();
  else
	 pD = (CImageDoc*)pView->GetDocument();
  return &pD->measureCtrls;
}

CBoundCtrls* GetBoundCtrls(CView* pView/*NULL*/)
{
  CImageDoc* pD;
  if(pView==NULL)
     pD = (CImageDoc*)GetWIActiveDocument();
  else
	 pD = (CImageDoc*)pView->GetDocument();
  return &pD->boundCtrls;
}

CDigitInfo* GetDigitInfo(CView* pView/*NULL*/)
{
  CImageDoc* pD;
  if(pView==NULL)
     pD = (CImageDoc*)GetWIActiveDocument();
  else
	 pD = (CImageDoc*)pView->GetDocument();
  return &pD->Digit;
}

bool MakeCopyDocument(LPCTSTR RealPath, LPCTSTR RealName, LPCTSTR ActivePath)
{
    CControls* pCtrls = GetControls();
	CString newPath;
	pCtrls->GetUniqueImagePath(newPath, ActivePath);
	::CopyFile(ActivePath, LPCTSTR(newPath), FALSE);
	
    AfxGetApp()->OpenDocumentFile(LPCTSTR(newPath));
	CImageDoc* pDoc = (CImageDoc*) GetWIActiveDocument();
	pDoc->SetRealPath(RealPath);
	pDoc->SetRealName(RealName);
	
	return true;
}
/*
CSingleDocTemplate* GetTextInfo()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  while(pos){
    CDocTemplate* pDT = pApp->GetNextDocTemplate(pos);
	POSITION pos2 = pDT->GetFirstDocPosition();
	if(!pos2)
		continue;
    CDocument* pDoc = pDT->GetNextDoc(pos2);
    if(pDoc->IsKindOf(RUNTIME_CLASS(CBaseTextDoc)))
		return (CSingleDocTemplate*)pDT;
  }
  return 0; 
}
*/
CSingleDocTemplate* GetTemplTextInfo()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CDocTemplate* pDT = pApp->GetNextDocTemplate(pos);
  pDT = pApp->GetNextDocTemplate(pos);
  return (CSingleDocTemplate*)pDT; 
}

CDocument* GetDocTextInfo()
{
	CSingleDocTemplate* pDT = GetTemplTextInfo();
	POSITION pos = pDT->GetFirstDocPosition();
	if(!pos)
		return 0;
    CDocument* pDoc = pDT->GetNextDoc(pos);
    return pDoc;
}

CMultiDocTemplate* Get3DTempl()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  POSITION pos = pApp->GetFirstDocTemplatePosition();
  CDocTemplate* pDT = pApp->GetNextDocTemplate(pos);
  pDT = pApp->GetNextDocTemplate(pos);
  pDT = pApp->GetNextDocTemplate(pos);
  pDT = pApp->GetNextDocTemplate(pos);
  return (CMultiDocTemplate*)pDT; 
}

LPCTSTR CRS(LPCTSTR rusLang, LPCTSTR engLang)
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  if(pApp->GetCurrentLang() == RUS_LANG)
	  return rusLang;
  else
	  return engLang;
}

LPTSTR CRS(LPTSTR rusLang, LPTSTR engLang, int /*a*/)
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  if(pApp->GetCurrentLang() == RUS_LANG)
	  return rusLang;
  else
	  return engLang;
}

BOOL IsRussianLang()
{
  CDigitApp* pApp= (CDigitApp*) AfxGetApp(); 
  if(pApp->GetCurrentLang() == RUS_LANG)
	  return TRUE;
  else
	  return FALSE;
}

LPCTSTR GetWorkPath()
{
  LPCTSTR dir = "Working directory";
  return dir;
}

BOOL InitSection(LPCTSTR _secName, LPCTSTR _iniFile)
{
	return ::WritePrivateProfileString(_secName, NULL, NULL, _iniFile); 
}

BOOL LoadKeySection(LPCTSTR _secName,LPCTSTR _Key, CString& Text, LPCTSTR _iniFile, BOOL cutComments)
{
	TCHAR buf[256];
	DWORD size = ::GetPrivateProfileString(_secName, _Key, "", buf, 256, _iniFile);
	if(size > 0){
	  Text = buf;
	  Text.TrimLeft(' ');
	  if(cutComments){
	    int iP = Text.Find(';');
	    if(iP > -1)
		  Text = Text.Left(iP);
	  }
	  return true;
	}
	else{
	  return false;
	}
}

BOOL SaveKeySection(LPCTSTR _secName,LPCTSTR _Key, CString Text, LPCTSTR _iniFile, BOOL saveComments/*TRUE*/)
{
	if(saveComments){
		CString oldText;
		CString key = _Key;
		BOOL res = LoadKeySection(_secName, _Key, oldText, _iniFile, FALSE);
		if(res){
			oldText.TrimLeft(' ');
			int iP = oldText.Find(';');
			oldText = oldText.Mid(iP);
			int iP1 = Text.GetLength();
			int iP2 = key.GetLength();
			iP = iP1+iP2;
			if(iP < 34){
				int n = 34 - iP;
				for(int i=0; i < n; i++){
					Text += ' ';
				}
			}
		}
		Text += oldText;
	}
	return ::WritePrivateProfileString(_secName, _Key, LPCTSTR(Text), _iniFile);
}
//===================================================================
UINT InheritedThread(LPVOID pParam);

BOOL BeginNotePadProcess(LPCTSTR strCommand)
{
  // �������� ������������ ������
  SECURITY_ATTRIBUTES sa = 
  { 
    sizeof(SECURITY_ATTRIBUTES),
    NULL, TRUE 
  };

  // �������� ������ �������� ������
  CWinThread* pThread = AfxBeginThread(
    (AFX_THREADPROC)InheritedThread, 0, 
    THREAD_PRIORITY_IDLE, 0, 0, &sa);

  // ��������� ����������� ��� �������� ������ ��������
  PROCESS_INFORMATION pi;
  STARTUPINFO si;

  // ��������� ��������� STARTUPINFO 
  GetStartupInfo(&si);

  // ��������� ��������� ������ ��� 
  // ��������/������� ������ ��������
  // (������ ������ CWinTread �������� 
  // ���������� ������)
  CString csCommand;
  csCommand = "notepad "; csCommand += strCommand;
 //   csCommand.Format("Notepad %d", pThread->m_hThread);

  // �������� ��������� �� �������������� ������
  LPTSTR lpCmdLine = 
    csCommand.GetBuffer(csCommand.GetLength());

  // �������� ��������� ��������
  CreateProcess(
    NULL,       // ��� ������ ��������� �� ������ ���������
    lpCmdLine,                 // ��������� ������ �������� 
    NULL,                     // ���������� �������� ������ 
                                   // �������� �� ��������� 
    NULL, // ���������� �������� ������ ������ �� ��������� 
    TRUE,                        // ����������� ����������� 
    CREATE_DEFAULT_ERROR_MODE,      // ���������� ��������� 
                                                  // ������ 
    NULL,                  // ���������� ��������� �� ����� 
    NULL,                  // ������� ������� �� ���������� 
    &si,                      // �������� ����� ����������� 
                                   // ��������� STARTUPINFO 
    &pi // ������� ��������� � ������ ���������� � �������� 
    );

   CDigitApp* pApp = (CDigitApp*) AfxGetApp();
//   pApp->Ctrls.notepadCtrls.hHandle.Add(pi.hProcess);
   
  return TRUE;
}
// ����������� ������� ������
UINT InheritedThread(LPVOID pParam)
{
  // ����� ����������� ������ ��� ����������
  // �������� ��� ������� ������� ��-���!
  // �������������� ����� �� ����������� �������
  for (; ;)
    Sleep(10);  // ��� �������� ������������� ����������

  return 0;
}

void TerminateAllNotePadProcess()
{
/*   CDigitApp* pApp = (CDigitApp*) AfxGetApp();
   for(int i=0; i < pApp->Ctrls.notepadCtrls.hHandle.GetSize(); i++){
      TerminateProcess(pApp->Ctrls.notepadCtrls.hHandle[i], NULL);
   }
   pApp->Ctrls.notepadCtrls.hHandle.RemoveAll();*/
}
