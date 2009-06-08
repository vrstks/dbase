// dbflist.h - editable

#ifndef __DATALIST_H__
   #include "datalist.h"
#endif

class wxDBFListCtrl : public wxDataListCtrl
{
public:
   wxDBFListCtrl(wxWindow* parent, const wxPoint& pos, const wxSize& size, long style = wxLC_REPORT | wxLC_VIRTUAL) :
      wxDataListCtrl(parent, pos, size, style)
   {
   }

   bool Edit(size_t row, int col);
   bool Edit();
   bool AddNew();

protected:
   void OnDblClick         (wxCommandEvent&);
   DECLARE_EVENT_TABLE()
};
