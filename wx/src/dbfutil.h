// dbfutil.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

class wxDBase;

extern wxString dbf_getstruct_c(const wxString& tablename, wxDBase*);
extern size_t dbf_getproperties(wxDBase* db, wxArrayString*, bool header = true);
