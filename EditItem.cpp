// EditItem.cpp : implementation file
//

#include "stdafx.h"
#include "EditItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditItem

//CEditItem::CEditItem(int nItem, int nSubItem, CString &sContent)
CEditItem::CEditItem(int nItem, int nSubItem, CString sContent)
{
	m_sContent=sContent;// taroljuk a parametereket
	m_nItem=nItem;
	m_nSubItem=nSubItem;
}

CEditItem::~CEditItem()
{
}


BEGIN_MESSAGE_MAP(CEditItem, CEdit)
	//{{AFX_MSG_MAP(CEditItem)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditItem message handlers

BOOL CEditItem::PreTranslateMessage(MSG* pMsg) //hogy ne az ablak kapja az uzeneteket, hanem az edit control
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if( pMsg->wParam == VK_RETURN || //ezeket az uzeneteket az editbox kapja
			pMsg->wParam == VK_DELETE ||
			pMsg->wParam == VK_ESCAPE ||
			GetKeyState( VK_CONTROL))
		{			
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return 1;
		}	
	}
	
	return CEdit::PreTranslateMessage(pMsg);
}

int CEditItem::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();// beallitjuk a betutipust a szulo objektum alapjan (listctrl)
	SetFont(font);
	SetWindowText(m_sContent);// Beallitjuk a szoveget a parameter alapjan
	SetFocus();	// beallitjuk a fokuszt az edit controllra
	SetSel(0, -1);// kijeloljuk a szoveget
	
	return 0;
}

void CEditItem::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default

	if( nChar == VK_ESCAPE ||   // ha escapet vagy entert nyomunk
		nChar == VK_RETURN)	
	{		
		if( nChar == VK_ESCAPE)SetWindowText(m_sContent);// escape eseten visszaallitjuk az eredeti tartalmat	
		GetParent()->SetFocus(); //a fokuszt visszaallitjuk a listctr-ra
		return;	
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CEditItem::OnKillFocus(CWnd* pNewWnd) // ha elhagyjuk az editboxot
{
	CEdit::OnKillFocus(pNewWnd);


	CString str;
	GetWindowText(str);// az ablak tartalmat taroljuk.
	// Send Notification to parent of ListView ctrl	
	LV_DISPINFO lvDispInfo;
	lvDispInfo.hdr.hwndFrom = GetParent()->m_hWnd;
	lvDispInfo.hdr.idFrom = GetDlgCtrlID();	
	lvDispInfo.hdr.code = LVN_ENDLABELEDIT;
	lvDispInfo.item.mask = LVIF_TEXT;	
	lvDispInfo.item.iItem = m_nItem;
	lvDispInfo.item.iSubItem = m_nSubItem;
	lvDispInfo.item.pszText =  LPTSTR((LPCTSTR)str);
	lvDispInfo.item.cchTextMax = str.GetLength();
	GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(),(LPARAM)&lvDispInfo); 
	DestroyWindow();
	
	// TODO: Add your message handler code here
	
}

void CEditItem::OnNcDestroy() // amikor megszuntetjuk az objektumot felszabaditjuk a memoriat.
{
	CEdit::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete this;
}
