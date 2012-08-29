// datamodel.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATAMODEL_H__
#define __DATAMODEL_H__

/*

In wx2.8, the wxDataViewListModel class may be used as a universal data model, to represent
any kind of flat (row,col) database type, ready to hook up for (row,col) viewing (wxListCtrl).
This worked very well but requires ticket 11515 to be applied (trac.wxwidgets.org/ticket/11515)

In wx2.9, the wxDataView[Index]ListModel class was changed to dynamically handle several 'dimensions',
the (row,col) perspective no longer applies: the GetRowCount() member function no longer exists.

Here we [re]introduce the wx2.8 (row,col) sorted model class in wx2.9, wxDataViewSortedListModel.

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

   virtual void GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const
   {
       m_child->GetValueByRow(variant, m_array[row], col);
   }

   virtual bool SetValueByRow(const wxVariant &variant, unsigned int row, unsigned int col)
   {
       bool ret = m_child->SetValueByRow(variant, m_array[row], col);

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

   virtual bool GetColumn(unsigned int col, ColumnInfo*) const = 0;
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
#if (wxVERSION_NUMBER >= 2900)
   virtual unsigned int GetRowCount() const = 0;
   virtual unsigned int GetColumnCount() const = 0;
#endif
   virtual wxString GetDataModelName(void) const = 0;

private:
#if (wxVERSION_NUMBER >= 2900)
   virtual void GetValue(wxVariant&, unsigned int row, unsigned int col) const;
   virtual wxString GetColumnType(unsigned int col) const
   {
      ColumnInfo info;

      GetColumn(col, &info);
      return info.type;
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
      ColumnInfo info;

      GetColumn(col, &info);
      return info.type;
   }
#endif
};

/////////////////////////////////////////////////////////////////////////////
// wxDataModelSorted

class wxDataModelSorted : public wxDataViewSortedListModel, public wxDataModelBase
{
   typedef wxDataViewSortedListModel base;
protected:
   int m_sort_column;
   wxDataModel* m_child;
public:
   wxDataModelSorted(wxDataModel* child);

   int GetSortColumn() const { return m_sort_column; }
   wxDataModel* GetChild() { return m_child; }
   void Resort(const wxArrayInt* row_array);

   virtual ~wxDataModelSorted() {}

   virtual size_t GetProperties(wxArrayString*, bool header) const;
   virtual void Resort()
   {
       Resort(NULL);
   }

   virtual bool DeleteRow(unsigned int row, bool bDelete = true );
   virtual bool IsOpen(void) const;
   virtual bool IsEditable(void) const;
   virtual bool AddNew(void);

   virtual bool GetValueByRow(      wxString* , unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxString& , unsigned int row, unsigned int col);
   virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col);

   virtual bool GetColumn(unsigned int col, ColumnInfo*) const;
   virtual bool IsRowDeleted(unsigned int row)
   {
       return m_child->IsRowDeleted(GetArrayValue(row));
   }

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
   virtual wxString GetColumnType(unsigned int col) const
   {
      ColumnInfo info;
      GetColumn(col, &info);
      return info.type;
   }
   virtual unsigned int GetCount() const
   {
      return GetRowCount();
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

inline bool wxDataModelBase::GetColumn(unsigned int WXUNUSED(col), ColumnInfo* info) const
{
   info->type = wxT("string");
   //info->name;
   info->len = 255;
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

inline wxDataModelSorted::wxDataModelSorted(wxDataModel*child) : wxDataViewSortedListModel(child),
   wxDataModelBase(this), m_sort_column(0), m_child(child)
{
#if (wxVERSION_NUMBER >= 2900)
   //Resort();
#endif
}

inline bool wxDataModelSorted::GetColumn(unsigned int col, ColumnInfo* info) const
{
   return m_child->GetColumn(col, info);
}

inline bool wxDataModelSorted::GetValueByRow(wxString* str, unsigned int row, unsigned int col) const
{
    return m_child->GetValueByRow( str, GetArrayValue(row), col);
}

inline bool wxDataModelSorted::SetValueByRow(const wxString& str, unsigned int row, unsigned int col)
{
    bool ret = m_child->SetValueByRow(str, GetArrayValue(row), col);

    // Do nothing here as the change in the
    // child model will be reported back.

    return ret;
}

inline void wxDataModelSorted::GetValueByRow(wxVariant& var, unsigned int row, unsigned int col) const
{
   base::GetValueByRow(var, row, col);
}

inline bool wxDataModelSorted::SetValueByRow(const wxVariant& var, unsigned int row, unsigned int col)
{
   return base::SetValueByRow(var, row, col);
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

#endif // __DATAMODEL_H__
