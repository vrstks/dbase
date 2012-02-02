// dbfframe.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "dbfdoc.h"
#include "wxext.h"

IMPLEMENT_DYNAMIC_CLASS(DBFFrame, wxDocMDIChildFrame)

DBFFrame::DBFFrame() : wxDocMDIChildFrame(), m_mru(NULL)
{
}

DBFFrame::~DBFFrame()
{
   if (m_mru) m_mru->Detach(this);
}

BEGIN_EVENT_TABLE(DBFFrame, wxDocMDIChildFrame)
END_EVENT_TABLE()

bool DBFFrame::Create(wxDocument* doc, wxMDIParentFrame* parent, wxRecentFileList* mru)
{
   bool ok = base::Create(doc, doc->GetFirstView(), parent, wxID_ANY, wxEmptyString,
                             wxDefaultPosition, wxDefaultSize,
                             wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE);
   if (ok)
   {
       m_mru = mru;
       SetMenuBar(CreateMenuBar());
   }
   return ok;
}

wxMenuBar* DBFFrame::CreateMenuBar() const
{
   wxMenuBar* menubar = wxXmlResource::Get()->LoadMenuBar(wxT("menu_dbf"));
   
   wxAcceleratorHelper::SetAccelText(menubar, GetAccelerator());
   return menubar;
}
/*static*/ const AcceleratorArray& DBFFrame::GetAccelerator()
{
   static AcceleratorArray array;

   if (array.IsEmpty())
   {
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_EXIT));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_CUT));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_COPY));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_PASTE));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_HELP));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_NEW));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_OPEN));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_CLOSE));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_SAVE));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_PRINT));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_PREVIEW));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_SELECTALL));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_PROPERTIES));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_FIND));
      array.Add(wxAcceleratorHelper::GetStockAccelerator(wxID_ABOUT));

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


void DBFFrame::SetMenuBar(wxMenuBar* menubar)
{
    base::SetMenuBar(menubar);
    if (m_mru) m_mru->Attach(menubar);
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
