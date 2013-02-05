// dbfres.cpp
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wx/ext/wx.h"
#include "dbfres.h"
#include "dbfdefs.h"

#include "../xmlres/wxdbf.xrc.c"

bool DBFResource::Init()
{
    const wxString fileName = wxString(WXDBF_APP_EXETITLE) + wxFILE_SEP_EXT + wxXmlResourceHelper::FileExt;
    bool ok;
    
#ifdef _DEBUG
    // load xrc file directly
    ok = wxXmlResourceHelper::LoadFromFile(__FILE__, fileName);
    if (!ok) wxLogError(_("Failed to load resources from %s"), fileName.wx_str());
#else
    // load xrc file from temp file
    ok = wxXmlResourceHelper::LoadFromMemory(wxdbf_xrc, sizeof(wxdbf_xrc), fileName, &m_xrcFile);
    if (!ok) wxLogError(_("Failed to load resources"));
#endif
    return ok;
}

DBFResource::~DBFResource(void)
{
    ::wxRemoveFile(&m_xrcFile);
}
