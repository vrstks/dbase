// datalist.cpp
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

#include "wx/ext/wx.h"
#include "datalist.h"
#include "datamodel.h"

#if USE_DATALISTVIEW
IMPLEMENT_CLASS(DataModelListCtrl, wxDataViewListCtrl)
#else
IMPLEMENT_CLASS(DataModelListCtrl, wxTrunkListView)
#endif


#if USE_DATALISTVIEW
BEGIN_EVENT_TABLE(DataModelListCtrl, wxDataViewListCtrl)
   EVT_KEY_DOWN(DataModelListCtrl::OnKeyDown)
END_EVENT_TABLE()
#else
BEGIN_EVENT_TABLE(DataModelListCtrl, wxTrunkListView)
   EVT_KEY_DOWN(DataModelListCtrl::OnKeyDown)
   EVT_LIST_ITEM_SELECTED(wxID_ANY, DataModelListCtrl::OnSelectionChanged)
   EVT_LIST_ITEM_ACTIVATED(wxID_ANY, DataModelListCtrl::OnItemActivated)
   EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, DataModelListCtrl::OnBeginLabelEdit)
   EVT_LIST_END_LABEL_EDIT(wxID_ANY, DataModelListCtrl::OnEndLabelEdit)
END_EVENT_TABLE()
#endif

DataModelListCtrl::DataModelListCtrl()
{
    m_model = NULL;
    m_column_clicked = wxNOT_FOUND;
    m_id_edit = m_id_selchange = m_id_delete = 0;
}

DataModelListCtrl::~DataModelListCtrl()
{
}

bool DataModelListCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style,
           const wxValidator& validator,
           const wxString& name)
{
#if USE_DATALISTVIEW
    style = wxDV_ROW_LINES;
    bool ok = base::Create(parent, id, pos, size, style);
    if (ok)
    {
        SetValidator(validator);
    }
#else
    bool ok = base::Create(parent, id, pos, size, style, validator, name);
    if (ok)
       EnableAlternateRowColours();
#endif
    return ok;
}

void DataModelListCtrl::OnKeyDown(wxKeyEvent& event)
{
   switch (event.GetKeyCode())
   {
      case WXK_DELETE:
         if (m_id_delete)
         {
        #if USE_DATALISTVIEW
        #else
            if (GetFirstSelected() != wxNOT_FOUND)
                wxPostMenuCommand(GetParent(), m_id_delete);
        #endif
         }
         else
             event.Skip();
         break;
      default:
         event.Skip();
         break;
   }
}

void DataModelListCtrl::InitColumns()
{
   wxDataModelBase* model = GetModel();

#if USE_DATALISTVIEW
   //AssociateModel(model->GetSource());
#endif
   for (unsigned int i = 0, count = model->GetColumnCount();
        i < count;
        i++)
   {
      int format = wxLIST_FORMAT_LEFT;
      wxDataModel::ColumnInfo info;

      model->GetColumn(i, &info);
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
    #if USE_DATALISTVIEW
        AppendTextColumn(info.name);
    #else
        InsertColumn(i, info.name, format, 150);
    #endif
      //InsertColumn(i, wxString::Format(wxT("%d"), i), wxLIST_FORMAT_LEFT, 150);
   }
   Fill();
}

void DataModelListCtrl::Fill()
{
#if USE_DATALISTVIEW
#else
    SetItemCount(GetModel()->GetRowCount());
    Refresh();
#endif
}

wxString DataModelListCtrl::OnGetItemText(long item, long col) const
{
   DataModelListCtrl* pThis = wxStaticCast(this, DataModelListCtrl);
   wxDataModelBase* db = pThis->GetModel(); // unconst
   wxString str;

   db->GetValueByRow(&str, item, col);
   return str;
}

#if !USE_DATALISTVIEW
wxListItemAttr* DataModelListCtrl::OnGetItemAttr(long item) const
{
    wxListItemAttr* attr = base::OnGetItemAttr(item);
    wxDataModelBase* db = wxStaticCast(this, DataModelListCtrl)->GetModel(); // unconst

    if (db->IsRowDeleted(item) && attr)
    {
        const wxColour gray = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
        wxListItemAttr* mine = wxConstCast(&m_attr, wxListItemAttr);

        mine->AssignFrom(*attr);
        mine->SetTextColour(gray);
        attr = mine;
    }
    return attr;
}
#endif

bool DataModelListCtrl::IsRecordOk(unsigned int index)
{ 
   return index < GetModel()->GetRowCount();
}

bool DataModelListCtrl::IsRecordDeleted(unsigned int index)
{
   return GetModel()->IsRowDeleted(index);
}

