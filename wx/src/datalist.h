// datalist.h
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
#define __DATALIST_H__

#if (wxVERSION_NUMBER >= 2904)
#define USE_DATALISTVIEW 0 // work in progress
#else
#define USE_DATALISTVIEW 0
#endif

#if USE_DATALISTVIEW
#include <wx/dataview.h>
#else
//#include <wx/listctrl.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// DataModelListCtrl

class wxDataModelBase;

#if USE_DATALISTVIEW
class DataModelListCtrl : public wxDataViewListCtrl
{
   typedef wxDataViewListCtrl base;
#else
class DataModelListCtrl : public wxTrunkListView
{
   typedef wxTrunkListView base;
#endif
   DECLARE_CLASS(DataModelListCtrl)
// Attributes
public:
   DataModelListCtrl();

   void InitColumns();

   bool IsAnyUnselected(void);
   bool IsUndeletedInSelection(void);
   bool IsDeletedInSelection(void);
   void DeleteSelection(bool bDelete);
   void DeleteAll(bool bDelete);
   bool IsOpen() const;
   int GetEditWindowID() const
   {
      return m_id_edit;
   }
   bool IsEditable() const;

   bool Create(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxLC_REPORT | wxLC_VIRTUAL,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxListCtrlNameStr);

protected:
    bool CanEditLabel(void);

// Implementation
public:
    virtual ~DataModelListCtrl();
    virtual wxString OnGetItemText(long item, long col) const;
#if USE_DATALISTVIEW
#else
    virtual wxListItemAttr* OnGetItemAttr(long item) const;
#endif
    virtual bool DeleteRecord(unsigned int index, bool bDelete);
    virtual bool IsRecordDeleted(unsigned int index);
    virtual bool IsRecordOk(unsigned int index);
    virtual void Fill();

    void AssociateModel(wxDataModelBase* model) { m_model = model; }
          wxDataModelBase* GetModel()       { return m_model; }
    const wxDataModelBase* GetModel() const { return m_model; }
    //virtual wxString Format(long col, const wxVariant&) const;
public:
    void OnUpdateSelectAll  (wxUpdateUIEvent&);
    void OnUpdateNeedSel_Deleted(wxUpdateUIEvent&);
    void OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent&);
    void OnUpdateNeedSel    (wxUpdateUIEvent&);
    void OnKeyDown(wxKeyEvent&);

#if USE_DATALISTVIEW
#else
   void OnBeginLabelEdit   (wxListEvent&);
   void OnEndLabelEdit     (wxListEvent&);
   void OnSelectionChanged(wxListEvent&);
   void OnItemActivated(wxListEvent&);
#endif
   DECLARE_EVENT_TABLE()

protected:
   int m_column_clicked;
private:
   wxDataModelBase* m_model;
   wxListItemAttr m_attr;
   
   int m_id_edit;
   int m_id_selchange;
   int m_id_delete;
};

/////////////////////////////////////////////////////////////////////////////

#endif // __DATALIST_H__
