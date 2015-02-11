// printout.h
// Copyright (c) 2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __PRINTOUT_H__
#define __PRINTOUT_H__

#ifdef _WX_HTMPRINT_H_
class wxDataModel;
class DataModelPrintout : public wxHtmlPrintout
{
public:
    DataModelPrintout(wxDataModel*, const std::string& title);
};
#endif

#endif // __PRINTOUT_H__
