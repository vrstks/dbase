// wx/ext/wx.h
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#define WXK_HELP       WXK_F1
#define WXK_FULLSCREEN WXK_F11

#define wxART_APP          wxART_MAKE_ART_ID(wxART_APP)

#define wxFD_DEFAULT_STYLE_OPEN (wxFD_OPEN | wxFD_FILE_MUST_EXIST)
#define wxFD_DEFAULT_STYLE_SAVE (wxFD_SAVE | wxFD_OVERWRITE_PROMPT)

class WXDLLIMPEXP_FWD_CORE wxDataObject;
class WXDLLIMPEXP_FWD_CORE wxMDIParentFrame;

class wxArrayFileName : public std::vector<wxFileName> {};

#ifdef _WX_ARTPROV_H_
#define wxART_PREVIEW      wxART_MAKE_ART_ID(wxART_PREVIEW)
#define wxART_PREFERENCES  wxART_MAKE_ART_ID(wxART_PREFERENCES)
#define wxART_SETUP        wxART_MAKE_ART_ID(wxART_SETUP)
#define wxART_ZOOM_IN      wxART_MAKE_ART_ID(wxART_ZOOM_IN)
#define wxART_ZOOM_OUT     wxART_MAKE_ART_ID(wxART_ZOOM_OUT)
#define wxART_ZOOM_100     wxART_MAKE_ART_ID(wxART_ZOOM_100)
#define wxART_ADD          wxART_MAKE_ART_ID(wxART_ADD)
extern wxArtID wxID2ArtID(int wx_id);
#endif

extern void wxFrame_ToggleFullScreen(wxFrame*, long style);
extern void wxFrame_OnUpdateFullScreen(wxFrame*, wxUpdateUIEvent&);
extern void wxFrame_ClonePosition(wxFrame*, wxWindow* otherwindow = NULL);
extern bool wxWindow_Toggle(wxWindow*);
extern wxStdDialogButtonSizer* wxCreateStdDialogButtonSizer(wxWindow* parent, long flags);
extern void wxModalTextDialog(wxWindow* parent, const wxString& text, const wxString& caption = wxEmptyString);
extern bool wxRemoveFile(wxFFile*);

class wxClipboardHelper
{
public:
    enum Type
    {
        Default = 1, // use the normal clipboard
        Primary = 2, // use the primary clipboard
        Both    = 3  // use both clipboards (only valid for set functions)
    };
    // Set the text to the specified clipboard(s).
    static bool SetText(const wxString& str, Type clip_type = Default);

    static bool Set(wxDataObject* def, wxDataObject* primary = NULL);
};

#if defined(_WX_EVENT_H_) || defined(_WX_EVENT_H__)
inline void wxPostCommandEvent(wxEvtHandler* dest, wxEventType commandType, int id)
{
   wxCommandEvent event(commandType, id);
   if (dest == NULL) dest = wxTheApp->GetTopWindow();
   wxPostEvent(dest, event);
}

inline void wxPostMenuCommand(wxEvtHandler* dest, int id)
{
   wxPostCommandEvent(dest, wxEVT_COMMAND_MENU_SELECTED, id);
}
#endif

class WXDLLIMPEXP_FWD_BASE wxFFile;
class wxXmlResourceHelper
{
public:
    static const wxString FileExt;
    static const wxString FileExtCompressed;
    static const wxString DefaultFolder;

    static void Init();

    static bool LoadFromFile(const wxFileName&);
    static bool LoadFromFile(const char* srcmodule, const wxString& filename);
    static bool LoadFromMemory(const void* buf, size_t buf_len, const wxString& filename, wxFFile* tempfile = NULL);
};

#if wxUSE_ACCEL
class WXDLLIMPEXP_FWD_CORE wxMenuBar;

class wxAcceleratorVector : public std::vector<wxAcceleratorEntry> // inline class, not exported
{
public:
    wxAcceleratorTable ToArray() const
    {
        return wxAcceleratorTable(size(), &at(0));
    }
};

