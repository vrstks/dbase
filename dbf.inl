// dbf.inl
// Copyright (c) 2007-2008 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

inline CDBase::CDBase() : m_handle(NULL)
{
}

inline CDBase::~CDBase()
{
	if (m_handle) Close();
}

inline bool CDBase::IsOpen(void) const
{ 
   return (m_handle != NULL);
}

inline bool CDBase::Attach(void* stream, struct zlib_filefunc_def_s* api, bool editable, enum dbf_charconv conv, void* memo)
{
   m_handle = ::dbf_attach(stream, api, editable, conv, memo);
   return (m_handle != NULL);
}

inline bool CDBase::Attach(DBF_HANDLE handle)
{
   m_handle = handle;
   return (m_handle != NULL);
}

inline bool CDBase::Attach(CDBase* db)
{
   return Attach(db->Detach());
}

inline DBF_HANDLE CDBase::Detach(void)
{
   DBF_HANDLE handle = m_handle;
   m_handle = NULL;
   return handle;
}

inline bool CDBase::Open(const char* filename, struct zlib_filefunc_def_s* api, bool editable, enum dbf_charconv charconv)
{
   m_handle = ::dbf_open(filename, api, editable, charconv);
   return (m_handle != NULL);
}

inline bool CDBase::Open(const char* filename, bool editable, enum dbf_charconv charconv)
{
   return Open(filename, NULL, editable, charconv);
}

inline bool CDBase::Create(void* stream, struct zlib_filefunc_def_s* api, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv, void* memo)
{
   m_handle = ::dbf_create_attach(stream, api, array, array_count, charconv, memo);
   return (m_handle != NULL);
}

inline bool CDBase::Create(const char* filename, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv)
{
   m_handle = ::dbf_create(filename, array, array_count, charconv);
   return (m_handle != NULL);
}

inline void CDBase::Close()
{
   ::dbf_close(&m_handle);
}

inline void CDBase::CloseMemoFile()
{
   ::dbf_close_memo(m_handle);
}

inline size_t CDBase::GetPosition() const
{ 
   return ::dbf_getposition(m_handle); 
}

inline size_t CDBase::GetRecordCount() const
{ 
   return ::dbf_getrecordcount(m_handle); 
}

inline size_t CDBase::GetFieldCount() const 
{ 
   return ::dbf_getfieldcount(m_handle);
}

inline bool CDBase::SetPosition(size_t index)
{
   return ::dbf_setposition(m_handle, index) ? true : false;
}

inline bool CDBase::PutRecord(size_t index)
{
   return ::dbf_putrecord(m_handle, index) ? true : false;
}

inline bool CDBase::PutRecord(void)
{
   return PutRecord(GetPosition());
}

inline size_t CDBase::Read(const DBF_FIELD* field, char* buf, size_t buf_len)
{
   return ::dbf_getfield(m_handle, field, buf, buf_len, DBF_DATA_TYPE_ANY);
}

inline size_t CDBase::Read(const char* field, char* buf, size_t buf_len)
{
   return ::dbf_getfield(m_handle, GetFieldPtr(field), buf, buf_len, DBF_DATA_TYPE_ANY);
}

inline bool CDBase::GetFirstRecord()
{
   bool ok = ::dbf_move_prepare(m_handle) ? true : false;
	if (ok) ok = SetPosition(0);
   return ok;
}

inline bool CDBase::GetLastRecord()
{
   bool ok = ::dbf_move_prepare(m_handle) ? true : false;
	if (ok) ok = SetPosition(GetRecordCount()-1);
   return ok;
}

inline bool CDBase::GetNextRecord()
{
   bool ok = ::dbf_move_prepare(m_handle) ? true : false;
   if (ok) ok = SetPosition(GetPosition() + 1);
   return ok;
}

inline bool CDBase::GetPrevRecord()
{
   bool ok = ::dbf_move_prepare(m_handle) ? true : false;
   if (ok) ok = SetPosition(GetPosition() - 1);
   return ok;
}

inline bool CDBase::ReadDate(const DBF_FIELD* field, int* year, int* month, int* mday)
{
   return ::dbf_getfield_date(m_handle, field, year, month, mday) ? true : false;
}

inline bool CDBase::ReadDate(const char* field, int* year, int* month, int* mday)
{
   return ReadDate(GetFieldPtr(field), year, month, mday);
}

