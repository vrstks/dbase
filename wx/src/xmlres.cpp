// xmlres.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wxext.h"
#include "xmlres.h"

#include "../xmlres/wxdbf.xrc.c"

bool Resource::Init()
{
    const wxString filename = wxTheApp->GetAppName().Lower() + wxFILE_SEP_EXT + wxXmlResourceHelper::FileExt;
    
#ifdef __WXDEBUG__
    // load xrc file directly
    return wxXmlResourceHelper::LoadFromFile(__FILE__, filename);
#else
    // load xrc file from temp file
    return wxXmlResourceHelper::LoadFromMemory(wxdbf_xrc, sizeof(wxdbf_xrc), filename, &m_xrcFile);
#endif
}

Resource::~Resource(void)
{
    if (m_xrcFile.IsOpened())
    {
        wxString filename = m_xrcFile.GetName();

        m_xrcFile.Close();
        wxLogNull no_log;
        wxRemoveFile(filename);
    }
}
