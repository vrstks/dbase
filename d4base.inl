// d4base.inl
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

////////////////////////////////////////////////////////////////////////
// CCodebase

inline /*static*/ void CCodebase::Init(void)
{ 
   ::d4init(); 
}

inline /*static*/ void CCodebase::Term(void)
{ 
   ::d4init_undo(); 
}

inline CCodebase::CCodebase(void) : m_handle(hFileNull)
{ 
}

inline CCodebase::~CCodebase(void)
{ 
   if (IsOpen()) Close(); 
}

inline const TCHAR* CCodebase::GetFilePath(void) const
{
   Select();
   return ::d4name();
}

inline bool CCodebase::Attach(int handle)
{
   _ASSERTE(m_handle == hFileNull);
   handle = hFileNull;
   return (handle != hFileNull);
}

inline int CCodebase::Detach(void)
{
   _ASSERTE(m_handle != hFileNull);
   int handle = m_handle;
   m_handle = hFileNull;
   return handle;
}

inline bool CCodebase::IsDeleted(void) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::d4deleted() ? true : false;
}

inline void CCodebase::Close(void)
{ 
   _ASSERTE(m_handle != hFileNull);
   ::d4select(Detach());
   ::d4close(); 
}

// Cursor operations
inline void CCodebase::MoveNext(void)  { Move(AFX_DAO_NEXT ); }
inline void CCodebase::MovePrev(void)  { Move(AFX_DAO_PREV ); }
inline void CCodebase::MoveFirst(void) { Move(AFX_DAO_FIRST); }
inline void CCodebase::MoveLast(void)  { Move(AFX_DAO_LAST ); }

inline bool CCodebase::IsEmpty        (void) const { return (GetRecordCount() == 0); }

inline bool CCodebase::IsOpen(void) const  { return (m_handle >= 0); }

inline bool CCodebase::SetFilter( int (* filter_routine)(void*), void* user)
{
   Select();
   return (-1 != ::x4filter(filter_routine, user));
}

inline bool CCodebase::Create(const TCHAR *name, struct tzlib_filefunc_def_s* file_api, const FIELD *array, int array_count, int safety)
{
   _ASSERTE(m_handle == hFileNull);
   m_handle = ::d4create(name, file_api, array_count, array, safety);
   bool ok = (m_handle >= 0);
   if (ok)
   {
      Select();
      ::x4top();
   }
   return ok;
}

inline bool CCodebase::Open(LPCTSTR lpsz, struct tzlib_filefunc_def_s* file_api, bool bReadOnly)
{ 
   _ASSERTE(m_handle == hFileNull);
   m_handle = ::d4use2(lpsz, file_api, bReadOnly);
   bool ok = (m_handle >= 0);
   if (ok)
   {
      Select();
      if (bReadOnly)
      {
         ok = !IsEmpty();
         if (!ok) Close();
      }
      if (ok) ::x4top();
   }
   return ok;
}

inline void CCodebase::Select(void) const
{ 
   _ASSERTE(m_handle != hFileNull);
   ::d4select(m_handle); 
}

inline BYTE CCodebase::GetLanguage(void) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::d4language_get(); 
}

inline void CCodebase::SetLanguage(BYTE language) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   ::d4language_set(language);
}

inline bool CCodebase::Pack(void)           
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return (0 == ::d4pack()); 
}

inline bool CCodebase::Delete(long start, long end)
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return (0 == ::d4zap(start+1, end+1)); 
}

inline bool CCodebase::Delete(long record)
{ 
   return Delete(record, record); 
}

inline bool CCodebase::DeleteAll(void)      
{ 
   return Delete(0L, GetRecordCount()-1);
}

inline bool CCodebase::IsEOF(void) const    
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::d4eof() ? true : false; 
}

inline bool CCodebase::IsBOF(void) const    
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::d4bof() ? true : false; 
}

inline size_t CCodebase::GetRecordCount(void) const
{ 
   Select();
   return ::d4reccount(); 
}

inline bool CCodebase::SetAbsolutePosition(long lPosition)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return (0 == ::x4go(lPosition + 1));
}

