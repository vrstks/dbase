// app.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __APP_H__
#define __APP_H__

#include "wx29.h" // wxAppEx

class CommandLine
{
// Attributes
public:
   wxArrayFileName m_fileNames;

// Construction
public:
   CommandLine();
};

/////////////////////////////////////////////////////////////////////////////
// App

class MainFrame;
class wxArrayAccelerator;
class wxRecentFileList;
class DBFResource;

class App : public wxAppEx
{
   typedef wxAppEx base;
protected:
   CommandLine       m_cmdline;
   wxRecentFileList* m_mru;
   wxLocale          m_locale;
   DBFResource*      m_res;
public:
   App(void);

   virtual bool OnInit(void);
   virtual int OnExit(void);
   virtual void OnInitCmdLine(wxCmdLineParser&);
   virtual bool OnCmdLineParsed(wxCmdLineParser&);

   wxRecentFileList* GetRecentFileList() const { return m_mru; }

protected:
   wxDocManager* CreateDocManager();
   
protected:
   void OnMenuAbout(wxCommandEvent&);
   DECLARE_EVENT_TABLE()
   friend class DocManager;
};

DECLARE_APP(App)

#endif // __APP_H__
