/****************************************************************/
/*																*/
/*  dbf_mfc.h												    */
/*																*/
/*	This file is part of the DBF Explorer application.			*/
/*																*/
/*  Programmed by Pablo van der Meer							*/
/*  Partially based on Turbo C source code by Mark Sadler.		*/
/*																*/
/*  Copyright Pablo Software Solutions 2002						*/
/*																*/
/*  Last updated: 08 August 2002								*/
/*  Troels: Turned into thin dbf.c wrapper: 12 Sept 2006        */
/*																*/
/****************************************************************/

#ifndef __DBF_MFC_H__
#define __DBF_MFC_H__

#include "ioapi/zlib.h"
#include "ioapi/ioapi.h"
#include "bool.h"
#include "dbf.h"
#include "dbf.hpp"
#include "dbf.inl"

class COleDateTime;
class CDbaseFile : public CObject, public CDBase
{
   typedef CDBase base;
public:
	CDbaseFile();           

// Attributes
protected:
	CString m_strFileName;			// Name of data file 
protected:

// Operations
public:
    int Open(const TCHAR* lpszFileName, dbf_editmode editmode = dbf_editmode_editable);
    int Open(const TCHAR* lpszFileName, dbf_editmode editmode, const DBF_OPEN& parm);
    int Create(const TCHAR* lpszFileName, const CStringArray& strFieldArray);
    bool Create(const TCHAR* lpszFileName, 
      void* stream, struct zlib_filefunc_def_s*, 
      const DBF_FIELD_INFO* array, size_t array_count, 
      dbf_charconv charconv = dbf_charconv_compatible, void* memo = NULL);
    bool CloneDatabase(const TCHAR* lpszCloneName, bool bCopyRecords = FALSE, bool bSkipDeleted = FALSE);

    const char* GetLastError(void) const;
    const CString& GetFilename(void) const;

    int GetRecord(size_t index);
    int PutRecord(size_t index);
    bool PutRecord(void);

    int DeleteRecord(bool bDelete = true);
    int AddRecord();
	int InsertRecord(size_t index);

	int GetFirstRecord();
	int GetLastRecord();
	int GetNextRecord();
	int GetPrevRecord();

    bool Read(const DBF_FIELD* , COleDateTime*);
    bool Read(const char* field, COleDateTime*);
    bool Read(size_t      field, COleDateTime*);

    bool Write(const DBF_FIELD* , const COleDateTime&);
    bool Write(const char* field, const COleDateTime&);
    bool Write(size_t      field, const COleDateTime&);

#ifdef _WINBASE_
    bool Write(const DBF_FIELD* , const SYSTEMTIME&);
#endif
    bool Write(const DBF_FIELD* , const CTime&, int ms = 0);
    bool Write(const char* field, const CTime&, int ms = 0);
    bool Write(size_t      field, const CTime&, int ms = 0);

    bool Write(const DBF_FIELD* , const bool&);
    bool Write(const char* field, const bool&);
    bool Write(size_t field     , const bool&);

    bool Write(const char* field, const TCHAR*);
    bool Write(const DBF_FIELD* , const TCHAR*);
    bool Write(size_t field     , const TCHAR*);

    bool Write(const char* field, long);
    bool Write(const DBF_FIELD* , long);
    bool Write(size_t field     , long);

    bool Write(const char* field, double);
    bool Write(const DBF_FIELD* , double);
    bool Write(size_t field     , double);

    bool Read(const DBF_FIELD* , double*);
    bool Read(const char* field, double*);
    bool Read(size_t      field, double*);

    bool Read(const DBF_FIELD* , long*);
    bool Read(const char* field, long*);
    bool Read(size_t      field, long*);

    bool Read(const DBF_FIELD* , CTime*, int* ms = NULL);
    bool Read(const char* field, CTime*, int* ms = NULL);
    bool Read(size_t      field, CTime*, int* ms = NULL);

    size_t Read(const DBF_FIELD* , CString*);
    size_t Read(const char* field, CString*);
    size_t Read(size_t      field, CString*);

    bool Read(const DBF_FIELD* , bool*);   
    bool Read(const char* field, bool*);   
    bool Read(size_t field     , bool*);

    long GetNumericField(const DBF_FIELD*);
    long GetNumericField(size_t field);
    long GetNumericField(const char* field);
    double GetFloatField(const DBF_FIELD*);
    double GetFloatField(size_t field);
    double GetFloatField(const char* field);
    bool GetLogicalField(const DBF_FIELD*);
    bool GetLogicalField(size_t field);
    bool GetLogicalField(const char* field);

    bool IsOpen(void) const { return (m_handle != NULL); }

    int PutNumericField(const DBF_FIELD* , long);
    int PutNumericField(size_t field     , long);
    int PutNumericField(const char* field, long);
	
    int PutFloatField(const DBF_FIELD* , double);
    int PutFloatField(size_t field     , double);
    int PutFloatField(const char* field, double);

    int PutLogicalField(const DBF_FIELD* , bool);
    int PutLogicalField(size_t field     , bool);
    int PutLogicalField(const char* field, bool);

    size_t SearchRecord(const DBF_FIELD* , const TCHAR* criteria, size_t nStartRec = 0);
    size_t SearchRecord(const char* field, const TCHAR* criteria, size_t nStartRec = 0);
    size_t SearchRecord(size_t field     , const TCHAR* criteria, size_t nStartRec = 0);

    int Pack();
    DBF_HANDLE Detach(void);

// Implementation
public:
    virtual ~CDbaseFile();
    virtual void Close();

public:
    CString GetMemoFileName(const TCHAR* lpszFileName = NULL) const;
    int OpenMemoFile(const TCHAR* lpszFileName = NULL);
    //int CreateMemoFile();
	
    size_t GetMemoFieldLength(size_t field);
    int GetMemoField(size_t field     , char *buf, size_t buf_len);
    int ClearMemoField(size_t field);
    int ClearMemoField(const char* field);	
};

#include "dbf_mfc.inl"

#endif // __DBF_MFC_H__
