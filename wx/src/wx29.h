// wx29.h

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

#endif // __WX29_H__
