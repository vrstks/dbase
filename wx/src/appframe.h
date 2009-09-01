// appframe.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __APPFRAME_H__
#define __APPFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// MainFrame

class MainFrame : public wxDocMDIParentFrame
{
   typedef wxDocMDIParentFrame base;
   DECLARE_CLASS(MainFrame)
public:
   wxMenu* m_file_menu;
   MainFrame(wxDocManager*, wxFrame*, const wxString& title, 
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, 
      long type = wxDEFAULT_FRAME_STYLE);

   wxToolBar* CreateToolBar();

protected:
   void OnStatusBar  (wxCommandEvent&);
   void OnToolBar    (wxCommandEvent&);
   void OnAbout      (wxCommandEvent&);
   void OnFullscreen (wxCommandEvent&);
   
   void OnUpdateFullscreen (wxUpdateUIEvent&);
   void OnUpdateDisable    (wxUpdateUIEvent&);
   void OnUpdateStatusBar  (wxUpdateUIEvent&);
   void OnUpdateToolBar    (wxUpdateUIEvent&);

   DECLARE_EVENT_TABLE()
};

#endif // __APPFRAME_H__
