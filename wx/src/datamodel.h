// datamodel.h
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <vector>

#ifdef _WX_DATAVIEW_H_BASE_

#ifndef __DATAMODEL_H__
#define __DATAMODEL_H__

/*

In wx2.8, the wxDataViewListModel class may be used as a universal data model, to represent
any kind of flat (row,col) database type, ready to hook up for (row,col) viewing (wxListCtrl).

In wx2.9, the wxDataView[Index]ListModel class was changed to dynamically handle several 'dimensions',
the (row,col) perspective no longer applies: the GetRowCount() member function no longer exists.

*/

#if (wxVERSION_NUMBER >= 2900)
class wxDataViewListModelEx : public wxDataViewIndexListModel
{
public:
    wxDataViewListModelEx() : wxDataViewIndexListModel()
    {
    }
    virtual unsigned int GetRowCount() const = 0;
};
#else
#define wxDataViewListModelEx wxDataViewListModel
#endif // wx2.9

/////////////////////////////////////////////////////////////////////////////
// wxDataModelBase

class wxDataModelColumnInfo
{
public:
    wxDataModelColumnInfo() : Length(0) {}
    wxDataModelColumnInfo(const wxString& name, const wxString& varianttype, size_t length)
        : Name(name), VariantType(varianttype), Length(length) {}

    wxString Name;
    wxString VariantType;
    size_t   Length;
};
class wxDataModelColumnInfoVector : public std::vector<wxDataModelColumnInfo> {};

class wxDataModel;
class wxDataModelBase
{
protected:
    wxDataViewListModelEx* m_src;

public:
    wxDataModelBase(wxDataViewListModelEx* src) : m_src(src) {}

    wxDataViewListModelEx* GetSource(void) const { return m_src; }

    virtual ~wxDataModelBase() {}
    virtual int FindColumn(const wxString& colname) const;

    virtual size_t GetProperties(wxArrayString*, bool header) const;

    virtual bool DeleteRow(unsigned int row, bool bDelete = true );
    virtual bool IsOpen(void) const;
    virtual bool IsEditable(void) const;
    virtual bool AddNew(void);
    virtual wxDataModel* GetChild() { return NULL; }

    wxDataModelColumnInfoVector GetColumns() const
    {
        wxDataModelColumnInfoVector vec;
        vec.resize(GetColumnCount());

        for (unsigned int col = 0; col < vec.size(); col++)
            GetColumn(col, &vec[col]);
        return vec;
    }

    virtual bool GetColumn(unsigned int col, wxDataModelColumnInfo*) const = 0;
    virtual bool GetValueByRow(      wxString* , unsigned int row, unsigned int col) const;
    virtual bool SetValueByRow(const wxString& , unsigned int row, unsigned int col);
    virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const = 0;
    virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col) = 0;
    virtual unsigned int GetRowCount() const = 0;
    virtual unsigned int GetColumnCount() const = 0;
    virtual wxString GetTableName(void) const
    {
        return wxEmptyString;
    }

    size_t GetRow(unsigned int row, wxArrayString*, bool header = true) const;

#if (wxVERSION_NUMBER >= 2900)
    virtual unsigned int GetCount() const
    {
        return GetRowCount();
    }
#endif

    bool FindRowByColumnValue(const wxVariant&, unsigned int col, unsigned int* row) const;
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModel

class wxDataModel :
#if (wxVERSION_NUMBER >= 2900)
    public wxObject,
#endif
    public wxDataViewListModelEx, public wxDataModelBase
{
public:
    wxDataModel();

    virtual bool GetValueByRow(      wxString* , unsigned int row, unsigned int col) const;
    virtual bool SetValueByRow(const wxString& , unsigned int row, unsigned int col);
    virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const = 0;
    virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col) = 0;
    virtual wxString GetDataModelName(void) const = 0;
    virtual int Compare(unsigned int row1, unsigned int row2, unsigned int column, bool ascending);

#if (wxVERSION_NUMBER >= 2900)
    virtual unsigned int GetRowCount() const = 0;
    virtual unsigned int GetColumnCount() const = 0;
#else
    void RowValueChanged(unsigned int row, unsigned int col)
    {
        ValueChanged(col, row);
    }
#endif

#if (wxVERSION_NUMBER >= 2900)
    void Reset(unsigned int new_size)
    {
        wxDataViewListModelEx::Reset(new_size);
    }
    void Reset()
    {
        Reset(GetRowCount());
    }
#else
    void Reset()
    {
    }
#endif

private:
#if (wxVERSION_NUMBER >= 2900)
    virtual void GetValue(wxVariant&, unsigned int row, unsigned int col) const;
    virtual wxString GetColumnType(unsigned int col) const
    {
        wxDataModelColumnInfo info;

        GetColumn(col, &info);
        return info.VariantType;
    }
    virtual unsigned int GetCount() const
    {
        return GetRowCount();
    }
