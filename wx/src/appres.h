// appres.h
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

class Resource
{
    wxFileName m_xrcFileName;
public:
    bool Init();

    virtual ~Resource();
};
