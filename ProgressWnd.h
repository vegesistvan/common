// ProgressWnd.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Copyright 1998.
//
// CProgressWnd is a drop-in popup progress window for use in
// programs that a time consuming. Check out the accompanying HTML 
// doc file for details.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is not sold for
// profit without the authors written consent, and providing that this
// notice and the authors name is included. If the source code in 
// this file is used in any commercial application then an email to 
// me would be nice.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to your
// computer, causes your pet cat to fall ill, increases baldness or
// makes you car start emitting strange noises when you start it up.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 


#ifndef _INCLUDE_PROGRESSWND_H
#define _INCLUDE_PROGRESSWND_H

/////////////////////////////////////////////////////////////////////////////
// CProgressWnd window
//! Dialog window to display progress of an operation
class CProgressWnd : public CWnd
{
// Construction/Destruction
public:
	CString       m_strTitle;
    CProgressWnd();															//!< constructor
    CProgressWnd(CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE);	//!< constructor
    virtual ~CProgressWnd();												//!< constructor

    BOOL Create(CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth = FALSE);		//!< create modless dialog
    BOOL GoModal(LPCTSTR pszTitle =L"Progress", BOOL bSmooth = FALSE);	//!< display dialog window

protected:
    void CommonConstruct();

// Operations
public:
    void SetRange(int nLower, int nUpper, int nStep = 1);		//!< Set range and step size
    int OffsetPos(int nPos);									//!< Same as CProgressCtrl
    int StepIt();												//!< Same as CProgressCtrl
    int SetStep(int nStep);										//!< Same as CProgressCtrl
    int SetPos(int nPos);										//!< Same as CProgressCtrl
    void SetText(LPCTSTR fmt, ...);								//!< Set text in text area
    void Clear();												//!< Clear text, reset bar
    void Hide();												//!< Hide window
    void Show();												//!< Show window

    BOOL Cancelled() { return m_bCancelled; }					//!< Was "Cancel" hit?

	void SetWindowSize(int nNumTextLines,int nWindowWidth=390); //!< set window size

    void PeekAndPump(BOOL bCancelOnESCkey = TRUE);				//!< Message pumping for modal operation   
    
// Implementation
protected:
    void GetPreviousSettings();
    void SaveCurrentSettings();

protected:
    CWnd * m_wRenenableWnd;

    BOOL m_bCancelled;
    BOOL m_bModal;
    BOOL m_bPersistantPosition;
    int  m_nPrevPos, m_nPrevPercent;
    int  m_nStep;
    int  m_nMaxValue, m_nMinValue;
    int  m_nNumTextLines;

    CStatic       m_Text;
    CProgressCtrl m_wndProgress;
    CButton       m_CancelButton;
    CString       m_strCancelLabel;
    CFont         m_font;


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressWnd)
	public:
	virtual BOOL DestroyWindow();							//!< destroy window
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
    //{{AFX_MSG(CProgressWnd)
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);					//!< erase background
	//}}AFX_MSG
    afx_msg void OnCancel();								//!< Cancel process
    DECLARE_MESSAGE_MAP()
};


#endif
/////////////////////////////////////////////////////////////////////////////