inline LPCSTR CCodebase::GetFieldName(field_ref ref) const
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::f4name(ref); 
}

inline long CCodebase::GetAbsolutePosition(void) const
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::d4recno() - 1;
}

inline bool CCodebase::Read(field_ref ref, long* value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      *value = ::f4long(ref);
   }
   return ok;
}

inline bool CCodebase::Write(field_ref ref, long value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      ::f4r_long(ref, value);
   }
   return ok;
}

inline bool CCodebase::Read(field_ref ref, double* value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      *value = ::f4double(ref);
   }
   return ok;
}

inline bool CCodebase::Write(field_ref ref, double value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      ::f4r_double(ref, value);
   }
   return ok;
}

inline bool CCodebase::ReadBool(field_ref ref, bool* value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok) switch (::f4true(ref))
   {
      case 0:
         *value = FALSE;
         break;
      case 1:
         *value = TRUE;
         break;
      case -1:
      default:
         ok = FALSE;
         break;
   }
   return ok;
}

inline bool CCodebase::WriteBool(field_ref ref, bool value)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      ::f4r_char(ref, value ? 'Y' : 'N');
   }
   return ok;
}

inline bool CCodebase::Read(field_ref ref, char* str, int str_len)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      if (IsMemo(ref))
      {
         const long pos = ::d4recno();
         const int  len = ::m4read(ref, pos, str, str_len);
         if (len == 0)
         {
            str[0] = '\0';
         }
         ok = (len >= 0);
      }
      else
      {
         LPCSTR temp = ::f4str(ref);
         if (temp != NULL)
         {
            strncpy(str, temp, str_len);

            // trim right
            for (str_len = strlen(temp); (str_len > 0) && (' ' == str[str_len-1]); str_len--)
            {
               str[str_len - 1] = '\0';
            }
         }
         else
         {
            str[0] = '\0';
         }
      }
   #ifdef _WIN32
      OemToCharBuffA(str, str, str_len);
   #endif
   }
   return ok;
}

inline bool CCodebase::Write(field_ref ref, LPCSTR str)
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   bool ok = (ref != -1);
   if (ok)
   {
      char* p = strdup(str);
   #ifdef _WIN32
      CharToOemA(p, p);
   #endif
      if (IsMemo(ref))
      {
         ok = (::m4write(ref, ::d4recno(), p, strlen(p)) >= 0);
      }
      else
      {
         ::f4r_str(ref, p);
         ok = TRUE;
      }
      free(p);
   }
   return ok;
}

#ifdef _TM_DEFINED
inline bool CCodebase::Read(field_ref ref, struct tm* ptm)
{
   long temp;
   bool ok = Read(ref, &temp);
   if (ok)
   {
      ptm->tm_sec = 0;
      ptm->tm_min = 0;
      ptm->tm_hour = 0;
      ptm->tm_mday = (temp / 1    ) % 100;
      ptm->tm_mon  = (temp / 100  ) % 100 - 1;
      ptm->tm_year = (temp / 10000) - 1900;
      ptm->tm_wday = 0;
      ptm->tm_yday = 0;
      ptm->tm_isdst = 0;
   }
   return ok;
}

inline bool CCodebase::Write(field_ref ref, const struct tm* ptm)
{
   char sz[80];
   sprintf(sz, "%04d%02d%02d", ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday);
   return Write(ref, sz);
}
#endif

inline bool CCodebase::Read(LPCSTR field, struct tm* ptm)
{
   return Read(GetFieldRef(field), ptm);
}

inline bool CCodebase::Write(LPCSTR field, const struct tm* ptm)
{
   return Write(GetFieldRef(field), ptm);
}

inline bool CCodebase::Read(LPCSTR field, long* value)
{
   return Read(GetFieldRef(field), value);
}

inline bool CCodebase::Write(LPCSTR field, long value)
{
   return Write(GetFieldRef(field), value);
}

inline bool CCodebase::ReadBool(LPCSTR field, bool* value)
{
   return ReadBool(GetFieldRef(field), value);
}

