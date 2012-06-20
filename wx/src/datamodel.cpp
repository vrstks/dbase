// datamodel.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/hash.h>

#include "datamodel.h"

int wxCALLBACK wxDataViewListModelSortedDefaultCompare
      (unsigned int row1, unsigned int row2, unsigned int col, wxDataModelBase* model )
{
//  static const long hash_string   = wxHashTableBase::MakeKey(wxT("string")); not needed
    static const long hash_long     = wxHashTableBase::MakeKey(wxT("long"));
    static const long hash_double   = wxHashTableBase::MakeKey(wxT("double"));
    static const long hash_datetime = wxHashTableBase::MakeKey(wxT("datetime"));

#if (wxVERSION_NUMBER >= 2900)
    const long hash          = wxHashTableBase::MakeKey(model->GetSource()->GetColumnType(col));
#else
    const long hash          = wxHashTableBase::MakeKey(model->GetSource()->GetColType(col));
#endif
    int result;

    if (   (hash == hash_long)
        || (hash == hash_double)
        || (hash == hash_datetime)
       )
    {
        wxVariant value1,value2;

        model->GetValueByRow(value1, row1, col);
        model->GetValueByRow(value2, row2, col);

        if (hash == hash_long) // long
        {
            const long l1 = value1.GetLong();
            const long l2 = value2.GetLong();

            result = l1-l2;
        }
        else if (hash == hash_double) // double
        {
            const double d1 = value1.GetDouble();
            const double d2 = value2.GetDouble();

            if (d1 == d2) result = 0;
            else if (d1 < d2) result = 1;
            else result = -1;
        }
        else // datetime
        {
            const wxDateTime dt1 = value1.GetDateTime();
            const wxDateTime dt2 = value2.GetDateTime();

            if (dt1.IsEqualTo(dt2)) result = 0;
            else result = dt1.IsEarlierThan(dt2) ? -1 : +1;
        }
    }
    else // string or unknown
    {
        wxString str[2];

        model->GetValueByRow( str + 0, row1, col);
        model->GetValueByRow( str + 1, row2, col);
      #ifdef atoi_tcscmpx
        result = atoi_tcscmpx(str[0], str[1], FALSE);
      #else
        result = str[0].CmpNoCase( str[1] );
      #endif
    }
    if ((result == 0) && (col != 0))
    {
       result = wxDataViewListModelSortedDefaultCompare(row1, row2, 0, model);
    }
    return result;
}

static wxDataModelBase             *s_CmpModel = NULL;
static unsigned int                 s_CmpCol = 0;

static int wxDataViewIntermediateCmpAscend( unsigned int row1, unsigned int row2 )
{
    return wxDataViewListModelSortedDefaultCompare( row1, row2, s_CmpCol, s_CmpModel );
}

static int wxDataViewIntermediateCmpDescend( unsigned int row1, unsigned int row2 )
{
    return wxDataViewListModelSortedDefaultCompare( row2, row1, s_CmpCol, s_CmpModel );
}

void wxDataModelSorted::Resort(const wxArrayInt* row_array)
{
    s_CmpModel = m_child;
    s_CmpCol = m_sort_column;

    wxDataViewSortedIndexArray sorted_array(IsSortOrderAscending() ? wxDataViewIntermediateCmpAscend : wxDataViewIntermediateCmpDescend);

    if (row_array)
    {
        for (wxArrayInt::const_iterator it = row_array->begin();
             it != row_array->end();
             it++)
        {
            sorted_array.Add(*it);
        }
    }
    else
    {
        const unsigned int n = m_child->GetRowCount();

        for (unsigned int row = 0; row < n; row++)
        {
            sorted_array.Add(row);
        }
    }
    SetSortArray(sorted_array);
}

size_t wxDataModelBase::GetRow(unsigned int row, wxArrayString* as_ptr, bool header) const
{
   wxArrayString as;

   for (unsigned int col = 0; col < GetColumnCount(); col++)
   {
      wxString str;

      GetValueByRow(&str, row, col);
      if (header)
      {
         ColumnInfo info;

         GetColumn(col, &info);
         str.Printf(wxT("%s:\t%s"),
             info.name.wx_str(),
             str.wx_str());
      }
      as.Add(str);
   }
   if (as_ptr) as_ptr->operator=(as);
   return as.GetCount();
}

size_t wxDataModelBase::GetProperties(wxArrayString* as_ptr, bool header) const
{
   wxArrayString as;

   if (header)
   {
      wxString str;

      if (!GetTableName().empty())
      {
         str.Printf(_("Table:\t%s\t\t"), GetTableName().wx_str());
         as.Add(str);
      }
      str.Printf(_("Records:\t%d\t\t"), GetRowCount());
      as.Add(str);
      as.Add(wxEmptyString);
      as.Add(_("Fields:"));
      //as.Add(wxEmptyString);
   }
   for (unsigned int i = 0; i < GetColumnCount(); i++)
   {
      ColumnInfo info;
      wxString str;

      if (GetColumn(i, &info))
      {
          str.Printf(wxT("%d\t%s:\t%s\t%d"),
             (int)i,
             info.name.wx_str(),
             info.type.wx_str(),
             info.len
             );
      }
      as.Add(str);
   }
   if (as_ptr) *as_ptr = as;
   return as.GetCount();
}
