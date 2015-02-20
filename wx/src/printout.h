// printout.h
// Copyright (c) 2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __PRINTOUT_H__
#define __PRINTOUT_H__

#ifdef _WX_HTMPRINT_H_
class wxDataModel;
class DataModelPrintout : public wxHtmlPrintout
{
    typedef wxHtmlPrintout base;
public:
    DataModelPrintout(const wxDataModel*, const std::string& title);

    void SetMargins(const wxPageSetupDialogData& page)
    {
        wxPoint topleft_mm = page.GetMarginTopLeft();
        wxPoint bottomright_mm = page.GetMarginBottomRight();
        base::SetMargins(topleft_mm.y, bottomright_mm.y, topleft_mm.x, bottomright_mm.x);
    }
};
#endif

#endif // __PRINTOUT_H__
