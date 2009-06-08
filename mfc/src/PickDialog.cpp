// PickDialog.cpp : implementation file

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
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPickDialog dialog

class CPickDialog : public CDialog
{
// Construction
public:
	CPickDialog(CWnd* parent, const CString& caption, const CStringArray& as);

// Dialog Data
	//{{AFX_DATA(CPickDialog)
	enum { IDD = IDD_PICK };
	CString	m_selection;
	//}}AFX_DATA
   CListBox m_list;
   const CStringArray& m_as;
   const CString& m_caption;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPickDialog)
	protected:
	virtual void DoDataExchange(CDataExchange*);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPickDialog)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPickDialog dialog

CPickDialog::CPickDialog(CWnd* pParent, const CString& caption, const CStringArray& as) : 
      CDialog(CPickDialog::IDD, pParent), m_as(as), m_caption(caption)
{
	//{{AFX_DATA_INIT(CPickDialog)
	//}}AFX_DATA_INIT
   m_selection = as[0];
}

void CPickDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);

   if (pDX->m_bSaveAndValidate)
   {
   }
   else
   {
      DDX_Control(pDX, IDC_LIST, m_list);
      SetWindowText(m_caption);
      m_list.ResetContent();
      for (int i = 0; i < m_as.GetSize(); i++)
      {
         m_list.AddString(m_as[i]);
      }
      m_list.SetFocus();
   }

	//{{AFX_DATA_MAP(CPickDialog)
	DDX_LBString(pDX, IDC_LIST, m_selection);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPickDialog, CDialog)
	//{{AFX_MSG_MAP(CPickDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

bool DoModal_Pick(CWnd* parent, const CString& caption, const CStringArray& as, CString* sel)
{
   CPickDialog dlg(parent, caption, as);
   bool ok = (IDOK == dlg.DoModal());
   if (ok) sel->operator=(dlg.m_selection);
   return ok;
}
