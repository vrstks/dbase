// appframe.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include "wxext.h"
#include "appframe.h"
#include "wx/aboutdlg.h"
#include "app.h"
#include "../../bool.h"
#include "../../dbf.h"

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

MainFrame::MainFrame(wxDocManager* manager, const wxString& title,
    const wxPoint& pos, const wxSize& size)
   : wxDocMDIParentFrame(manager, NULL, wxID_ANY, title, pos, size)
{
   CreateStatusBar();
   GetStatusBar()->PushStatusText(_("Ready"));
   SetToolBar(CreateToolBar());
#ifdef __WXMSW__
   SetIcon(wxIcon(wxT("app")));
#endif
   wxMenuBar* menu = wxXmlResource::Get()->LoadMenuBar(wxT("menu_mdi"));
   SetMenuBar(menu);
   wxGetApp().GetRecentFileList()->Attach(this);
   ::wxSetAcceleratorTable(this, wxGetApp().GetAccelerator());
   ::wxMenu_SetAccelText(GetMenuBar(), wxGetApp().GetAccelerator());
   Show();
}

typedef struct _wxTOOLBARITEM
{
   int id;
   const wxChar* id_art;
   const wxChar* text;
   const wxChar* help;
} wxTOOLBARITEM;

wxToolBar* MainFrame::CreateToolBar()
{
   const AcceleratorArray& accel = wxGetApp().GetAccelerator();
   
   wxToolBar* tb = base::CreateToolBar(wxTB_TEXT | wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_FLAT);
   const wxSize size = tb->GetToolBitmapSize();

   const wxTOOLBARITEM aID[] =
   {
      { wxID_NEW           , NULL               , NULL         , NULL      },
      { wxID_OPEN          , NULL               , NULL         , NULL      },
      { wxID_SAVE          , NULL               , NULL         , NULL      },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
      { wxID_ADD           , NULL               , NULL         , NULL      },
      { XRCID("edit")      , wxART_HELP_SETTINGS, _("Edit")    , _("Edit") },
      { wxID_DELETE        , NULL               , NULL         , NULL      },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
      { wxID_PRINT         , NULL               , NULL         , NULL      },
      { wxID_PREVIEW       , NULL               , _("Preview") , NULL      },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
      { XRCID("struct")    , wxART_HELP_SIDE_PANEL, _("Structure"), _("Modify Structure") },
      { XRCID("pack")      , wxART_DEL_BOOKMARK , _("Pack")  , _("Pack Database") },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
      { wxID_FIND          , NULL               , NULL         , NULL      },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
      { wxID_ABOUT /*wxID_HELP*/, NULL          , _("Help")    , NULL      },
      { wxID_EXIT          , NULL               , NULL         , NULL      },
      { wxID_SEPARATOR     , NULL               , NULL         , NULL      },
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
            const wxArtID art_id = element.id_art ? element.id_art : wxID2ArtID(element.id).wx_str();
            wxBitmap icon = wxArtProviderEx::GetBitmap(art_id, wxART_OTHER, size);
            wxString str = element.text ? element.text : wxGetStockLabelEx(element.id, wxSTOCK_PLAINTEXT).wx_str();
            tool = tb->AddTool(element.id, str, icon);
            if (tool)
            {
               str = element.help ? element.help : wxGetStockLabelEx(element.id, wxSTOCK_PLAINTEXT).wx_str();
               int index = wxAcceleratorEntry_Find(accel, element.id);
               if (index != wxNOT_FOUND)
               {
                  str+=wxString::Format(wxT(" (%s)"), wxGetAccelText(accel.Item(index)).wx_str());
               }
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
   info.SetDescription(wxString::Format(wxT("\nThis demo program demonstrates the usage of wxDBase\n\nUsing:\n%s\n%s\n"),
                       wxString(wxConvertMB2WX(::dbf_libversionstring())).wx_str(),
                       wxVERSION_STRING
                       ));
   info.SetCopyright(wxT("Copyright (c) 2007-2010 Troels K"));
   info.AddDeveloper(wxT("Troels K"));
   info.SetWebSite(wxT("http://sf.net/projects/dbase"));
   info.SetLicense(wxT("wxWindows"));
   ::wxAboutBox(info, this);
}

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
