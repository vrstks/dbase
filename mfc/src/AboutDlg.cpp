/****************************************************************/
/*																*/
/*  AboutDlg.cpp												*/
/*																*/
/*  Implementation of the CAboutDlg class.						*/
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
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/) : CDialog(CAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_MAIL, m_MailLink);
	DDX_Control(pDX, IDC_STATIC_LINK, m_HomePageLink);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************************/
/*																	*/
/* Function name : OnInitDialog										*/
/* Description   : Initialize dialog.								*/
/*																	*/
/********************************************************************/
BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_MailLink.SetURL(_T("mailto:info@pablovandermeer.nl"));
	m_HomePageLink.SetURL(_T("http://www.pablovandermeer.nl"));

    CString strExeName = AfxGetApp()->m_pszExeName;
    strExeName += _T(".exe");

	UpdateVersionInformation(strExeName);
	return TRUE; 
}

/********************************************************************/
/*																	*/
/* Function name : UpdateVersionInformation							*/
/* Description   : Show version information from resource.			*/
/*																	*/
/********************************************************************/
void CAboutDlg::UpdateVersionInformation(LPCTSTR lpszModuleName)
{
	BYTE		*pDataBlock = NULL;
	DWORD FAR	*translation;
	DWORD FAR	*buffer;     
	DWORD		dwHandle;
	UINT		nBytes;
	
	// WinAPI wants non-const arguments
	LPTSTR lpszExeName = const_cast<LPTSTR>(lpszModuleName); 

	TCHAR szName[512];       // StringFileInfo data block.

	// Get the actual size of the information block.
	nBytes = (UINT)GetFileVersionInfoSize(lpszExeName, &dwHandle);

	if (nBytes)
	{
		pDataBlock = new BYTE[nBytes];

		// Get the actual block for the version information
		if (GetFileVersionInfo(lpszExeName, dwHandle, nBytes, pDataBlock))
		{
			if (VerQueryValue(pDataBlock, _T("\\VarFileInfo\\Translation"), (VOID FAR * FAR *)&translation, (UINT FAR *)&nBytes)) 
			{
				// The File Version for this application
				wsprintf(szName, _T("\\StringFileInfo\\%04x%04x\\PrivateBuild"), LOWORD(*translation), HIWORD(*translation));

				if (VerQueryValue(pDataBlock, szName, (VOID FAR * FAR *)&buffer, (UINT FAR *)&nBytes)) 
				{
					CString szData = (TCHAR*)buffer;
					//GetDlgItem(IDC_FILE_VERSION)->SetWindowText(szData);
				}
			}
			else 
			{
				// sorry ...
			}
		}
		if (pDataBlock)
			delete[] pDataBlock;
	}
}
