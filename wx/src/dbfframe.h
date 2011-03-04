// dbfframe.h
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFFRAME_H__
#define __DBFFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// DBFFrame

class DBFFrame : public wxDocMDIChildFrame
{
   typedef wxDocMDIChildFrame base;
public:
   DBFFrame(void);

   bool Create(wxDocument*, wxMDIParentFrame*);

#if (wxVERSION_NUMBER < 2900)
   wxMDIParentFrame* GetMDIParent() const;
#endif

   virtual ~DBFFrame();
   virtual void SetMenuBar(wxMenuBar*);
#if (wxVERSION_NUMBER < 2900)
   // To get statusbar menu help
   virtual void DoGiveHelp(const wxString& text, bool show);
#endif
#ifdef __WXGTK__
    virtual void SetLabel(const wxString& label)
    {
       SetTitle(label);
    }
#endif

protected:
   wxMenuBar* CreateMenuBar() const;
protected:
   DECLARE_EVENT_TABLE()
};

#endif // __DBFFRAME_H__
