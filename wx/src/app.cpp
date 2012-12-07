// app.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include <wx/aboutdlg.h>
#include <wx/cmdline.h>

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "app.h"
#include "appframe.h"
#include "dbfres.h"
#include "dbfdefs.h"

IMPLEMENT_APP(App)

BEGIN_EVENT_TABLE(App, wxAppEx)
   EVT_MENU(wxID_ABOUT, App::OnMenuAbout)
END_EVENT_TABLE()

App::App(void) : wxAppEx(), m_mru(NULL), m_res(new DBFResource())
{
}

App::~App(void)
{
    delete m_res;
}

bool App::OnInit(void)
{
    //CheckBuildOptions(WX_BUILD_OPTIONS_SIGNATURE, "program");
    bool ok = base::OnInit();

    if (ok)
    {
        m_locale.Init();

        SetVendorName(WXDBF_APP_VENDOR);
        SetAppName(WXDBF_APP_NAME);

        wxFileSystem::AddHandler(new wxZipFSHandler());
        wxXmlResourceHelper::Init();

        ok = m_res->Init();
        if (ok)
        {
            MainFrame* frame = new MainFrame();
            wxDocManager* docManager = CreateDocManager();

            ok = frame->Create(docManager, GetAppName());
            if (ok)
            {
                frame->Show();
                m_mru->Load();

                if (!m_cmdline.m_fileNames.empty())
                {
                    // get filenames from the commandline
                    for (wxArrayFileName::const_iterator it = m_cmdline.m_fileNames.begin(); it != m_cmdline.m_fileNames.end(); it++)
                    {
                        const wxFileName& fileName = *it;
                        wxDocument* doc = docManager->CreateDocument(fileName.GetFullPath(), wxDOC_SILENT);

                        if (doc == NULL)
                        {
                            docManager->OnOpenFileFailure();
                            wxLogError(wxString::Format(_("Failed to open %s"), fileName.GetFullPath().wx_str()));
                        }
                    }
                }
                else
                {
                    wxFileName fileName;

                    if (m_mru->GetFile(0, &fileName) && fileName.FileExists())
                    {
                        docManager->CreateDocument(fileName.GetFullPath(), wxDOC_SILENT);
                    }
                }
            }
            else
            {
                delete frame;
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
    { wxCMD_LINE_PARAM,  wxT_2(""),  wxT_2(""), wxT_2("input filename(s)"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL|wxCMD_LINE_PARAM_MULTIPLE },
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
      m_cmdline.m_fileNames.push_back(wxFileName(parser.GetParam(i)));
   }
   return base::OnCmdLineParsed(parser);
}
