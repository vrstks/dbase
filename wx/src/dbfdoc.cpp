// dbfdoc.cpp
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"
#include "dbfdoc.h"
#include "wx/ext/wx.h"
#include "wx/ext/trunk.h"
#include "dbfdlgs.h"
#include "wxstreamc.h"

IMPLEMENT_DYNAMIC_CLASS(DBFDocument, wxDocument)

DBFDocument::DBFDocument(void) : wxDocument(), m_database(new wxDBase()), m_stream(NULL)
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

bool DBFDocument::AttachStream(wxInputStream& stream, const wxString& filePath)
{
    const wxFileName fileName(filePath);
    struct zlib_filefunc_def_s api;
    bool ok;
    const wxString tablename = fileName.GetName();

    ::fill_filefunc(&api, &stream);

    ok = m_database->Attach(&stream, &api, dbf_editmode_readonly, dbf_charconv_default, NULL, tablename);
    if (ok)
    {
        m_tablename = tablename;
        m_stream = &stream;
        SetTitle(fileName.GetFullPath());
        SetFilename(filePath, true);
        wxFileLoadedHint hint;
        UpdateAllViews(NULL, &hint);
    }
    else
        wxLogError(_("Bad file format in file \"%s\"."), fileName.GetFullPath().wx_str());
    return ok;
}

bool DBFDocument::DeleteContents()
{
    bool ok = base::DeleteContents();
    wxDELETE(m_stream);
    if (m_database->IsOpen())
        m_database->Close();
    return ok;
}

bool DBFDocument::DoOpenDocument(const wxString& filePath)
{
    const wxFileName fileName(filePath);
    //wxFileInputStream stream(fileName.GetFullPath());
    //bool ok = stream.IsOk();
    bool ok = fileName.FileExists();

    if (ok)
    {
        ok = m_database->Open(fileName, dbf_editmode_editable);
        if (!ok)
            ok = m_database->Open(fileName, dbf_editmode_readonly);
        if (ok)
        {
            m_tablename = fileName.GetName();
            SetTitle(fileName.GetFullPath());
            wxFileLoadedHint hint;
            UpdateAllViews(NULL, &hint);
        }
        else
            wxLogError(_("Bad file format in file \"%s\"."), fileName.GetFullPath().wx_str());
    }
    else
        wxLogError(_("Failed to open the file \"%s\" for reading."), fileName.GetFullPath().wx_str());
    return ok;
}

bool DBFDocument::IsModified(void) const
{
   //return m_database->IsOpen() && m_database->IsModified();
    return base::IsModified();
}

void DBFDocument::Modify(bool mod)
{
   base::Modify(mod);
}

bool DBFDocument::OnCloseDocument()
{
   bool ok = base::OnCloseDocument();

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
      if (ok)
          ok = ::DoModal_Structure(wxTheApp->GetTopWindow(), &database, _("New database structure"), GetFilename());
      if (ok && database.IsOpen())
      {
          m_database->Attach(&database);
          wxFileLoadedHint hint;
          UpdateAllViews(NULL, &hint);
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

/*static*/ const wxDocTemplateClassInfo DatabaseDocTemplate::m_DefaultClassInfo(wxCLASSINFO(DBFDocument), wxCLASSINFO(DBFView), wxCLASSINFO(DBFFrame));

DatabaseDocTemplate::DatabaseDocTemplate(wxDocManager* docManager,
                                         const wxDocTemplateClassInfo& classInfo,
                                         wxRecentFileList* mru) : wxDocTemplate(docManager, 
        wxString::Format(_("%s Files"), wxT(DBF_FORMAT_NAME)), wxT("*.")wxT(DBF_FILEEXT),
        wxT(""), wxT(DBF_FILEEXT), wxT("dbf doc"), wxT("dbf view"),
        classInfo.m_doc, classInfo.m_view), m_frameClassInfo(classInfo.m_frame), m_mru(mru)
{
    ms_instance = this;
}

/*static*/ DatabaseDocTemplate* DatabaseDocTemplate::Create(wxDocManager* docManager, wxRecentFileList* mru)
{
   return new DatabaseDocTemplate(docManager, m_DefaultClassInfo, mru);
}

/*static*/ wxIcon DatabaseDocTemplate::GetIcon()
{
    return wxIcon(database_xpm);
}

wxFrame* DatabaseDocTemplate::CreateViewFrame(wxView* view)
{
    DBFFrame* subframe = wxStaticCast(m_frameClassInfo->CreateObject(), DBFFrame);

    if (subframe->Create(wxStaticCast(wxTheApp->GetTopWindow(), wxMDIParentFrame), view, m_mru))
        subframe->SetIcon(GetIcon());
    else
        wxDELETE(subframe);
    return subframe;
}

/////////////////////////////////////////////////////////////////////////////
