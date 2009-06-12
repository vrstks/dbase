// dbfframe.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFFRAME_H__
#define __DBFFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// wxDBFFrame

class wxDBFFrame : public wxDocMDIChildFrame
{
   typedef wxDocMDIChildFrame base;
public:
   wxMenu* m_file_menu;

   wxDBFFrame(wxDocument*, wxView*, wxMDIParentFrame*);

   virtual ~wxDBFFrame();
#ifndef __WXMSW__
    virtual void SetLabel(const wxString& label)
    {
       SetTitle(label);
    }
#endif
protected:
   void OnActivate(wxActivateEvent&);
   DECLARE_EVENT_TABLE()
};

#endif // __DBFFRAME_H__
