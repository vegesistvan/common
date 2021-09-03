#pragma once

/* 
| #include <afxtempl.h>			// MFC support for CArray
| Ezt a sort az strafx.h ba kell betenni.
| A dialog headerben deklaralni kell a kovetkezot
| DECLARE_EASYSIZE
| A programban barhol hozzaadhatjuk a kontrollokat
| EASYSIZE_ADD(control,left,top,right,bottom,options);
| es kozvetlenul utanna kell egy
| EASYSIZE_INIT()
|
| atmeretezes meghivasa:
| EASYSIZE_RESIZE()
| 
|	Optional: Insert 'EASYSIZE_MINSIZE(mx,my);' in your OnSizing handler
|    if you want to specify a minimum size for your dialog
|
| a control kivetele az atmeretezes alol:
| EASYSIZE_REMOVE(control)
| 
|
|  Set the style of dialog box to WS_CLIPCHILDREN for the flickerfree drawing
|  EASYSIZE_ADD(control,left,top,right,bottom,options);
|  left,top,right,bottom = id of any control vagy ES_BORDER,ES_KEEPSIZE,ES_KEEPRATIO
|  options = ES_CENTER,ES_HCENTER,ES_VCENTER,0,
|
|	Ha akarunk kis jelet a jobb also sarokban, ami jelzi, hogy meretezheto az ablak, akkor
|   az init elott kell egy EASYSIZE_SETGRIPPER()
*/

/*============================================================================*\
|                                                                              |
|  EASY-SIZE Macros                                                            |
|                                                                              |
|  Copyright (c) 2001 - Marc Richarme                                          |
|      devix@devix.cjb.clt_srv                                                     |
|      http://devix.cjb.clt_srv                                                    |
|                                                                              |
|  License:                                                                    |
|                                                                              |
|  You may use this code in any commersial or non-commersial application,      |
|  and you may redistribute  this file (and even modify it if you wish) as     |
|  long as you keep this notice untouched in any version you redistribute.     |
|                                                                              |
|  Usage:                                                                      |
|                                                                              |
|  - Insert 'DECLARE_EASYSIZE' somewhere in your class declaration             |
|  - Insert an easysize map in the beginning of your class implementation      |
|    (see documentation) and outside of any function.                          |
|  - Insert 'INIT_EASYSIZE;' in your OnInitDialog handler.                     |
|  - Insert 'UPDATE_EASYSIZE' in your OnSize handler                           |
|  - Optional: Insert 'EASYSIZE_MINSIZE(mx,my);' in your OnSizing handler      |
|    if you want to specify a minimum size for your dialog                     |
|                                                                              |
|   Check http://devix.cjb.clt_srv for the docs and new versions                   |
|                                                                              |
|  History:                                                                    |
|                                                                              |
|  21 Aug 2006 by Hans Dietrich                                                |
|  - Added automatic support for DeferWindowPos() -  this required changing    |
|    END_EASYSIZE_MAP macro to END_EASYSIZE_MAP(theClass)                      |
|  - Merged gripper code by gniemcew                                           |
|  - Added macro EASYSIZE_MINSIZE2 for OnGetMinMaxInfo() handler               |
|  - Merged modifications by Jubjub to add support for ES_KEEPRATIO.  See      |
|    this message for details:                                                 |
|       http://www.codeproject.com/dialog/easysize.asp?msg=871006#xx871006xx   |
|  - Initialized some local variables and removed others to eliminate          |
|    compiler warnings at level 4                                              |
|                                                                              |
|  14/03/2003 by Ali (ayachali@yahoo.fr) - boulwiss                            |
|    The control options are stored in an array instead of duplicating the     |
|    body of the RepositionControls function, that reduce too much the code    |
|    inserted by the macros. the size of the RepositionControls function is    |
|    always the same !                                                         |
\*============================================================================*/


#ifndef EASYSIZE_H
#define EASYSIZE_H

#define ES_BORDER		0x00000001
#define ES_KEEPSIZE		0x00000002
#define ES_KEEPRATIO	0x00000004
#define ES_HCENTER		0x00000010
#define ES_VCENTER		0x00000020

#ifdef _DEBUG
#define ES_OUTPUTDEBUGSTRING OutputDebugString
#else
#define ES_OUTPUTDEBUGSTRING __noop
#endif

