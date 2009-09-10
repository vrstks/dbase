// dbf.hpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBF_HPP__
#define __DBF_HPP__

#ifndef __DBF_H__
#include <dbf.h>
#endif

#ifndef OPENCLOSECLASS_DEFINED
#define OPENCLOSECLASS_DEFINED
class COpenClose
{
public:
   virtual void Close() = 0;
   virtual bool IsOpen(void) const = 0;
};
#endif

class CDBase : public COpenClose
{
public:
   CDBase();

// Attributes
public:   
   DBF_HANDLE m_handle;
protected:

// Operations
public:
   bool Open(const char* filename, struct zlib_filefunc_def_s* api = NULL, bool editable = true, enum dbf_charconv charconv = ENUM_dbf_charconv_compatible);
   bool Open(const char* filename, bool editable = true, enum dbf_charconv charconv = ENUM_dbf_charconv_compatible);
   bool Create(const char* filename, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv = ENUM_dbf_charconv_compatible);
   bool Create(void* stream, struct zlib_filefunc_def_s*, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv = ENUM_dbf_charconv_compatible, void* memo = NULL);
   bool CloneDatabase(const char* lpszCloneName, bool bCopyRecords = false, bool bSkipDeleted = false);
   void GetFileAPI(struct zlib_filefunc_def_s*) const;
   void* GetMemoFile(void);
   bool IsEditable(void) const;
   bool IsModified(void) const;

   time_t GetLastUpdate(void) const;
   int         GetLastError(void) const;
   const char* GetLastErrorStr(void) const;

   virtual bool SetPosition(size_t index);
   bool PutRecord(void);
   bool PutRecord(size_t index);
   bool IsEmpty(void) const;
   bool IsFieldNull(const DBF_FIELD*) const;

   bool DeleteRecord(bool bDelete = true);
   bool AddNew();
   //int InsertRecord(size_t nRecordIndex);

   bool GetFirstRecord();
   bool GetLastRecord();
   bool GetNextRecord();
   bool GetPrevRecord();
   bool IsRecordDeleted() const;

   // field operations
   const DBF_FIELD* GetFieldPtr(size_t field) const;
   const DBF_FIELD* GetFieldPtr(const char* field) const;

   int FindField(const char* fieldname) const;
   
   size_t Read(const DBF_FIELD* , char* buf, size_t buf_len, enum dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   size_t Read(size_t field     , char* buf, size_t buf_len, enum dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   size_t Read(const char* field, char* buf, size_t buf_len, enum dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   
   int GetField_Date(size_t field     , char*);
   int GetField_Date(const char* field, char*);
   
   bool Read(const DBF_FIELD* , struct tm*, int* ms = NULL);
   bool Read(const char* field, struct tm*, int* ms = NULL);
   bool Read(size_t field     , struct tm*, int* ms = NULL);

   bool ReadDateTime(const DBF_FIELD* , time_t*, int* ms = NULL);
   bool ReadDateTime(const char* field, time_t*, int* ms = NULL);
   bool ReadDateTime(size_t field     , time_t*, int* ms = NULL);

   bool Read(const DBF_FIELD* , long*);
   bool Read(const char* field, long*);
   bool Read(size_t field     , long*);

   size_t Read(const DBF_FIELD* , char* buf, size_t buf_len);
   size_t Read(const char* field, char* buf, size_t buf_len);

   bool Read(const DBF_FIELD* , bool*);   
   bool Read(const char* field, bool*);   
   bool Read(size_t field     , bool*);

   bool Read(const DBF_FIELD* , double*);   
   bool Read(const char* field, double*);   
   bool Read(size_t field     , double*);

#ifdef _WINBASE_
   bool Write(const DBF_FIELD*, const SYSTEMTIME&);
#endif
   bool Write(const DBF_FIELD* , const struct tm&, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(const char* field, const struct tm&, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(size_t field     , const struct tm&, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);

   bool WriteTime(const DBF_FIELD* , time_t, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool WriteTime(const char* field, time_t, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool WriteTime(size_t field     , time_t, int ms = 0, enum dbf_data_type type = DBF_DATA_TYPE_ANY);

   bool Write(const DBF_FIELD* , const bool&);
   bool Write(const char* field, const bool&);
   bool Write(size_t field     , const bool&);

   bool Write(const char* field, const char*);
   bool Write(const DBF_FIELD* , const char*);
   bool Write(size_t field     , const char*);

   bool Write(const char* field, long);
   bool Write(const DBF_FIELD* , long);
   bool Write(size_t field     , long);

   bool Write(const char* field, double);
   bool Write(const DBF_FIELD* , double);
   bool Write(size_t field     , double);

   bool Copy(void* stream, void* stream_memo, struct zlib_filefunc_def_s*, bool include_records, bool include_deleted_records);

   size_t GetPosition(void) const;
   size_t GetRecordCount(void) const;
   size_t GetFieldCount(void) const;

   bool       Attach(void* stream, struct zlib_filefunc_def_s*, 
                     bool editable = true, enum dbf_charconv conv = ENUM_dbf_charconv_compatible, 
                     void* memo = NULL);
   bool       Attach(DBF_HANDLE);
   bool       Attach(CDBase*);
   DBF_HANDLE Detach(void);
   bool GetFieldInfo(size_t index, DBF_FIELD_INFO*) const;
   enum dbf_data_type GetFieldType(size_t field) const;
   enum dbf_data_type GetFieldType(const DBF_FIELD*) const;

// Implementation
public:
   virtual ~CDBase();
   virtual void Close();
   virtual bool IsOpen(void) const;

public:
   //CString GetMemoFileName(const char* filename = NULL);
   int OpenMemoFile(const char* filename = NULL);
   int CreateMemoFile();
   void CloseMemoFile();
   
   bool OpenMemo(void* stream);
   bool CreateMemo(void* stream);
   static bool ParseDate(const char*, struct tm*, int* ms, enum dbf_data_type);
};

#endif // __DBF_HPP__
