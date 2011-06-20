// dbfdlgs.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include <wx/valgen.h>

#include "dbfdlgs.h"
#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"
#include "wxext.h"

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

class wxStructListView : public wxListView
{
   DECLARE_DYNAMIC_CLASS(wxStructListView)
public:
   DBF_FIELD_INFO* m_array;
   unsigned int m_array_count;
   enum
   {
      ENUM_col_name,
      ENUM_col_type,
      ENUM_col_length,
      //ENUM_col_decimals,
      ENUM_col_enumcount
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

IMPLEMENT_DYNAMIC_CLASS(wxStructListView, wxListView)

wxStructListView::wxStructListView() : wxListView(), m_array(NULL), m_array_count(0)
{
}

wxStructListView::~wxStructListView()
{
   free(m_array);
}

void wxStructListView::Init(wxDBase* db)
{
   static const wxChar* aszType[] =
   {
      wxT("Name"),
      wxT("Type"),
      wxT("Length"),
      //wxT("Decimals")
   };
   C_ASSERT_(1, ENUM_col_enumcount == WXSIZEOF(aszType));
   size_t i;

   for (i = 0; i < WXSIZEOF(aszType); i++)
   {
      InsertColumn((long)i, aszType[i], (i == ENUM_col_length) ? wxLIST_FORMAT_RIGHT : wxLIST_FORMAT_LEFT, 80);
   }

   m_array_count = (db && db->IsOpen()) ? db->GetFieldCount() : 0;
   m_array = (DBF_FIELD_INFO*)realloc(m_array, sizeof(*m_array) * m_array_count);

   for (i = 0; i < m_array_count; i++)
   {
      db->GetFieldInfo((dbf_uint)i, m_array + i);
   }
   Fill();
   ::wxListView_SetCurSel(this, 0);
   SetFocus();
}

void wxStructListView::Fill()
{
   SetItemCount(m_array_count);
   Refresh();
}

void wxStructListView::Add(const DBF_FIELD_INFO& info)
{
   m_array = (DBF_FIELD_INFO*)realloc(m_array, sizeof(*m_array) * (m_array_count + 1));
   m_array[m_array_count] = info;
   m_array_count++;
   Fill();
   ::wxListView_SetCurSel(this, m_array_count-1);
}

wxString wxStructListView::OnGetItemText(long item, long col) const
{
   wxString str;
   const DBF_FIELD_INFO* info = m_array + item;
   
   switch (col)
   {
      case ENUM_col_name:
         str = wxConvertMB2WX(info->name);
         break;
      case ENUM_col_type:
         str = MOD_aszType[info->type];
         break;
      case ENUM_col_length:
         if (info->decimals)
         {
            str.Printf(wxT("%d:%d"), info->length, info->decimals);
         }
         else
         {
            str.Printf(wxT("%d"), info->length);
         }
         break;
         /*
      case ENUM_col_decimals:
         str.Printf(wxT("%d"), info.decimals);
         break;
         */
   }
   return str;
}

/////////////////////////////////////////////////////////////////////////////
// wxDBFStructDialog

class wxDBFStructDialog : public wxDialog
{
protected:
   wxDBase* m_database;
public:
   wxStructListView* m_list;

public:
   wxDBFStructDialog(wxDBase*);

   bool Create(wxWindow* parent);

// Implementation
public:
   virtual ~wxDBFStructDialog();

protected:
   void OnAdd(wxCommandEvent&);
   void OnEdit(wxCommandEvent&);
   void OnDelete(wxCommandEvent&);
   void OnCalendar(wxCommandEvent&);
   void OnUpdateNeedSel(wxUpdateUIEvent&);
   void OnUpdateNeedData(wxUpdateUIEvent&);
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDBFStructDialog, wxDialog)
   EVT_BUTTON(wxID_ADD        , wxDBFStructDialog::OnAdd)
   EVT_BUTTON(wxID_DELETE     , wxDBFStructDialog::OnDelete)
   EVT_BUTTON(XRCID("edit")   , wxDBFStructDialog::OnEdit)
   EVT_BUTTON(XRCID("calendar"), wxDBFStructDialog::OnCalendar)
   EVT_UPDATE_UI(wxID_DELETE  , wxDBFStructDialog::OnUpdateNeedSel)
   EVT_UPDATE_UI(XRCID("edit"), wxDBFStructDialog::OnUpdateNeedSel)
   EVT_COMMAND_LEFT_DCLICK(XRCID("list"), wxDBFStructDialog::OnEdit)
   EVT_UPDATE_UI(wxID_OK, wxDBFStructDialog::OnUpdateNeedData)
END_EVENT_TABLE()

wxDBFStructDialog::wxDBFStructDialog(wxDBase* db) : wxDialog(), m_database(db)
{
}

bool wxDBFStructDialog::Create(wxWindow* parent)
{
   bool ok = wxXmlResource::Get()->LoadDialog(this, parent, wxT("struct"));
   if (ok)
   {
      m_list = XRCCTRL(*this, "list", wxStructListView);
      m_list->Init(m_database);
      //Center();
   }
   return ok;
}

wxDBFStructDialog::~wxDBFStructDialog()
{
}

void wxDBFStructDialog::OnUpdateNeedData(wxUpdateUIEvent& event)
{
   event.Enable(m_list->GetItemCount() != 0);
}

void wxDBFStructDialog::OnAdd(wxCommandEvent&)
{
   DBF_FIELD_INFO info = { "", DBF_DATA_TYPE_CHAR, 10, 0 };
   
   if (::DoModal_FieldEdit(this, &info, wxT("Add Field")))
   {
      m_list->Add(info);
   }
}

void wxDBFStructDialog::OnEdit(wxCommandEvent&)
{
   const int item = m_list->GetFirstSelected();
   DBF_FIELD_INFO info = m_list->m_array[item];

   if (::DoModal_FieldEdit(/*wxTheApp->GetTopWindow()*/this, &info, wxT("Edit Field")))
   {
      m_list->m_array[item] = info;
      m_list->Refresh();
   }
}

void wxDBFStructDialog::OnDelete(wxCommandEvent&)
{
}

void wxDBFStructDialog::OnCalendar(wxCommandEvent&)
{
   static const DBF_FIELD_INFO aField[] =
   {
      { "DATE"    , DBF_DATA_TYPE_DATE   , DBF_LEN_DATE, 0 },
      { "TEXT"    , DBF_DATA_TYPE_CHAR   , 80, 0 },
      { "ANNIVERS", DBF_DATA_TYPE_BOOLEAN, DBF_LEN_BOOLEAN, 0 }
   };
   m_list->m_array_count = WXSIZEOF(aField);
   size_t len = m_list->m_array_count * sizeof(*m_list->m_array);
   m_list->m_array = (DBF_FIELD_INFO*)realloc(m_list->m_array, len);
   memcpy(m_list->m_array, aField, len);
   m_list->Fill();
}

void wxDBFStructDialog::OnUpdateNeedSel(wxUpdateUIEvent& event)
{
   event.Enable(m_list->GetFirstSelected() != wxNOT_FOUND);
}

bool DoModal_Structure(wxWindow* parent, wxDBase* db, const wxString& caption, const wxString& filename)
{
   wxDBFStructDialog dlg(db);
   
   bool ok = dlg.Create(parent);
   if (ok)
   {
       if (caption.Length()) dlg.SetTitle(caption);
       ok = (wxID_OK == dlg.ShowModal());
       if (ok && filename.Length() && !db->IsOpen())
       {
          ok = db->Create(filename, dlg.m_list->m_array, dlg.m_list->m_array_count);
       }
   }
   return ok;
}

/////////////////////////////////////////////////////////////////////////////
// wxDBFFieldDialog

class wxDBFFieldDialog : public wxDialog
{
    typedef wxDialog base;
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
   wxDBFFieldDialog();

