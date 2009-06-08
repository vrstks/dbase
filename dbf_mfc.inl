// dbf_mfc.inl

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
   bool ok = CDBase::Create(stream, api, array, array_count, charconv, memo);
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
   return CTime(CDBase::GetLastUpdate()).Format(_T("%x"));
}

// Load record from dBase file into memory
inline int CDbaseFile::GetRecord(size_t index)
{
   return SetPosition(index - 1) ? DBASE_SUCCESS : CDBase::GetLastError();
}

/* Write record to file (update record) */
inline int CDbaseFile::PutRecord(size_t index)
{
   return dbf_putrecord(m_handle, index - 1) ? DBASE_SUCCESS : CDBase::GetLastError();
}

/* Mark record as "DELETED" or "UNDELETED" */
inline int CDbaseFile::DeleteRecord(bool bDelete)
{
   return CDBase::DeleteRecord(bDelete) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::AddRecord()
{
   return CDBase::AddNew() ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::InsertRecord(size_t index)
{
   return ::dbf_insertrecord(m_handle, index) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline const DBF_FIELD* CDbaseFile::GetFieldPtr(size_t field)
{
   return CDBase::GetFieldPtr(field - 1);
}

inline const DBF_FIELD* CDbaseFile::GetFieldPtr(const char* field)
{
   return CDBase::GetFieldPtr(field);
}

inline int CDbaseFile::GetField(const DBF_FIELD* field, char* buf)
{
   return CDBase::GetField(field, buf, 255);
}

inline int CDbaseFile::GetField(size_t field, char* buf)
{
   return CDBase::GetField(field-1, buf, 255);
}

inline int CDbaseFile::GetField(const char* field, char* buf)
{
   return CDBase::GetField(field, buf, 255);
}

inline int CDbaseFile::GetFirstRecord()
{
	return CDBase::GetFirstRecord() ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::GetLastRecord()
{
	return CDBase::GetLastRecord() ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::GetNextRecord()
{
	return CDBase::GetNextRecord() ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::GetPrevRecord()
{
	return CDBase::GetPrevRecord() ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline size_t CDbaseFile::GetCurRecNo() const
{
   return CDBase::GetPosition() + 1;
}

inline CString CDbaseFile::GetCharField(const DBF_FIELD* field)
{
   CString str;
#if defined(UNICODE) || defined(_UNICODE)
   char temp[512];
   CDBase::Read(field, temp, sizeof(temp));
   str.operator=(temp);
#else
   CDBase::Read(field, str.GetBuffer(255), 255);
   str.ReleaseBuffer();
#endif
   return str;
}

inline CString CDbaseFile::GetCharField(const char* field)
{
   return GetCharField(GetFieldPtr(field));
}

inline CString CDbaseFile::GetCharField(size_t field)
{
   return GetCharField(GetFieldPtr(field));
}

inline int CDbaseFile::PutFloatField(const DBF_FIELD* field, double value)
{
   return CDBase::Write(field, value) ? DBASE_SUCCESS : CDBase::GetLastError();
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
   CDBase::Read(field, &n);
   return n;
}

inline long CDbaseFile::GetNumericField(size_t field)
{
   long n;
   CDBase::Read(field-1, &n);
   return n;
}

inline long CDbaseFile::GetNumericField(const char* field)
{
   long n;
   CDBase::Read(field, &n);
   return n;
}

inline double CDbaseFile::GetFloatField(const DBF_FIELD* field)
{
   double d;
   CDBase::Read(field, &d);
   return d;
}

inline double CDbaseFile::GetFloatField(size_t field)
{
   double d;
   CDBase::Read(field, &d);
   return d;
}

inline double CDbaseFile::GetFloatField(const char* field)
{
   double d;
   CDBase::Read(field, &d);
   return d;
}

inline bool CDbaseFile::GetLogicalField(const DBF_FIELD* field)
{
   bool b;
   CDBase::Read(field, &b);
   return b;
}

inline bool CDbaseFile::GetLogicalField(size_t field)
{
   bool b;
   CDBase::Read(field, &b);
   return b;
}

inline bool CDbaseFile::GetLogicalField(const char* field)
{
   bool b;
   CDBase::Read(field, &b);
   return b;
}

inline int CDbaseFile::PutField(const DBF_FIELD* field, const char* buf)
{
   return CDBase::Write(field, buf) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::PutField(const char* field, const char* buf)
{
   return PutField(GetFieldPtr(field), buf);
}

inline int CDbaseFile::PutField(size_t field, const char* buf)
{
   return PutField(GetFieldPtr(field), buf);
}

inline int CDbaseFile::PutNumericField(const DBF_FIELD* field, long value)
{
   return CDBase::Write(field, value) ? DBASE_SUCCESS : CDBase::GetLastError();
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
   return CDBase::Write(field, value) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::PutLogicalField(size_t field, bool value)
{
   return PutLogicalField(GetFieldPtr(field), value);
}

inline int CDbaseFile::PutLogicalField(const char* field, bool value)
{
   return PutLogicalField(GetFieldPtr(field), value) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline const char* CDbaseFile::GetLastError(void) const
{
   return CDBase::GetLastErrorStr();
}

inline int CDbaseFile::GetMemoField(size_t field, char* buf, size_t buf_len)
{
   CDBase::Read(GetFieldPtr(field), buf, buf_len);
   return CDBase::GetLastError();
}

inline size_t CDbaseFile::GetMemoFieldLength(size_t field)
{
   size_t len = CDBase::Read(GetFieldPtr(field), NULL, 0);
   return len ? (len + 1) : 0;
}

inline int CDbaseFile::PutMemoField(size_t field, const char* buf, size_t /*buf_len*/)
{
   CDBase::Write(GetFieldPtr(field), buf);
   return CDBase::GetLastError();
}

inline int CDbaseFile::PutMemoField(const char* field, const char* buf, size_t /*buf_len*/)
{
   CDBase::Write(GetFieldPtr(field), buf);
   return CDBase::GetLastError();
}

/*

inline int CDbaseFile::GetMemoField(const char* field, char* buf, size_t buf_len)
{
   return GetMemoField(GetFieldPtr(field), buf, buf_len);
}

inline int CDbaseFile::PutMemoField(const DBF_FIELD* field, const char* buf, size_t buf_len)
{
   return CDBase::PutMemoField(field, buf, buf_len) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline size_t CDbaseFile::GetMemoFieldLength(const char* field)
{
   return GetMemoFieldLength(GetFieldPtr(field));
}

inline size_t CDbaseFile::GetMemoFieldLength(const DBF_FIELD* field)
{
   return CDBase::GetMemoLength(field);
}

inline int CDbaseFile::GetMemoField(const DBF_FIELD* field, char* buf, size_t buf_len)
{
   return CDBase::GetMemoField(field, buf, buf_len) ? DBASE_SUCCESS : CDBase::GetLastError();
}

inline int CDbaseFile::ClearMemoField(const DBF_FIELD* field)
{
   return CDBase::ClearMemoField(field) ? DBASE_SUCCESS : CDBase::GetLastError();
}

*/

inline int CDbaseFile::ClearMemoField(size_t field)
{
   CDBase::Write(GetFieldPtr(field), "");
   return CDBase::GetLastError();
}

inline int CDbaseFile::ClearMemoField(const char* field)
{
   CDBase::Write(GetFieldPtr(field), "");
   return CDBase::GetLastError();
}

inline size_t CDbaseFile::SearchRecord(const char* field, const char* criteria, size_t nStartRec)
{
   return SearchRecord(GetFieldPtr(field), criteria, nStartRec);
}

inline size_t CDbaseFile::SearchRecord(size_t field, const char* criteria, size_t nStartRec)
{
   return SearchRecord(GetFieldPtr(field), criteria, nStartRec);
}

inline DBF_HANDLE CDbaseFile::Detach(void)
{
   m_strFileName.Empty();
   return CDBase::Detach();
}

inline void CDbaseFile::Close()
{
   CDBase::Close();
   m_strFileName.Empty();
}

inline int CDbaseFile::OpenMemoFile(const TCHAR* filename)
{
   USES_CONVERSION;
   zlib_filefunc_def api;
   GetFileAPI(&api);
   const CString str = GetMemoFileName(filename);
   void* stream = (*api.zopen_file)(api.opaque, T2CA(str), ZLIB_FILEFUNC_MODE_EXISTING | ZLIB_FILEFUNC_MODE_WRITE);
   return OpenMemo(stream) ? DBASE_SUCCESS : CDBase::GetLastError();
}

/*
inline int CDbaseFile::CreateMemoFile()
{
   USES_CONVERSION;
   zlib_filefunc_def api;
   GetFileAPI(&api);
	void* stream = (*api.zopen_file)(api.opaque, T2CA(GetMemoFileName()), ZLIB_FILEFUNC_MODE_CREATE | ZLIB_FILEFUNC_MODE_WRITE);
   return CreateMemo(stream) ? DBASE_SUCCESS : CDBase::GetLastError();
}
*/

inline bool CDbaseFile::Read(const DBF_FIELD* field, long* n)
{
   return CDBase::Read(field, n);
}

inline bool CDbaseFile::Read(const char* field, long* n)
{
   return CDBase::Read(field, n);
}

inline size_t CDbaseFile::Read(const DBF_FIELD* field, CString* str)
{
   size_t len = 512;
#ifdef _UNICODE
   char* temp = (char*)malloc(sizeof(char) * len);
   len = CDBase::Read(field, temp, len);
   str->operator=(temp);
   free(temp);
#else
   len = CDBase::Read(field, str->GetBuffer(len), len);
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
   return CDBase::Read(field, b);
}

inline bool CDbaseFile::Read(const char* field, bool* b)
{
   return CDBase::Read(field, b);
}

inline bool CDbaseFile::Read(size_t field, bool* b)
{
   return CDBase::Read(field, b);
}

inline bool CDbaseFile::PutRecord(void)
{
   return CDBase::PutRecord();
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool CDbaseFile::Write(size_t field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool CDbaseFile::Write(const char* field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool CDbaseFile::Write(const char* field, const char* str)
{
   return CDBase::Write(field, str);
}

inline bool CDbaseFile::Write(size_t field, const char* str)
{
   return CDBase::Write(field, str);
}

inline bool CDbaseFile::Read(size_t field     , long* value)
{
   return CDBase::Read(field, value);
}

inline bool CDbaseFile::Read(const DBF_FIELD* field, double* value)
{
   return CDBase::Read(field, value);
}

inline bool CDbaseFile::Read(const char* field, double* value)
{
   return CDBase::Read(field, value);
}

inline bool CDbaseFile::Read(size_t field     , double* value)
{
   return CDBase::Read(field, value);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, long value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Write(const char* field, long value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Write(size_t field, long value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Write(const DBF_FIELD* field, double value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Write(const char* field, double value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Write(size_t field, double value)
{
   return CDBase::Write(field, value);
}

inline bool CDbaseFile::Read(const DBF_FIELD* field, CTime* utc, int* ms)
{
   time_t temp = (time_t)utc->GetTime(); // MSVC7: GetTime() is 64bits, time_t is not
   bool ok = CDBase::ReadDateTime(field, &temp, ms);
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

inline bool CDbaseFile::Write(const DBF_FIELD* field, const CTime& utc, int ms)
{
   return CDBase::WriteDateTime(field, (time_t)utc.GetTime(), ms);
}

inline bool CDbaseFile::Write(const char* field, const CTime& utc, int ms)
{
   return Write(GetFieldPtr(field), utc, ms);
}

inline bool CDbaseFile::Write(size_t field, const CTime& utc, int ms)
{
   return Write(GetFieldPtr(field), utc, ms);
}

