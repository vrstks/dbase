// dbfframe.h
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFFRAME_H__
#define __DBFFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// DBFFrame

class wxRecentFileList;
class wxArrayAcceleratorEntry;
class DBFFrame : public wxDocMDIChildFrame
{
   DECLARE_DYNAMIC_CLASS(DBFFrame)
   typedef wxDocMDIChildFrame base;
   wxRecentFileList* m_mru;
public:
   DBFFrame(void);

   bool Create(wxDocument*, wxMDIParentFrame*, wxRecentFileList*);

#if (wxVERSION_NUMBER < 2900)
   wxMDIParentFrame* GetMDIParent() const;
#endif
   static const wxArrayAcceleratorEntry& GetAccelerator();

   virtual ~DBFFrame();
   virtual void SetMenuBar(wxMenuBar*);
#if (wxVERSION_NUMBER < 2900)
   // To get statusbar menu help
   virtual void DoGiveHelp(const wxString& text, bool show);
#endif
    virtual void Raise();
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