   bool Create(wxWindow* parent);

   virtual bool TransferDataFromWindow();

protected:
   void OnUpdateNeedData(wxUpdateUIEvent&);
   void OnUpdateLength(wxUpdateUIEvent&);
   void OnUpdateDecimals(wxUpdateUIEvent&);
   DECLARE_EVENT_TABLE()
};

BEGIN_EVENT_TABLE(wxDBFFieldDialog, wxDialog)
   EVT_UPDATE_UI(wxID_OK, wxDBFFieldDialog::OnUpdateNeedData)
   EVT_UPDATE_UI(XRCID("edit2"), wxDBFFieldDialog::OnUpdateLength)
   EVT_UPDATE_UI(XRCID("edit3"), wxDBFFieldDialog::OnUpdateDecimals)
END_EVENT_TABLE()

void wxDBFFieldDialog::OnUpdateLength(wxUpdateUIEvent& event)
{
   const bool boolean = (m_edit1->GetSelection() == DBF_DATA_TYPE_BOOLEAN);
   const bool memo    = (m_edit1->GetSelection() == DBF_DATA_TYPE_MEMO);
   event.Enable(!(boolean || memo));
   if (boolean) m_edit2->SetLabel(wxT("1"));
   else if (memo) m_edit2->SetLabel(wxT("10"));
}

void wxDBFFieldDialog::OnUpdateDecimals(wxUpdateUIEvent& event)
{
   const bool flt     = (m_edit1->GetSelection() == DBF_DATA_TYPE_FLOAT);
   event.Enable(flt);
   if (!flt) m_edit3->SetLabel(wxT("0"));
}

void wxDBFFieldDialog::OnUpdateNeedData(wxUpdateUIEvent& event)
{
   bool enable = !(   m_edit0->GetValue().IsEmpty()
                   || (m_edit1->GetSelection() == wxNOT_FOUND)
                   || m_edit2->GetValue().IsEmpty()
                   || m_edit3->GetValue().IsEmpty());
   event.Enable(enable);
}

wxDBFFieldDialog::wxDBFFieldDialog() : wxDialog()
{
}

bool wxDBFFieldDialog::Create(wxWindow* parent)
{
   bool ok = wxXmlResource::Get()->LoadDialog(this, parent, wxT("field_edit"));
   
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
       Center();
   }
   return ok;
}

bool wxDBFFieldDialog::TransferDataFromWindow()
{
   bool ok = base::TransferDataFromWindow();

   if (ok) m_name.MakeUpper();
   return ok;
}

bool DoModal_FieldEdit(wxWindow* parent, DBF_FIELD_INFO* info, const wxString& caption)
{
   wxDBFFieldDialog dlg;
   bool ok = dlg.Create(parent);
   
   if (ok)
   {
       dlg.m_name = wxConvertMB2WX(info->name);
       dlg.m_type = info->type;
       dlg.m_length = (int)info->length;
       dlg.m_decimals = info->decimals;
       if (caption.Length()) dlg.SetTitle(caption);
       ok = (wxID_OK == dlg.ShowModal());
       if (ok)
       {
          strncpy(info->name, dlg.m_name.mb_str(), sizeof(info->name));
          info->type = (enum dbf_data_type)dlg.m_type;
          info->length = dlg.m_length;
          info->decimals = dlg.m_decimals;
       }
   }
   return ok;
}

/////////////////////////////////////////////////////////////////////////////
