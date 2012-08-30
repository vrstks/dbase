// art.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/wx.h"

#include "../art/help.xpm"
#include "../art/plus.xpm"
#include "../art/preview.xpm"

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
   ART(wxART_HELP      , help)
   ART(wxART_ADD       , plus)
   ART(wxART_PREVIEW   , preview)
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
