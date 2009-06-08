/****************************************************************/
/*																*/
/*  StructureDlg.cpp											*/
/*																*/
/*  Implementation of the CStructureDlg class.					*/
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
#include "DBFExplorer.h"
#include "StructureDlg.h"
#include "AddFieldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

CStructureDlg::CStructureDlg(CWnd* pParent) : CDialogResize(CStructureDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStructureDlg)
	//}}AFX_DATA_INIT
	m_bModified = FALSE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStructureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogResize::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStructureDlg)
	DDX_Control(pDX, IDC_ADD, m_btnAdd);
	DDX_Control(pDX, IDC_FIELDS, m_FieldListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStructureDlg, CDialogResize)
	//{{AFX_MSG_MAP(CStructureDlg)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_NOTIFY(NM_DBLCLK, IDC_FIELDS, OnDblclkFields)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DLGRESIZE_MAP(CStructureDlg)
	DLGRESIZE_CONTROL(IDC_FIELDS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
	DLGRESIZE_CONTROL(IDC_ADD, DLSZ_MOVE_X)
	DLGRESIZE_CONTROL(IDC_EDIT, DLSZ_MOVE_X)
	DLGRESIZE_CONTROL(IDC_DELETE, DLSZ_MOVE_X)
	DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
	DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_X | DLSZ_MOVE_Y)
END_DLGRESIZE_MAP()

/********************************************************************/
/*																	*/
/* Function name : OnInitDialog										*/		
/* Description   : Initialize dialog								*/
/*																	*/
/********************************************************************/
BOOL CStructureDlg::OnInitDialog() 
{
	CDialogResize::OnInitDialog();
	
	DWORD dwStyle = m_FieldListCtrl.GetExtendedStyle();
	dwStyle |= (LVS_EX_FULLROWSELECT);
    m_FieldListCtrl.SetExtendedStyle(dwStyle);
	
	m_FieldListCtrl.InsertColumn(0, _T("Field Name"));
	m_FieldListCtrl.InsertColumn(1, _T("Type"));
	m_FieldListCtrl.InsertColumn(2, _T("Length"));
	m_FieldListCtrl.InsertColumn(3, _T("Decimals"));
	AutoSizeColumns(&m_FieldListCtrl);

	InitResizing();

	// iterate through all arguments and create a list of all the fields
	for (int i=0; i < m_strFieldArray.GetSize(); i++)
	{
		CString strBuff;
	
		m_FieldListCtrl.InsertItem(i, _T(""));

		for (int j=0; j < 4; j++)
		{
			AfxExtractSubString(strBuff, m_strFieldArray[i], j, ',');
			strBuff.TrimLeft();
			strBuff.TrimRight();
			m_FieldListCtrl.SetItemText(i, j, strBuff);
		}
	}
	
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	if (!m_strTitle.IsEmpty())
		SetWindowText(m_strTitle);

	CenterWindow();

	return TRUE;
}

/********************************************************************/
/*																	*/
/* Function name : OnOK												*/		
/* Description   : Validate and close dialog						*/
/*																	*/
/********************************************************************/
void CStructureDlg::OnOK() 
{
	if (!m_FieldListCtrl.GetItemCount())
	{
		MessageBox(_T("There must at least be one field defined !"), _T("Edit Structure"), MB_OK | MB_ICONEXCLAMATION);
        return;
    }

	// any modification ?
	if(m_bModified)
	{
		m_strFieldArray.RemoveAll();

		// add field data to array
		for (int i=0; i< m_FieldListCtrl.GetItemCount(); i++)
		{
			CString strFieldData;
			strFieldData = m_FieldListCtrl.GetItemText(i, 0);
			strFieldData += ",";
			strFieldData += m_FieldListCtrl.GetItemText(i, 1);
			strFieldData += ",";
			strFieldData += m_FieldListCtrl.GetItemText(i, 2);
			strFieldData += ",";
			strFieldData += m_FieldListCtrl.GetItemText(i, 3);
			m_strFieldArray.Add(strFieldData);
		}
		CDialogResize::OnOK();
	}
	else
	{
		CDialogResize::OnCancel();
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnAdd											*/		
/* Description   : Add field to structure							*/
/*																	*/
/********************************************************************/
void CStructureDlg::OnAdd() 
{
	CAddFieldDlg dlg(this);

	if (dlg.DoModal() == IDOK)
	{
		// insert new field data
		int nItem = m_FieldListCtrl.GetItemCount();
		nItem = m_FieldListCtrl.InsertItem(nItem, dlg.m_strName);

		CString strText;
		const TCHAR * const szTypes[] = {_T("C"), _T("D"), _T("N"), _T("L"), _T("F"), _T("M")};
		m_FieldListCtrl.SetItemText(nItem, 1, szTypes[dlg.m_nType]);

		strText.Format(_T("%d"), dlg.m_nLength);
		m_FieldListCtrl.SetItemText(nItem, 2, strText);

		strText.Format(_T("%d"), dlg.m_nDecimals);
		m_FieldListCtrl.SetItemText(nItem, 3, strText);
		
		m_bModified = TRUE;
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnEdit											*/		
/* Description   : Edit field data									*/
/*																	*/
/********************************************************************/
void CStructureDlg::OnEdit() 
{
	// get selected item
	int nIndex = m_FieldListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if (nIndex == -1)
		return;

	CAddFieldDlg dlg(this);

	dlg.m_strTitle = _T("Edit Field");

	dlg.m_strName = m_FieldListCtrl.GetItemText(nIndex, 0);

	CString strText = _T("CDNLFM");
	dlg.m_nType = strText.Find(m_FieldListCtrl.GetItemText(nIndex, 1));
	dlg.m_nLength = _ttoi(m_FieldListCtrl.GetItemText(nIndex, 2));
	dlg.m_nDecimals = _ttoi(m_FieldListCtrl.GetItemText(nIndex, 3));

	if (dlg.DoModal() == IDOK)
	{
		// update field data
		m_FieldListCtrl.SetItemText(nIndex, 0, dlg.m_strName);

		const TCHAR * const szTypes[] = {_T("C"), _T("D"), _T("N"), _T("L"), _T("F"), _T("M")};
		m_FieldListCtrl.SetItemText(nIndex, 1, szTypes[dlg.m_nType]);

		strText.Format(_T("%d"), dlg.m_nLength);
		m_FieldListCtrl.SetItemText(nIndex, 2, strText);

		strText.Format(_T("%d"), dlg.m_nDecimals);
		m_FieldListCtrl.SetItemText(nIndex, 3, strText);
		
		m_bModified = TRUE;
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnDblclkFields									*/		
/* Description   : Double clicked on item							*/
/*																	*/
/********************************************************************/
void CStructureDlg::OnDblclkFields(NMHDR*, LRESULT* pResult) 
{
	OnEdit();
	*pResult = 0;
}

/********************************************************************/
/*																	*/
/* Function name : OnDelete											*/		
/* Description   : Delete field data								*/
/*																	*/
/********************************************************************/
void CStructureDlg::OnDelete() 
{
	// get selected item
	int nIndex = m_FieldListCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
	if (nIndex == -1)
		return;

	m_FieldListCtrl.DeleteItem(nIndex);
	
	m_bModified = TRUE;
}
