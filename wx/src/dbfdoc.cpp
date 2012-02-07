// dbfdoc.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfdlgs.h"

#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"
#include "dbfdoc.h"
#include "wxext.h"

IMPLEMENT_DYNAMIC_CLASS(DBFDocument, wxDocument)

DBFDocument::DBFDocument(void) : wxDocument(), m_database(new wxDBase)
{
}

DBFDocument::~DBFDocument(void)
{
   delete m_database;
}

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
bool DBFDocument::DoSaveDocument(const wxString& WXUNUSED(filename))
{
    /*
    DBFView* view = (DBFView*)GetFirstView();
    if (!view->textsw->SaveFile(filename))
        return false;
        */
    Modify(false);
    return true;
}

bool DBFDocument::DoOpenDocument(const wxString& path)
{
   wxFileName filename(path);
   bool ok = filename.FileExists();
   if (ok)
   {
      ok = m_database->Open(filename, true, ENUM_dbf_charconv_compatible);
      if (!ok) ok = m_database->Open(filename, false, ENUM_dbf_charconv_compatible);
      if (ok)
      {
         UpdateAllViews(NULL, (wxObject*)(long)ENUM_hint_initialupdate);
         m_tablename = filename.GetName();
      }
      else
      {
         wxMessageBox(_("Bad file format"));
      }
   }
   else
   {
      wxMessageBox(_("File not found"));
   }
   return ok;
}

bool DBFDocument::IsModified(void) const
{
   return false && m_database->IsOpen() && m_database->IsModified();
}

void DBFDocument::Modify(bool mod)
{
   base::Modify(mod);
}

bool DBFDocument::OnCloseDocument()
{
   bool ok = base::OnCloseDocument();
   if (m_database->IsOpen()) m_database->Close();
   return ok;
}

bool DBFDocument::IsEditable(void) const
{
   return m_database->IsOpen() && m_database->IsEditable();
}

bool DBFDocument::OnNewDocument()
{
   bool ok = base::OnNewDocument();
   if (ok)
   {
      wxDBase database;

      ok = Save();
      if (ok) ok = ::DoModal_Structure(wxTheApp->GetTopWindow(), &database, _("New database structure"), GetFilename());
      if (ok && database.IsOpen())
      {
         m_database->Attach(&database);
         UpdateAllViews(NULL, (wxObject*)(long)ENUM_hint_initialupdate);
      }
   }
   return ok;
}

bool DBFDocument::SaveAs()
{
   bool ok = base::SaveAs();
   if (ok)
   {
   }
   return ok;
}

/////////////////////////////////////////////////////////////////////////////
// DatabaseDocTemplate

#include "dbfview.h"
#include "dbfframe.h"
#include "../art/database.xpm"

IMPLEMENT_CLASS(DatabaseDocTemplate, wxDocTemplate)

/*static*/ DatabaseDocTemplate* DatabaseDocTemplate::ms_instance = NULL;

DatabaseDocTemplate::DatabaseDocTemplate(wxDocManager* docManager,
                                         wxClassInfo* docClassInfo,
                                         wxClassInfo* viewClassInfo,
                                         wxClassInfo* frameClassInfo,
                                         wxRecentFileList* mru) : wxDocTemplate(docManager, 
        wxString::Format(_("%s Files"), wxT(DBF_FORMAT_NAME)), wxT("*.")wxT(DBF_FILEEXT),
        wxT(""), wxT(DBF_FILEEXT), wxT("dbf doc"), wxT("dbf view"),
        docClassInfo, viewClassInfo), m_frameClassInfo(frameClassInfo), m_mru(mru)
{
    ms_instance = this;
}

/*static*/ DatabaseDocTemplate* DatabaseDocTemplate::Create(wxDocManager* docManager, wxRecentFileList* mru)
{
   return new DatabaseDocTemplate(docManager, CLASSINFO(DBFDocument), CLASSINFO(DBFView), CLASSINFO(DBFFrame), mru);
}

/*static*/ wxIcon DatabaseDocTemplate::GetIcon()
{
    return wxIcon(database_xpm);
}

wxFrame* DatabaseDocTemplate::CreateViewFrame(wxView* view)
{
    DBFFrame* subframe = wxStaticCast(m_frameClassInfo->CreateObject(), DBFFrame);

    if (subframe->Create(view->GetDocument(), wxStaticCast(wxTheApp->GetTopWindow(), wxMDIParentFrame), m_mru))
    {
        subframe->SetIcon(GetIcon());
    }
    else
    {
        wxDELETE(subframe);
    }
    return subframe;
}

/////////////////////////////////////////////////////////////////////////////