#define EASYSIZE_SETGRIPPER() __ES__SetGrip();

#define DECLARE_EASYSIZE \
public:\
	CScrollBar m_Grip;\
	typedef struct __ctrlmap{UINT id, l, t, r, b, o, es_l, es_t, es_r, es_b, hRatio, vRatio;} _ctrlmap;\
	CArray<_ctrlmap,_ctrlmap> ctrlmap;\
void __ES__CalcBottomRight(CWnd *pThis, BOOL bBottom, int &bottomright, int &topleft, UINT id, UINT br, int es_br, CRect& rect, int clientbottomright)\
{\
	if (br == ES_BORDER)\
		bottomright = clientbottomright-es_br;\
	else\
	{\
		if (br == ES_KEEPSIZE)\
			bottomright = topleft+es_br;\
		else if (br == ES_KEEPRATIO)\
			bottomright = topleft+es_br;\
		else\
		{\
			CRect rect2;\
			pThis->GetDlgItem(br)->GetWindowRect(rect2);\
			pThis->ScreenToClient(rect2);\
			bottomright = (bBottom?rect2.top:rect2.left) - es_br;\
		}\
	}\
} \
void __ES__RepositionControls(BOOL bInit)\
{\
	TCHAR s[100];\
	HDWP hdwp = 0;\
	CRect rect, rect2, client;\
	GetClientRect(client);\
	int i = 0;\
	CWnd* pWnd = NULL;\
	while (ctrlmap[i].id)\
	{\
		pWnd = GetDlgItem(ctrlmap[i].id);\
		if (pWnd)\
		{\
			if (bInit)\
			{\
				pWnd->GetWindowRect(rect);\
				ScreenToClient(rect);\
				if (ctrlmap[i].o & ES_HCENTER)\
					ctrlmap[i].es_l = rect.Width()/2;\
				else\
				{\
					if (ctrlmap[i].l == ES_BORDER)\
						ctrlmap[i].es_l = rect.left;\
					else if (ctrlmap[i].l == ES_KEEPSIZE)\
						ctrlmap[i].es_l = rect.Width();\
					else if (ctrlmap[i].l == ES_KEEPRATIO)\
						ctrlmap[i].es_l = rect.left;\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_l = rect.left-rect2.right;\
					}\
				}\
				if (ctrlmap[i].o & ES_VCENTER)\
					ctrlmap[i].es_t = rect.Height()/2;\
				else\
				{\
					if (ctrlmap[i].t == ES_BORDER)\
						ctrlmap[i].es_t = rect.top;\
					else if (ctrlmap[i].t == ES_KEEPSIZE)\
						ctrlmap[i].es_t = rect.Height();\
					else if (ctrlmap[i].t == ES_KEEPRATIO)\
						ctrlmap[i].es_t = rect.top;\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_t = rect.top-rect2.bottom;\
					}\
				}\
				if (ctrlmap[i].o & ES_HCENTER)\
					ctrlmap[i].es_r = rect.Width();\
				else\
				{\
					if (ctrlmap[i].r == ES_BORDER)\
						ctrlmap[i].es_r = client.right-rect.right;\
					else if (ctrlmap[i].r == ES_KEEPSIZE)\
						ctrlmap[i].es_r = rect.Width();\
					else if (ctrlmap[i].r == ES_KEEPRATIO)\
						ctrlmap[i].es_r = client.right-rect.right;\
					else\
					{\
						GetDlgItem(ctrlmap[i].r)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_r = rect2.left-rect.right;\
					}\
				}\
				if (ctrlmap[i].o & ES_VCENTER)\
					ctrlmap[i].es_b = rect.Height();\
				else\
				{\
					if (ctrlmap[i].b == ES_BORDER)\
						ctrlmap[i].es_b = client.bottom-rect.bottom;\
					else if (ctrlmap[i].b == ES_KEEPSIZE)\
						ctrlmap[i].es_b = rect.Height();\
					else if (ctrlmap[i].b == ES_KEEPRATIO)\
						ctrlmap[i].es_b = client.bottom-rect.bottom;\
					else\
					{\
						GetDlgItem(ctrlmap[i].b)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						ctrlmap[i].es_b = rect2.top-rect.bottom;\
					}\
				}\
			}\
			else\
			{\
				int _nControls = ctrlmap.GetSize();\
				if (_nControls && (hdwp == 0))\
				{\
					hdwp = BeginDeferWindowPos(_nControls);\
					_stprintf_s(s, _T("BeginDeferWindowPos called for %d windows\r\n"), _nControls);\
					ES_OUTPUTDEBUGSTRING(s);\
				}\
				int left = 0, top = 0, right = 0, bottom = 0;\
				int iPossibleNewWidth = (int)((double)client.Width() * ctrlmap[i].hRatio);\
				int iPossibleNewHeight = (int)((double)client.Height() * ctrlmap[i].vRatio);\
				if (ctrlmap[i].o & ES_HCENTER)\
				{\
					int _a = 0, _b = 0;\
					if (ctrlmap[i].l == ES_BORDER)\
						_a = client.left;\
					else if (ctrlmap[i].l == ES_KEEPRATIO)\
					{\
						_a = (client.Width() - iPossibleNewWidth)/2;\
						_b = _a + iPossibleNewWidth;\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_a = rect2.right;\
					}\
					if (ctrlmap[i].l != ES_KEEPRATIO)\
					{\
						if (ctrlmap[i].r == ES_BORDER)\
							_b = client.right;\
						else if (ctrlmap[i].r == ES_KEEPRATIO)\
						{\
							_a = (client.Width() - iPossibleNewWidth)/2;\
							_b = _a + iPossibleNewWidth;\
						}\
						else\
						{\
							GetDlgItem(ctrlmap[i].r)->GetWindowRect(rect2);\
							ScreenToClient(rect2);\
							_b = rect2.left;\
						}\
					}\
					left = _a+((_b-_a)/2-ctrlmap[i].es_l);\
					right = left + ctrlmap[i].es_r;\
				}\
				else\
				{\
					if (ctrlmap[i].l == ES_BORDER)\
					{\
						left = ctrlmap[i].es_l;\
						if (ctrlmap[i].r == ES_KEEPRATIO)\
							right = left + iPossibleNewWidth;\
					}\
					else if (ctrlmap[i].l == ES_KEEPSIZE)\
					{\
						if (ctrlmap[i].r != ES_KEEPRATIO)\
						{\
							__ES__CalcBottomRight(this,FALSE,right,left,ctrlmap[i].id,ctrlmap[i].r,ctrlmap[i].es_r,rect,client.right);\
							left = right-ctrlmap[i].es_l;\
						}\
						else\
						{\
							/* ignore the keepsize on the left because right is keepratio (takes priority) */ \
							/* assume l == ES_BORDER*/ \
							left = ctrlmap[i].es_l;\
						}\
					}\
					else if (ctrlmap[i].l == ES_KEEPRATIO)\
					{\
						if (ctrlmap[i].r == ES_BORDER || ctrlmap[i].r == ES_KEEPSIZE)\
						{\
							right = client.right - ctrlmap[i].es_r;\
							left = right-iPossibleNewWidth;\
						}\
						else\
						{\
							GetDlgItem(ctrlmap[i].r)->GetWindowRect(rect2);\
							ScreenToClient(rect2);\
							right = rect2.left - ctrlmap[i].es_r;\
							left = right-iPossibleNewWidth;\
						}\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].l)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						left = rect2.right + ctrlmap[i].es_l;\
						if (ctrlmap[i].r == ES_KEEPRATIO)\
							right = left + iPossibleNewWidth;\
					}\
					if (ctrlmap[i].l != ES_KEEPSIZE && ctrlmap[i].l != ES_KEEPRATIO && ctrlmap[i].r != ES_KEEPRATIO)\
						__ES__CalcBottomRight(this,FALSE,right,left,ctrlmap[i].id,ctrlmap[i].r,ctrlmap[i].es_r,rect,client.right);\
				}\
				if (ctrlmap[i].o & ES_VCENTER)\
				{\
					int _a = 0, _b = 0;\
					if (ctrlmap[i].t == ES_BORDER)\
						_a = client.top;\
					else if (ctrlmap[i].t == ES_KEEPRATIO)\
					{\
						_a = (client.Height() - iPossibleNewHeight)/2;\
						_b = _a + iPossibleNewHeight;\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						_a = rect2.bottom;\
					}\
					if (ctrlmap[i].t != ES_KEEPRATIO)\
					{\
						if (ctrlmap[i].b == ES_BORDER)\
							_b = client.bottom;\
						else if (ctrlmap[i].b == ES_KEEPRATIO)\
						{\
							_a = (client.Height() - iPossibleNewHeight)/2;\
							_b = _a + iPossibleNewHeight;\
						}\
						else\
						{\
							GetDlgItem(ctrlmap[i].b)->GetWindowRect(rect2);\
							ScreenToClient(rect2);\
							_b = rect2.top;\
						}\
					}\
					if (ctrlmap[i].t != ES_KEEPRATIO && ctrlmap[i].b != ES_KEEPRATIO)\
					{\
						top = _a+((_b-_a)/2-ctrlmap[i].es_t);\
						bottom = top + ctrlmap[i].es_b;\
					}\
					else\
					{\
						top = _a;\
						bottom = _b;\
					}\
				}\
				else\
				{\
					if (ctrlmap[i].t == ES_BORDER)\
					{\
						top = ctrlmap[i].es_t;\
						if (ctrlmap[i].b == ES_KEEPRATIO)\
							bottom = top + iPossibleNewHeight;\
					}\
					else if (ctrlmap[i].t == ES_KEEPSIZE)\
					{ \
						if (ctrlmap[i].b != ES_KEEPRATIO)\
						{\
							__ES__CalcBottomRight(this,TRUE,bottom,top,ctrlmap[i].id,ctrlmap[i].b,ctrlmap[i].es_b,rect,client.bottom);\
							top = bottom-ctrlmap[i].es_t;\
						}\
						else\
						{\
							/*ignore the keepsize on the top because left is keepratio (takes priority)*/\
							/*assume t == ES_BORDER*/\
							top = ctrlmap[i].es_t;\
						}\
					}\
					else if (ctrlmap[i].t == ES_KEEPRATIO)\
					{\
						if (ctrlmap[i].b == ES_BORDER || ctrlmap[i].b == ES_KEEPSIZE)\
						{\
							bottom = ctrlmap[i].es_b;\
							top = bottom-iPossibleNewHeight;\
						}\
						else\
						{\
							GetDlgItem(ctrlmap[i].b)->GetWindowRect(rect2);\
							ScreenToClient(rect2);\
							bottom = rect2.top - ctrlmap[i].es_b;\
							top = bottom-iPossibleNewHeight;\
						}\
					}\
					else\
					{\
						GetDlgItem(ctrlmap[i].t)->GetWindowRect(rect2);\
						ScreenToClient(rect2);\
						top = rect2.bottom + ctrlmap[i].es_t;\
						if (ctrlmap[i].b == ES_KEEPRATIO)\
							bottom = top + iPossibleNewHeight;\
					}\
					if (ctrlmap[i].t != ES_KEEPSIZE && ctrlmap[i].t != ES_KEEPRATIO && ctrlmap[i].b != ES_KEEPRATIO)\
						__ES__CalcBottomRight(this,TRUE,bottom,top,ctrlmap[i].id,ctrlmap[i].b,ctrlmap[i].es_b,rect,client.bottom);\
				}\
				_stprintf_s(s, _T("hdwp=0x%X\r\n"), hdwp);\
				ES_OUTPUTDEBUGSTRING(s);\
				if ((right != left) && (bottom != top))\
				{\
					if (hdwp)\
						DeferWindowPos(hdwp,pWnd->m_hWnd,0,left,top,right-left,bottom-top,SWP_NOZORDER);\
					else\
						pWnd->MoveWindow(left,top,right-left,bottom-top,FALSE);\
				}\
			}\
			i++;\
		} \
	} /* while */ \
	if (hdwp)\
	{\
		EndDeferWindowPos(hdwp); hdwp = 0; \
		_stprintf_s(s, _T("EndDeferWindowPos called\r\n"));\
		ES_OUTPUTDEBUGSTRING(s);\
	}\
	Invalidate();\
	UpdateWindow();\
} \
void __ES__SetGrip()\
{\
	if(!m_Grip.GetSafeHwnd())\
	{\
		CRect rect(5,5,5,5);\
		CRect rect2;\
		GetClientRect(rect2);\
		rect2.left = rect2.right - rect.Width();\
		rect2.top = rect2.bottom - rect.Height();\
		m_Grip.Create(WS_CHILD | SBS_SIZEBOX | SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEGRIP | WS_VISIBLE, rect2, this, AFX_IDW_SIZE_BOX);\
		/* Add the gripper to the list */\
		__ES__AddControl(AFX_IDW_SIZE_BOX, ES_KEEPSIZE, ES_KEEPSIZE, ES_BORDER, ES_BORDER, 0);\
	}\
}\
void __ES__Init()\
{\
	__ES__RepositionControls(TRUE);\
	__ES__RepositionControls(FALSE);\
}\
void __ES__Resize()\
{\
	if (GetWindow(GW_CHILD) != NULL)\
		__ES__RepositionControls(FALSE);\
}\
void __ES__AddControl(UINT id,UINT l,UINT t,UINT r,UINT b,UINT o)\
{\
	int nCount = ctrlmap.GetSize();\
	_ctrlmap map = {id,l,t,r,b,o,0,0,0,0,0,0};\
	if(nCount == 0) \
	{ \
		ctrlmap.Add(map); \
		map.id = map.l = map.t = map.b = map.r = 0;\
		ctrlmap.Add(map); \
	} \
	else \
		ctrlmap.InsertAt(nCount-1, map); \
}\
BOOL __ES__UpdateControl(UINT id,UINT l,UINT t,UINT r,UINT b,UINT o)\
{\
	if(id == 0) return FALSE; \
	int nCount = ctrlmap.GetSize();\
	for(int i = 0; i < nCount; i++) \
	{ \
		if(ctrlmap[i].id == id) \
		{\
			ctrlmap[i].l = l;\
			ctrlmap[i].t = t;\
			ctrlmap[i].b = b;\
			ctrlmap[i].r = r;\
			ctrlmap[i].o = o;\
			return TRUE; \
		} \
	} \
	return FALSE; \
}\
BOOL __ES__RemoveControl(UINT id)\
{\
	if(id == 0) return FALSE; \
	int nCount = ctrlmap.GetSize();\
	for(int i = 0; i < nCount; i++) \
	{ \
		if(ctrlmap[i].id == id) \
		{\
			ctrlmap.RemoveAt(i);\
			return TRUE; \
		} \
	} \
	return FALSE; \
}
// #define DECLARE_EASYSIZE