inline bool CCodebase::WriteBool(LPCSTR field, bool value)
{
   return WriteBool(GetFieldRef(field), value);
}

inline bool CCodebase::Read(LPCSTR field, double * value)
{
   return Read(GetFieldRef(field), value);
}

inline bool CCodebase::Write(LPCSTR field, double value)
{
   return Write(GetFieldRef(field), value);
}

inline bool CCodebase::Read(LPCSTR field, LPSTR str, int str_len)
{
   return Read(GetFieldRef(field), str, str_len);
}

inline bool CCodebase::Write(LPCSTR field, LPCSTR str)
{
   return Write(GetFieldRef(field), str);
}

#ifdef __AFXDISP_H__
inline bool CCodebase::Read(field_ref ref, COleDateTime* utc)
{
   struct tm _tm;
   bool ok = Read(ref, &_tm);
   if (ok)
   {
      utc->SetDateTime(_tm.tm_year + 1900, _tm.tm_mon + 1, _tm.tm_mday, 0, 0, 0);
   }
   return ok;
}

inline bool CCodebase::Write(field_ref ref, const COleDateTime& utc)
{
   struct tm _tm;
   _tm.tm_sec = 0;
   _tm.tm_min = 0;
   _tm.tm_hour = 0;
   _tm.tm_mday = utc.GetDay();
   _tm.tm_mon  = utc.GetMonth();
   _tm.tm_year = utc.GetYear() - 1900;
   _tm.tm_wday = 0;
   _tm.tm_yday = 0;
   _tm.tm_isdst = 0;
   return Write(ref, &_tm);
}
#endif

inline bool CCodebase::Read(LPCSTR field, COleDateTime* utc)
{
   return Read(GetFieldRef(field), utc);
}

inline bool CCodebase::Write(LPCSTR field, const COleDateTime& utc)
{
   return Write(GetFieldRef(field), utc);
}

#ifdef __AFXCONV_H__

inline bool CCodebase::Read(LPCSTR field, LPWSTR str, int str_len)
{
   return Read(GetFieldRef(field), str, str_len);
}

inline bool CCodebase::Read(field_ref ref, LPWSTR str, int str_len)
{
   LPSTR temp = (LPSTR)malloc(str_len);
   bool ok = Read(ref, temp, str_len);
   _mbstowcsz(str, temp, str_len);
   free(temp);
   return ok;
}

inline bool CCodebase::Write(field_ref ref, LPCWSTR str)
{
   USES_CONVERSION;
   return Write(ref, W2CA(str));
}

inline bool CCodebase::Write(LPCSTR field, LPCWSTR str)
{
   return Write(GetFieldRef(field), str);
}

inline field_ref CCodebase::GetFieldRef(LPCWSTR field)  const
{
   USES_CONVERSION;
   return GetFieldRef(W2A(field));
}

#endif

#if defined(__AFXCONV_H__) || !defined(_UNICODE)
inline bool CCodebase::Read(field_ref  ref, CString* str, int nPreAllocSize /*= 255*/)
{
   bool ok = Read(ref, str->GetBuffer(nPreAllocSize), nPreAllocSize);
   str->ReleaseBuffer();
   str->TrimRight();
   return ok;
}

inline bool CCodebase::Read(LPCSTR field, CString* str, int nPreAllocSize /*= 255*/)
{
   return Read(GetFieldRef(field), str, nPreAllocSize);
}
#endif

inline bool CCodebase::Append(bool bBlank)
{
   Select();
   return (0 == (bBlank ? ::d4append_blank() : ::d4append()));
}

inline bool CCodebase::Update(void)
{
   Select();
   bool ok = (0 == ::d4write(::d4recno()));
   if (ok)
   {
      ::d4skip(+1L);
   }
   return ok;
}

