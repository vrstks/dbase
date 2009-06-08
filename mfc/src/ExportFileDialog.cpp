// ExportFileDialog.cpp : implementation file
//

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
#include "dbfexplorer.h"
#include "ExportFileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportFileDialog

IMPLEMENT_DYNAMIC(CExportFileDialog, CFileDialog)

CExportFileDialog::CExportFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
		DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd) :
		CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

BEGIN_MESSAGE_MAP(CExportFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CExportFileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExportFileDialog::OnTypeChange()
{
	HWND hComboExt = ::GetDlgItem(::GetParent(m_hWnd), 0x0470);
	if (hComboExt)
	{
		::SendMessage(hComboExt, CB_GETLBTEXT, (WPARAM)(m_ofn.nFilterIndex - 1), (LPARAM)(LPCTSTR)m_strType.GetBuffer(128));
		m_strType.ReleaseBuffer();
	}
}

BOOL CExportFileDialog::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
	
	OnTypeChange();
	return TRUE;
}
