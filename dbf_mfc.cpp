/****************************************************************/
/*																*/
/*  dbf_mfc.cpp												    */
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

#include <afxwin.h>
#include <afxconv.h>
#include <afxdisp.h> // COleDateTime
#pragma warning(disable:4097)
#include "dbf_mfc.h"

bool CDbaseFile::Write(const DBF_FIELD* field, const COleDateTime& dt)
{
   return WriteDate(field, dt.GetYear(), dt.GetMonth()-1, dt.GetDay());
}

/* Search for a specific record where criteria = field value	*/
/* Start searching from record 'nStartRec'						*/
size_t CDbaseFile::SearchRecord(const DBF_FIELD* field, const TCHAR* lpszCriteria, size_t nStartRec)
{
	CString buf;
   int nResult;
	// walk through all records
   for(nResult=GetRecord(nStartRec); nResult==DBASE_SUCCESS; nResult=GetNextRecord())
  	{
		if (!IsRecordDeleted())
		{
			Read(field, &buf);
			if (!_tcscmp(buf, lpszCriteria))
			{
				return GetPosition();              			
			}
		}
	}
	return 0;
}

CString CDbaseFile::GetMemoFileName(const TCHAR* filename) const
{
   TCHAR name[_MAX_PATH];
   lstrcpy(name, filename ? filename : m_strFileName.operator LPCTSTR());

	int nLength = lstrlen(name)-1;
	if (nLength < 0) return name;
	
	switch(name[nLength])
	{
		case 'F':
			name[nLength] = 'T';
			break;
		default:
		case 'f':
			name[nLength] = 't';
			break;
	}
	return name;
}

bool CDbaseFile::CloneDatabase(const TCHAR* lpszCloneName, bool bCopyRecords, bool bSkipDeleted)
{
   USES_CONVERSION;
	CString strField;
	CStringArray fieldsArray;
	// insert fieldnames
	for (size_t i = 0; i < GetFieldCount(); i++)
	{
      DBF_FIELD_INFO info;
      if (GetFieldInfo(i, &info))
		{
			strField.Format(_T("%s,%c,%d,%d"), A2CT(info.name), dbf_gettype_ext2int(info.type), info.length, info.decimals);
			fieldsArray.Add(strField);
		}
   }	

	CDbaseFile temp;
	// create empty database.dbf
	bool ok = (temp.Create(lpszCloneName, fieldsArray) == DBASE_SUCCESS);
   if (ok)
	{
	   if (bCopyRecords)
	   {
		   uLong dwCounter = 0;
		   CString szBuff;

		   // copy all records
		   for (int rc=GetFirstRecord(); rc==DBASE_SUCCESS; rc=GetNextRecord())
  		   {
			   if(bSkipDeleted && IsRecordDeleted())
				   continue;

			   dwCounter++;
			   temp.AddRecord();
			   temp.GetRecord(dwCounter);

			   // copy all fields
			   for (size_t i = 0; i < GetFieldCount(); i++)
			   {
               DBF_FIELD_INFO info;
               if (GetFieldInfo(i, &info))
		         {
					   if (Read(i, &szBuff))
					   {
						   temp.Write(i, szBuff);
					   }
					   
					   // copy memo data
					   if (info.type == DBF_DATA_TYPE_MEMO)
					   {
						   uLong dwLength = GetMemoFieldLength(i);
						   if (dwLength)
						   {
							   char* buf = (char*)malloc(dwLength+1);
							   GetMemoField(i, buf, dwLength);
                        buf[dwLength] = 0;

							   // set value
							   temp.PutMemoField(i, buf, strlen(buf));
							   free(buf);
						   }	
						   else
						   {
							   // initialize to zero
							   temp.ClearMemoField(i);
						   }
					   }
				   }
				   else
				   {
					   // field does not exists-> do not copy data
				   }
			   }
			   temp.PutRecord(dwCounter);
		   }
		   temp.Close();
	   }
   }
	return ok;
}

/* Remove all deleted records from datafile.					*/
int CDbaseFile::Pack()
{
   const CString filename = m_strFileName;
	TCHAR szTempFileName[MAX_PATH];
	bool bMemoFileExists = false;

	// create temporary file
	GetTempPath(sizeof(szTempFileName), szTempFileName);
	lstrcat(szTempFileName, _T("~DBFPACK.DBF"));

	// open memo file .DBT
	if (GetMemoFile() == NULL) OpenMemoFile();
	bMemoFileExists = (GetMemoFile() != NULL);

	// clone database, copy all records except those marked 'deleted'
	if (!CloneDatabase(szTempFileName, true, true))
	{
		return DBASE_NO_FILE;
	}
	// close original data file (and DBT file)
	Close();
   
	// delete original data file
	if (_tunlink(filename) != 0)
	{
		//_sntprintf(MOD_lasterrormsg, _countof(MOD_lasterrormsg), "Failed to delete file '%s'.", filename.operator LPCTSTR());
		return DBASE_NO_FILE;
	}

	// rename temporary file to original name
	if (_trename(szTempFileName, filename) != 0) 
	{
		//_sntprintf(MOD_lasterrormsg, _countof(MOD_lasterrormsg), "Failed to rename file '%s'.", szTempFileName);
		return DBASE_NO_FILE;
	}
	
	if (bMemoFileExists)
	{
		// delete original data file
		_tunlink(GetMemoFileName());

		// rename temporary memo file to original name
		if (_trename(GetMemoFileName(szTempFileName), GetMemoFileName()) != 0) 
		{
			return DBASE_NO_FILE;
		}
	}
	// reopen file
	return Open(filename);
}

/* Create new database file, based on structure in parameters.	*/
int CDbaseFile::Create(const TCHAR* filename, const CStringArray& strFieldArray)
{
   USES_CONVERSION;
      
   const size_t array_count = strFieldArray.GetSize();
   DBF_FIELD_INFO* array = (DBF_FIELD_INFO*)malloc(array_count*sizeof(DBF_FIELD_INFO));
	for (size_t x = 0; x < array_count; x++)
	{
      DBF_FIELD_INFO* field = array + x;
		
      CString strName, strBuff;
		TCHAR nType; 
		Byte nLength; 
		Byte nDecCount; 

		// get name
		AfxExtractSubString(strName, strFieldArray[x], 0, ',');
		strName.TrimLeft();
		strName.TrimRight();
		// get type
		AfxExtractSubString(strBuff, strFieldArray[x], 1, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nType = strBuff.GetAt(0);
		// get length
		AfxExtractSubString(strBuff, strFieldArray[x], 2, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nLength = (Byte)_ttoi(strBuff);
		// get field dec count
		AfxExtractSubString(strBuff, strFieldArray[x], 3, ',');
		strBuff.TrimLeft();
		strBuff.TrimRight();
		nDecCount = (Byte)_ttoi(strBuff);		

		if (strName.IsEmpty() || 
			(nType != 'C' && nType != 'D' && nType != 'N' && nType != 'L' && nType != 'F' && nType != 'M') ||
			nLength < 1 || 
			nDecCount < 0)
      {
         break;
      }

      strncpy(field->name, T2CA(strName), sizeof(field->name));
      field->type = dbf_gettype_int2ext((Byte)nType);
      field->length = nLength;
      field->decimals = nDecCount;
	}
   m_handle = ::dbf_create(T2CA(filename), array, array_count, ENUM_dbf_charconv_compatible);
   free(array); array = NULL;
   if (m_handle)
   {
      m_strFileName = filename;
	}
   else
   {
      //free(array);
      return DBASE_NO_FILE;
   }
	return DBASE_SUCCESS;
}
