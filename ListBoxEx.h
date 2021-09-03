#pragma once

#if !defined(AFX_LISTBOXEX_H__37FAF93C_B0DC_4257_9ECA_FE4F05B482F1__INCLUDED_)
#define AFX_LISTBOXEX_H__37FAF93C_B0DC_4257_9ECA_FE4F05B482F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//
#define WM_LISTBOX_MENU WM_USER + 0x7FA4	
/////////////////////////////////////////////////////////////////////////////
// CListBoxEx window
typedef struct _LISTBOX_COLOR_
{
	CString strText;
	COLORREF fgColor;
	COLORREF bgColor;
	_LISTBOX_COLOR_()
	{
		strText.Empty();
		fgColor = RGB(0, 0, 0);
		bgColor = RGB(255, 255, 255);
	}
}LISTBOX_COLOR, *PLISTBOX_COLOR;

class CListBoxEx : public CListBox
{
// Construction
public:
	CListBoxEx();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBoxEx)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListBoxEx();

	void AppendString(LPCSTR lpszText, COLORREF fgColor, COLORREF bgColor);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	// Generated message map functions
protected:
	//{{AFX_MSG(CListBoxEx)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	//}}AFX_MSG
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__37FAF93C_B0DC_4257_9ECA_FE4F05B482F1__INCLUDED_)
