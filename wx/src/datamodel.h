// datamodel.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATAMODEL_H__
#define __DATAMODEL_H__

#ifndef WXROWCOL_DEFINED
#define WXROWCOL_DEFINED
typedef struct _wxDataViewRowCol
{
   unsigned int row;
   unsigned int col;
} wxRowCol;
#endif
/*

In wx2.8, the wxDataViewListModel class may be used as a universal data model, to represent
any kind of flat (row,col) database type, ready to hook up for (row,col) viewing (wxListCtrl).
This worked very well but requires ticket 11515 to be applied (trac.wxwidgets.org/ticket/11515)

In wx2.9, the wxDataView[Index]ListModel class was changed to dynamically handle several 'dimensions',
the (row,col) perspective no longer applies: the GetRowCount() member function no longer exists.

Here we [re]introduce the wx2.8 (row,col) sorted model class in wx2.9, wxDataViewSortedListModel.

*/

#if (wxVERSION_NUMBER >= 2900)

class wxDataViewListModelEx : public wxDataViewListModel
{
public:
   wxDataViewListModelEx() : wxDataViewListModel()
   {
   }
   virtual unsigned int GetRowCount() const = 0;
   
   virtual bool GetValue(wxVariant*, const wxRowCol& rowcol) const = 0;
   virtual bool SetValue(const wxVariant&, const wxRowCol&) = 0;

   virtual void GetValueByRow( wxVariant &variant,
                        unsigned int row, unsigned int col ) const
   {
      wxRowCol rowcol;
      rowcol.row = row;
      rowcol.col = col;
      GetValue(&variant, rowcol);
   }

   virtual bool SetValueByRow( const wxVariant &variant,
                        unsigned int row, unsigned int col )
   {
      wxRowCol rowcol;
      rowcol.row = row;
      rowcol.col = col;
      return SetValue(variant, rowcol);
   }

private:
   virtual unsigned int GetChildren(const wxDataViewItem&, wxDataViewItemArray&) const
   {
      return 0;
   }
   virtual unsigned GetRow( const wxDataViewItem&) const
   {
      return 0;
   }
};

WX_DEFINE_SORTED_ARRAY_SIZE_T(unsigned int, wxDataViewSortedIndexArray);

class wxDataViewSortedListModel : public wxDataViewListModelEx
{
private:
   wxDataViewListModelEx* m_child;
   wxDataViewSortedIndexArray m_array;
   bool m_ascending;
public:
   wxDataViewSortedListModel( wxDataViewListModelEx *child ) : wxDataViewListModelEx(),
      m_child(child), m_array(NULL), m_ascending(true)
   {
   }

   void SetSortOrder( bool ascending ) { m_ascending = ascending; }
   bool IsSortOrderAscending() const { return m_ascending; }

   void SetSortArray(const wxDataViewSortedIndexArray& data) { m_array = data; }
   unsigned int GetArrayValue(unsigned int index) const { return m_array[index]; }
   wxDataViewListModelEx* GetChild() { return m_child; }

   virtual unsigned int GetRowCount() const
   {
       return m_array.GetCount();
   }

   virtual unsigned int GetColumnCount() const
   {
       return m_child->GetColumnCount();
   }

   virtual void GetValue( wxVariant &variant, const wxRowCol& rowcol) const
   {
       wxRowCol child;
       child.row = m_array[rowcol.row];
       child.col = rowcol.col;
       m_child->GetValue(&variant, child);
   }

   virtual bool SetValue( const wxVariant &variant, const wxRowCol& rowcol)
   {
       wxRowCol child;
       child.row = m_array[rowcol.row];
       child.col = rowcol.col;
       bool ret = m_child->SetValue(variant, child);

       // Do nothing here as the change in the
       // child model will be reported back.

       return ret;
   }

};

#else
#define wxDataViewListModelEx wxDataViewListModel
#endif // wx2.9

