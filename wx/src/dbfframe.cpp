// dbfframe.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "wxext.h"
#include "app.h"

DBFFrame::DBFFrame() : wxDocMDIChildFrame()
{
}

DBFFrame::~DBFFrame()
{
   wxGetApp().GetRecentFileList()->Detach(this);
}

BEGIN_EVENT_TABLE(DBFFrame, wxDocMDIChildFrame)
END_EVENT_TABLE()

bool DBFFrame::Create(wxDocument* doc, wxMDIParentFrame* parent)
{
   bool ok = base::Create(doc, doc->GetFirstView(), parent, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE);
   if (ok)
   {
       SetIcon(wxICON(dbf));
       SetMenuBar(CreateMenuBar());
   }
   return ok;
}

wxMenuBar* DBFFrame::CreateMenuBar() const
{
   wxMenuBar* menubar = wxXmlResource::Get()->LoadMenuBar(wxT("menu_dbf"));
   
   wxAcceleratorHelper::SetAccelText(menubar, wxGetApp().GetAccelerator());
   return menubar;
}

void DBFFrame::SetMenuBar(wxMenuBar* menubar)
{
    base::SetMenuBar(menubar);
    wxGetApp().GetRecentFileList()->Attach(menubar);
}

#if (wxVERSION_NUMBER < 2900) // trac.wxwidgets.org/ticket/11442
void DBFFrame::DoGiveHelp(const wxString& text, bool show)
{
   wxMDIParentFrame* frame = GetMDIParent();
   frame->DoGiveHelp(text, show);
   //base::DoGiveHelp(text, show);
}
#endif

#if (wxVERSION_NUMBER < 2900) // trac.wxwidgets.org/ticket/8988
wxMDIParentFrame* DBFFrame::GetMDIParent() const
{
#ifdef __WXMSW__
   return wxStaticCast(base::GetParent(), wxMDIParentFrame);
#elif __WXGTK__
   return wxStaticCast(m_parent->GetParent(), wxMDIParentFrame);
#else
   #error
#endif
}
#endif