inline void CCodebase::Move(long lRows)
{
   Select();
   bool ok = TRUE;
   _ASSERTE(m_handle != hFileNull);
   switch(lRows)
	{
	   case AFX_DAO_FIRST:
         ok = (::x4top() >= 0);
		   break;
	   case AFX_DAO_LAST:
         ok = (::x4bottom() >= 0);
		   break;
	   // General case
	   default:
         ok = (::x4skip(lRows) >= 0);
		   break;
	}
   //return ok;
}

inline bool CCodebase::IsMemo(field_ref ref) const
{ 
   Select();
   _ASSERTE(m_handle != hFileNull);
   return (::f4type(ref) == 'M');
}

inline int CCodebase::GetFieldCount()
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::f4num_fields();
}

inline bool CCodebase::IsNull(int index) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::f4isnull(GetFieldRef(index)) ? true : false;
}

inline bool CCodebase::IsNull(field_ref field) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::f4isnull(field) ? true : false;
}

inline field_ref CCodebase::GetFieldRef(LPCSTR field) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return ::f4ref((char*)field);
}

inline DataTypeEnum CCodebase::GetFieldType(field_ref ref) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   DataTypeEnum type = (DataTypeEnum)-1;
   /*if (!IsNull(ref))*/ switch (::f4type(ref))
   {
      case 'N':
         type = dbNumeric;
         break;
      case 'F':
         type = dbDouble;
         break;
      case 'D':
         type = dbDate;
         break;
      case 'L':
         type = dbBoolean;
         break;
      case 'C':
         type = dbText;
         break;
      case 'M':
         type = dbMemo;
         break;
      case '\0':
      default:
         break;
   }
   return type;   
}

inline DataTypeEnum CCodebase::GetFieldType(LPCSTR field) const
{
   return GetFieldType(GetFieldRef(field));
}

inline DataTypeEnum CCodebase::GetFieldType(int index) const
{
   return GetFieldType(GetFieldRef(index));
}

inline field_ref CCodebase::GetFieldRef(int index) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return (field_ref)::f4j_ref(index + 1);
}

inline int CCodebase::GetFieldWidth(field_ref ref) const
{
   Select();
   _ASSERTE(m_handle != hFileNull);
   return IsMemo(ref) ? 0 : ::f4width(ref);
}

////////////////////////////////////////////////////////////////////////
// CCodebaseRecordset

inline CCodebaseRecordset::CCodebaseRecordset(void) : CCodebase(), 
   m_bUseArray(FALSE), m_aiRecord(NULL), m_recordcount(0), m_bInitialized(FALSE)
{ 
}

inline CCodebaseRecordset::~CCodebaseRecordset(void)
{ 
   if (IsOpen())
   {
      Close(); 
   }
   free(m_aiRecord);
}

inline size_t CCodebaseRecordset::GetRecordCount(void)
{ 
   _ASSERTE(m_handle != hFileNull);
   if (m_bUseArray)
   {
      if (m_aiRecord == NULL)
      {
         m_aiRecord = (long*)realloc(m_aiRecord, CCodebase::GetRecordCount()*sizeof(long));
         for (CCodebase::Move(AFX_DAO_FIRST), m_recordcount = 0; !CCodebase::IsEOF(); CCodebase::Move(AFX_DAO_NEXT), m_recordcount++)
         {
            m_aiRecord[m_recordcount] = CCodebase::GetAbsolutePosition();
         }
         if (m_recordcount != 0)
         {
            m_aiRecord = (long*)realloc(m_aiRecord, m_recordcount*sizeof(m_aiRecord[0]));
         }
         else
         {
            free(m_aiRecord); m_aiRecord = NULL;
         }
         return m_recordcount;
      }
      else
      {
         return m_recordcount;
      }
   }
   return CCodebase::GetRecordCount();
}

