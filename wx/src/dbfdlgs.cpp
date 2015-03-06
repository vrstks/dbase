// dbfdlgs.cpp
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"

#include "dbfdlgs.h"
#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"

#define C_ASSERT_(n,e) typedef char __C_ASSERT__##n[(e)?1:-1]

static const wxChar* const MOD_aszType[] =
{
   wxT("Char"),
   wxT("Number"),
   wxT("Float"),
   wxT("Date"),
   wxT("Time"),
   wxT("DateTime"), /* non standard */
   wxT("Memo"),     /* non standard */
   wxT("Logical")
};
C_ASSERT_(1,WXSIZEOF(MOD_aszType) == DBF_DATA_TYPE_ENUMCOUNT);

class wxStructListView : public wxTrunkListView
{
    typedef wxTrunkListView base;
    DECLARE_DYNAMIC_CLASS(wxStructListView)
public:
    DBaseFieldVector m_array;

    enum col
    {
        col_name,
        col_type,
        col_length,
        //col_decimals,
        col_enumcount
    };

   wxStructListView();
   void Init(wxDBase*);
   void Fill();
   void Add(const DBF_FIELD_INFO&);

// Implementation
public:
   virtual ~wxStructListView();
   virtual wxString OnGetItemText(long item, long col) const;
};

IMPLEMENT_DYNAMIC_CLASS(wxStructListView, wxTrunkListView)

wxStructListView::wxStructListView() : wxTrunkListView(), m_array()
{
}

wxStructListView::~wxStructListView()
{
}

void wxStructListView::Init(wxDBase* db)
{
   const wxChar* aszType[] =
   {
      wxT("Name"),
      wxT("Type"),
      wxT("Length"),
      //wxT("Decimals")
   };
   C_ASSERT_(1, col_enumcount == WXSIZEOF(aszType));
   size_t i;

   for (i = 0; i < WXSIZEOF(aszType); i++)
      AppendColumn(aszType[i], (i == col_length) ? wxLIST_FORMAT_RIGHT : wxLIST_FORMAT_LEFT, 80);

   size_t count = (db && db->IsOpen()) ? db->GetFieldCount() : 0;

   for (i = 0; i < count; i++)
   {
       DBF_FIELD_INFO info;

       db->GetFieldInfo((dbf_uint)i, &info);
       m_array.push_back(info);
   }
   Fill();
   SelectRow(0);
   SetFocus();
}

void wxStructListView::Fill()
{
    SetItemCount((long)m_array.size());
    Refresh();
}

void wxStructListView::Add(const DBF_FIELD_INFO& info)
{
    m_array.push_back(info);
    Fill();
    SelectRow((long)(m_array.size() - 1));
}

wxString wxStructListView::OnGetItemText(long item, long col) const
{
   wxString str;
   const DBF_FIELD_INFO& info = m_array[item];
   
   switch (col)
   {
      case col_name:
         str = wxConvertMB2WX(info.name); break;
      case col_type:
         str = MOD_aszType[info.type]; break;
      case col_length:
         if (info.decimals)
            str.Printf(wxT("%d:%d"), info.length, info.decimals);
         else
            str.Printf(wxT("%d"), info.length);
         break;
         /*
      case col_decimals:
         str.Printf(wxT("%d"), info.decimals); break;
         */
   }
   return str;
}

/////////////////////////////////////////////////////////////////////////////
// DbfStructPanel

class DbfStructPanel : public wxPanel
{
    typedef wxPanel base;
protected:
    wxDBase* m_database;
    wxStructListView* m_list;
public:

public:
    DbfStructPanel(wxDBase*);

    bool Create(wxWindow* parent);

