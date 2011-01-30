/****************************************************************/
/*																*/
/*  DBFExplorerDoc.cpp											*/
/*																*/
/*  Implementation of the DBFExplorerDoc class.					*/
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

#include "MainFrm.h"
#include "DBFExplorerDoc.h"
#include "DBFExplorerView.h"
#include "StructureDlg.h"
#include "ExportFileDialog.h"
#include <afxpriv.h>	// for WM_SETMESSAGESTRING message
#include "..\..\dbf_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerDoc

IMPLEMENT_DYNCREATE(CDBFExplorerDoc, CDocument)

BEGIN_MESSAGE_MAP(CDBFExplorerDoc, CDocument)
	//{{AFX_MSG_MAP(CDBFExplorerDoc)
	ON_COMMAND(ID_FILE_STRUCTURE, OnFileStructure)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_PACKDATABASE, OnFilePackdatabase)
	ON_COMMAND(ID_FILE_EXPORT, OnFileExport)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerDoc construction/destruction

CDBFExplorerDoc::CDBFExplorerDoc() : CDocument(), m_dBaseFile(new CDbaseFile)
{
	*m_szTempFileName = 0;
}	

CDBFExplorerDoc::~CDBFExplorerDoc()
{
   delete m_dBaseFile;
}

void CDBFExplorerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

