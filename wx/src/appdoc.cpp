// appdoc.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "app.h"
#include "dbfdoc.h"
#include "appframe.h"
#include "../../bool.h"
#include "../../dbf.h"

class DocManager : public wxDocManagerEx
{
   typedef wxDocManagerEx base;
public:
   DocManager();
   virtual wxDocument* CreateDocument(const wxString& path, long flags);
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
#if (wxVERSION_NUMBER >= 2901)
    return base::CreateDocument(path, flags);
#else
    size_t count = GetDocuments().size();
    wxDocument* doc = base::CreateDocument(path, flags);

    if (doc && (count == GetDocuments().size()))
    {
        wxTrunkDocView::ActivateDocument(doc); // restore if minimized mdi window
    }
    return doc;
#endif
}

DocManager::DocManager() : wxDocManagerEx()
{
    wxRecentFileList* mru = new wxRecentFileList(GetFileHistory());

    wxGetApp().m_mru = mru;
    DatabaseDocTemplate::Create(this, mru);
}

wxDocManager* App::CreateDocManager()
{
   DocManager* docManager = new DocManager();

   return docManager;
}

/////////////////////////////////////////////////////////////////////////////
