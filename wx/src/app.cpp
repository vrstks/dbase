// app.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include <wx/aboutdlg.h>
#include <wx/cmdline.h>

#include "wxext.h"
#include "app.h"
#include "appframe.h"
#include "wx29.h"

IMPLEMENT_APP(App)

BEGIN_EVENT_TABLE(App, wxAppEx)
   EVT_MENU(wxID_ABOUT, App::OnMenuAbout)
END_EVENT_TABLE()

App::App(void) : wxAppEx(), m_mru(NULL)
{
}

bool App::OnInit(void)
{
   bool ok = base::OnInit();
   if (ok)
   {
      m_locale.Init();

      SetVendorName(wxT("Troels K"));
      SetAppName(wxT("wxDBF"));

      wxFileSystem::AddHandler(new wxZipFSHandler());
      ok = ::wxInitXRC();
      if (ok)
      {
         MainFrame* frame = new MainFrame();
         wxDocManager* docManager = CreateDocManager();
         wxDocument* doc;

         frame->Create(docManager, GetAppName());

         m_mru->Load();

         SetTopWindow(frame);

          if (m_cmdline.m_fileNames.GetCount())
          {
             // get filenames from the commandline
             for (size_t i = 0; i < m_cmdline.m_fileNames.GetCount(); i++)
             {
                const wxFileName& filename = m_cmdline.m_fileNames.Item(i);
                
                doc = docManager->CreateDocument(filename.GetFullPath(), wxDOC_SILENT);
                if (doc == NULL)
                {
                    docManager->OnOpenFileFailure();
                    wxMessageBox(wxString::Format(_("Failed to open %s"), filename.GetFullPath().wx_str()), wxMessageBoxCaption);
                }
             }
          }
          else
          {
             wxFileName filename;
             if (m_mru->GetFile(0, &filename) && filename.FileExists())
             {
                docManager->CreateDocument(filename.GetFullPath(), wxDOC_SILENT);
             }
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
      array.Add(wxGetStockAcceleratorEx(wxID_ABOUT));

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

void App::OnMenuAbout(wxCommandEvent& WXUNUSED(event))
{
   wxAboutDialogInfo info;
   MainFrame::GetVersionInfo(&info);
   ::wxAboutBox(info, GetTopWindow());
}

CommandLine::CommandLine()
{
}

static const wxCmdLineEntryDesc cmdLineDesc[] =
{
    { wxCMD_LINE_PARAM,  wxT_2(""),  wxT_2(""), _("input filename(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE },
    { wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0 },
};

void App::OnInitCmdLine(wxCmdLineParser& parser)
{
   parser.SetDesc(cmdLineDesc);
   base::OnInitCmdLine(parser);
}

bool App::OnCmdLineParsed(wxCmdLineParser& parser)
{
   for (size_t i = 0; i < parser.GetParamCount(); i++)
   {
      m_cmdline.m_fileNames.Add(wxFileName(parser.GetParam(i)));
   }
   return base::OnCmdLineParsed(parser);
}
