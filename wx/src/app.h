// app.h
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __APP_H__
#define __APP_H__

#include "wx29.h" // wxAppEx

/////////////////////////////////////////////////////////////////////////////
// App

class MainFrame;
class AcceleratorArray;
class wxRecentFileList;

class App : public wxAppEx
{
   typedef wxAppEx base;
protected:
   wxRecentFileList* m_mru;
   wxLocale m_locale;
public:
   App(void);

   virtual bool OnInit(void);
   virtual int OnExit(void);

   static const AcceleratorArray& GetAccelerator();

   wxRecentFileList* GetRecentFileList() const { return m_mru; }

protected:
   wxDocManager* CreateDocManager();
   
protected:
   DECLARE_EVENT_TABLE()
};

DECLARE_APP(App)

#endif // __APP_H__
