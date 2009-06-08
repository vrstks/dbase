/****************************************************************/
/*																*/
/*  MemoEditorDlg.cpp											*/
/*																*/
/*  Implementation of the CMMemoEditorDlg class.				*/
/*	This file is part of the DBF Explorer application.			*/
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
#include "dbfexplorer.h"
#include "MemoEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

CMemoEditorDlg::CMemoEditorDlg(CWnd* pParent) : CDialogResize(CMemoEditorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMemoEditorDlg)
	m_strText = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMemoEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMemoEditorDlg)
	DDX_Text(pDX, IDC_TEXT, m_strText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMemoEditorDlg, CDialogResize)
	//{{AFX_MSG_MAP(CMemoEditorDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DLGRESIZE_MAP(CMemoEditorDlg)
	DLGRESIZE_CONTROL(IDC_TEXT, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
END_DLGRESIZE_MAP()

BOOL CMemoEditorDlg::OnInitDialog() 
{
	CDialogResize::OnInitDialog();
	
	InitResizing();
	
	CenterWindow();

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	return TRUE;
}
