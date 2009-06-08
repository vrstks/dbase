// msaccess.cpp
// Copyright (c) 2007-2008 by Troels K. All rights reserved.

/* 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "stdafx.h"
#include "..\..\dbf_mfc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDaoConverter

class CDaoConverter : public CDaoRecordset
{
public:
   CDaoConverter(CDaoDatabase* db, const TCHAR* tablename) : CDaoRecordset(db)
   {
		CString strQuery;
      strQuery.Format(_T("SELECT * FROM [%s]"), tablename);
      Open(AFX_DAO_USE_DEFAULT_TYPE, strQuery);
   }
   bool Loop()
   {
      size_t row = 0;
      for (MoveFirst(); !IsEOF(); MoveNext(), row++)
      {
         //TRACE(_T("row %3d\n"), row);
         NewRecord();
         for (int i = 0; i < GetFieldCount(); i++)
         {
            COleVariant var;
      	   GetFieldValue((size_t)i, var);
            SaveField(i, var);
         }
         SaveRecord();
      }
      return true;
   }
   virtual ~CDaoConverter() {}
   virtual void NewRecord() = 0;
   virtual void SaveField(size_t field, const COleVariant&) = 0;
   virtual void SaveRecord() = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CDaoConverter_DBF

class CDaoConverter_DBF : public CDaoConverter
{
public:
   CDbaseFile m_dbf;
   CDaoConverter_DBF(CDaoDatabase* db, const TCHAR* tablename) : CDaoConverter(db, tablename) {}

   bool Create(const TCHAR* file)
   {
      USES_CONVERSION;
      int array_count = GetFieldCount();
      DBF_FIELD_INFO* array = new DBF_FIELD_INFO[array_count];

	   for (int i = 0; i < array_count; i++)
      {
         CDaoFieldInfo info;
	      GetFieldInfo(i, info);
         DBF_FIELD_INFO* item = array + i;
         strncpy(item->name, T2CA(info.m_strName), _countof(item->name));
         item->name[_countof(item->name) - 1] = 0;
         item->decimals = 0;
         item->length = info.m_lSize;
         //TRACE(_T("Field %d %s\n"), i, info.m_strName.operator LPCTSTR());

         switch (info.m_nType)
         {
            case dbDate:
               item->type = DBF_DATA_TYPE_DATE;
               break;
            case dbDecimal:
            case dbFloat:
            case dbSingle:
            case dbDouble:
               item->type = DBF_DATA_TYPE_FLOAT;
               break;
            case dbText:
            case dbMemo:
            case dbChar:
               item->type = DBF_DATA_TYPE_CHAR;
               if (0 == item->length) item->length = 80;
               break;
            case dbInteger:
            case dbLong:
            case dbByte:
            case dbNumeric:
            case dbBigInt:
               item->type = DBF_DATA_TYPE_INTEGER;
               break;
            case dbBoolean:
               item->type = DBF_DATA_TYPE_BOOLEAN;
               break;
            case dbCurrency:
            case dbBinary:
            case dbLongBinary:
            case dbGUID:
            case dbVarBinary:
            case dbTime:
            case dbTimeStamp:
            default:
               _asm NOP
               return false;
         }
      }

      zlib_filefunc_def api;
      fill_fopen_filefunc(&api);
      void* stream = api.zopen_file(api.opaque, T2CA(file), ZLIB_FILEFUNC_MODE_CREATE | ZLIB_FILEFUNC_MODE_WRITE);
      bool ok = (NULL != stream);
      if (ok)
      {
         ok = m_dbf.Create(file, stream, &api, array, array_count);
      }
      delete [] array;
      return ok;
   }
   
   virtual ~CDaoConverter_DBF() {}
   virtual void NewRecord()
   {
      m_dbf.AddNew();
   }
   virtual void SaveField(size_t field, const COleVariant& var)
   {
      switch (var.vt)
      {
         case VT_BOOL:
         {
            bool b = (var.boolVal != 0);
            m_dbf.Write(field, b);
            break;
         }
         case VT_UI1:
            m_dbf.Write(field, (long)var.bVal);
            break;
         case VT_DATE:
         {
            COleDateTime dt(var.date);
            m_dbf.WriteDate(field, dt.GetYear(), dt.GetMonth(), dt.GetDay());
            break;
         }
         case VT_R4:
            m_dbf.Write(field, var.fltVal);
            break;
         case VT_R8:
            m_dbf.Write(field, var.dblVal);
            break;
         case VT_I2:
            m_dbf.Write(field, (long)var.iVal);
            break;
         case VT_I4:
            m_dbf.Write(field, (long)var.lVal);
            break;
         case VT_BSTR:
         {
         #ifdef _UNICODE // T2CA() gets worn down by big databases, use wcstombs
            char sz[200];
            wcstombs(sz, (LPCTSTR)var.bstrVal, _countof(sz));
            m_dbf.Write(field, sz);
         #else
            m_dbf.Write(field, (LPCTSTR)var.bstrVal);
         #endif
            break;
         }
         default:
            _asm NOP
            break;
      }
   }
   virtual void SaveRecord()
   {
      m_dbf.PutRecord();
   }
};

/////////////////////////////////////////////////////////////////////////////
// CDaoConverter_CSV

class CDaoConverter_CSV : public CDaoConverter
{
public:
   FILE* m_stream;
   CString m_row;
   TCHAR m_sep;
   CDaoConverter_CSV(CDaoDatabase* db, const TCHAR* tablename) : CDaoConverter(db, tablename), m_stream(NULL)
   {
      TCHAR sep[2] = _T(";");
      ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLIST, sep, _countof(sep));
      m_sep = sep[0];
   }

   bool Create(const TCHAR* file)
   {
	   m_stream = _tfopen(file, _T("wt"));
      bool ok = (NULL != m_stream);
      {
         CString str;
         for (int i = 0; i < GetFieldCount(); i++)
         {
            CDaoFieldInfo info;
	         GetFieldInfo(i, info);
            if (i) str+=m_sep;
            str+=info.m_strName;
         }
         str+=_T("\n");
         _fputts(str, m_stream);
      }
      return ok;
   }

   virtual ~CDaoConverter_CSV()
   {
      if (m_stream) fclose(m_stream);
   }
   virtual void NewRecord()
   {
      m_row.Empty();
   }
   virtual void SaveField(size_t field, const COleVariant& var)
   {
      CString str;
      switch (var.vt)
      {
         case VT_BOOL:
            str = var.boolVal ? _T("1") : _T("0");
            break;
         case VT_UI1:
            str.Format(_T("%d"), (int)var.bVal);
            break;
         case VT_DATE:
         {
            COleDateTime dt(var.date);
            str = dt.Format();
            break;
         }
         case VT_R4:
            str.Format(_T("%g"), var.fltVal);
            break;
         case VT_R8:
            str.Format(_T("%g"), var.dblVal);
            break;
         case VT_I2:
            str.Format(_T("%d"), (int)var.iVal);
            break;
         case VT_I4:
            str.Format(_T("%d"), (int)var.lVal);
            break;
         case VT_BSTR:
            str = (LPCTSTR)var.bstrVal;
            break;
         default:
            _asm NOP
            break;
      }
      if (field) m_row+=m_sep;
      m_row+=str;
   }
   virtual void SaveRecord()
   {
      m_row.Replace('\n', ' ');
      m_row.Replace('\r', ' ');
      m_row.Replace('\t', ' ');
      m_row+=_T("\n");
      _fputts(m_row, m_stream);
   }
};

/////////////////////////////////////////////////////////////////////////////

static bool OpenFileSaveDialog(CWnd* parent, const TCHAR* src, const TCHAR* tablename, CString* str_ptr, bool* dbf)
{
   TCHAR drive[_MAX_PATH];
   TCHAR dir[_MAX_PATH];
   TCHAR dst[_MAX_PATH];
 	static TCHAR szFilters[] = 
      _T("dBase Files (*.dbf)|*.dbf|")
      _T("Comma Delimited Text Files (*.csv)|*.csv|")
      _T("|")
      ;

   _tsplitpath(src, drive, dir, NULL, NULL);
   _tmakepath(dst, drive, dir, tablename, NULL);
	CFileDialog dlg(FALSE, _T(FILEEXT_DBASE), dst, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, parent);
	dlg.m_ofn.lpstrTitle = _T("Save As");

	bool ok = (dlg.DoModal() == IDOK);
   if (ok)
	{
      if (dbf) *dbf = (0 == _tcsicmp(dlg.GetFileExt(), _T(FILEEXT_DBASE)));
      str_ptr->operator=(dlg.GetPathName());
   }
   return ok;
}

extern bool DoModal_Pick(CWnd* parent, const CString& caption, const CStringArray& as, CString* sel);

bool mdb2dbf(CWnd* parent, const TCHAR* src, CString* newfile)
{
   CDaoDatabase db;
   bool ok = true;
   try
   {
      db.Open(src, false, true);

      CStringArray as;
	   for (int i = 0; i < db.GetTableDefCount(); i++)
      {
         CDaoTableDefInfo info;
	      db.GetTableDefInfo(i, info);
         as.Add(info.m_strName);      
      }
      CString tablename;
      ok = DoModal_Pick(parent, _T("Select table"), as, &tablename);
      if (ok)
      {
         CString str;
         bool dbf;
         ok = OpenFileSaveDialog(parent, src, tablename, &str, &dbf);
         if (ok)
         {
            CWaitCursor wc;
            if (dbf)
            {
               CDaoConverter_DBF table(&db, tablename);
               ok = table.Create(str);
               if (ok) ok = table.Loop();
               if (ok && newfile) newfile->operator=(str);
            }
            else // csv
            {
               CDaoConverter_CSV table(&db, tablename);
               ok = table.Create(str);
               if (ok) ok = table.Loop();
            }
	      }
      }
   }
   catch(CDaoException*)
   {
      ok = false;
   }
   return ok;
}

/////////////////////////////////////////////////////////////////////////////
