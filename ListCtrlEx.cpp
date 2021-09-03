// ListCtrlEx.cpp : implementation file
//
// az osztaly mukodesehez az StdAfx.h fajban incudolni kell a kovetkezot:
// #include <afxtempl.h>
/*
Az osztaly a CListCtrl osztaly kibovitese.
Hasznalata:
publikus fuggvenyek:
InsertColumn		(ugyanugy mukodik, mint az InsertColumn, csak utolso parameternek megadhato az oszlop tipusa)
					Az oszlop tipusok lehetnek:[COL_EDIT, COL_COMBO][COL_NUM, COL_TEXT][COL_HIDDEN] vagy ezek variacioi (editbox tipusu, combobox tipusu, szam tipusu, szoveg tipusu, rejtett)
GetColumnType		Az oszlopszam alapjan visszaadja az oszlop tipusat
SetColumnType		Beallitja az adott oszlop tipusat az altalunk meghatarozottra.
IsColumnVisible		Megmondja, hogy rejtett-e az oszlop
HiddeColumn			Elrejti a megadott oszlopot
ShowColumn			Megjeleniti a megadott oszlopot
SortByHeaderClick   Engedelyezi (vagy tiltja) a rendezest a fejlecre kattintas eseten.
					A rendezes szam, vagy szoveg alapon tortenik a COL_NUM vagy COL_TEXT parameterek alapjan.
					Ha ezek a parameterek nincsenek beallitva, akkor szoveg szerint tortenik a rendezes.
GetRecOrigPos		Visszaadja a megadott sor eredeti poziciojat. Erre akkor lehet szukseg, ha pl. egy adatbazisba vissza akarunk
					irni adatoket, de mar rendeztuk a sorokat valamilyen szempontbol, es nincs kulcs oszlop
KeepSortOrder		Megorzi a rendezesi sorrendet. Ha nincs engedelyezve, akkor elem hozzaadasakor torlodnek a rendezesi szempontok
ReSort				Ujrarendezi a tablazatot. Ha mar megadtunk rendezesi szempontokat, es uj adatokat vettunk fel, akkor lehet erre szukseg.
AutoReSort			TRUE eseten, ha pl. uj sort illesztettunk be, es a KeepSortOrder TRUE, akkor automatikusan ujrarendezi a sorokat. Nem kell meghivni a ReSort fuggvenyt.
					Hatranya, hogy minden uj sor eseten rendezes tortenik, tehat nagy modositasok eseten LASSU lehet!
ResetSortOrder		Visszaallitja az eredeti sorrendet.
SetSortOrder		Rendezesi sorrend kozvetlen beallitasa. Parameterkent egesz szamokat adunk meg vesszovel elvalasztva, melyek az oszlopok azonositoi.
					Az elso parameter a rendezni kivant oszlopok szama, a tobbi parameter sorrendje hatarozza meg a rendezes prioritasat. Negativ szamok forditott
					sorrendet jelolnek. pl: SetSortOrder(3,-1,4,2); harom oszlop szerint lesz rendezett a tablazat. az elso, negyedik es masodik szerint. Az elso forditott
					sorrendben. Fontos, hogy az oszlopok sorszama 1-el kezdodik. Rendezesi parameterkent atadhato egy CIntArray tipusu tomb is. Ebben az esetben
					a tomb elemei tartalmazzak a rendezes sorrendjet. Itt termeszetesen nem kell az elemek szamat megadni.
DeleteByDeleteKey   Engedelyezi a delete billentyuvel valo sortorlest. Amennyiben ez engedelyezett, a soron allva a delete billentyu megnyomasara egy WM_ROWTODELETE
					uzenet kerul elkuldesre. Kozvetlen torles nem tortenik.
AttachDataSet		Egy vectoros TCHAR vagy FIELD elemekbol allo tombot csatol a controllhoz. Ez akkor fontos, ha virtual modba akarjuk hasznalni a clistctrlt (style LVS_OWNERDATA)
					Virtual modban figyelni kell arra, hogy a dataset csatolasa utan ne valtozzanak az adatok. Ha megis valtoztatni kell, akkor eloszor csatoljuk le
					a datasetet, majd csatoljuk vissza. Virtual modban a grid nem tartalmaz valos adatokat ezert az insert fuggvenyek nem mukodnek (hibat generalnak)
					A datasetet az oszlopok letrehozasa utan csatoljuk
DetachDataSet		Lecsatolja a datasetet
IsDatasetAttached	Ertelemszeru
EnableHeaderChkbox  Megjeleníti a fejlécen a checkboxot, es aktivalja az LVS_EX_CHECKBOXES stilust. Csak ha nem virtualis a kontroll (ownerdata=false)
					Csak WINVER 0x0600 vagy kesobbi verzioknal mukodik (vista-tol)
SetSortCaseSense	Be tudjuk vele allitani a szoveg tipusu oszlop rendezesenek fajtalyat (kisbetu/nagybetu szamit vagy nem)

(Az oszlopok felcserelhetoseget a SetExtendedStyle fuggvenynel a LVS_EX_HEADERDRAGDROP parameterrel allithatjuk, mint az eredeti osztalyban)

Az oszlokpok szinet illetve a bennuk tarolt szoveg szinet WM_SET_COLUMN_COLOR uzenetre deffinialt fuggvenyben kell megadni.
Az uzenet kuldesekor 2 parameter kerul atadasra.
	Az elso egy int tipusu, az aktualis oszlop szama
	A masodik egy CELLCOLOR tipusu mutato, mely a LictCtrl.h-ban van deffinialva, es a .bg a hatterszin, a .tx a szovegszin COLORDEF tipusu valtozoja.

A sor szinet illetve a bennuk tarolt szoveg szinet WM_SET_ROW_COLOR uzenetre deffinialt fuggvenyben kell megadni.
Az uzenet kuldesekor 2 parameter kerul atadasra.
	Az elso egy int tipusu, az aktualis sor szama
	A masodik egy CELLCOLOR tipusu mutato, mely a LictCtrl.h-ban van deffinialva, es a .bg a hatterszin, a .tx a szovegszin COLORDEF tipusu valtozoja.

Amennyiben egy cellara sor es oszlopszinbeallitas is vonatkozik, abban az esetben a cella szine a sor szin alapjan lesz beallitva.

Az oszlopokhoz tartozo combolist-eket a WM_SET_COMBO uzenetre deffinialt fuggvenyben kell megadni.
Az uzenet kuldesekor 2 parameter kerul atadasra.
	Az elso egy int tipusu, az aktualis oszlop szama
	A masodik egy COMBODATA struktura tipusu mutato, melyet tartalmazza a sor szamat, es fel kell tolteni a comboboxban megjeleniteni kivant adatokkal.

Az oszlopok modositasarol a WM_ITEM_UPDATED uzenetre deffinialt fuggvenyben ertesulunk.
Az uzenet kuldesekor 2 parameter kerul atadasra.
	Az elso egy int tipusu, mely 0, ha nem tortent valtozas es 1, ha a cella erteke megvaltozott.
	A masodik egy LVITEM tipusu mutato, mely informaciokat tartalmaz az uj cellara vonatkozoan
	- LVITEM->iItem	a megvaltozott cella sor szama
	- LVITEM->iSubItem a megvaltozott cella oszlop szama
	- LVITEM->pszText a megvaltozott cella szovege, melyet mar a GetItem fuggvennyel is lekerdezhetunk

A kontrollra valo jobb gombbal kattintasrol a WM_LISTCTRL_MENU uzenetre deffinialt fuggvenyben ertesulunk.
Az uzenet kuldesekor 2 parameter kerul atadasra.
	Az elso egy int tipusu, megly mindig 0.
	A masodik egy CPoint tipusu mutato, mely informaciot tartalmaz a kattintas helyerol.

Amennyiben a fejlec kattintas alapjan torteno rendezest engedelyeztuk(SortByHeaderClick), akkor a WM_CLICKED_COLUMN uzenet ertesit bennuket,
hogy melyik oszlop szerint rendezett a grid. A rendezett oszlop szamat a wParam valtozo tartalmazza. Ez annak az oszlopnak a szama amelyik fejlecere
utoljara kattintottunk.

A kivalasztott soron allva a delete gomb megnyomasara torlodik a sor (atmenetileg kikapcsoltam ezt a funkciot)

Ha azt akarjuk, hogy a szoveg szerinti rendezes a nyelvi szabalyoknak megfeleloen mukodjon, akkor a programunk elejen be kell allitani a localet.
pl:

#include "locale.h"
setlocale(LC_ALL,_T("Hungarian_Hungary.1250"));

ha a kornyezet (windows) beallitasait szeretnenk hasznalni, akkor
setlocale(LC_ALL,_T(""));
*/


