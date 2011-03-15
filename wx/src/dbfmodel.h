// dbfmodel.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFMODEL_H__
#define __DBFMODEL_H__

#ifndef __DATAMODEL_H__
#include "datamodel.h"
#endif

class wxDBase;
class wxOutputStream;
class wxDBFModel : public wxDataModel
{
   typedef wxDataModel base;
// Attributes
public:
   wxDBase* m_database;
   bool m_delete_on_exit;
   static const wxChar* m_fileext;

   wxDBFModel(wxDBase*);

   static bool SaveAs(wxDataModel*, wxOutputStream*);

// Implementation
public:
   virtual ~wxDBFModel(void);

// wxDataViewListModel implementation
public:
   virtual unsigned int GetRowCount(void) const;
   virtual unsigned int GetColumnCount(void) const;
   virtual void GetColumn( unsigned int col, ColumnInfo*) const;
   virtual bool IsRowDeleted( unsigned int row );
   virtual int FindColumn(const wxString& colname) const;

// wxDataModel implementation
public:
   virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col);
   virtual bool DeleteRow(unsigned int row, bool bDelete = true);
   virtual bool GetValueByRow(      wxString*, unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxString&, unsigned int row, unsigned int col);
   virtual size_t GetProperties(wxArrayString*, bool header) const;
   virtual bool IsOpen(void) const;
   virtual bool IsEditable(void) const;
   virtual bool AddNew(void);
   virtual wxString GetDataModelName(void) const
   {
      return wxT("dBASE");
   }
   virtual wxString GetTableName(void) const;
};

#endif // __DBFMODEL_H__
