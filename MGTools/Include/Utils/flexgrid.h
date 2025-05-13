// FlexGrid.h: interface for the CFlexGrid class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FLEXGRID_H__D8C62986_AE39_11D2_9104_FE0EE4B0136E__INCLUDED_)
#define AFX_FLEXGRID_H__D8C62986_AE39_11D2_9104_FE0EE4B0136E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "..\..\Include\Utils\NotificationCode.h"
#include "..\..\Include\Utils\fixmem.h"
#include "..\..\Include\Table\msflexgrid.h"
#include "..\..\Include\Table\TComboBox.h"

enum TableDirection { TBL_HORZ=0, TBL_VERT=1 };
enum TableCoord { TBL_ABS=0, TBL_REL=1 };
enum TableControl { TBL_EDIT, TBL_COMBOBOX, TBL_BUTTON };

class CFlexGrid;

class CCell
{
	bool bSelect;
	bool bEnable;
    int TypeControl;
    Mem<CString> sPossibleInput;
    int nIndex;
public:    
	CCell();
    CCell & operator=(CCell & C);

    void SetControl(int Type);
    void Select(bool bSelect);
    void Enable(bool bEneble);
    void ClearList();
    void AddToList(CString S);

    int GetControl() {return TypeControl;}
    bool GetSelect() {return bSelect;}
    bool GetEnable() {return bEnable;}
    CString GetInputString(int nIndex) {return sPossibleInput(nIndex);}

    int GetStringsNumber() {return sPossibleInput.GetSize();}
    int GetIndex() {return nIndex;}
    void SetIndex(int nInd) {nIndex=nInd;}
};

class CTableEdit : public CEdit
{
    CTComboBox m_Combo;
    CFont Font;
public:
	CFlexGrid *pParent;
    bool bDroppedDown;

    void Update();
    void Init();
    CCell & GetCell();

    CTableEdit();

    bool IsDroppedDown() {return bDroppedDown;}
protected:
	// Generated message map functions
	//{{AFX_MSG(CTableEdit)
	afx_msg UINT OnGetDlgCode( );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnSysChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnSysDeadChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnPaint();
    afx_msg void OnMouseMove( UINT, CPoint );
    afx_msg void OnMove(int x, int y);

	DECLARE_MESSAGE_MAP()
	//}}AFX_MSG
};

class CFlexGrid : public CMSFlexGrid
{
protected:
	DECLARE_DYNCREATE(CFlexGrid)
public:
	CFlexGrid();
	virtual ~CFlexGrid();


// Attributes
public:
	int iRowCur;
	int iColCur;
	UINT id_OK;

	bool bEnableEditAll;

protected:
    Mem<CCell> cell;
	CTableEdit m_Edit;

	CBrush EditBkBrush;

	int Align;
	double Cx;
	double Cy;
	CFont EditFont;

	bool bDoShiftInEdit;

	COLORREF BkCursorColor;
	COLORREF TextCursorColor;

// Operations
public:
    CPoint GetCellsNumber() {return CPoint(cell.GetNx(), cell.GetNy());}
	void Encode();
	void Clear(int Coord=TBL_REL);
	void SetColWidth(int iCol, int ColWidth, int Coord=TBL_ABS);
	CString GetTextMatrix(int iRow, int iCol, int Coord=TBL_ABS);
	bool GetRedraw();
	bool bReDraw;
	int GetColWidth(int col, int Coord=TBL_REL);
	int GetRowHeight(int row, int Coord=TBL_REL);
	int GetRowPos(int row, int Coord=TBL_REL);
	int GetColPos(int Col, int Coord=TBL_REL);
	bool IsModify();
	bool bModify;
	COLORREF rgbBkEnabled;
	COLORREF rgbTxtEnabled;
	COLORREF rgbBkDisabled;
	COLORREF rgbTxtDisabled;
	void SetColor(bool State, COLORREF TxtColor=RGB(0,0,0), COLORREF BkColor=-1);
	void SetRow(int row, int Coord=TBL_REL);
	void SetCol(int col, int Coord=TBL_REL);
	int GetCol(int Coord=TBL_REL);
	int GetRow(int Coord=TBL_REL);
	int GetCols(int Coord=TBL_REL);
	int GetRows(int Coord=TBL_REL);
	bool IsInitialized() {return m_Edit.m_hWnd!=0;}
	bool IsSelectCell(int row, int col, int Coord=TBL_REL);
	bool IsEnabledCell(int row, int col, int Coord=TBL_REL);
	int AddColumn(CString ColHeaderText="");
	int AddRow(CString RowHeaderText="");
	void DeleteColumn(int col, int Coord=TBL_REL);
	void DeleteRow(int row, int Coord=TBL_REL);
	virtual bool DoRButtonDown(UINT nFlags, CPoint point ) {return true;}
	virtual void DoMoveToCell() {};

