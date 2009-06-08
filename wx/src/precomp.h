// precomp.h
// Copyright (c) 2007 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/wx.h>
#include <wx/mdi.h>
#include <wx/docview.h>
#include <wx/docmdi.h>
#include <wx/txtstrm.h>
#include <wx/listctrl.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/config.h>
#include <wx/clipbrd.h>
#include <wx/dataview.h>

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#if !wxUSE_MDI_ARCHITECTURE
#error You must set wxUSE_MDI_ARCHITECTURE to 1 in setup.h!
#endif
