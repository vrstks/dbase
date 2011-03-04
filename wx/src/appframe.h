// appframe.h
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __APPFRAME_H__
#define __APPFRAME_H__

/////////////////////////////////////////////////////////////////////////////
// MainFrame

class WXDLLIMPEXP_FWD_ADV wxAboutDialogInfo;

class MainFrame : public wxDocMDIParentFrame
{
   typedef wxDocMDIParentFrame base;
   DECLARE_CLASS(MainFrame)
public:
   MainFrame(void);

   bool Create(wxDocManager*, const wxString& title, 
       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);

   virtual void SetMenuBar(wxMenuBar*);
   virtual ~MainFrame();

   static void GetVersionInfo(wxAboutDialogInfo*);
protected:
    wxMenuBar* CreateMenuBar() const;
    wxToolBar* CreateToolBar();

protected:
   void OnStatusBar  (wxCommandEvent&);
   void OnToolBar    (wxCommandEvent&);
   void OnFullscreen (wxCommandEvent&);
   
   void OnUpdateFullscreen (wxUpdateUIEvent&);
   void OnUpdateStatusBar  (wxUpdateUIEvent&);
   void OnUpdateToolBar    (wxUpdateUIEvent&);

   DECLARE_EVENT_TABLE()
};

#endif // __APPFRAME_H__