/********************************************************************/
/*																	*/
/* Function name : OnNewDocument									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBFExplorerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CStructureDlg dlg(AfxGetMainWnd());
	dlg.m_strTitle = _T("Create New DBF file");
	if (dlg.DoModal() == IDOK)
	{
		TCHAR szPath[MAX_PATH];
		GetTempPath(_countof(szPath), szPath);
      _tmakepath(m_szTempFileName, NULL, szPath, GetTitle(), _T(DBF_FILEEXT));
		if (m_dBaseFile->Create(m_szTempFileName, dlg.m_strFieldArray) == DBASE_SUCCESS)
		{
			if (OnOpenDocument(m_szTempFileName))
			{
				SetModifiedFlag(TRUE);
				return TRUE;
			}
			return FALSE;
		}
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* Function name : OnOpenDocument									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBFExplorerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	// open database
   bool editable = true;
	if (m_dBaseFile->Open(lpszPathName, editable) != DBASE_SUCCESS)
	{
      editable = false;
	   if (m_dBaseFile->Open(lpszPathName, editable) != DBASE_SUCCESS)
		   return FALSE;
	}
	
	SetModifiedFlag(FALSE);     // start off with unmodified
	return TRUE;
}

/********************************************************************/
/*																	*/
/* Function name : OnCloseDocument									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::OnCloseDocument() 
{
	// close database
	if (m_dBaseFile->IsOpen()) m_dBaseFile->Close();

	// delete temperary files
	DeleteFile(m_szTempFileName);
	DeleteFile(m_dBaseFile->GetMemoFileName(m_szTempFileName));

	CDocument::OnCloseDocument();
}

/********************************************************************/
/*																	*/
/* Function name : OnSaveDocument									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
BOOL CDBFExplorerDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// close database
	m_dBaseFile->Close();

	if (_tcscmp(m_szTempFileName, _T("")) != 0)
	{
		// copy memo file
		CString strMemoFile1 = m_dBaseFile->GetMemoFileName(m_szTempFileName);
		CString strMemoFile2 = m_dBaseFile->GetMemoFileName(lpszPathName);

		CopyFile(strMemoFile1, strMemoFile2, FALSE);

		if (CopyFile(m_szTempFileName, lpszPathName, FALSE))
		{
			// empty name
			lstrcpy(m_szTempFileName, _T(""));
			return OnOpenDocument(lpszPathName);
		}
		else
			return FALSE;
	}

	// copy memo file
	CString strMemoFile1 = m_dBaseFile->GetMemoFileName(GetPathName());
	CString strMemoFile2 = m_dBaseFile->GetMemoFileName(lpszPathName);

	CopyFile(strMemoFile1, strMemoFile2, FALSE);

	// make copy and open new file
	if (CopyFile(GetPathName(), lpszPathName, FALSE))
	{
		return OnOpenDocument(lpszPathName);
	}

	return OnOpenDocument(lpszPathName);
}

/********************************************************************/
/*																	*/
/* Function name : OnFileStructure									*/
/* Description   : Change file structure							*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::OnFileStructure() 
{
   USES_CONVERSION;
	CStructureDlg dlg(AfxGetMainWnd());

	// copy fieldnames
	for (size_t i = 0; i < m_dBaseFile->GetFieldCount(); i++)
	{
      DBF_FIELD_INFO info;
      if (m_dBaseFile->GetFieldInfo(i, &info))
		{
			CString strFieldData;
			strFieldData.Format(_T("%s,%c,%d,%d"), A2CT(info.name), dbf_gettype_ext2int(info.type), info.length, info.decimals);
			dlg.m_strFieldArray.Add(strFieldData);
		}
	}

	if (dlg.DoModal() == IDOK)
	{
		if(MessageBox(GetFocus(), _T("The database structure has been changed.\nSome data may be lost during the conversion of the database.\nAre you sure you want to apply the changes?"), _T("DBF Explorer"), MB_YESNO | MB_ICONQUESTION) == IDYES)
		{
			// close database
			m_dBaseFile->Close();
	
			CString strFileName, strFileBackup, strMemoBackup;

			// temp file ?
			if (_tcscmp(m_szTempFileName, _T("")) != 0)
			{
				strFileName = m_szTempFileName;
			}
			else
				strFileName = GetPathName();

			strFileBackup = strFileName;
			strMemoBackup = m_dBaseFile->GetMemoFileName(strFileName);

			// make backup names
			int nPos = strFileBackup.ReverseFind('\\');
			if (nPos != -1)
			{
				strFileBackup.SetAt(nPos+1, '_');
				strMemoBackup.SetAt(nPos+1, '_');
			}
			// delete existing backups
			DeleteFile(strFileBackup);
			DeleteFile(strMemoBackup);

			// make backup copy of existing file
			if (!MoveFile(strFileName, strFileBackup))
			{
				MessageBox(GetFocus(), _T("Error while making backup"), _T("Updating structure"), MB_ICONSTOP);
				return;
			}
			
			// make backup copy of memo file
			MoveFile(m_dBaseFile->GetMemoFileName(strFileName), strMemoBackup);

			// create file with new structure
			if (m_dBaseFile->Create(strFileName, dlg.m_strFieldArray) == DBASE_SUCCESS)
			{
				if (OnOpenDocument(strFileName))
				{
					CopyBackupData(strFileBackup);

					if (_tcscmp(m_szTempFileName, _T("")) != 0)
						SetModifiedFlag();

					// reload records
					POSITION pos = GetFirstViewPosition();
					while (pos)
						((CDBFExplorerView *)GetNextView(pos))->ShowRecords(GetActiveFrame()->m_bShowDeletedRecords);
				}
			}
		}
	}	
}

/********************************************************************/
/*																	*/
/* Function name : OnUpdateFileSave									*/
/* Description   :													*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetPathName().IsEmpty());	
}

/********************************************************************/
/*																	*/
/* Function name : OnFilePackdatabase								*/
/* Description   : Remove deleted records							*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::OnFilePackdatabase() 
{
	if (MessageBox(GetFocus(), _T("Are you sure you want to remove all deleted records from the database?"), _T("Pack Database"), MB_YESNO | MB_ICONQUESTION) != IDYES)
		return;

	if (m_dBaseFile->Pack() == DBASE_SUCCESS)
	{
		MessageBox(GetFocus(), _T("Successfully packed database."), _T("Pack Database"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(GetFocus(), _T("Failed to pack database!"), _T("Pack Database"), MB_OK | MB_ICONSTOP);
	}

	// reload records
	POSITION pos = GetFirstViewPosition();
	while (pos)
		((CDBFExplorerView *)GetNextView(pos))->ShowRecords(FALSE);
}

/********************************************************************/
/*																	*/
/* Function name : CopyBackupData									*/
/* Description   : Try to copy data from backup file to the new file*/
/*																	*/
/********************************************************************/
BOOL CDBFExplorerDoc::CopyBackupData(LPCTSTR lpszBackupFile)
{
   USES_CONVERSION;
	// copy all data
	CDbaseFile backupDBF;

	DWORD dwCounter = 0;
	CString szBuff;

	// open backup file
	if (backupDBF.Open(lpszBackupFile) == DBASE_SUCCESS)
	{
		// show all records
		for(int rc=backupDBF.GetFirstRecord(); rc==DBASE_SUCCESS; rc=backupDBF.GetNextRecord())
  		{
			dwCounter++;
			m_dBaseFile->AddRecord();
			m_dBaseFile->GetRecord(dwCounter);

			// copy all fields
			for (size_t i = 0; i <backupDBF.GetFieldCount(); i++)
			{
            DBF_FIELD_INFO pBackupField;
            backupDBF.GetFieldInfo(i, &pBackupField);

            DBF_FIELD_INFO pField;
            if (m_dBaseFile->GetFieldInfo(m_dBaseFile->FindField(pBackupField.name), &pField))
				{
					if (backupDBF.Read(i, &szBuff))
					{
						m_dBaseFile->Write(pBackupField.name, szBuff);
					}
					
					// copy memo data
					if ( (pField.type == DBF_DATA_TYPE_MEMO) && (pBackupField.type == DBF_DATA_TYPE_MEMO))
					{
						char* buff = NULL;

						const size_t nLength = backupDBF.GetMemoFieldLength(i);
						if (nLength)
						{
							buff = new char[nLength+1];
							backupDBF.GetMemoField(i, buff, nLength);
						}			
						// set value
						if (buff) m_dBaseFile->Write(m_dBaseFile->GetFieldPtr(pBackupField.name), A2CT(buff));
						delete buff;
					}
					else
					if (pField.type == DBF_DATA_TYPE_MEMO)
					{
						// initialize data
						m_dBaseFile->ClearMemoField(pBackupField.name);
					}
				}
				else
				{
					// field does not exists-> do not copy data
				}
			}
			m_dBaseFile->PutRecord(dwCounter);
		}
		backupDBF.Close();
		return TRUE;
	}
	return FALSE;
}

