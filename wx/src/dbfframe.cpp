// dbfframe.cpp
// Copyright (c) 2007 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "app.h"

wxDBFFrame::wxDBFFrame(wxDocument* doc, wxView* view, wxMDIParentFrame* parent)
   : wxDocMDIChildFrame(doc, view, parent, wxID_ANY, wxT("Child Frame"),
                             wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE)

{
#ifdef __WXMSW__
   SetIcon(wxString(wxT("dbf")));
#endif
   wxMenuBar* menu = wxXmlResource::Get()->LoadMenuBar(wxT("menu_dbf"));
   SetMenuBar(menu);
   m_file_menu = menu->GetMenu(0);
   wxGetApp().FileHistoryUseMenu(m_file_menu);
}

BEGIN_EVENT_TABLE(wxDBFFrame, wxDocMDIChildFrame)
    EVT_ACTIVATE(wxDBFFrame::OnActivate)
END_EVENT_TABLE()

wxDBFFrame::~wxDBFFrame()
{
   wxGetApp().FileHistoryRemoveMenu(m_file_menu);
}

#ifdef __WXMSW__
void wxDBFFrame::InternalSetMenuBar()
{
   wxDocMDIChildFrame::InternalSetMenuBar();
}
#endif

void wxDBFFrame::OnActivate(wxActivateEvent& event)
{
    //if (event.GetActive() && canvas)
    //    canvas->SetFocus();
   if (event.GetActive())
   {
   }
   else
   {
      wxGetApp().FileHistoryRemoveMenu(m_file_menu);
   }
   event.Skip();
}

