// dbfdoc.h
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
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

public:
    DBFDocument();

    wxDBase* GetDatabase() const { return m_database; }
    wxString GetTablename() const { return m_tablename; }
    wxFileName GetFilename() const { return wxFileName(base::GetFilename()); }
    bool IsEditable() const;

// Implementation
public:
    virtual ~DBFDocument();
    virtual bool OnNewDocument();
    virtual bool SaveAs();
    virtual bool IsModified() const;
    virtual void Modify(bool mod);
    virtual bool OnCloseDocument();

    virtual bool DeleteContents();

    bool AttachStream(wxInputStream&, const wxString& filePath);
protected:
    virtual bool DoSaveDocument(const wxString& filePath);
    virtual bool DoOpenDocument(const wxString& filePath);

// Attributes
protected:
    wxDBase* m_database;
    wxString m_tablename;
    wxInputStream* m_stream;
};

/////////////////////////////////////////////////////////////////////////////
// DatabaseDocTemplate

class wxRecentFileList;

#ifndef wxDocTemplateClassInfo_Defined
#define wxDocTemplateClassInfo_Defined
class wxDocTemplateClassInfo
{
public:
    wxDocTemplateClassInfo() {}
    wxDocTemplateClassInfo(wxClassInfo* doc, wxClassInfo* view, wxClassInfo* frame) : m_doc(doc), m_view(view), m_frame(frame) { }

    wxClassInfo* m_doc;
    wxClassInfo* m_view;
    wxClassInfo* m_frame;
};
#endif

class DatabaseDocTemplate : public wxDocTemplate
{
    DECLARE_CLASS(DatabaseDocTemplate)
    static DatabaseDocTemplate* ms_instance;
    DatabaseDocTemplate(wxDocManager*, const wxDocTemplateClassInfo&, wxRecentFileList*);
protected:
    const wxClassInfo* m_frameClassInfo;
    wxRecentFileList* m_mru;

    virtual wxFrame* CreateViewFrame(wxView*);
public:
    static const wxDocTemplateClassInfo m_DefaultClassInfo;
    static DatabaseDocTemplate* Create(wxDocManager*, wxRecentFileList*);
    static wxIcon GetIcon();
    wxRecentFileList* GetRecentFileList() const { return m_mru; }

    static DatabaseDocTemplate* GetInstance() { return ms_instance; }

    friend class DBFView;
};

#endif // __DBFDOC_H__
