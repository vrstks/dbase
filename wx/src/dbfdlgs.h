// dbfdlgs.h
// Copyright (c) 2006 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

class wxDBase;
struct _DBF_FIELD_INFO;

extern bool DoModal_Structure(wxWindow* parent, wxDBase*, const wxString& caption = wxEmptyString, const wxString& filename = wxEmptyString);
extern bool DoModal_FieldEdit(wxWindow* parent, struct _DBF_FIELD_INFO*, const wxString& caption);
