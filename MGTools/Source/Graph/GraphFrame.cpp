// GraphFrame.cpp : implementation file
//

#include "..\..\Include\Graph\GraphFrame.h"
#include "..\..\Include\Utils\utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////
TCHAR W_szFormat[] = _T("%u,%u,%d,%d,%d,%d,%d,%d,%d,%d");
TCHAR W_strBuffer[sizeof("-32767")*8 + sizeof("65535")*2];

BOOL W_ReadWndPlacement(LPWINDOWPLACEMENT pwp,
                      LPCTSTR Section, LPCTSTR Key, LPCTSTR File)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
    LPCTSTR Def = "ERROR";
    ::GetPrivateProfileString(Section, Key, Def, W_strBuffer,
                             sizeof("-32767")*8 + sizeof("65535")*2, File);
    if (CString(W_strBuffer).IsEmpty())
         return FALSE;

    WINDOWPLACEMENT wp;
    int nRead = _stscanf(W_strBuffer, W_szFormat,
        &wp.flags, &wp.showCmd,
        &wp.ptMinPosition.x, &wp.ptMinPosition.y,
        &wp.ptMaxPosition.x, &wp.ptMaxPosition.y,
        &wp.rcNormalPosition.left, &wp.rcNormalPosition.top,
        &wp.rcNormalPosition.right, &wp.rcNormalPosition.bottom);

    if (nRead != 10)
        return FALSE;

    wp.length = sizeof wp;
    *pwp = wp;
    return TRUE;
}

BOOL W_WriteWndPlacement(LPWINDOWPLACEMENT pwp,
                       LPCTSTR Section, LPCTSTR Key,
                       LPCTSTR File)
{
  AFX_MANAGE_STATE(AfxGetStaticModuleState());
   wsprintf(W_strBuffer, W_szFormat,
        pwp->flags, pwp->showCmd,
        pwp->ptMinPosition.x, pwp->ptMinPosition.y,
        pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
        pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
        pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);

   CString Str = Key;
   int iPos = Str.Find("-");
   if(iPos != -1) Str = Str.Mid(0,iPos-1);
   return ::WritePrivateProfileString(Section, LPCTSTR(Str), W_strBuffer, File);
}
/////////////////////////////////////////////////////////////////////////////
// CGraphFrame

IMPLEMENT_DYNCREATE(CGraphFrame, CFrameWnd)
CGraphFrame::CGraphFrame(LPCTSTR _Title/*=NULL*/, LPCTSTR _iniFile/*NULL*/,
						 bool _ReadOnlyPos/*=FALSE*/)
{
	p2DGraph = NULL;
	p3DGraph = NULL;
	pIsolineGraph = NULL;
	pMapGraph = NULL;
	iniFile = _iniFile;
    TypeGraph = GRAPH_2D;
	Title = _Title;
	ReadOnlyPos = _ReadOnlyPos;
}

CGraphFrame::~CGraphFrame()
{
  DestroyWindow();
}

BEGIN_MESSAGE_MAP(CGraphFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CGraphFrame)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGraphFrame message handlers
BOOL CGraphFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~FWS_ADDTOTITLE;
	hParent = cs.hwndParent;
     WINDOWPLACEMENT wp;
     if(W_ReadWndPlacement(&wp,"WINDOW_PLACEMENT", LPCTSTR(Title), LPCTSTR(iniFile))){
        cs.x = wp.rcNormalPosition.left;
        cs.y = wp.rcNormalPosition.top;
      if(!ReadOnlyPos){
        cs.cx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
        cs.cy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
      }
     }
	return CFrameWnd::PreCreateWindow(cs);
}

int CGraphFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect clientRect;
	GetClientRect(&clientRect);
    CalcWindowRect(&clientRect);

	if(TypeGraph == GRAPH_2D){
		p2DGraph = new C2DGraph(LPCTSTR(iniFile));
		p2DGraph->Create(NULL,"",WS_VISIBLE|WS_CHILD, clientRect, this, WND_2D_GRAPH);
		SetActiveView(p2DGraph);
	}
	else if(TypeGraph == GRAPH_3D){
		p3DGraph = new C3DGraph(LPCTSTR(iniFile));
		p3DGraph->Create(NULL,"",WS_VISIBLE|WS_CHILD, clientRect, this, WND_3D_GRAPH);
		SetActiveView(p3DGraph);
	}
	else if(TypeGraph == GRAPH_ISOLINE){
		pIsolineGraph = new CIsolineGraph(LPCTSTR(iniFile));
		pIsolineGraph->Create(NULL,"",WS_VISIBLE|WS_CHILD, clientRect, this, WND_2D_GRAPH);
		SetActiveView(pIsolineGraph);
	}
	else if(TypeGraph == GRAPH_MAP){
		pMapGraph = new CMapGraph(LPCTSTR(iniFile));
		pMapGraph->Create(NULL,"",WS_VISIBLE|WS_CHILD, clientRect, this, WND_2D_GRAPH);
		SetActiveView(pMapGraph);
	}	
	return 0;
}

BOOL CGraphFrame::DestroyWindow()
{
	return CFrameWnd::DestroyWindow();
}

void CGraphFrame::PostNcDestroy()
{
    this->m_hWnd = NULL;
}

void CGraphFrame::OnClose()
{
	WINDOWPLACEMENT wp;
    wp.length = sizeof wp;
    if(GetWindowPlacement(&wp)) {
        wp.flags = 0;
        if (IsZoomed())
            wp.flags |= WPF_RESTORETOMAXIMIZED;
        W_WriteWndPlacement(&wp, "WINDOW_PLACEMENT", LPCTSTR(Title), LPCTSTR(iniFile));
    }

	CFrameWnd::OnClose();
}


void CGraphFrame::OnDestroy()
{
   if(hParent)
	::SetFocus(hParent);


	CFrameWnd::OnDestroy();
}

void CGraphFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	CRect clientRect;
	GetClientRect(&clientRect);
	CalcWindowRect(&clientRect);

	CSize cs;
	cs.cx = clientRect.Width();
	cs.cy = clientRect.Height();

	if(TypeGraph == GRAPH_2D && p2DGraph){
		p2DGraph->SetWindowPos(NULL, 0, 0, cs.cx, cs.cy,
			SWP_NOMOVE | SWP_NOOWNERZORDER);
		p2DGraph->RefreshPicture();
	}
	else if(TypeGraph == GRAPH_3D && p3DGraph && p3DGraph->bRender){
		p3DGraph->SetWindowPos(NULL, 0, 0, cs.cx, cs.cy,
			SWP_NOMOVE | SWP_NOOWNERZORDER);
		p3DGraph->RefreshPicture();
	}
	else if(TypeGraph == GRAPH_ISOLINE && pIsolineGraph && pIsolineGraph->bRender){
		pIsolineGraph->SetWindowPos(NULL, 0, 0, cs.cx, cs.cy,
			SWP_NOMOVE | SWP_NOOWNERZORDER);
		pIsolineGraph->RefreshPicture();
	}
	else if(TypeGraph == GRAPH_MAP && pMapGraph && pMapGraph->bRender){
		pMapGraph->SetWindowPos(NULL, 0, 0, cs.cx, cs.cy,
			SWP_NOMOVE | SWP_NOOWNERZORDER);
		pMapGraph->RefreshPicture();
	}
}

void CGraphFrame::OnSetPreviewMode( BOOL bPreview, CPrintPreviewState* pModeStuff )
{
	CFrameWnd::OnSetPreviewMode(bPreview, pModeStuff );
}

void CGraphFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CFrameWnd::OnPaint();

}