#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
	m_PrevItem=-1;//az elozo kattintaskor kapott sor szama
	m_PrevSubItem=-1;// az elozo kattintaskor kapott oszlop szama
	m_bHeaderSort=FALSE;// nem rendezunk a fejlecre kattintaskor, ha nem allitjuk kulon be
	m_bKeepSortOrder=FALSE;//Ha ez nincs engedelyezve, akkor a tablazat modositasakor a rendezesi iranyok torlodnek
	m_bEnableDeleteKey=FALSE;
	m_bAutoReSort=FALSE;//Engedelyezes eseten minden modositaskor megtortenik az ujrarendezes (ha a keepsortorder is engedelyezve van)
	vSDataSet=NULL;//ha nincs csatolva dataset, akkor mindig visszaallitjuk NULL-ra
	vFDataSet=NULL;//ha nincs csatolva dataset, akkor mindig visszaallitjuk NULL-ra
	m_bHeaderCheckBox=false;//alapesetben a fejlec checkbox nem jelenik meg;
	sortCaseSensitive=true;//alapesetben a rendezes case sensitive
}

CListCtrlEx::~CListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, OnInsertitem)
	ON_WM_CREATE()
	ON_WM_CHAR()
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW,OnCustomDraw)//itt tudjuk beallitani az oszlopok szinet az oszlopszin beallito fuggveny alapjan
	ON_MESSAGE(WM_DATA_CHANGED, OnDataChanged)//ha megvaltozott a listctrl tartalma
	ON_NOTIFY_REFLECT_EX(LVN_GETDISPINFO, OnLvnGetdispinfo)
	ON_NOTIFY(HDN_ITEMSTATEICONCLICK, 0, &OnHdnItemStateIconClick)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, &OnLvnItemchanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	ModifyStyle(0, LVS_REPORT);//LVS_SINGLESEL|LVS_SHOWSELALWAYS|WS_CHILD  <- ezeket lehet, hogy erdemes hasznalni
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT); //csak akkor mukodik, ha az egesz sor kijeloles aktiv
	CListCtrl::PreSubclassWindow();
}

