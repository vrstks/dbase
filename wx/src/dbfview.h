// dbfview.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFVIEW_H__
#define __DBFVIEW_H__

/////////////////////////////////////////////////////////////////////////////
// DBFView

class DBFDocument;
class DBFWindow;
class DBFView : public wxViewEx
{
   typedef wxViewEx base;
   DECLARE_DYNAMIC_CLASS(DBFView)
public:
   DBFView();

   DBFDocument* GetDocument() const;
   DBFWindow* GetWindow() const;
   wxWindow* GetModalParent() const { return GetFrame(); }

// Implementation
public:
   virtual ~DBFView();
   virtual bool OnClose(bool deleteWindow = true);
   virtual bool OnCreate(wxDocument*, long flags);
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

   friend class DBFWindow;
};

#endif // __DBFVIEW_H__
