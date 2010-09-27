// wxext.h
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#define WXK_HELP       WXK_F1
#define WXK_FULLSCREEN WXK_F11

class wxListCtrl;
class wxListView;

WX_DECLARE_OBJARRAY(wxAcceleratorEntry, AcceleratorArray);

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

#ifdef _WX_FRAME_H_BASE_
inline bool wxWindow_Toggle(wxFrame* parent, wxWindow* wnd)
{
   const bool show = !wnd->IsShown();
   wnd->Show(show);
   parent->SendSizeEvent();
   return show;
}
#endif

#ifdef _WX_ARTPROV_H_
class wxArtProviderEx : public wxArtProvider
{
public:
   static wxBitmap GetBitmap(const wxArtID&, const wxArtClient& client = wxART_OTHER, const wxSize& size = wxDefaultSize);
protected:
   virtual wxBitmap CreateBitmap(const wxArtID&, const wxArtClient&, const wxSize&);
};
#endif

#if !defined(__WXCODE_H__) && defined(_WX_LISTCTRL_H_)

inline void wxListCtrl_SelectAll(wxListCtrl* ctrl, bool on = true)
{
   for (int i = 0; i < ctrl->GetItemCount(); i++)
   {
      ctrl->SetItemState(i, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
   }
}

inline bool wxListView_SetCurSel(wxListCtrl* ctrl, long index, bool focus = true)
{ 
   bool ok = (index != wxNOT_FOUND) && (index < ctrl->GetItemCount());
   if (ok)
   {
      bool on = true;
      ctrl->SetItemState(index, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED); //ctrl->Select(index);
      if (focus)
      {
         ctrl->SetItemState(index, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
         ctrl->EnsureVisible(index);
         //ctrl->Focus(index);
      }
   }
   return ok;
}

#endif

extern bool wxListCtrl_GetItemRect(const wxListView&, long row, long col, wxRect*);
extern long wxListView_HitTest(const wxListView&, const wxPoint&, int* flags, long* col);
extern wxString wxListView_GetItemText(const wxListCtrl&, int row, int col);
extern bool wxListCtrl_EndEditLabel(wxListCtrl* ctrl, bool cancel);

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

extern bool wxInitXRC();

extern wxAcceleratorEntry wxGetStockAcceleratorEx(wxWindowID);

class WXDLLIMPEXP_FWD_CORE wxMenuBar;
extern void wxMenu_SetAccelText(wxMenuBar*, const AcceleratorArray&);
extern void wxSetAcceleratorTable(wxWindow*, const AcceleratorArray&);
extern int wxAcceleratorEntry_Find(const AcceleratorArray&, int id);
extern wxString wxGetAccelText(const wxAcceleratorEntry&);

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

class wxFileHistory;
class wxDocManager;
class wxConfigBase;

class wxRecentFileList
{
protected:
   wxFileHistory* m_fileHistory;

public:
   wxRecentFileList(wxFileHistory*);

   void Load(wxConfigBase* config = NULL);
   void Save(wxConfigBase* config = NULL);

   void Attach(wxMenuBar*);
   void Attach(wxFrame*);
   void Detach(wxMenuBar*);
   void Detach(wxFrame*);

   wxFileHistory* GetImplementation() const;

   bool GetFile(size_t, wxFileName*) const;
protected:
   wxMenuItem* GetSubMenu(wxMenuBar*, wxMenu**) const;
};

class wxDocument;
extern void wxDocument_Info(const wxDocument*, wxArrayString*);

inline void wxJoin(wxArrayString* dst, const wxArrayString& src)
{
   wxString str;
   for (size_t i = 0; i < src.GetCount(); i++)
   {
      dst->Add(src.Item(i));
   }
}
