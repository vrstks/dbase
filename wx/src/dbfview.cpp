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

IMPLEMENT_DYNAMIC_CLASS(wxDBFView, wxView)

wxDBFView::wxDBFView() : wxView(), m_list(NULL)
{
}

wxDBFView::~wxDBFView()
{
}

BEGIN_EVENT_TABLE(wxDBFView, wxView)
   EVT_MENU(XRCID("struct"), wxDBFView::OnStruct)
	EVT_UPDATE_UI(XRCID("struct"), wxDBFView::OnUpdateNeedEditable)
   EVT_MENU(XRCID("tool_struct"), wxDBFView::OnStructClipboard)
   EVT_MENU(wxID_PROPERTIES, wxDBFView::OnProperties)

   EVT_MENU(wxID_SELECTALL       , wxDBFView::OnSelectAll)
   EVT_UPDATE_UI(wxID_SELECTALL  , wxDBFView::OnUpdateSelectAll)
   EVT_MENU(wxID_UNDELETE        , wxDBFView::OnUndelete)
   EVT_UPDATE_UI(wxID_UNDELETE   , wxDBFView::OnUpdateNeedSel_Deleted)
   EVT_MENU(wxID_ADD             , wxDBFView::OnAdd)
   EVT_UPDATE_UI(wxID_ADD        , wxDBFView::OnUpdateNeedEditable)
   EVT_UPDATE_UI(wxID_REPLACE    , wxDBFView::OnUpdateNeedEditable)
   EVT_MENU(wxID_DELETE          , wxDBFView::OnDelete)
   EVT_UPDATE_UI(wxID_DELETE     , wxDBFView::OnUpdateNeedSel_NotDeleted)
   EVT_MENU(wxID_CLEAR           , wxDBFView::OnDeleteAll)

   EVT_MENU(XRCID("edit")        , wxDBFView::OnEdit)
   EVT_UPDATE_UI(XRCID("edit")   , wxDBFView::OnUpdateNeedSel)

END_EVENT_TABLE()

class MyDBFListCtrl : public wxDBFListCtrl
{
protected:
   wxDBFDoc* m_doc;
   wxDBFModel m_datamodel;
public:
   MyDBFListCtrl(wxWindow* parent, wxDBFDoc* doc) : 
      wxDBFListCtrl(parent, wxPoint(0, 0), parent->GetClientSize(), wxLC_REPORT | wxLC_VIRTUAL | wxLC_EDIT_LABELS),
         m_doc(doc), m_datamodel(doc->GetDatabase())
   {
   }
   virtual wxDataModelBase* GetModel(void)
   {
      return &m_datamodel;
   }
};

bool wxDBFView::OnCreate(wxDocument* doc, long WXUNUSED(flags) )
{
   wxASSERT(doc->GetFirstView() == this);
   wxMDIChildFrame* frame = wxGetApp().NewFrame(doc);
   SetFrame(frame);
   m_list = new MyDBFListCtrl(frame, wxStaticCast(doc, wxDBFDoc));
   frame->Show(true);
   Activate(true);
   return true;
}

void wxDBFView::OnStruct(wxCommandEvent&)
{
   ::DoModal_Structure(GetFrame()->GetParent() /*wxTheApp->GetTopWindow()*/, GetDocument()->GetDatabase());
}

void wxDBFView::OnUpdateNeedEditable(wxUpdateUIEvent& event)
{
   event.Enable(GetDocument()->IsEditable());
}

void wxDBFView::OnDraw(wxDC *WXUNUSED(dc) )
{
}

void wxDBFView::OnUpdate(wxView *WXUNUSED(sender), wxObject* hint)
{
   switch ((long)hint)
   {
      case wxDBFDoc::ENUM_hint_initialupdate:
         m_list->Init();
         ::wxListView_SetCurSel(m_list, 0);
         break;
   }
}

bool wxDBFView::OnClose(bool deleteWindow)
{
   bool ok = base::OnClose(deleteWindow);
   if (ok)
   {
      Activate(false);
      if (deleteWindow)
      {
         delete GetFrame();
         SetFrame(NULL);
      }
   }
   return ok;
}

wxDBFDoc* wxDBFView::GetDocument() const
{
   return wxStaticCast(base::GetDocument(), wxDBFDoc);
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

void wxDBFView::OnStructClipboard(wxCommandEvent&)
{
   const wxDBFDoc* doc = GetDocument();
   const wxString str = ::dbf_getstruct_c(doc->GetTablename(), doc->GetDatabase());
   bool ok = wxClipboard_Set(str);
   ::wxMessageBox(ok ? _("Struct is now on the Clipboard") : _("Failed to open clipboard"),
      wxMessageBoxCaption, wxOK | wxCENTRE, doc->GetDocumentWindow());
}

void wxDBFView::OnProperties(wxCommandEvent&)
{
   const wxDBFDoc* doc = GetDocument();
   wxArrayString as;
   //::dbf_getproperties(GetDocument()->GetDatabase(), &as);

   ::wxDocument_Info(doc, &as);

   wxDBFModel datamodel(doc->GetDatabase());
   datamodel.GetProperties(&as, true);

   wxString str = wxJoin(as, wxT('\n'));
   ::wxMessageBox(str, wxMessageBoxCaption, wxOK | wxCENTRE, doc->GetDocumentWindow());
}

void wxDBFView::OnSelectAll(wxCommandEvent&)
{
   ::wxListCtrl_SelectAll(m_list);
}

void wxDBFView::OnUndelete(wxCommandEvent&)
{
   m_list->DeleteSelection(false);
}

void wxDBFView::OnUpdateSelectAll(wxUpdateUIEvent& event)
{
   m_list->OnUpdateSelectAll(event);
}

void wxDBFView::OnUpdateNeedSel_Deleted(wxUpdateUIEvent& event)
{
   m_list->OnUpdateNeedSel_Deleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void wxDBFView::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
   m_list->OnUpdateNeedSel_NotDeleted(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void wxDBFView::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   m_list->OnUpdateNeedSel(event);
   if (!GetDocument()->IsEditable()) event.Enable(false);
}

void wxDBFView::OnDeleteAll(wxCommandEvent&)
{
   const wxDBFDoc* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete all?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      m_list->DeleteAll(true);
   }
}

void wxDBFView::OnDelete(wxCommandEvent&)
{
   const wxDBFDoc* doc = GetDocument();
   if (wxOK == wxMessageBox(_("Delete selection?"), wxMessageBoxCaption, wxOK | wxCANCEL | wxICON_QUESTION, doc->GetDocumentWindow()))
   {
      m_list->DeleteSelection(true);
   }
}

void wxDBFView::OnAdd(wxCommandEvent&)
{
	m_list->AddNew();
}

void wxDBFView::OnEdit(wxCommandEvent&)
{
   m_list->Edit();
}

