// dbfdoc.cpp
// Copyright (c) 2007 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfview.h"
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

IMPLEMENT_DYNAMIC_CLASS(wxDBFDoc, wxDocument)

wxDBFDoc::wxDBFDoc(void) : wxDocument(), m_database(new wxDBase)
{
}

wxDBFDoc::~wxDBFDoc(void)
{
   wxDELETE(m_database);
}

// Since text windows have their own method for saving to/loading from files,
// we override OnSave/OpenDocument instead of Save/LoadObject
bool wxDBFDoc::DoSaveDocument(const wxString& WXUNUSED(filename))
{
    /*
    wxDBFView* view = (wxDBFView*)GetFirstView();
    if (!view->textsw->SaveFile(filename))
        return false;
        */
    Modify(false);
    return true;
}

bool wxDBFDoc::DoOpenDocument(const wxString& filename)
{
   bool ok = m_database->Open(filename, true, ENUM_dbf_charconv_compatible);
   if (!ok) ok = m_database->Open(filename, false, ENUM_dbf_charconv_compatible);
   if (ok)
   {
      UpdateAllViews(NULL, (wxObject*)(long)ENUM_hint_initialupdate);
      wxFileName::SplitPath(filename, NULL, &m_tablename, NULL);
   }
   else
   {
      wxMessageBox(_("Bad file format"));
   }
   return ok;
}

bool wxDBFDoc::GetPrintableName(wxString& buf) const
{
   buf = m_documentFile;
   return true;
}

bool wxDBFDoc::IsModified(void) const
{
   return false && m_database->IsOpen() && m_database->IsModified();
}

void wxDBFDoc::Modify(bool mod)
{
   wxDocument::Modify(mod);
}

bool wxDBFDoc::OnCloseDocument()
{
   bool ok = wxDocument::OnCloseDocument();
   if (m_database->IsOpen()) m_database->Close();
   return ok;
}

bool wxDBFDoc::IsEditable(void) const
{
   return m_database->IsOpen() && m_database->IsEditable();
}

/*static*/ wxDocTemplate* wxDBFDoc::CreateDocTemplate(wxDocManager* docManager)
{
   return new wxDocTemplate(docManager, wxT("dBASE III Files"), wxT("*.")wxT(FILEEXT_DBASE),
      wxT(""), wxT(FILEEXT_DBASE), wxT("dbf doc"), wxT("dbf view"),
          CLASSINFO(wxDBFDoc), CLASSINFO(wxDBFView));
}

/*static*/ wxDocument* wxDBFDoc::CreateDocument(wxDocManager* docManager, const wxString& path, long flags)
{
   wxDBFDoc* doc = NULL;
   wxDBase database;
   wxBusyCursor wc;
   bool ok = path.IsEmpty() || wxFileExists(path);
   if (ok)
   {
      doc = (wxDBFDoc*)docManager->wxDocManager::CreateDocument(path, flags);
      if (doc && (wxDOC_NEW == flags))
      {
         ok = doc->Save();
         if (ok) ok = ::DoModal_Structure(wxTheApp->GetTopWindow(), &database, _("New database structure"), doc->GetFilename());
         if (!ok)
         {
            doc->DeleteAllViews();
            doc = NULL;
         }
      }
      if (doc && database.IsOpen())
      {
         doc->m_database->Attach(&database);
         if (wxDOC_NEW == flags) doc->UpdateAllViews(NULL, (wxObject*)(long)ENUM_hint_initialupdate);
      }
   }
   else
   {
      wxMessageBox(_("File not found"));
   }
   return doc;
}

