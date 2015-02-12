// datalist.cpp
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/trunk.h"
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

wxAlignment DataViewColumnInfo::GetAlignment() const
{
    wxAlignment align = Alignment;
    switch (align)
    {
        case (wxAlignment)wxALIGN_INVALID:
            if (   (wxT("long"  ) == VariantType)
                || (wxT("double") == VariantType) )
            {
                align = wxALIGN_RIGHT;
            }
            else if (   (wxT("datetime") == VariantType)
                     || (wxT("bool"    ) == VariantType) )
            {
                align = wxALIGN_CENTER;
            }
            else
                align = wxALIGN_LEFT;
            break;
        default:
            break;
    }
    return align;
}

//static
DataViewColumnInfoVector DataViewColumnInfoVector::MakeDefaultVector(const wxDataModelBase* model)
{
    DataViewColumnInfoVector vec;

    vec.resize(model->GetColumnCount());
    for (unsigned int col = 0; col < vec.size(); col++)
        model->GetColumn(col, &vec[col]);
    return vec;
}

void DataModelListCtrl::InitColumns(bool row_column)
{
    const wxDataModelBase* model = GetModel();
    const DataViewColumnInfoVector vec = DataViewColumnInfoVector::MakeDefaultVector(model);

    InitColumns(vec, row_column);
}

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
        return base::SetValue(value.IsNull() ? wxVariant((bool)false) : value);
    }
};

void DataModelListCtrl::InitColumns(const DataViewColumnInfoVector& columns, bool row_column)
{
    wxDataModelBase* model = GetModel();
    wxDataViewColumn* column;
    wxDataViewRenderer* tr;
    size_t col = 0;

    m_row_column = row_column;
    if (m_row_column)
    {
        wxASSERT(false);
        tr = new wxDataViewTextRenderer(wxT("long"), wxDATAVIEW_CELL_INERT);
        column = new wxDataViewColumn(wxEmptyString, tr, col, 75, wxALIGN_LEFT);
        AppendColumn(column);
    }

    wxASSERT(columns.size() == model->GetColumnCount());
    for (DataViewColumnInfoVector::const_iterator it = columns.begin(); it != columns.end(); it++)
    {
        wxAlignment align = it->GetAlignment();

        if ( (align == wxALIGN_RIGHT) && ((it + 1) == columns.end())) // last column expands to the right
            align = wxALIGN_LEFT;
        if (wxT("bool") == it->VariantType)
            tr = new MyDataViewToggleRenderer(it->VariantType, wxDATAVIEW_CELL_ACTIVATABLE);
        else
            tr = new wxDataViewTextRenderer(it->VariantType, wxDATAVIEW_CELL_EDITABLE);
        wxString colName = it->Name;
    #ifdef x_DEBUG
        colName += wxString::Format(wxT(" %d"), col);
    #endif
        column = new wxDataViewColumn(colName, tr, col++, it->Width, align,
            wxDATAVIEW_COL_RESIZABLE | (it->Sortable ? wxDATAVIEW_COL_SORTABLE : 0));
        AppendColumn(column);
    }
    Fill();
}

void DataModelListCtrl::OnDoubleClick(wxDataViewEvent& event)
{
    EditItem(event.GetItem(), event.GetDataViewColumn());
    event.Skip();
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

void DataModelListCtrl::InitColumns(const DataViewColumnInfoVector& columns, bool row_column)
{
    wxDataModelBase* model = GetModel();
    m_row_column = row_column;

    if (m_row_column)
        AppendColumn(wxEmptyString, wxLIST_FORMAT_LEFT, 75);

    wxASSERT(columns.size() == model->GetColumnCount());
    for (DataViewColumnInfoVector::const_iterator it = columns.begin(); it != columns.end(); it++)
    {
        wxAlignment align = it->GetAlignment();
        wxListColumnFormat format = wxLIST_FORMAT_LEFT;

        switch(align)
        {
            case wxALIGN_RIGHT:
                format = wxLIST_FORMAT_RIGHT; break;
            case wxALIGN_CENTER:
                format = wxLIST_FORMAT_CENTER; break;
            case wxALIGN_LEFT:
            case wxALIGN_INVALID:
            default:
                format = wxLIST_FORMAT_LEFT; break;
        }
        AppendColumn(it->Name, format, it->Width);
    }
    Fill();
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

bool DataModelListCtrl::IsEditable() const
{
    const wxDataModelBase* db = ((DataModelListCtrl*)this)->GetModel();

    return (GetWindowStyleFlag() & wxLC_EDIT_LABELS) && db->IsEditable();
}

bool DataModelListCtrl::CanEditLabel()
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

bool DataModelListCtrl::IsAnyUnselected()
{
    bool ok = false;
#if USE_DATALISTVIEW
    for (unsigned int row = 0; (!ok) && (row < GetItemCount()); row++)
    {
        wxDataViewItem item = GetItemByRow(row);

        ok = ok || !IsSelected(item);
    }
#else
    for (unsigned int row = 0; (!ok) && (row < (unsigned int)GetItemCount()); row++)
        ok = ok || !IsSelected(row);
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

bool DataModelListCtrl::IsRecordOk(unsigned int row)
{
    return row < GetModel()->GetRowCount();
}

bool DataModelListCtrl::IsOpen() const
{
    const wxDataModelBase* db = wxStaticCast(this, DataModelListCtrl)->GetModel(); // unconst

    return db && db->IsOpen();
}

void DataModelListCtrl::Fill()
{
#if USE_DATALISTVIEW
    GetStore()->Reset(GetItemCount());
#else
    SetItemCount(GetModel() ? GetModel()->GetRowCount() : 0);
    Refresh();
#endif
}