class wxAcceleratorHelper
{
public:
    static wxAcceleratorEntry GetStockAccelerator(wxWindowID);
    static void SetAccelText(wxMenuBar*, const wxAcceleratorVector&);
    static bool SetAccelText(wxMenuItem*, const wxString& accel, bool append);
};
extern wxString wxToolBarTool_MakeShortHelp(const wxString&, const wxAcceleratorVector& accel, int id);
#endif

#define wxMessageBoxCaption      wxGetApp().GetAppDisplayName()

#ifdef _WX_STOCKITEM_H_
#if (wxVERSION_NUMBER < 2901)
#define wxSTOCK_WITHOUT_ELLIPSIS 4
#define wxSTOCK_FOR_BUTTON (wxSTOCK_WITH_MNEMONIC | wxSTOCK_WITHOUT_ELLIPSIS)
#endif
#define wxSTOCK_PLAINTEXT wxSTOCK_WITHOUT_ELLIPSIS
extern wxString wxGetStockLabelEx(wxWindowID, long flags = wxSTOCK_WITH_MNEMONIC);
#endif

/////////////////////////////////////////////////////////////////////////////
// wxRecentFileList
// wxFileHistory is confusing and difficult to use, esp in MDI:
// wrap up the eccentricities

class WXDLLIMPEXP_FWD_CORE wxFileHistory;
class WXDLLIMPEXP_FWD_BASE wxConfigBase;

class wxRecentFileList
{
public:
    wxRecentFileList(size_t maxFiles = 9, wxWindowID idBase = wxID_FILE1);
    wxRecentFileList(wxFileHistory*);
    virtual ~wxRecentFileList();

    void Load(wxConfigBase* config = NULL, const wxString& configPath = wxEmptyString);
    void Save(wxConfigBase* config = NULL, const wxString& configPath = wxEmptyString);

    void Attach(wxMenuBar*);
    void Attach(wxFrame*);
    bool Detach(wxMenuBar*);
    bool Detach(wxFrame*);

    wxFileHistory* GetImplementation() const
    {
        return m_fileHistory;
    }

    void AddFileToHistory(const wxString& file);
    void AddFileToHistory(const wxFileName& fileName)
    {
        AddFileToHistory(fileName.GetFullPath());
    }
    bool GetFile(size_t, wxString*) const;
    bool GetFile(size_t index, wxFileName* fileName) const
    {
        wxString str;
        if (!GetFile(index, &str))
            return false;
        if (fileName)
            *fileName = str;
        return true;
    }

    int IndexFromID(wxWindowID) const;
    int IsInRange(wxWindowID id) const
    {
        return wxNOT_FOUND != IndexFromID(id);
    }
private:
    wxFileHistory* m_fileHistory;
    wxFileHistory* m_DeleteMe;
};

class WXDLLIMPEXP_FWD_CORE wxDocument;

class wxDocViewHelpers
{
public:
    static wxView* GetCurrentView(const wxDocument&);
    static void GetInfo(const wxDocument&, wxArrayString*);
};

extern void wxFrame_SetInitialPosition(wxFrame*,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, int margin_pct = 5);

extern void wxJoin(wxArrayString* dst, const wxArrayString& src);

#ifdef _WX_DOCH__
class wxViewEx : public wxView
{
    typedef wxView base;
    DECLARE_CLASS(wxViewEx)
public:
    wxViewEx() : wxView(), m_viewWindow(NULL)
    {
    }

    void SetWindow(wxWindow* window) { m_viewWindow = window; } // trac.wxwidgets.org/ticket/13040
    wxWindow* GetWindow() const { return m_viewWindow; }

    virtual void OnDraw(wxDC*)
    {
    }
private:
    wxWindow* m_viewWindow;
};
#endif

class wxFileLoadedHint : public wxObject
{
    DECLARE_CLASS(wxFileLoadedHint)
};

/////////////////////////////////////////////////////////////////////////////
// MDIWindowMenuEvtHandler

class MDIWindowMenuEvtHandler : public wxEvtHandler
{
protected:
    wxMDIParentFrame* m_target_wnd;
public:
    MDIWindowMenuEvtHandler(wxMDIParentFrame*);

