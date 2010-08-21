// dbfframe.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "app.h"
#include "wxext.h"

DBFFrame::DBFFrame(wxDocument* doc, wxMDIParentFrame* parent)
   : wxDocMDIChildFrame(doc, doc->GetFirstView(), parent, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE)

{
#ifdef __WXMSW__
   SetIcon(wxIcon(wxT("dbf")));
#endif
   wxMenuBar* menu = wxXmlResource::Get()->LoadMenuBar(wxT("menu_dbf"));
   ::wxMenuBar_Fixup(menu, wxGetApp().GetAccelerator());
   SetMenuBar(menu);
   wxGetApp().GetRecentFileList()->Attach(this);
}

BEGIN_EVENT_TABLE(DBFFrame, wxDocMDIChildFrame)
END_EVENT_TABLE()

DBFFrame::~DBFFrame()
{
   wxGetApp().GetRecentFileList()->Detach(this);
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
