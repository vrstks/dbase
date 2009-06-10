// dbflist.cpp
// Copyright (c) 2007 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include <zlib.h>
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"

#include "datalist.h"
#include "dbflist.h"
#include "wxext.h"
#include "datamodel.h"

BEGIN_EVENT_TABLE(wxDBFListCtrl, wxDataListCtrl)
   EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, wxDBFListCtrl::OnBeginLabelEdit)
   EVT_LIST_END_LABEL_EDIT(wxID_ANY, wxDBFListCtrl::OnEndLabelEdit)
   EVT_COMMAND_LEFT_DCLICK(wxID_ANY, wxDBFListCtrl::OnDblClick)
END_EVENT_TABLE()

class wxFieldEdit : public wxTextCtrl
{
   DECLARE_DYNAMIC_CLASS(wxFieldEdit)
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

IMPLEMENT_DYNAMIC_CLASS(wxFieldEdit, wxTextCtrl)

BEGIN_EVENT_TABLE(wxFieldEdit, wxTextCtrl)
   EVT_MOVE(wxFieldEdit::OnMove)
END_EVENT_TABLE()

void wxFieldEdit::OnMove( wxMoveEvent& event )
{
   // Windows wants us to move to the first column, readjust
   event.Skip();
   Fixate(m_fixate);
}

bool wxDBFListCtrl::Edit(size_t row, int col)
{
   wxRect rect;
   bool ok = ::wxListCtrl_GetItemRect(*this, row, col, &rect);
   if (ok)
   {
      EnsureVisible(row);
      m_column_clicked = col;
      wxFieldEdit* edit = (wxFieldEdit*)EditLabel(row, CLASSINFO(wxFieldEdit));
      ok = (NULL != edit);
      if (ok)
      {
         edit->SetValue(::wxListView_GetItemText(*this, row, col));
         rect.height+=3;
         edit->Fixate(rect);
      }
   }
   return ok;
}

bool wxDBFListCtrl::Edit()
{
   ScrollList(-GetScrollPos(wxHORIZONTAL), 0);
   return Edit((size_t)GetFirstSelected(), 0);
}

void wxDBFListCtrl::OnDblClick(wxCommandEvent& /*event*/)
{
   wxDataModelBase* db = GetModel();
   if (!db->IsEditable()) return;

   if (m_id_edit)
   {
      if (GetFirstSelected() != wxNOT_FOUND) wxPostMenuCommand(GetParent(), m_id_edit);
   }
   else
   {
      wxPoint pt = ::wxGetMousePosition();
      pt = ScreenToClient(pt);
      long col;
      long row = ::wxListView_HitTest(*this, pt, NULL, &col);
      if (row != wxNOT_FOUND)
      {
         Edit((size_t)row, col);
      }
   }
}

bool wxDBFListCtrl::AddNew()
{
   wxDataModelBase* db = GetModel();
   bool ok = db->AddNew();
   if (ok)
   {
      Fill();
      ::wxListCtrl_SelectAll(this, false);
      ::wxListView_SetCurSel(this, GetItemCount()-1);
      Edit();
   }
   return ok;
}

