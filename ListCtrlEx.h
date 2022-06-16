#pragma once

#include "ListCtrlEx_messages.h"

#if !defined(AFX_LISTCTRLEX_H__BC648F5B_2CC3_454B_BE3E_DBDCB6445329__INCLUDED_)
#define AFX_LISTCTRLEX_H__BC648F5B_2CC3_454B_BE3E_DBDCB6445329__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlEx.h : header file
//
#include <vector>
#include "EditItem.h"
#include "ComboItem.h"
#include "sfqSort.h"

//#define WM_SET_COMBO	WM_USER + 0x1FF1//a combobox lista kitolteset kero fuggveny uzenete
//#define WM_SET_COLUMN_COLOR	WM_USER + 0x1FF2//a listcontrol oszlopainak szinet kero fuggveny uzenete
//#define WM_SET_ROW_COLOR	WM_USER + 0x1FF3//a listcontrol oszlopainak szinet kero fuggveny uzenete
//#define WM_ITEM_UPDATED	WM_USER + 0x1FF4//az editalast koveto valtozast jelento fv uzenete
//#define WM_LISTCTRL_MENU WM_USER + 0x1FF5//a jobb gombbal kattintast jelento uzenet
//#define WM_CLICKED_COLUMN WM_USER + 0x1FF6//rendezes utan a rendezett oszlop szamat adja vissza ez az uzenet (utoljara clickelt oszlop)
//#define WM_DATA_CHANGED WM_USER + 0x1FF7//a listctrl adatainak megvaltozasarol kuld uzenetet, de csak sajat maganak
//#define WM_ROWTODELETE WM_USER + 0x1FF8//a listctrl delete gombbal torteno esemenyerol kuld infot
////#define WM_FILL_TABLE WM_USER + 0x1FF9//a Table_firstname-nek küld üzenetet, hogy újra kell listázni az adatbázist
//#define WM_MAIN_TITLE WM_USER + 0x1FF9//a listctrl delete gombbal torteno esemenyerol kuld infot
//#define WM_SYNCRONIZE_PEOPLE WM_USER + 0x1FFA 

#define COL_EDIT   0x0001	//editalhato
#define COL_COMBO  0x0002   //combo list
#define COL_NUM    0x0004   //numeric data
#define COL_TEXT   0x0008   //szoveg data
#define COL_HIDDEN 0x0010   //rejtett oszlop


	
/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx window
#ifndef FIELD_TYPE
#define FIELD_TYPE

typedef struct FIELD
{
	FIELD (TCHAR* x_, double y_) :fText(x_), fValue(y_) {}
	TCHAR* fText;
	double fValue;
} FIELD;
#endif

typedef struct DOINF
{
	DOINF() : tmpCurLevel(0),pSArray(NULL),pFArray(NULL),pFSArray(NULL) {}//az elso a 0. rendezendo oszlop indexe
	std::vector<TCHAR*> *pSArray;//mutato a tablara, TCHAR* vector tipusu
	std::vector<FIELD> *pFArray;//mutato a tablara, FIELD vector tipusu
	std::vector<FIELD> *pFSArray;//mutato a tablara, FIELD vector tipusu. csak a rendezendo oszlopokat tartalmazza

	int *iArray;//index tomb, mely tartalmazza az adott rendezest
	int colNum;//tabla oszlopainak szama
	int colOrderNum;//a rendezni kivant oszlopok szama
	int tmpCurLevel;//a rendezni kivant oszlop pillanatnyi indexe
	int *typeArray;//a rendezni kivant oszlopok tipusai (0:szoveg, 1:szam)
	int *signArray;//a rendezni kivant oszlopok elojelei es sorszamai (pl: -1,5,3)
	bool sortCaseSensit;//a szoveg szerinti rendezesnel erdekes
} DOINF;

struct CELLCOLOR	// struktura az oszlop szineihez
{
	COLORREF bg;//hatterszin
	COLORREF tx;//szovegszin
};

typedef struct //struktura, mely a combobox sorszamat es adatait tartalmazza
{
	int nItem;
	CStringList m_strList;

} COMBODATA;

typedef CArray <int, int> CIntArray;  // int tipusu carray tombhoz

class CListCtrlEx : public CListCtrl
{
// Construction
public:
	CListCtrlEx();


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlEx)
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	
// Implementation
public:
	virtual ~CListCtrlEx();