    const wxStructListView* GetList() const { return m_list; }

// Implementation
public:
    virtual ~DbfStructPanel();

protected:
    void OnAdd(wxCommandEvent&);
    void OnEdit(wxCommandEvent&);
    void OnDelete(wxCommandEvent&);
    void OnCalendar(wxCommandEvent&);
    void OnUpdateNeedSel(wxUpdateUIEvent&);
    void OnUpdateNeedData(wxUpdateUIEvent&);
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(DbfStructPanel, wxPanel)
   EVT_BUTTON(wxID_ADD        , DbfStructPanel::OnAdd)
   EVT_BUTTON(wxID_DELETE     , DbfStructPanel::OnDelete)
   EVT_BUTTON(XRCID("edit")   , DbfStructPanel::OnEdit)
   EVT_BUTTON(XRCID("calendar"), DbfStructPanel::OnCalendar)
   EVT_UPDATE_UI(wxID_DELETE  , DbfStructPanel::OnUpdateNeedSel)
   EVT_UPDATE_UI(XRCID("edit"), DbfStructPanel::OnUpdateNeedSel)
   EVT_COMMAND_LEFT_DCLICK(XRCID("list"), DbfStructPanel::OnEdit)
   EVT_UPDATE_UI(wxID_OK, DbfStructPanel::OnUpdateNeedData)
wxEND_EVENT_TABLE();

DbfStructPanel::DbfStructPanel(wxDBase* db) : wxPanel(), m_database(db), m_list(NULL)
{
}

bool DbfStructPanel::Create(wxWindow* parent)
{
    bool ok = wxXmlResource::Get()->LoadPanel(this, parent, wxT("struct"));

    if (ok)
    {
        m_list = XRCCTRL(*this, "list", wxStructListView);
        m_list->Init(m_database);
    }
    return ok;
}

DbfStructPanel::~DbfStructPanel()
{
}

void DbfStructPanel::OnUpdateNeedData(wxUpdateUIEvent& event)
{
   event.Enable(m_list->GetItemCount() != 0);
}

void DbfStructPanel::OnAdd(wxCommandEvent&)
{
   DBF_FIELD_INFO info = { "", DBF_DATA_TYPE_CHAR, 10, 0 };
   
   if (::DoModal_FieldEdit(this, &info, _("Add Field")))
      m_list->Add(info);
}

void DbfStructPanel::OnEdit(wxCommandEvent&)
{
   const int item = m_list->GetSelectedRow();
   DBF_FIELD_INFO info = m_list->m_array[item];

   if (::DoModal_FieldEdit(/*wxTheApp->GetTopWindow()*/this, &info, _("Edit Field")))
   {
       m_list->m_array.at(item) = info;
       m_list->Refresh();
   }
}

void DbfStructPanel::OnDelete(wxCommandEvent&)
{
    const int item = m_list->GetSelectedRow();

    m_list->m_array.erase(m_list->m_array.begin() + item);
    m_list->Fill();
}

void DbfStructPanel::OnCalendar(wxCommandEvent&)
{
   const DBF_FIELD_INFO fields[] =
   {
      { "DATE"    , DBF_DATA_TYPE_DATE   , DBF_LEN_DATE, 0 },
      { "TEXT"    , DBF_DATA_TYPE_CHAR   , 80, 0 },
      { "ANNIVERS", DBF_DATA_TYPE_BOOLEAN, DBF_LEN_BOOLEAN, 0 }
   };
   m_list->m_array.assign(fields, fields + WXSIZEOF(fields));
   m_list->Fill();
}

void DbfStructPanel::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   event.Enable(m_list->HasSelection());
}