/********************************************************************/
/*																	*/
/* Function name : OnFileExport										*/
/* Description   : Export data to other format						*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::OnFileExport() 
{
 	static TCHAR szFilters[] = _T("Text Files (*.txt;*.csv)|*.txt;*.csv|HTML Files (*.html;*.htm)|*.html;*.htm||");
	
	CString strFileName = GetTitle();
	int nPos = strFileName.ReverseFind('.');
	if (nPos != -1)
		strFileName = strFileName.Left(nPos);
	
	CExportFileDialog dlg(FALSE, NULL, strFileName, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, GetActiveFrame());

	dlg.m_ofn.lpstrTitle = _T("Export database");

	if (dlg.DoModal() == IDOK)
	{
		strFileName = dlg.GetPathName();
		CString strExtension = dlg.GetFileExt();
		if (dlg.m_strType == _T("Text Files (*.txt;*.csv)"))
		{
			if (strExtension.IsEmpty())
				strFileName += _T(".txt");

			ExportToText(strFileName);
		}
		else
		if (dlg.m_strType == _T("HTML Files (*.html;*.htm)"))
		{
			if (strExtension.IsEmpty())
				strFileName += _T(".html");

			ExportToHTML(strFileName);
		}
	}	
}

/********************************************************************/
/*																	*/
/* Function name : ExportToText										*/
/* Description   : Export data to text format						*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::ExportToText(LPCTSTR lpszFileName)
{
   USES_CONVERSION;
	try
	{
		CFile file;

		// Create "progress bar"
		CProgressCtrl wndProgress;

		GetActiveFrame()->ShowProgressBar(&wndProgress, _T("Exporting records, press Esc to cancel..."));
    
		// Initialize progress control range and step size
		wndProgress.SetRange(0, 100);

		int nCount = m_dBaseFile->GetRecordCount();

		if (file.Open(lpszFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL))
		{
	    	// show all records
			for(int rc=m_dBaseFile->GetFirstRecord(); rc==DBASE_SUCCESS; rc=m_dBaseFile->GetNextRecord())
  			{
				// Test for "Esc" key
				if( ::GetAsyncKeyState(VK_ESCAPE) < 0)
				{
					break;
				}

				int nItem = m_dBaseFile->GetPosition();
				
				// do not show deleted records
				if (m_dBaseFile->IsRecordDeleted() && !GetActiveFrame()->m_bShowDeletedRecords)
					continue;

				for (size_t i = 0; i < m_dBaseFile->GetFieldCount(); i++)
				{
					CString szBuff;

               DBF_FIELD_INFO info;
               m_dBaseFile->GetFieldInfo(i, &info);
					if (info.type == DBF_DATA_TYPE_MEMO)
					{
						szBuff = _T("MEMO");
					}
					else
					{
						m_dBaseFile->Read(i, &szBuff);
					}
					if (i != 1)
						file.Write(",", 1);
					file.Write(T2CA(szBuff), szBuff.GetLength());
				}
				file.Write("\r\n", 2);
				// Update progress control
				int nPos = (nItem*100)/nCount;
				wndProgress.SetPos(nPos);
			}	
		}
	}
	catch(CFileException *e)
	{
		e->Delete();
		MessageBox(GetFocus(), _T("Failed to export database!"), _T("Export Database"), MB_OK | MB_ICONSTOP);
	}
	// Ensures that idle message is displayed again
	GetActiveFrame()->PostMessage(WM_SETMESSAGESTRING, (WPARAM) AFX_IDS_IDLEMESSAGE, 0L);
}	

/********************************************************************/
/*																	*/
/* Function name : ExportToHTML										*/
/* Description   : Export data to HTML format						*/
/*																	*/
/********************************************************************/
void CDBFExplorerDoc::ExportToHTML(LPCTSTR lpszFileName)
{
   USES_CONVERSION;
	try
	{
		CFile file;
		CString strHTML;

		// Create "progress bar"
		CProgressCtrl wndProgress;

		GetActiveFrame()->ShowProgressBar(&wndProgress, _T("Exporting records, press Esc to cancel..."));
    
		// Initialize progress control range and step size
		wndProgress.SetRange(0, 100);

		int nCount = m_dBaseFile->GetRecordCount();

		if (file.Open(lpszFileName, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, NULL))
		{
			strHTML = "<HTML>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<HEAD>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html;charset=windows-1252\">\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML.Format(_T("<TITLE>%s</TITLE>\r\n"), GetTitle());
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</HEAD>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "<BODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML.Format(_T("<TABLE BORDER=1 BGCOLOR=#FFFFFF CELLSPACING=0><FONT FACE=\"Arial\" COLOR=#000000><CAPTION><B>%s</B></CAPTION></FONT>\r\n"), GetTitle().operator LPCTSTR());
			file.Write(strHTML, strHTML.GetLength());

			strHTML = "<THEAD>\r\n<TR>\r\n";
			file.Write(strHTML, strHTML.GetLength());

			// show fieldnames
			for (size_t i = 0; i < m_dBaseFile->GetFieldCount(); i++)
			{
            DBF_FIELD_INFO info;
            if (m_dBaseFile->GetFieldInfo(i, &info))
		      {
					strHTML.Format(_T("<TH BGCOLOR=#c0c0c0 BORDERCOLOR=#000000 ><FONT SIZE=2 FACE=\"Arial\" COLOR=#000000>%s</FONT></TH>\r\n"), 
                  A2CT(info.name));
					file.Write(strHTML, strHTML.GetLength());
				}
			}
			strHTML = "</TR>\r\n</THEAD>\r\n\r\n<TBODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());

			// show all records
			for(int rc=m_dBaseFile->GetFirstRecord(); rc==DBASE_SUCCESS; rc=m_dBaseFile->GetNextRecord())
  			{
				// Test for "Esc" key
				if( ::GetAsyncKeyState(VK_ESCAPE) < 0)
				{
					break;
				}

				int nItem = m_dBaseFile->GetPosition();
				
				// do not show deleted records
				if (m_dBaseFile->IsRecordDeleted() && !GetActiveFrame()->m_bShowDeletedRecords)
					continue;

				strHTML = "<TR VALIGN=TOP>\r\n";
				file.Write(strHTML, strHTML.GetLength());
				
				for (size_t i = 0; i < m_dBaseFile->GetFieldCount(); i++)
				{
					CString szBuff;

               DBF_FIELD_INFO info;
               m_dBaseFile->GetFieldInfo(i, &info);
					if (info.type == DBF_DATA_TYPE_MEMO)
					{
						szBuff = _T("MEMO");
					}
					else
					{
						m_dBaseFile->Read(i, &szBuff);
					}
					strHTML.Format(_T("<TD BORDERCOLOR=#c0c0c0 ><FONT SIZE=2 FACE=\"Arial\" COLOR=#000000>%s</FONT></TD>\r\n"), 
                  szBuff.operator LPCTSTR());
					file.Write(strHTML, strHTML.GetLength());
				}

				strHTML = "</TR>\r\n";
				file.Write(strHTML, strHTML.GetLength());

				// Update progress control
				int nPos = (nItem*100)/nCount;
				wndProgress.SetPos(nPos);
			}	
			strHTML = "</TBODY>\r\n<TFOOT></TFOOT>\r\n</TABLE>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</BODY>\r\n";
			file.Write(strHTML, strHTML.GetLength());
			strHTML = "</HTML>\r\n";
			file.Write(strHTML, strHTML.GetLength());
		}
	}
	catch(CFileException *e)
	{
		e->Delete();
		MessageBox(GetFocus(), _T("Failed to export database!"), _T("Export Database"), MB_OK | MB_ICONSTOP);
	}
	// Ensures that idle message is displayed again
	GetActiveFrame()->PostMessage(WM_SETMESSAGESTRING, (WPARAM) AFX_IDS_IDLEMESSAGE, 0L);
}

int CDBFExplorerDoc::GetRecordCount()
{
   return m_dBaseFile->GetRecordCount();
}
