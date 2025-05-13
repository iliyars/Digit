class CStateListBox : public CCheckListBox
{
// Construction
public:
	CStateListBox();

// Attributes
public:

// Operations
public:
   void SetListItems(CStringArray& Str, CUIntArray& State);
   void GetStateList(CUIntArray& State);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBox)
	public:
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStateListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStateListBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
