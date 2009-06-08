/****************************************************************/
/*																*/
/*  ChildFrm.cpp												*/
/*																*/
/*  Implementation of the CChildFrm class.						*/
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

#include "ChildFrm.h"
#include "MainFrm.h"
#include "DBFExplorerDoc.h"
#include "DBFExplorerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_RECCOUNT, OnUpdateRecordCount)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame() : CMDIChildWnd()
{
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

void CChildFrame::OnUpdateRecordCount(CCmdUI *pCmdUI)
{
	CDBFExplorerView *pView = (CDBFExplorerView *)GetActiveView();
	
	if (pView)
	{
		CDBFExplorerDoc *pDoc = pView->GetDocument();
		
		CString txt;
		txt.Format(_T("Total records: %d"), pDoc->GetRecordCount());

		::GetActiveFrame()->SetPaneWidth(1, txt);
		pCmdUI->SetText(txt);
	}
	else
	{
		::GetActiveFrame()->SetPaneWidth(1, _T(""));
		pCmdUI->SetText(_T(""));
	}
}
