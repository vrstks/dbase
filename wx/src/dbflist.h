// dbflist.h - editable
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#ifndef __DATALIST_H__
   #include "datalist.h"
#endif

class wxDBFListCtrl : public wxDataListCtrl
{
public:
   wxDBFListCtrl() : wxDataListCtrl()
   {
   }

   bool Edit(size_t row, int col);
   bool Edit();
   bool AddNew();

protected:
   void OnDblClick         (wxCommandEvent&);
   DECLARE_EVENT_TABLE()
};
