// app.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "appframe.h"

IMPLEMENT_APP(App)

BEGIN_EVENT_TABLE(App, wxApp)
END_EVENT_TABLE()

App::App(void) : wxApp(), m_docManager(NULL)
{
}

void App::FileHistoryUseMenu(wxMenu* file_menu)
{
   if(0)if (file_menu == NULL)
   {
      file_menu = GetMainFrame()->m_file_menu;
   }
   //m_docManager->FileHistoryUseMenu(file_menu);
   //m_docManager->FileHistoryAddFilesToMenu();
}

void App::FileHistoryRemoveMenu(wxMenu* file_menu)
{
   m_docManager->FileHistoryRemoveMenu(file_menu);
}

bool App::OnInit(void)
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

         MainFrame* frame = new MainFrame(m_docManager, NULL, GetAppName(), wxDefaultPosition, wxSize(640,480),
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

void App::FileHistoryLoadSave(bool save)
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

int App::OnExit(void)
{
   FileHistoryLoadSave(true);
   wxDELETE(m_docManager);
   wxTheClipboard->Flush(); 
   return wxApp::OnExit();
}

MainFrame* App::GetMainFrame(void)
{
   return wxStaticCast(GetTopWindow(), MainFrame);
}

