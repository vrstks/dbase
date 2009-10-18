// wx29.h
// Copyright (c) 2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __WX29_H__
#define __WX29_H__

#if defined(_WX_ABOUTDLG_H_) && (wxVERSION_NUMBER < 2900)
inline void wxAboutBox(const wxAboutDialogInfo& info, wxWindow* WXUNUSED(parent))
{
   wxAboutBox(info);
}
#endif

inline wxString wxGetAppDisplayName()
{
#if (wxVERSION_NUMBER >= 2900)
   return wxTheApp->GetAppDisplayName();
#else
   return wxTheApp->GetAppName();
#endif
}

class wxDocManagerEx : public wxDocManager
{
protected:
#if (wxVERSION_NUMBER < 2900)
   void OnUpdateDisableIfNoDoc(wxUpdateUIEvent&);
#endif
   DECLARE_EVENT_TABLE()
};

#endif // __WX29_H__
