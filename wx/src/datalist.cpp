// datalist.cpp
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/wx.h"
#include "datamodel.h"
#include "datalist.h"

const wxEventType wxEVT_LIST_CELL_CLICK = wxNewEventType();
const wxEventType wxEVT_LIST_CELL_DCLICK = wxNewEventType();

#if USE_DATALISTVIEW
IMPLEMENT_DYNAMIC_CLASS(DataModelListCtrl, wxDataViewCtrl)
BEGIN_EVENT_TABLE(DataModelListCtrl, wxDataViewCtrl)
    EVT_DATAVIEW_ITEM_ACTIVATED(wxID_ANY, DataModelListCtrl::OnDoubleClick)
END_EVENT_TABLE()
#else
IMPLEMENT_DYNAMIC_CLASS(DataModelListCtrl, wxTrunkListView)
BEGIN_EVENT_TABLE(DataModelListCtrl, wxTrunkListView)
    EVT_KEY_DOWN(DataModelListCtrl::OnKeyDown)
    EVT_LIST_ITEM_SELECTED(wxID_ANY, DataModelListCtrl::OnSelectionChanged)
    EVT_LIST_ITEM_ACTIVATED(wxID_ANY, DataModelListCtrl::OnItemActivated)
    EVT_LIST_BEGIN_LABEL_EDIT(wxID_ANY, DataModelListCtrl::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(wxID_ANY, DataModelListCtrl::OnEndLabelEdit)
    EVT_COMMAND_LEFT_CLICK(wxID_ANY, DataModelListCtrl::OnClick)
    EVT_COMMAND_LEFT_DCLICK(wxID_ANY, DataModelListCtrl::OnDoubleClick)
END_EVENT_TABLE()
#endif

DataModelListCtrl::DataModelListCtrl()
{
    m_row_column = false;
#if !USE_DATALISTVIEW
    m_model = NULL;
    m_column_clicked = wxNOT_FOUND;
    m_id_edit = m_id_selchange = m_id_delete = 0;
#endif
}

DataModelListCtrl::~DataModelListCtrl()
{
    //AssociateModel(NULL); // this is too late to do it for some reason
}

bool DataModelListCtrl::Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos, const wxSize& size,
           long style,
           const wxValidator& validator)
{
#if USE_DATALISTVIEW
    bool ok = base::Create(parent, id, pos, size, style);
    if (ok)
        SetValidator(validator);
#else
    bool ok = base::Create(parent, id, pos, size, style, validator);
    if (ok)
       EnableAlternateRowColours();
#endif
    return ok;
}

#if USE_DATALISTVIEW

class MyDataViewToggleRenderer : public wxDataViewToggleRenderer
{
    typedef wxDataViewToggleRenderer base;
public:
    MyDataViewToggleRenderer(const wxString &varianttype = wxT("bool"),
                              wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT)
                              : wxDataViewToggleRenderer(varianttype, mode)
    {
    }
    virtual bool SetValue(const wxVariant& value)
    {
        if (!value.IsNull())
            return base::SetValue(value);
        else
            return base::SetValue(wxVariant((bool)false));
    }

};

void DataModelListCtrl::InitColumns(const std::vector<int>& col_width, bool row_column)
{
    wxDataModelBase* model = GetModel();
    wxDataViewColumn* column;
    wxDataViewRenderer* tr;
    size_t col = 0;
    
    m_row_column = row_column;
    if (m_row_column)
    {
        tr = new wxDataViewTextRenderer(wxT("long"), wxDATAVIEW_CELL_INERT);
        column = new wxDataViewColumn(wxEmptyString, tr, col++, 75, wxALIGN_LEFT);
        AppendColumn(column);
    }

    const wxDataModelColumnInfoVector columns = model->GetColumns();
    wxASSERT(col_width.empty() || (col_width.size() == columns.size()) );
    for (wxDataModelColumnInfoVector::const_iterator it = columns.begin(); it != columns.end(); it++)
    {
        wxAlignment align = wxALIGN_LEFT;

        if (   (wxT("long"  ) == it->VariantType)
            || (wxT("double") == it->VariantType) )
        {
            if ( (it + 1) != columns.end()) // last column expands to the right
                align = wxALIGN_RIGHT;
        }
        else if (   (wxT("datetime") == it->VariantType)
                 || (wxT("bool"    ) == it->VariantType) )
        {
            align = wxALIGN_CENTER;
        }

        if (wxT("bool") == it->VariantType)
            tr = new MyDataViewToggleRenderer(it->VariantType, wxDATAVIEW_CELL_ACTIVATABLE);
        else
            tr = new wxDataViewTextRenderer(it->VariantType, wxDATAVIEW_CELL_EDITABLE);

        wxString colName = it->Name;
    #ifdef x_DEBUG
        colName += wxString::Format(wxT(" %d"), col);
    #endif
        column = new wxDataViewColumn(colName, tr, col++, col_width.empty() ? 150 : col_width[it - columns.begin()], align);
        AppendColumn(column);
    }
    Fill();
}

void DataModelListCtrl::OnDoubleClick(wxDataViewEvent& event)
{
    EditItem(event.GetItem(), event.GetDataViewColumn());
}

