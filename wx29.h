// wx29.h

#ifndef __WX29_H__
#define __WX29_H__

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

#endif // __WX29_H__
