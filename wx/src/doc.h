// doc.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

/////////////////////////////////////////////////////////////////////////////
// DatabaseDocTemplate

class DatabaseDocTemplate : public wxDocTemplate
{
   DECLARE_CLASS(DatabaseDocTemplate)
private:
   DatabaseDocTemplate(wxDocManager*);
public:
   wxFrame* GetViewFrame(wxView*);

   static DatabaseDocTemplate* Create(wxDocManager*);
};

/////////////////////////////////////////////////////////////////////////////