int CListCtrlEx::InsertColumn(int nCol, const LVCOLUMN* pColumn, int nColType)// az insertcolumn fuggveny kibovitese az editalhatosaggal
{
	int retval=CListCtrl::InsertColumn(nCol, pColumn);
	if(retval != -1)
	{
		if((nColType&COL_HIDDEN)==COL_HIDDEN)
			SetColumnVisibility(nCol,FALSE);
		m_ColType.InsertAt(nCol,nColType);
		SendMessage(WM_DATA_CHANGED,(WPARAM)TRUE,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg
	}
	return retval;
}

int CListCtrlEx::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat, int nWidth, int nSubItem, int nColType)
{
	int retval=CListCtrl::InsertColumn(nCol, lpszColumnHeading, nFormat, nWidth, nSubItem);
	if(retval != -1)
	{
		if((nColType&COL_HIDDEN)==COL_HIDDEN)
			SetColumnVisibility(nCol,FALSE);
		m_ColType.InsertAt(nCol,nColType);
		SendMessage(WM_DATA_CHANGED,(WPARAM)TRUE,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg

	}
	return retval;
}

BOOL CListCtrlEx::DeleteColumn(int nCol)//oszlop torlese a tipus torlesevel egyutt
{
	if(CListCtrl::DeleteColumn(nCol))
	{
		if(nCol<0||nCol>m_ColType.GetUpperBound())return FALSE;
		m_ColType.RemoveAt(nCol,1);
		SendMessage(WM_DATA_CHANGED,(WPARAM)TRUE,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg
		return TRUE;
	}
	return FALSE;
}

int CListCtrlEx::GetColumnType(int nCol)// az oszlop editalhatosaganak lekerdezese
{
	if(nCol<0||nCol>m_ColType.GetUpperBound())return 0;
	int value=m_ColType.GetAt(nCol);
	if(value<0)value=0;
	return value;
}

BOOL CListCtrlEx::SetColumnType(int nCol, int nColType)// az oszlop editalhatosaganak utolagos beallitasa
{
	if(nCol<0||nCol>m_ColType.GetUpperBound())return FALSE;
	m_ColType.SetAt(nCol,nColType);
	SetColumnVisibility(nCol,!((nColType&COL_HIDDEN)==COL_HIDDEN));
	SendMessage(WM_DATA_CHANGED,(WPARAM)TRUE,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg
	return TRUE;
}

BOOL CListCtrlEx::SetColumnVisibility(int nCol, bool bShow)
{

	int nColCount = GetHeaderCtrl()->GetItemCount();
	int* pOrderArray = new int[nColCount];
	VERIFY( GetColumnOrderArray(pOrderArray, nColCount) );
	if(bShow)//lathatova tesszuk
	{
		int nCurIndex = -1;
		for(int i = 0; i < nColCount ; ++i)
		{
			if (pOrderArray[i]==nCol)
				nCurIndex = i;
			else
			if (nCurIndex!=-1)
			{
				// We want to move it to the original position,
				// and after the last hidden column
				//if ( (i <= columnState.m_OrgPosition) || !IsColumnVisible(pOrderArray[i]))
				if (!IsColumnVisible(pOrderArray[i]))
				   
				{
					pOrderArray[nCurIndex] = pOrderArray[i];
					pOrderArray[i] = nCol;
					nCurIndex = i;
				}
			}
		}

		SetColumnWidth(nCol, 100);
	}
	else//elrejtjuk
	{
		int nCurIndex(-1);
		for(int i = nColCount-1; i >=0 ; --i)
		{
			if (pOrderArray[i]==nCol)
			{
	//			// Backup the current position of the column
	//			columnState.m_OrgPosition = i;
				nCurIndex = i;
			}
			else
			if (nCurIndex!=-1)
			{
				pOrderArray[nCurIndex] = pOrderArray[i];
				pOrderArray[i] = nCol;
				nCurIndex = i;
			}
		} 
		SetColumnOrderArray(nColCount, pOrderArray);
		SetColumnWidth(nCol, 0); 
	}
	delete [] pOrderArray;
	return TRUE;
}

BOOL CListCtrlEx::IsColumnVisible(int nCol)
{
	return ((GetColumnType(nCol)&COL_HIDDEN)!=COL_HIDDEN);

}

void CListCtrlEx::HiddeColumn(int nCol)
{
	SetColumnType(nCol,GetColumnType(nCol)|COL_HIDDEN);
}

void CListCtrlEx::ShowColumn(int nCol)
{
	SetColumnType(nCol,(GetColumnType(nCol)|COL_HIDDEN)^COL_HIDDEN);
}

BOOL CListCtrlEx::OnClick(NMHDR* pNMHDR, LRESULT* pResult) //ha egy sorra kattintunk
{
	// TODO: Add your control notification handler code here
	if( GetFocus() != this) //atallitjuk a focust a listviewre(ha nincs ott);
	SetFocus();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR; // a megklikkelt listview-ra allitjuk a pNMListView mutatot


	if(pNMListView->iItem==m_PrevItem && pNMListView->iSubItem==m_PrevSubItem)// ha egy kijelolt sorra kattintunk
	{

		DoControl(pNMListView->iItem,pNMListView->iSubItem,GetColumnType(pNMListView->iSubItem));

	}
	else
	{
		m_PrevItem=pNMListView->iItem;
		m_PrevSubItem=pNMListView->iSubItem;
	}


	*pResult = 0;
	return FALSE;
}

void CListCtrlEx::DeleteByDeleteKey(BOOL bDelete)
{
	m_bEnableDeleteKey=bDelete;
}

void CListCtrlEx::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) // delete gombbal torlunk egy sort
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here

	switch(pLVKeyDow->wVKey)
	{
		case VK_DELETE: 
			{
				if(!m_bEnableDeleteKey)  //ha nem engedelyezzuk a delete gombra torteno torlest
					break;

				GetParent()->SendMessage(WM_ROWTODELETE, (WPARAM)NULL, (LPARAM)NULL);// uzenet kuldese
					
				//	int nItem = GetSelectionMark();// a selected sor indexe
			//	int nItem = (int) GetFirstSelectedItemPosition(); //ezt is hasznalhatjuk
			//	if(nItem!=-1) // valid item 					
			//	{
			//		GetParent()->SendMessage(WM_ROWTODELETE, (WPARAM)nItem, (LPARAM)NULL);// uzenet kuldese a torolt sor szamaval
			//	}
			}	break;

		default :break;
	}
	
	*pResult = 0;
}

void CListCtrlEx::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) //ezt az uzenetet kuldtuk, amikor elhagytuk az editboxot vagy a comboboxot
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	LV_ITEM *plvItem = &pDispInfo->item;

	if( plvItem->iItem != -1 &&  // valid item
		plvItem->pszText )		// valid text
	{
		BOOL Changed=FALSE;
		CString oldString=GetItemText(plvItem->iItem, plvItem->iSubItem);
		if(oldString.Compare(plvItem->pszText)!=0)
		{
			Changed=TRUE;

			if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
				SetItemText( plvItem->iItem, plvItem->iSubItem, plvItem->pszText);// beallitjuk a listctrl megfelelo cellajat a kapott struktura alapjan
			else
			{
				int vNum=GetHeaderCtrl()->GetItemCount()*m_ItemOrder[plvItem->iItem]+plvItem->iSubItem;
				
				if(vSDataSet)//ha string datasetet csatoltunk
				{
					delete[] vSDataSet->operator [](vNum);
					int tLen=_tcslen(plvItem->pszText)+1;
					LPTSTR newStr=new TCHAR[tLen];
					_tcscpy_s(newStr,tLen,plvItem->pszText);
					vSDataSet->operator [](vNum)=newStr;
				}
				else if(vFDataSet)//ha field tipusu datasetet csatoltunk
				{
					delete[] vFDataSet->operator [](vNum).fText;
					int tLen=_tcslen(plvItem->pszText)+1;
					LPTSTR newStr=new TCHAR[tLen];
					_tcscpy_s(newStr,tLen,plvItem->pszText);
					vFDataSet->operator [](vNum).fText=newStr;
					vFDataSet->operator [](vNum).fValue=_tstof(newStr);
				}
				Invalidate();			
			}
		}
		GetParent()->SendMessage(WM_ITEM_UPDATED, (WPARAM)Changed, (LPARAM)plvItem);// uzenet kuldese a mezo modositasarol
		SendMessage(WM_DATA_CHANGED,(WPARAM)Changed,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg
	}
	*pResult = 0;
}

BOOL CListCtrlEx::DoControl(int nItem, int nSubItem, int nType)// az oszlop tipusa alapjan meghivjuk a megfelelo modosio controllt
{
	  switch ( nType&(COL_EDIT|COL_COMBO) )
      {
         case COL_EDIT://editbox
            return EditItem(nItem, nSubItem);
            break;
         case COL_COMBO://combobox read only
			return ComboItem(nItem, nSubItem);
            break;
         default:
            return FALSE;
      }
}

BOOL CListCtrlEx::EditItem(int nItem, int nSubItem)// editbox deffiniallasa a cellara
{
	if(!EnsureVisible(nItem, TRUE))// ha nem lathato az item (pl. scroll)
		return FALSE;	
	
	CRect rect;	
	GetSubItemRectEx(nItem, nSubItem, rect); //meghatarozzuk a subitem teglalapjat

	LV_COLUMN lvcol;// oszlopinformacio tipusu valtozo
	lvcol.mask = LVCF_FMT;
	GetColumn(nSubItem, &lvcol);// lekerjuk a cellahoz tartozo oszlopinfot

	DWORD dwStyle;// ebbe a valtozoba beallitjuk, hogy a cella szovege jobbra, balra, vagy kozepre van igazitva
	if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else 
		dwStyle = ES_CENTER;

	dwStyle |=WS_BORDER|WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL; // a stilust kibovitjuk meg nehany parameterrel
	CEdit *pEdit = new CEditItem(nItem, nSubItem, GetItemText(nItem, nSubItem));// letrehozzuk a cella alapjan az editbox objektumot
#define IDC_EDITCTRL 0x1234
	pEdit->Create(dwStyle, rect, this, IDC_EDITCTRL);//letrehozzuk az editboxot

	return TRUE;
}