#define EASYSIZE_ADD(id,l,t,r,b,o) __ES__AddControl(id,l,t,r,b,o)
#define EASYSIZE_UPDATE(id,l,t,r,b,o) __ES__UpdateControl(id,l,t,r,b,o)
#define EASYSIZE_REMOVE(id) __ES__RemoveControl(id)
#define EASYSIZE_INIT() __ES__Init();
#define EASYSIZE_RESIZE() __ES__Resize();
#define EASYSIZE_MINSIZE(mx,my,s,r)\
{\
	if (r->right-r->left < mx)\
	{\
		if ((s == WMSZ_BOTTOMLEFT) || (s == WMSZ_LEFT) || (s == WMSZ_TOPLEFT))\
			r->left = r->right-mx;\
		else\
			r->right = r->left+mx;\
	}\
	if (r->bottom-r->top < my)\
	{\
		if ((s == WMSZ_TOP) || (s == WMSZ_TOPLEFT) || (s == WMSZ_TOPRIGHT))\
			r->top = r->bottom-my;\
		else\
			r->bottom = r->top+my;\
	}\
} // #define EASYSIZE_MINSIZE(mx,my,s,r)

// for OnGetMinMaxInfo() handler
#define EASYSIZE_MINSIZE2(mx,my,lpMMI)\
{\
	int cx = lpMMI->ptMinTrackSize.x;\
	int cy = lpMMI->ptMinTrackSize.y;\
	lpMMI->ptMinTrackSize.x = (cx < mx) ? mx : cx;\
	lpMMI->ptMinTrackSize.y = (cy < my) ? my : cy;\
} // #define EASYSIZE_MINSIZE(mx,my,s,r)

// compatibilities
#define EASYSIZE EASYSIZE_ADD;
#define INIT_EASYSIZE EASYSIZE_INIT;
#define UPDATE_EASYSIZE EASYSIZE_RESIZE

#endif //EASYSIZE_H
