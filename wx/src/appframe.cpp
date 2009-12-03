// appframe.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include "appframe.h"
#include "wxext.h"
#include "app.h"
#include <wx/aboutdlg.h>
#include "wx29.h"

IMPLEMENT_CLASS(MainFrame, wxDocMDIParentFrame)

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
   EVT_MENU(XRCID("statusbar")      , MainFrame::OnStatusBar)
   EVT_MENU(XRCID("toolbar")        , MainFrame::OnToolBar)
   EVT_MENU(wxID_ABOUT              , MainFrame::OnAbout)
   EVT_MENU(XRCID("view_fullscreen"), MainFrame::OnFullscreen)
	EVT_UPDATE_UI(XRCID("view_fullscreen"), MainFrame::OnUpdateFullscreen)
	EVT_UPDATE_UI(XRCID("toolbar")   , MainFrame::OnUpdateToolBar)
   EVT_UPDATE_UI(XRCID("statusbar") , MainFrame::OnUpdateStatusBar)
// EVT_UPDATE_UI(wxID_HELP          , MainFrame::OnUpdateDisable)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager* manager, wxFrame* frame, const wxString& title,
    const wxPoint& pos, const wxSize& size, long type)
   : wxDocMDIParentFrame(manager, frame, wxID_ANY, title, pos, size, type, wxT("myFrame"))
{
   CreateStatusBar();
   GetStatusBar()->PushStatusText(_("Ready"));
   SetToolBar(CreateToolBar());
#ifdef __WXMSW__
   SetIcon(wxIcon(wxT("app")));
#endif
   wxMenuBar* menu = wxXmlResource::Get()->LoadMenuBar(wxT("menu_mdi"));
   SetMenuBar(menu);
   wxGetApp().GetRecentFileList()->MenuAdd(this);
#ifndef __WXMAC__
   Show(true);
#endif //ndef __WXMAC__
}

wxToolBar* MainFrame::CreateToolBar()
{
   wxToolBar* tb = base::CreateToolBar(wxTB_TEXT | wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_FLAT);

   static const wxTOOLBARITEM aID[] =
   {
      { wxID_NEW           , NULL               , NULL         , NULL      , wxACCEL_CTRL, 'N' },
      { wxID_OPEN          , NULL               , NULL         , NULL      , wxACCEL_CTRL, 'O' },
      { wxID_SAVE          , NULL               , NULL         , NULL      , wxACCEL_CTRL, 'S' },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
      { wxID_ADD           , NULL               , NULL         , NULL      , wxACCEL_NORMAL, WXK_NUMPAD_ADD },
      { XRCID("edit")      , wxART_HELP_SETTINGS, _("Edit")  , _("Edit"), wxACCEL_NORMAL, WXK_F2 },
      { wxID_DELETE        , NULL               , NULL         , NULL      , wxACCEL_NORMAL, WXK_DELETE },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
      { wxID_PRINT         , NULL               , NULL         , NULL      , wxACCEL_CTRL, 'P' },
      { wxID_PREVIEW       , NULL               , _("Preview"), NULL     , wxACCEL_CTRL | wxACCEL_SHIFT, 'P' },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
      { XRCID("struct")    , wxART_HELP_SIDE_PANEL, _("Structure"), _("Modify Structure"), wxACCEL_CTRL, 'M' },
      { XRCID("pack")      , wxART_DEL_BOOKMARK , _("Pack")  , _("Pack Database"), wxACCEL_CTRL, 'D' },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
      { wxID_FIND          , NULL               , NULL         , NULL      , wxACCEL_CTRL, 'F'},
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
      { wxID_ABOUT /*wxID_HELP*/, NULL          , _("Help")    , NULL      , wxACCEL_NORMAL, WXK_F1 },
      { wxID_EXIT          , NULL               , NULL         , NULL      , wxACCEL_ALT   , WXK_F4  },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      , 0, 0                  },
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
         #ifdef __WXMSW__
            const wxSize size = wxToolBar().GetToolBitmapSize();
         #else
            const wxSize size = tb->GetToolBitmapSize();
         #endif
            const wxArtID art_id = element.id_art ? element.id_art : wxID2ArtID(element.id).wx_str();
            wxBitmap icon = wxArtProviderEx::GetBitmap(art_id, wxART_OTHER, size);
            wxString str = element.text ? element.text : wxGetStockLabelEx(element.id, false).wx_str();
            tool = tb->AddTool(element.id, str, icon);
            if (tool)
            {
               str = element.help ? element.help : wxGetStockLabelEx(element.id, false).wx_str();
               if (element.keyCode) str+=wxString::Format(wxT(" (%s)"), wxGetAccelText(element.flags, element.keyCode).wx_str());
               tool->SetShortHelp(str);
            }
            break;
         }
      }
   }
   tb->Realize();
   return tb;
}

void MainFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
   wxAboutDialogInfo info;
   info.SetDescription(wxString(wxT("\nThis demo program demonstrates the usage of wxDBase\n\n")) + wxVERSION_STRING);
   info.SetCopyright(wxT("Copyright (c) 2007-2009 Troels K"));
   info.AddDeveloper(wxT("Troels K"));
   info.SetWebSite(wxT("http://sf.net/projects/dbase"));
   info.SetLicense(wxT("wxWindows"));
   wxAboutBox(info, this);
}

void MainFrame::OnStatusBar(wxCommandEvent&)
{
   ::wxWindow_Toggle(GetStatusBar(), true);
   SendSizeEvent();
}

void MainFrame::OnUpdateStatusBar(wxUpdateUIEvent& event)
{
   event.Check(GetStatusBar()->IsShown());
}

void MainFrame::OnToolBar(wxCommandEvent&)
{
   ::wxWindow_Toggle(GetToolBar(), true);
   SendSizeEvent();
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
