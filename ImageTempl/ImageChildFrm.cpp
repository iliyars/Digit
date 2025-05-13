// ImageChildFrm.cpp : implementation file
//
/***********************************************************************************
 ������: ImageChildFrm.cpp
 ����������:
         ����� ����������� ��������-�������������      
************************************************************************************/

#include "..\stdafx.h"
#include "ImageChildFrm.h"
#include "ImageDoc.h"
#include "ImageView.h"
#include "..\Utils\mutils.h"
#include "..\MGTools\Include\Utils\Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CImageChildFrm

IMPLEMENT_DYNCREATE(CImageChildFrm, CMDIChildWnd)

CImageChildFrm::CImageChildFrm()
{
}

CImageChildFrm::~CImageChildFrm()
{
}

BEGIN_MESSAGE_MAP(CImageChildFrm, CMDIChildWnd)
	//{{AFX_MSG_MAP(CImageChildFrm)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageChildFrm message handlers

// ������ Microsoft Visual C++ ������������
void CImageChildFrm::ActivateFrame(int nCmdShow) 
{
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

// ������ Microsoft Visual C++ ������������
BOOL CImageChildFrm::PreCreateWindow(CREATESTRUCT& cs) 
{
    WINDOWPLACEMENT wp;
    if(ReadWndPlacement(&wp,"IMAGEFRAME","PLACEMENT", GetIniFile())){
	  cs.cx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	  cs.cy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	}
	
	return CMDIChildWnd::PreCreateWindow(cs);
}

// ������ Microsoft Visual C++ ������������
int CImageChildFrm::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

// ������ ��������� ���� � ini ����
void CImageChildFrm::SavePlacement()
{
  WINDOWPLACEMENT wp;
  wp.length = sizeof wp;
  if (GetWindowPlacement(&wp))
    WriteWndPlacement(&wp,"IMAGEFRAME","PLACEMENT", GetIniFile());
}

// ������ Microsoft Visual C++ ������������
void CImageChildFrm::OnClose() 
{
	SavePlacement();
	CMDIChildWnd::OnClose();
}

// ������ Microsoft Visual C++ ������������
void CImageChildFrm::OnSize(UINT nType, int cx, int cy) 
{
	CMDIChildWnd::OnSize(nType, cx, cy);
	
    CImageDoc* pDoc = (CImageDoc*)GetActiveDocument();
	if(pDoc && pDoc->IsFotoSections()){
      pDoc->AlignFotoSections();
	  pDoc->GetView()->Invalidate(FALSE);
	}
}

// ������ Microsoft Visual C++ ������������
void CImageChildFrm::OnMove(int x, int y) 
{
	CMDIChildWnd::OnMove(x, y);

    CImageDoc* pDoc = (CImageDoc*)GetActiveDocument();
	if(pDoc && pDoc->IsFotoSections()){
      pDoc->AlignFotoSections();
	  pDoc->GetView()->Invalidate(FALSE);
	}
}


