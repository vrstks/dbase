// dbfview.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFVIEW_H__
#define __DBFVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// wxDBFView

class wxDBFDoc;
class wxDBFListCtrl;
class wxDBFView : public wxView
{
   typedef wxView base;
   DECLARE_DYNAMIC_CLASS(wxDBFView)

// Attributes
public:
   wxMDIChildFrame* m_frame;
   wxDBFListCtrl* m_list;
  
   wxDBFView();

   wxDBFDoc* GetDocument() const;

// Implementation
public:
   virtual ~wxDBFView();
   virtual bool OnClose(bool deleteWindow = true);
   virtual bool OnCreate(wxDocument*, long flags);
   virtual void OnDraw(wxDC*);
   virtual void OnUpdate(wxView* sender, wxObject* hint = NULL);
protected:
   void OnStructClipboard(wxCommandEvent&);
   void OnStruct(wxCommandEvent&);
   void OnProperties(wxCommandEvent&);
   
   void OnUpdateNeedSel (wxUpdateUIEvent&);
   void OnSelectAll     (wxCommandEvent&);
   void OnUpdateSelectAll(wxUpdateUIEvent&);
   void OnAdd           (wxCommandEvent&);
   void OnEdit          (wxCommandEvent&);
   void OnUpdateNeedEditable(wxUpdateUIEvent&);
   void OnUndelete      (wxCommandEvent&);
   void OnUpdateNeedSel_Deleted(wxUpdateUIEvent&);
   void OnDelete        (wxCommandEvent&);
   void OnUpdateNeedSel_NotDeleted(wxUpdateUIEvent&);
   void OnDeleteAll     (wxCommandEvent&);
   DECLARE_EVENT_TABLE()
};

#endif // __DBFVIEW_H__
