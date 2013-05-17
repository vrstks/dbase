// dbf.hpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBF_HPP__
#define __DBF_HPP__

#ifndef __DBF_H__
#include <dbf.h>
#endif

#include <string>
#include <vector>

#ifndef OPENCLOSECLASS_DEFINED
#define OPENCLOSECLASS_DEFINED
class OpenCloseClass
{
public:
   virtual void Close() = 0;
   virtual bool IsOpen(void) const = 0;
};
#endif

typedef std::vector<DBF_FIELD_INFO> DBaseFieldVector;

class CDBase : public OpenCloseClass
{
public:
   CDBase();

// Attributes
public:   
   DBF_HANDLE m_handle;
protected:

// Operations
public:
    bool Open(const char* filename, dbf_editmode);
    bool Open(const char* filename, dbf_editmode, const DBF_OPEN&);
    bool Create(const char* filename, const DBaseFieldVector&);
    bool Create(const char* filename, const DBaseFieldVector&, const DBF_OPEN&);
    bool Create(void* stream, const struct zlib_filefunc_def_s*, const DBaseFieldVector&, dbf_charconv charconv = dbf_charconv_compatible, void* memo = NULL);
    bool CloneDatabase(const char* CloneName, bool bCopyRecords = false, bool bSkipDeleted = false);
    void GetFileAPI(struct zlib_filefunc_def_s*) const;
    void* GetMemoFile(void);
    bool IsEditable(void) const;
    bool IsModified(void) const;

   void        GetInfo(DBF_INFO*) const;
   int         GetLastError(void) const;
   const char* GetLastErrorStr(void) const;

   virtual bool SetPosition(dbf_uint index);
   bool PutRecord(void);
   bool PutRecord(dbf_uint index);
   bool IsEmpty(void) const;
   bool IsFieldNull(const DBF_FIELD*) const;

   bool DeleteRecord(bool bDelete = true);
   bool AddNew();
   //int InsertRecord(dbf_uint nRecordIndex);

   bool GetFirstRecord();
   bool GetLastRecord();
   bool GetNextRecord();
   bool GetPrevRecord();
   bool IsRecordDeleted() const;

   // field operations
   const DBF_FIELD* GetFieldPtr(dbf_uint field) const;
   const DBF_FIELD* GetFieldPtr(const char* field) const;

   int FindField(const char* fieldname) const;
   
   size_t Read(const DBF_FIELD* , std::string* buf, size_t buf_len, dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   size_t Read(dbf_uint field   , std::string* buf, size_t buf_len, dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   size_t Read(const char* field, std::string* buf, size_t buf_len, dbf_data_type type = DBF_DATA_TYPE_ANY) const;
   
   int GetField_Date(dbf_uint field , std::string*);
   int GetField_Date(const char* field, std::string*);
   
   bool Read(const DBF_FIELD* , struct tm*, int* ms = NULL);
   bool Read(const char* field, struct tm*, int* ms = NULL);
   bool Read(dbf_uint field , struct tm*, int* ms = NULL);

   bool ReadDateTime(const DBF_FIELD* , time_t*, int* ms = NULL);
   bool ReadDateTime(const char* field, time_t*, int* ms = NULL);
   bool ReadDateTime(dbf_uint field , time_t*, int* ms = NULL);

   bool Read(const DBF_FIELD* , long*);
   bool Read(const char* field, long*);
   bool Read(dbf_uint field , long*);

   size_t Read(const DBF_FIELD* , std::string* buf, size_t buf_len);
   size_t Read(const char* field, std::string* buf, size_t buf_len);

   bool Read(const DBF_FIELD* , bool*);   
   bool Read(const char* field, bool*);   
   bool Read(dbf_uint field , bool*);

   bool Read(const DBF_FIELD* , double*);   
   bool Read(const char* field, double*);   
   bool Read(dbf_uint field , double*);

#ifdef _WINBASE_
   bool Write(const DBF_FIELD*, const SYSTEMTIME&);
#endif
   bool Write(const DBF_FIELD* , const struct tm&, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(const char* field, const struct tm&, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(dbf_uint field , const struct tm&, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);

   bool WriteTime(const DBF_FIELD*, time_t, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool WriteTime(const char* field, time_t, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool WriteTime(dbf_uint field, time_t, int ms = 0, dbf_data_type type = DBF_DATA_TYPE_ANY);

   bool Write(const DBF_FIELD* , const bool&);
   bool Write(const char* field, const bool&);
   bool Write(dbf_uint field , const bool&);

   bool Write(const char* field, const std::string&);
   bool Write(const DBF_FIELD* , const std::string&);
   bool Write(dbf_uint field , const std::string&);

   bool Write(const char* field, const long&);
   bool Write(const DBF_FIELD* , const long&);
   bool Write(dbf_uint field , const long&);

   bool Write(const char* field, const double&);
   bool Write(const DBF_FIELD* , const double&);
   bool Write(dbf_uint field , const double&);

   bool Copy(void* stream, void* stream_memo, const struct zlib_filefunc_def_s*, bool include_records, bool include_deleted_records);

   dbf_uint GetPosition(void) const;
   dbf_uint GetRecordCount(void) const;
   dbf_uint GetFieldCount(void) const;

   bool       Attach(void* stream, const struct zlib_filefunc_def_s*, 
                     dbf_editmode editmode = dbf_editmode_editable,
                     dbf_charconv conv = dbf_charconv_compatible, 
                     void* memo = NULL,
                     const std::string* tablename = NULL);
   bool       Attach(DBF_HANDLE);
   bool       Attach(CDBase*);
   DBF_HANDLE Detach(void);
   bool GetFieldInfo(dbf_uint index, DBF_FIELD_INFO*) const;
   dbf_data_type GetFieldType(dbf_uint field) const;
   dbf_data_type GetFieldType(const DBF_FIELD*) const;

// Implementation
public:
   virtual ~CDBase();
   virtual void Close();
   virtual bool IsOpen(void) const;

public:
   //std::string GetMemoFileName(const std::string* filename = NULL);
   int OpenMemoFile(const std::string* filename = NULL);
   int CreateMemoFile();
   void CloseMemoFile();
   
   bool OpenMemo(void* stream);
   bool CreateMemo(void* stream);
   static bool ParseDate(const char*, struct tm*, int* ms, dbf_data_type);
};

#endif // __DBF_HPP__
