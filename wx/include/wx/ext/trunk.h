// wx/wxt/trunk.h
// Copyright (c) 2010-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __WX_EXT_TRUNK_H__
#define __WX_EXT_TRUNK_H__

#ifndef wxCLASSINFO
#define wxCLASSINFO CLASSINFO
#endif
#ifndef wxDECLARE_EVENT_TABLE
#define wxDECLARE_EVENT_TABLE DECLARE_EVENT_TABLE
#endif
#ifndef wxBEGIN_EVENT_TABLE
#define wxBEGIN_EVENT_TABLE BEGIN_EVENT_TABLE
#endif
#ifndef wxEND_EVENT_TABLE
#define wxEND_EVENT_TABLE END_EVENT_TABLE
#endif
#ifndef wxDECLARE_CLASS
#define wxDECLARE_CLASS DECLARE_CLASS
#endif
#ifndef wxIMPLEMENT_CLASS
#define wxIMPLEMENT_CLASS IMPLEMENT_CLASS
#endif
#ifndef wxDECLARE_DYNAMIC_CLASS
#define wxDECLARE_DYNAMIC_CLASS DECLARE_DYNAMIC_CLASS
#endif
#ifndef wxIMPLEMENT_DYNAMIC_CLASS
#define wxIMPLEMENT_DYNAMIC_CLASS IMPLEMENT_DYNAMIC_CLASS
#endif
#ifndef wxDECLARE_NO_COPY_CLASS
#define wxDECLARE_NO_COPY_CLASS DECLARE_NO_COPY_CLASS
#endif
#ifndef wxCMD_LINE_DESC_END
#define wxCMD_LINE_DESC_END { wxCMD_LINE_NONE, NULL, NULL, NULL, wxCMD_LINE_VAL_NONE, 0x0 }
#endif
#ifndef wxSTB_DEFAULT_STYLE
#define wxSTB_DEFAULT_STYLE    (wxST_SIZEGRIP|wxFULL_REPAINT_ON_RESIZE)
#endif
#ifndef wxAPPLY
#define wxAPPLY 0x00000020
#endif

#ifndef wxEVENT_HANDLER_CAST
    #define wxEVENT_HANDLER_CAST( functype, func ) \
        ( wxObjectEventFunction )( wxEventFunction )wxStaticCastEvent( functype, &func )
#endif

#if (wxVERSION_NUMBER < 2905)
#include <vector>
typedef std::vector<wxDocument*> wxDocVector;
#define wxALIGN_INVALID ((wxAlignment)-1)
#endif

#ifdef _WX_DOCH__
class wxTrunkDocView
{
public:
#if (wxVERSION_NUMBER >= 2905)
    static inline wxDocVector GetDocumentsVector(const wxDocManager& docManager)
    {
        return docManager.GetDocumentsVector();
    }
#else
    static wxDocVector GetDocumentsVector(const wxDocManager&);
#endif
    static inline void ActivateDocument(wxDocument* doc)
    {
    #if (wxVERSION_NUMBER >= 2905)
        doc->Activate();
    #else
        doc->GetFirstView()->GetFrame()->Raise();
    #endif
    }
};
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
