// dbf_wx.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/wx.h>
#include <wx/variant.h>
#include <wx/docview.h>

#include "ioapi/zlib.h"
#include "ioapi/ioapi.h"
#include "bool.h"
#include "dbf.h"
#include "dbf.hpp"
#include "dbf.inl"
#include "dbf_wx.h"
#include "dbf_wx.inl"
#include "wx/src/wxstreamc.h"

wxString wxDBase::GetColType(size_t col)
{
   switch (GetFieldType(GetFieldPtr(col)))
   {
      case DBF_DATA_TYPE_INTEGER:
         return wxT("long");
      case DBF_DATA_TYPE_FLOAT  :
         return wxT("double");
      case DBF_DATA_TYPE_DATE   :
         return wxT("datetime");
      case DBF_DATA_TYPE_BOOLEAN:
         return wxT("bool");
      case DBF_DATA_TYPE_MEMO   :
      case DBF_DATA_TYPE_CHAR   :
      case DBF_DATA_TYPE_UNKNOWN:
      default:
         return wxT("string");
   }
}

// get value into a wxVariant
bool wxDBase::GetValue(wxVariant* var, const wxRowCol& rowcol)
{
   bool ok = SetPosition(rowcol.row); // unconst
   
   if (ok) switch (GetFieldType(GetFieldPtr(rowcol.col)))
   {
      case DBF_DATA_TYPE_INTEGER:
      {
         long n;
         ok = Read(rowcol.col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_FLOAT  :
      {
         double n;
         ok = Read(rowcol.col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_DATE   :
      {
         wxDateTime n;
         ok = Read(rowcol.col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_BOOLEAN:
      {
         bool n;
         ok = Read(rowcol.col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_MEMO   :
      case DBF_DATA_TYPE_CHAR   :
      {
         wxString str;
         /*ok = */Read(rowcol.col, &str);
         if (ok)
         {
            var->operator=(str);
         }
         break;
      }
      case DBF_DATA_TYPE_UNKNOWN:
      default:
         ok = false;
         break;
   }
   return ok;
}

// set value, call ValueChanged() afterwards!
bool wxDBase::SetValue(const wxVariant& var, const wxRowCol& rowcol)
{
   bool ok = true;
   if (ok) ok = SetPosition(rowcol.row);
   if (var.IsType(wxT("datetime")))
   {
      ok = Write(rowcol.col, var.GetDateTime());
   }
   else
   {
      ok = Write(rowcol.col, var.MakeString());
   }
   if (ok) ok = PutRecord(rowcol.row);
   return ok;
}

/*
void wxDBase::SetFilename(const wxString& WXUNUSED(filename))
{
   //m_filename = filename;
}

wxString wxDBase::GetFilename(void) const
{
   return wxEmptyString;//m_filename;
}
*/

bool wxDBase::Attach(wxInputStream* stream, bool editable, enum dbf_charconv conv, wxInputStream* memo)
{
   zlib_filefunc_def_s api;
   fill_filefunc(&api, stream);
   bool ok = base::Attach(stream, &api, editable, conv, memo);
   //if (ok) SetFilename(filename);
   return ok;
}


/////////////////////////////////////////////////////////////////////////////
