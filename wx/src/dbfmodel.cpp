// dbfmodel.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/dataview.h>
#include <wx/txtstrm.h>
#include <wx/filename.h>
#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"

#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"
#include "dbfmodel.h"
#include "dbfutil.h"
#include "wxstreamc.h"

/*static*/ const wxChar* wxDBFModel::m_fileext = wxT(DBF_FILEEXT);

wxDBFModel::wxDBFModel(wxDBase* database) : wxDataModel(), m_database(database), m_delete_on_exit(false)
{
}

wxDBFModel::~wxDBFModel(void)
{
   if (m_delete_on_exit)
   {
      if (m_database->IsOpen())
      {
         DBF_HANDLE handle = m_database->Detach();
         ::dbf_detach(&handle);
      }
      delete m_database;
   }
}

int wxDBFModel::FindColumn(const wxString& colname) const
{
   return m_database->FindField(colname.mb_str());
}

size_t wxDBFModel::GetProperties(wxArrayString* as, bool header) const
{
   return ::dbf_getproperties(m_database, as, header);
}

void wxDBFModel::GetColumn( unsigned int col, ColumnInfo* info) const
{
   info->type = m_database->GetColType(col);
   info->len = 255;
   wxString str;
   DBF_FIELD_INFO dbf_info;
   if (m_database->GetFieldInfo(col, &dbf_info))
   {
      info->name = wxConvertMB2WX(dbf_info.name);
      info->len = dbf_info.length;
   }
}

unsigned int wxDBFModel::GetRowCount() const
{    
   return m_database->GetRecordCount();
}

unsigned int wxDBFModel::GetColumnCount() const
{
   return m_database->GetFieldCount();
}

bool wxDBFModel::GetValueByRow(wxString* str, unsigned int row, unsigned int col) const
{
   bool ok = m_database->SetPosition(row);
   if (ok) switch (m_database->GetFieldType(col))
   {
      case DBF_DATA_TYPE_BOOLEAN:
      {
         bool n;
         if (m_database->Read(col, &n))
         {
            str->operator=(n ? _("True") : _("False"));
         }
         break;
      }
      case DBF_DATA_TYPE_FLOAT:
      {
         double n;
         if (m_database->Read(col, &n))
         {
            str->Printf(wxT("%g"), n); // date only, not time of day
         }
         break;
      }
      case DBF_DATA_TYPE_DATE:
      {
         wxDateTime n;
         if (m_database->Read(col, &n))
         {
            str->operator=(n.Format(wxT("%x"))); // date only, not time of day
         }
         break;
      }
      default:
         m_database->Read(col, str); // skip wxVariant
         break;
   }
   return ok;
}

void wxDBFModel::GetValueByRow(wxVariant& var, unsigned int row, unsigned int col) const
{
   m_database->GetValueByRow(&var, row, col);
}

bool wxDBFModel::SetValueByRow(const wxString& value, unsigned int row, unsigned int col)
{
   bool ok = true;
   if (ok) ok = m_database->SetPosition(row);
   if (ok) ok = m_database->Write(col, value);
   if (ok) ok = m_database->PutRecord(row);
   return ok;
}

bool wxDBFModel::SetValueByRow(const wxVariant& var, unsigned int row, unsigned int col)
{
   return m_database->SetValueByRow(var, row, col);
}

bool wxDBFModel::IsRowDeleted( unsigned int row )
{
   m_database->SetPosition(row);
   return m_database->IsRecordDeleted();
}

bool wxDBFModel::DeleteRow(unsigned int row, bool bDelete)
{
   m_database->SetPosition(row);
   return m_database->DeleteRecord(bDelete);
}

bool wxDBFModel::IsOpen(void) const
{
   return m_database && m_database->IsOpen();
}

bool wxDBFModel::IsEditable(void) const
{
   return m_database && m_database->IsEditable();
}

bool wxDBFModel::AddNew(void)
{
   return m_database->AddNew();
}

/*static*/ bool wxDBFModel::SaveAs(wxDataModel* model, wxOutputStream* stream)
{
   const size_t row_count = model->GetRowCount();
   const size_t col_count = model->GetColumnCount();
   DBF_FIELD_INFO* array = new DBF_FIELD_INFO[col_count];

   size_t col;
   size_t row;
	for (col = 0; col < col_count; col++)
   {
      ColumnInfo info;
	   model->GetColumn(col, &info);
      
      DBF_FIELD_INFO* item = array + col;
      strncpy(item->name, wxConvertWX2MB(info.name), WXSIZEOF(item->name));
      item->name[WXSIZEOF(item->name) - 1] = 0;
      item->decimals = 0;
      item->length = wxMin(info.len, 255UL);

      if (wxT("datetime") == info.type)
      {
         item->type = DBF_DATA_TYPE_DATE;
      }
      else if (wxT("double") == info.type)
      {
         item->type = DBF_DATA_TYPE_FLOAT;
      }
      else if (wxT("long") == info.type)
      {
         item->type = DBF_DATA_TYPE_INTEGER;
      }
      else if (wxT("bool") == info.type)
      {
         item->type = DBF_DATA_TYPE_BOOLEAN;
      }
      else //if (wxT("string") == info.type)
      {
         item->type = DBF_DATA_TYPE_CHAR;
         if (0 == item->length) item->length = 80;
      }
   }

   wxDBase dbf;
   zlib_filefunc_def api;
   ::fill_filefunc(&api, stream);

   bool ok = dbf.Create(stream, &api, array, col_count);
   if (ok) for (row = 0; row < row_count; row++)
   {
      dbf.AddNew();
      for (col = 0; col < col_count; col++)
      {
         wxVariant var;
         model->GetValueByRow(var, row, col);
         ok = true;
         if (ok)
         {
            switch (array[col].type)
            {
               case DBF_DATA_TYPE_DATE:
                  dbf.Write(col, var.GetDateTime());
                  break;
               case DBF_DATA_TYPE_FLOAT:
                  dbf.Write(col, var.GetDouble());
                  break;
               case DBF_DATA_TYPE_INTEGER:
                  dbf.Write(col, var.GetLong());
                  break;
               case DBF_DATA_TYPE_BOOLEAN:
                  dbf.Write(col, var.GetBool());
                  break;
               case DBF_DATA_TYPE_CHAR:
               default:
                  dbf.Write(col, var.MakeString());
                  break;
            }
         }
         else
         {
            break;
         }
      }
      dbf.PutRecord();
   }
   delete [] array;
   return ok;
}

wxString wxDBFModel::GetTableName(void) const
{
   DBF_INFO info;
   m_database->GetInfo(&info);
   return wxConvertMB2WX(info.tablename);
}
