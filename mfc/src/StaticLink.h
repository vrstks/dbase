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

class CStaticLink : public CStatic
{
// Construction
public:
	CStaticLink();

// Attributes
public:
     COLORREF m_clrText;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStaticLink)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	HCURSOR m_hCursor;
	void SetURL(LPCTSTR lpszURL);
	virtual ~CStaticLink();

	// Generated message map functions
protected:
	CString m_strURL;
     CFont m_font;

	//{{AFX_MSG(CStaticLink)
	afx_msg HBRUSH CtlColor(CDC*, UINT nCtlColor);
	afx_msg BOOL OnSetCursor(CWnd*, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint);
	//}}AFX_MSG
#if (_MFC_VER < 0x0800)
   afx_msg UINT OnNcHitTest(CPoint);
#else
   afx_msg LRESULT OnNcHitTest(CPoint);
#endif
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
