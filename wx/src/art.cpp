// art.cpp
// Copyright (c) 2007-2014 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/wx.h"

#if (wxVERSION_NUMBER < 3000)
#include "../art/help.xpm"
#endif
#include "../art/plus.xpm"
#include "../art/tango/print-preview16.xpm"

/////////////////////////////////////////////////////////////////////////////
// ArtProvider

class ArtProvider : public wxArtProvider
{
   typedef wxArtProvider base;
public:
protected:
   virtual wxBitmap CreateBitmap(const wxArtID&, const wxArtClient&, const wxSize&);
};

#define ART(artid, xpmRc) \
    if (id == (artid)) return wxBitmap(xpmRc##_xpm);

wxBitmap ArtProvider::CreateBitmap(const wxArtID& id, const wxArtClient& WXUNUSED(client), const wxSize& WXUNUSED(size))
{
    if (id == wxART_APP)
        return wxBitmap(wxICON(app));
#if (wxVERSION_NUMBER < 3000)
    ART(wxART_HELP      , help)
#endif
    ART(wxART_ADD       , plus)
    ART(wxART_PREVIEW   , print_preview16)
    return wxNullBitmap;
}

/////////////////////////////////////////////////////////////////////////////
// ArtModule
// trac.wxwidgets.org/ticket/12519

class ArtModule : public wxModule
{
   DECLARE_DYNAMIC_CLASS(ArtModule)
public:
   ArtModule() : wxModule() {}

   virtual bool OnInit()
   {
      wxArtProvider::Push(new ArtProvider());
      return true;
   }
   virtual void OnExit()
   {
   }
};

IMPLEMENT_DYNAMIC_CLASS(ArtModule, wxModule)

/////////////////////////////////////////////////////////////////////////////
