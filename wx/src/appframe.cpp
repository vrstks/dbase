// appframe.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include <wx/stdpaths.h>
#include <wx/aboutdlg.h>

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "appframe.h"
#include "app.h"
#include "dbfdlgs.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf_wx.h"
#include "dbfframe.h"

/////////////////////////////////////////////////////////////////////////////
// MainFrame

IMPLEMENT_CLASS(MainFrame, wxDocMDIParentFrame)

MainFrame::MainFrame() : wxDocMDIParentFrame(), m_windowMenuEvtHandler(NULL)
{
}

MainFrame::~MainFrame()
{
    delete m_windowMenuEvtHandler;
}

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
   EVT_MENU(XRCID("statusbar")      , MainFrame::OnStatusBar)
   EVT_MENU(XRCID("toolbar")        , MainFrame::OnToolBar)
   EVT_MENU(XRCID("view_fullscreen"), MainFrame::OnFullscreen)
   EVT_MENU(XRCID("windows")        , MainFrame::OnWindowsDialog)
   EVT_UPDATE_UI(XRCID("view_fullscreen"), MainFrame::OnUpdateFullscreen)
   EVT_UPDATE_UI(XRCID("toolbar")   , MainFrame::OnUpdateToolBar)
   EVT_UPDATE_UI(XRCID("statusbar") , MainFrame::OnUpdateStatusBar)
// EVT_UPDATE_UI(wxID_HELP          , MainFrame::OnUpdateDisable)
END_EVENT_TABLE()

bool MainFrame::Create(wxDocManager* manager, const wxString& title)
{
    bool ok = base::Create(manager, NULL, wxID_ANY, title);

    if (ok)
    {
        CreateStatusBar()->PushStatusText(_("Ready"));
        SetToolBar(CreateToolBar());
        SetIcon(wxICON(app));
        SetMenuBar(CreateMenuBar());
        wxAcceleratorHelper::SetAcceleratorTable(this, DBFFrame::GetAccelerator());
        ::wxFrame_SetInitialPosition(this);
        m_windowMenuEvtHandler = new MDIWindowMenuEvtHandler(this);

        wxMenu* windowMenu = GetWindowMenu();
        windowMenu->AppendSeparator();
        windowMenu->Append(XRCID("windows"), _("&Windows..."), _("Windows dialog"));

    #ifdef x_DEBUG
        wxPostMenuCommand(this, XRCID("windows"));
        //wxPostMenuCommand(this, XRCID("struct"));
    #endif
    }
    return ok;
}

wxMenuBar* MainFrame::CreateMenuBar() const
{
   wxMenuBar* menubar = wxXmlResource::Get()->LoadMenuBar(wxT("menu_mdi"));

   wxAcceleratorHelper::SetAccelText(menubar, DBFFrame::GetAccelerator());
   return menubar;
}

void MainFrame::SetMenuBar(wxMenuBar* menubar)
{
    base::SetMenuBar(menubar);
    wxGetApp().GetRecentFileList()->Attach(menubar);
}

typedef struct _wxTOOLBARITEM
{
   int id;
#if (wxVERSION_NUMBER >= 2902)
   const char* id_art;
#else
   const wxChar* id_art;
#endif
   wxString text;
   wxString help;
} wxTOOLBARITEM;