#else
    virtual void GetValue(wxVariant&, unsigned int col, unsigned int row);
    virtual bool SetValue(wxVariant&, unsigned int col, unsigned int row);
    virtual unsigned int GetNumberOfCols()
    {
        return GetColumnCount();
    }
    virtual unsigned int GetNumberOfRows()
    {
        return GetRowCount();
    }
    virtual wxString GetColType(unsigned int col)
    {
        wxDataModelColumnInfo info;

        GetColumn(col, &info);
        return info.VariantType;
    }
#endif
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModelSorted
// Cloned from wxDataViewSortedListModel in wx28; removed in wx29

class wxDataModelSorted : public wxDataModel
{
    typedef wxDataModel base;
public:
    wxDataModelSorted(wxDataModel* child, bool row_column = false);

    int GetSortColumn() const { return m_sort_column; }
    virtual wxDataModel* GetChild() { return m_child; }
    void SetChild(wxDataModel* child) { m_child = child; }
    void Resort(const wxArrayInt* row_array);
    bool HasRowColumn() const { return m_row_column; }

    virtual ~wxDataModelSorted() { delete m_child; }

    virtual size_t GetProperties(wxArrayString*, bool header) const;
    virtual void Resort()
    {
        Resort(NULL);
    }
    virtual wxString GetDataModelName(void) const
    {
        return m_child->GetDataModelName();
    }

    virtual bool DeleteRow(unsigned int row, bool bDelete = true );
    virtual bool IsOpen(void) const;
    virtual bool IsEditable(void) const;
    virtual bool AddNew(void);

    virtual bool GetValueByRow(      wxString* , unsigned int row, unsigned int col) const;
    virtual bool SetValueByRow(const wxString& , unsigned int row, unsigned int col);
    virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const;
    virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col);

    virtual bool GetColumn(unsigned int col, wxDataModelColumnInfo*) const;

    virtual unsigned int GetRowCount() const
    {
        return (unsigned int)m_array.size();
    }
    virtual unsigned int GetColumnCount() const
    {
        return m_child->GetColumnCount() + (m_row_column ? 1 : 0);
    }

    void SetSortOrder(bool ascending, int sort_column)
    {
        SetSortOrder(ascending);
        m_sort_column = sort_column;
    }
/*
    virtual wxString GetColumnType(unsigned int col) const
    {
        wxDataModelColumnInfo info;
        GetColumn(col, &info);
        return info.type;
    }
    virtual unsigned int GetCount() const
    {
        return GetRowCount();
    }
*/
    void SetSortOrder( bool ascending ) { m_ascending = ascending; }
    bool IsSortOrderAscending() const { return m_ascending; }
    void SetSortArray(const wxArrayInt& data) { m_array = data; }
    unsigned int GetArrayValue(unsigned int row) const { return m_array[row]; }
protected:
    int m_sort_column;
    bool m_row_column;
    wxDataModel* m_child;
    bool m_ascending;
    wxArrayInt m_array;
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModelBase

inline bool wxDataModelBase::GetColumn(unsigned int WXUNUSED(col), wxDataModelColumnInfo* info) const
{
    info->VariantType = wxT("string");
    //info->name;
    info->Length = 255;
    return true;
}

inline int wxDataModelBase::FindColumn(const wxString& WXUNUSED(colname)) const
{
    return wxNOT_FOUND;
}

inline bool wxDataModelBase::DeleteRow(unsigned int WXUNUSED(row), bool WXUNUSED(bDelete))
{
    return false;
}

inline bool wxDataModelBase::IsEditable(void) const
{
    return false;
}

inline bool wxDataModelBase::AddNew(void)
{
    return false;
}

inline bool wxDataModelBase::IsOpen(void) const
{
    return true;
}

inline bool wxDataModelBase::GetValueByRow(wxString* str, unsigned int row, unsigned int col) const
{
    wxVariant var;
    GetValueByRow(var, row, col);
    str->operator=(var.GetString());
    return true;
}

inline bool wxDataModelBase::SetValueByRow(const wxString& str, unsigned int row, unsigned int col)
{
    const wxVariant var(str);
    return SetValueByRow(var, row, col);
}

