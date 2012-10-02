// dbfframe.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfframe.h"
#include "dbfdoc.h"
#include "wx/ext/wx.h"

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
   wxRect rect = parent->GetClientWindow()->GetClientRect();

   bool ok = base::Create(doc, doc->GetFirstView(), parent, wxID_ANY, wxEmptyString, rect.GetPosition(), rect.GetSize(),
       wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

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
/*static*/ const wxArrayAccelerator& DBFFrame::GetAccelerator()
{
   static wxArrayAccelerator array;

   if (array.empty())
   {
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_EXIT));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_CUT));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_COPY));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_PASTE));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_HELP));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_NEW));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_OPEN));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_CLOSE));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_SAVE));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_PRINT));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_PREVIEW));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_SELECTALL));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_PROPERTIES));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_FIND));
      array.push_back(wxAcceleratorHelper::GetStockAccelerator(wxID_ABOUT));

      array.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_FULLSCREEN, XRCID("view_fullscreen")));

      array.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_INSERT, wxID_ADD));
      array.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_DELETE, wxID_DELETE));
      array.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F2, XRCID("edit")));
      array.push_back(wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F3, XRCID("find_next")));
      array.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'H', wxID_REPLACE));
      array.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'X', XRCID("export")));
      array.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'D', XRCID("pack")));
      array.push_back(wxAcceleratorEntry(wxACCEL_CTRL, 'M', XRCID("struct")));

      array.push_back(wxAcceleratorEntry(wxACCEL_CMD | wxACCEL_SHIFT, 'S', wxID_SAVEAS));
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

void DBFFrame::Raise()
{
#if (wxVERSION_NUMBER >= 2904)
    base::Raise();
#else
    if (IsIconized())
    {
        Restore();
    }
    Activate();
#endif
}
