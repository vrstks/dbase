// DBFExplorerView.h : interface of the CDBFExplorerView class

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

class CDBFExplorerDoc;
class CDBFExplorerView : public CListView
{
	typedef struct tagITEMINFO 
	{
		bool bIsDeleted; 
		DWORD dwRecordIndex; 
	} ITEMINFO;
	
protected: // create from serialization only
	CDBFExplorerView();
	DECLARE_DYNCREATE(CDBFExplorerView)

// Attributes
public:
	CDBFExplorerDoc* GetDocument();
	void ShowRecords(bool bShowDeletedRecords = FALSE);
	
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFExplorerView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT&);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo*);
	virtual void OnPrint(CDC*, CPrintInfo*);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBFExplorerView();

protected:
	int CellRectFromPoint(CPoint &, RECT * cellrect, int * col) const;
	int HitTestEx(CPoint&, int *col) const;
	CEdit* EditSubItem(int nItem, int nSubItem);

	CFindReplaceDialog *m_pFindReplaceDialog;

// Generated message map functions
protected:
	int m_nRowsPerPage;
	CFont m_PrintFont;	// special font for printing
	enum {	TOP_MARGIN = 100, BOTTOM_MARGIN = 100, LEFT_MARGIN = 200 };

	void EditField(int nIndex, int nSubItem);
	void ReplaceCurrentText(CPoint&, LPCTSTR lpszText, LPCTSTR lpszReplaceText);
	CString m_strReplaceString;
	bool m_bSearchDown;
	bool m_bMatchWholeWord;
	bool m_bMatchCase;
	CPoint FindString(CString &str, bool bMatchCase = FALSE, bool bMatchWholeWord = FALSE, bool bSearchDown = TRUE, int nStart = -1);
	CString m_strFindString;
	bool m_bItemHighlighted;
	//{{AFX_MSG(CDBFExplorerView)
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnEndlabeledit(NMHDR*, LRESULT*);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar*);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*);
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditDelete();
	afx_msg void OnDeleteitem(NMHDR*, LRESULT*);
	afx_msg void OnEditUndelete();
	afx_msg void OnUpdateEditUndelete(CCmdUI*);
	afx_msg void OnUpdateEditDelete(CCmdUI*);
	afx_msg void OnContextMenu(CWnd*, CPoint);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint);
	afx_msg void OnEditAdd();
	afx_msg void OnEditRecord();
	afx_msg void OnUpdateNeedEditable(CCmdUI*);
	afx_msg void OnUpdateEditRecord(CCmdUI*);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindNext();
	afx_msg void OnEditReplace();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR*, LRESULT*);
	afx_msg LRESULT OnEditSubItem(WPARAM, LPARAM);
	afx_msg LRESULT OnFindDialogMessage(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DBFExplorerView.cpp
inline CDBFExplorerDoc* CDBFExplorerView::GetDocument()
   { return (CDBFExplorerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
