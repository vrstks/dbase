// dbf_wx.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBF_WX_H__
#define __DBF_WX_H__

#ifndef __DBF_HPP__
#include <dbf.hpp>
#endif

/////////////////////////////////////////////////////////////////////////////
// wxDBase

class wxVariant;
class wxInputStream;
class wxOutputStream;
class wxFileName;
class wxDBase : public wxObject, public CDBase
{
   typedef CDBase base;

// Attributes
public:
   wxInputStream* m_stream;
   wxInputStream* m_stream_memo;

// Construction
public:
	wxDBase();           

// Operations
public:	
   bool Open  (const wxFileName&, enum dbf_editmode);
   bool Open  (const wxFileName&, enum dbf_editmode, const DBF_OPEN&);
   bool Open  (wxInputStream*, enum dbf_charconv charconv = dbf_charconv_compatible);
   bool Create(const wxFileName&, const DBF_FIELD_INFO* array, dbf_uint array_count);
   bool Create(const wxFileName&, const DBF_FIELD_INFO* array, dbf_uint array_count, const DBF_OPEN&);
   bool Create(/*const wxString& filename, */void* stream, const struct zlib_filefunc_def_s*, 
      const DBF_FIELD_INFO* array, dbf_uint array_count, 
      enum dbf_charconv charconv = dbf_charconv_compatible, void* memo = NULL);
   bool Create(wxOutputStream*, 
      const DBF_FIELD_INFO* array, dbf_uint array_count, 
      enum dbf_charconv charconv = dbf_charconv_compatible, void* memo = NULL);

   size_t Read(const DBF_FIELD* , wxString*, size_t buf_len = 1024);
   size_t Read(const char* field, wxString*, size_t buf_len = 1024);
   size_t Read(dbf_uint field   , wxString*, size_t buf_len = 1024);
   
   bool Read(const DBF_FIELD* , wxDateTime*);
   bool Read(const char* field, wxDateTime*);
   bool Read(dbf_uint field , wxDateTime*);
   
   bool Read(const DBF_FIELD* , bool*);   
   bool Read(const char* field, bool*);   
   bool Read(dbf_uint field , bool*);   
   
   bool Read(const DBF_FIELD* , long*);   
   bool Read(const char* field, long*);   
   bool Read(dbf_uint field , long*);   

   bool Read(const DBF_FIELD* , double*);   
   bool Read(const char* field, double*);   
   bool Read(dbf_uint field , double*);   

   bool Write(const DBF_FIELD* , const wxDateTime&, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(const char* field, const wxDateTime&, enum dbf_data_type type = DBF_DATA_TYPE_ANY);
   bool Write(dbf_uint field , const wxDateTime&, enum dbf_data_type type = DBF_DATA_TYPE_ANY);

   bool Write(const DBF_FIELD* , const bool&);
   bool Write(const char* field, const bool&);
   bool Write(dbf_uint field , const bool&);

   bool Write(const DBF_FIELD* , const wxString&);
   bool Write(const char* field, const wxString&);
   bool Write(dbf_uint field , const wxString&);

   bool Write(const DBF_FIELD* , long);
   bool Write(const char* field, long);
   bool Write(dbf_uint field , long);
   
   bool Write(const DBF_FIELD* , double);
   bool Write(const char* field, double);
   bool Write(dbf_uint field , double);

   bool Write(const char* field, const char*);

   void       GetInfo(DBF_INFO*, wxDateTime* dt = NULL) const;

   bool       Attach(/*const wxString& filename, */void* stream, struct zlib_filefunc_def_s*, 
                     enum dbf_editmode editmode = dbf_editmode_editable, enum dbf_charconv conv = dbf_charconv_compatible, void* memo = NULL);
   bool       Attach(DBF_HANDLE/*, const wxString& filename*/);
   bool       Attach(wxDBase*);
   bool       Attach(wxInputStream*, enum dbf_editmode editmode = dbf_editmode_editable, enum dbf_charconv conv = dbf_charconv_compatible, 
                     wxInputStream* memo = NULL, const wxString& tablename = wxEmptyString);
   DBF_HANDLE Detach(void);
   wxInputStream* DetachStream(void)
   {
      wxInputStream* stream = m_stream;
      m_stream = NULL;
      return stream;
   }

#ifdef _WX_VERSIONINFO_H_
   static wxVersionInfo GetVersionInfo();
#endif

// Implementation
public:
   virtual ~wxDBase();
   virtual void Close();
   virtual bool SetPosition(dbf_uint index);
   //virtual void SetFilename(const wxString&);
   //virtual wxString GetFilename(void) const;

// wxDataViewListModel "compatibility"
   wxString GetColType(unsigned int col);
   bool GetValueByRow(wxVariant*, unsigned int row, unsigned int col);
   bool SetValueByRow(const wxVariant&, unsigned int row, unsigned int col);
protected:
   virtual void DoDataExchange(bool bSaveAndValidate);
   
// Operations
public:
   void Fixups(void);
   bool Update(/*int index*/);
   static bool ParseDate(const wxString&, wxDateTime::Tm*, enum dbf_data_type);
};

#endif // __DBF_WX_H__
