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

class CFieldEdit : public CEdit
{
// Construction
public:
	CFieldEdit(int iItem, int iSubItem, LPCTSTR lpszText);

// Attributes
public:
	CFont m_Font;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG*);
	//}}AFX_VIRTUAL

// Implementation
public:
	enum dbf_data_type m_type;
	virtual ~CFieldEdit();

	// Generated message map functions
protected:
	int m_iNextSubItem;
	int m_iItem;
	int m_iSubItem;
	CString m_strText;
	bool m_bEscape;

	//{{AFX_MSG(CFieldEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcDestroy();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int OnCreate(LPCREATESTRUCT);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