bool DataModelListCtrl::DeleteRecord(unsigned int index, bool bDelete)
{
   return GetModel()->DeleteRow(index, bDelete);
}

bool DataModelListCtrl::IsUndeletedInSelection(void)
{
    bool ok = false;
#if USE_DATALISTVIEW
#else
    for (long index = GetFirstSelected(); (!ok) && (index != wxNOT_FOUND); index = GetNextSelected(index))
        ok = ok || (IsRecordOk(index) && !IsRecordDeleted(index));
#endif
    return ok;
}

bool DataModelListCtrl::IsDeletedInSelection(void)
{
    bool ok = false;
#if USE_DATALISTVIEW
#else
    for (long index = GetFirstSelected(); (!ok) && (index != wxNOT_FOUND); index = GetNextSelected(index))
        ok = ok || (IsRecordOk(index) && IsRecordDeleted(index));
#endif
    return ok;
}

bool DataModelListCtrl::IsAnyUnselected(void)
{
    bool ok = false;
#if USE_DATALISTVIEW
#else
    for (unsigned int index = 0; (!ok) && (index < (unsigned int)GetItemCount()); index++)
        ok = ok || !IsSelected(index);
#endif
    return ok;
}

void DataModelListCtrl::DeleteSelection(bool bDelete)
{
#if USE_DATALISTVIEW
#else
    for (long index = GetFirstSelected(); index != wxNOT_FOUND; index = GetNextSelected(index))
        DeleteRecord(index, bDelete);
#endif
    Fill();
}

void DataModelListCtrl::DeleteAll(bool bDelete)
{
   for (unsigned int index = 0; index < (unsigned int)GetItemCount(); index++)
   {
      DeleteRecord(index, bDelete);
   }
   Fill();
}

void DataModelListCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsAnyUnselected());
}

void DataModelListCtrl::OnUpdateNeedSel_Deleted(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsDeletedInSelection());
}

void DataModelListCtrl::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
   event.Enable(GetModel() && GetModel()->IsOpen() && IsUndeletedInSelection());
}

void DataModelListCtrl::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
#if USE_DATALISTVIEW
    event.Skip();
#else
    const int index = GetFirstSelected();

    event.Enable(GetModel() && GetModel()->IsOpen() && (index != wxNOT_FOUND));
#endif
}

bool DataModelListCtrl::IsEditable() const
{
    const wxDataModelBase* db = ((DataModelListCtrl*)this)->GetModel();
    
    return (GetWindowStyleFlag() & wxLC_EDIT_LABELS) && db->IsEditable();
}

bool DataModelListCtrl::CanEditLabel(void)
{
   return (m_column_clicked != -1) && IsEditable();
}

#if !USE_DATALISTVIEW
void DataModelListCtrl::OnSelectionChanged(wxListEvent& event)
{
    if (m_id_selchange && (GetFirstSelected() != wxNOT_FOUND))
        wxPostMenuCommand(GetParent(), m_id_selchange);
    event.Skip();
}

void DataModelListCtrl::OnItemActivated(wxListEvent& event)
{
    if (m_id_edit && (GetFirstSelected() != wxNOT_FOUND))
        wxPostMenuCommand(GetParent(), m_id_edit);
    event.Skip();
}

void DataModelListCtrl::OnBeginLabelEdit(wxListEvent& event)
{
   if (CanEditLabel())
      event.Skip(); // does nothing :(
   else
      EndEditLabel(true);
}

void DataModelListCtrl::OnEndLabelEdit(wxListEvent& event)
{
   if (!event.IsEditCancelled())
   {
      wxDataModelBase* db = GetModel();
      const wxString str = event.GetLabel();
      const int pos = GetFirstSelected();
      bool ok = (pos != wxNOT_FOUND);
      wxString errorMsg = _("Failed");

      if (ok)
      {
         wxVariant var;
         wxDataModel::ColumnInfo info;

         ok = GetModel()->GetColumn(m_column_clicked, &info);
         if (ok && (info.type == wxT("string")) )
         {
             ok = (str.length() <= info.len);
             if (!ok) errorMsg = _("Text too long");
         }
         if (ok)
         {
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
      }
      if (!ok)
      {
          wxMessageBox(errorMsg);
          event.Veto();
      }
   }
   m_column_clicked = -1;
}
#endif

bool DataModelListCtrl::IsOpen() const
{
   const wxDataModelBase* db = wxStaticCast(this, DataModelListCtrl)->GetModel(); // unconst

   return db && db->IsOpen();
}
