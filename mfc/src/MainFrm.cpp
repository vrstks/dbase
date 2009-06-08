/****************************************************************/
/*																*/
/*  MainFrm.cpp													*/
/*																*/
/*  Implementation of the CMainFrame class.						*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*	http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 21 July 2002									*/
/*																*/
/****************************************************************/

/* 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"
#include "DBFExplorer.h"

#include "MainFrm.h"
#include "DBFExplorerDoc.h"
#include "DBFExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_DELETEDRECORDS, OnUpdateViewDeletedrecords)
	ON_COMMAND(ID_VIEW_DELETEDRECORDS, OnViewDeletedrecords)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static const UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_RECCOUNT,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/********************************************************************/
/*																	*/
/* Function name : GetActiveFrame									*/
/* Description   : Get pointer to CMainFrame						*/
/*																	*/
/********************************************************************/
CMainFrame *GetActiveFrame()
{
	CWnd* pMainWnd = AfxGetMainWnd();
	ASSERT_VALID(pMainWnd);

	if (!pMainWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
	{
		return NULL;
	}
	return (CMainFrame*)pMainWnd;//((CFrameWnd *)(pMainWnd))->GetActiveFrame();
}


CMainFrame::CMainFrame()
{
   m_bShowDeletedRecords = AfxGetApp()->GetProfileInt(_T("Settings"), _T("ShowDeletedRecords"), 0) ? true : false;
	
}

CMainFrame::~CMainFrame()
{
}


 BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
 {
//     cs.style &= ~FWS_ADDTOTITLE;    // no title
     return CFrameWnd::PreCreateWindow(cs);
 }

/********************************************************************/
/*																	*/
/* Function name : OnCreate											*/
/* Description   : Setup toolbar/statusbar							*/
/*																	*/
/********************************************************************/
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	// Set the text for each button
	//CToolBarCtrl& bar = m_wndToolBar.GetToolBarCtrl();

	int	nIndex = 0;
	TBBUTTON tb;

    for (nIndex = m_wndToolBar.GetToolBarCtrl().GetButtonCount() - 1; nIndex >= 0; nIndex--)
	{
		ZeroMemory(&tb, sizeof(TBBUTTON));
		m_wndToolBar.GetToolBarCtrl().GetButton(nIndex, &tb);

		// Do we have a separator?
		if ((tb.fsStyle & TBSTYLE_SEP) ==  TBSTYLE_SEP)
			continue;

		// Have we got a valid command id?
		if (tb.idCommand == 0)
			continue;

		// Get the resource string if there is one.
		CString strText;
		LPCTSTR lpszButtonText = NULL;
		CString	strButtonText(_T(""));
		_TCHAR	seps[] = _T("\n");

		strText.LoadString(tb.idCommand);

		if (!strText.IsEmpty())
		{
			lpszButtonText = _tcstok((LPTSTR)(LPCTSTR)strText, seps);

			while(lpszButtonText)
			{
				strButtonText = lpszButtonText;
				lpszButtonText = _tcstok(NULL, seps);
			}
		}

		if (!strButtonText.IsEmpty())
			m_wndToolBar.SetButtonText(nIndex, strButtonText);
	}

	// Resize the buttons so that the text will fit.
	CRect rc(0, 0, 0, 0);
	CSize sizeMax(0, 0);

	for (nIndex = m_wndToolBar.GetToolBarCtrl().GetButtonCount() - 1; nIndex >= 0; nIndex--)
	{
		m_wndToolBar.GetToolBarCtrl().GetItemRect(nIndex, rc);

		rc.NormalizeRect();
		sizeMax.cx = __max(rc.Size().cx, sizeMax.cx);
		sizeMax.cy = __max(rc.Size().cy, sizeMax.cy);
	}
	m_wndToolBar.SetSizes(sizeMax, CSize(16,16));

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  _countof(indicators)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

/********************************************************************/
/*																	*/
/* Function name : ShowProgressBar									*/
/* Description   : Show progress bar on statusbar					*/
/*																	*/
/********************************************************************/
bool CMainFrame::ShowProgressBar(CProgressCtrl* pProgressCtrl, LPCTSTR lpszText, UINT nIDControl)
{
	// Calculate destination rectangle for progress control
    CRect rc;
    m_wndStatusBar.GetItemRect(0, &rc);

	// Define progress bar horizontal offset
	if(lpszText)
	{
		// Compute message text extent
		CClientDC dc(&m_wndStatusBar);
		CFont* pFont = m_wndStatusBar.GetFont();
		CFont* pOldFont = dc.SelectObject(pFont);
		CSize sizeText = dc.GetTextExtent(lpszText);
		dc.SelectObject(pOldFont);
		rc.left += sizeText.cx;
	}

	// Display message text
	m_wndStatusBar.SetPaneText(0, lpszText);
	m_wndStatusBar.RedrawWindow();

	// Create progress control
   return pProgressCtrl->Create(WS_CHILD | WS_VISIBLE, rc, &m_wndStatusBar, nIDControl) ? true : false;
}

/********************************************************************/
/*																	*/
/* Function name : OnClose											*/
/* Description   : Save app settings								*/
/*																	*/
/********************************************************************/
void CMainFrame::OnClose() 
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("ShowDeletedRecords"), m_bShowDeletedRecords);	
	CMDIFrameWnd::OnClose();
}

