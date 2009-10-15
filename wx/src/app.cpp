// app.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "appframe.h"
#include "wxext.h"
#include "wx29.h"

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

      wxFileSystem::AddHandler(new wxZipFSHandler);
      ok = ::wxInitXRC();
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
         ::wxSetAcceleratorTable(frame, GetAccelerator());
         ::wxMenuBar_Fixup(frame->GetMenuBar(), GetAccelerator());

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

/*static*/ const AcceleratorArray& App::GetAccelerator()
{
   static AcceleratorArray array;
   if (array.IsEmpty())
   {
      array.Add(wxGetStockAcceleratorEx(wxID_EXIT));
      array.Add(wxGetStockAcceleratorEx(wxID_CUT));
      array.Add(wxGetStockAcceleratorEx(wxID_COPY));
      array.Add(wxGetStockAcceleratorEx(wxID_PASTE));
      array.Add(wxGetStockAcceleratorEx(wxID_HELP));
      array.Add(wxGetStockAcceleratorEx(wxID_NEW));
      array.Add(wxGetStockAcceleratorEx(wxID_OPEN));
      array.Add(wxGetStockAcceleratorEx(wxID_CLOSE));
      array.Add(wxGetStockAcceleratorEx(wxID_SAVE));
      array.Add(wxGetStockAcceleratorEx(wxID_PRINT));
      array.Add(wxGetStockAcceleratorEx(wxID_PREVIEW));
      array.Add(wxGetStockAcceleratorEx(wxID_SELECTALL));
      array.Add(wxGetStockAcceleratorEx(wxID_PROPERTIES));
      array.Add(wxGetStockAcceleratorEx(wxID_FIND));
      array.Add(wxGetStockAcceleratorEx(wxID_SELECTALL));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F1, wxID_ABOUT));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_FULLSCREEN, XRCID("view_fullscreen")));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_INSERT, wxID_ADD));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_DELETE, wxID_DELETE));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F2, XRCID("edit")));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F3, XRCID("next")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'H', wxID_REPLACE));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'X', XRCID("export")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'D', XRCID("pack")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'M', XRCID("struct")));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F12, wxID_SAVEAS));
   }
   return array;
}

