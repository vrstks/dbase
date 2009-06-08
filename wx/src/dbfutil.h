// dbfutil.h

class wxDBase;

extern wxString dbf_getstruct_c(const wxString& tablename, wxDBase*);
extern size_t dbf_getproperties(wxDBase* db, wxArrayString*, bool header = true);
