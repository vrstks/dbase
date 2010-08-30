/****************************************************************/
/*																*/
/*  DBFExplorer.cpp												*/
/*																*/
/*  Implementation of the DBFExplorerApp class.					*/
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
#include <locale.h>

#include "MainFrm.h"
#include "ChildFrm.h"
#include "DBFExplorerDoc.h"
#include "DBFExplorerView.h"
#include "aboutdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CDBFExplorerApp, CWinApp)
	//{{AFX_MSG_MAP(CDBFExplorerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_HELP_INDEX, OnHelpIndex)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerApp construction

CDBFExplorerApp::CDBFExplorerApp() : CWinApp(), m_doctemplate(NULL)
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDBFExplorerApp object

static CDBFExplorerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDBFExplorerApp initialization

BOOL CDBFExplorerApp::InitInstance()
{
   _tsetlocale(LC_ALL, _T(""));
   // Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	InitCommonControls();
   AfxDaoInit();

	// Change the registry key under which our settings are stored.
   SetRegistryKey(_T("Pablo Software Solutions"));

	LoadStdProfileSettings(max(_AFX_MRU_COUNT, 9));  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

   AddDocTemplates();

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE | FWS_PREFIXTITLE))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
#ifdef _DEBUG
   RegisterShellFileTypes(TRUE);
#endif

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;

	// prevent application from opening a new (empty) document
	//cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) 
      cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	m_nCmdShow = SW_SHOWMAXIMIZED;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	return TRUE;
}


// App command to run the dialog
void CDBFExplorerApp::OnAppAbout()
{
	CAboutDlg(m_pMainWnd).DoModal();
}

/********************************************************************/
/*																	*/
/* Function name : OnHelpIndex										*/
/* Description   : Command to show help file.						*/
/*																	*/
/********************************************************************/
void CDBFExplorerApp::OnHelpIndex() 
{
	// launch help
	::WinHelp(AfxGetMainWnd()->m_hWnd, AfxGetApp()->m_pszHelpFilePath, HELP_CONTENTS, 0L);	
}

extern bool mdb2dbf(CWnd* parent, const TCHAR* src, CStringArray* newfile, const CString& rtablename);

class CAccessDocTemplate : public CMultiDocTemplate
{
public:
   CAccessDocTemplate() : CMultiDocTemplate(
		IDR_TYPE_MDB,
		RUNTIME_CLASS(CDocument),
		RUNTIME_CLASS(CMDIChildWnd),
		RUNTIME_CLASS(CView))
   {
   }
   virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL /*bMakeVisible*/)
   {
      CWnd* parent = GetActiveFrame();
      CDocument* doc = NULL;
      CStringArray newfile;
      CString tablename;
   #ifdef x_DEBUG
      tablename = _T("årstal");
      //tablename = _T("tur_tur");
      //tablename = _T("Adresser");
   #endif
      bool ok = ::mdb2dbf(parent, lpszPathName, &newfile, tablename);
      if (ok)
      {
         if (newfile.GetSize())
         {
            for (int i = 0; i < newfile.GetSize(); i++)
            {
	            doc = theApp.m_doctemplate->OpenDocumentFile(newfile.ElementAt(i));
            }
         }
         else // user chose csv
         {
            AfxMessageBox(_T("Done"));
         }
         AfxGetApp()->AddToRecentFileList(lpszPathName);
      }
      else AfxMessageBox(_T("Error"));
      return doc;
   }
};

void CDBFExplorerApp::AddDocTemplates()
{
   CMultiDocTemplate* doctemplate = m_doctemplate = new CMultiDocTemplate(
		IDR_TYPE_DBF,
		RUNTIME_CLASS(CDBFExplorerDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDBFExplorerView));
	AddDocTemplate(doctemplate);

   doctemplate = new CAccessDocTemplate;
	AddDocTemplate(doctemplate);
}
