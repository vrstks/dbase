/****************************************************************/
/*																*/
/*  DBFExplorerView.cpp											*/
/*																*/
/*  Implementation of the CDBFExplorerView class.				*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*	http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 08 August 2002								*/
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

#include "MainFrm.h"
#include "DBFExplorerView.h"
#include "DBFExplorerDoc.h"
#include "FieldEdit.h"
#include "MemoEditorDlg.h"
#include "..\..\dbf_mfc.h"

#include <afxpriv.h>	// for WM_SETMESSAGESTRING message

#define WM_EDIT_SUBITEM	WM_USER + 40
#define IDC_SUBEDIT 1444

static UINT g_FindDialogMessage = ::RegisterWindowMessage(FINDMSGSTRING);

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerView

IMPLEMENT_DYNCREATE(CDBFExplorerView, CListView)

BEGIN_MESSAGE_MAP(CDBFExplorerView, CListView)
	//{{AFX_MSG_MAP(CDBFExplorerView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, OnDeleteitem)
	ON_COMMAND(ID_EDIT_UNDELETE, OnEditUndelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDELETE, OnUpdateEditUndelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_EDIT_ADD, OnEditAdd)
	ON_COMMAND(ID_EDIT_RECORD, OnEditRecord)
	ON_UPDATE_COMMAND_UI(ID_EDIT_ADD, OnUpdateNeedEditable)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RECORD, OnUpdateEditRecord)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_COMMAND(ID_EDIT_FIND_NEXT, OnEditFindNext)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
	ON_MESSAGE(WM_EDIT_SUBITEM, OnEditSubItem)
	ON_REGISTERED_MESSAGE(g_FindDialogMessage, OnFindDialogMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerView construction/destruction

CDBFExplorerView::CDBFExplorerView()
{
	m_bItemHighlighted = FALSE;
	m_pFindReplaceDialog = NULL;
	m_strFindString = "";
	m_strReplaceString = "";
	m_bMatchCase = FALSE;
	m_bMatchWholeWord = FALSE;
	m_bSearchDown = TRUE;
	m_nRowsPerPage = 0;
}

CDBFExplorerView::~CDBFExplorerView()
{
}

BOOL CDBFExplorerView::PreCreateWindow(CREATESTRUCT& cs)
{
   cs.style|=LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS|LVS_AUTOARRANGE|LVS_OWNERDATA;
	return CListView::PreCreateWindow(cs);
}

/********************************************************************/
/*																	*/
/* Function name : OnInitialUpdate									*/
/* Description   : Called by the framework after the view is first	*/
/*				   attached to the document.						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	DWORD dwStyle = GetListCtrl().GetExtendedStyle();
	dwStyle |= (LVS_EX_FULLROWSELECT);
    GetListCtrl().SetExtendedStyle(dwStyle);

	//CDBFExplorerDoc* pDoc = GetDocument();

	ShowRecords(GetActiveFrame()->m_bShowDeletedRecords);
}

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerView diagnostics

#ifdef _DEBUG
CDBFExplorerDoc* CDBFExplorerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDBFExplorerDoc)));
	return (CDBFExplorerDoc*)m_pDocument;
}
#endif //_DEBUG


/********************************************************************/
/*																	*/
/* Function name : OnCreate											*/
/* Description   : Window is created								*/
/*																	*/
/********************************************************************/
int CDBFExplorerView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS);		
	return 0;
}

/********************************************************************/
/*																	*/
/* Function name : ShowRecords										*/
/* Description   : Show all the records in the database.			*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::ShowRecords(bool /*bShowDeletedRecords*/)
{
   USES_CONVERSION;
	CDBFExplorerDoc* pDoc = GetDocument();
	size_t nColumnCount = (size_t)GetListCtrl().GetHeaderCtrl()->GetItemCount();

	// Delete all of the columns.
	size_t i;
   for (i=0;i < nColumnCount;i++)
	{
		GetListCtrl().DeleteColumn(0);
	}

	// insert fieldnames
	for (i = 0; i < pDoc->m_dBaseFile->GetFieldCount(); i++)
	{
      DBF_FIELD_INFO info;
      if (pDoc->m_dBaseFile->GetFieldInfo(i, &info))
		{
			int nWidth = 100;//max(GetListCtrl().GetStringWidth(_T("X")) * pField->m_Length, GetListCtrl().GetStringWidth(A2CT(pField->m_Name)));
			int nFormat = LVCFMT_LEFT;
//			if (pField->m_Type == 'N' || pField->m_Type == 'F' || pField->m_Type == 'L')
//				nFormat = LVCFMT_RIGHT;

			GetListCtrl().InsertColumn(i, A2CT(info.name), nFormat, 3*nWidth/2);
		}
	}
	int nCount = pDoc->m_dBaseFile->GetRecordCount();
   GetListCtrl().SetItemCount(nCount);

   // Ensures that idle message is displayed again
	GetActiveFrame()->PostMessage(WM_SETMESSAGESTRING, (WPARAM) AFX_IDS_IDLEMESSAGE, 0L);
}

