// dbfdoc.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFDOC_H__
#define __DBFDOC_H__

/////////////////////////////////////////////////////////////////////////////
// DBFDocument

class wxDBase;
class DBFDocument : public wxDocument
{
   typedef wxDocument base;
   DECLARE_DYNAMIC_CLASS(DBFDocument)

// Attributes
protected:
   wxDBase* m_database;
   wxString m_tablename;
public:
   DBFDocument(void);

   enum
   {
      ENUM_hint_initialupdate = 123 // must be non-null
   };

   wxDBase* GetDatabase(void) const { return m_database; }
   wxString GetTablename(void) const { return m_tablename; }
   bool IsEditable(void) const;

// Implementation
public:
   virtual ~DBFDocument(void);
   virtual bool OnNewDocument();
   virtual bool SaveAs();
   virtual bool DoSaveDocument(const wxString& filename);
   virtual bool DoOpenDocument(const wxString& filename);
   virtual bool IsModified(void) const;
   virtual void Modify(bool mod);
   virtual bool OnCloseDocument();
};

/////////////////////////////////////////////////////////////////////////////
// DatabaseDocTemplate

class wxRecentFileList;
class DatabaseDocTemplate : public wxDocTemplate
{
    DECLARE_CLASS(DatabaseDocTemplate)
    DatabaseDocTemplate(wxDocManager*, wxClassInfo* docClassInfo,
                                       wxClassInfo* viewClassInfo,
                                       wxClassInfo* frameClassInfo,
                                       wxRecentFileList*);
protected:
    wxClassInfo* m_frameClassInfo;
    wxRecentFileList* m_mru;

    virtual wxFrame* CreateViewFrame(wxView*);
public:
    static DatabaseDocTemplate* Create(wxDocManager*, wxRecentFileList*);
    static wxIcon GetIcon();
    wxRecentFileList* GetRecentFileList() const { return m_mru; }

    friend class DBFView;
};

#endif // __DBFDOC_H__
