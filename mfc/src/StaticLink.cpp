/****************************************************************/
/*																*/
/*  StaticLink.cpp	 											*/
/*																*/
/*  Implementation of the CStaticLink class.					*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Copyright Pablo Software Solutions 2002						*/
/*	http://www.pablovandermeer.nl								*/
/*																*/
/*  Last updated: 15 may 2002									*/
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
#include "resource.h"
#include "StaticLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

CStaticLink::CStaticLink() : CStatic()
{
	m_clrText = RGB(0,0,255);
	m_strURL = "http://www.pablovandermeer.nl";
	m_hCursor = LoadCursor(AfxGetResourceHandle(), MAKEINTRESOURCE(IDC_FINGER));
}

CStaticLink::~CStaticLink()
{
}

BEGIN_MESSAGE_MAP(CStaticLink, CStatic)
	//{{AFX_MSG_MAP(CStaticLink)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_NCHITTEST()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/********************************************************************/
/*																	*/
/* Function name : CtlColor											*/
/* Description   : Change text color of static control.				*/
/*																	*/
/********************************************************************/
HBRUSH CStaticLink::CtlColor(CDC* pDC, UINT /*nCtlColor*/) 
{
    pDC->SetTextColor(m_clrText);
    pDC->SetBkMode(TRANSPARENT);

    // return hollow brush to let parent background color shine through
    return (HBRUSH)::GetStockObject(HOLLOW_BRUSH);
}

/********************************************************************/
/*																	*/
/* Function name : PreSubclassWindow								*/
/* Description   : Called just before the window is subclassed.		*/
/*																	*/
/********************************************************************/
void CStaticLink::PreSubclassWindow() 
{
	// set up font
	LOGFONT lf;
    GetFont()->GetObject(sizeof(lf), &lf);
    lf.lfUnderline = TRUE;
    m_font.CreateFontIndirect(&lf);

	SetFont(&m_font);
	
	CStatic::PreSubclassWindow();
}

/********************************************************************/
/*																	*/
/* Function name : SetURL											*/
/* Description   : Member function to set URL.						*/
/*																	*/
/********************************************************************/
void CStaticLink::SetURL(LPCTSTR lpszURL)
{
	m_strURL = lpszURL;
}

/********************************************************************/
/*																	*/
/* Function name : OnNcHitTest										*/
/* Description   : 'Activate' mouse messages.						*/
/*																	*/
/********************************************************************/
#if (_MFC_VER < 0x0800)
UINT CStaticLink::OnNcHitTest(CPoint) 
#else
LRESULT CStaticLink::OnNcHitTest(CPoint) 
#endif
{
	return HTCLIENT;
}

/********************************************************************/
/*																	*/
/* Function name : OnSetCursor										*/
/* Description   : Change default cursor to finger.					*/
/*																	*/
/********************************************************************/
BOOL CStaticLink::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (m_hCursor) 
	{
		::SetCursor(m_hCursor);
		return TRUE;
	}	
	return CStatic::OnSetCursor(pWnd, nHitTest, message);
}

/********************************************************************/
/*																	*/
/* Function name : OnLButtonDown									*/
/* Description   : Link has been clicked, launch explorer.			*/
/*																	*/
/********************************************************************/
void CStaticLink::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_strURL.IsEmpty())
	{
		// get it from window text
		GetWindowText(m_strURL);  
	}
 
    // open URL in the browser.
	ShellExecute(NULL, _T("open"), m_strURL, NULL, NULL, SW_SHOWNORMAL);	

	CStatic::OnLButtonDown(nFlags, point);
}
