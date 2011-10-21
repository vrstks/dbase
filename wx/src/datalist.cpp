// datalist.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/app.h>
#include <wx/settings.h>
#include <wx/msgdlg.h>

#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"

#include "datalist.h"
#include "wxext.h"
#include "datamodel.h"

//IMPLEMENT_CLASS(wxDataListCtrl, wxListView)

BEGIN_EVENT_TABLE(wxDataListCtrl, wxListView)
   EVT_KEY_DOWN(wxDataListCtrl::OnKeyDown)
   EVT_LIST_ITEM_SELECTED(wxID_ANY, wxDataListCtrl::OnSelChange)
   EVT_LIST_ITEM_ACTIVATED(wxID_ANY, wxDataListCtrl::OnSelect)
END_EVENT_TABLE()

wxDataListCtrl::wxDataListCtrl(): wxListView()
{
   const wxColour gray = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);

   m_attr[ENUM_attr_deleted].SetTextColour(gray);
   m_column_clicked = -1;
   m_id_edit = m_id_selchange = m_id_delete = 0;
}

void wxDataListCtrl::OnSelChange(wxListEvent& event)
{
   if (m_id_selchange && (GetFirstSelected() != wxNOT_FOUND))
   {
      wxPostMenuCommand(GetParent(), m_id_selchange);
   }
   event.Skip();
}

void wxDataListCtrl::OnSelect(wxListEvent&)
{
   if (m_id_edit)
   {
      if (GetFirstSelected() != wxNOT_FOUND)
      {
         wxPostMenuCommand(GetParent(), m_id_edit);
      }
   }
}

void wxDataListCtrl::OnKeyDown(wxKeyEvent& event)
{
   switch (event.GetKeyCode())
   {
      case WXK_DELETE:
         if (m_id_delete)
         {
            if (GetFirstSelected() != wxNOT_FOUND) wxPostMenuCommand(GetParent(), m_id_delete);
         }
         else event.Skip();
         break;
      default:
         event.Skip();
         break;
   }
}

void wxDataListCtrl::Init()
{
   for (unsigned int i = 0, count = GetModel()->GetColumnCount(); i < count; i++)
   {
      int format = wxLIST_FORMAT_LEFT;
      wxDataModel::ColumnInfo info;

      GetModel()->GetColumn(i, &info);
      if (   (wxT("long"  ) == info.type)
          || (wxT("double") == info.type) )
      {
         format = wxLIST_FORMAT_RIGHT;
      }
/*
      else if (   (wxT("datetime") == type)
               || (wxT("bool"    ) == type) )
      {
         format = wxLIST_FORMAT_CENTER;
      }
*/
      InsertColumn(i, info.name, format, 150);
      //InsertColumn(i, wxString::Format(wxT("%d"), i), wxLIST_FORMAT_LEFT, 150);
   }
   Fill();
}

void wxDataListCtrl::Fill()
{
   SetItemCount(GetModel()->GetRowCount());
   Refresh();
}

wxString wxDataListCtrl::OnGetItemText(long item, long col) const
{
   wxDataListCtrl* pThis = wxStaticCast(this, wxDataListCtrl);
   wxDataModelBase* db = pThis->GetModel(); // unconst
   wxString str;

   db->GetValueByRow(&str, item, col);
   return str;
}

wxListItemAttr* wxDataListCtrl::OnGetItemAttr(long item) const
{
   wxDataModelBase* db = wxStaticCast(this, wxDataListCtrl)->GetModel(); // unconst
   const bool bDeleted = db->IsRowDeleted(item);
   const enum attr attr = bDeleted ? ENUM_attr_deleted : ENUM_attr_none;

   return (attr != ENUM_attr_none) ? (wxListItemAttr*)(m_attr + attr) : base::OnGetItemAttr(item);
}

bool wxDataListCtrl::IsRecordOk(unsigned int index)
{ 
   return index < GetModel()->GetRowCount();
}

bool wxDataListCtrl::IsRecordDeleted(unsigned int index)
{
   return GetModel()->IsRowDeleted(index);
}

bool wxDataListCtrl::DeleteRecord(unsigned int index, bool bDelete)
{
   return GetModel()->DeleteRow(index, bDelete);
}

bool wxDataListCtrl::IsUndeletedInSelection(void)
{
   bool ok = false;

   for (long index = GetFirstSelected(); (!ok) && (index != wxNOT_FOUND); index = GetNextSelected(index))
   {
      ok = ok || (IsRecordOk(index) && !IsRecordDeleted(index));
   }
   return ok;
}

bool wxDataListCtrl::IsDeletedInSelection(void)
{
   bool ok = false;

   for (long index = GetFirstSelected(); (!ok) && (index != wxNOT_FOUND); index = GetNextSelected(index))
   {
      ok = ok || (IsRecordOk(index) && IsRecordDeleted(index));
   }
   return ok;
}

bool wxDataListCtrl::IsAnyUnselected(void)
{
   bool ok = false;

   for (unsigned int index = 0; (!ok) && (index < (unsigned int)GetItemCount()); index++)
   {
      ok = ok || !IsSelected(index);
   }
   return ok;
}

void wxDataListCtrl::DeleteSelection(bool bDelete)
{
   for (long index = GetFirstSelected(); index != wxNOT_FOUND; index = GetNextSelected(index))
   {
      DeleteRecord(index, bDelete);
   }
   Fill();
}

void wxDataListCtrl::DeleteAll(bool bDelete)
{
   for (unsigned int index = 0; index < (unsigned int)GetItemCount(); index++)
   {
      DeleteRecord(index, bDelete);
   }
   Fill();
}

void wxDataListCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsAnyUnselected());
}

void wxDataListCtrl::OnUpdateNeedSel_Deleted(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsDeletedInSelection());
}

void wxDataListCtrl::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsUndeletedInSelection());
}

void wxDataListCtrl::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   const int index = GetFirstSelected();

   event.Enable(GetModel() && GetModel()->IsOpen() && (index != wxNOT_FOUND));
}

bool wxDataListCtrl::CanEditLabel(void)
{
   return (m_column_clicked != -1) && GetModel()->IsEditable();
}

void wxDataListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
   if (CanEditLabel())
   {
      event.Skip(); // does nothing :(
   }
   else
   {
      ::wxListCtrl_EndEditLabel(this, true);
   }
}

void wxDataListCtrl::OnEndLabelEdit(wxListEvent& event)
{
   if (!event.IsEditCancelled())
   {
      wxDataModelBase* db = GetModel();
      const wxString str = event.GetLabel();
      const int pos = GetFirstSelected();
      bool ok = (pos != wxNOT_FOUND);

      if (ok)
      {
         wxVariant var;
         wxDataModel::ColumnInfo info;

         GetModel()->GetColumn(m_column_clicked, &info);
         if (info.type == wxT("datetime"))
         {
            wxDateTime date;

            ok = (NULL != date.ParseFormat(str, wxT("%x")));
            if (ok)
            {
               wxASSERT(date.IsValid());
               var = date;
            }
         }
         else
         {
            var = str;
         }
         if (ok) ok = db->SetValueByRow(var, pos, m_column_clicked);
      }
      if (!ok) wxMessageBox(_("Failed"));
   }
   m_column_clicked = -1;
}

bool wxDataListCtrl::IsOpen() const
{
   const wxDataModelBase* db = wxStaticCast(this, wxDataListCtrl)->GetModel(); // unconst

   return db && db->IsOpen();
}