    virtual ~MDIWindowMenuEvtHandler();
protected:
    void OnClose(wxCommandEvent&);
    void OnCloseAll(wxCommandEvent&);
    void OnUpdateNeedWindow(wxUpdateUIEvent&);
    DECLARE_EVENT_TABLE()
};

#ifdef _WX_LISTCTRL_H_BASE_
class wxTrunkListView : public wxListView
{
    typedef wxListView base;
    DECLARE_DYNAMIC_CLASS(wxTrunkListView)
public:
#if (wxVERSION_NUMBER < 2905)
    void SetAlternateRowColour(const wxColour& colour) { m_alternateRowColour.SetBackgroundColour(colour); }
    void EnableAlternateRowColours(bool enable = true);
    virtual wxListItemAttr* OnGetItemAttr(long row) const;
    long AppendColumn(const wxString& heading,
                      wxListColumnFormat format = wxLIST_FORMAT_LEFT,
                      int width = -1)
    {
        return InsertColumn(GetColumnCount(), heading, format, width);
    }
#endif

    void SelectAll()
    {
        for (int i = 0; i < GetItemCount(); i++)
            SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    }

    void SelectNone()
    {
        for (int i = 0; i < GetItemCount(); i++)
            SetItemState(i, 0, wxLIST_STATE_SELECTED);
    }

    long GetSelectedRow(wxString* str = NULL) const
    {
        long row = GetFirstSelected();

        if (str && (wxNOT_FOUND != row))
            *str = GetItemText(row);
        return row;
    }

