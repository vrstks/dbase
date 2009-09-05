// dbf_mfc.inl
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

inline CDbaseFile::CDbaseFile() : CObject(), CDBase()
{
}

inline CDbaseFile::~CDbaseFile()
{
}

/* Open dBase file, read dbf header and fill field list */
inline int CDbaseFile::Open(const TCHAR* filename, bool editable, enum dbf_charconv charconv)
{
   USES_CONVERSION;
   m_handle = dbf_open(T2CA(filename), NULL, editable, charconv);
   if (m_handle) m_strFileName = filename;
   return m_handle ? DBASE_SUCCESS : DBASE_NO_FILE;
}

inline bool CDbaseFile::Create(const TCHAR* lpszFileName, 
   void* stream, struct zlib_filefunc_def_s* api, 
   const DBF_FIELD_INFO* array, size_t array_count, 
   enum dbf_charconv charconv, void* memo)
{
   bool ok = base::Create(stream, api, array, array_count, charconv, memo);
   if (ok) m_strFileName = lpszFileName;
   return ok;
}

inline const CString& CDbaseFile::GetFilename(void) const
{
   return m_strFileName;
}

// Get last update date
inline CString CDbaseFile::GetLastUpdate()
{
   return CTime(base::GetLastUpdate()).Format(_T("%x"));
}

// Load record from dBase file into memory
inline int CDbaseFile::GetRecord(size_t index)
{
   return SetPosition(index - 1) ? DBASE_SUCCESS : base::GetLastError();
}

/* Write record to file (update record) */
inline int CDbaseFile::PutRecord(size_t index)
{
   return dbf_putrecord(m_handle, index - 1) ? DBASE_SUCCESS : base::GetLastError();
}

