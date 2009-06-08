// RecordListBox.cpp : implementation file

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
#include "RecordListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRecordListBox

CRecordListBox::CRecordListBox()
{
	m_nDateField = -1;
}

CRecordListBox::~CRecordListBox()
{
}

BEGIN_MESSAGE_MAP(CRecordListBox, CListBox)
	//{{AFX_MSG_MAP(CRecordListBox)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRecordListBox message handlers

void CRecordListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// if there are no items in the control yet, indicate focus
	if (lpDrawItemStruct->itemID == -1)
	{
		DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&lpDrawItemStruct->rcItem);
		return;
	}

	// normal/any item draw action -- checks for selection and focus
	if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		// Use the itemID to get to the text string
		//SendMessage(LB_GETTEXT, (WPARAM)lpDrawItemStruct->itemID, (LPARAM)(LPSTR)szField);
		//lstrcpy(szValue, szProp[lpDrawItemStruct->itemID]);

		HandleSelectionState(lpDrawItemStruct);

		// Handle focus state if necessary
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&lpDrawItemStruct->rcItem);
		return;
	}

	// selected/unselected item draw action -- red text on blue background
	if (lpDrawItemStruct->itemAction & ODA_SELECT)
	{
		// Use the itemID to get to the text string
		//SendMessage(LB_GETTEXT, (WPARAM)lpDrawItemStruct->itemID, (LPARAM)(LPSTR)szField);
		//lstrcpy(szValue,szProp[lpDrawItemStruct->itemID]);

		HandleSelectionState(lpDrawItemStruct);

		// Handle focus state if necessary
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&lpDrawItemStruct->rcItem);
		return;
	}

	// focus/unfocus item draw action -- dashed rectangular border
	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&lpDrawItemStruct->rcItem);
	}
}

void CRecordListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	TEXTMETRIC tm;
	// get device context
	CDC *pDC = GetDC();                     
	// get text information
	pDC->GetTextMetrics(&tm);              
	lpMeasureItemStruct->itemHeight = tm.tmHeight;
	// release device context	
	ReleaseDC(pDC);                  
}

/* Handles indicating selection of item by drawing text white on black background.	*/
void CRecordListBox::HandleSelectionState(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HBRUSH hBrush;
	DWORD dwOldTextColor, dwOldBkColor;
	CRect rc;
	HPEN hOldPen, hPen;
	HFONT hOldFont;
	//HWND hwndEdit;
	static int nOldItemID = -1;
	//int day, mon, yr;
  	//char Ch1, Ch2;
	//char szVar[255];
	//SYSTEMTIME SysTime;
	//char szTemp[12];

	// if selected, draw selected item's text red, highlight rectangle blue
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		if ((nOldItemID != (int)lpDrawItemStruct->itemID) && (m_nDateField != -1))
		{
/*			DateTime_GetSystemtime(hWndDate, &SysTime);
			if (SysTime.wYear < 2000)
				wsprintf(szVar,"%02d/%02d/%02d",SysTime.wMonth,SysTime.wDay,SysTime.wYear-1900);
			else
				wsprintf(szVar,"%02d/%02d/%02d",SysTime.wMonth,SysTime.wDay,SysTime.wYear-2000);
//		  lstrcpy(szProp[itemIDold],szVar); */
		}
		nOldItemID = lpDrawItemStruct->itemID;
		
		// draw black background rectangle
		hBrush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)); 
		rc = lpDrawItemStruct->rcItem;
		rc.right = (rc.right-rc.left)/2;
		FillRect(lpDrawItemStruct->hDC, (LPRECT)&rc, hBrush);
		DeleteObject(hBrush);

		rc = lpDrawItemStruct->rcItem;
		
		// change text and back ground color of list box's selection
		dwOldTextColor = SetTextColor(lpDrawItemStruct->hDC, RGB(255, 255,255));
		dwOldBkColor = SetBkColor(lpDrawItemStruct->hDC, GetSysColor(COLOR_HIGHLIGHT));
		hOldFont = (HFONT)SelectObject(lpDrawItemStruct->hDC, m_hBoldFont);
		
		CString strField;
		GetText(lpDrawItemStruct->itemID, strField);
		TextOut(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left+2, lpDrawItemStruct->rcItem.top+1,
				strField, strField.GetLength());

		SelectObject(lpDrawItemStruct->hDC, hOldFont);

		ITEMINFO *pInfo = (ITEMINFO *)lpDrawItemStruct->itemData;