inline bool wxDataModelBase::FindRowByColumnValue(const wxVariant& value, unsigned int col, unsigned int* row_ptr) const
{
    for (unsigned int row = 0; row < GetRowCount(); row++)
    {
        wxVariant var;
        GetValueByRow(var, row, col);
        if ( (!var.IsNull()) && (value == var) )
        {
            if (row_ptr)
                *row_ptr = row;
            return true;
        }
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// wxDataModel

inline wxDataModel::wxDataModel() : wxDataViewListModelEx(), wxDataModelBase(this)
{
}

inline bool wxDataModel::GetValueByRow(wxString* str, unsigned int row, unsigned int col) const
{
    return wxDataModelBase::GetValueByRow(str, row, col);
}

inline bool wxDataModel::SetValueByRow(const wxString& str, unsigned int row, unsigned int col)
{
    return wxDataModelBase::SetValueByRow(str, row, col);
}

inline void
#if (wxVERSION_NUMBER >= 2900)
wxDataModel::GetValue(wxVariant& var, unsigned int row, unsigned int col) const
#else
wxDataModel::GetValue(wxVariant& var, unsigned int col, unsigned int row )
#endif
{
    GetValueByRow(var, row, col);
}

#if (wxVERSION_NUMBER < 2900)
inline bool wxDataModel::SetValue(wxVariant& var, unsigned int col, unsigned int row )
{
    return SetValueByRow(var, row, col);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// wxDataModelSorted

inline wxDataModelSorted::wxDataModelSorted(wxDataModel*child, bool row_column) : //wxDataViewSortedListModel(child),
   wxDataModel(), m_sort_column(wxNOT_FOUND), m_row_column(row_column), m_child(child), m_ascending(true)
{
    Resort();
}

inline bool wxDataModelSorted::GetColumn(unsigned int col, wxDataModelColumnInfo* info) const
{
    if ( (col == 0) && m_row_column)
    {
        info->VariantType = wxT("long");
        info->Name = wxEmptyString;
        return true;
    }
    else
        return m_child->GetColumn(col - (m_row_column ? 1 : 0), info);
}

inline bool wxDataModelSorted::GetValueByRow(wxString* str, unsigned int row, unsigned int col) const
{
    if ( (col == 0) && m_row_column)
    {
        str->Printf(wxT("%d"), m_array[row] + 1);
        return true;
    }
    else
        return m_child->GetValueByRow( str, GetArrayValue(row), col - (m_row_column ? 1 : 0));
}

inline bool wxDataModelSorted::SetValueByRow(const wxString& str, unsigned int row, unsigned int col)
{
    if ( (col == 0) && m_row_column)
        return false;
    else
        return m_child->SetValueByRow(str, GetArrayValue(row), col - (m_row_column ? 1 : 0));
}

inline void wxDataModelSorted::GetValueByRow(wxVariant& var, unsigned int row, unsigned int col) const
{
    if ( (col == 0) && m_row_column)
        var = (long)(m_array[row] + 1);
    else
        m_child->GetValueByRow( var, GetArrayValue(row), col - (m_row_column ? 1 : 0));
}

inline bool wxDataModelSorted::SetValueByRow(const wxVariant& var, unsigned int row, unsigned int col)
{
    if ( (col == 0) && m_row_column)
        return false;
    else
        return m_child->SetValueByRow( var, GetArrayValue(row), col - (m_row_column ? 1 : 0));
}

inline size_t wxDataModelSorted::GetProperties(wxArrayString* as, bool header) const
{
    return m_child->GetProperties(as, header);
}

inline bool wxDataModelSorted::DeleteRow(unsigned int row, bool bDelete)
{
    return m_child->DeleteRow(row, bDelete);
}

inline bool wxDataModelSorted::IsOpen(void) const
{
    return m_child->IsOpen();
}

inline bool wxDataModelSorted::IsEditable(void) const
{
    return m_child->IsEditable();
}

inline bool wxDataModelSorted::AddNew(void)
{
    return m_child->AddNew();
}

#if (wxVERSION_NUMBER >= 2900)
class wxDataViewModelNotifierEx : public wxDataViewModelNotifier
#else
class wxDataViewModelNotifierEx : public wxDataViewListModelNotifier
#endif
{
public:
    wxDataViewModelNotifierEx() { }
    virtual ~wxDataViewModelNotifierEx() { }

#if (wxVERSION_NUMBER >= 2900)
    virtual bool ItemAdded(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItem& WXUNUSED(item))
    {
        return true;
    }
    virtual bool ItemDeleted(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItem& WXUNUSED(item))
    {
        return true;
    }
    virtual bool ItemChanged(const wxDataViewItem& WXUNUSED(item))
    {
        return true;
    }
    virtual bool ItemsAdded(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItemArray& WXUNUSED(items))
    {
        return true;
    }
    virtual bool ItemsDeleted(const wxDataViewItem& WXUNUSED(parent), const wxDataViewItemArray& WXUNUSED(items))
    {
        return true;
    }
    virtual bool ItemsChanged(const wxDataViewItemArray& WXUNUSED(items))
    {
        return true;
    }
    virtual bool ValueChanged(const wxDataViewItem& WXUNUSED(item), unsigned int WXUNUSED(col))
    {
        return true;
    }
    virtual bool Cleared()
    {
        return true;
    }
    virtual void Resort()
    {
    }
#else
    virtual bool RowAppended()
    {
        return true;
    }
    virtual bool RowPrepended()
    {
        return true;
    }
    virtual bool RowInserted(unsigned int WXUNUSED(before))
    {
        return true;
    }
    virtual bool RowDeleted(unsigned int WXUNUSED(row))
    {
        return true;
    }
    virtual bool RowChanged(unsigned int WXUNUSED(row))
    {
        return true;
    }
    virtual bool ValueChanged(unsigned int WXUNUSED(col), unsigned int WXUNUSED(row))
    {
        return true;
    }
    virtual bool RowsReordered(unsigned int* WXUNUSED(new_order))
    {
        return true;
    }
    virtual bool Cleared()
    {
        return true;
    }
#endif
};

#endif // __DATAMODEL_H__

#endif // _WX_DATAVIEW_H_BASE_
