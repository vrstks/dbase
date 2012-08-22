// dbfframe.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DBFFRAME_H__
#define __DBFFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// DBFFrame

class wxRecentFileList;
class wxArrayAccelerator;
class DBFFrame : public wxDocMDIChildFrame
{
   DECLARE_DYNAMIC_CLASS(DBFFrame)
   typedef wxDocMDIChildFrame base;
   wxRecentFileList* m_mru;
public:
   DBFFrame(void);

   virtual bool Create(wxDocument*, wxMDIParentFrame*, wxRecentFileList*,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE | wxMAXIMIZE);

#if (wxVERSION_NUMBER < 2900)
   wxMDIParentFrame* GetMDIParent() const;
#endif
   static const wxArrayAccelerator& GetAccelerator();

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