/////////////////////////////////////////////////////////////////////////////
// wxDataModelBase

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

   //virtual wxString GetFilename(void) const;
   virtual size_t GetProperties(wxArrayString*, bool header) const;

   virtual bool DeleteRow(unsigned int row, bool bDelete = true );
   virtual bool IsOpen(void) const;
   virtual bool IsEditable(void) const;
   virtual bool AddNew(void);
   virtual bool IsRowDeleted(unsigned int WXUNUSED(row))
   {
      return false;
   }

   class ColumnInfo
   {
   public:
      wxString name;
      wxString type;
      size_t len;
   };

   virtual void GetColumn(unsigned int col, ColumnInfo*) const = 0;
   virtual bool GetValue(      wxString* , const wxRowCol&) const;
   virtual bool SetValue(const wxString& , const wxRowCol&);
   virtual bool GetValue(      wxVariant*, const wxRowCol&) const = 0;
   virtual bool SetValue(const wxVariant&, const wxRowCol&) = 0;
   virtual unsigned int GetRowCount() const = 0;
   virtual unsigned int GetColumnCount() const = 0;
   virtual wxString GetTableName(void) const
   {
      return wxEmptyString;
   }

   size_t GetRow(unsigned int row, wxArrayString*, bool header = true) const;
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModel

class wxDataModel : public wxDataViewListModelEx, public wxDataModelBase
#if (wxVERSION_NUMBER >= 2900)
   , public wxObject
#endif
{
public:
   wxDataModel();

   virtual bool GetValue(      wxString* , const wxRowCol&) const;
   virtual bool SetValue(const wxString& , const wxRowCol&);
   virtual bool GetValue(      wxVariant*, const wxRowCol&) const = 0;
   virtual bool SetValue(const wxVariant&, const wxRowCol&) = 0;
#if (wxVERSION_NUMBER >= 2900)
   virtual unsigned int GetRowCount() const = 0;
   virtual unsigned int GetColumnCount() const = 0;
#endif

private:
#if (wxVERSION_NUMBER >= 2900)
   virtual void GetValue(wxVariant&, unsigned int row, unsigned int col) const;
   virtual wxString GetColumnType(unsigned int col) const
   {
      ColumnInfo info;
      GetColumn(col, &info);
      return info.type;
   }
#else
   virtual void GetValue(wxVariant&, unsigned int col, unsigned int row );
   virtual bool SetValue(wxVariant&, unsigned int col, unsigned int row );
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
      ColumnInfo info;
      GetColumn(col, &info);
      return info.type;
   }
   virtual wxString GetDataModelName(void) const = 0;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModelSorted

class wxDataModelSorted : public wxDataViewSortedListModel, public wxDataModelBase
{
   typedef wxDataViewSortedListModel base;
public:
   int m_sort_column;

   wxDataModel* m_child;
   wxDataModelSorted(wxDataModel* child);

   virtual ~wxDataModelSorted() {}
   virtual void Resort();

   virtual size_t GetProperties(wxArrayString*, bool header) const;

   virtual bool DeleteRow(unsigned int row, bool bDelete = true );
   virtual bool IsOpen(void) const;
   virtual bool IsEditable(void) const;
   virtual bool AddNew(void);

   virtual bool GetValue(      wxString* , const wxRowCol&) const;
   virtual bool SetValue(const wxString& , const wxRowCol&);
   virtual bool GetValue(      wxVariant*, const wxRowCol&) const;
   virtual bool SetValue(const wxVariant&, const wxRowCol&);

   virtual void GetColumn(unsigned int col, ColumnInfo*) const;

   //virtual unsigned int GetRowCount() { return m_child->wxDataViewListModelEx::GetNumberOfRows(); }
   virtual unsigned int GetRowCount() const
   {
   #if (wxVERSION_NUMBER >= 2900)
      return base::GetRowCount(); // eh
   #else
      return ((wxDataModelSorted*)this)->base::GetNumberOfRows();
   #endif
   }
   virtual unsigned int GetColumnCount() const
   {
   #if (wxVERSION_NUMBER >= 2900)
      return base::GetColumnCount();
   #else
      return ((wxDataModelSorted*)this)->GetNumberOfCols(); // unconst
   #endif
   }
   //size_t GetArrayValue(size_t array_index) const { return m_array[array_index]; }

