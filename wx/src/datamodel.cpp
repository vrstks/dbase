// datamodel.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/hash.h>

#include "datamodel.h"

int wxDataModel::Compare(unsigned int row1, unsigned int row2, unsigned int column, bool ascending)
{
//  static const long hash_string   = wxHashTableBase::MakeKey(wxT("string")); not needed
    static const long hash_long     = wxHashTableBase::MakeKey(wxT("long"));
    static const long hash_double   = wxHashTableBase::MakeKey(wxT("double"));
    static const long hash_datetime = wxHashTableBase::MakeKey(wxT("datetime"));

#if (wxVERSION_NUMBER >= 2900)
    const wxString colType = GetSource()->GetColumnType(column);
#else
    const wxString colType = GetSource()->GetColType(column);
#endif
    const long hash = wxHashTableBase::MakeKey(colType);
    int result = 0;
    bool processed = false;

    if (   (hash == hash_long)
        || (hash == hash_double)
        || (hash == hash_datetime)
       )
    {
        wxVariant value1,value2;

        GetValueByRow(value1, row1, column);
        GetValueByRow(value2, row2, column);

        if (hash == hash_long) // long
        {
            const long l1 = value1.GetLong();
            const long l2 = value2.GetLong();

            result = l1-l2;
            processed = true;
        }
        else if (hash == hash_double) // double
        {
            const double d1 = value1.GetDouble();
            const double d2 = value2.GetDouble();

            if (d1 == d2)
                result = 0;
            else if (d1 < d2)
                result = 1;
            else
                result = -1;
            processed = true;
        }
        else // datetime
        {
            const wxDateTime dt1 = value1.GetDateTime();
            const wxDateTime dt2 = value2.GetDateTime();

            if (dt1.IsValid() && dt2.IsValid())
            {
                if (dt1.IsEqualTo(dt2))
                    result = 0;
                else
                    result = dt1.IsEarlierThan(dt2) ? -1 : +1;
                processed = true;
            }
        }
    }
    if (!processed)
    {
        wxString str[2];

        GetValueByRow( str + 0, row1, column);
        GetValueByRow( str + 1, row2, column);
      #ifdef atoi_tcscmpx
        result = atoi_tcscmpx(str[0], str[1], FALSE);
      #else
        result = str[0].CmpNoCase( str[1] );
      #endif
    }
    if ((result == 0) && (column != 0))
        result = Compare(row1, row2, 0, ascending);
    else
        result *= ascending ? +1 : -1;
    return result;
}

static wxDataModel* s_CmpModel = NULL;
static unsigned int s_CmpCol = 0;

static int wxDataViewIntermediateCmpAscend( int* row1, int* row2 )
{
    return s_CmpModel->Compare( *row1, *row2, s_CmpCol, true);
}

static int wxDataViewIntermediateCmpDescend( int* row1, int* row2 )
{
    return s_CmpModel->Compare( *row1, *row2, s_CmpCol, false);
}

void wxDataModelSorted::Resort(const wxArrayInt* row_array)
{
    wxArrayInt sorted_array;

    if (row_array)
        sorted_array = *row_array;
    else
    {
        sorted_array.resize(m_child->GetRowCount());
        for (unsigned int row = 0; row < sorted_array.size(); row++)
            sorted_array[row] = row;
    }
    if (m_sort_column != wxNOT_FOUND)
    {
        s_CmpModel = m_child;
        s_CmpCol = m_sort_column;
        sorted_array.Sort(IsSortOrderAscending() ? wxDataViewIntermediateCmpAscend : wxDataViewIntermediateCmpDescend);
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
         wxDataModelColumnInfo info;

         GetColumn(col, &info);
         str.Printf(wxT("%s:\t%s"),
             info.name.wx_str(),
             str.wx_str());
      }
      as.push_back(str);
   }
   if (as_ptr)
       as_ptr->operator=(as);
   return as.size();
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
         as.push_back(str);
      }
      str.Printf(_("Records:\t%d\t\t"), GetRowCount());
      as.push_back(str);
      as.push_back(wxEmptyString);
      as.push_back(_("Fields:"));
      //as.push_back(wxEmptyString);
   }
   const wxDataModelColumnInfoVector columns = GetColumns();
   size_t i = 0;
   for (wxDataModelColumnInfoVector::const_iterator it = columns.begin(); it != columns.end(); it++, i++)
   {
      const wxDataModelColumnInfo& info = *it;
      wxString str;

      str.Printf(wxT("%d\t%s:\t%s\t%d"),
         (int)i,
         info.name.wx_str(),
         info.type.wx_str(),
         info.len
         );
      as.push_back(str);
   }
   if (as_ptr) *as_ptr = as;
   return as.size();
}
