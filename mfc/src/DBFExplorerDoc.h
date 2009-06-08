// DBFExplorerDoc.h : interface of the CDBFExplorerDoc class

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

class CDbaseFile;
class CDBFExplorerDoc : public CDocument
{
protected: // create from serialization only
	CDBFExplorerDoc();
	DECLARE_DYNCREATE(CDBFExplorerDoc)

// Attributes
public:
	CDbaseFile* m_dBaseFile;

// Operations
public:
   int GetRecordCount();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBFExplorerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive&);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDBFExplorerDoc();

protected:

// Generated message map functions
protected:
	void ExportToHTML(LPCTSTR lpszFileName);
	void ExportToText(LPCTSTR lpszFileName);
	BOOL CopyBackupData(LPCTSTR lpszBackupFile);
	TCHAR m_szTempFileName[MAX_PATH];
	//{{AFX_MSG(CDBFExplorerDoc)
	afx_msg void OnFileStructure();
	afx_msg void OnUpdateFileSave(CCmdUI*);
	afx_msg void OnFilePackdatabase();
	afx_msg void OnFileExport();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
