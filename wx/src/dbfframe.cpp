// dbfframe.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "app.h"
#include "wxext.h"

wxDBFFrame::wxDBFFrame(wxDocument* doc, wxView* view, wxMDIParentFrame* parent)
   : wxDocMDIChildFrame(doc, view, parent, wxID_ANY, wxEmptyString,
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

BEGIN_EVENT_TABLE(wxDBFFrame, wxDocMDIChildFrame)
END_EVENT_TABLE()

wxDBFFrame::~wxDBFFrame()
{
   wxGetApp().GetRecentFileList()->Detach();
}

#if (wxVERSION_NUMBER < 2900)
void wxDBFFrame::DoGiveHelp(const wxString& text, bool show)
{
#ifdef __WXMSW__
   wxMDIParentFrame* frame = wxStaticCast(GetParent(), wxMDIParentFrame);
   frame->DoGiveHelp(text, show);
#elif __WXGTK__
   wxMDIParentFrame* frame = wxStaticCast(m_parent->GetParent(), wxMDIParentFrame);
   frame->DoGiveHelp(text, show); // this works, MSW+GTK
#else
   base::DoGiveHelp(text, show);
#endif
}
#endif

