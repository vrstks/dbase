// dbfframe.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "app.h"
#include "wxext.h"

wxDBFFrame::wxDBFFrame(wxDocument* doc, wxView* view, wxMDIParentFrame* parent)
   : wxDocMDIChildFrame(doc, view, parent, wxID_ANY, wxT("Child Frame"),
                             wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE)

{
#ifdef __WXMSW__
   SetIcon(wxIcon(wxT("dbf")));
#endif
   wxMenuBar* menu = wxXmlResource::Get()->LoadMenuBar(wxT("menu_dbf"));
   ::wxMenuBar_Fixup(menu, wxGetApp().GetAccelerator());
   SetMenuBar(menu);
   wxGetApp().GetRecentFileList()->MenuAdd(this);
}

BEGIN_EVENT_TABLE(wxDBFFrame, wxDocMDIChildFrame)
END_EVENT_TABLE()

wxDBFFrame::~wxDBFFrame()
{
   wxGetApp().GetRecentFileList()->MenuRemove(this);
}