BOOL CListCtrlEx::ComboItem(int nItem, int nSubItem)//combobox deffiniallasa a cellara
{
	COMBODATA d_Combo;// a combo list sor, oszlop informaciojat es tartalmat tarolja
	d_Combo.nItem=nItem;// a sor szamanak beallitasa

	CString strFind = GetItemText(nItem, nSubItem);//a cella tartalma

	// Make sure that the item is visible
	if( !EnsureVisible(nItem, TRUE)) 
		return FALSE;
	
	CRect rect;
	GetSubItemRectEx(nItem, nSubItem, rect);//meghatarozzuk a subitem teglalapjat
	rect.bottom += 10 * rect.Height();//dropdown area
	
	DWORD dwStyle =  WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;// combobox stilusa

	GetParent()->SendMessage(WM_SET_COMBO, (WPARAM)nSubItem, (LPARAM)&d_Combo);
	CComboBox *pCombo = new CComboItem(nItem, nSubItem, &d_Combo.m_strList);

#define IDC_COMBOBOXINLISTVIEW 0x1235
	pCombo->Create(dwStyle, rect, this, IDC_COMBOBOXINLISTVIEW);
//	pCombo->ModifyStyleEx(0,WS_EX_CLIENTEDGE);//can we tell at all
	pCombo->ShowDropDown();
	pCombo->SelectString(-1, strFind.GetBuffer(1));//a comboboxot beallitjuk ugy, hogy kikeressuk a listabol a cella tartalmanak megfelelo erteket
//	pCombo->SetCurSel(0);

	// The returned pointer should not be saved
	return TRUE;
}

void CListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) // hogy editalas megszunjon, ha scrollozunk
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this )
	SetFocus();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) // hogy editalas megszunjon, ha scrollozunk
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this )
	SetFocus();
	
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CListCtrlEx::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)  // hogy editalas megszunjon, ha scrollozunk
{
	// TODO: Add your message handler code here and/or call default
	if( GetFocus() != this )
	SetFocus();
		
	return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CListCtrlEx::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) // hogy editalas megszunjon, ha atmeretezzuk az oszlopot, illetve 
{							// az oszlopok felcserelesekor kihagyjuk a rejtett oszlopokat, 
							// tiltjuk a rejtett oszlopok atmeretezeset
	// TODO: Add your specialized code here and/or call the base class

	HD_NOTIFY   *pHDN = (HD_NOTIFY*)lParam;
	if((pHDN->hdr.code == HDN_BEGINTRACKW || pHDN->hdr.code == HDN_BEGINTRACKA || pHDN->hdr.code ==HDN_DIVIDERDBLCLICKA || pHDN->hdr.code ==HDN_DIVIDERDBLCLICKW))
	{
		if(GetFocus() != this)
		SetFocus();//hogy az editalas megszunjon
		if (!IsColumnVisible(pHDN->iItem)) //a rejtett oszlopot nem engedjuk atmeretezni
		{
			*pResult = TRUE;                // ha nem akarjuk, hogy atmeretezodjon
			return TRUE;                    // 
		}
	}
	if(pHDN->hdr.code == HDN_ENDDRAG)// az oszlopok csereje eseten
	{

		NMHEADER* pNMH = (NMHEADER*)lParam;
		if (pNMH->pitem->mask & HDI_ORDER)
		{
			// Correct iOrder so it is just after the last hidden column
			int nColCount = GetHeaderCtrl()->GetItemCount();
			int* pOrderArray = new int[nColCount];
			VERIFY( GetColumnOrderArray(pOrderArray, nColCount) );

			for(int i = 0; i < nColCount ; ++i)
			{
				if (IsColumnVisible(pOrderArray[i]))
				{
	                pNMH->pitem->iOrder = max(pNMH->pitem->iOrder,i);
					break;
				}
			}
		delete [] pOrderArray;
		}
		return FALSE;

	}

	return CListCtrl::OnNotify(wParam, lParam, pResult);
}

BOOL CListCtrlEx::GetSubItemRectEx(int nItem, int nSubItem, CRect &rect)// a GetSubItemRect rosszul adja vissza a 0. subitem meretet, ezert maskepp csinaljuk
{
    CRect rowRect;
	if (GetItemRect(nItem, rowRect, LVIR_BOUNDS)==FALSE)
        return FALSE;

    // Find the left and right of the cell-rectangle using the CHeaderCtrl
    CRect colRect;
    if (GetHeaderCtrl()->GetItemRect(nSubItem, colRect)==FALSE)
        return FALSE;

    // Adjust for scrolling
    colRect.left -= GetScrollPos(SB_HORZ);
    colRect.right -= GetScrollPos(SB_HORZ);

    rect.left = colRect.left;
    rect.top = rowRect.top;
    rect.right = colRect.right;
    rect.bottom = rowRect.bottom;
    return TRUE;
}

BOOL CListCtrlEx::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) //a fejlec kattintasra rajzolunk (rendezunk)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if (m_bHeaderSort)		//ha automatikus sorbarendezes engedelyezett
	{
		int clickedColumn=pNMListView->iSubItem+1;//a 0. oszlopnal nem tudjuk az elojellel jelezni a rendezes iranyat, ezert a 0. oszlopbol 1. csinalunk.
		if(!(GetKeyState(VK_CONTROL) & 0x80))//ha nem nyomjuk kozben a ctrl gombot
		{
			if(m_SortColumns.GetSize() > 1)// ha tobb oszlop szerint rendeztunk eddig
			{
				int tempSC=m_SortColumns.GetAt(0);// akkor csak a fo oszolpot tartjuk meg
				m_SortColumns.RemoveAll();
				m_SortColumns.Add(tempSC);
			}
			if(m_SortColumns.GetSize() != 0)// ha mar van rendezesi oszlop
				if(abs(m_SortColumns.GetAt(0))!=clickedColumn)// ha nem rendezett oszlopra kattintottunk
				{
					m_SortColumns.SetAt(0, -clickedColumn);//atallitjuk a rendezest erre az oszlopra
				}
		}
		m_SortColumns.Add(clickedColumn);//hozzaadjuk az uj elemet a vegehez
		if(m_SortColumns.GetSize() > 0)//ha meg eddig nem volt rendezve	 (GetItemCount() > 1) //ha mar rendezve volt, (es a sorok szama 1 nel tobb) <- ezt kihagyjuk. A rendezeskor vizsgaljuk
		{
			for (int i=0; i<m_SortColumns.GetSize()-1; i++)// vegigmegyunk a tombbon
			{
				if(abs(m_SortColumns.GetAt(i))==clickedColumn)// ha a klikkelt oszlop szerint mar rendezve van, akkor forditunk a rendezesen
				{
					m_SortColumns.SetAt(i,-m_SortColumns.GetAt(i));
					m_SortColumns.RemoveAt(m_SortColumns.GetSize()-1);//ha a tombben az utolso elem elott megtalaltuk mar ezt az oszlopszamot, akkor toroljuk az utolso elemet
					break;
				}
			}
		}
		GetParent()->SendMessage(WM_CLICKED_COLUMN, (WPARAM)pNMListView->iSubItem, (LPARAM)NULL);// uzenet kuldese a rendezesrol
		Sorting();
		SetSortArrow();
	}
	*pResult = 0;
	return FALSE;
}

