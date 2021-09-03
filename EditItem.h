#pragma once

#if !defined(AFX_EDITITEM_H__532B5463_D15B_4D34_88AF_17F163E3F880__INCLUDED_)
#define AFX_EDITITEM_H__532B5463_D15B_4D34_88AF_17F163E3F880__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditItem.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditItem window

class CEditItem : public CEdit
{
// Construction
public:
	//CEditItem(int nItem, int nSubItem, CString &sContent);
	CEditItem(int nItem, int nSubItem, CString sContent);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditItem)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEditItem();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEditItem)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_nSubItem;
	int m_nItem;
	CString m_sContent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITITEM_H__532B5463_D15B_4D34_88AF_17F163E3F880__INCLUDED_)
