// app.cpp
// Copyright (c) 2007 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "appframe.h"
//#include <locale.h>

IMPLEMENT_APP(wxDBFApp)

BEGIN_EVENT_TABLE(wxDBFApp, wxApp)
END_EVENT_TABLE()

wxDBFApp::wxDBFApp(void) : wxApp(), m_docManager(NULL)
{
}

void wxDBFApp::FileHistoryUseMenu(wxMenu* file_menu)
{
   if(0)if (file_menu == NULL)
   {
      file_menu = GetMainFrame()->m_file_menu;
   }
   //m_docManager->FileHistoryUseMenu(file_menu);
   //m_docManager->FileHistoryAddFilesToMenu();
}

void wxDBFApp::FileHistoryRemoveMenu(wxMenu* file_menu)
{
   m_docManager->FileHistoryRemoveMenu(file_menu);
}

bool wxDBFApp::OnInit(void)
{
   bool ok = wxApp::OnInit();
   if (ok)
   {
      //_tsetlocale(LC_ALL, wxT(""));
      m_locale.Init();

      SetVendorName(wxT("Troels K"));
      SetAppName(wxT("wxDBF"));
      //wxConfigBase* pConfig = wxConfigBase::Get();

      wxXmlResource::Get()->InitAllHandlers();
      wxFileSystem::AddHandler(new wxZipFSHandler);
      wxString dir;
      wxFileName::SplitPath(this->argv[0], &dir, NULL, NULL);

      const wxString xrc = wxFileName(dir, GetAppName(), wxT("xrc")).GetFullPath();
      ok = wxXmlResource::Get()->Load(xrc);
      if (ok)
      {
         AddDocTemplates();

         wxDBFAppFrame* frame = new wxDBFAppFrame(m_docManager, NULL, GetAppName(), wxDefaultPosition, wxSize(640,480),
                            wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

         wxMenuBar* menu = frame->GetMenuBar();
         wxMenu* file_menu = menu->GetMenu(0);
         m_docManager->FileHistoryUseMenu(file_menu);
         FileHistoryLoadSave(false);

         SetTopWindow(frame);

         wxFileHistory* mru = m_docManager->GetFileHistory();
         if (mru->GetCount()) m_docManager->CreateDocument(mru->GetHistoryFile(0), wxDOC_SILENT);
      }
   }
   return ok;
}

void wxDBFApp::FileHistoryLoadSave(bool save)
{
   wxConfigBase* pConfig = wxConfigBase::Get();
   pConfig->SetPath(wxT("MRU"));
   if (save)
   {
      m_docManager->FileHistorySave(*pConfig);
   }
   else
   {
      m_docManager->FileHistoryLoad(*pConfig);
   }
   pConfig->SetPath(wxT("/"));
}

int wxDBFApp::OnExit(void)
{
   FileHistoryLoadSave(true);
   wxDELETE(m_docManager);
   wxTheClipboard->Flush(); 
   return wxApp::OnExit();
}

//   EVT_ACTIVATE(CMDIChild::OnActivate)

wxDBFAppFrame* wxDBFApp::GetMainFrame(void)
{
   wxASSERT(wxIS_KIND_OF(GetTopWindow(), wxDBFAppFrame));
   return (wxDBFAppFrame*)GetTopWindow();
}

