// MainFrm.h : interface of the CMainFrame class

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

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	CStatusBar  m_wndStatusBar;
// Operations
public:
	bool ShowProgressBar(CProgressCtrl* pProgressCtrl, LPCTSTR lpszText = NULL, UINT nIDControl = 1);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetPaneWidth(int nIndex, LPCTSTR lpszText);
	bool m_bShowDeletedRecords;
	virtual ~CMainFrame();

   virtual BOOL PreCreateWindow(CREATESTRUCT&);

protected:  // control bar embedded members
	CToolBar    m_wndToolBar;
	void SetToolBarButtonText();

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnClose();
	afx_msg void OnUpdateViewDeletedrecords(CCmdUI*);
	afx_msg void OnViewDeletedrecords();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CMainFrame *GetActiveFrame();

/////////////////////////////////////////////////////////////////////////////
