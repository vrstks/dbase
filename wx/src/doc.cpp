// doc.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "dbfdoc.h"
#include "dbfview.h"
#include "dbfframe.h"
#include "appframe.h"
#include "wxext.h"
#include "../../bool.h"
#include "../../dbf.h"

class DocManager : public wxDocManager
{
   typedef wxDocManager base;
public:
   virtual wxDocument * CreateDocument(const wxString& path, long flags);
};

wxDocument* DocManager::CreateDocument(const wxString& path, long flags)
{
   //wxDocument* doc = base::CreateDocument(path, flags);
   wxDocument* doc = wxDBFDoc::CreateDocument(this, wxFileName(path), flags);
   if (doc && (wxDOC_NEW != flags))
   {
      AddFileToHistory(doc->GetFilename());
      ::wxDocument_SetTitleFullPath(doc);
   }
   return doc;
}

wxDocManager* App::CreateDocManager()
{
   wxDocManager* docManager = new DocManager();
   m_mru = new wxRecentFileList(docManager);
   new wxDocTemplate(docManager, wxT("dBASE III Files"), wxT("*.")wxT(FILEEXT_DBASE),
      wxT(""), wxT(FILEEXT_DBASE), wxT("dbf doc"), wxT("dbf view"),
          CLASSINFO(wxDBFDoc), CLASSINFO(wxDBFView));
   return docManager;
}

wxMDIChildFrame* App::NewFrame(wxDocument* doc, wxView* view)
{
   wxDocMDIChildFrame* subframe = new wxDBFFrame(doc, view, GetMainFrame());   
   return subframe;
}
