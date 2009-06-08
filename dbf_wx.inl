// dbf_wx.inl

inline wxDBase::wxDBase() : wxObject(), CDBase()
{
}

inline wxDBase::~wxDBase()
{
}

inline void wxDBase::Close()
{
   CDBase::Close();
   //SetFilename(wxEmptyString);
}

inline bool wxDBase::Open(const wxString& filename, bool editable, enum dbf_charconv charconv)
{
   bool ok = CDBase::Open(filename.mb_str(), editable, charconv);
   //if (ok) SetFilename(filename);
   return ok;
}

inline bool wxDBase::Create(const wxString& filename, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv)
{
   bool ok = CDBase::Create(filename.mb_str(), array, array_count, charconv);
   //if (ok) SetFilename(filename);
   return ok;
}

inline bool wxDBase::Create(/*const wxString& filename, */void* stream, struct zlib_filefunc_def_s* api, 
   const DBF_FIELD_INFO* array, size_t array_count, 
   enum dbf_charconv charconv, void* memo)
{
   bool ok = CDBase::Create(stream, api, array, array_count, charconv, memo);
   //if (ok) SetFilename(filename);
   return ok;
}

inline bool wxDBase::Attach(/*const wxString& filename, */void* stream, struct zlib_filefunc_def_s* api,
                            bool editable, enum dbf_charconv conv, void* memo)
{
   bool ok = CDBase::Attach(stream, api, editable, conv, memo);
   //if (ok) SetFilename(filename);
   return ok;
}

inline bool wxDBase::Attach(DBF_HANDLE handle/*, const wxString& filename*/)
{
   bool ok = CDBase::Attach(handle);
   //if (ok) SetFilename(filename);
   return ok;
}

inline bool wxDBase::Attach(wxDBase* db)
{
   //const wxString filename = db->GetFilename();
   return Attach(db->Detach()/*, filename*/);
}

inline DBF_HANDLE wxDBase::Detach(void)
{
   //SetFilename(wxEmptyString);
   return CDBase::Detach();
}

inline size_t wxDBase::Read(const DBF_FIELD* field, wxString* str, size_t buf_len)
{
   wxCharBuffer buf(buf_len);
   size_t ret = CDBase::Read(field, buf.data(), buf_len);
   str->operator=(wxConvertMB2WX(buf));
   return ret;
}

inline size_t wxDBase::Read(size_t field, wxString* str, size_t buf_len)
{
   return Read(GetFieldPtr(field), str, buf_len);
}

inline size_t wxDBase::Read(const char* field, wxString* str, size_t buf_len)
{
   return Read(GetFieldPtr(field), str, buf_len);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const wxString& str)
{
   return CDBase::Write(field, str.mb_str());
}

inline bool wxDBase::Write(size_t field, const wxString& str)
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
   bool ok = CDBase::ReadDateTime(field, &tm, &ms);
   if (ok && dt)
   {
      wxDateTime::Tm Tm(tm, wxDateTime::TimeZone());
      Tm.msec = ms;
/*      
      Tm.sec  = tm.tm_sec;
      Tm.min  = tm.tm_min;
      Tm.hour = tm.tm_hour;
      Tm.mday = tm.tm_mday;
      Tm.mon  = (enum wxDateTime::Month)tm.tm_mon;
      Tm.year = tm.tm_year + 1900;
      Tm.msec = ms; // we want ms *and* years < 1970
      */
      dt->Set(Tm); // don't use tm ctor, we want ms and years < 1970
      if (ms) dt->SetMillisecond(ms);
   }
   return ok;
}

inline bool wxDBase::Read(size_t field, wxDateTime* dt)
{
   return Read(GetFieldPtr(field), dt);
}

inline bool wxDBase::Read(const char* field, wxDateTime* dt)
{
   return Read(GetFieldPtr(field), dt);
}

inline bool wxDBase::Write(size_t field, const wxDateTime& dt)
{
   return Write(GetFieldPtr(field), dt);
}

inline bool wxDBase::Write(const char* field, const wxDateTime& dt)
{
   return Write(GetFieldPtr(field), dt);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const wxDateTime& dt)
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

      ok = CDBase::WriteDateTime(field, &tm);
   }
   return ok;
}

inline void wxDBase::DoDataExchange(bool WXUNUSED(bSaveAndValidate))
{
}

inline bool wxDBase::SetPosition(size_t index)
{
   bool ok = CDBase::SetPosition(index);
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

inline bool wxDBase::Read(size_t field, bool* b)
{
   return CDBase::Read(field, b);
}

inline bool wxDBase::Read(const char* field, bool* b)
{
   return CDBase::Read(field, b);
}

inline bool wxDBase::Write(const DBF_FIELD* field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool wxDBase::Write(size_t field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool wxDBase::Write(const char* field, const bool& b)
{
   return CDBase::Write(field, b);
}

inline bool wxDBase::Write(const char* field, const char* str)
{
   return CDBase::Write(field, str);
}

inline bool wxDBase::Read(const DBF_FIELD* field, long* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Read(const char* field, long* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Read(size_t field     , long* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Read(const DBF_FIELD* field, double* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Read(const char* field, double* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Read(size_t field     , double* value)
{
   return CDBase::Read(field, value);
}

inline bool wxDBase::Write(const DBF_FIELD* field, long value)
{
   return CDBase::Write(field, value);
}

inline bool wxDBase::Write(const char* field, long value)
{
   return CDBase::Write(field, value);
}

inline bool wxDBase::Write(size_t field, long value)
{
   return CDBase::Write(field, value);
}

inline bool wxDBase::Write(const DBF_FIELD* field, double value)
{
   return CDBase::Write(field, value);
}

inline bool wxDBase::Write(const char* field, double value)
{
   return CDBase::Write(field, value);
}

inline bool wxDBase::Write(size_t field, double value)
{
   return CDBase::Write(field, value);
}

inline wxDateTime wxDBase::GetLastUpdate(wxString* str) const
{
   // avoid wxDateTime(time_t) ctor to enable compilation
   // against "old" wx compilation (MSVC6+7: time_t = 32bits, MSVC8: time_t=64 bits)
   const time_t utc = CDBase::GetLastUpdate();
   const struct tm* ptm = localtime(&utc);
   wxDateTime t;
   if (ptm)
   {
      t.Set(*ptm);
      if (str) str->operator=(t.Format(wxT("%x")));
   }
   return t;
}

