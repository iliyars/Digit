// BookMarkFolder.cpp : implementation file
//

#include "..\..\stdafx.h"
#include "..\..\Include\Utils\BookMark.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBookMarkInfo::CBookMarkInfo()
{
	pDlg=0;
	Title="";
}
/////////////////////////////////////////////////////////////////////////////
// CBookMarkControl

CBookMarkControl::CBookMarkControl() : CTabCtrl()
{
	iDefDlg=iCurrDlg=0;

	GrayColor=RGB(128, 128, 128);
	TxtColor=RGB(0,0,0);
	SelTxtColor=RGB(0, 0, 0);

	PenB.CreatePen(PS_INSIDEFRAME, 1, RGB(0,0,0));
	PenW.CreatePen(PS_INSIDEFRAME, 1, RGB(255,255,255));
	PenG.CreatePen(PS_INSIDEFRAME, 1, GetSysColor(COLOR_BTNFACE));
	BrushD.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	SetMSDrawStyle(false);
}

CBookMarkControl::~CBookMarkControl()
{
}


BEGIN_MESSAGE_MAP(CBookMarkControl, CTabCtrl)
	//{{AFX_MSG_MAP(CBookMarkControl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBookMarkControl message handlers



void CBookMarkControl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	if(!BI(iCurrDlg).pDlg->DoKillFocus()) return;

	CTabCtrl::OnLButtonDown(nFlags, point);
}

void CBookMarkControl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	int iCurr=GetCurSel();
	
	ShowDlg(iCurr);
}

int CBookMarkControl::AddPage(CBookMarkDlg *pD, UINT id, CString Text)
{
	pD->Create(id, this);
	pD->ShowWindow(SW_HIDE);

	pD->pTabControl=this;

	int N=BI.GetSize();
	BI.ReSet(N+1);

	BI(N).pDlg=pD;
	BI(N).Title=Text;

	return N;
}

int CBookMarkControl::AddPage(CBookMarkDlg *pD, CString Text)
{
	pD->pTabControl=this;

	int N=BI.GetSize();
	BI.ReSet(N+1);

	BI(N).pDlg=pD;
	BI(N).Title=Text;

	return N;
}

