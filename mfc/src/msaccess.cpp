// msaccess.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.

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
      if (!(IsBOF() && IsEOF())) for (MoveFirst(); !IsEOF(); MoveNext(), row++)
      {
         //TRACE(_T("row %3d\n"), row);
         NewRecord();
         for (int i = 0; i < GetFieldCount(); i++)
         {
            COleVariant var;
      	   GetFieldValue(i, var);
            SaveField(i, var);
         }
         SaveRecord();
      }
      return true;
   }
   virtual ~CDaoConverter() {}
   virtual void NewRecord() = 0;
   virtual bool SaveField(size_t field, const COleVariant&) = 0;
   virtual void SaveRecord() = 0;
};

/////////////////////////////////////////////////////////////////////////////
// CDaoConverter_DBF

class CDaoConverter_DBF : public CDaoConverter
{
public:
   CDbaseFile m_dbf;
   CDaoConverter_DBF(CDaoDatabase* db, const TCHAR* tablename) : CDaoConverter(db, tablename) {}

   enum dbf_data_type GetFieldType(int field, const CDaoFieldInfo& info)
   {
      enum dbf_data_type type;
      switch (info.m_nType)
      {
         case dbDate:
         {
            bool date = false;
            bool time = false;
            if (!(IsBOF() && IsEOF())) for (MoveFirst(); !IsEOF(); MoveNext())
            {
               COleVariant var;
      	      GetFieldValue(field, var);
               if (var.vt == VT_DATE)
               {
                  COleDateTime dt(var.date);
                  date = date || (dt.GetYear() != 1899);
                  time = time || dt.GetHour() || dt.GetMinute() || dt.GetSecond();
               }
            }
            if (date && time)
            {
               type = DBF_DATA_TYPE_DATETIME;
            }
            else
            {
               type = date ? DBF_DATA_TYPE_DATE : DBF_DATA_TYPE_TIME;
            }
            break;
         }
         case dbDecimal:
         case dbFloat:
         case dbSingle:
         case dbDouble:
            type = DBF_DATA_TYPE_FLOAT;
            break;
         case dbText:
         case dbMemo:
         case dbChar:
            type = DBF_DATA_TYPE_CHAR;
            break;
         case dbInteger:
         case dbLong:
         case dbByte:
         case dbNumeric:
         case dbBigInt:
            type = DBF_DATA_TYPE_INTEGER;
            break;
         case dbBoolean:
            type = DBF_DATA_TYPE_BOOLEAN;
            break;
         case dbCurrency:
         case dbBinary:
         case dbLongBinary:
         case dbGUID:
         case dbVarBinary:
         case dbTime:
         case dbTimeStamp:
         default:
            type = DBF_DATA_TYPE_UNKNOWN;
            break;
      }
      return type;
   }

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
         item->type = GetFieldType(i, info);
         if ( (item->type == DBF_DATA_TYPE_CHAR) && (0 == item->length))
         {
            item->length = 80;
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
   virtual bool SaveField(size_t field, const COleVariant& var)
   {
      bool ok = false;
      switch (var.vt)
      {
         case VT_BOOL:
         {
            bool b = (var.boolVal != 0);
            ok = m_dbf.Write(field, b);
            break;
         }
         case VT_UI1:
            ok = m_dbf.Write(field, (long)var.bVal);
            break;
         case VT_DATE:
         {
            COleDateTime dt(var.date);
            ok = m_dbf.Write(field, dt);
            break;
         }
         case VT_R4:
            ok = m_dbf.Write(field, var.fltVal);
            break;
         case VT_R8:
            ok = m_dbf.Write(field, var.dblVal);
            break;
         case VT_I2:
            ok = m_dbf.Write(field, (long)var.iVal);
            break;
         case VT_I4:
            ok = m_dbf.Write(field, (long)var.lVal);
            break;
         case VT_BSTR:
         {
         #ifdef _UNICODE // T2CA() gets worn down by big databases, use wcstombs
            char sz[200];
            wcstombs(sz, (LPCTSTR)var.bstrVal, _countof(sz));
            ok = m_dbf.Write(field, sz);
         #else
            ok = m_dbf.Write(field, (LPCTSTR)var.bstrVal);
         #endif
            break;
         }
         default:
            _asm NOP
            break;
      }
      return ok;
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
   virtual bool SaveField(size_t field, const COleVariant& var)
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
      return true;
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

static bool mdb2dbf(CDaoDatabase* db, const TCHAR* tablename, CWnd* parent, const TCHAR* src, CString* newfile)
{
   CString str;
   bool dbf;
   bool ok = OpenFileSaveDialog(parent, src, tablename, &str, &dbf);
   if (ok)
   {
      CWaitCursor wc;
      if (dbf)
      {
         CDaoConverter_DBF table(db, tablename);
         ok = table.Create(str);
         if (ok) ok = table.Loop();
         if (ok && newfile) newfile->operator=(str);
      }
      else // csv
      {
         CDaoConverter_CSV table(db, tablename);
         ok = table.Create(str);
         if (ok) ok = table.Loop();
      }
   }
   return ok;
}

bool mdb2dbf(CWnd* parent, const TCHAR* src, CStringArray* newfilearray, const CString& rtablename)
{
   CDaoDatabase db;
   bool ok = true;
   try
   {
      db.Open(src, false, true);

      bool all = false;
      CStringArray as;
      CString tablename = rtablename;
      if (tablename.IsEmpty())
      {
	      for (int i = 0; i < db.GetTableDefCount(); i++)
         {
            CDaoTableDefInfo info;
	         db.GetTableDefInfo(i, info);
            tablename = info.m_strName;
            if (tablename.Left(4) != _T("MSys"))
            {
               as.Add(tablename);
            }
         }
         all = (IDYES == AfxMessageBox(_T("Convert all ?"), MB_YESNO | MB_ICONQUESTION));
         if (!all)
         {
            ok = DoModal_Pick(parent, _T("Select table"), as, &tablename);
         }
      }
      if (ok)
      {
         if (all)
         {
            for (int i = 0; ok && (i < as.GetSize()); i++)
            {
               tablename = as.ElementAt(i);
               CString newfile;
               ok = mdb2dbf(&db, tablename, parent, src, &newfile);
               if (ok) newfilearray->Add(newfile);
            }
         }
         else
         {
            CString newfile;
            ok = mdb2dbf(&db, tablename, parent, src, &newfile);
            if (ok) newfilearray->Add(newfile);
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
