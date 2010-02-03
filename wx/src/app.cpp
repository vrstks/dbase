// app.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "appframe.h"
#include "wxext.h"
#include "wx29.h"

IMPLEMENT_APP(App)

BEGIN_EVENT_TABLE(App, wxAppEx)
END_EVENT_TABLE()

App::App(void) : wxAppEx(), m_mru(NULL)
{
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
         wxDocManager* docManager = CreateDocManager();

         MainFrame* frame = new MainFrame(docManager, NULL, GetAppName(), wxDefaultPosition, wxSize(700,610),
                            wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

         m_mru->Load();

         SetTopWindow(frame);
         ::wxSetAcceleratorTable(frame, GetAccelerator());
         ::wxMenuBar_Fixup(frame->GetMenuBar(), GetAccelerator());

         wxFileName filename;
         if (m_mru->GetFile(0, &filename) && filename.FileExists())
         {
            docManager->CreateDocument(filename.GetFullPath(), wxDOC_SILENT);
         }
      }
   }
   return ok;
}

int App::OnExit(void)
{
   if (m_mru)
   {
      m_mru->Save();
      wxDELETE(m_mru);
   }
   delete wxDocManager::GetDocumentManager();
   wxTheClipboard->Flush(); 
   return base::OnExit();
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

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_HELP, wxID_ABOUT));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_FULLSCREEN, XRCID("view_fullscreen")));

      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_INSERT, wxID_ADD));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_DELETE, wxID_DELETE));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F2, XRCID("edit")));
      array.Add(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F3, XRCID("find_next")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'H', wxID_REPLACE));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'X', XRCID("export")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'D', XRCID("pack")));
      array.Add(wxAcceleratorEntry(wxACCEL_CTRL, 'M', XRCID("struct")));

      array.Add(wxAcceleratorEntry(wxACCEL_CMD | wxACCEL_SHIFT, 'S', wxID_SAVEAS));
   }
   return array;
}