inline bool CCodebaseRecordset::SetAbsolutePosition(long lPosition)
{
   _ASSERTE(m_handle != hFileNull);
   bool ok = (GetAbsolutePosition() == lPosition);
   if (!(ok && m_bInitialized))
   {
      bool bFixups = TRUE;
      if (m_aiRecord != NULL)
      {
         if (lPosition < m_recordcount)
         {
            lPosition = m_aiRecord[lPosition];
         }
         else
         {
            lPosition = m_aiRecord[m_recordcount-1]+1;
            bFixups = FALSE;
         }
      }
      //_ASSERTE((lPosition >= 0) && (lPosition < CCodebase::GetRecordCount()));
      ok = CCodebase::SetAbsolutePosition(lPosition);
      if (ok)
      {
         m_bInitialized = TRUE;
         if (bFixups)
         {
            Fixups();
         }
      }
   }
   return ok;
}

inline long CCodebaseRecordset::GetAbsolutePosition(void) const
{ 
   _ASSERTE(m_handle != hFileNull);
   const long pos         = CCodebase::GetAbsolutePosition();
   if (m_aiRecord != NULL)
   {
      for (int i = 0, count = CCodebase::GetRecordCount(); i < count; i++)
      {
         if (m_aiRecord[i] == pos)
         {
            return i;
         }
      }
      return m_recordcount; //m_aiRecord[m_recordcount-1]+1;
   }
   else
   {
      return pos;
   }
}

inline void CCodebaseRecordset::Close(void)
{ 
   CCodebase::Close();
   free(m_aiRecord); m_aiRecord = NULL;
   m_bInitialized = FALSE;
}

inline void CCodebaseRecordset::InitRecordCount(void)
{
   GetRecordCount();
}

inline bool CCodebaseRecordset::Open(LPCTSTR lpsz, struct tzlib_filefunc_def_s* file_api, bool bInitRecordCount, bool bReadOnly)
{ 
   bool ok = CCodebase::Open(lpsz, file_api, bReadOnly);
   if (ok)
   {
      SetFilter(Filter, this);
      if (bInitRecordCount) InitRecordCount(); // needed for SetAbsolutePosition
   }
   return ok;
}

inline bool CCodebaseRecordset::Filter(void)
{
   bool ok = !IsDeleted();
   if (ok)
   {
      Fixups();
   }
   return ok;
}

inline /*static*/ int CCodebaseRecordset::Filter(void* pThis)
{
   return ((CCodebaseRecordset*)pThis)->Filter() ? 0 : 1;
}

inline void CCodebaseRecordset::Fixups(void)
{
}

inline void CCodebaseRecordset::Sort(bool bAscending)
{
   ::LMRT_qsort(m_aiRecord, GetRecordCount(), sizeof(m_aiRecord[0]), Compare, bAscending, this);
}

inline /*static*/ int CCodebaseRecordset::Compare (const void *elem0, const void *elem1, LPVOID lpUser)
{
   CCodebaseRecordset* pThis = (CCodebaseRecordset*)lpUser;
   long record[2];
   record[0] = ((LPBYTE)elem0 - (LPBYTE)pThis->m_aiRecord) / sizeof(pThis->m_aiRecord[0]);
   record[1] = ((LPBYTE)elem1 - (LPBYTE)pThis->m_aiRecord) / sizeof(pThis->m_aiRecord[0]);
   return pThis->Compare(record);
}

inline int CCodebaseRecordset::Compare(const long /*record*/[2])
{
   return 0;
}

inline void CCodebaseRecordset::Move(long lRows)
{
   if (m_aiRecord != NULL)
   {
      switch (lRows)
      {
         case AFX_DAO_FIRST:
            SetAbsolutePosition(0);
            break;
         case AFX_DAO_LAST:
            SetAbsolutePosition(GetRecordCount() - 1);
            break;
         default:
            SetAbsolutePosition(GetAbsolutePosition() + lRows);
            break;
      }
   }
   else CCodebase::Move(lRows);
}

inline bool CCodebaseRecordset::IsEOF(void) const
{
   return (m_aiRecord == NULL) ? CCodebase::IsEOF() : (/*CCodebase::*/GetAbsolutePosition() >= m_recordcount /*m_aiRecord[m_recordcount-1]*/);
}

inline bool CCodebaseRecordset::IsBOF(void) const
{
   return (m_aiRecord == NULL) ? CCodebase::IsBOF() : (0              == GetAbsolutePosition());
}

