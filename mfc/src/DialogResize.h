// dialog resize map macros

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

#define DECLARE_DLGRESIZE_MAP \
	virtual const AfxResizeMap* GetDlgResizeMap();

#define BEGIN_DLGRESIZE_MAP(theClass) \
	const theClass::AfxResizeMap* theClass::GetDlgResizeMap() \
	{ \
		static const AfxResizeMap theMap[] = \
		{

#define END_DLGRESIZE_MAP() \
			{ -1, 0 }, \
		}; \
		return theMap; \
	}

#define DLGRESIZE_CONTROL(id, flags) \
		{ id, flags },

#define BEGIN_DLGRESIZE_GROUP() \
		{ -1, _DLSZ_BEGIN_GROUP },

#define END_DLGRESIZE_GROUP() \
		{ -1, _DLSZ_END_GROUP },

class CDialogResize : public CDialog
{
public:
// Data declarations and members
	enum
	{
		DLSZ_SIZE_X		= 0x00000001,
		DLSZ_SIZE_Y		= 0x00000002,
		DLSZ_MOVE_X		= 0x00000004,
		DLSZ_MOVE_Y		= 0x00000008,
		DLSZ_REPAINT		= 0x00000010,

		// internal use only
		_DLSZ_BEGIN_GROUP	= 0x00001000,
		_DLSZ_END_GROUP		= 0x00002000,
		_DLSZ_GRIPPER		= 0x00004000
	};

	struct AfxResizeMap
	{
		int m_nCtlID;
		DWORD m_dwResizeFlags;
	};

	struct AfxResizeData
	{
		int m_nCtlID;
		DWORD m_dwResizeFlags;
		RECT m_rect;
		int GetGroupCount() const
		{
			return (int)LOBYTE(HIWORD(m_dwResizeFlags));
		}
		void SetGroupCount(int nCount)
		{
			ASSERT(nCount > 0 && nCount < 256);
			DWORD dwCount = (DWORD)MAKELONG(0, MAKEWORD(nCount, 0));
			m_dwResizeFlags &= 0xFF00FFFF;
			m_dwResizeFlags |= dwCount;
		}
		BOOL operator==(const AfxResizeData& r) const
		{ 
			return (m_nCtlID == r.m_nCtlID && m_dwResizeFlags == r.m_dwResizeFlags); 
		}
	};

	CArray<AfxResizeData, AfxResizeData&> m_arrData;
	SIZE m_sizeDialog;
	POINT m_ptMinTrackSize;
	BOOL m_bGripper;

	void InitResizing(BOOL bAddGripper = TRUE, BOOL bUseMinTrackSize = TRUE, DWORD dwForceStyle = WS_THICKFRAME | WS_CLIPCHILDREN);
	void UpdateLayout(int cxWidth, int cyHeight);
	BOOL PositionControl(int cxWidth, int cyHeight, RECT& rectGroup, AfxResizeData& data, bool bGroup, int xyStartNext = -1);

// Construction
public:
	CDialogResize(UINT uResourceID, CWnd* pParent = NULL);
	CDialogResize(LPCTSTR pszResourceID, CWnd* pParent = NULL);
	CDialogResize();

	void CommonConstruct();

// Dialog Data
	//{{AFX_DATA(CDialogResize)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDialogResize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd m_wndGripper;

	// Generated message map functions
	//{{AFX_MSG(CDialogResize)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	DECLARE_DLGRESIZE_MAP;
};
