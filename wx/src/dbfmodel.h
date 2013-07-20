// dbfmodel.h
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFMODEL_H__
#define __DBFMODEL_H__

#ifndef __DATAMODEL_H__
#include "datamodel.h"
#endif

class wxDBase;
class wxOutputStream;
class DBFModel : public wxDataModel
{
   typedef wxDataModel base;

// Attributes
protected:
   wxDBase* m_database;
public:
   bool m_delete_on_exit;
   static const wxChar* m_fileext;

   DBFModel(wxDBase*);

// Operations
public:
   void     SetDatabase(wxDBase* database) { m_database = database; }
   wxDBase* GetDatabase() const { return m_database; }

   static bool SaveAs(wxDataModel*, wxOutputStream*);

// Implementation
public:
   virtual ~DBFModel(void);

// wxDataViewListModel implementation
public:
   virtual unsigned int GetRowCount(void) const;
   virtual unsigned int GetColumnCount(void) const;
   virtual bool GetColumn( unsigned int col, wxDataModelColumnInfo*) const;
   virtual int FindColumn(const wxString& colname) const;

// wxDataModel implementation
public:
   virtual void GetValueByRow(      wxVariant&, unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col);
   virtual bool GetValueByRow(      wxString*, unsigned int row, unsigned int col) const;
   virtual bool SetValueByRow(const wxString&, unsigned int row, unsigned int col);
   virtual size_t GetProperties(wxArrayString*, bool header) const;
   virtual bool IsOpen(void) const;
   virtual bool IsEditable(void) const;
   virtual bool AddNew(void);
   virtual wxString GetDataModelName(void) const;
   virtual wxString GetTableName(void) const;

    // new virtuals
    virtual bool IsRowDeleted( unsigned int row );
    virtual bool DeleteRow(unsigned int row, bool bDelete = true);
};

#endif // __DBFMODEL_H__
