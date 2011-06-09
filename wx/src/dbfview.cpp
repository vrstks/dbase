// dbfview.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wxext.h"
#include "app.h"
#include "dbfview.h"
#include "dbfdoc.h"
#include "dbfdlgs.h"
#include "datalist.h"
#include "dbflist.h"
#include "dbfutil.h"
#include "wx29.h"
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
       return base::Create(parent, wxID_ANY, wxPoint(0,0), parent->GetClientSize(), wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS);
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
      frame->Show();
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
      case DBFDocument::ENUM_hint_initialupdate:
         GetWindow()->Init();
         ::wxListView_SetCurSel(GetWindow(), 0);
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

bool wxClipboard_Set(const wxString& str)
{
   bool ok = wxTheClipboard->Open();
   if (ok)
   {
      ok = wxTheClipboard->SetData( new wxTextDataObject(str) );
      wxTheClipboard->Close();
   }
   return ok;
}

void DBFView::OnStructClipboard(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   const wxString str = ::dbf_getstruct_c(doc->GetTablename(), doc->GetDatabase());
   bool ok = wxClipboard_Set(str);
   ::wxMessageBox(ok ? _("Struct is now on the Clipboard") : _("Failed to open clipboard"),
      wxMessageBoxCaption, wxOK | wxCENTRE, doc->GetDocumentWindow());
}

void DBFView::OnProperties(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   wxArrayString as;

   ::wxDocument_Info(doc, &as);

   wxDBFModel datamodel(doc->GetDatabase());
   datamodel.GetProperties(&as, true);

   ::wxMessageBox(::wxJoin(as, wxT('\n')), wxMessageBoxCaption, wxOK | wxCENTRE, GetFrame());
}

void DBFView::OnSelectAll(wxCommandEvent&)
{
   ::wxListCtrl_SelectAll(GetWindow());
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
   if (wxOK == wxMessageBox(_("Delete all?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      GetWindow()->DeleteAll(true);
   }
}

void DBFView::OnDelete(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete selection?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
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