inline bool CDBase::ReadDate(size_t field, int* year, int* month, int* mday)
{
   return ReadDate(GetFieldPtr(field), year, month, mday);
}

inline bool CDBase::ReadDateTime(const DBF_FIELD* field, struct tm* tm, int* ms)
{
   return ::dbf_getfield_tm(m_handle, field, tm, ms) ? true : false;
}

inline bool CDBase::ReadDateTime(const char* field, struct tm* tm, int* ms)
{
   return ReadDateTime(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::ReadDateTime(size_t field, struct tm* tm, int* ms)
{
   return ReadDateTime(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::ReadDateTime(const DBF_FIELD* field, time_t* utc, int* ms)
{
   return ::dbf_getfield_time(m_handle, field, utc, ms) ? true : false;
}

inline bool CDBase::ReadDateTime(const char* field, time_t* utc, int* ms)
{
   return ReadDateTime(GetFieldPtr(field), utc, ms);
}

inline bool CDBase::ReadDateTime(size_t field, time_t* utc, int* ms)
{
   return ReadDateTime(GetFieldPtr(field), utc, ms);
}

inline bool CDBase::WriteDate(const DBF_FIELD* field, int year, int month, int mday)
{
   return ::dbf_putfield_date(m_handle, field, year, month, mday) ? true : false;
}

inline bool CDBase::WriteDate(const char* field, int year, int month, int mday)
{
   return WriteDate(GetFieldPtr(field), year, month, mday);
}

inline bool CDBase::WriteDate(size_t field, int year, int month, int mday)
{
   return WriteDate(GetFieldPtr(field), year, month, mday);
}

inline bool CDBase::WriteDateTime(const DBF_FIELD* field, const struct tm* tm, int ms)
{
   return ::dbf_putfield_tm(m_handle, field, tm, ms) ? true : false;
}

inline bool CDBase::WriteDateTime(const char* field, const struct tm* tm, int ms)
{
   return WriteDateTime(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::WriteDateTime(size_t field, const struct tm* tm, int ms)
{
   return WriteDateTime(GetFieldPtr(field), tm, ms);
}

inline bool CDBase::WriteDateTime(const DBF_FIELD* field, const time_t& utc, int ms)
{
   return ::dbf_putfield_time(m_handle, field, utc, ms) ? true : false;
}

inline bool CDBase::WriteDateTime(const char* field, const time_t& utc, int ms)
{
   return WriteDateTime(GetFieldPtr(field), utc, ms);
}

inline bool CDBase::WriteDateTime(size_t field, const time_t& utc, int ms)
{
   return WriteDateTime(GetFieldPtr(field), utc, ms);
}

inline bool CDBase::Read(const DBF_FIELD* field, bool* b)
{
   BOOL temp;
   bool ok = ::dbf_getfield_bool(m_handle, field, &temp) ? true : false;
   if (ok) *b = temp ? true : false;
   return ok;
}

inline bool CDBase::Read(size_t field, bool* b)
{
   return Read(GetFieldPtr(field), b);
}

inline bool CDBase::Read(const char* field, bool* b)
{
   return Read(GetFieldPtr(field), b);
}

inline bool CDBase::Write(const DBF_FIELD* field, const bool& b)
{
   return ::dbf_putfield_bool(m_handle, field, b) ? true : false;
}

inline bool CDBase::Write(const char* field, const bool& b)
{
   return Write(GetFieldPtr(field), b);
}

inline bool CDBase::Write(size_t field, const bool& b)
{
   return Write(GetFieldPtr(field), b);
}

inline bool CDBase::Write(const DBF_FIELD* field, const char* str)
{
   return ::dbf_putfield(m_handle, field, str) ? true : false;
}

inline bool CDBase::Write(const char* field, const char* str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool CDBase::Write(size_t field, const char* str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool CDBase::AddNew(void)
{
   return ::dbf_addrecord(m_handle) ? true : false;
}

inline const DBF_FIELD* CDBase::GetFieldPtr(size_t field) const
{
   return ::dbf_getfieldptr(m_handle, field);
}

inline const DBF_FIELD* CDBase::GetFieldPtr(const char* field) const
{
   return ::dbf_getfieldptr_name(m_handle, field);
}

inline size_t CDBase::GetField(const DBF_FIELD* field, char* buf, size_t buf_len, enum dbf_data_type type) const
{
   return ::dbf_getfield(m_handle, field, buf, buf_len, type);
}

inline size_t CDBase::GetField(size_t field, char* buf, size_t buf_len, enum dbf_data_type type) const
{
   return GetField(GetFieldPtr(field), buf, buf_len, type);
}

inline size_t CDBase::GetField(const char* field, char* buf, size_t buf_len, enum dbf_data_type type) const
{
   return GetField(GetFieldPtr(field), buf, buf_len, type);
}

inline bool CDBase::IsRecordDeleted() const
{
   return ::dbf_isrecorddeleted(m_handle) ? true : false;
} 

inline bool CDBase::DeleteRecord(bool bDelete)
{
   return ::dbf_deleterecord(m_handle, bDelete) ? true : false;
}

inline bool CDBase::IsEmpty(void) const
{
   return (0 == GetRecordCount());
}

inline bool CDBase::Copy(void* stream, void* stream_memo, struct zlib_filefunc_def_s* api, bool include_records, bool include_deleted_records)
{
   return ::dbf_copy(m_handle, stream, stream_memo, api, include_records, include_deleted_records) ? true : false;
}

inline int CDBase::GetLastError(void) const
{
   return ::dbf_getlasterror(m_handle);
}

inline const char* CDBase::GetLastErrorStr(void) const
{
   return ::dbf_getlasterror_str(m_handle);
}

inline time_t CDBase::GetLastUpdate(void) const
{
   return ::dbf_getlastupdate(m_handle);
}

inline bool CDBase::GetFieldInfo(size_t index, DBF_FIELD_INFO* info) const
{
   return ::dbf_getfield_info(m_handle, index, info) ? true : false;
}

inline enum dbf_data_type CDBase::GetFieldType(const DBF_FIELD* field) const
{
   return ::dbf_getfield_type(m_handle, field);
}

inline enum dbf_data_type CDBase::GetFieldType(size_t field) const
{
   return ::dbf_getfield_type(m_handle, GetFieldPtr(field));
}

inline bool CDBase::OpenMemo(void* stream)
{
   return ::dbf_memo_open(m_handle, stream) ? true : false;
}
/*
inline bool CDBase::CreateMemo(void* stream)
{
   return ::dbf_memo_create(m_handle, stream) ? true : false;
}
*/

inline void CDBase::GetFileAPI(struct zlib_filefunc_def_s* api) const
{
   ::dbf_getfileapi(m_handle, api);
}

inline void* CDBase::GetMemoFile()
{
   return ::dbf_getmemofile(m_handle);
}

inline bool CDBase::IsEditable(void) const
{
   return ::dbf_iseditable(m_handle) ? true : false;
}

inline bool CDBase::IsModified(void) const
{
   return ::dbf_ismodified(m_handle) ? true : false;
}

inline bool CDBase::IsFieldNull(const DBF_FIELD* field) const
{
   return ::dbf_isnull(m_handle, field) ? true : false;
}

inline bool CDBase::Read(const DBF_FIELD* field, long* n)
{
   return ::dbf_getfield_numeric(m_handle, field, n) ? true : false;
}

inline bool CDBase::Read(const char* field, long* n)
{
   return ::dbf_getfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(size_t field, long* n)
{
   return ::dbf_getfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(const DBF_FIELD* field, double* n)
{
   return ::dbf_getfield_float(m_handle, field, n) ? true : false;
}

inline bool CDBase::Read(const char* field, double* n)
{
   return ::dbf_getfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Read(size_t field, double* n)
{
   return ::dbf_getfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const char* field, long n)
{
   return ::dbf_putfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const DBF_FIELD* field, long n)
{
   return ::dbf_putfield_numeric(m_handle, field, n) ? true : false;
}

inline bool CDBase::Write(size_t field, long n)
{
   return ::dbf_putfield_numeric(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const char* field, double n)
{
   return ::dbf_putfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline bool CDBase::Write(const DBF_FIELD* field, double n)
{
   return ::dbf_putfield_float(m_handle, field, n) ? true : false;
}

inline bool CDBase::Write(size_t field, double n)
{
   return ::dbf_putfield_float(m_handle, GetFieldPtr(field), n) ? true : false;
}

inline int CDBase::FindField(const char* fieldname) const
{
   return ::dbf_findfield(m_handle, fieldname);
}

