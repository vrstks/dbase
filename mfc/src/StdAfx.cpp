// stdafx.cpp : source file that includes just the standard includes

/* 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "stdafx.h"

/********************************************************************/
/*																	*/
/* Function name : AutoSizeColumns									*/
/* Description   : 													*/
/*																	*/
/********************************************************************/
void AutoSizeColumns(CListCtrl *pListCtrl) 
{
	// Call this after your the control is filled
	pListCtrl->SetRedraw(FALSE);
	int mincol = 0;
    int maxcol = pListCtrl->GetHeaderCtrl()->GetItemCount()-1;
    for (int col = mincol; col <= maxcol; col++) 
	{
		pListCtrl->SetColumnWidth(col, LVSCW_AUTOSIZE);
        int wc1 = pListCtrl->GetColumnWidth(col);
        pListCtrl->SetColumnWidth(col, LVSCW_AUTOSIZE_USEHEADER);
        int wc2 = pListCtrl->GetColumnWidth(col);
        // 10 is minumim column width
		int wc = max(10, max(wc1,wc2));
        pListCtrl->SetColumnWidth(col,wc);
     }
     pListCtrl->SetRedraw(TRUE);
}