   void SetSortOrder(bool ascending, int sort_column)
   {
   #if (wxVERSION_NUMBER >= 2900)
      base::SetSortOrder(ascending);
   #else
      base::SetAscending(ascending);
   #endif
      m_sort_column = sort_column;
   }
#if (wxVERSION_NUMBER >= 2900)
   virtual void GetValue(wxVariant&, unsigned int col, unsigned int row ) const;
   virtual wxString GetColumnType(unsigned int col) const
   {
      ColumnInfo info;
      GetColumn(col, &info);
      return info.type;
   }
#else
   bool IsSortOrderAscending() /*const*/
   {
      return GetAscending();
   }
#endif
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModelBase

inline void wxDataModelBase::GetColumn(unsigned int WXUNUSED(col), ColumnInfo* info) const
{
   info->type = wxT("string");
   //info->name;
   info->len = 255;
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

/*
inline wxString wxDataModelBase::GetFilename(void) const
{
   return wxEmptyString;
}
*/

inline bool wxDataModelBase::GetValue(wxString* str, const wxRowCol& rowcol) const
{
   wxVariant var;
   bool ok = GetValue(&var, rowcol);
   if (ok && str) str->operator=(var.GetString());
   return ok;
}

inline bool wxDataModelBase::SetValue(const wxString& str, const wxRowCol& rowcol)
{
   const wxVariant var(str);
   return SetValue(var, rowcol);
}

/////////////////////////////////////////////////////////////////////////////
// wxDataModel

inline wxDataModel::wxDataModel() : wxDataViewListModelEx(), wxDataModelBase(this)
{
}

inline bool wxDataModel::GetValue(wxString* str, const wxRowCol& rowcol) const
{
   return wxDataModelBase::GetValue(str, rowcol);
}

inline bool wxDataModel::SetValue(const wxString& str, const wxRowCol& rowcol)
{
   return wxDataModelBase::SetValue(str, rowcol);
}

inline void
#if (wxVERSION_NUMBER >= 2900)
wxDataModel::GetValue(wxVariant& var, unsigned int row, unsigned int col) const
#else
wxDataModel::GetValue(wxVariant& var, unsigned int col, unsigned int row )
#endif
{
   wxRowCol rowcol;
   rowcol.row = row;
   rowcol.col = col;
   wxVariant* var_ptr = &var;
   GetValue(var_ptr, rowcol);
}

#if (wxVERSION_NUMBER < 2900)
inline bool wxDataModel::SetValue(wxVariant& var, unsigned int col, unsigned int row )
{
   wxRowCol rowcol;
   rowcol.row = row;
   rowcol.col = col;
   return SetValue((const wxVariant&)var, rowcol);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// wxDataModelSorted

inline wxDataModelSorted::wxDataModelSorted(wxDataModel*child) : wxDataViewSortedListModel(child),
   wxDataModelBase(this), m_sort_column(0), m_child(child)
{
#if (wxVERSION_NUMBER >= 2900)
   //Resort();
#endif
}

#if (wxVERSION_NUMBER >= 2900)
inline void wxDataModelSorted::GetValue(wxVariant& var, unsigned int col, unsigned int row ) const
{
   wxRowCol rowcol;
   rowcol.row = row;
   rowcol.col = col;
   GetValue(&var, rowcol);
}
#endif

inline void wxDataModelSorted::GetColumn(unsigned int col, ColumnInfo* info) const
{
/*
#if (wxVERSION_NUMBER >= 2900)
   info->type = base::GetColumnType(col);
   info->name = m_child->GetColumnName(col);
   info->len = 255;
#else
   m_child->GetColumn(col, info);
#endif
*/
   m_child->GetColumn(col, info);
}

inline bool wxDataModelSorted::GetValue(wxString* str, const wxRowCol& rowcol) const
{
    wxRowCol child;
    child.row = GetArrayValue(rowcol.row);
    child.col = rowcol.col;
    return m_child->GetValue( str, child);
}

inline bool wxDataModelSorted::SetValue(const wxString& str, const wxRowCol& rowcol)
{
    wxRowCol child;
    child.row = GetArrayValue(rowcol.row);
    child.col = rowcol.col;
    bool ret = m_child->SetValue(str, child);

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

inline bool wxDataModelSorted::GetValue(wxVariant* var, const wxRowCol& rowcol) const
{
   wxDataModelSorted* pThis = (wxDataModelSorted*)this; // unconst

#if (wxVERSION_NUMBER >= 2900)
   pThis->base::GetValue(*var, rowcol);
#else // vice versa
   pThis->base::GetValue(*var, rowcol.col, rowcol.row);
#endif
   return true;
}

inline bool wxDataModelSorted::SetValue(const wxVariant& var, const wxRowCol& rowcol)
{
#if (wxVERSION_NUMBER >= 2900)
   return base::SetValue(var, rowcol);
#else
   return base::SetValue((wxVariant&)var, rowcol.row, rowcol.col);
#endif
}

/*
inline wxString wxDataModelSorted::GetFilename(void) const
{
   return m_child->GetFilename();
}
*/

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

#endif // __DATAMODEL_H__
