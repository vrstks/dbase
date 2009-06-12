// app.h
// Copyright (c) 2006 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __APP_H__
#define __APP_H__

/////////////////////////////////////////////////////////////////////////////
// App

class MainFrame;
class App : public wxApp
{
   typedef wxApp base;
protected:
   wxDocManager* m_docManager;
   wxLocale m_locale;
public:
   App(void);

   MainFrame* GetMainFrame(void);

   virtual bool OnInit(void);
   virtual int OnExit(void);

   wxMDIChildFrame* NewFrame(wxDocument*, wxView*);
   void FileHistoryUseMenu(wxMenu*);
   void FileHistoryRemoveMenu(wxMenu*);
   void FileHistoryLoadSave(bool save);
protected:
   void AddDocTemplates();
   
protected:
   DECLARE_EVENT_TABLE()
};

DECLARE_APP(App)

#endif // __APP_H__
