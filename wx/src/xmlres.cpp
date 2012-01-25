// xmlres.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "wxext.h"
#include "xmlres.h"

#include "../xmlres/wxdbf.xrc.c"

bool Resource::Init(void)
{
    const wxString title = wxTheApp->GetAppName().Lower();
    
#ifdef __WXDEBUG__
    return wxXmlResourceHelper::LoadFromFile(__FILE__, title);
#else
    return wxXmlResourceHelper::LoadFromMemory(wxdbf_xrc, sizeof(wxdbf_xrc), title, &m_xrcFileName);
#endif
}

Resource::~Resource(void)
{
    if (m_xrcFileName.FileExists())
    {
        wxRemoveFile(m_xrcFileName.GetFullPath());
    }
}
