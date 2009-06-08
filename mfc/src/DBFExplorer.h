// DBFExplorer.h : main header file for the DBFEXPLORER application

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

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerApp:
// See DBFExplorer.cpp for the implementation of this class
//

class CDBFExplorerApp : public CWinApp
{
protected:
	CMultiDocTemplate* m_doctemplate;
public:
	CDBFExplorerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFExplorerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

protected:
   void AddDocTemplates();

// Implementation
protected:
	//{{AFX_MSG(CDBFExplorerApp)
	afx_msg void OnAppAbout();
	afx_msg void OnHelpIndex();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

   friend class CAccessDocTemplate;
};

/////////////////////////////////////////////////////////////////////////////