/*		if(pInfo->nType == 'D')
		{
			m_nDateField = lpDrawItemStruct->itemID;
     		lstrcpy(szTemp, szProp[lpDrawItemStruct->itemID]);
	  		sscanf(szTemp,"%02d%c%02d%c%02d",&mon,&Ch1,&day,&Ch2,&yr);
			SysTime.wMonth = (WORD)mon;
			SysTime.wDay = (WORD)day;
			if (yr > 25)
				SysTime.wYear = (WORD)(yr + 1900);
			else
				SysTime.wYear = (WORD)(yr + 2000);

			ShowWindow(hwndEdit,SW_HIDE);
			MoveWindow(hWndDate,(rc.right-rc.left)/2+1,rc.top,
                         (rc.right-rc.left)/2,rc.bottom-rc.top+2,TRUE);
			ShowWindow(hWndDate,SW_SHOW);
			SendMessage(hWndDate, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&SysTime);
		}
		else */
		{
			m_nDateField = -1;

			if (IsWindow(m_WndEdit.m_hWnd))
			{
				m_WndEdit.ShowWindow(SW_SHOW);
				m_WndEdit.MoveWindow(rc.Width()/2 + 2, 
									 rc.top + 1,
									 rc.Width()/2 - 2,
									 rc.Height()-2,
									 TRUE);
				
				m_WndEdit.SetWindowText(CString(pInfo->szValue));
				m_WndEdit.SetLimitText(pInfo->nLength);
			}

		//	ShowWindow(hWndDate, SW_HIDE);
		}
		//	  TextOut(lpDrawItemStruct->hDC, (int)((rc.right-rc.left)/2)+2, (int)(lpDrawItemStruct->rcItem.top+1),
//			  (LPSTR)szValue, lstrlen(szValue));

		// restore text and back ground color of list box's selection
		SetTextColor(lpDrawItemStruct->hDC, dwOldTextColor);
		SetBkColor(lpDrawItemStruct->hDC, dwOldBkColor);
	}
	// item not selected; draw black text on white background
	else 
	{
		// draw white back ground rectangle
  		hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
		FillRect(lpDrawItemStruct->hDC, (LPRECT)&lpDrawItemStruct->rcItem, hBrush);
		hPen = CreatePen(PS_SOLID,1,RGB(192,192,192)); // grijze lijn
		hBrush = CreateSolidBrush(RGB(0,0,0));
		hOldPen = (HPEN)SelectObject(lpDrawItemStruct->hDC, hPen);
		rc = lpDrawItemStruct->rcItem;

		MoveToEx(lpDrawItemStruct->hDC, rc.left, rc.top,0);
		LineTo(lpDrawItemStruct->hDC, rc.right, rc.top);
		MoveToEx(lpDrawItemStruct->hDC, rc.left, rc.bottom,0);
		LineTo(lpDrawItemStruct->hDC, rc.right, rc.bottom);

		MoveToEx(lpDrawItemStruct->hDC, (rc.right - rc.left)/2, rc.top,0);
		LineTo(lpDrawItemStruct->hDC, (rc.right - rc.left)/2, rc.bottom);

		DeleteObject(hBrush);
		SelectObject(lpDrawItemStruct->hDC,hOldPen);
		DeleteObject(hPen);

		LPTSTR lpszItemData = (LPTSTR)lpDrawItemStruct->itemData;
		if (m_nDateField != (int)lpDrawItemStruct->itemID)
	  		TextOut(lpDrawItemStruct->hDC, (int)((rc.right-rc.left)/2)+2, (int)(lpDrawItemStruct->rcItem.top+1),
			  lpszItemData, lstrlen(lpszItemData));

		hOldFont = (HFONT)SelectObject(lpDrawItemStruct->hDC, m_hBoldFont);

		CString strField;
		GetText(lpDrawItemStruct->itemID, strField);

		TextOut(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left+2, lpDrawItemStruct->rcItem.top+1,
			  strField, strField.GetLength());
		SelectObject(lpDrawItemStruct->hDC, hOldFont);
   }
   return;
}

int CRecordListBox::AddString(LPCTSTR lpszText, LPCTSTR lpszValue, char nType, int nLength)
{
	int nIndex = CListBox::AddString(lpszText);
	
	ITEMINFO *pInfo = new ITEMINFO;
	lstrcpy(pInfo->szValue, lpszValue);
	pInfo->nType = nType;
	pInfo->nLength = nLength;
	SetItemData(nIndex, (DWORD)pInfo);
	return nIndex;
}

void CRecordListBox::DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct) 
{
	// delete item data
	if (!lpDeleteItemStruct->itemData)
		delete(ITEMINFO*) lpDeleteItemStruct->itemData;

	CListBox::DeleteItem(lpDeleteItemStruct);
}

void CRecordListBox::PreSubclassWindow() 
{
	if (m_WndEdit.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, 30))
	{
		m_WndEdit.SetFont(GetFont());
	}

	CFont *pFont = GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	// makeit bold
	lf.lfWeight = FW_BOLD;
	m_hBoldFont = CreateFontIndirect(&lf);

	CListBox::PreSubclassWindow();
}

void CRecordListBox::OnSelchange() 
{
	if (m_WndEdit.IsWindowVisible())
	{
		m_WndEdit.SetFocus();
	}
}