    bool SelectRow(long row, bool focus = true)
    {
        bool ok = (row != wxNOT_FOUND) && (row < GetItemCount());
        if (ok)
        {
            bool on = true;

            SetItemState(row, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
            if (focus)
            {
                SetItemState(row, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
                EnsureVisible(row);
            }
        }
        return ok;
    }

    bool HasSelection() const
    {
        return (GetFirstSelected() != wxNOT_FOUND);
    }

    int GetSelections( wxArrayInt& sel ) const
    {
        for (long row = GetFirstSelected(); row != wxNOT_FOUND; row = GetNextSelected(row))
            sel.push_back(row);
        return (int)sel.size();
    }

#if (wxVERSION_NUMBER < 2900)
    bool GetSubItemRect( long row, long col, wxRect& rect, int code = wxLIST_RECT_BOUNDS ) const;
#endif
    long HitTest(const wxPoint&, int* flags, long* col) const;
    bool EndEditLabel(bool cancel);

    bool RefreshSubItem(long row, long col, bool eraseBackground = true)
    {
        wxRect rect;
        bool ok = GetSubItemRect(row, col, rect);

        if (ok)
        {
            rect.x+=1, rect.width-=2;
            RefreshRect(rect, eraseBackground);
        }
        return ok;
    }

#if (wxVERSION_NUMBER < 2900)
    wxString GetItemText(long row, long col = 0) const
    {
       wxString str;
       wxListItem info;

       info.m_itemId = row;
       info.m_col = col;
       info.m_mask = wxLIST_MASK_TEXT;
       if (GetItem(info))
          str = info.m_text;
       return str;
    }
#endif

private:
#if (wxVERSION_NUMBER < 2905)
    // user defined color to draw row lines, may be invalid
    wxListItemAttr m_alternateRowColour;
#endif
};
#endif

#ifdef _WX_INTL_H_
class wxLocaleEx : public wxLocale
{
public:
    bool Init(wxLanguage lang = wxLANGUAGE_DEFAULT, int flags = wxLOCALE_LOAD_DEFAULT)
    {
    #if (wxVERSION_NUMBER >= 2900) // trac.wxwidgets.org/ticket/15257
        if (lang == wxLANGUAGE_DEFAULT)
            lang = (wxLanguage)GetSystemLanguage();
    #endif
        return wxLocale::Init(lang, flags);
    }
};
#endif

class TextWriter
{
public:
    TextWriter() : m_indent(0), m_linePos(0), m_encoding(), m_tabString()
    {
    }
    virtual ~TextWriter()
    {
    }

    virtual bool IsOpen() const = 0;
    virtual void Close() = 0;

    void SetEncoding(const std::string& encoding) { m_encoding = encoding; }
    void SetTabString(const std::string& tabstring) { m_tabString = tabstring; }
    std::string GetEncoding() const { return m_encoding; }
    std::string GetTabString() const { return m_tabString; }
    int GetIndent() const { return m_indent; }
    virtual std::string GetNewLine() const
    {
        return "\n";
    }
    void Write(const std::string& str)
    {
        if (m_linePos == 0)
            for (int i = 0; i < m_indent; i++)
                DoWrite(m_tabString);
        m_linePos += DoWrite(str);
    }
    void Write(char ch)
    {
        Write(std::string(1, ch));
    }
    void WriteLine(const std::string& str = std::string())
    {
        Write(str + GetNewLine());
        m_linePos = 0;
    }
    void WriteLine(char ch)
    {
        WriteLine(std::string(1, ch));
    }
    void IncreaseIndent()
    {
        m_indent++;
    }
    void DecreaseIndent()
    {
        wxASSERT(m_indent != 0);
        m_indent--;
    }
protected:
    virtual size_t DoWrite(const std::string& str) = 0;

    int m_indent;
    size_t m_linePos;
    std::string m_encoding;
    std::string m_tabString;
};

typedef std::vector<std::string> std_string_vector;

class XmlWriter : public TextWriter
{
public:
    static const char TagLeftChar = '<';
    static const char TagRightChar = '>';
    static const std::string EndTagLeftChars;
    static const std::string DefaultEncoding; // utf8
    static const std::string DefaultTabString; // 3 spaces

    XmlWriter() : m_open(true), m_ElementOpened(false)
    {
        m_tabString = DefaultTabString, m_encoding = DefaultEncoding;
    }
    virtual ~XmlWriter()
    {
        if (IsOpen())
            Close();
    }
    bool Open(const std::string& tableName)
    {
        m_TableName = tableName;
        WriteLine("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
        WriteStartElement(tableName);

        return true;
    }
    virtual void Close()
    {
        WriteEndElement();
        m_open = false;
        m_linePos = 0;
        m_indent = 0;
    }

    void WriteStartElement(const std::string& elmName)
    {
        if (m_indent && m_ElementOpened)
            WriteLine(TagRightChar);
        Write(TagLeftChar);
        Write(elmName);
        ElementList.push_back(elmName);
        m_indent++;
        m_ElementOpened = true;
    }
    void WriteElement(const std::string& elmName, const std::string& text)
    {
        if (m_ElementOpened)
        {
            WriteLine(TagRightChar);
            m_ElementOpened = false;
        }
        WriteStartElement(elmName);
        Write(TagRightChar);
        m_ElementOpened = false;
        Write(text);
        WriteEndElement();
    }
    void WriteEndElement()
    {
        if (m_ElementOpened)
        {
            WriteLine(TagRightChar);
            m_ElementOpened = false;
        }
        m_indent--;
        std::string elmName = ElementList.back();
        ElementList.pop_back();
        Write(EndTagLeftChars);
        WriteLine(elmName + TagRightChar);
    }
    void WriteAttribute(const std::string& attrName, const std::string& text)
    {
        Write(' ');
        Write(attrName + "=\"");
        Write(text + "\"");
    }
    void WriteAttribute(const std::string& attrName, const bool& value)
    {
        WriteAttribute(attrName, std::string(value ? "true" : "false"));
    }
    virtual bool IsOpen() const { return m_open; }
protected:
    bool m_open;
    std::string m_TableName;
    std_string_vector ElementList;
    bool m_ElementOpened;
};

class wxXmlWriter : public XmlWriter
{
    typedef XmlWriter base;
public:
    wxXmlWriter() : XmlWriter(), m_stream(NULL) {}

    bool Open(const std::string& tableName, wxOutputStream* stream)
    {
        m_stream = stream;
        return base::Open(tableName);
    }
    virtual void Close()
    {
        base::Close();
        m_stream = NULL;
    }
    virtual std::string GetNewLine() const
    {
    #ifdef __WINDOWS__
        return "\r\n";
    #elif defined(__WXOSX__)
        return "\r";
    #else
        return base::GetNewLine();
    #endif
    }
protected:
    virtual size_t DoWrite(const std::string& str)
    {
        return m_stream->Write(str.c_str(), str.length()).LastWrite();
    }
private:
    wxOutputStream* m_stream;
};

class HtmlTextWriter : public TextWriter
{
public:
    static const char TagLeftChar = '<';
    static const char TagRightChar = '>';
    static const std::string EndTagLeftChars;
    static const std::string DefaultEncoding; // utf8
    static const std::string DefaultTabString; // 3 spaces

    HtmlTextWriter();

    virtual ~HtmlTextWriter()
    {
        if (IsOpen())
            Close();
    }
    virtual bool IsOpen() const { return m_open; }

    void WriteBeginTag(const std::string& tagName)
    {
        Write(TagLeftChar);
        Write(tagName);
    }
    void WriteFullBeginTag(const std::string& tagName)
    {
        WriteBeginTag(tagName + TagRightChar);
    }
    void WriteEndTag(const std::string& tagName)
    {
        Write(EndTagLeftChars + tagName + TagRightChar);
    }
    void WriteAttribute(const std::string& name, const std::string& value)
    {
        Write(' ');
        Write(name);
        Write("=\"");
        Write(value);
        Write('\"');
    }
    void WriteBreak()
    {
        Write("<br/>");
    }
    virtual void Close()
    {
        m_linePos = 0;
        m_indent = 0;
        m_open = false;
    }
    static std::string Escape(const std::string&);
protected:
    bool m_open;
};

class wxHtmlTextWriter : public HtmlTextWriter
{
    typedef HtmlTextWriter base;
public:
    wxHtmlTextWriter(wxOutputStream* stream) : HtmlTextWriter(), m_stream(stream) {}
    virtual void Close()
    {
        base::Close();
        m_stream = NULL;
    }
    virtual std::string GetNewLine() const
    {
    #ifdef __WINDOWS__
        return "\r\n";
    #elif defined(__WXOSX__)
        return "\r";
    #else
        return base::GetNewLine();
    #endif
    }
protected:
    virtual size_t DoWrite(const std::string& str)
    {
        return m_stream->Write(str.c_str(), str.length()).LastWrite();
    }
private:
    wxOutputStream* m_stream;
};

class HtmlItem
{
public:
    virtual ~HtmlItem() {}
    virtual void Render(HtmlTextWriter*) const = 0;
};

enum HtmlTextWeight
{
    HtmlTextWeight_Normal,
    HtmlTextWeight_Bold
};

class HtmlTextParagraph : public HtmlItem
{
public:
    std::string Text;
    HtmlTextWeight Weight;

    HtmlTextParagraph(const std::string& str, HtmlTextWeight weight = HtmlTextWeight_Normal)
    {
        Text = str, Weight = weight;
    }
    virtual void Render(HtmlTextWriter*) const;

    static std::string ToBold(const std::string& str)
    {
        return std::string("<b>") + str + "</b>";
    }
};

class HtmlTableRow
{
public:
    std_string_vector ColumnTextArray;
    std_string_vector ColumnAttribute;
};
typedef std::vector<HtmlTableRow> HtmlTableVector;

class HtmlTable : public HtmlItem
{
public:
    HtmlTableVector List;
    std::vector<int> PercentArray;
    int Border;
    HtmlTable() : HtmlItem(), Border(1) {}
    virtual void Render(HtmlTextWriter*) const;
    virtual ~HtmlTable() {}
};

typedef std::vector<HtmlItem*> HtmlItemVector;

class wxHtmlTableWriter : public wxObject
{
public:
    std::string Title;
    HtmlItemVector List;
    void SaveFile(wxOutputStream*, std::string encoding = HtmlTextWriter::DefaultEncoding);
    virtual ~wxHtmlTableWriter();
};
