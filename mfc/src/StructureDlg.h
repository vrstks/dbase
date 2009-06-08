// StructureDlg.h : header file

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

#include "DialogResize.h"

class CStructureDlg : public CDialogResize
{
// Construction
public:
	CString m_strTitle;
	CStringArray m_strFieldArray;
	CStructureDlg(CWnd* parent);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CStructureDlg)
	enum { IDD = IDD_STRUCTURE };
	CButton	m_btnAdd;
	CListCtrl	m_FieldListCtrl;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStructureDlg)
	protected:
	virtual void DoDataExchange(CDataExchange*);
	//}}AFX_VIRTUAL

// Implementation
protected:
	bool m_bModified;
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CStructureDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDblclkFields(NMHDR*, LRESULT*);
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_DLGRESIZE_MAP;
};