/* Mark record as "DELETED" or "UNDELETED" */
inline int CDbaseFile::DeleteRecord(bool bDelete)
{
   return base::DeleteRecord(bDelete) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::AddRecord()
{
   return base::AddNew() ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::InsertRecord(size_t index)
{
   return ::dbf_insertrecord(m_handle, index) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::GetFirstRecord()
{
	return base::GetFirstRecord() ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::GetLastRecord()
{
	return base::GetLastRecord() ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::GetNextRecord()
{
	return base::GetNextRecord() ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::GetPrevRecord()
{
	return base::GetPrevRecord() ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::PutFloatField(const DBF_FIELD* field, double value)
{
   return base::Write(field, value) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::PutFloatField(size_t field, double value)
{
   return PutFloatField(GetFieldPtr(field), value);
}

inline int CDbaseFile::PutFloatField(const char* field, double value)
{
   return PutFloatField(GetFieldPtr(field), value);
}

inline long CDbaseFile::GetNumericField(const DBF_FIELD* field)
{
   long n;
   base::Read(field, &n);
   return n;
}

inline long CDbaseFile::GetNumericField(size_t field)
{
   long n;
   base::Read(field-1, &n);
   return n;
}

inline long CDbaseFile::GetNumericField(const char* field)
{
   long n;
   base::Read(field, &n);
   return n;
}

inline double CDbaseFile::GetFloatField(const DBF_FIELD* field)
{
   double d;
   base::Read(field, &d);
   return d;
}

inline double CDbaseFile::GetFloatField(size_t field)
{
   double d;
   base::Read(field, &d);
   return d;
}

inline double CDbaseFile::GetFloatField(const char* field)
{
   double d;
   base::Read(field, &d);
   return d;
}

inline bool CDbaseFile::GetLogicalField(const DBF_FIELD* field)
{
   bool b;
   base::Read(field, &b);
   return b;
}

inline bool CDbaseFile::GetLogicalField(size_t field)
{
   bool b;
   base::Read(field, &b);
   return b;
}

inline bool CDbaseFile::GetLogicalField(const char* field)
{
   bool b;
   base::Read(field, &b);
   return b;
}

inline int CDbaseFile::PutNumericField(const DBF_FIELD* field, long value)
{
   return base::Write(field, value) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::PutNumericField(size_t field, long value)
{
   return PutNumericField(GetFieldPtr(field), value);
}

inline int CDbaseFile::PutNumericField(const char* field, long value)
{
   return PutNumericField(GetFieldPtr(field), value);
}

inline int CDbaseFile::PutLogicalField(const DBF_FIELD* field, bool value)
{
   return base::Write(field, value) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::PutLogicalField(size_t field, bool value)
{
   return PutLogicalField(GetFieldPtr(field), value);
}

inline int CDbaseFile::PutLogicalField(const char* field, bool value)
{
   return PutLogicalField(GetFieldPtr(field), value) ? DBASE_SUCCESS : base::GetLastError();
}

inline const char* CDbaseFile::GetLastError(void) const
{
   return base::GetLastErrorStr();
}

inline int CDbaseFile::GetMemoField(size_t field, char* buf, size_t buf_len)
{
   base::Read(GetFieldPtr(field), buf, buf_len);
   return base::GetLastError();
}

inline size_t CDbaseFile::GetMemoFieldLength(size_t field)
{
   size_t len = base::Read(GetFieldPtr(field), (char*)NULL, 0);
   return len ? (len + 1) : 0;
}

/*

inline int CDbaseFile::GetMemoField(const char* field, char* buf, size_t buf_len)
{
   return GetMemoField(GetFieldPtr(field), buf, buf_len);
}

inline int CDbaseFile::PutMemoField(const DBF_FIELD* field, const char* buf, size_t buf_len)
{
   return base::PutMemoField(field, buf, buf_len) ? DBASE_SUCCESS : base::GetLastError();
}

inline size_t CDbaseFile::GetMemoFieldLength(const char* field)
{
   return GetMemoFieldLength(GetFieldPtr(field));
}

inline size_t CDbaseFile::GetMemoFieldLength(const DBF_FIELD* field)
{
   return base::GetMemoLength(field);
}

inline int CDbaseFile::GetMemoField(const DBF_FIELD* field, char* buf, size_t buf_len)
{
   return base::GetMemoField(field, buf, buf_len) ? DBASE_SUCCESS : base::GetLastError();
}

inline int CDbaseFile::ClearMemoField(const DBF_FIELD* field)
{
   return base::ClearMemoField(field) ? DBASE_SUCCESS : base::GetLastError();
}

*/

inline int CDbaseFile::ClearMemoField(size_t field)
{
   base::Write(GetFieldPtr(field), "");
   return base::GetLastError();
}

inline int CDbaseFile::ClearMemoField(const char* field)
{
   base::Write(GetFieldPtr(field), "");
   return base::GetLastError();
}

inline size_t CDbaseFile::SearchRecord(const char* field, const TCHAR* criteria, size_t nStartRec)
{
   return SearchRecord(GetFieldPtr(field), criteria, nStartRec);
}

inline size_t CDbaseFile::SearchRecord(size_t field, const TCHAR* criteria, size_t nStartRec)
{
   return SearchRecord(GetFieldPtr(field), criteria, nStartRec);
}

inline DBF_HANDLE CDbaseFile::Detach(void)
{
   m_strFileName.Empty();
   return base::Detach();
}

inline void CDbaseFile::Close()
{
   base::Close();
   m_strFileName.Empty();
}

inline int CDbaseFile::OpenMemoFile(const TCHAR* filename)
{
   USES_CONVERSION;
   zlib_filefunc_def api;
   GetFileAPI(&api);
   const CString str = GetMemoFileName(filename);
   void* stream = (*api.zopen_file)(api.opaque, T2CA(str), ZLIB_FILEFUNC_MODE_EXISTING | ZLIB_FILEFUNC_MODE_WRITE);
   return OpenMemo(stream) ? DBASE_SUCCESS : base::GetLastError();
}

/*
inline int CDbaseFile::CreateMemoFile()
{
   USES_CONVERSION;
   zlib_filefunc_def api;
   GetFileAPI(&api);
	void* stream = (*api.zopen_file)(api.opaque, T2CA(GetMemoFileName()), ZLIB_FILEFUNC_MODE_CREATE | ZLIB_FILEFUNC_MODE_WRITE);
   return CreateMemo(stream) ? DBASE_SUCCESS : base::GetLastError();
}
*/

inline bool CDbaseFile::Read(const DBF_FIELD* field, long* n)
{
   return base::Read(field, n);
}

inline bool CDbaseFile::Read(const char* field, long* n)
{
   return base::Read(field, n);
}

inline size_t CDbaseFile::Read(const DBF_FIELD* field, CString* str)
{
   size_t len = 512;
#ifdef _UNICODE
   char* temp = (char*)malloc(sizeof(char) * len);
   len = base::Read(field, temp, len);
   str->operator=(temp);
   free(temp);
#else
   len = base::Read(field, str->GetBuffer(len), len);
   str->ReleaseBuffer();
#endif
   return len;
}

inline size_t CDbaseFile::Read(const char* field, CString* str)
{
   return Read(GetFieldPtr(field), str);
}

inline size_t CDbaseFile::Read(size_t field, CString* str)
{
   return Read(GetFieldPtr(field), str);
}

inline bool CDbaseFile::Read(const DBF_FIELD* field, bool* b)
{
   return base::Read(field, b);
}

inline bool CDbaseFile::Read(const char* field, bool* b)
{
   return base::Read(field, b);
}

inline bool CDbaseFile::Read(size_t field, bool* b)
{
   return base::Read(field, b);
}

inline bool CDbaseFile::PutRecord(void)
{
   return base::PutRecord();
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, const bool& b)
{
   return base::Write(field, b);
}

inline bool CDbaseFile::Write(size_t field, const bool& b)
{
   return base::Write(field, b);
}

inline bool CDbaseFile::Write(const char* field, const bool& b)
{
   return base::Write(field, b);
}

/*
inline CString CDbaseFile::GetCharField(const DBF_FIELD* field)
{
   CString str;
#if defined(UNICODE) || defined(_UNICODE)
   char temp[512];
   base::Read(field, temp, sizeof(temp));
   str.operator=(temp);
#else
   base::Read(field, str.GetBuffer(255), 255);
   str.ReleaseBuffer();
#endif
   return str;
}
*/

inline bool CDbaseFile::Write(const DBF_FIELD* field, const TCHAR* str)
{
   USES_CONVERSION;
   return base::Write(field, T2CA(str));
}


inline bool CDbaseFile::Write(const char* field, const TCHAR* str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool CDbaseFile::Write(size_t field, const TCHAR* str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool CDbaseFile::Read(size_t field     , long* value)
{
   return base::Read(field, value);
}

inline bool CDbaseFile::Read(const DBF_FIELD* field, double* value)
{
   return base::Read(field, value);
}

inline bool CDbaseFile::Read(const char* field, double* value)
{
   return base::Read(field, value);
}

inline bool CDbaseFile::Read(size_t field     , double* value)
{
   return base::Read(field, value);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, long value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Write(const char* field, long value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Write(size_t field, long value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, double value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Write(const char* field, double value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Write(size_t field, double value)
{
   return base::Write(field, value);
}

inline bool CDbaseFile::Read(const DBF_FIELD* field, CTime* utc, int* ms)
{
   time_t temp = (time_t)utc->GetTime(); // MSVC7: GetTime() is 64bits, time_t is not
   bool ok = base::ReadDateTime(field, &temp, ms);
   if (ok) utc->operator=(temp);
   return ok;
}

inline bool CDbaseFile::Read(const char* field, CTime* utc, int* ms)
{
   return Read(GetFieldPtr(field), utc, ms);
}

inline bool CDbaseFile::Read(size_t field, CTime* utc, int* ms)
{
   return Read(GetFieldPtr(field), utc, ms);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, const SYSTEMTIME& st)
{
   return base::Write(field, st);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, const CTime& utc, int ms)
{
   return base::WriteTime(field, (time_t)utc.GetTime(), ms);
}

inline bool CDbaseFile::Write(const char* field, const CTime& utc, int ms)
{
   return Write(GetFieldPtr(field), utc, ms);
}

inline bool CDbaseFile::Write(size_t field, const CTime& utc, int ms)
{
   return Write(GetFieldPtr(field), utc, ms);
}

inline bool CDbaseFile::Write(size_t field, const COleDateTime& dt)
{
   return Write(GetFieldPtr(field), dt);
}