wxToolBar* MainFrame::CreateToolBar()
{
   const wxArrayAccelerator& accel = DBFFrame::GetAccelerator();
   wxToolBar* tb = base::CreateToolBar(wxTB_TEXT | wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_FLAT);
   const wxSize size = tb->GetToolBitmapSize();

   const wxTOOLBARITEM aID[] =
   {
      { wxID_NEW           , NULL               , wxEmptyString, wxEmptyString },
      { wxID_OPEN          , NULL               , wxEmptyString, wxEmptyString },
      { wxID_SAVE          , NULL               , wxEmptyString, wxEmptyString },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
      { wxID_ADD           , NULL               , wxEmptyString, wxEmptyString },
      { XRCID("edit")      , wxART_HELP_SETTINGS, _("Edit")    , _("Edit") },
      { wxID_DELETE        , NULL               , wxEmptyString, wxEmptyString },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
      { wxID_PRINT         , NULL               , wxEmptyString, wxEmptyString },
      { wxID_PREVIEW       , NULL               , _("Preview") , wxEmptyString },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
      { XRCID("struct")    , wxART_HELP_SIDE_PANEL, _("Structure"), _("Modify Structure") },
      { XRCID("pack")      , wxART_DEL_BOOKMARK , _("Pack")  , _("Pack Database") },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
      { wxID_FIND          , NULL               , wxEmptyString, wxEmptyString },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
      { wxID_ABOUT /*wxID_HELP*/, NULL          , _("Help")    , wxEmptyString },
      { wxID_EXIT          , NULL               , wxEmptyString, wxEmptyString },
      { wxID_SEPARATOR     , NULL               , wxEmptyString, wxEmptyString },
   };

   for (size_t i = 0; i < WXSIZEOF(aID); i++)
   {
      const wxTOOLBARITEM& element = aID[i];
      wxToolBarToolBase* tool = NULL;

      switch (element.id)
      {
         case wxID_SEPARATOR:
            tb->AddSeparator();
            break;
         default:
         {
            const wxArtID art_id = element.id_art ? wxString(element.id_art) : wxID2ArtID(element.id);
            wxBitmap icon = wxArtProvider::GetBitmap(art_id, wxART_OTHER, size);
            wxString str = element.text.empty() ? wxGetStockLabelEx(element.id, wxSTOCK_PLAINTEXT) : element.text;

            tool = tb->AddTool(element.id, str, icon);
            if (tool)
            {
               str = element.help.empty() ? wxGetStockLabelEx(element.id, wxSTOCK_PLAINTEXT) : element.help;
               str = wxToolBarTool_MakeShortHelp(str, accel, element.id);
               tool->SetShortHelp(str);
            }
            break;
         }
      }
   }
   tb->Realize();
   return tb;
}

/*static*/ void MainFrame::GetVersionInfo(wxAboutDialogInfo* info)
{
   wxVersionInfo vi = wxDBase::GetVersionInfo();

   info->SetName(vi.GetName());
   //info->SetDescription(vi.GetDescription() + wxT("\n\n") + wxStandardPaths::Get().GetExecutablePath());
   info->SetDescription(wxT("\nThis demo program demonstrates the usage of wxDBase\n\n")
       + wxStandardPaths::Get().GetExecutablePath());

   info->SetCopyright(vi.GetCopyright());
   info->SetVersion(wxString::Format(wxT("%d.%d svn r%d"), vi.GetMajor(), vi.GetMinor(), vi.GetMicro()));
   info->AddDeveloper(wxT(DBF_AUTHOR));
   info->SetLicense(wxT("wxWindows"));
   info->SetWebSite(wxT(DBF_WEBSITE));
}

#ifdef __WXMSW__
bool MainFrame::MSWTranslateMessage(WXMSG* msg)
{
    bool processed = false;

#if (wxVERSION_NUMBER < 2900)
    if ( (!processed) && (msg->message == WM_COMMAND))
    {
        switch (msg->wParam)
        {
            //case wxID_CLOSE:
            case wxID_CLOSE_ALL:
                wxPostMenuCommand(m_windowMenuEvtHandler, msg->wParam);
                processed = true;
                break;
            default:
                if ((int)msg->wParam == XRCID("windows"))
                {
                    wxPostMenuCommand(m_windowMenuEvtHandler, msg->wParam);
                    processed = true;
                }
                break;
        }
    }
#endif

    if (!processed)
    {
         processed = base::MSWTranslateMessage(msg);
    }
    return processed;
}
#endif

void MainFrame::OnStatusBar(wxCommandEvent&)
{
   ::wxWindow_Toggle(GetStatusBar());
}

void MainFrame::OnUpdateStatusBar(wxUpdateUIEvent& event)
{
   event.Check(GetStatusBar()->IsShown());
}

void MainFrame::OnToolBar(wxCommandEvent&)
{
   ::wxWindow_Toggle(GetToolBar());
}

void MainFrame::OnUpdateToolBar(wxUpdateUIEvent& event)
{
   event.Check(GetToolBar()->IsShown());
}

void MainFrame::OnFullscreen(wxCommandEvent&)
{
   long style = wxFULLSCREEN_NOBORDER|wxFULLSCREEN_NOCAPTION;

   ::wxFrame_ToggleFullScreen(this, style);
}

void MainFrame::OnUpdateFullscreen (wxUpdateUIEvent& event)
{
   ::wxFrame_OnUpdateFullScreen(this, event);
}

void MainFrame::OnWindowsDialog(wxCommandEvent&)
{
    ::DoModal_Windows(this, wxDocViewHelpers::GetDocumentsVector(*m_docManager));
}