	void Enable(int row, int col, bool bEnable=true, int Coord=TBL_REL);
	bool Enable(bool bEnable);
	void EnableRow(int row, bool bEnable, int Coord=TBL_REL);
	void EnableCol(int col, bool bEnable, int Coord=TBL_REL);

	CCell & GetCell(int row=-1, int col=-1, int Coord=TBL_REL);
	COLORREF GetCellBackColor(int row, int col, int Coord=TBL_REL);
	bool GetEnableAll();
	void Init();
	void InsertColumn(int col, CString HorzHeaderText="", int Coord=TBL_REL);
	void InsertRow(int row, CString VertHeaderText="", int Coord=TBL_REL);
	void MoveEditToCell();
	void Reformat(bool bAlignment=false, int Coord=TBL_REL);
	void Refresh();
	void SetRedraw(bool bNewValue);
	void SelectCell(int row, int col, bool bSelect, int Coord=TBL_REL);
	void SelectRow(int row, bool bSelect, int Coord=TBL_REL);
	void SelectCol(int col, bool bSelect, int Coord=TBL_REL);
	void SetAlign(int Align);
	
	void SetCellBackColor(int row, int col, COLORREF color, int Coord=TBL_REL);
	void SetCellColor(int row, int col, COLORREF TextColor, COLORREF BkColor=-1, int Coord=TBL_REL);

	void SetRowBackColor(int row, COLORREF color, int Coord=TBL_REL);
	void SetColBackColor(int col, COLORREF color, int Coord=TBL_REL);
	void SetRowColor(int row, COLORREF TextColor, COLORREF BkColor=-1, int Coord=TBL_REL);
	void SetColColor(int col, COLORREF TextColor, COLORREF BkColor=-1, int Coord=TBL_REL);

	void SetCols(long nNewValue, int Coord=TBL_REL);
	void SetCursorColor(COLORREF TextColor, COLORREF BkColor);
	void SetFormatString(LPCTSTR lpszNewValue);
	void SetRows(long nNewValue, int Coord=TBL_REL);
	void SetTextMatrix(long Row, long Col, LPCTSTR lpszNewValue, int Coord=TBL_ABS);
	void SetOKId(UINT _id_OK) {id_OK=_id_OK;}


protected:
	virtual LRESULT DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam );
//{{AFX_EVENT(CFlexGrid)
	afx_msg void OnClickTable();
	afx_msg void OnEnterCellTable();
	afx_msg void OnScrollTable();
//}}AFX_EVENT

	DECLARE_EVENTSINK_MAP()

//{{AFX_MSG(CFlexGrid)
	afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
	afx_msg void OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void OnWindowPosChanged( WINDOWPOS* lpwndpos );
	afx_msg void OnSetFocus(CWnd* pOldWnd);
//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FLEXGRID_H__D8C62986_AE39_11D2_9104_FE0EE4B0136E__INCLUDED_)
