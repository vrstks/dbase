// appdoc.cpp
// Copyright (c) 2007-2008 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "../../dbf_wx.h" // wxDBFDocument
#include "dbfdoc.h"
#include "dbfview.h"
#include "dbfframe.h"
#include "appframe.h"

class wxDBFDocManager : public wxDocManager
{
public:
   virtual void OnOpenFileFailure()
   {
      wxDocManager::OnOpenFileFailure();
      //wxMessageBox(wxT("Bad file format"));
   }
   virtual wxDocument * CreateDocument(const wxString& path, long flags);
};

wxDocument* wxDBFDocManager::CreateDocument(const wxString& path, long flags)
{
   return wxDBFDoc::CreateDocument(this, path, flags);
}

void App::AddDocTemplates()
{
   m_docManager = new wxDBFDocManager;

   wxDBFDoc::CreateDocTemplate(m_docManager);
}

wxMDIChildFrame* App::NewFrame(wxDocument* doc, wxView* view)
{
   wxDocMDIChildFrame *subframe =
      new wxDBFFrame(doc, view, GetMainFrame());
   
   return subframe;
}