void CListCtrlEx::CreateSortIcons()
{
	CBrush brush(GetSysColor( COLOR_GRAYTEXT ));
	CPen pen( PS_SOLID, 1, GetSysColor( COLOR_GRAYTEXT ) );
	CPoint Pt[3];

//	COLORMAP cm = {RGB(0, 0, 0), GetSysColor(COLOR_GRAYTEXT)};
	CBitmap	m_bigUpArrow;
	CBitmap	m_bigDownArrow;
	CBitmap m_littleArrow;

	CDC* pDC = GetDC();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CRect fullRect(0,0,23,16);
	CRect numberRect(7,0,23,16);
	CFont hf;
	hf.CreateFont(12, 0, 0, 0, 0, FALSE, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
	CString str;


//******** nagy nyilak
	m_bigUpArrow.CreateCompatibleBitmap(pDC, fullRect.Width(), fullRect.Height());
	m_bigDownArrow.CreateCompatibleBitmap(pDC, fullRect.Width(), fullRect.Height());

	memDC.SelectObject(&m_bigUpArrow);
	memDC.FillSolidRect(fullRect,RGB(255,255,255));
	memDC.SelectObject( &pen );
	memDC.SelectObject(&brush);
	Pt[0] = CPoint(8,  2);	// Top
	Pt[1] = CPoint(3,  11);	// Left
	Pt[2] = CPoint(13,  11);// Right
	memDC.Polygon(Pt,3);

	memDC.SelectObject(&m_bigDownArrow);
	memDC.FillSolidRect(fullRect,RGB(255,255,255));
	memDC.SelectObject( &pen );
	memDC.SelectObject(&brush);
	Pt[0] = CPoint(8,  11);	// Bottom
	Pt[1] = CPoint(3,  2);	// Left
	Pt[2] = CPoint(13,  2);	// Right
	memDC.Polygon(Pt,3);
//*********
	m_imglstSortIcons.Create (23, 16, ILC_COLOR24 | ILC_MASK, 2, 0);

	memDC.DeleteDC();

	if(m_SortColumns.GetAt(0)>0)//rendezes iranya szerint valasztunk nagy nyilat
		m_imglstSortIcons.Add(&m_bigUpArrow, RGB(255, 255, 255));
	else
		m_imglstSortIcons.Add(&m_bigDownArrow, RGB(255, 255, 255));

	for(int i=1;i<m_SortColumns.GetSize();i++)//ha nem csak 1 oszlop szerint rendezunk
	{
		memDC.CreateCompatibleDC(pDC);
		m_littleArrow.CreateCompatibleBitmap(pDC, fullRect.Width(), fullRect.Height());
		memDC.SelectObject(&m_littleArrow);
		memDC.FillSolidRect(fullRect,RGB(255,255,255));
		memDC.SelectObject( &pen );
		memDC.SelectObject(&brush);

		if(m_SortColumns.GetAt(i)<0)//megrajzoljuk a kisnyilat a rendezes iranyanak megfeleloen
		{
			Pt[0] = CPoint(3,  10);	// Bottom
			Pt[1] = CPoint(1,  6);	// Left
			Pt[2] = CPoint(5,  6);  // Right
		}
		else
		{
			Pt[0] = CPoint(3,  6);	// Top
			Pt[1] = CPoint(1,  10);	// Left
			Pt[2] = CPoint(5,  10); // Right
		}
		
		memDC.Polygon(Pt,3);

		memDC.SelectObject(hf);
		memDC.SetTextColor(GetSysColor( COLOR_GRAYTEXT ));
		str.Format(_T("%d"),i+1);//meghatarozzuk, hogy hanyadik rendezesi szempont
		memDC.DrawText(str,-1,numberRect,DT_SINGLELINE | DT_LEFT |DT_VCENTER);//a kis nyil melle irjuk a numberRect altal meghatarozott teglalapba

		memDC.DeleteDC();

		m_imglstSortIcons.Add(&m_littleArrow, RGB(255, 255, 255));//a megrajzolt ikonokat felvesszuk az ikon listara

		m_littleArrow.DeleteObject();
	}

	GetHeaderCtrl()->SetImageList(&m_imglstSortIcons);//az ikonlistat hozzarendeljuk a headerhez
}

void CListCtrlEx::SetSortArrow()//felrajzoljuk a rendezes iranyat
{
	if(m_imglstSortIcons)//ujracsinaljuk az image list-et
		m_imglstSortIcons.DeleteImageList();

	if(m_SortColumns.GetSize())//ha van rendezesi oszlop
		CreateSortIcons();//letrehozzuk a nyilakat

	for(int i = 0; i < GetHeaderCtrl()->GetItemCount(); ++i) // vegigmegyunk az oszlopokon
	{
		HDITEM hditem = {0};
		hditem.mask = HDI_FORMAT | HDI_IMAGE;
		VERIFY( GetHeaderCtrl()->GetItem(i, &hditem ));
		hditem.iImage=-2; //I_IMAGENONE ha nincs image. vc6 alatt nincs deffinialva. (ez a default)

		for (int z=0;z<m_SortColumns.GetSize();z++)//vegigmegyunk a rendezesi oszlokok szamat tartalmazo tombbon
		{
			if(abs(m_SortColumns.GetAt(z))-1 == i)//ha epp olyan oszlopon allunk, ami rendezett, akkor felrajzoljuk ra az ikonlistaban szereplo kepet
			{
				hditem.iImage = z;
				//hditem.fmt = hditem.fmt & HDF_JUSTIFYMASK | HDF_IMAGE | HDF_STRING | HDF_BITMAP_ON_RIGHT;
				hditem.fmt |= HDF_IMAGE | HDF_STRING | HDF_BITMAP_ON_RIGHT;
			}
		}

		if(hditem.iImage<0)//ha nem rajzoltunk ra semmit, akkor ezt kell csinalni
		{
			hditem.fmt &= ~(HDF_IMAGE | HDF_BITMAP_ON_RIGHT);
			hditem.mask &= ~HDI_IMAGE;
		}

		VERIFY( CListCtrl::GetHeaderCtrl()->SetItem( i, &hditem ) );// akkor hozzarendeljuk.
	}
}

void CListCtrlEx::EnableHeaderChkbox(bool state)
{
	if(GetStyle() & LVS_OWNERDATA)//ha virtualiskent hasznaljuk
		return;

	m_bHeaderCheckBox=state;

	HWND header = ListView_GetHeader(this->GetSafeHwnd());
	DWORD dwHeaderStyle = GetWindowLong(header, GWL_STYLE);

	if(m_bHeaderCheckBox)
	{
		SetExtendedStyle(GetExtendedStyle()|LVS_EX_CHECKBOXES);
		dwHeaderStyle |= HDS_CHECKBOXES;
	}
	else
	{
		SetExtendedStyle(GetExtendedStyle()&~LVS_EX_CHECKBOXES);
		dwHeaderStyle &= ~HDS_CHECKBOXES;
	}

	SetWindowLong(header, GWL_STYLE, dwHeaderStyle);
}

void CListCtrlEx::OnHdnItemStateIconClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER pnmHeader = (LPNMHEADER)pNMHDR;

    if (pnmHeader->pitem->mask & HDI_FORMAT && pnmHeader->pitem->fmt & HDF_CHECKBOX)
	{
		bool state=!(pnmHeader->pitem->fmt & HDF_CHECKED);

		for (int nItem = 0; nItem < GetItemCount(); nItem++)
			SetCheck(nItem,state);
		
//		HWND header = ListView_GetHeader(this->GetSafeHwnd());
		HDITEM hdi = { 0 };
		hdi.mask = HDI_FORMAT;
		VERIFY( GetHeaderCtrl()->GetItem(0, &hdi ));
		
		if (state)
			hdi.fmt |= HDF_CHECKED;
		else	    
			hdi.fmt &= ~HDF_CHECKED;
		
		VERIFY( GetHeaderCtrl()->SetItem( 0, &hdi ) );
    }
}