/********************************************************************/
/*																	*/
/* Function name : OnLButtonDblClk									*/
/* Description   : Start edit of a sub item label on double click	*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListView::OnLButtonDblClk(nFlags, point);

	int nIndex;
	int nColumn;
	if((nIndex = HitTestEx(point, &nColumn)) != -1)
	{
		UINT nFlag = LVIS_FOCUSED;
		if((GetListCtrl().GetItemState(nIndex, nFlag) & nFlag) == nFlag && nColumn >= 0)
		{
			EditField(nIndex, nColumn);
		}
		else
		{
			// select the item
			GetListCtrl().SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 
		}
	}		
}

/********************************************************************/
/*																	*/
/* Function name : HitTestEx										*/
/* Description   : Determine the row index/column index for point	*/
/*																	*/
/********************************************************************/
int CDBFExplorerView::HitTestEx(CPoint &point, int *col) const
{
	int colnum = 0;
	int row = GetListCtrl().HitTest(point, NULL);
	
	if (col) 
		*col = 0;

	// Get the top and bottom row visible
	row = GetListCtrl().GetTopIndex();
	int bottom = row + GetListCtrl().GetCountPerPage();
	if(bottom > GetListCtrl().GetItemCount())
		bottom = GetListCtrl().GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetListCtrl().GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for(;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetListCtrl().GetItemRect(row, &rect, LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			for(colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = GetListCtrl().GetColumnWidth(colnum);
				if(point.x >= rect.left 
					&& point.x <= (rect.left + colwidth))
				{
					if(col) 
						*col = colnum;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

/********************************************************************/
/*																	*/
/* Function name : CellRectFromPoint								*/
/* Description   : Determine rect of cell							*/
/*																	*/
/********************************************************************/
int CDBFExplorerView::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
{
	int colnum;

	// Get the top and bottom row visible
	int row = GetListCtrl().GetTopIndex();
	int bottom = row + GetListCtrl().GetCountPerPage();
	if(bottom > GetListCtrl().GetItemCount())
		bottom = GetListCtrl().GetItemCount();
	
	// Get the number of columns
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetListCtrl().GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();

	// Loop through the visible rows
	for(;row <=bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetListCtrl().GetItemRect(row, &rect, LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			for(colnum = 0; colnum < nColumnCount; colnum++)
			{
				int colwidth = GetListCtrl().GetColumnWidth(colnum);
				if(point.x >= rect.left 
					&& point.x <= (rect.left + colwidth))
				{
					RECT rectClient;
					GetListCtrl().GetClientRect(&rectClient);
					if(col) *col = colnum;
					rect.right = rect.left + colwidth;

					// Make sure that the right extent does not exceed
					// the client area
					if(rect.right > rectClient.right) 
						rect.right = rectClient.right;
					*cellrect = rect;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

/********************************************************************/
/*																	*/
/* Function name : EditSubItem										*/
/* Description   : Start edit of a sub item label					*/
/*																	*/
/********************************************************************/
CEdit* CDBFExplorerView::EditSubItem(int nItem, int nSubItem)
{
	// Make sure that the item is visible
	if(!GetListCtrl().EnsureVisible(nItem, TRUE)) 
		return NULL;

	CDBFExplorerDoc* pDoc = GetDocument();

	if (!pDoc->m_dBaseFile->IsEditable()) return NULL;

	// Make sure that nColumn is valid
	CHeaderCtrl* pHeader = (CHeaderCtrl*)GetListCtrl().GetDlgItem(0);
	int nColumnCount = pHeader->GetItemCount();
	if(nSubItem >= nColumnCount || GetListCtrl().GetColumnWidth(nSubItem) < 5)
		return NULL;

	// Get the column offset
	int offset = 0;
	for(int i = 0; i < nSubItem; i++)
		offset += GetListCtrl().GetColumnWidth(i);

	CRect rect;
	GetListCtrl().GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetListCtrl().GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset + rect.left;
		size.cy = 0;
		GetListCtrl().Scroll(size);
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvColumn;
	lvColumn.mask = LVCF_FMT;
	GetListCtrl().GetColumn(nSubItem, &lvColumn);
	DWORD dwStyle = NULL;
	if((lvColumn.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else 
	if((lvColumn.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else 
		dwStyle = ES_CENTER;

	rect.left += offset;
	rect.right = rect.left + GetListCtrl().GetColumnWidth(nSubItem);
	if(rect.right > rcClient.right) 
		rect.right = rcClient.right;
	
	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;

	CEdit *pEdit = new CFieldEdit(nItem, nSubItem, GetListCtrl().GetItemText(nItem, nSubItem));
	pEdit->Create(dwStyle, rect, &GetListCtrl(), IDC_SUBEDIT);
	return pEdit;
}

/********************************************************************/
/*																	*/
/* Function name : OnEndlabeledit									*/
/* Description   : Done with editing								*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
   USES_CONVERSION;
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM	*plvItem = &pDispInfo->item;
	if (plvItem->pszText)
	{
		GetListCtrl().SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);

		ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(plvItem->iItem);

		// update database
		CDBFExplorerDoc* pDoc = GetDocument();

		if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
		{
			if (pDoc->m_dBaseFile->Write(plvItem->iSubItem, plvItem->pszText))
         {
			   pDoc->m_dBaseFile->PutRecord(pItem->dwRecordIndex);
         }
         else
         {
		      //GetListCtrl().SetItemText(plvItem->iItem, plvItem->iSubItem, _T(""));
         }
		}

		if ((int)plvItem->lParam != plvItem->iSubItem)
		{
			PostMessage(WM_EDIT_SUBITEM, (WPARAM)plvItem->iItem, plvItem->lParam);
		}
	}
	*pResult = FALSE;	
}

/********************************************************************/
/*																	*/
/* Function name : OnHScroll										*/
/* Description   : Leave edit mode while scrolling					*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this) 
		SetFocus();	
	CListView::OnHScroll(nSBCode, nPos, pScrollBar);
}

/********************************************************************/
/*																	*/
/* Function name : OnVScroll										*/
/* Description   : Leave edit mode while scrolling					*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if(GetFocus() != this) 
		SetFocus();
	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
}

/********************************************************************/
/*																	*/
/* Function name : OnEditSelectAll									*/
/* Description   : Select all items in the list						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditSelectAll() 
{
	for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
	{
		GetListCtrl().SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED , LVIS_SELECTED | LVIS_FOCUSED);
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnEditDelete										*/
/* Description   : Select all items in the list						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditDelete() 
{
   USES_CONVERSION;
	int nIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nIndex == -1)
		return;

	bool bShowDeletedRecords = GetActiveFrame()->m_bShowDeletedRecords;

	// delete items
	while (nIndex != -1)
	{
		ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(nIndex);
		
		// update database
		CDBFExplorerDoc* pDoc = GetDocument();

		if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
		{
			if (pDoc->m_dBaseFile->DeleteRecord() == DBASE_SUCCESS)
			{
				if (!bShowDeletedRecords)
					GetListCtrl().DeleteItem(nIndex);
				else
					pItem->bIsDeleted = TRUE;
			}
			else
			{
				break;
			}
		}
		else
		{
			// failed to delete record
			MessageBox(A2CT(pDoc->m_dBaseFile->GetLastError()), _T("Delete Record Error"), MB_ICONSTOP);
			break;
		}
		nIndex = GetListCtrl().GetNextItem(bShowDeletedRecords ? nIndex : -1, LVNI_ALL | LVNI_SELECTED);
	}
	Invalidate();
	UpdateWindow();
}

/********************************************************************/
/*																	*/
/* Function name : OnUpdateEditDelete								*/
/* Description   : Enable/disable delete							*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_dBaseFile->IsEditable() && GetListCtrl().GetSelectedCount());	
}

/********************************************************************/
/*																	*/
/* Function name : OnEditUndelete									*/
/* Description   : Select all items in the list						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditUndelete() 
{
   USES_CONVERSION;
	int nIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nIndex == -1)
		return;

	// undelete items
	while (nIndex != -1)
	{
		ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(nIndex);
		
		CDBFExplorerDoc* pDoc = GetDocument();

		if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
		{
			if (pDoc->m_dBaseFile->DeleteRecord(FALSE) == DBASE_SUCCESS)
			{
				pItem->bIsDeleted = FALSE;
			}
			else
			{
				break;
			}
		}
		else
		{
			// failed to undelete record
			MessageBox(A2CT(pDoc->m_dBaseFile->GetLastError()), _T("Undelete Record Error"), MB_ICONSTOP);
			break;
		}
		nIndex = GetListCtrl().GetNextItem(nIndex, LVNI_ALL | LVNI_SELECTED);
	}
	Invalidate();
	UpdateWindow();
}

/********************************************************************/
/*																	*/
/* Function name : OnUpdateEditUndelete								*/
/* Description   : Enable/disable undelete							*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnUpdateEditUndelete(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_dBaseFile->IsEditable() && GetListCtrl().GetSelectedCount() && GetActiveFrame()->m_bShowDeletedRecords);
}

/********************************************************************/
/*																	*/
/* Function name : OnDeleteitem										*/
/* Description   : Delete allocated item data						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	delete(ITEMINFO*)GetListCtrl().GetItemData(pNMListView->iItem);
	
	*pResult = 0;
}

/********************************************************************/
/*																	*/
/* Function name : OnCustomDraw										*/
/* Description   : Use custom colors								*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnCustomDraw(NMHDR *pHdr, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *lvcd = (NMLVCUSTOMDRAW *)pHdr;
	NMCUSTOMDRAW   &nmcd = lvcd->nmcd;

	switch (nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;

		case CDDS_ITEMPREPAINT:
		{	
			ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(nmcd.dwItemSpec);

			if (pItem && pItem->bIsDeleted)
			{
				m_bItemHighlighted = (GetListCtrl().GetItemState(nmcd.dwItemSpec, LVIS_SELECTED) != 0);
				// is item highlighted ?
				if (m_bItemHighlighted)
				{
					// temperarely turn off selected state
					GetListCtrl().SetItemState(nmcd.dwItemSpec, 0, LVIS_SELECTED);
					lvcd->clrText = RGB(192,192,192);
					lvcd->clrTextBk = RGB(128,128,128);
					
					// enable item post-paint notification to handle highlighting.
				      *pResult = CDRF_NOTIFYPOSTPAINT;
				}
				else
				{
					lvcd->clrText = GetSysColor(COLOR_GRAYTEXT);
					*pResult = CDRF_NEWFONT;
				}
			}
			else
			{
				*pResult = CDRF_DODEFAULT;
			}
		}
		break;

		case CDDS_ITEMPOSTPAINT:
		{
			if (m_bItemHighlighted)
			{
				// turn highlighting back on
				 GetListCtrl().SetItemState(nmcd.dwItemSpec, 0xff, LVIS_SELECTED);
			}
			*pResult = CDRF_DODEFAULT;
		}

		default:
			*pResult = CDRF_DODEFAULT;
			break;
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnContextMenu									*/
/* Description   : Show context menu								*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
{
	CMenu menu;
	if (menu.LoadMenu(IDR_CONTEXTMENU))
	{
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}	
}

/********************************************************************/
/*																	*/
/* Function name : OnEditAdd										*/
/* Description   : Add new record									*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditAdd() 
{
	CDBFExplorerDoc* pDoc = GetDocument();
	DWORD dwCount = pDoc->m_dBaseFile->GetRecordCount()+1;
	// add new record
	pDoc->m_dBaseFile->AddRecord();
	pDoc->m_dBaseFile->GetRecord(dwCount);
	pDoc->m_dBaseFile->PutRecord(dwCount);

	int nItem = GetListCtrl().GetItemCount();
	GetListCtrl().InsertItem(nItem, _T(""));
	
	// set record index
	ITEMINFO* pItem = new ITEMINFO;
	pItem->dwRecordIndex = dwCount;
	pItem->bIsDeleted = FALSE;
	GetListCtrl().SetItemData(nItem, (DWORD)pItem);
	
	GetListCtrl().EnsureVisible(nItem, FALSE);

	// deselect all
	for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
	{
		if (GetListCtrl().GetItemState(i, LVNI_SELECTED))
			GetListCtrl().SetItemState(i, (UINT)(~LVIS_SELECTED), LVIS_SELECTED);
	}
	// select the item
	GetListCtrl().SetItemState(nItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 

	// edit first item
	EditField(nItem, 0);
}

/********************************************************************/
/*																	*/
/* Function name : OnEditSubItem									*/
/* Description   : Edit sub item									*/
/*																	*/
/********************************************************************/
LRESULT CDBFExplorerView::OnEditSubItem(WPARAM wParam, LPARAM lParam)
{
	// edit sub item
	EditField((int)wParam, (int)lParam);
	return 0L;
}

/********************************************************************/
/*																	*/
/* Function name : OnEditRecord										*/
/* Description   : Start editing selected record					*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditRecord() 
{
	int nIndex = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (nIndex == -1)
		return;

	// edit first item
	EditField(nIndex, 0);
}

/********************************************************************/
/*																	*/
/* Function name : OnUpdateEditRecord								*/
/* Description   : Enable/disable 'Edit Record'						*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnUpdateEditRecord(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_dBaseFile->IsEditable() && GetListCtrl().GetSelectedCount());
}

void CDBFExplorerView::OnUpdateNeedEditable(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetDocument()->m_dBaseFile->IsEditable());
}

/********************************************************************/
/*																	*/
/* Function name : OnEditFind										*/
/* Description   : Finds the specified text							*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditFind() 
{
	if(m_pFindReplaceDialog == NULL)
	{
		// Must be created on the heap
		m_pFindReplaceDialog = new CFindReplaceDialog();  

		if (m_pFindReplaceDialog->Create(TRUE, m_strFindString, _T(""), FR_DOWN, this)) 
			m_pFindReplaceDialog->CenterWindow();
   }
}

/********************************************************************/
/*																	*/
/* Function name : OnFindDialogMessage								*/
/* Description   : Noification from CFindReplaceDialog				*/
/*																	*/
/********************************************************************/
LRESULT CDBFExplorerView::OnFindDialogMessage(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    ASSERT(m_pFindReplaceDialog);

	// closing find dialog?
    if (m_pFindReplaceDialog->IsTerminating())
    {
        m_pFindReplaceDialog = NULL;
        return 0;
    }

    if ((m_pFindReplaceDialog->FindNext()) || (m_pFindReplaceDialog->ReplaceCurrent()))
    {
		int nSelected = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
        
		// read data from dialog
      m_strFindString = m_pFindReplaceDialog->GetFindString();
      m_bMatchCase = m_pFindReplaceDialog->MatchCase() ? true : false;
		m_bMatchWholeWord = m_pFindReplaceDialog->MatchWholeWord() ? true : false;
		m_bSearchDown = m_pFindReplaceDialog->SearchDown() ? true : false;

		CPoint pt = FindString(m_strFindString, 
								m_bMatchCase,
								m_bMatchWholeWord,
								m_bSearchDown,
								nSelected);

		// select the found item
		if (pt.x != -1)
		{
			// deselect all
			for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
			{
				if (GetListCtrl().GetItemState(i, LVNI_SELECTED))
					GetListCtrl().SetItemState(i, (UINT)(~LVIS_SELECTED), LVIS_SELECTED);
			}
			GetListCtrl().SetItemState(pt.x, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
			GetListCtrl().EnsureVisible(pt.x, false);
			
			// replace text ?
			if (m_pFindReplaceDialog->ReplaceCurrent())
			{
				m_strReplaceString = m_pFindReplaceDialog->GetReplaceString();
				ReplaceCurrentText(pt, m_strFindString, m_strReplaceString);
			}
		}
		else
		{
			AfxMessageBox(_T("DBF Explorer has finished searching the database."), MB_ICONINFORMATION);
		}
	}
  
    return 0;
}

/********************************************************************/
/*																	*/
/* Function name : OnEditFindNext									*/
/* Description   : Finds specified text								*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditFindNext() 
{
	if (m_strFindString.IsEmpty())
	{
		OnEditFind();
	}
	else
	{
		int nSelected = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		CPoint pt = FindString(m_strFindString, m_bMatchCase, m_bMatchWholeWord, m_bSearchDown, nSelected);
		// select the found item
		if (pt.x != -1)
		{
			// deselect all
			for (int i = 0; i < GetListCtrl().GetItemCount(); i++)
			{
				if (GetListCtrl().GetItemState(i, LVNI_SELECTED))
					GetListCtrl().SetItemState(i, (UINT)(~LVIS_SELECTED), LVIS_SELECTED);
			}
			GetListCtrl().SetItemState(pt.x, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED); 
		}
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnEditFindNext									*/
/* Description   : Finds specified text								*/
/*																	*/
/********************************************************************/
CPoint CDBFExplorerView::FindString(CString &strSearchText, bool bMatchCase, bool bMatchWholeWord, bool bSearchDown, int nStart)
{
   CWaitCursor wc;
   int nLength = strSearchText.GetLength();

	if(nLength == 0) 
		return CPoint(-1, -1);
	
	if(!bMatchCase)
		strSearchText.MakeLower();

	if (nStart == -1)
		nStart = bSearchDown ? -1 : GetListCtrl().GetItemCount();
	
	int i = nStart;
	for (;;)
	{
		if (bSearchDown)
		{
			// get next item
			i++;
			if (i > GetListCtrl().GetItemCount())
				break;
		}
		else
		{
			// get previous item
			i--;
			if (i < 0)
				break;
		}

		// check all columns
		for (int j=0; j < GetListCtrl().GetHeaderCtrl()->GetItemCount(); j++)
		{
			CString strItemText = GetListCtrl().GetItemText(i, j);
				
			if(!bMatchCase)
				strItemText.MakeLower();
			
			int nPos;
			while((nPos = strItemText.Find(strSearchText)) != -1)
			{
				// Search string found
				if(bMatchWholeWord)
				{
					// Check preceding character
					if(nPos != 0)
					{
						// Not a whole word
						strItemText = strItemText.Right(strItemText.GetLength() - nPos - nLength);
						continue;
					}

					// Check succeeding character
					if(strItemText.GetLength() > nPos + nLength)
					{
						// Not a whole word
						strItemText = strItemText.Right(strItemText.GetLength() - nPos - strSearchText.GetLength());
						continue;
					}
				}
				return CPoint(i, j);
			}
		}
	}
	return CPoint(-1, -1);
}

/********************************************************************/
/*																	*/
/* Function name : OnEditReplace									*/
/* Description   : Replace specific text with different text		*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnEditReplace() 
{
	if(m_pFindReplaceDialog == NULL)
	{
		// Must be created on the heap
		m_pFindReplaceDialog = new CFindReplaceDialog();  

		if (m_pFindReplaceDialog->Create(FALSE, m_strFindString, m_strReplaceString, FR_DOWN, this)) 
			m_pFindReplaceDialog->CenterWindow();
   }	
}

/********************************************************************/
/*																	*/
/* Function name : ReplaceCurrentText								*/
/* Description   : Replace item text								*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::ReplaceCurrentText(CPoint &pt, LPCTSTR lpszText, LPCTSTR lpszReplaceText)
{
   USES_CONVERSION;
	CString strItemText = GetListCtrl().GetItemText(pt.x, pt.y);
	CString strText = lpszText;

	if (!m_bMatchCase)
	{
		strText.MakeLower();
		strItemText.MakeLower();
	}

	if (strItemText.Replace(strText, lpszReplaceText))
	{
		ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(pt.x);

		// update database
		CDBFExplorerDoc* pDoc = GetDocument();

		if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
		{
			LPTSTR lpszText = strItemText.GetBuffer(strItemText.GetLength());
			pDoc->m_dBaseFile->Write(pt.y, lpszText);
			pDoc->m_dBaseFile->PutRecord(pItem->dwRecordIndex);
			strItemText.ReleaseBuffer();
			
			GetListCtrl().SetItemText(pt.x, pt.y, strItemText);
		}

	}
}

/********************************************************************/
/*																	*/
/* Function name : EditField										*/
/* Description   : Start editing field, if it's a memo field ->		*/
/*				   bring up the memo field editor.					*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::EditField(int nItem, int nSubItem)
{
	CDBFExplorerDoc* pDoc = GetDocument();

   DBF_FIELD_INFO info;
   if (!(pDoc->m_dBaseFile->IsEditable() && pDoc->m_dBaseFile->GetFieldInfo(nSubItem, &info)))
	{
      return;
   }

	if (info.type == DBF_DATA_TYPE_MEMO)
	{
		ITEMINFO *pItem = (ITEMINFO *)GetListCtrl().GetItemData(nItem);
		CMemoEditorDlg dlg(GetParent());

		if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
		{
			char*buff;

			int nLength = pDoc->m_dBaseFile->GetMemoFieldLength(nSubItem+1);
			if (nLength)
			{
				try
				{
					buff = new char[nLength+1];
					pDoc->m_dBaseFile->GetMemoField(nSubItem+1, buff, nLength);
					dlg.m_strText = buff;
					delete buff;
				}
				catch(...)
				{
				}
			}
		}
		if (dlg.DoModal() == IDOK)
		{
			if (pDoc->m_dBaseFile->GetRecord(pItem->dwRecordIndex) == DBASE_SUCCESS)
			{
				// Set new value
				pDoc->m_dBaseFile->PutMemoField(nSubItem+1, (char *)(LPCTSTR)dlg.m_strText, dlg.m_strText.GetLength());

				pDoc->m_dBaseFile->PutRecord(pItem->dwRecordIndex);

				GetListCtrl().SetItemText(nItem, nSubItem, _T("MEMO"));
			}
		}
	}
	else
	{
		CFieldEdit *pEdit = (CFieldEdit *)EditSubItem(nItem, nSubItem);
		pEdit->m_type = info.type;
		pEdit->SetLimitText(info.length);
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnPreparePrinting								*/
/* Description   : Called before document is printed or previewed.	*/
/*																	*/
/********************************************************************/
BOOL CDBFExplorerView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	CPrintDialog dlg(FALSE);
	if (!dlg.GetDefaults()) 
		return FALSE;
	
	CDC dc;
	dc.Attach(dlg.GetPrinterDC());
	
	// get text dimensions
	TEXTMETRIC tm;  
	dc.GetTextMetrics(&tm);
	int cyChar = tm.tmHeight;
	int nLineOffset = cyChar + cyChar / 4;
	
	// calculate number of rows per page
	m_nRowsPerPage = dc.GetDeviceCaps(VERTRES)/nLineOffset;

	int nCount = GetListCtrl().GetItemCount(); 
	if(nCount < 1) 
		return FALSE;
	
	// calculate number of pages
	int nPages = nCount/m_nRowsPerPage + 1;
	pInfo->SetMaxPage(nPages);
	pInfo->m_nCurPage = 1;
	return DoPreparePrinting(pInfo);
}

#define LEFT_MARGIN 200
#define RIGHT_MARGIN 200

/********************************************************************/
/*																	*/
/* Function name : OnPrint											*/
/* Description   : Print or preview a page of the document			*/
/*																	*/
/********************************************************************/
void CDBFExplorerView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	CRect rcRow;	
	GetListCtrl().GetClientRect(&rcRow);

	// Get the list control window DC
	CDC *pControlDC = GetListCtrl().GetDC(); 
	if(pControlDC == NULL) 
		return;

	// get the average character width
	TEXTMETRIC tm;   
	pControlDC->GetTextMetrics(&tm);

	// get ratios for scaling to printer DC
	int xRatio = pDC->GetDeviceCaps(HORZRES)/rcRow.Width();

	GetListCtrl().ReleaseDC(pControlDC);

	// create logfont
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
	lf.lfHeight			= -38;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_TT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= PROOF_QUALITY;
	lf.lfPitchAndFamily = FF_SWISS | VARIABLE_PITCH;

	// create normal text font
	CFont* pOldFont = NULL;
	CFont fontNormalText;
	_tcscpy(lf.lfFaceName, _T("Arial"));
	if(fontNormalText.CreateFontIndirect(&lf))
		pOldFont = pDC->SelectObject(&fontNormalText);
	
	// Italic
	CFont fontItalicText;
	_tcscpy(lf.lfFaceName, _T("Arial Italic"));
	lf.lfItalic = TRUE;
	fontItalicText.CreateFontIndirect(&lf);
	
	// Bold
	CFont fontBoldText;
	lf.lfHeight = -72;
	fontBoldText.CreateFontIndirect(&lf);
	
	// get printer dimensions
	CRect rectPage = pInfo->m_rectDraw;
	rectPage.left += LEFT_MARGIN;

	// get text dimensions
	pDC->GetTextMetrics(&tm);
	int cyChar = tm.tmHeight;
	int nLineOffset = cyChar + cyChar / 4;

	rectPage.top += (nLineOffset * 3);

	// print database name
	CString strHeader ="Database: ";
	strHeader += GetDocument()->GetTitle();
	CSize sizeHeader = pDC->GetTextExtent(strHeader);

	pDC->SelectObject(&fontBoldText);
	pDC->TextOut(rectPage.left + (rectPage.Width()/2)- sizeHeader.cx, rectPage.top, strHeader);
	rectPage.top += (nLineOffset * 3);

	CDBFExplorerDoc* pDoc = GetDocument();

	// print database information
	CString strText;
	pDC->SelectObject(&fontItalicText);
	strText = "Number of records: ";
	pDC->TextOut(rectPage.left, rectPage.top, strText);

	// number of records
	pDC->SelectObject(&fontNormalText);
	strText.Format(_T("%d"), pDoc->m_dBaseFile->GetRecordCount());
	pDC->TextOut(rectPage.left + 350, rectPage.top, strText);
	rectPage.top += nLineOffset;	

	// last update
	pDC->SelectObject(&fontItalicText);
	strText = "Last update: ";
	pDC->TextOut(rectPage.left, rectPage.top, strText);
	pDC->SelectObject(&fontNormalText);
	pDC->TextOut(rectPage.left + 350, rectPage.top, pDoc->m_dBaseFile->GetLastUpdate());
	rectPage.top += nLineOffset;

	rectPage.top += nLineOffset;

	// print header
	pDC->SelectObject(&fontItalicText);

	TCHAR szBuff[128];
	HDITEM hdi;
	hdi.mask = HDI_TEXT | HDI_WIDTH;
	hdi.cchTextMax = 128;
	hdi.pszText = szBuff;

	int nHeaderOffset=0;
	int nColumnCount = GetListCtrl().GetHeaderCtrl()->GetItemCount();
	for (int i=0;i < nColumnCount;i++)
	{
		GetListCtrl().GetHeaderCtrl()->GetItem(i, &hdi);
		pDC->TextOut(rectPage.left+nHeaderOffset, rectPage.top, szBuff);
		nHeaderOffset += (hdi.cxy*xRatio);
	}

	rectPage.top += nLineOffset;

	// draw line
	pDC->MoveTo(rectPage.left, rectPage.top - (nLineOffset / 6));
	pDC->LineTo(rectPage.right, rectPage.top - (nLineOffset / 6));

	rectPage.top += nLineOffset;

	int nStart = m_nRowsPerPage * (pInfo->m_nCurPage - 1);

	for (int x=nStart; x <(nStart+ m_nRowsPerPage); x++)
	{
		nHeaderOffset=0;
		for (int y=0; y<nColumnCount; y++)
		{
			GetListCtrl().GetHeaderCtrl()->GetItem(y, &hdi);
			pDC->TextOut(rectPage.left+nHeaderOffset, rectPage.top, GetListCtrl().GetItemText(x, y));
			nHeaderOffset += (hdi.cxy*xRatio);
		}
		rectPage.top += nLineOffset;
	}

	// print page footer
	CString strFooter;
	strFooter.Format(_T("Page %d of %d"), pInfo->m_nCurPage, pInfo->GetMaxPage());
	CSize sizeFooter = pDC->GetTextExtent(strFooter);

	// Draw footer
	pDC->TextOut(rectPage.left + (rectPage.Width()/2)- sizeFooter.cx, rectPage.bottom - (nLineOffset * 8), strFooter);

	// select old font
	pDC->SelectObject(pOldFont);	
}

void CDBFExplorerView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pInfo = (LV_DISPINFO*)pNMHDR;
	
   if ((pInfo->item.mask & LVIF_TEXT) == LVIF_TEXT)
   {
      CString str;
	   CDBFExplorerDoc* pDoc = GetDocument();
      if (pDoc->m_dBaseFile->IsOpen())
      {
         int row = pInfo->item.iItem;
	      int col = pInfo->item.iSubItem;
         pDoc->m_dBaseFile->SetPosition((size_t)row);
         pDoc->m_dBaseFile->Read((size_t)(col + 1), &str);
      }
		_tcsncpy(pInfo->item.pszText, str, pInfo->item.cchTextMax);//set item text
   }

   *pResult = 0;
}
