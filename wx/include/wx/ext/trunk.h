// wx/wxt/trunk.h
// Copyright (c) 2010-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __WX_EXT_TRUNK_H__
#define __WX_EXT_TRUNK_H__

#ifndef wxCLASSINFO
    #define wxCLASSINFO CLASSINFO
#endif

#ifndef wxAPPLY
    #define wxAPPLY 0x00000020
#endif

#if (wxVERSION_NUMBER < 2905)
#include <vector>
typedef std::vector<wxDocument*> wxDocVector;
#endif

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
      if (m_appDisplayName.empty())
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
inline wxString wxJoin(const wxArrayString& as, wxChar sep, wxChar escape = wxT('\\'))
{
   wxString str;
   wxUnusedVar(escape);

   for (wxArrayString::const_iterator it = as.begin();
        it != as.end();
        it++)
   {
      if (it != as.begin()) str+=sep;
      str+=*it;
   }
   return str;
}
#endif

#if (wxVERSION_NUMBER < 2902)
#define _WX_VERSIONINFO_H_

class wxVersionInfo
{
public:
    wxVersionInfo(const wxString& name,
                  int major,
                  int minor,
                  int micro = 0,
                  const wxString& description = wxString(),
                  const wxString& copyright = wxString())
    {
        m_name = name;
        m_major = major;
        m_minor = minor;
        m_micro = micro;
        m_description = description;
        m_copyright = copyright;
    }

    // Default copy ctor, assignment operator and dtor are ok.


    const wxString& GetName() const { return m_name; }

    int GetMajor() const { return m_major; }
    int GetMinor() const { return m_minor; }
    int GetMicro() const { return m_micro; }

    wxString ToString() const
    {
        return HasDescription() ? GetDescription() : GetVersionString();
    }

    wxString GetVersionString() const
    {
        wxString str;
        str << m_name << wxT(' ') << GetMajor() << wxT('.') << GetMinor();
        if ( GetMicro() )
            str << wxT('.') << GetMicro();

        return str;
    }

    bool HasDescription() const { return !m_description.empty(); }
    const wxString& GetDescription() const { return m_description; }

    bool HasCopyright() const { return !m_copyright.empty(); }
    const wxString& GetCopyright() const { return m_copyright; }

private:
    wxString m_name,
             m_description,
             m_copyright;

    int m_major,
        m_minor,
        m_micro;
};

#endif // _WX_VERSIONINFO_H_

#endif // __WX_EXT_TRUNK_H__