public:
	bool AttachDataset(std::vector<TCHAR*> *dataSet);//adattomb csatolasa (TCHAR*) (vitualis listctrl eseten)
	bool AttachDataset(std::vector<FIELD> *dataSet);//adattomb csatolasa (FIELD) (vitualis listctrl eseten)
	void DetachDataset();//adattomb lecsatolasa
	bool IsDatasetAttached();//van csatolva adattomb?
	void KeepSortOrder(BOOL rStatus);//a rendezesi informacio megtartasa a grid torlese utan
	void AutoReSort(BOOL bASort);//automatikus ujrarendezes az adatok valtozasakor
	void ReSort();//ujrarendezese az adatoknak
	void EnableHeaderChkbox(bool state);//engedelyezzuk a headeren a checkboxot (csak ha nem ownerdata a control, es a LVS_EX_CHECKBOXES stilus aktiv
	void ResetSortOrder();//eredeti rendezes visszaallitasa
	void SetSortOrder(CIntArray* psortArray);//sorrend beallitasa elore megadott oszlopsorrenddel
	void SetSortOrder(int x,...);//sorrend beallitasa parameterekkel
	void HiddeColumn(int nCol);//oszlop elrejtese
	void ShowColumn(int nCol);//oszlop megjelenitese
	BOOL IsColumnVisible(int nCol);//lathato az oszlop?
	int GetRecOrigPos(int nRec);//az eredeti sor szama (rendezes eseten nemvaltozik)
	void SortByHeaderClick(BOOL bSort);//feljec kattintasos rendezes engedelyezese
	void DeleteByDeleteKey(BOOL bDelete);//sor torlese delete bill.-el
	virtual BOOL DeleteColumn(int nCol);//oszlop torlese. virtual listctrl eseten nem mukodik
	BOOL SetColumnType(int nCol, int nColType);//oszlop tipusanak megvaltoztatasa
	BOOL SetColumnVisibility(int nCol, bool bShow);
	int GetColumnType(int nCol);//oszlop tipusanak lekerdezese
	virtual int InsertColumn(int nCol, const LVCOLUMN* pColumn, int nColType=0);//oszlop beszurasa. virtual listctrl eseten nem mukodik
	virtual int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1,int nColType=0);//oszlop beszurasa. virtual listctrl eseten nem mukodik
	
	CString GetItemText(int nItem, int nSubItem);

	// Generated message map functions
protected:
	BOOL EditItem(int nItem, int nSubItem);
	BOOL ComboItem(int nItem, int nSubItem);
	//{{AFX_MSG(CListCtrlEx)
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnInsertitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnCustomDraw(NMHDR *pNMHDR, LRESULT *pResult);  //ez kell a szin valtashoz
	afx_msg LRESULT OnDataChanged(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemStateIconClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()

private:
	static int CompareData (const void *pInfo,const void * a, const void * b);
//	BOOL SetColumnVisibility(int nCol, bool bShow);
	void Sorting();
	void SetSortCaseSense(bool caseSense);
	static int CALLBACK CompareFunct(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	BOOL m_bHeaderSort;
	BOOL m_bKeepSortOrder;
	BOOL m_bAutoReSort;
	BOOL m_bEnableDeleteKey;
	bool m_bHeaderCheckBox;
	void SetSortArrow();
	BOOL GetSubItemRectEx(int nItem, int nSubItem, CRect& rect);
	BOOL DoControl(int nItem, int nSubItem, int nType);
	int m_PrevItem;
	int m_PrevSubItem;
	bool sortCaseSensitive;
	CUIntArray m_ColType;
	std::vector<int> m_ItemOrder;//indextomb. az sorok adott rendezes szerinti sorrendjet tartalmazza. Tehat ebbol kaphatjuk meg, az eredeti sorszamat az adott sornak

	CIntArray m_SortColumns;//ez tarolja az oszlopok rendezesi sorrendjet (pl:1,-2,4)
	
	void CreateSortIcons();
	CImageList m_imglstSortIcons;
	int m_iUpArrow;
	int m_iDownArrow;

	std::vector<TCHAR*> *vSDataSet;//csak TCHAR sztringeket tartalmazo dataset
	std::vector<FIELD> *vFDataSet;//FIELD strukturat tartalmazo dataset
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCTRLEX_H__BC648F5B_2CC3_454B_BE3E_DBDCB6445329__INCLUDED_)
