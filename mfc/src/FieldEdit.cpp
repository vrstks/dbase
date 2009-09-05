/****************************************************************/
/*																*/
/*  FieldEdit.cpp												*/
/*																*/
/*  Implementation of the CFieldEdit class.						*/
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
#include "FieldEdit.h"
#include "..\..\bool.h"
#include "..\..\dbf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

CFieldEdit::CFieldEdit(int iItem, int iSubItem, LPCTSTR lpszText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bEscape = FALSE;
	m_iNextSubItem = iSubItem;
	m_strText = lpszText;
	m_strText.TrimLeft();
	m_strText.TrimRight();
}

CFieldEdit::~CFieldEdit()
{
}

BEGIN_MESSAGE_MAP(CFieldEdit, CEdit)
	//{{AFX_MSG_MAP(CFieldEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************************/
/*																	*/
/* Function name : PreTranslateMessage								*/
/* Description   : Catch specific keys before they go to parent		*/
/*																	*/
/********************************************************************/
BOOL CFieldEdit::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(pMsg->wParam == VK_RETURN
				|| pMsg->wParam == VK_TAB
				|| pMsg->wParam == VK_DELETE
				|| pMsg->wParam == VK_ESCAPE
				|| GetKeyState(VK_CONTROL))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}

/********************************************************************/
/*																	*/
/* Function name : OnKillFocus										*/
/* Description   : We are losing focus, forward data to listctrl	*/
/*				   and kill ourselves.								*/
/*																	*/
/********************************************************************/
void CFieldEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	CString strText;
	GetWindowText(strText);

	// Send notification to parent of ListViewCtrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bEscape ? NULL : LPTSTR((LPCTSTR)strText);
	dispinfo.item.cchTextMax = strText.GetLength();

	dispinfo.item.lParam = m_iNextSubItem;

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	DestroyWindow();	
}

/********************************************************************/
/*																	*/
/* Function name : OnNcDestroy										*/
/* Description   : Control is dynamically created so clean when		*/
/*				   destroyed!										*/
/*																	*/
/********************************************************************/
void CFieldEdit::OnNcDestroy() 
{
	CEdit::OnNcDestroy();
	
	delete this;	
}

/********************************************************************/
/*																	*/
/* Function name : OnChar											*/
/* Description   : Process typed characters							*/
/*																	*/
/********************************************************************/
void CFieldEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_ESCAPE || nChar == VK_RETURN || nChar == VK_TAB)
	{
		if(nChar == VK_ESCAPE)
			m_bEscape = TRUE;
		if(nChar == VK_TAB)
		{
			if (GetKeyState(VK_SHIFT) >= 0)
				m_iNextSubItem = m_iSubItem+1;
			else
			{
				m_iNextSubItem = m_iSubItem-1;
				if (m_iNextSubItem < 0)
					m_iNextSubItem = 0;
			}
		}
		GetParent()->SetFocus();
		return;
	}
	
	TCHAR tChar = (TCHAR)nChar;

	if ( (m_type == DBF_DATA_TYPE_INTEGER) || (m_type == DBF_DATA_TYPE_FLOAT))
	{
		if(tChar != '+' && tChar != '-' && tChar != '.' && tChar != '0' && tChar != '1' &&
			tChar != '2' && tChar != '3' && tChar != '4' && tChar != '5' && tChar != '6' &&
			tChar != '7' && tChar != '8' && tChar != '9' && tChar != ',' && tChar != '\b')
			return;
	}

	if (m_type == DBF_DATA_TYPE_BOOLEAN)
	{
		if(tChar != 'T' && tChar != 't' && tChar != 'F' && tChar != 'f' &&
		   tChar != 'Y' && tChar != 'y' && tChar != 'N' && tChar != 'n' && tChar != '?' &&
		   tChar != '\b')
			return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString strText;

	GetWindowText(strText);
	CWindowDC dc(this);
	CFont *pFont = GetParent()->GetFont();
	CFont *pFontDC = dc.SelectObject(pFont);
	CSize size = dc.GetTextExtent(strText);
	dc.SelectObject(pFontDC);
	size.cx += 5;			   	// add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect(&rect);
	GetParent()->GetClientRect(&parentrect);

	// Transform rect to parent coordinates
	ClientToScreen(&rect);
	GetParent()->ScreenToClient(&rect);

	// Check whether control needs to be resized
	// and whether there is space to grow
	if(size.cx > rect.Width())
	{
		if(size.cx + rect.left < parentrect.right)
			rect.right = rect.left + size.cx;
		else
			rect.right = parentrect.right;
		MoveWindow(&rect);
	}	
}

/********************************************************************/
/*																	*/
/* Function name : OnCreate											*/
/* Description   : Set better looking font on creation				*/
/*																	*/
/********************************************************************/
int CFieldEdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetWindowText(m_strText);

	SetFocus();
	SetSel(0, -1);

	// Set the proper font
	CFont *pFont = GetParent()->GetFont();
	SetFont(pFont);

	CRect rc;
	GetWindowRect(rc);

	CDC *pDC = GetDC();
	CSize textSize = pDC->GetTextExtent(m_strText);
	ReleaseDC(pDC);

	int nHeight = textSize.cy + (GetSystemMetrics(SM_CYBORDER) * 2);
	if (rc.Height() < nHeight)
	{
		SetWindowPos(NULL, 0, 0, rc.Width(), nHeight, SWP_NOMOVE | SWP_NOZORDER);
	}
	return 0;
}