#else // !USE_DATALISTVIEW
bool DataModelListCtrl::SendEvent( const wxEventType type, int row, int col)
{
    wxListCellEvent evt(type, GetId());

    if (m_row_column && (col > 0) )
        col--;
    evt.SetEventObject( this );
    evt.m_itemIndex = row;
    evt.m_col = col;
    evt.m_item.m_itemId = row;
    evt.m_item.m_col = col;
    GetItem(evt.m_item);
    return GetEventHandler()->ProcessEvent(evt);
}

void DataModelListCtrl::OnClick(wxCommandEvent& event)
{
    const wxPoint pt = ScreenToClient(::wxGetMousePosition());
    long col;
    long row = HitTest(pt, NULL, &col);

    if (row != wxNOT_FOUND)
        SendEvent(wxEVT_LIST_CELL_CLICK, row, col);
    event.Skip();
}

void DataModelListCtrl::OnDoubleClick(wxCommandEvent& event)
{
    const wxPoint pt = ScreenToClient(::wxGetMousePosition());
    long col;
    long row = HitTest(pt, NULL, &col);

    if (row != wxNOT_FOUND)
        SendEvent(wxEVT_LIST_CELL_DCLICK, row, col);
    event.Skip();
}

void DataModelListCtrl::OnKeyDown(wxKeyEvent& event)
{
   switch (event.GetKeyCode())
   {
      case WXK_DELETE:
         if (m_id_delete)
         {
            if (HasSelection())
                wxPostMenuCommand(GetParent(), m_id_delete);
         }
         else
             event.Skip();
         break;
      default:
         event.Skip();
         break;
   }
}

void DataModelListCtrl::InitColumns(const std::vector<int>& col_width, bool row_column)
{
    wxDataModelBase* model = GetModel();
    m_row_column = row_column;

    if (m_row_column)
        AppendColumn(wxEmptyString, wxLIST_FORMAT_LEFT, 75);

    const wxDataModelColumnInfoVector columns = model->GetColumns();
    wxASSERT(col_width.empty() || (col_width.size() == columns.size()) );
    for (wxDataModelColumnInfoVector::const_iterator it = columns.begin(); it != columns.end(); it++)
    {
        wxListColumnFormat format = wxLIST_FORMAT_LEFT;
        const wxDataModelColumnInfo& info = *it;

        if (   (wxT("long"  ) == info.VariantType)
            || (wxT("double") == info.VariantType) )
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
        AppendColumn(info.Name, format, col_width.empty() ? 150 : col_width[it - columns.begin()]);
    }
    Fill();
}

void DataModelListCtrl::Fill()
{
    SetItemCount(GetModel() ? GetModel()->GetRowCount() : 0);
    Refresh();
}

wxString DataModelListCtrl::OnGetItemText(long row, long col) const
{
    DataModelListCtrl* pThis = wxStaticCast(this, DataModelListCtrl);
    wxDataModelBase* db = pThis->GetModel(); // unconst
    wxString str;

    if (m_row_column && (col == 0))
        str.Printf(wxT("%d"), (int)row + 1);
    else
    {
        if (m_row_column)
            col--;
        db->GetValueByRow(&str, row, col);
    }
    return str;
}

bool DataModelListCtrl::IsRecordOk(unsigned int index)
{
    return index < GetModel()->GetRowCount();
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

void DataModelListCtrl::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
    event.Enable(GetModel() && GetModel()->IsOpen() && IsAnyUnselected());
}

void DataModelListCtrl::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
    event.Enable(GetModel() && GetModel()->IsOpen() && HasSelection());
}

bool DataModelListCtrl::IsEditable() const
{
    const wxDataModelBase* db = ((DataModelListCtrl*)this)->GetModel();

    return (GetWindowStyleFlag() & wxLC_EDIT_LABELS) && db->IsEditable();
}

bool DataModelListCtrl::CanEditLabel(void)
{
    return (m_column_clicked != wxNOT_FOUND) && IsEditable();
}

void DataModelListCtrl::OnSelectionChanged(wxListEvent& event)
{
    if (m_id_selchange && HasSelection())
        wxPostMenuCommand(GetParent(), m_id_selchange);
    event.Skip();
}

void DataModelListCtrl::OnItemActivated(wxListEvent& event)
{
    if (m_id_edit && HasSelection())
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
            wxDataModelColumnInfo info;

            ok = GetModel()->GetColumn(m_column_clicked, &info);
            if (ok && (info.VariantType == wxT("string")) )
            {
                ok = (str.length() <= info.Length);
                if (!ok)
                    errorMsg = _("Text too long");
            }
            if (ok)
            {
                wxVariant var = str;
                if (ok)
                    ok = db->SetValueByRow(var, pos, m_column_clicked);
            }
        }
        if (!ok)
        {
            wxMessageBox(errorMsg);
            event.Veto();
        }
    }
    m_column_clicked = wxNOT_FOUND;
}
#endif

bool DataModelListCtrl::IsOpen() const
{
    const wxDataModelBase* db = wxStaticCast(this, DataModelListCtrl)->GetModel(); // unconst

    return db && db->IsOpen();
}
