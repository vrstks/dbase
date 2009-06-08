// AddFieldDlg.h : header file

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
// CAddFieldDlg dialog

class CAddFieldDlg : public CDialog
{
// Construction
public:
	CString m_strTitle;
	CAddFieldDlg(CWnd* parent);   // standard constructor
	int	m_nType;

// Dialog Data
	//{{AFX_DATA(CAddFieldDlg)
	enum { IDD = IDD_ADDFIELD };
	CComboBox	m_cmbType;
	int		m_nDecimals;
	int		m_nLength;
	CString	m_strName;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddFieldDlg)
	protected:
	virtual void DoDataExchange(CDataExchange*);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAddFieldDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeName();
	afx_msg void OnSelchangeType();
	//}}AFX_MSG
	void OnUpdateLength(CCmdUI*);
	void OnUpdateDecimals(CCmdUI*);
	void OnUpdateOK(CCmdUI*);
	DECLARE_MESSAGE_MAP()
};
