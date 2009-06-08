// RecordListBox.h : header file

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

/////////////////////////////////////////////////////////////////////////////
// CRecordListBox window

class CRecordListBox : public CListBox
{
	typedef struct tagITEMINFO
	{
		TCHAR szValue[255]; 
		char nType;
		int nLength;
	} ITEMINFO;

// Construction
public:
	CRecordListBox();

// Attributes
public:

// Operations
public:
	void HandleSelectionState(LPDRAWITEMSTRUCT);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRecordListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT);
	virtual void DeleteItem(LPDELETEITEMSTRUCT);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	int AddString(LPCTSTR lpszText, LPCTSTR lpszData, char nType, int nLength);
	virtual ~CRecordListBox();

	// Generated message map functions
protected:
	HFONT m_hBoldFont;
	CEdit m_WndEdit;
	int m_nDateField;
	void HandleSelectionState();
	//{{AFX_MSG(CRecordListBox)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
