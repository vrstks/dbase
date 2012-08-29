// datalist.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
#define __DATALIST_H__

/////////////////////////////////////////////////////////////////////////////
// wxDataListCtrl

class wxDataModelBase;
class wxDataListCtrl : public wxAltColourListView
{
   typedef wxAltColourListView base;
//   DECLARE_CLASS(wxDataListCtrl)
// Attributes
public:
   enum attr
   {
      attr_deleted,
      attr_enumcount,
      attr_none = -1
   };
   wxListItemAttr m_attr[attr_enumcount];
   
   int m_column_clicked;
   
   int m_id_edit;
   int m_id_selchange;
   int m_id_delete;

   wxDataListCtrl();
   void Init();

   bool IsAnyUnselected(void);
   bool IsUndeletedInSelection(void);
   bool IsDeletedInSelection(void);
   void DeleteSelection(bool bDelete);
   void DeleteAll(bool bDelete);
   bool IsOpen() const;

protected:
   bool CanEditLabel(void);

// Implementation
public:
   virtual ~wxDataListCtrl();
   virtual wxString OnGetItemText(long item, long col) const;
   virtual wxListItemAttr* OnGetItemAttr(long item) const;
   virtual bool DeleteRecord(unsigned int index, bool bDelete);
   virtual bool IsRecordDeleted(unsigned int index);
   virtual bool IsRecordOk(unsigned int index);
   virtual void Fill();
   virtual wxDataModelBase* GetModel(void) = 0;
   //virtual wxString Format(long col, const wxVariant&) const;
public:
   void OnUpdateSelectAll  (wxUpdateUIEvent&);
   void OnUpdateNeedSel_Deleted(wxUpdateUIEvent&);
   void OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent&);
   void OnUpdateNeedSel    (wxUpdateUIEvent&);
   void OnBeginLabelEdit   (wxListEvent&);
   void OnEndLabelEdit     (wxListEvent&);
   void OnKeyDown(wxKeyEvent&);
   void OnSelChange     (wxListEvent&);
   void OnSelect(wxListEvent&);
   DECLARE_EVENT_TABLE()
};

/////////////////////////////////////////////////////////////////////////////

#endif // __DATALIST_H__
