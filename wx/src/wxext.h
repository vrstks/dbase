// wxext.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#define WXK_HELP       WXK_F1
#define WXK_FULLSCREEN WXK_F11

class WXDLLIMPEXP_FWD_CORE wxDataObject;
class WXDLLIMPEXP_FWD_CORE wxMDIParentFrame;

WX_DECLARE_OBJARRAY(wxFileName, wxArrayFileName);

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
extern bool wxWindow_Toggle(wxWindow*);
extern wxStdDialogButtonSizer* wxCreateStdDialogButtonSizer(wxWindow* parent, long flags);

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
WX_DECLARE_OBJARRAY(wxAcceleratorEntry, wxArrayAccelerator);
class wxAcceleratorHelper
{
public:
    static wxAcceleratorEntry GetStockAccelerator(wxWindowID);
    static void SetAccelText(wxMenuBar*, const wxArrayAccelerator&);
    static void SetAcceleratorTable(wxWindow*, const wxArrayAccelerator&);
    static bool SetAccelText(wxMenuItem*, const wxString& accel, bool append);
};
extern wxString wxToolBarTool_MakeShortHelp(const wxString&, const wxArrayAccelerator& accel, int id);
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
class WXDLLIMPEXP_FWD_CORE wxDocManager;
class WXDLLIMPEXP_FWD_CORE wxConfigBase;

class wxRecentFileList
{
protected:
   wxFileHistory* m_fileHistory;

public:
   wxRecentFileList(wxFileHistory*);

   void Load(wxConfigBase* config = NULL, const wxString& configPath = wxEmptyString);
   void Save(wxConfigBase* config = NULL, const wxString& configPath = wxEmptyString);

   void Attach(wxMenuBar*);
   void Attach(wxFrame*);
   bool Detach(wxMenuBar*);
   bool Detach(wxFrame*);

   wxFileHistory* GetImplementation() const;

   bool GetFile(size_t, wxFileName*) const;
};

class wxDocument;
extern void wxDocument_Info(const wxDocument*, wxArrayString*);
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
class wxAltColourListView : public wxListView
{
    typedef wxListView base;
    DECLARE_DYNAMIC_CLASS(wxAltColourListView)
public:
    wxColour GetAlternateRowColour() const             { return m_alternateRowColour.GetBackgroundColour(); }
    void SetAlternateRowColour(const wxColour& colour) { m_alternateRowColour.SetBackgroundColour(colour); }
    void SetAlternateRowColour();

    virtual wxListItemAttr* OnGetItemAttr(long row) const;

    void SelectAll(bool on = true)
    {
       for (int i = 0; i < GetItemCount(); i++)
       {
          SetItemState(i, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
       }
    }

    long GetSelectedRow(wxString* str = NULL) const
    { 
       long row = InReportView() ? GetFirstSelected() : GetFocusedItem();

       if (str && (wxNOT_FOUND != row)) str->operator=(GetItemText(row));
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

private:
    // user defined color to draw row lines, may be invalid
    wxListItemAttr m_alternateRowColour;
};
#endif
