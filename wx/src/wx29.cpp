// wx29.cpp
// Copyright (c) 2010-2012 by Troels K. All rights reserved.
// License: wxWindows

#include "precomp.h"
#include "wx29.h"

BEGIN_EVENT_TABLE(wxDocManagerEx, wxDocManager)
END_EVENT_TABLE()

#if (wxVERSION_NUMBER < 2900)
void wxDocManagerEx::OnUpdateDisableIfNoDoc(wxUpdateUIEvent& event)
{
    event.Enable( GetCurrentDocument() != NULL );
}
#endif

#if (wxVERSION_NUMBER < 2900)
wxString wxListCtrl_GetItemText(const wxListCtrl& ctrl, long row, long col)
{
   wxString str;
   wxListItem info;

   info.m_itemId = row;
   info.m_col = col;
   info.m_mask = wxLIST_MASK_TEXT;
   if (ctrl.GetItem(info))
   {
      str = info.m_text;
   }
   return str;
}
#endif
