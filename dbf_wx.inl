// dbf_wx.inl
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

inline wxDBase::wxDBase() : wxObject(), CDBase(), m_stream(NULL), m_stream_memo(NULL)
{
}

inline wxDBase::~wxDBase()
{
   if (IsOpen())
   {
      Close();
   }
   wxASSERT(m_stream == NULL);
   wxASSERT(m_stream_memo == NULL);
}

inline bool wxDBase::Open(const wxFileName& filename, enum dbf_editmode editmode)
{
   wxASSERT(!IsOpen());
   bool ok = base::Open(filename.GetFullPath().mb_str(), editmode);
   return ok;
}

inline bool wxDBase::Open(const wxFileName& filename, enum dbf_editmode editmode, const DBF_OPEN& parm)
{
   wxASSERT(!IsOpen());
   bool ok = base::Open(filename.GetFullPath().mb_str(), editmode, parm);
   return ok;
}

inline bool wxDBase::Create(const wxFileName& filename, const DBF_FIELD_INFO* array, dbf_uint array_count)
{
   wxASSERT(!IsOpen());
   bool ok = base::Create(filename.GetFullPath().mb_str(), array, array_count);
   return ok;
}

inline bool wxDBase::Create(const wxFileName& filename, const DBF_FIELD_INFO* array, dbf_uint array_count, const DBF_OPEN& parm)
{
   wxASSERT(!IsOpen());
   bool ok = base::Create(filename.GetFullPath().mb_str(), array, array_count, parm);
   return ok;
}

inline bool wxDBase::Create(void* stream, const struct zlib_filefunc_def_s* api, 
   const DBF_FIELD_INFO* array, dbf_uint array_count, 
   enum dbf_charconv charconv, void* memo)
{
   wxASSERT(!IsOpen());
   bool ok = base::Create(stream, api, array, array_count, charconv, memo);

   return ok;
}

inline bool wxDBase::Attach(void* stream, struct zlib_filefunc_def_s* api,
                            enum dbf_editmode editmode, enum dbf_charconv conv, void* memo)
{
   wxASSERT(!IsOpen());
   bool ok = base::Attach(stream, api, editmode, conv, memo);

   return ok;
}

inline bool wxDBase::Attach(DBF_HANDLE handle)
{
   wxASSERT(!IsOpen());
   bool ok = base::Attach(handle);

   return ok;
}

inline bool wxDBase::Attach(wxDBase* db)
{
   wxASSERT(!IsOpen());
   return Attach(db->Detach());
}

inline size_t wxDBase::Read(const DBF_FIELD* field, wxString* str, size_t buf_len)
{
   wxCharBuffer buf(buf_len);
   size_t ret = base::Read(field, buf.data(), buf_len);

   str->operator=(wxConvertMB2WX(buf));
   return ret;
}

inline size_t wxDBase::Read(dbf_uint field, wxString* str, size_t buf_len)
{
   return Read(GetFieldPtr(field), str, buf_len);
}

inline size_t wxDBase::Read(const char* field, wxString* str, size_t buf_len)
{
   return Read(GetFieldPtr(field), str, buf_len);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const wxString& str)
{
   return base::Write(field, str.mb_str());
}

