// wx29.cpp
// Copyright (c) 2010 by Troels K. All rights reserved.
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
