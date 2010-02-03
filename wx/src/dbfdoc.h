// dbfdoc.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFDOC_H__
#define __DBFDOC_H__

/////////////////////////////////////////////////////////////////////////////
// wxDBFDoc

class wxDBase;
class wxDBFDoc : public wxDocument
{
   typedef wxDocument base;
   DECLARE_DYNAMIC_CLASS(wxDBFDoc)

// Attributes
protected:
   wxDBase* m_database;
   wxString m_tablename;
public:
   wxDBFDoc(void);

   enum
   {
      ENUM_hint_initialupdate = 123 // must be non-null
   };

   static wxDocument* CreateDocument(wxDocManager*, const wxFileName&, long flags);

   wxDBase* GetDatabase(void) const { return m_database; }
   wxString GetTablename(void) const { return m_tablename; }
   bool IsEditable(void) const;

// Implementation
public:
   virtual ~wxDBFDoc(void);
   virtual bool SaveAs();
   virtual bool DoSaveDocument(const wxString& filename);
   virtual bool DoOpenDocument(const wxString& filename);
   virtual bool IsModified(void) const;
   virtual void Modify(bool mod);
   virtual bool OnCloseDocument();
};

#endif // __DBFDOC_H__