/********************************************************************/
/*																	*/
/* Function name : OnUpdateViewDeletedrecords						*/
/* Description   : Check/uncheck 'Show Deleted Records'				*/
/*																	*/
/********************************************************************/
void CMainFrame::OnUpdateViewDeletedrecords(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bShowDeletedRecords);
}

/********************************************************************/
/*																	*/
/* Function name : OnViewDeletedrecords								*/
/* Description   : Show/Hide Deleted Records						*/
/*																	*/
/********************************************************************/
void CMainFrame::OnViewDeletedrecords() 
{
	m_bShowDeletedRecords = !m_bShowDeletedRecords;
	
	CWinApp* pApp = AfxGetApp();
	ASSERT_VALID(pApp);

	// Iterate through the application's document templates list
	POSITION posTemplate = pApp->GetFirstDocTemplatePosition();
	while(posTemplate)
	{
		// For each document template object...
		CDocTemplate* pTemplate = pApp->GetNextDocTemplate(posTemplate);
		ASSERT_VALID(pTemplate);
		ASSERT_KINDOF(CDocTemplate, pTemplate);

		// Iterate through the template's document list
		POSITION posDocument = pTemplate->GetFirstDocPosition();
		while(posDocument)
		{
			// For each document object...
			CDocument* pDoc = pTemplate->GetNextDoc(posDocument);
			ASSERT_VALID(pDoc);
			ASSERT_KINDOF(CDocument, pDoc);

			// reload records
			POSITION pos = pDoc->GetFirstViewPosition();
			while (pos)
				((CDBFExplorerView *)pDoc->GetNextView(pos))->ShowRecords(m_bShowDeletedRecords);
		}
	}
}

/********************************************************************/
/*																	*/
/* Function name : SetPaneWidth										*/
/* Description   : Set width of specified statusbar pane			*/
/*																	*/
/********************************************************************/
void CMainFrame::SetPaneWidth(int nIndex, LPCTSTR lpszText)
{
	// compute the width of the string
	CSize size;

	HGDIOBJ hOldFont = NULL;
	HFONT hFont = (HFONT)m_wndStatusBar.SendMessage(WM_GETFONT);
	CClientDC dc(NULL);
	if (hFont) 
		hOldFont = dc.SelectObject(hFont);
	size = dc.GetTextExtent(lpszText);
	if (hOldFont) 
		dc.SelectObject(hOldFont);

	// Update the pane to reflect the current date
	UINT nID, nStyle;
	int nWidth;
	m_wndStatusBar.GetPaneInfo(nIndex, nID, nStyle, nWidth);
	m_wndStatusBar.SetPaneInfo(nIndex, nID, nStyle, size.cx);
}
