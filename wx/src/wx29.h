// wx29.h
// Copyright (c) 2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __WX29_H__
#define __WX29_H__

#if defined(_WX_ABOUTDLG_H_) && (wxVERSION_NUMBER < 2900)
inline void wxAboutBox(const wxAboutDialogInfo& info, wxWindow* WXUNUSED(parent))
{
   wxAboutBox(info);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// wxAppEx

class wxAppEx : public wxApp
{
   typedef wxApp base;
#if (wxVERSION_NUMBER < 2900)
protected:
   wxString m_appDisplayName;
public:
   wxString GetAppDisplayName() const { return m_appDisplayName; }

   void SetAppName(const wxString& name)
   {
      base::SetAppName(name);   
      if (m_appDisplayName.IsEmpty())
      {
         SetAppDisplayName(name);
      }
   }
   void SetAppDisplayName(const wxString& name) { m_appDisplayName = name; }
#endif
};

/////////////////////////////////////////////////////////////////////////////
// wxDocManagerEx

class wxDocManagerEx : public wxDocManager
{
protected:
#if (wxVERSION_NUMBER < 2900)
   void OnUpdateDisableIfNoDoc(wxUpdateUIEvent&);
#endif
   DECLARE_EVENT_TABLE()
};

#if (wxVERSION_NUMBER < 2900)
inline wxString wxJoin(const wxArrayString& as, wxChar sep)
{
   wxString str;
   for (size_t i = 0; i < as.GetCount(); i++)
   {
      if (i) str+=sep;
      str+=as.Item(i);
   }
   return str;
}
#endif

#endif // __WX29_H__
