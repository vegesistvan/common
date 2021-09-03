// ComboItem.cpp : implementation file
//

#include "stdafx.h"
#include "ComboItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CComboItem

CComboItem::CComboItem( int nItem, int nSubItem, CStringList *psList )
{
	m_nItem		= nItem;// taroljuk a parametereket
	m_nSubItem	= nSubItem;
	if ( psList )
		m_sList.AddTail( psList );
}

CComboItem::~CComboItem()
{
}


BEGIN_MESSAGE_MAP(CComboItem, CComboBox)
	//{{AFX_MSG_MAP(CComboItem)
	ON_WM_NCDESTROY()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboItem message handlers

BOOL CComboItem::PreTranslateMessage(MSG* pMsg) //hogy ezeket az uzeneteket a combobox kapja ne az ablak
{
	// TODO: Add your specialized code here and/or call the base class
	
	if( pMsg->message == WM_KEYDOWN )	
	{		
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)	//ezeket az uzeneteket a combo boxnak kuldjuk
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);			
			return 1;
		}	
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

int CComboItem::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CFont* font = GetParent()->GetFont();//betutipus lekerdezese
	SetFont(font);//betutipus beallitasa
	//add the items from CStringlist
	POSITION pos = m_sList.GetHeadPosition();
	while(pos != NULL)
		AddString((LPCTSTR)(m_sList.GetNext(pos)));	
	SetFocus();	
	
	return 0;
}

void CComboItem::OnNcDestroy() //az objektum megszunesekor felszabaditjuk a memoriat
{
	CComboBox::OnNcDestroy();
	
	// TODO: Add your message handler code here
	delete this;
}

void CComboItem::OnCloseup() // combobox becsukasakor visszatesszuk a fokuszt a listcontrolra
{
	// TODO: Add your control notification handler code here
	GetParent()->SetFocus();
}

void CComboItem::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);

	// TODO: Add your message handler code here
	CString str;
	GetWindowText(str);// taroljuk a kivalasztott stringet

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO lvDispInfo;
	lvDispInfo.hdr.hwndFrom = GetParent()->m_hWnd;
	lvDispInfo.hdr.idFrom = GetDlgCtrlID();
	lvDispInfo.hdr.code = LVN_ENDLABELEDIT;
	lvDispInfo.item.mask = LVIF_TEXT;
	lvDispInfo.item.iItem = m_nItem;
	lvDispInfo.item.iSubItem = m_nSubItem;
	lvDispInfo.item.pszText = str.IsEmpty() ? FALSE : LPTSTR((LPCTSTR)str);
	lvDispInfo.item.cchTextMax = str.GetLength();
	
	GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&lvDispInfo);

	// Close the control
	PostMessage(WM_CLOSE);
	
}
