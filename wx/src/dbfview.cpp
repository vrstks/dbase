// dbfview.cpp
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include <wx/numdlg.h>
#include <wx/wfstream.h>
#include <wx/html/htmprint.h>

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"
#include "dbfview.h"
#include "dbfdoc.h"
#include "dbfdlgs.h"
#include "datamodel.h"
#include "datalist.h"
#include "dbfutil.h"
#include "dbfmodel.h"
#include "dbflist.h"
#include "printout.h"

/////////////////////////////////////////////////////////////////////////////
// DBFWindow

class DBFWindow : public DBFListCtrl
{
    typedef DBFListCtrl base;
    wxDECLARE_CLASS(DBFWindow);
protected:
    DBFModel m_datamodel;
    DBFView* m_view;
public:
    DBFWindow(DBFView* view) : DBFListCtrl(), m_datamodel(view->GetDocument()->GetDatabase()), m_view(view)
    {
    }

    bool Create(wxWindow* parent)
    {
       bool ok = base::Create(parent, wxID_ANY);
       if (ok)
           AssociateModel(&m_datamodel);
       return ok;
    }

    virtual ~DBFWindow()
    {
        //AssociateModel(NULL);
    }
};

wxIMPLEMENT_CLASS(DBFWindow, DBFListCtrl)

/////////////////////////////////////////////////////////////////////////////
// DBFView

wxIMPLEMENT_DYNAMIC_CLASS(DBFView, wxViewEx)

DBFView::DBFView() : wxViewEx()
{
}

DBFView::~DBFView()
{
}

wxBEGIN_EVENT_TABLE(DBFView, wxViewEx)
    EVT_MENU(XRCID("struct"), DBFView::OnStruct)
    EVT_UPDATE_UI(XRCID("struct"), DBFView::OnUpdateNeedEditable)
    EVT_MENU(XRCID("tool_struct"), DBFView::OnStructClipboard)
    EVT_MENU(wxID_PROPERTIES, DBFView::OnProperties)
    EVT_MENU(XRCID("export"), DBFView::OnFileExport)

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

    EVT_MENU(XRCID("goto")        , DBFView::OnGoto)
wxEND_EVENT_TABLE()

