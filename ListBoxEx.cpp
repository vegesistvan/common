// ListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxEx.h"
#include "Color.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx

CListBoxEx::CListBoxEx()
{
}

CListBoxEx::~CListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CListBoxEx, CListBox)
	//{{AFX_MSG_MAP(CListBoxEx)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxEx message handlers

void CListBoxEx::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	GetParent()->SendMessage(WM_LISTBOX_MENU, (WPARAM)0, (LPARAM)&point);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CListBoxEx::AppendString(LPCSTR lpszText, COLORREF fgColor, COLORREF bgColor)
{
	LISTBOX_COLOR* pInfo = new LISTBOX_COLOR;
	pInfo->strText.Format( L"%s", lpszText);
	pInfo->fgColor = fgColor; 
	pInfo->bgColor = bgColor;
	SetItemDataPtr(AddString(pInfo->strText), pInfo);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	// TODO: Add your code to determine the size of specified item
	ASSERT(lpMeasureItemStruct->CtlType == ODT_LISTBOX);
	
	CString strText(L"");
	GetText(lpMeasureItemStruct->itemID, strText);
	ASSERT(TRUE != strText.IsEmpty());

	CRect rect;
	GetItemRect(lpMeasureItemStruct->itemID, &rect);
	
	CDC* pDC = GetDC(); 
	lpMeasureItemStruct->itemHeight = pDC->DrawText(strText, -1, rect, DT_WORDBREAK | DT_CALCRECT); 
	ReleaseDC(pDC);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your code to draw the specified item
	ASSERT(lpDrawItemStruct->CtlType == ODT_LISTBOX);

	LISTBOX_COLOR* pListBox = (LISTBOX_COLOR*)GetItemDataPtr(lpDrawItemStruct->itemID);
	ASSERT(NULL != pListBox);

	CDC dc;
	
	dc.Attach(lpDrawItemStruct->hDC);
	
	// Save these value to restore them when done drawing.
	COLORREF crOldTextColor = dc.GetTextColor();
	COLORREF crOldBkColor = dc.GetBkColor();
	
	// If this item is selected, set the background color 
	// and the text color to appropriate values. Also, erase
	// rect by filling it with the background color.
	if ((lpDrawItemStruct->itemAction | ODA_SELECT) &&
		(lpDrawItemStruct->itemState & ODS_SELECTED))
	{
		dc.SetTextColor(pListBox->bgColor);
		dc.SetBkColor(pListBox->fgColor);
//		dc.SetTextColor( LIGHTBLUE );
//		dc.SetBkColor( WHITE );
//		dc.FillSolidRect(&lpDrawItemStruct->rcItem, pListBox->fgColor);
	}
	else
	{
		dc.SetTextColor(pListBox->fgColor);
		dc.SetBkColor(pListBox->bgColor);
//		dc.SetTextColor( LIGHTBLUE );
//		dc.SetBkColor( WHITE );
		dc.FillSolidRect(&lpDrawItemStruct->rcItem, pListBox->bgColor);

	}
	
	lpDrawItemStruct->rcItem.left += 5;
	// Draw the text.
	
	dc.DrawText(pListBox->strText, pListBox->strText.GetLength(), &lpDrawItemStruct->rcItem, DT_WORDBREAK);
	
	// Reset the background color and the text color back to their
	// original values.
	dc.SetTextColor(crOldTextColor);
	dc.SetBkColor(crOldBkColor);
	
	dc.Detach();	
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CListBoxEx::OnDestroy() 
{
	CListBox::OnDestroy();
	
	// TODO: Add your message handler code here	
	int nCount = GetCount();
	for(int i=0; i<nCount; i++)
	{
		LISTBOX_COLOR* pList = (LISTBOX_COLOR*)GetItemDataPtr(i);
		delete pList;
		pList = NULL;
	}
}
/*


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CLineListBox::AddItem(const CString& str, COLORREF rgbText)
{
   int   nIndex;
   nIndex = AddString(str);
   if( CB_ERR != nIndex )
      SetItemData(nIndex, rgbText);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CListBoxEx::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC			dc;
	CRect		rcItem(lpDIS->rcItem);
	UINT		nIndex = lpDIS->itemID;
	COLORREF	rgbBkgnd = ::GetSysColor( (lpDIS->itemState & ODS_SELECTED) ? COLOR_HIGHLIGHT : COLOR_WINDOW);
	dc.Attach(lpDIS->hDC);
	CBrush br(rgbBkgnd);
	dc.FillRect(rcItem, &br);
	if( lpDIS->itemState & ODS_FOCUS )
		dc.DrawFocusRect(rcItem);
	if( nIndex != (UINT)-1 )
	{
		// The text color is stored as the item data.
	//	COLORREF rgbText = (lpDIS->itemState & ODS_SELECTED) ? ::GetSysColor(COLOR_HIGHLIGHTTEXT) : GetItemData(nIndex);
		COLORREF rgbText = colorAliceBlue;

		CString str;
		GetText(nIndex, str);
		dc.SetBkColor(rgbBkgnd);
		dc.SetTextColor(rgbText);
		dc.TextOut(rcItem.left + 2, rcItem.top + 2, str);
	}
	dc.Detach();
}
void CListBoxEx::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
   // Set the item height. Get the DC, select the font for the
   // list box, and compute the average height.
	CClientDC	dc(this);
	TEXTMETRIC	tm;
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);
	dc.GetTextMetrics(&tm);
	dc.SelectObject(pOldFont);
	lpMIS->itemHeight = tm.tmHeight + 4;   
}
*/