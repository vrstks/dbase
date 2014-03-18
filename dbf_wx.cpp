// dbf_wx.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <vector>
#include <wx/wx.h>
#include <wx/variant.h>
#include <wx/docview.h>
#include <wx/filename.h>
#if (wxVERSION_NUMBER >= 2902)
#include <wx/versioninfo.h>
#endif

#include "wx/ext/trunk.h"

#include "ioapi/zlib.h"
#include "ioapi/ioapi.h"
#include "bool.h"
#include "dbf.h"
#include "dbf.hpp"
#include "dbf.inl"
#include "dbf_wx.h"
#include "dbf_wx.inl"
#include "wx/src/wxstreamc.h"

//static
const wxString wxDBase::FileExt = wxT(DBF_FILEEXT);

bool wxDBase::Attach(wxInputStream* stream, dbf_editmode editmode, dbf_charconv conv, wxInputStream* stream_memo, const wxString& tablename)
{
   zlib_filefunc_def_s api;
   bool ok;

   fill_filefunc(&api, stream);
   ok = base::Attach(stream, &api, editmode, conv, stream_memo, tablename.mb_str());
   if (ok)
   {
      m_stream      = stream;
      m_stream_memo = stream_memo;
   }
   return ok;
}

void wxDBase::Close()
{
   wxASSERT(IsOpen());
   base::Close();
   wxDELETE(m_stream);
   wxDELETE(m_stream_memo);
}

DBF_HANDLE wxDBase::Detach(void)
{
   wxASSERT(IsOpen());
   DBF_HANDLE handle = base::Detach();
   wxDELETE(m_stream);
   wxDELETE(m_stream_memo);
   return handle;
}

wxString wxDBase::GetColType(unsigned int col)
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
bool wxDBase::GetValueByRow(wxVariant* var, unsigned int row, unsigned int col)
{
   bool ok = SetPosition(row); // unconst

   if (ok) switch (GetFieldType(GetFieldPtr(col)))
   {
      case DBF_DATA_TYPE_INTEGER:
      {
         long n;

         ok = Read(col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_FLOAT  :
      {
         double n;

         ok = Read(col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_DATE   :
      {
         wxDateTime n;

         ok = Read(col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_BOOLEAN:
      {
         bool n;

         ok = Read(col, &n);
         if (ok) var->operator=(n);
         break;
      }
      case DBF_DATA_TYPE_MEMO   :
      case DBF_DATA_TYPE_CHAR   :
      {
         wxString str;

         /*ok = */Read(col, &str);
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
bool wxDBase::SetValueByRow(const wxVariant& var, unsigned int row, unsigned int col)
{
   bool ok = true;

   if (ok) ok = SetPosition(row);
   if (var.IsType(wxT("datetime")))
   {
      ok = Write(col, var.GetDateTime());
   }
   else
   {
      ok = Write(col, var.MakeString());
   }
   if (ok)
   {
      ok = PutRecord(row);
   }
   return ok;
}

#ifdef _WX_VERSIONINFO_H_
/*static*/ wxVersionInfo wxDBase::GetVersionInfo()
{
   return wxVersionInfo(wxT(DBF_LIB_NAME), DBF_MAJOR_VERSION, DBF_MINOR_VERSION, DBF_SVN_VERSION,
       wxT("dbase interface library"),
       wxT("Copyright (c) 2007-2012 by Troels K")
       );
}
#endif

/////////////////////////////////////////////////////////////////////////////
