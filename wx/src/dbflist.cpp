// dbflist.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "datalist.h"
#include "dbflist.h"
#include "datamodel.h"

BEGIN_EVENT_TABLE(DBFListCtrl, DataModelListCtrl)
   EVT_COMMAND_LEFT_DCLICK(wxID_ANY, DBFListCtrl::OnDblClick)
END_EVENT_TABLE()

class FieldEdit : public wxTextCtrl
{
   typedef wxTextCtrl base;
   DECLARE_DYNAMIC_CLASS(FieldEdit)
public:
   wxRect m_fixate;
   void Fixate(const wxRect& rect)
   {
      m_fixate = rect;
      SetSize(rect);
   }
protected:
   void OnMove( wxMoveEvent& event );
   DECLARE_EVENT_TABLE()
};

IMPLEMENT_DYNAMIC_CLASS(FieldEdit, wxTextCtrl)

BEGIN_EVENT_TABLE(FieldEdit, wxTextCtrl)
   EVT_MOVE(FieldEdit::OnMove)
END_EVENT_TABLE()

void FieldEdit::OnMove(wxMoveEvent& event)
{
   // Windows wants us to move to the first column, adjust pos
   event.Skip();
   Fixate(m_fixate);
}

bool DBFListCtrl::Edit(long row, long col)
{
#if USE_DATALISTVIEW
   return false;
#else
   wxRect rect;
   bool ok = GetSubItemRect(row, col, rect);

   if (ok)
   {
      EnsureVisible(row);
      m_column_clicked = col;
      FieldEdit* edit = wxStaticCast(EditLabel(row, wxCLASSINFO(FieldEdit)), FieldEdit);
      ok = (NULL != edit);
      if (ok)
      {
         edit->SetValue(GetItemText(row, col));
         rect.height+=3;
         edit->Fixate(rect);
      }
   }
   return ok;
#endif
}

bool DBFListCtrl::Edit()
{
#if USE_DATALISTVIEW
    return false;
#else
    ScrollList(-GetScrollPos(wxHORIZONTAL), 0);
    return Edit((size_t)GetSelectedRow(), 0);
#endif
}

void DBFListCtrl::OnDblClick(wxCommandEvent& event)
{
#if USE_DATALISTVIEW
    event.Skip();
#else
    wxUnusedVar(event);
    if (!IsEditable()) return;

    if (GetEditWindowID())
    {
        if (HasSelection())
            wxPostMenuCommand(GetParent(), GetEditWindowID());
    }
    else
    {
        wxPoint pt = ScreenToClient(::wxGetMousePosition());
        long col;
        long row = HitTest(pt, NULL, &col);

        if (row != wxNOT_FOUND)
            Edit((size_t)row, col);
    }
#endif
}

bool DBFListCtrl::AddNew()
{
#if USE_DATALISTVIEW
    return false;
#else
    wxDataModelBase* db = GetModel();
    bool ok = db->AddNew();

    if (ok)
    {
        Fill();
        SelectNone();
        SelectRow(GetItemCount()-1);
        Edit();
    }
    return ok;
#endif
}