BOOL CListCtrlEx::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

    if ((pNMLV->uChanged & LVIF_STATE) && m_bHeaderCheckBox)
	{
//		HWND header = ListView_GetHeader(this->GetSafeHwnd());
		HDITEM hdi = { 0 };
		hdi.mask = HDI_FORMAT;
		VERIFY( GetHeaderCtrl()->GetItem(0, &hdi ));

		bool hState=(hdi.fmt&HDF_CHECKED)!=0;
		
		if (hState)
		{
			for (int nItem = 0; nItem < GetItemCount(); nItem++)
			{
				if(!GetCheck(nItem) && hState)
				{
					hdi.fmt &= ~HDF_CHECKED;
					break;
				}
			}
		}
		if(!hState)
		{
			hState=true;
			for (int nItem = 0; nItem < GetItemCount(); nItem++)
			{
				if(!GetCheck(nItem))
				{
					hState=false;
					break;
				}
			}
			if(hState)
			{
				hdi.fmt |= HDF_CHECKED;
			}
		}
		VERIFY( GetHeaderCtrl()->SetItem( 0, &hdi ) );
    }
	*pResult = 0;
	return false;
}

void CListCtrlEx::AutoReSort(BOOL bASort)
{
	m_bAutoReSort=bASort;
}

void CListCtrlEx::SortByHeaderClick(BOOL bSort)
{
	m_bHeaderSort=bSort;
}

void CListCtrlEx::KeepSortOrder(BOOL rStatus)
{
	m_bKeepSortOrder=rStatus;
}

void CListCtrlEx::ResetSortOrder()
{
	if(m_SortColumns.GetSize())//ha van valamilyen rendezesi sorrend beallitva
	{
		m_SortColumns.RemoveAll();//toroljuk a rendezesi oszlopok listajat
	}
	SetSortArrow();//toroljuk a fejlec nyilakat
}

void CListCtrlEx::ReSort()//ha valtozott valami adat, akkor ezzel lehet ujrarendezni az oszlopokat
{
	//if (m_bKeepSortOrder && m_bHeaderSort && (m_SortColumns.GetSize()))
	if (m_bKeepSortOrder && (m_SortColumns.GetSize()))
	{
		Sorting();
		SetSortArrow();
	}
}

int CALLBACK CListCtrlEx::CompareFunct(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)//a rendezeshez szukseges callback fuggveny.
{
    if (lParam1 < lParam2) { return -1; }
    if (lParam1 > lParam2) { return  1; }
    return  0;
}

int CListCtrlEx::CompareData (const void *pInfo,const void * a, const void * b)//ez hasonlitja ossze a rendezeshez szukseges adatokat
{
	double retVal;

	if( ((DOINF*)pInfo)->pFSArray)//ha statikus a listctrl
	{
		if( ((DOINF*)pInfo)->typeArray[((DOINF*)pInfo)->tmpCurLevel] ) //ha szoveg
		{
			if(((DOINF*)pInfo)->sortCaseSensit)// ha case sensitive a rendezes
				retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcscoll( ((DOINF*)pInfo)->pFSArray->operator [](*(int*)a*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fText , ((DOINF*)pInfo)->pFSArray->operator [](*(int*)b*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fText );//TCHAR* vector szoveg alapjan hasonlit
			else
				retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcsicoll( ((DOINF*)pInfo)->pFSArray->operator [](*(int*)a*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fText , ((DOINF*)pInfo)->pFSArray->operator [](*(int*)b*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fText );//TCHAR* vector szoveg alapjan hasonlit
		}
		else// ha szam
		{
			retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * ( ((DOINF*)pInfo)->pFSArray->operator [](*(int*)a*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fValue - ((DOINF*)pInfo)->pFSArray->operator [](*(int*)b*((DOINF*)pInfo)->colOrderNum+((DOINF*)pInfo)->tmpCurLevel).fValue  );//TCHAR* tipusu vector float alapjan hasonlit
		}

	}
	else//ha dinamikus a listctrl
	{
		if( ((DOINF*)pInfo)->typeArray[((DOINF*)pInfo)->tmpCurLevel] ) //ha szoveg
		{
			if(((DOINF*)pInfo)->pSArray)
				if(((DOINF*)pInfo)->sortCaseSensit)// ha case sensitive a rendezes
					retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcscoll( ((DOINF*)pInfo)->pSArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) , ((DOINF*)pInfo)->pSArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) );//TCHAR* vector szoveg alapjan hasonlit
				else
					retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcsicoll( ((DOINF*)pInfo)->pSArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) , ((DOINF*)pInfo)->pSArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) );//TCHAR* vector szoveg alapjan hasonlit
			else
				if(((DOINF*)pInfo)->sortCaseSensit)// ha case sensitive a rendezes
					retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcscoll( ((DOINF*)pInfo)->pFArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fText , ((DOINF*)pInfo)->pFArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fText );//TCHAR* vector szoveg alapjan hasonlit
				else
					retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * _tcsicoll( ((DOINF*)pInfo)->pFArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fText , ((DOINF*)pInfo)->pFArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fText );//TCHAR* vector szoveg alapjan hasonlit
		}
		else// ha szam
		{
			if(((DOINF*)pInfo)->pSArray)
				retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * ( _tstof( ((DOINF*)pInfo)->pSArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) ) - _tstof( ((DOINF*)pInfo)->pSArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1) ) );//TCHAR* tipusu vector float alapjan hasonlit
			else
				retVal=(-2*(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel]<0)+1) * ( ((DOINF*)pInfo)->pFArray->operator [](*(int*)a*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fValue - ((DOINF*)pInfo)->pFArray->operator [](*(int*)b*((DOINF*)pInfo)->colNum+abs(((DOINF*)pInfo)->signArray[((DOINF*)pInfo)->tmpCurLevel])-1).fValue  );//TCHAR* tipusu vector float alapjan hasonlit
		}
	}

	if(retVal==0)
	{
		if( ((DOINF*)pInfo)->tmpCurLevel<(((DOINF*)pInfo)->colOrderNum-1) )
		{
			((DOINF*)pInfo)->tmpCurLevel++;
			retVal=CompareData(pInfo,a,b);
			((DOINF*)pInfo)->tmpCurLevel--;
		}
	}

	return (retVal>0)-(retVal<0);

	//ha a>b ,az eredmeny: +
	//ha a<b ,az eredmeny: -
	//ha a=b ,az eredmeny: 0
}