bool DoModal_Structure(wxWindow* parent, wxDBase* db, const wxString& caption, const wxFileName& fileName)
{
    wxDialog dlg;
    bool ok = dlg.Create(parent, wxID_ANY, _("Structure"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    if (ok)
    {
        DbfStructPanel* panel = new DbfStructPanel(db);   
        ok = panel->Create(&dlg);
        if (ok)
        {
            wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
            topSizer->Add(panel, 1, wxEXPAND);
            topSizer->Add(dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Align(wxEXPAND).Border());
            dlg.SetSizerAndFit(topSizer);
            dlg.CenterOnParent();
            dlg.GetSizer()->SetSizeHints(&dlg);
            dlg.SetExtraStyle(dlg.GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);
            dlg.SetIcon(wxArtProvider::GetIcon(wxART_APP));
            if (!caption.empty())
               dlg.SetTitle(caption);
            ok = (wxID_OK == dlg.ShowModal());
            if (ok && fileName.IsOk() && !db->IsOpen())
               ok = db->Create(fileName, panel->GetList()->m_array);
        }
   }
   return ok;
}

/////////////////////////////////////////////////////////////////////////////
// DbfFieldPanel

class DbfFieldPanel : public wxPanel
{
    typedef wxPanel base;
public:
    wxString m_name;
    int m_type;
    int m_length;
    int m_decimals;
protected:
    wxTextCtrl* m_edit0;
    wxComboBox* m_edit1;
    wxTextCtrl* m_edit2;
    wxTextCtrl* m_edit3;

public:
    DbfFieldPanel();

    bool Create(wxWindow* parent);

    virtual bool TransferDataFromWindow();

protected:
    void OnUpdateNeedData(wxUpdateUIEvent&);
    void OnUpdateLength(wxUpdateUIEvent&);
    void OnUpdateDecimals(wxUpdateUIEvent&);
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(DbfFieldPanel, wxPanel)
   EVT_UPDATE_UI(wxID_OK, DbfFieldPanel::OnUpdateNeedData)
   EVT_UPDATE_UI(XRCID("edit2"), DbfFieldPanel::OnUpdateLength)
   EVT_UPDATE_UI(XRCID("edit3"), DbfFieldPanel::OnUpdateDecimals)
wxEND_EVENT_TABLE();

void DbfFieldPanel::OnUpdateLength(wxUpdateUIEvent& event)
{
   const bool boolean = (m_edit1->GetSelection() == DBF_DATA_TYPE_BOOLEAN);
   const bool memo    = (m_edit1->GetSelection() == DBF_DATA_TYPE_MEMO);

   event.Enable(!(boolean || memo));
   if (boolean)
       m_edit2->SetLabel(wxT("1"));
   else if (memo)
       m_edit2->SetLabel(wxT("10"));
}

void DbfFieldPanel::OnUpdateDecimals(wxUpdateUIEvent& event)
{
   const bool flt = (m_edit1->GetSelection() == DBF_DATA_TYPE_FLOAT);

   event.Enable(flt);
   if (!flt)
       m_edit3->SetLabel(wxT("0"));
}

void DbfFieldPanel::OnUpdateNeedData(wxUpdateUIEvent& event)
{
   bool enable = !(   m_edit0->GetValue().empty()
                   || (m_edit1->GetSelection() == wxNOT_FOUND)
                   || m_edit2->GetValue().empty()
                   || m_edit3->GetValue().empty());

   event.Enable(enable);
}

DbfFieldPanel::DbfFieldPanel() : wxPanel()
{
}

bool DbfFieldPanel::Create(wxWindow* parent)
{
    bool ok = wxXmlResource::Get()->LoadPanel(this, parent, wxT("field_edit"));
   
    if (ok)
    {
        m_edit0 = XRCCTRL(*this, "edit0", wxTextCtrl);
        m_edit1 = XRCCTRL(*this, "edit1", wxComboBox);
        m_edit2 = XRCCTRL(*this, "edit2", wxTextCtrl);
        m_edit3 = XRCCTRL(*this, "edit3", wxTextCtrl);

        for (size_t i = 0; i < WXSIZEOF(MOD_aszType); i++)
        {
             m_edit1->Append(MOD_aszType[i], (void*)i);
            //m_combo->GetClientData()
        }
        m_edit0->SetValidator(wxGenericValidator(&m_name    ));
        m_edit1->SetValidator(wxGenericValidator(&m_type));
        m_edit2->SetValidator(wxGenericValidator(&m_length  ));
        m_edit3->SetValidator(wxGenericValidator(&m_decimals));
    }
    return ok;
}

bool DbfFieldPanel::TransferDataFromWindow()
{
   bool ok = base::TransferDataFromWindow();

   if (ok)
   {
       m_name.Truncate(DBF_DBASE3_FIELDNAMELENGTH);
       m_name.MakeUpper();
   }
   return ok;
}

bool DoModal_FieldEdit(wxWindow* parent, DBF_FIELD_INFO* info, const wxString& caption)
{
    wxDialog dlg;
    bool ok = dlg.Create(parent, wxID_ANY, _("Field edit"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    if (ok)
    {
        DbfFieldPanel* panel = new DbfFieldPanel();   
        ok = panel->Create(&dlg);
        if (ok)
        {
            wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
            topSizer->Add(panel, 1, wxEXPAND);
            topSizer->Add(dlg.CreateStdDialogButtonSizer(wxOK | wxCANCEL), wxSizerFlags().Align(wxEXPAND).Border());
            dlg.SetSizerAndFit(topSizer);
            dlg.CenterOnParent();
            dlg.GetSizer()->SetSizeHints(&dlg);
            dlg.SetExtraStyle(dlg.GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);
            dlg.SetIcon(wxArtProvider::GetIcon(wxART_APP));
            panel->m_name = wxConvertMB2WX(info->name);
            panel->m_type = info->type;
            panel->m_length = (int)info->length;
            panel->m_decimals = info->decimals;
            if (!caption.empty())
                dlg.SetTitle(caption);
            ok = (wxID_OK == dlg.ShowModal());
            if (ok)
            {
                strncpy(info->name, panel->m_name.mb_str(), sizeof(info->name));
                info->type     = (dbf_data_type)panel->m_type;
                info->length   = panel->m_length;
                info->decimals = panel->m_decimals;
            }
        }
   }
   return ok;
}

class WindowsPanel : public wxPanel
{
    typedef wxPanel base;
public:
    WindowsPanel();

    bool Create(wxWindow* parent, const wxDocVector&);

    wxDocument* GetDocument(int item)
    {
        return wxStaticCast(m_listBox->GetClientData(item), wxDocument);
    }
    int GetSelection() const
    {
        return m_selection;
    }
protected:
    void OnClose(wxCommandEvent&);
    void OnActivate(wxCommandEvent&);
    void OnUpdateNeedSel(wxUpdateUIEvent&);
    void OnDblClick(wxCommandEvent&);
    wxDECLARE_EVENT_TABLE();

protected:
    wxListBox* m_listBox;
    int m_selection;
};

wxBEGIN_EVENT_TABLE(WindowsPanel, wxPanel)
    EVT_BUTTON(wxID_CLOSE, WindowsPanel::OnClose)
    EVT_BUTTON(XRCID("activate"), WindowsPanel::OnActivate)
    EVT_UPDATE_UI(wxID_CLOSE, WindowsPanel::OnUpdateNeedSel)
    EVT_UPDATE_UI(XRCID("activate"), WindowsPanel::OnUpdateNeedSel)
    EVT_LISTBOX_DCLICK(wxID_ANY, WindowsPanel::OnDblClick)
wxEND_EVENT_TABLE()

WindowsPanel::WindowsPanel() : wxPanel(), m_listBox(NULL), m_selection(wxNOT_FOUND)
{
}

bool WindowsPanel::Create(wxWindow* parent, const wxDocVector& docList)
{
    bool ok = wxXmlResource::Get()->LoadPanel(this, parent, wxT("windows"));

    if (ok)
    {
        m_listBox = XRCCTRL(*this, "list", wxListBox);

        for (wxDocVector::const_iterator it = docList.begin(); it != docList.end(); it++)
        {
            wxDocument* doc = *it;
            wxDocManager* docManager = doc->GetDocumentManager();
            wxString text = doc->GetFilename();
            
            if (text.empty())
                text = doc->GetTitle();
            int index = m_listBox->Append(text, doc);

            if (doc == docManager->GetCurrentDocument())
                m_listBox->SetSelection(index);
        }
    }
    return ok;
}

void WindowsPanel::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
    wxArrayInt selections;

    event.Enable(0 != m_listBox->GetSelections(selections));
}

void WindowsPanel::OnClose(wxCommandEvent&)
{
    wxArrayInt selections;

    m_listBox->GetSelections(selections);
    for (wxArrayInt::const_reverse_iterator it = selections.rbegin(); it != selections.rend(); it++)
    {
        int index = *it;
        wxDocument* doc = GetDocument(index);

        if (doc->GetDocumentManager()->CloseDocument(doc))
            m_listBox->Delete(index);
    }
}

void WindowsPanel::OnActivate(wxCommandEvent&)
{
    wxArrayInt selections;

    m_listBox->GetSelections(selections);
    m_selection = selections[0];
    wxStaticCast(GetParent(), wxDialog)->EndModal(wxID_OK);
}

void WindowsPanel::OnDblClick(wxCommandEvent& event)
{
    switch (event.GetSelection())
    {
        case wxNOT_FOUND:
            event.Skip(); break;
        default:
            m_selection = event.GetSelection();
            wxStaticCast(GetParent(), wxDialog)->EndModal(wxID_OK);
            break;
    }
}

void DoModal_Windows(wxWindow* parent, const wxDocVector& docList)
{
    wxDialog dlg;
    bool ok = dlg.Create(parent, wxID_ANY, _("Field edit"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);
    if (ok)
    {
        WindowsPanel* panel = new WindowsPanel();   
        ok = panel->Create(&dlg, docList);
        if (ok)
        {
            wxStdDialogButtonSizer* buttonpane = dlg.CreateStdDialogButtonSizer(wxOK);
            buttonpane->GetAffirmativeButton()->SetId(wxID_CANCEL);
            buttonpane->SetAffirmativeButton(NULL);

            wxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
            topSizer->Add(panel, 1, wxEXPAND | wxRIGHT, 5);
            topSizer->Add(buttonpane, wxSizerFlags().Align(wxEXPAND).Border());
            dlg.SetSizerAndFit(topSizer);
            dlg.CenterOnParent();
            dlg.GetSizer()->SetSizeHints(&dlg);
            dlg.SetExtraStyle(dlg.GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY);
            dlg.SetIcon(wxArtProvider::GetIcon(wxART_APP));
            if (wxID_OK == dlg.ShowModal())
                wxTrunkDocView::ActivateDocument(panel->GetDocument(panel->GetSelection()));
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
