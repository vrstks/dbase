// d4base.hpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __D4BASE_HPP__
#define __D4BASE_HPP__

#ifndef __D4BASE_H__
   #include "d4base.h"
#endif

#ifndef __AFXDAO_H
enum
{
   AFX_DAO_NEXT  = +1,
   AFX_DAO_PREV  = -1,
   AFX_DAO_FIRST = LONG_MIN,
   AFX_DAO_LAST  = LONG_MAX
};
#endif
#ifndef _DBDAOINT_H_
typedef enum DataTypeEnum {
    dbBoolean = 1,
    dbByte = 2,
    dbInteger = 3,
    dbLong = 4,
    dbCurrency = 5,
    dbSingle = 6,
    dbDouble = 7,
    dbDate = 8,
    dbBinary = 9,
    dbText = 10,
    dbLongBinary = 11,
    dbMemo = 12,
    dbGUID = 15,
    dbBigInt = 16,
    dbVarBinary = 17,
    dbChar = 18,
    dbNumeric = 19,
    dbDecimal = 20,
    dbFloat = 21,
    dbTime = 22,
    dbTimeStamp = 23
} DataTypeEnum;
#endif

typedef long field_ref;
struct tm;
class COleDateTime;

////////////////////////////////////////////////////////////////////////
// CCodebase

class CCodebase
{
// Attributes
public:
   int   m_handle;
	enum { hFileNull = -1 };

// Construction
public:
   CCodebase(void);

	// Cursor operations
   void MoveNext(void);
	void MovePrev(void);
	void MoveFirst(void);
	void MoveLast(void);

   bool Attach(int handle);
   int Detach(void);

   bool IsOpen(void) const;
   bool IsEmpty(void) const;
   bool Create(const TCHAR *name, struct tzlib_filefunc_def_s*, const FIELD *array, int array_count, int safety = 1);

	int GetFieldCount(void);
	const TCHAR* GetFilePath(void) const;

   field_ref    GetFieldRef(LPCSTR field)  const;
   field_ref    GetFieldRef(int nIndex) const;
   DataTypeEnum GetFieldType(field_ref) const;
   DataTypeEnum GetFieldType(int index) const;
   DataTypeEnum GetFieldType(LPCSTR field) const;
   LPCSTR    GetFieldName(field_ref) const;
   int       GetFieldWidth(field_ref) const;
   bool      Append(bool bBlank = TRUE);
   bool      Update(void);
   bool      IsNull(int index) const;
   bool      IsNull(field_ref) const;

   bool IsMemo(field_ref ref) const;
   bool IsDeleted(void) const;

   bool Read (LPCSTR field, LPSTR str, int str_len);
   bool Read (field_ref   , LPSTR str, int str_len);
   bool Read (LPCSTR field, long*);
   bool Read (field_ref   , long*);
   bool Read (LPCSTR field, double*);
   bool Read (field_ref   , double*);
   bool ReadBool(LPCSTR field, bool*);
   bool ReadBool(field_ref   , bool*);
   bool Write(LPCSTR field, LPCSTR str);
   bool Write(field_ref   , LPCSTR str);
   bool Write(LPCSTR field, long value);
   bool Write(field_ref   , long value);
   bool Write(LPCSTR field, double value);
   bool Write(field_ref   , double value);
   bool WriteBool(LPCSTR field, bool value);
   bool WriteBool(field_ref   , bool value);

   bool Read (LPCSTR field, struct tm*);
   bool Read (field_ref   , struct tm*);
   bool Write(LPCSTR field, const struct tm*);
   bool Write(field_ref   , const struct tm*);

   bool Read (LPCSTR field, COleDateTime*);
   bool Read (field_ref   , COleDateTime*);
   bool Write(LPCSTR field, const COleDateTime&);
   bool Write(field_ref   , const COleDateTime&);

#ifdef __AFXCONV_H__
   field_ref    GetFieldRef(LPCWSTR field)  const;
   bool Read (LPCSTR field, LPWSTR str, int str_len);
   bool Read (field_ref   , LPWSTR str, int str_len);
   bool Write(LPCSTR field, LPCWSTR str);
   bool Write(field_ref    , LPCWSTR str);
#endif
#if defined(__AFXCONV_H__) || !defined(_UNICODE)
   bool Read(LPCSTR field , CString*, int nPreAllocSize = 255);
   bool Read(field_ref    , CString*, int nPreAllocSize = 255);
#endif
   void Select(void) const;
   
   bool Pack(void);
   bool Delete(long record);
   bool Delete(long start, long end);
   bool DeleteAll(void);

   bool IsEOF(void) const;
   bool IsBOF(void) const;
   size_t GetRecordCount(void) const;
   long GetAbsolutePosition(void) const;
	bool SetAbsolutePosition(long lPosition);
   bool SetFilter( int (* filter_routine)(void*), void* user);
   BYTE GetLanguage(void) const;
   void SetLanguage(BYTE) const;
   
// Implementation
public:
   virtual ~CCodebase(void);
   virtual bool Open(LPCTSTR lpsz, struct tzlib_filefunc_def_s* file_api = NULL, bool bReadOnly = FALSE);
   virtual void Close(void);
	virtual void Move(long lRows);

   static void Init(void);
   static void Term(void);
};

////////////////////////////////////////////////////////////////////////
// CCodebaseRecordset

class CCodebaseRecordset : public CCodebase
{
// Attributes
public:
   bool m_bUseArray;
   bool m_bInitialized;
   long* m_aiRecord;
protected:
   long  m_recordcount;

// Construction
public:
   CCodebaseRecordset(void);

   void InitRecordCount(void);
   size_t GetRecordCount(void);
   long GetAbsolutePosition(void) const;
	bool SetAbsolutePosition(long lPosition);
   
   bool IsEOF(void) const;
   bool IsBOF(void) const;

   void Sort(bool bAscending = TRUE);

// Implementation
public:
   virtual ~CCodebaseRecordset(void);
   virtual bool Open(LPCTSTR lpsz, struct tzlib_filefunc_def_s*file_api = NULL, bool bInitRecordCount = TRUE, bool bReadOnly = FALSE);
   virtual void Close(void);
   virtual int Compare(const long record[2]);
	virtual void Move(long lRows);

protected:
   virtual void Fixups();
   virtual bool Filter(void);
   static int Filter(void*);
   static int Compare (const void *elem0, const void *elem1, LPVOID lpUser);
};
#include "d4base.inl"

#endif // __D4BASE_HPP__
