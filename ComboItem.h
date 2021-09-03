#pragma once
#if !defined(AFX_COMBOITEM_H__174250A0_9B04_42CB_9B7B_DCF9F6001FE4__INCLUDED_)
#define AFX_COMBOITEM_H__174250A0_9B04_42CB_9B7B_DCF9F6001FE4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComboItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComboItem window

class CComboItem : public CComboBox
{
// Construction
public:
	CComboItem( int nItem, int nSubItem, CStringList *psList = NULL);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboItem)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CComboItem();

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboItem)
	afx_msg void OnNcDestroy();
	afx_msg void OnCloseup();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CStringList m_sList;
	int m_nSubItem;
	int m_nItem;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMBOITEM_H__174250A0_9B04_42CB_9B7B_DCF9F6001FE4__INCLUDED_)