inline bool wxDBase::Write(dbf_uint field, const wxString& str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool wxDBase::Write(const char* field, const wxString& str)
{
   return Write(GetFieldPtr(field), str);
}

inline bool wxDBase::Read(const DBF_FIELD* field, wxDateTime* dt)
{
   struct tm tm;
   int ms;
   bool ok = base::Read(field, &tm, &ms);

   if (ok && dt)
   {
      wxDateTime::Tm Tm(tm, wxDateTime::TimeZone());

      Tm.msec = ms;
      dt->Set(Tm); // don't use tm ctor, we want ms and years < 1970
      if (ms) dt->SetMillisecond(ms);
   }
   return ok;
}

inline bool wxDBase::Read(dbf_uint field, wxDateTime* dt)
{
   return Read(GetFieldPtr(field), dt);
}

inline bool wxDBase::Read(const char* field, wxDateTime* dt)
{
   return Read(GetFieldPtr(field), dt);
}

inline bool wxDBase::Write(dbf_uint field, const wxDateTime& dt, enum dbf_data_type type)
{
   return Write(GetFieldPtr(field), dt, type);
}

inline bool wxDBase::Write(const char* field, const wxDateTime& dt, enum dbf_data_type type)
{
   return Write(GetFieldPtr(field), dt, type);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const wxDateTime& dt, enum dbf_data_type type)
{
   bool ok = dt.IsValid();

   if (ok)
   {
      wxDateTime::Tm Tm = dt.GetTm();
      struct tm tm;

      tm.tm_sec  = Tm.sec;
      tm.tm_min  = Tm.min;
      tm.tm_hour = Tm.hour;
      tm.tm_mday = Tm.mday;
      tm.tm_mon  = Tm.mon;
      tm.tm_year = Tm.year - 1900;
      tm.tm_wday = 0;
      tm.tm_yday = 0;
      tm.tm_isdst = 0;

      ok = base::Write(field, tm, Tm.msec, type);
   }
   return ok;
}

inline void wxDBase::DoDataExchange(bool WXUNUSED(bSaveAndValidate))
{
}

inline bool wxDBase::SetPosition(dbf_uint index)
{
   bool ok = base::SetPosition(index);
   if (ok) Fixups();
   return ok;
}

inline void wxDBase::Fixups(void)
{
   DoDataExchange(false);
}

inline bool wxDBase::Update(/*int index*/)
{
   DoDataExchange(true);
   return PutRecord(/*index*/);
}

inline bool wxDBase::Read(const DBF_FIELD* field, bool* b)
{
   return base::Read(field, b);
}

inline bool wxDBase::Read(dbf_uint field, bool* b)
{
   return base::Read(field, b);
}

inline bool wxDBase::Read(const char* field, bool* b)
{
   return base::Read(field, b);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const bool& b)
{
   return base::Write(field, b);
}

inline bool wxDBase::Write(dbf_uint field, const bool& b)
{
   return base::Write(field, b);
}

inline bool wxDBase::Write(const char* field, const bool& b)
{
   return base::Write(field, b);
}

inline bool wxDBase::Write(const char* field, const char* str)
{
   return base::Write(field, str);
}

inline bool wxDBase::Read(const DBF_FIELD* field, long* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Read(const char* field, long* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Read(dbf_uint field     , long* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Read(const DBF_FIELD* field, double* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Read(const char* field, double* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Read(dbf_uint field     , double* value)
{
   return base::Read(field, value);
}

inline bool wxDBase::Write(const DBF_FIELD* field, long value)
{
   return base::Write(field, value);
}

inline bool wxDBase::Write(const char* field, long value)
{
   return base::Write(field, value);
}

inline bool wxDBase::Write(dbf_uint field, long value)
{
   return base::Write(field, value);
}

inline bool wxDBase::Write(const DBF_FIELD* field, double value)
{
   return base::Write(field, value);
}

inline bool wxDBase::Write(const char* field, double value)
{
   return base::Write(field, value);
}

inline bool wxDBase::Write(dbf_uint field, double value)
{
   return base::Write(field, value);
}

inline void wxDBase::GetInfo(DBF_INFO* info, wxDateTime* dt) const
{
   base::GetInfo(info);
   if (dt)
   {
      // avoid wxDateTime(time_t) ctor to enable linking
      // against "old" wx compilation (MSVC6+7: time_t = 32bits, MSVC8: time_t=64 bits)
      const struct tm* ptm = localtime(&info->lastupdate);

      if (ptm)
      {
         dt->Set(*ptm);
      }
   }
}

inline bool wxDBase::ParseDate(const wxString& buf, wxDateTime::Tm* dtm, enum dbf_data_type type)
{
   struct tm tm;
   int ms;
   bool ok = base::ParseDate(buf.mb_str(), &tm, &ms, type);

   if (ok)
   {
      wxDateTime::Tm temp(tm, wxDateTime::Local);

      temp.msec = ms;
      *dtm = temp;
   }
   return ok;
}