void CListCtrlEx::SetSortCaseSense(bool caseSense)
{
	sortCaseSensitive=caseSense;
}

void CListCtrlEx::Sorting()//rendezes a klikkelt oszlop szerint
{
	int nItems=GetItemCount();//ennyi sor van a gridben
	if(nItems<1)// ha nincs egy sor sem, akkor nem rendezunk
		return;
	
	int* xArray=new int[nItems];//xArray a sorrendet tarolja az eredetihez kepest (index tomb)
	for (int i=0;i<nItems;i++)//a default sorrendel feltoltjuk a tombot. Ezt fogjuk rendezni
		xArray[i]=i;

	int sortColNum=m_SortColumns.GetSize();//rendezendo oszlopok szama
	int* xSign=new int[sortColNum];//tomb a rendezendo oszlopszamokkal
	for (int i=0;i<sortColNum;i++)
		xSign[i]=m_SortColumns.GetAt(i);//a rendezento oszlopok sorszama es iranya

	//int sortColType=m_SortColumns.GetSize();//rendezendo oszlopok szama
	int* xType=new int[sortColNum];//tomb a rendezendo oszlop tipusaival
	for (int i=0;i<sortColNum;i++)
	{
		int xT=GetColumnType(abs(m_SortColumns.GetAt(i))-1);//a rendezendo oszlopok tipusai (szoveg vagy szam)
		if((xT&COL_NUM)==COL_NUM)
			xType[i]=0;
		else
			xType[i]=1;
	}

	DOINF sortInfo;
	sortInfo.colNum=GetHeaderCtrl()->GetItemCount();//osszes oszlop szama
	sortInfo.colOrderNum=sortColNum;//rendezendo oszlopok szama
	sortInfo.pSArray=vSDataSet;//ez a vektor tomb ha TCHAR* -al hasznaljuk (kulonben null)
	sortInfo.pFArray=vFDataSet;//ez a vektor tomb ha FIELD struct-al hasznaljuk (kulonben null)
	sortInfo.iArray=xArray;//ez az index tomb. az igazi rendezes ezen tortenik
	sortInfo.signArray=xSign;//ez a rendezesi info tomb
	sortInfo.typeArray=xType;//ez az oszlop elemeinek tipusa tomb (0 szam, 1 szoveg)
	sortInfo.sortCaseSensit=sortCaseSensitive;//a szovegek rendezesi szabalya

	std::vector<FIELD> tmpDataset;
	if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
	{
		double cNum;
		for(int i=0;i<nItems;i++)
			for (int z=0;z<sortColNum;z++)
			{
				CString sT=GetItemText(i,abs(m_SortColumns.GetAt(z))-1);
				TCHAR* cStr=new TCHAR[sT.GetLength()+1];
				_tcscpy_s(cStr,sT.GetLength()+1,sT.GetBuffer());
				
				if((GetColumnType(abs(m_SortColumns.GetAt(z))-1)&COL_NUM)==COL_NUM)
					cNum=_tstof(cStr);					
				else
					cNum=0;
					
				tmpDataset.push_back(FIELD(cStr,cNum));//feltoltunk egy tombot az adatbazis rendezni kivant adataival
			}
		sortInfo.pFSArray=&tmpDataset;
	}

	CsfqSort sfqsort;
	sfqsort.Sort(xArray,nItems,&sortInfo, CompareData);//maga a rendezes

	int* tmpItemArray=new int[nItems];//tomb ami ideiglenesen tartalmazza a tablazat eredeti feltoltesi sorrendjet
	int* tmpRevItemArray=new int[nItems];//tomb ami ideiglenesen tartalmazza a tablazat visszakeresheto sorrendjet
	for (int i=0;i<nItems;i++)
	{
		if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
			tmpItemArray[i]=m_ItemOrder[i];// a tombot feltoltjuk az eredeti tarolt sorrenddel
		else
			tmpItemArray[i]=i;
	}

	int nSel=GetSelectedCount();// a kijelolt sorok szama
	int nCurItem;
	int* selItems=new int[nSel];

	if(GetStyle() & LVS_OWNERDATA)//ha virtualiskent hasznaljuk
	{
		for(int i=0;i<nSel;i++)
		{
			nCurItem=GetNextItem(-1,LVNI_SELECTED);//megkeressuk az elso kijeloltet
			SetItemState(nCurItem, ~LVIS_SELECTED, LVIS_SELECTED);//toroljuk a kijelolest
			selItems[i]=m_ItemOrder[nCurItem];//kikeressuk az eredeti helyet a kijelolt sornak
		}
	}

	for (int i=0;i<nItems;i++)
	{
		if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
			SetItemData(xArray[i],i);//beallitjuk az itemdata-t a sorrendnek megfeleloen
		m_ItemOrder[i]=tmpItemArray[xArray[i]];// meghatarozzuk a valtozast az eredeti sorrendet tartalmazo tombben
		tmpRevItemArray[xArray[i]]=i;//ebbol kesobb kiolvashatjuk, hogy pl. a 6. sorbol hanyadik lett a rendezes utan
	}

	if(GetStyle() & LVS_OWNERDATA)//ha virtualiskent hasznaljuk
		for(int i=0;i<nSel;i++)
			SetItemState(tmpRevItemArray[selItems[i]],LVIS_SELECTED,LVIS_SELECTED);//visszaallitjuk a kijelolest a rendezes utan

	while (!tmpDataset.empty())
	{
		delete[] tmpDataset.back().fText;
		tmpDataset.pop_back();
	}

	delete [] tmpItemArray;
	delete [] tmpRevItemArray;
	delete [] selItems;
	delete [] xSign;
	delete [] xType;
	delete [] xArray;

	if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
		SortItems(CompareFunct, 0);// elmozgatjuk a sorokat az itemdata alapjan a windowsos fuggvennyel.
	else
		Invalidate();
}

BOOL CListCtrlEx::OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult)// a szinezes itt tortenik meg
{
  *pResult = 0; 

  LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)pNMHDR;

  switch(lplvcd->nmcd.dwDrawStage)
  {
    case CDDS_PREPAINT :
    {
      *pResult = CDRF_NOTIFYITEMDRAW;//jelezzen az sor rajzolasakor
      return TRUE;
    }

    // Modify item text and or background
    case CDDS_ITEMPREPAINT:
    {
	  // lplvcd->clrTextBk = RGB(255,255,255);// a sor default szine
	  // lplvcd->clrText = RGB(0,0,0);// a sor hatterenek default szine

	  
      *pResult = CDRF_NOTIFYSUBITEMDRAW;// jelezzen a mezok rajzolasakor
      return TRUE;
    } 

    // Modify sub item text and/or background
//case CDDS_SUBITEM | CDDS_PREPAINT | CDDS_ITEM:// a mezok rajzolasakor valtoztatunk a szinen
	case CDDS_SUBITEM | CDDS_ITEMPREPAINT:// a mezok rajzolasakor valtoztatunk a szinen
    {
		CELLCOLOR cColor;
		cColor.bg=RGB(255,255,255);//default hatterszin
		cColor.tx=RGB(0,0,0);//default betuszin
		GetParent()->SendMessage(WM_SET_COLUMN_COLOR, (WPARAM)lplvcd->iSubItem, (LPARAM)&cColor);// oszlopszin beallitasa az uzenettel
		GetParent()->SendMessage(WM_SET_ROW_COLOR, (WPARAM)lplvcd->nmcd.dwItemSpec, (LPARAM)&cColor);// sorszin beallitasa az uzenettel
		lplvcd->clrTextBk = cColor.bg;
		lplvcd->clrText= cColor.tx;

	if ( ((GetStyle() & LVS_SHOWSELALWAYS)==LVS_SHOWSELALWAYS) & (GetItemState(lplvcd->nmcd.dwItemSpec, LVIS_SELECTED)==LVIS_SELECTED) & (GetFocus()!=this) )
	  {
		lplvcd->clrTextBk = RGB(180,180,180);
 		lplvcd->nmcd.uItemState &= ~CDIS_SELECTED;
	  }


		*pResult = CDRF_NEWFONT;
        return FALSE;
      }
  }
  return FALSE;
}

