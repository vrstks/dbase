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
    const wxString colType = GetColumnType(column);
#else
    const wxString colType = GetColType(column);
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
            const long n1 = value1.GetLong();
            const long n2 = value2.GetLong();

            if (n1 == n2)
                result = 0;
            else
                result = (n1 < n2) ? -1 : +1;
            processed = true;
        }
        else if (hash == hash_double) // double
        {
            const double n1 = value1.GetDouble();
            const double n2 = value2.GetDouble();

            if (n1 == n2)
                result = 0;
            else
                result = (n1 < n2) ? -1 : +1;
            processed = true;
        }
        else // datetime
        {
            const wxDateTime n1 = value1.GetDateTime();
            const wxDateTime n2 = value2.GetDateTime();

            if (n1.IsValid() && n2.IsValid())
            {
                if (n1.IsEqualTo(n2))
                    result = 0;
                else
                    result = n1.IsEarlierThan(n2) ? -1 : +1;
                processed = true;
            }
        }
    }
    if (!processed)
    {
        wxString str[2];

        GetValueByRow(str + 0, row1, column);
        GetValueByRow(str + 1, row2, column);
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
        if ((m_sort_column == 0) && m_row_column)
        {
            for (unsigned int row = 0; row < sorted_array.size(); row++)
                sorted_array[row] = (int)(IsSortOrderAscending() ? row : (sorted_array.size() - row - 1));
        }
        else
        {
            s_CmpModel = m_child;
            s_CmpCol   = m_sort_column - (m_row_column ? 1 : 0);
            sorted_array.Sort(IsSortOrderAscending() ? wxDataViewIntermediateCmpAscend : wxDataViewIntermediateCmpDescend);
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
         wxDataModelColumnInfo info;

         GetColumn(col, &info);
         str.Printf(wxT("%s:\t%s"),
             info.Name.wx_str(),
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
      wxString str;

      str.Printf(wxT("%d\t%s:\t%s\t%d"),
         (int)i,
         it->Name.wx_str(),
         it->VariantType.wx_str(),
         it->Length
         );
      as.push_back(str);
   }
   if (as_ptr)
       *as_ptr = as;
   return as.size();
}