void CBookMarkControl::Init()
{
	CFont *dlgFont=GetFont();
	LOGFONT LogFont;
	dlgFont->GetLogFont(&LogFont);

	if(EasyFont.m_hObject!=0) EasyFont.DeleteObject();
	if(SelFont.m_hObject!=0) SelFont.DeleteObject();
	
	EasyFont.CreateFont(LogFont.lfHeight, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");
	SelFont.CreateFont(LogFont.lfHeight, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "MS Sans Serif");

	CRect rcT, rc;
	GetWindowRect(rcT);

	CWnd *pParent=GetParent();
	pParent->ScreenToClient(rcT);
	int i = 0;
	int Wmax=0, Hmax=0;
	int y_top=0;
	CRect rcItem;
	for(i=0; i<BI.GetSize(); i++) {
		InsertItem(i, BI(i).Title);
		GetItemRect(i, rcItem);
		if(rcItem.bottom>y_top) y_top=rcItem.bottom;

		CBookMarkDlg *pD=BI(i).pDlg;
//		pD->ShowWindow(SW_HIDE);
		
		pD->GetWindowRect(rc);
		if(rc.Width()>Wmax) Wmax=rc.Width();
		if(rc.Height()>Hmax) Hmax=rc.Height();
		
	}
	for(i=0; i<BI.GetSize(); i++) {
		CBookMarkDlg *pD=BI(i).pDlg;
		if(pD->GetParent()->m_hWnd!=m_hWnd)
			pD->MoveWindow(rcT.left+2, rcT.top+y_top+3, Wmax, Hmax);
		else pD->MoveWindow(2, y_top+3, Wmax, Hmax);
	}
	rcT.bottom=rcT.top+Hmax+y_top+6;
	rcT.right=rcT.left+Wmax+5;
	MoveWindow(rcT);

	if(BI.GetSize()!=0) 
		BI(iDefDlg).pDlg->ShowWindow(SW_SHOW);
}

void CBookMarkControl::OnPaint() 
{
	if(bDrawMSStyle) {
		CWnd::OnPaint();
		return;
	}

	CPaintDC dc(this); // device context for painting

	dc.SetBkColor(GetSysColor(COLOR_BTNFACE));

	CRect rc, rcT;
	GetWindowRect(rcT);
	GetItemRect(0, rc);

	int W=rcT.Width();
	int H=rcT.Height();

	dc.SelectObject(&PenW);
	dc.MoveTo(0, H-1);
	dc.LineTo(0, rc.bottom);
	dc.LineTo(W-1, rc.bottom);

	dc.SelectObject(&PenB);
	dc.LineTo(W-1, H-1);
	dc.LineTo(0, H-1);

	int nB=BI.GetSize();
	int x_l, x_r, y_t, y_b;
	COLORREF CurrTextColor;

	for(int i=0; i<=nB; i++) {
		int iOrd=i;
		if(iOrd==iCurrDlg) continue;
		if(iOrd==nB) {
			iOrd=iCurrDlg;
		}

		CBookMarkInfo &bi=BI(iOrd);
		GetItemRect(iOrd, rc);

		x_l=rc.left; x_r=rc.right; y_b=rc.bottom; y_t=rc.top;
		if(iOrd!=iCurrDlg) {
			if(bi.pDlg->bEnable) CurrTextColor=bi.pDlg->TitleTxtColor;
			else				 CurrTextColor=bi.pDlg->TitleGrayColor;

			dc.SelectObject(EasyFont);
		}
		else {
			rc.top-=2;
			rc.bottom+=2;
			rc.right+=2;
			rc.left-=2;

			x_l-=2; x_r+=2; y_t-=2;
			if(iCurrDlg==0) y_b+=1;

			CurrTextColor=bi.pDlg->TitleSelTxtColor;
			dc.SelectObject(SelFont);
		}

		dc.SetTextAlign(TA_CENTER|TA_BASELINE);
		dc.SetTextColor(CurrTextColor);			

		dc.FillRect(rc, &BrushD);
		if(!bi.pDlg->bEnable) {
			dc.SetTextColor(RGB(255, 255, 255));
			dc.TextOut((rc.right+rc.left)/2+1,
						  (rc.top+rc.bottom)/2+4+1,
						  bi.Title);
			dc.SetBkMode(TRANSPARENT);
		}

		dc.SetTextColor(CurrTextColor);
		dc.TextOut((rc.right+rc.left)/2,
					  (rc.top+rc.bottom)/2+4,
					  bi.Title);
		dc.SetBkMode(OPAQUE);

		dc.SelectObject(&PenW);

		dc.MoveTo(x_l, y_b);
		dc.LineTo(x_l, y_t+3);
		dc.LineTo(x_l+3,y_t);
		dc.LineTo(x_r-3,y_t);

		dc.SelectObject(&PenB);

		dc.LineTo(x_r-1, y_t+3);
		dc.LineTo(x_r-1, y_b);
	}
}

void CBookMarkControl::SetMSDrawStyle(BOOL bEnable)
{
	bDrawMSStyle=bEnable;
}

CBookMarkDlg *CBookMarkControl::GetBookMark(int Num)
{
	if(Num<BI.GetSize())
		return BI(Num).pDlg;
	else return 0;
}

int CBookMarkControl::GetBookMark(CBookMarkDlg *pDlg)
{
	int iDlg=-1;
	for(int i=0; i<BI.GetSize(); i++) {
		if((CBookMarkDlg*)BI(i).pDlg==pDlg) {
			iDlg=i;
			break;
		}
	}
	return iDlg;
}

BOOL CBookMarkControl::ShowDlg(int N)
{
	if(N<0 || N>=BI.GetSize()) return false;
	if(!BI(N).pDlg->bEnable) return false;

	if(!BI(N).pDlg->DoSetFocus()) return false;

	BI(iCurrDlg).pDlg->ShowWindow(SW_HIDE);

	BI(N).pDlg->DoShow();
	BI(N).pDlg->ShowWindow(SW_NORMAL);

	iCurrDlg=N;
	Invalidate();

	return true;
}

BOOL CBookMarkControl::ShowDlg(CBookMarkDlg *pDlg)
{
	int iDlg=0;
	for(int i=0; i<BI.GetSize(); i++) {
		if((CBookMarkDlg*)BI(i).pDlg==pDlg) {
			iDlg=i;
			break;
		}
	}
	return ShowDlg(iDlg);
}

void CBookMarkControl::ChangeBookMark(int iBookMark)
{
	if(ShowDlg(iBookMark)) SetCurSel(iBookMark);
	else SetCurSel(iCurrDlg);
}

void CBookMarkControl::ChangeBookMark(CBookMarkDlg *pBookMark)
{
	if(ShowDlg(pBookMark)) SetCurSel(iCurrDlg);
}

void CBookMarkControl::SetBookMarkDefault(int iBookMark)
{
	iDefDlg=iBookMark;
	if(iDefDlg <0 || iDefDlg>= BI.GetSize()) iDefDlg=0;
}

void CBookMarkControl::SetBookMarkDefault(CBookMarkDlg *pBookMark)
{
	iDefDlg=GetBookMark(pBookMark);
	if(iDefDlg <0 || iDefDlg>= (int)BI.GetSize()) iDefDlg=0;
}

BOOL CBookMarkControl::PrintBookMark() {
	if(!IsBookMarkPrinted()) return false;

	return BI(iCurrDlg).pDlg->PrintBookMark();
}

BOOL CBookMarkControl::IsBookMarkPrinted()
{
	return BI(iCurrDlg).pDlg->IsBookMarkPrinted();
}

void CBookMarkControl::Enable(int iDlg, int State)
{
	if(iDlg<0 || iDlg>=BI.GetSize()) return;

	BI(iDlg).pDlg->bEnable=State;
	UpdateWindow();
}

void CBookMarkControl::Refresh()
{
	Invalidate();
	BI(iCurrDlg).pDlg->Invalidate();
}
//===================================================================
CBookMarkDlg::CBookMarkDlg(UINT Id, CWnd *pParent) : CDialog(Id, pParent)
{
	pTabControl=0;

	bEnable=true;
	SetTitleColor();
	SetGrayColor();
}

CBookMarkDlg::CBookMarkDlg() : CDialog()
{
	pTabControl=0;

	bEnable=true;
	SetTitleColor();
	SetGrayColor();
}

BOOL CBookMarkDlg::DoSetFocus()
{
	return true;
}

BOOL CBookMarkDlg::DoKillFocus()
{
	return true;
}

void CBookMarkDlg::DoShow()
{
}

BOOL CBookMarkDlg::PrintBookMark()
{
	return true;
}

BOOL CBookMarkDlg::IsBookMarkPrinted()
{
	return true;
}

void CBookMarkDlg::Enable(int State)
{
	bEnable=State;
	if(pTabControl) pTabControl->Refresh();
}

void CBookMarkDlg::SetTitleColor(COLORREF TxtColor, COLORREF SelTxtColor)
{
	TitleTxtColor=TxtColor;
	TitleSelTxtColor=SelTxtColor;

	if(pTabControl) pTabControl->Refresh();
}

void CBookMarkDlg::SetGrayColor(COLORREF GrayColor)
{
	TitleGrayColor=GrayColor;
	if(pTabControl) pTabControl->Refresh();
}
//================================================================


void CBookMarkDlg::OnOK()
{
}
