// dbfview.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "app.h"
#include "dbfview.h"
#include "dbfdoc.h"
#include "dbfdlgs.h"
#include "datalist.h"
#include "dbflist.h"
#include "dbfutil.h"
#include "wx29.h"
#include "wxext.h"
#include "dbfmodel.h"

/////////////////////////////////////////////////////////////////////////////
// DBFWindow

class DBFWindow : public wxDBFListCtrl
{
protected:
   wxDBFModel m_datamodel;
   DBFView* m_view;
public:
   DBFWindow(DBFView* view) : 
      wxDBFListCtrl(view->GetFrame(), wxPoint(0,0), view->GetFrame()->GetClientSize(), wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS),
         m_datamodel(view->GetDocument()->GetDatabase()), m_view(view)
   {
   }
   virtual ~DBFWindow()
   {
      m_view->m_wnd = NULL;
   }
   virtual wxDataModelBase* GetModel(void)
   {
      return &m_datamodel;
   }
};

/////////////////////////////////////////////////////////////////////////////
// DBFView

IMPLEMENT_DYNAMIC_CLASS(DBFView, wxView)

DBFView::DBFView() : wxView(), m_wnd(NULL)
{
}

DBFView::~DBFView()
{
   if (m_wnd)
   {
      m_wnd->Destroy();
   }
}

BEGIN_EVENT_TABLE(DBFView, wxView)
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
      wxASSERT(frame == GetFrame());
      m_wnd = new DBFWindow(this);
      frame->Show();
   }
   return ok;
}

void DBFView::OnStruct(wxCommandEvent&)
{
   ::DoModal_Structure(GetFrame()->GetParent() /*wxTheApp->GetTopWindow()*/, GetDocument()->GetDatabase());
}

void DBFView::OnUpdateNeedEditable(wxUpdateUIEvent& event)
{
   event.Enable(GetDocument()->IsEditable());
}

void DBFView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

void DBFView::OnUpdate(wxView* sender, wxObject* hint)
{
   switch ((long)hint)
   {
      case DBFDocument::ENUM_hint_initialupdate:
         m_wnd->Init();
         ::wxListView_SetCurSel(m_wnd, 0);
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
   //::dbf_getproperties(GetDocument()->GetDatabase(), &as);

   ::wxDocument_Info(doc, &as);

   wxDBFModel datamodel(doc->GetDatabase());
   datamodel.GetProperties(&as, true);

   wxString str = wxJoin(as, wxT('\n'));
   ::wxMessageBox(str, wxMessageBoxCaption, wxOK | wxCENTRE, doc->GetDocumentWindow());
}

void DBFView::OnSelectAll(wxCommandEvent&)
{
   ::wxListCtrl_SelectAll(m_wnd);
}

void DBFView::OnUndelete(wxCommandEvent&)
{
   m_wnd->DeleteSelection(false);
}

void DBFView::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
   m_wnd->OnUpdateSelectAll(event);
}

void DBFView::OnUpdateNeedSel_Deleted(wxUpdateUIEvent& event)
{
   m_wnd->OnUpdateNeedSel_Deleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
   m_wnd->OnUpdateNeedSel_NotDeleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   m_wnd->OnUpdateNeedSel(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void DBFView::OnDeleteAll(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete all?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      m_wnd->DeleteAll(true);
   }
}

void DBFView::OnDelete(wxCommandEvent&)
{
   const DBFDocument* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete selection?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      m_wnd->DeleteSelection(true);
   }
}

void DBFView::OnAdd(wxCommandEvent&)
{
	m_wnd->AddNew();
}

void DBFView::OnEdit(wxCommandEvent&)
{
   m_wnd->Edit();
}

