// dbfdoc.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
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

bool wxDBFDoc::DoOpenDocument(const wxString& path)
{
   wxFileName filename(path);
   bool ok = m_database->Open(filename, true, ENUM_dbf_charconv_compatible);
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
   return ok;
}

bool wxDBFDoc::IsModified(void) const
{
   return false && m_database->IsOpen() && m_database->IsModified();
}

void wxDBFDoc::Modify(bool mod)
{
   base::Modify(mod);
}

bool wxDBFDoc::OnCloseDocument()
{
   bool ok = base::OnCloseDocument();
   if (m_database->IsOpen()) m_database->Close();
   return ok;
}

bool wxDBFDoc::IsEditable(void) const
{
   return m_database->IsOpen() && m_database->IsEditable();
}

bool wxDBFDoc::OnNewDocument()
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

bool wxDBFDoc::SaveAs()
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

DatabaseDocTemplate::DatabaseDocTemplate(wxDocManager* docManager) : wxDocTemplate(docManager, _("dBASE Files"), wxT("*.")wxT(FILEEXT_DBASE),
      wxT(""), wxT(FILEEXT_DBASE), wxT("dbf doc"), wxT("dbf view"),
          CLASSINFO(wxDBFDoc), CLASSINFO(wxDBFView))
{
}

/*static*/ DatabaseDocTemplate* DatabaseDocTemplate::Create(wxDocManager* docManager)
{
   return new DatabaseDocTemplate(docManager);
}

wxFrame* DatabaseDocTemplate::CreateViewFrame(wxView* view)
{
   wxDocMDIChildFrame* frame = new wxDBFFrame(view->GetDocument(), wxStaticCast(wxTheApp->GetTopWindow(), wxMDIParentFrame));
   return frame;
}

/////////////////////////////////////////////////////////////////////////////
