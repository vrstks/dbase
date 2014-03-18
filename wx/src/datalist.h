// datalist.h
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
#define __DATALIST_H__

#if (wxVERSION_NUMBER >= 2904)
#define USE_DATALISTVIEW 0 // work in progress
#else
#define USE_DATALISTVIEW 0
#endif

#if USE_DATALISTVIEW
#include <wx/dataview.h>
#else
//#include <wx/listctrl.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// DataModelListCtrl

class wxDataModelBase;
class wxDataModel;
#if USE_DATALISTVIEW
class DataModelListCtrl : public wxDataViewCtrl
{
    typedef wxDataViewCtrl base;
    DECLARE_DYNAMIC_CLASS(DataModelListCtrl)
public:
    DataModelListCtrl();

    int GetSelectedRow() const
        { return ItemToRow(GetSelection()); }

    void SelectRow(unsigned row)
        { Select(RowToItem(row)); }

    unsigned int GetItemCount() const
        { return GetStore()->GetCount(); }

    int ItemToRow(const wxDataViewItem &item) const
        { return item.IsOk() ? (int)GetStore()->GetRow(item) : wxNOT_FOUND; }

    wxDataViewItem RowToItem(int row) const
        { return row == wxNOT_FOUND ? wxDataViewItem() : GetStore()->GetItem(row); }

    wxDataViewIndexListModel* GetStore()
        { return dynamic_cast<wxDataViewIndexListModel*>(GetModel()); }
    const wxDataViewIndexListModel* GetStore() const
        { return dynamic_cast<const wxDataViewIndexListModel*>(GetModel()); }

    void SelectNone()
    {
        UnselectAll();
    }
    void Fill()
    {
    }
    bool IsOpen() const;
    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDV_ROW_LINES,
                const wxValidator& validator = wxDefaultValidator);
    virtual ~DataModelListCtrl();

    void AssociateModel(wxDataModel* model)
    {
        base::AssociateModel(model);
        //if (model)
        //    model->Reset();
    }
          wxDataModelBase* GetModel()       { return dynamic_cast<      wxDataModel*>(base::GetModel()); }
    const wxDataModelBase* GetModel() const { return dynamic_cast<const wxDataModel*>(base::GetModel()); }

    void InitColumns(bool row_column = false) { InitColumns(std::vector<int>(), row_column); }
    void InitColumns(const std::vector<int>& col_width, bool row_column = false);

protected:
    DECLARE_EVENT_TABLE()
private:
    bool m_row_column;
};
#else
class DataModelListCtrl : public wxTrunkListView
{
    typedef wxTrunkListView base;
    DECLARE_DYNAMIC_CLASS(DataModelListCtrl)
public:
    DataModelListCtrl();

    void InitColumns(bool row_column = false) { InitColumns(std::vector<int>(), row_column); }
    void InitColumns(const std::vector<int>& col_width, bool row_column = false);

    bool IsAnyUnselected(void);
    bool IsOpen() const;
    int GetEditWindowID() const
    {
        return m_id_edit;
    }
    bool IsEditable() const;

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxLC_REPORT | wxLC_VIRTUAL,
                const wxValidator& validator = wxDefaultValidator);

protected:
    bool CanEditLabel(void);

// Implementation
public:
    virtual ~DataModelListCtrl();
    virtual wxString OnGetItemText(long row, long col) const;
    virtual bool IsRecordOk(unsigned int index);
    virtual void Fill();

    void AssociateModel(wxDataModelBase* model) { m_model = model; }
          wxDataModelBase* GetModel()       { return m_model; }
    const wxDataModelBase* GetModel() const { return m_model; }
    //virtual wxString Format(long col, const wxVariant&) const;

    void Clear()
    {
        AssociateModel(NULL);
        Fill();
    }

    bool SendEvent( const wxEventType type, int row, int col);

    void OnUpdateSelectAll  (wxUpdateUIEvent&);
    void OnUpdateNeedSel    (wxUpdateUIEvent&);
    void OnKeyDown(wxKeyEvent&);

    void OnClick(wxCommandEvent&);
    void OnDblClick(wxCommandEvent&);

    bool GetSubItemRect( long row, long col, wxRect& rect, int code = wxLIST_RECT_BOUNDS) const
    {
        return base::GetSubItemRect(row, col + (m_row_column ? 1 : 0), rect, code);
    }
    wxString GetItemText(long row, long col = 0) const
    {
        return base::GetItemText(row, col + (m_row_column ? 1 : 0));
    }
    void EnsureVisible(long row)
    {
        bool visible = (row >= GetTopItem()) && (row < (GetTopItem() + GetCountPerPage()));
        if (!visible)
            base::EnsureVisible(row);
    }

    bool HasRowColumn() const { return m_row_column; }
    void OnBeginLabelEdit   (wxListEvent&);
    void OnEndLabelEdit     (wxListEvent&);
    void OnSelectionChanged(wxListEvent&);
    void OnItemActivated(wxListEvent&);
    DECLARE_EVENT_TABLE()
protected:
    int m_column_clicked;
    int m_id_edit;
    int m_id_selchange;
    int m_id_delete;
private:
    wxDataModelBase* m_model;
    bool             m_row_column;
};
#endif

extern const wxEventType wxEVT_LIST_CELL_CLICK;
extern const wxEventType wxEVT_LIST_CELL_DCLICK;

class wxListCellEvent : public wxListEvent
{
public:
    wxListCellEvent(wxEventType commandType = wxEVT_NULL, wxWindowID winid = 0) : wxListEvent(commandType, winid) {}//, Row(0), Col(0) {}
    wxListCellEvent(const wxListCellEvent& src) : wxListEvent(src) {}
    virtual wxEvent *Clone() const
    {
        wxListCellEvent* evt = new wxListCellEvent(*this);
        return evt;
    }
    long GetRow() const { return GetIndex(); }
};

typedef void (wxEvtHandler::*wxListCellEventFunction)(wxListCellEvent&);

#define wxListCellEventHandler(func) \
    (wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxListCellEventFunction, &func)

#define wx__DECLARE_LISTCELLEVT(evt, id, fn) \
    wx__DECLARE_EVT1(wxEVT_LIST_CELL_ ## evt, id, wxListCellEventHandler(fn))

#define EVT_LIST_CELL_CLICK(id, fn)  wx__DECLARE_LISTCELLEVT(CLICK, id, fn)
#define EVT_LIST_CELL_DCLICK(id, fn) wx__DECLARE_LISTCELLEVT(DCLICK, id, fn)

/////////////////////////////////////////////////////////////////////////////

#endif // __DATALIST_H__
