// wxext.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

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

inline bool wxWindow_Toggle(wxWindow* wnd, bool layout)
{
   const bool show = !wnd->IsShown();
   wnd->Show(show);
   if (layout)
   {
      wxWindow* parent = wnd->GetParent();
      parent->Layout();
      parent->Refresh();
   }
   return show;
}

#ifdef _WX_ARTPROV_H_
class wxArtProviderEx : public wxArtProvider
{
public:
   static wxBitmap GetBitmap(const wxArtID&, const wxArtClient& client = wxART_OTHER, const wxSize& size = wxDefaultSize);
protected:
   virtual wxBitmap CreateBitmap(const wxArtID&, const wxArtClient&, const wxSize&);
};
#endif

#ifndef __WXCODE_H__

inline void wxListCtrl_SelectAll(wxListCtrl* ctrl, bool on = true)
{
   for (int i = 0; i < ctrl->GetItemCount(); i++)
   {
      ctrl->SetItemState(i, on ? wxLIST_STATE_SELECTED : 0, wxLIST_STATE_SELECTED);
   }
}
#endif

extern wxString wxGetAccelText(int flags, int keyCode);

#ifndef __WXCODE_H__

typedef struct _wxTOOLBARITEM
{
   int id;
   const wxChar* id_art;
   const wxChar* text;
   const wxChar* help;
   int           flags;
   int           keyCode;
} wxTOOLBARITEM;

#endif

extern bool wxListCtrl_GetItemRect(const wxListView&, long row, long col, wxRect*);
extern long wxListView_HitTest(const wxListView&, const wxPoint&, int* flags, long* col);
extern wxString wxListView_GetItemText(const wxListCtrl&, int row, int col);
extern bool wxListCtrl_EndEditLabel(wxListCtrl* ctrl, bool cancel);

#ifndef __WXCODE_H__

inline bool wxListView_SetCurSel(wxListCtrl* ctrl, long index, bool focus = true)
{ 
   bool ok = (index != -1) && (index < ctrl->GetItemCount());
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

inline void wxPostCommandEvent(wxEvtHandler *dest, wxEventType commandType, int id)
{
   wxCommandEvent event(commandType, id);
   if (dest == NULL) dest = wxTheApp->GetTopWindow();
   wxPostEvent(dest, event);
}

inline void wxPostMenuCommand(wxEvtHandler* dest, int id)
{
   wxPostCommandEvent(dest, wxEVT_COMMAND_MENU_SELECTED, id);
}
