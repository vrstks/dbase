// doc.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wxext.h"
#include "app.h"
#include "dbfdoc.h"
#include "appframe.h"
#include "wx29.h"
#include "../../bool.h"
#include "../../dbf.h"

class DocManager : public wxDocManagerEx
{
   typedef wxDocManagerEx base;
public:
   DocManager();
   virtual wxDocument * CreateDocument(const wxString& path, long flags);
protected:
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(DocManager, wxDocManagerEx)
	EVT_UPDATE_UI(wxID_ADD           , DocManager::OnUpdateDisableIfNoDoc)
	EVT_UPDATE_UI(wxID_FIND          , DocManager::OnUpdateDisableIfNoDoc)
	EVT_UPDATE_UI(wxID_DELETE        , DocManager::OnUpdateDisableIfNoDoc)
	EVT_UPDATE_UI(XRCID("edit")      , DocManager::OnUpdateDisableIfNoDoc)
	EVT_UPDATE_UI(XRCID("struct")    , DocManager::OnUpdateDisableIfNoDoc)
	EVT_UPDATE_UI(XRCID("pack")      , DocManager::OnUpdateDisableIfNoDoc)
END_EVENT_TABLE()

wxDocument* DocManager::CreateDocument(const wxString& path, long flags)
{
   wxDocument* doc = base::CreateDocument(path, flags);
   return doc;
}

DocManager::DocManager() : wxDocManagerEx()
{
   DatabaseDocTemplate::Create(this);
}

wxDocManager* App::CreateDocManager()
{
   DocManager* docManager = new DocManager();

   m_mru = new wxRecentFileList(docManager->GetFileHistory());
   return docManager;
}

/////////////////////////////////////////////////////////////////////////////