bool DBFView::OnCreate(wxDocument* doc, long flags)
{
    bool ok = base::OnCreate(doc, flags);

    if (ok)
    {
        wxFrame* frame = wxStaticCast(doc->GetDocumentTemplate(), DatabaseDocTemplate)->CreateViewFrame(this);
        DBFWindow* wnd = new DBFWindow(this);

        wxASSERT(frame == GetFrame());
        if (wnd->Create(frame))
            SetWindow(wnd);
        else
            delete wnd;
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
    if (wxDynamicCast(hint, wxFileLoadedHint))
    {
        //GetWindow()->InitColumns(150, true);
        DBFWindow* wnd = GetWindow();

        wxBusyCursor busy;
        wnd->Freeze();
        wnd->InitColumns();
        wnd->SelectRow(0);
        wnd->Thaw();

        /*
        unsigned int row;
        if (wnd->GetModel()->FindRowByColumnValue(wxT("GTOOLBAR"), 1, &row))
            _asm NOP
        */
    }
    else
        base::OnUpdate(sender, hint);
}

bool DBFView::OnClose(bool deleteWindow)
{
    bool ok = base::OnClose(deleteWindow);
    if (ok)
    {
        Activate(false);
        GetWindow()->AssociateModel(NULL);
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
        wxMessageBoxCaptionStr, wxOK | wxCENTRE, GetModalParent());
}

void DBFView::OnProperties(wxCommandEvent&)
{
    const DBFDocument* doc = GetDocument();
    wxArrayString as;

    wxDocViewHelpers::GetInfo(*doc, &as);

    DBFModel datamodel(doc->GetDatabase());
    datamodel.GetProperties(&as, true);

    wxString str = ::wxJoin(as, wxT('\n'), 0);

    ::wxModalTextDialog(GetModalParent(), str, doc->GetFilename().GetFullName());
}

void DBFView::OnFileExport(wxCommandEvent&)
{
    const DBFDocument* doc = GetDocument();
    wxString filter;
    enum fmt
    {
        fmt_html_table,
        fmt_xml,
        fmt_enumcount
    };
    wxFileName fileName = doc->GetFilename();
    wxString default_title = fileName.GetName();
    wxString dir = fileName.GetPath();

    wxString temp;
    temp.Printf(wxT("%s (%s)|%s"),
         _("HTML Files"),
         wxT("html"),
         wxT("*.html;*.htm")
         );
    filter+=temp;
    filter+=wxT('|');
    temp.Printf(wxT("%s (%s)|%s"),
         _("XML Files"),
         wxT("xml"),
         wxT("*.xml")
         );
    filter+=temp;
    wxFileDialog dlg(GetFrame(), _("Export"), dir, default_title, filter, wxFD_DEFAULT_STYLE_SAVE);
    if (dlg.ShowModal() != wxID_OK)
        return;
    fileName = dlg.GetPath();

    DBFModel datamodel(doc->GetDatabase());
    DBFModel* model = &datamodel;

    if (0 == fileName.GetExt().CmpNoCase("xml"))
    {
        wxFFile file;
        if (file.Open(fileName.GetFullPath(), wxT("wb")))
        {
            wxFFileOutputStream stream(file);
            DataModelPrintout::MakeXmlFile(model, &stream, std::string(doc->GetTablename().utf8_str()));
            file.Close();
        }
    }
    else if (   (0 == fileName.GetExt().CmpNoCase("html"))
             || (0 == fileName.GetExt().CmpNoCase("htm")))
    {
        wxFFile file;
        if (file.Open(fileName.GetFullPath(), wxT("wb")))
        {
            wxFFileOutputStream stream(file);
            DataModelPrintout::MakeHtmlTableFile(model, &stream, std::string(doc->GetFilename().GetFullPath().utf8_str()));
            file.Close();
        }
    }
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
    if (!GetDocument()->IsEditable())
        event.Enable(false);
}

void DBFView::OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent& event)
{
    GetWindow()->OnUpdateNeedSel_NotDeleted(event);
    if (!GetDocument()->IsEditable())
        event.Enable(false);
}

void DBFView::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
    GetWindow()->OnUpdateNeedSel(event);
    if (!GetDocument()->IsEditable())
        event.Enable(false);
}

void DBFView::OnDeleteAll(wxCommandEvent&)
{
    if (wxOK == wxMessageBox(_("Delete all?"), wxMessageBoxCaptionStr, wxOK | wxCANCEL | wxICON_QUESTION, GetModalParent()))
        GetWindow()->DeleteAll(true);
}

void DBFView::OnDelete(wxCommandEvent&)
{
    if (wxOK == wxMessageBox(_("Delete selection?"), wxMessageBoxCaptionStr, wxOK | wxCANCEL | wxICON_QUESTION, GetModalParent()))
        GetWindow()->DeleteSelection(true);
}

void DBFView::OnAdd(wxCommandEvent&)
{
    GetWindow()->AddNew();
}

void DBFView::OnEdit(wxCommandEvent&)
{
    GetWindow()->Edit();
}

void DBFView::OnGoto(wxCommandEvent&)
{
    DBFWindow* wnd = GetWindow();
    int rowcount = wnd->GetItemCount();
    long row = wnd->GetSelectedRow();
    wxString msg = wxString::Format(_("Row number : 1...%d"), rowcount);

    row = wxGetNumberFromUser(msg, wxEmptyString, _("Goto row"),
                                    row+1, 1, rowcount, GetModalParent());

    if (row > 0)
    {
        wnd->SelectNone();
        wnd->SelectRow(row-1);
    }
}

wxPrintout* DBFView::OnCreatePrintout()
{
    const DBFDocument* doc = GetDocument();
    DBFModel datamodel(doc->GetDatabase());
    DataModelPrintout* print = new DataModelPrintout(&datamodel, doc->GetTitle());
#if (wxVERSION_NUMBER >= 2900)
    const wxPageSetupDialogData& page = GetDocumentManager()->GetPageSetupDialogData();
    print->SetMargins(page);
#endif
    return print;
}
