// dbfview.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
//#include "app.h"
#include "dbfview.h"
#include "dbfdoc.h"
#include "dbfdlgs.h"
#include "datalist.h"
#include "dbflist.h"
#include "dbfutil.h"
#include "dbfmodel.h"

/////////////////////////////////////////////////////////////////////////////
// DBFWindow

class DBFWindow : public wxDBFListCtrl
{
   typedef wxDBFListCtrl base;
   DECLARE_CLASS(DBFWindow)
protected:
   wxDBFModel m_datamodel;
   DBFView* m_view;
public:
   DBFWindow(DBFView* view) : wxDBFListCtrl(), m_datamodel(view->GetDocument()->GetDatabase()), m_view(view)
   {
   }

   bool Create(wxWindow* parent)
   {
       bool ok = base::Create(parent, wxID_ANY, wxPoint(0,0), parent->GetClientSize(), wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS);
       SetAlternateRowColour();
       return ok;
   }

   virtual ~DBFWindow()
   {
   }
   virtual wxDataModelBase* GetModel(void)
   {
      return &m_datamodel;
   }
};

IMPLEMENT_CLASS(DBFWindow, wxDBFListCtrl)

/////////////////////////////////////////////////////////////////////////////
// DBFView

IMPLEMENT_DYNAMIC_CLASS(DBFView, wxViewEx)

DBFView::DBFView() : wxViewEx()
{
}

DBFView::~DBFView()
{
}

BEGIN_EVENT_TABLE(DBFView, wxViewEx)
   EVT_MENU(XRCID("struct"), DBFView::OnStruct)
   EVT_UPDATE_UI(XRCID("struct"), DBFView::OnUpdateNeedEditable)
   EVT_MENU(XRCID("tool_struct"), DBFView::OnStructClipboard)
   EVT_MENU(wxID_PROPERTIES, DBFView::OnProperties)

   EVT_MENU(wxID_SELECTALL       , DBFView::OnSelectAll)
   EVT_UPDATE_UI(wxID_SELECTALL  , DBFView::OnUpdateSelectAll)
   EVT_MENU(wxID_UNDELETE        , DBFView::OnUndelete)
   EVT_UPDATE_UI(wxID_UNDELETE   , DBFView::OnUpdateNeedSel_Deleted)
   EVT_MENU(wxID_ADD             , DBFView::OnAdd)
   EVT_UPDATE_UI(wxID_ADD        , DBFView::OnUpdateNeedEditable)
   EVT_UPDATE_UI(wxID_REPLACE    , DBFView::OnUpdateNeedEditable)
   EVT_MENU(wxID_DELETE          , DBFView::OnDelete)
   EVT_UPDATE_UI(wxID_DELETE     , DBFView::OnUpdateNeedSel_NotDeleted)
   EVT_MENU(wxID_CLEAR           , DBFView::OnDeleteAll)

   EVT_MENU(XRCID("edit")        , DBFView::OnEdit)
   EVT_UPDATE_UI(XRCID("edit")   , DBFView::OnUpdateNeedSel)

END_EVENT_TABLE()

bool DBFView::OnCreate(wxDocument* doc, long flags)
{
   bool ok = base::OnCreate(doc, flags);

   if (ok)
   {
      wxFrame* frame = wxStaticCast(doc->GetDocumentTemplate(), DatabaseDocTemplate)->CreateViewFrame(this);
      DBFWindow* wnd = new DBFWindow(this);

      wxASSERT(frame == GetFrame());
      wnd->Create(frame);
      SetWindow(wnd);
   }
   return ok;
}

DBFWindow* DBFView::GetWindow() const
{
    return wxStaticCast(base::GetWindow(), DBFWindow);
}

void DBFView::OnStruct(wxCommandEvent&)
{
   ::DoModal_Structure(GetFrame()->GetParent() /*wxTheApp->GetTopWindow()*/, GetDocument()->GetDatabase());
}

void DBFView::OnUpdateNeedEditable(wxUpdateUIEvent& event)
{
   event.Enable(GetDocument()->IsEditable());
}

void DBFView::OnUpdate(wxView* sender, wxObject* hint)
{
   switch ((long)hint)
   {
      case DBFDocument::hint_initialupdate:
         GetWindow()->Init();
         GetWindow()->SelectRow(0);
         break;
      default:
         base::OnUpdate(sender, hint);
         break;
   }
}

bool DBFView::OnClose(bool deleteWindow)
{
   bool ok = base::OnClose(deleteWindow);
   if (ok)
   {
      Activate(false);
      if (deleteWindow && GetFrame())
      {
         GetFrame()->Destroy();
         SetFrame(NULL);
      }
   }
   return ok;
}

DBFDocument* DBFView::GetDocument() const
{
   return wxStaticCast(base::GetDocument(), DBFDocument);
}

void DBFView::OnStructClipboard(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   const wxString str = ::dbf_getstruct_c(doc->GetTablename(), doc->GetDatabase());
   bool ok = wxClipboardHelper::SetText(str);

   ::wxMessageBox(ok ? _("Struct is now on the Clipboard") : _("Failed to open clipboard"),
      wxMessageBoxCaptionStr, wxOK | wxCENTRE, doc->GetDocumentWindow());
}

void DBFView::OnProperties(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   wxArrayString as;

   ::wxDocument_Info(doc, &as);

   wxDBFModel datamodel(doc->GetDatabase());
   datamodel.GetProperties(&as, true);

   ::wxMessageBox(::wxJoin(as, wxT('\n')), wxMessageBoxCaptionStr, wxOK | wxCENTRE, GetFrame());
}

void DBFView::OnSelectAll(wxCommandEvent&)
{
    GetWindow()->SelectAll();
}

void DBFView::OnUndelete(wxCommandEvent&)
{
   GetWindow()->DeleteSelection(false);
}

void DBFView::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
   GetWindow()->OnUpdateSelectAll(event);
}

void DBFView::OnUpdateNeedSel_Deleted(wxUpdateUIEvent& event)
{
   GetWindow()->OnUpdateNeedSel_Deleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
   GetWindow()->OnUpdateNeedSel_NotDeleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   GetWindow()->OnUpdateNeedSel(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnDeleteAll(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();

   if (wxOK == wxMessageBox(_("Delete all?"), wxMessageBoxCaptionStr, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      GetWindow()->DeleteAll(true);
   }
}

void DBFView::OnDelete(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete selection?"), wxMessageBoxCaptionStr, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      GetWindow()->DeleteSelection(true);
   }
}

void DBFView::OnAdd(wxCommandEvent&)
{
   GetWindow()->AddNew();
}

void DBFView::OnEdit(wxCommandEvent&)
{
   GetWindow()->Edit();
}
