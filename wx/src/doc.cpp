// appdoc.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "dbfdoc.h"
#include "dbfview.h"
#include "dbfframe.h"
#include "appframe.h"
#include "wxext.h"

class DocManager : public wxDocManager
{
public:
   virtual void OnOpenFileFailure()
   {
      wxDocManager::OnOpenFileFailure();
      //wxMessageBox(wxT("Bad file format"));
   }
   virtual wxDocument * CreateDocument(const wxString& path, long flags);
};

wxDocument* DocManager::CreateDocument(const wxString& path, long flags)
{
   return wxDBFDoc::CreateDocument(this, path, flags);
}

wxDocManager* App::CreateDocManager()
{
   wxDocManager* docManager = new DocManager();
   m_mru = new wxRecentFileList(docManager);
   wxDBFDoc::CreateDocTemplate(docManager);
   return docManager;
}

wxMDIChildFrame* App::NewFrame(wxDocument* doc, wxView* view)
{
   wxDocMDIChildFrame* subframe = new wxDBFFrame(doc, view, GetMainFrame());   
   return subframe;
}

