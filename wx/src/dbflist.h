// dbflist.h - editable
// Copyright (c) 2007-2013 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
   #include "datalist.h"
#endif

class DBFListCtrl : public DataModelListCtrl
{
public:
    DBFListCtrl() : DataModelListCtrl()
    {
    }

    bool Edit(long row, long col);
    bool Edit();
    bool AddNew();

protected:
    void OnDblClick(wxListCellEvent&);
    DECLARE_EVENT_TABLE()
};