void CListCtrlEx::OnInsertitem(NMHDR* pNMHDR, LRESULT* pResult) //uj sor hozzaadasakor taroljuk az eredeti sorrendet
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_ItemOrder.insert(m_ItemOrder.begin()+pNMListView->iItem,pNMListView->iItem);
	SendMessage(WM_DATA_CHANGED,(WPARAM)TRUE,0);//maganak a controllnak jelezzuk, hogy megvaltozott-e a listctrl tartalma. Pl. hogy tudjuk, hogy ervenyes e meg a rendezettseg
	*pResult = 0;
}

void CListCtrlEx::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) // sor torlesekor toroljuk az eredeti sorrendet tartalmazo tombbol is
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_ItemOrder.erase(m_ItemOrder.begin()+pNMListView->iItem);
	*pResult = 0;
}

int CListCtrlEx::GetRecOrigPos(int nRec)// visszaadja a sor eredeti poziciojat
{
	return m_ItemOrder[nRec];
}

void CListCtrlEx::OnContextMenu(CWnd* pWnd, CPoint point) // jobb gombal kattintasra uzenetet kuld, hogy tudjunk pl. context menut csinalni
{
	GetParent()->SendMessage(WM_LISTCTRL_MENU, (WPARAM)0, (LPARAM)&point);
}

LRESULT CListCtrlEx::OnDataChanged(WPARAM wParam, LPARAM lParam)//wparam: valtozott vagy nem, lparam: 0
{
	if (wParam)
	{
		if(m_SortColumns.GetSize())
		{
			if(!m_bKeepSortOrder)
			{
				ResetSortOrder();//ha nem tartjuk meg a rendezesi beallitasokat, akkor toroljuk azokat valtozas eseten
			}
			else if (m_bAutoReSort)
			{
				ReSort();// ujrarendezzuk a sorokat amennyiben az automatikus ujrarendezes aktiv
			}
		}
	}

	if(m_bHeaderCheckBox && GetHeaderCtrl()->GetItemCount())
	{
		HDITEM hdi = { 0 };
		hdi.mask = HDI_FORMAT;
		VERIFY( GetHeaderCtrl()->GetItem(0, &hdi ));
		hdi.fmt |= HDF_CHECKBOX | HDF_FIXEDWIDTH;
		VERIFY( GetHeaderCtrl()->SetItem( 0, &hdi ) );
	}

	return TRUE;
}

void CListCtrlEx::SetSortOrder(int numArgs,...)
{
	if(numArgs<1)
		return;

	m_SortColumns.RemoveAll();//toroljuk a rendezesi oszlopok listajat

	va_list aList;
	va_start(aList,numArgs);
	for(int i=0;i<numArgs;i++)
	{
		int p=va_arg(aList,int);
		m_SortColumns.Add(p);
	}
	va_end(aList); // cleanup , set 'list' to NULL
	Sorting();
	SetSortArrow();
}

void CListCtrlEx::SetSortOrder(CIntArray* psortArray)
{
	if(psortArray->GetSize())
	{
		m_SortColumns.RemoveAll();//toroljuk a rendezesi oszlopok listajat
		for (int i=0;i<psortArray->GetSize();i++)
		{
			m_SortColumns.Add(psortArray->GetAt(i));
		}
	}
	Sorting();
	SetSortArrow();
}

bool CListCtrlEx::AttachDataset(std::vector<TCHAR*> *dataSet)
{
	if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
		return false;

	if(IsDatasetAttached())
		DetachDataset();

	vSDataSet=dataSet;
	SetItemCountEx(vSDataSet->size()/GetHeaderCtrl()->GetItemCount());
	for(int i=0;i<GetItemCount();i++)
		m_ItemOrder.push_back(i);
	return true;
}

bool CListCtrlEx::AttachDataset(std::vector<FIELD> *dataSet)
{
	if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
		return false;

	if(IsDatasetAttached())
		DetachDataset();

	vFDataSet=dataSet;
	SetItemCountEx(vFDataSet->size()/GetHeaderCtrl()->GetItemCount());
	for(int i=0;i<GetItemCount();i++)
		m_ItemOrder.push_back(i);
	return true;
}

bool CListCtrlEx::IsDatasetAttached()
{
	return (vSDataSet!=NULL || vFDataSet!=NULL);
}

void CListCtrlEx::DetachDataset()
{
	if((GetStyle() & LVS_OWNERDATA)==0)//ha nem virtualiskent hasznaljuk
		return;
	SetItemCountEx(0);
	m_ItemOrder.clear();
	vSDataSet=NULL;
	vFDataSet=NULL;
}

BOOL CListCtrlEx::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if(IsDatasetAttached())//ha csatoltunk adattombot
	{
		LV_ITEM* pItem= &(pDispInfo)->item;
//		recordSet->MoveTo( pItem->iItem );

		if (pItem->mask & LVIF_TEXT) //valid text buffer?
		{
			//lstrcpy(pItem->pszText, recordSet->GetFieldString( pItem->iSubItem ) );
			if(vSDataSet)//ha szoveg vectort csatoltunk
				pItem->pszText=vSDataSet->operator [](GetHeaderCtrl()->GetItemCount()*m_ItemOrder[pItem->iItem]+pItem->iSubItem);
			else if(vFDataSet)
				pItem->pszText=vFDataSet->operator [](GetHeaderCtrl()->GetItemCount()*m_ItemOrder[pItem->iItem]+pItem->iSubItem).fText;

		}
		return true;
	}

	return false;
}

CString CListCtrlEx::GetItemText(int nItem, int nSubItem)
{
	if(IsDatasetAttached())
	{
		CString retStr;
		if(vSDataSet)
			retStr=vSDataSet->operator [](GetHeaderCtrl()->GetItemCount()*m_ItemOrder[nItem]+nSubItem);
		else if(vFDataSet)
			retStr=vFDataSet->operator [](GetHeaderCtrl()->GetItemCount()*m_ItemOrder[nItem]+nSubItem).fText;
		return retStr;
	}
	else
		return CListCtrl::GetItemText(nItem,nSubItem);
}