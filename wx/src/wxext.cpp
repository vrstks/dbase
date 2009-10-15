// wxext.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include "wxext.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(AcceleratorArray)

bool wxGetExeFolder(wxFileName* filename)
{
   filename->Assign(wxTheApp->argv[0]);
   filename->SetFullName(wxEmptyString);
   return filename->IsOk();
}

bool wxGetDevFolder(wxFileName* filename)
{
   wxGetExeFolder(filename);
   filename->RemoveLastDir();
   filename->AppendDir(wxT("dbf"));
   filename->AppendDir(wxT("wx"));
   return filename->DirExists();
}

bool wxInitXRC()
{
   wxXmlResource::Get()->InitAllHandlers();

   const wxString fullname = wxTheApp->GetAppName() + wxT(".xrc");

   wxFileName filename;
   wxGetExeFolder(&filename);
   filename.SetFullName(fullname);
   if (!filename.FileExists())
   {
      ::wxGetDevFolder(&filename);
      filename.AppendDir(wxT("src"));
      filename.AppendDir(wxT("res"));
      filename.SetFullName(fullname);
   }
   return wxXmlResource::Get()->Load(filename.GetFullPath());
}

wxArtID wxID2ArtID(int wx_id)
{
   wxArtID str;
   static const struct _MAP
   {
      int id;
      const wxChar* id_art;
   } aMap[] =
   {
      { wxID_UP            , wxART_GO_UP           },
      { wxID_DOWN          , wxART_GO_DOWN         },
      { wxID_BACKWARD      , wxART_GO_BACK         },
      { wxID_FORWARD       , wxART_GO_FORWARD      },
      { wxID_EXIT          , wxART_QUIT            },
      { wxID_CLOSE         , wxART_QUIT            },
      { wxID_HELP          , wxART_HELP            },
      { wxID_ABOUT         , wxART_HELP            },
      { wxID_COPY          , wxART_COPY            },
      { wxID_CUT           , wxART_CUT             },
      { wxID_PASTE         , wxART_PASTE           },
      { wxID_DELETE        , wxART_DELETE          },
      { wxID_NEW           , wxART_NEW             },
      { wxID_UNDO          , wxART_UNDO            },
      { wxID_REDO          , wxART_REDO            },
      { wxID_FIND          , wxART_FIND            },
      { wxID_REPLACE       , wxART_FIND_AND_REPLACE},
      { wxID_REPLACE_ALL   , wxART_FIND_AND_REPLACE},
      { wxID_OPEN          , wxART_FILE_OPEN       },
      { wxID_SAVE          , wxART_FILE_SAVE       },
      { wxID_SAVEAS        , wxART_FILE_SAVE_AS    },
      { wxID_HOME          , wxART_GO_HOME         },
      { wxID_REMOVE        , wxART_DEL_BOOKMARK    },
      { wxID_PRINT         , wxART_PRINT           },
      { wxID_PRINT_SETUP   , wxART_PRINT           },
      { wxID_PREVIEW       , wxART_PREVIEW         },
      { wxID_PREFERENCES   , wxART_PREFERENCES     },
      { wxID_SETUP         , wxART_SETUP           },
      { wxID_VIEW_DETAILS  , wxART_REPORT_VIEW     },
      { wxID_VIEW_LIST     , wxART_LIST_VIEW       },
      { wxID_ZOOM_IN       , wxART_ZOOM_IN         },
      { wxID_ZOOM_OUT      , wxART_ZOOM_OUT        },
      { wxID_ZOOM_100      , wxART_ZOOM_100        },
      { wxID_ADD           , wxART_ADD             }
   };
   for (size_t i = 0; i < WXSIZEOF(aMap); i++)
   {
      if (aMap[i].id == wx_id)
      {
         str = aMap[i].id_art;
         break;
      }
   }
   return str;
}

/*static*/ wxBitmap wxArtProviderEx::GetBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
   return wxArtProviderEx().CreateBitmap(id, client, size);
}

#include "res/help.xpm"
#include "res/plus.xpm"
#include "res/preview.xpm"

#define ART(artid, xpmRc) \
    if (id == (artid)) return wxBitmap(xpmRc##_xpm);

wxBitmap wxArtProviderEx::CreateBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& size)
{
    ART(wxART_HELP      , help)
    ART(wxART_ADD       , plus)
    ART(wxART_PREVIEW   , preview)
    return wxArtProvider::GetBitmap(id, client, size);
}

wxString wxGetAccelText(int flags, int keyCode)
{
   wxString str;
#if 0 && (wxVERSION_NUMBER >= 2800)
   wxAcceleratorEntry entry(flags, keyCode);
   str = entry.ToString(); // doesn't work (wxIsalnum(WXK_F2)), silly text (WXK_NUMPAD_ADD)
#else
   if (flags & wxACCEL_CTRL)
   {
      if (!str.IsEmpty()) str+='+';
      str+=_("Ctrl");
   }
   if (flags & wxACCEL_ALT)
   {
      if (!str.IsEmpty()) str+='+';
      str+=_("Alt");
   }
   if (flags & wxACCEL_SHIFT)
   {
      if (!str.IsEmpty()) str+='+';
      str+=_("Shift");
   }
   if (!str.IsEmpty()) str+='+';
   switch (keyCode)
   {
      case WXK_INSERT         : str+=_("Insert" ); break;
      case WXK_PAGEUP         : str+=_("PgUp"   ); break;
      case WXK_PAGEDOWN       : str+=_("PgDn"   ); break;
      case WXK_HOME           : str+=_("Home"   ); break;
      case WXK_END            : str+=_("End"    ); break;
      case WXK_RETURN         : str+=_("Return" ); break;
      case WXK_DELETE         : str+=_("Delete" ); break;
      case WXK_NUMPAD_ADD     : str+=_("Num+"   ); break;
      case WXK_NUMPAD_SUBTRACT: str+=_("Num-"   ); break;
      default:
         if ( (keyCode >= WXK_F1) && (keyCode <= WXK_F24) )
         {
            str+=wxString::Format(wxT("F%d"), keyCode - WXK_F1 + 1);
         }
         else str+=(wxChar)keyCode;
         break;
   }
#endif
   return str;
}

bool wxListCtrl_GetItemRect(const wxListView& ctrl, long row, long col, wxRect* rect)
{
   bool ok = ctrl.GetItemRect(row, *rect, wxLIST_RECT_BOUNDS);
   if (ok)
   {
      for (int i = 0/*, count = ctrl.GetColumnCount()*/; ; i++)
      {
         const int width = ctrl.GetColumnWidth(i);
         if (i < col)
         {
            rect->x+=width;
         }
         else if (i == col)
         {
            rect->width = width;
            break;
         }
         else
         {
            ok = false;
            break;
         }
      }
   }
   return ok;
}

long wxListView_HitTest(const wxListView& ctrl, const wxPoint& point, int* flags, long* col)
{
   int rFlags;
   const long row = ((wxListCtrl&)/*unconst*/ctrl).HitTest(point, rFlags);
   if (col && (row != wxNOT_FOUND))
   {
      const int offset = ctrl.GetScrollPos(wxHORIZONTAL);
      int pos = 0;
      *col = -1;
      for (int i = 0, count = ctrl.GetColumnCount(); i < count; i++)
      {
         pos+=ctrl.GetColumnWidth(i);
         if ((point.x + offset) < pos)
         {
            *col = i;
            break;
         }
      }
   }
   if (flags) *flags = rFlags;
   return row;
}

wxString wxListView_GetItemText(const wxListCtrl& ctrl, int row, int col)
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

#if defined(__WXMSW__)
#include <commctrl.h>
bool wxListCtrl_EndEditLabel(wxListCtrl* ctrl, bool cancel)
{
   HWND hwnd = ListView_GetEditControl((HWND)ctrl->GetHWND());
   bool b = (hwnd != NULL);
   if (b) ::PostMessage(hwnd, WM_CLOSE, 0, 0);
   return b;
}
#else
bool wxListCtrl_EndEditLabel(wxListCtrl* ctrl, bool cancel)
{
#ifdef __WXMSW__
   return ctrl->EndEditLabel(cancel);
#else
   return false;
#endif
}
#endif

#if wxUSE_ACCEL

wxAcceleratorEntry wxGetStockAcceleratorEx(wxWindowID id)
{
    wxAcceleratorEntry ret;

    #define STOCKITEM(stockid, flags, keycode)      \
        case stockid:                               \
            ret.Set(flags, keycode, stockid);       \
            break;

    switch (id)
    {
        STOCKITEM(wxID_PRINT,                wxACCEL_CMD,'P')
        STOCKITEM(wxID_PREVIEW,              wxACCEL_CMD | wxACCEL_SHIFT,'P')
        STOCKITEM(wxID_SAVEAS,               wxACCEL_CMD | wxACCEL_SHIFT,'S')
        STOCKITEM(wxID_SELECTALL,            wxACCEL_CMD,'A')
        STOCKITEM(wxID_UNDO,                 wxACCEL_CMD,'Z')
        STOCKITEM(wxID_REDO,                 wxACCEL_CMD,'Y')
        //STOCKITEM(wxID_PREFERENCES,          wxACCEL_CMD,'T')
        STOCKITEM(wxID_ICONIZE_FRAME,        wxACCEL_ALT,WXK_FULLSCREEN)
        STOCKITEM(wxID_REFRESH,              wxACCEL_NORMAL, WXK_F5)
        STOCKITEM(wxID_PROPERTIES,           wxACCEL_ALT,WXK_RETURN)
        STOCKITEM(wxID_BACKWARD,             wxACCEL_ALT , WXK_LEFT)
        STOCKITEM(wxID_FORWARD,              wxACCEL_ALT , WXK_RIGHT)
        STOCKITEM(wxID_HELP,                 wxACCEL_NORMAL, WXK_HELP)
        STOCKITEM(wxID_REPLACE,              wxACCEL_CMD,'H')
        STOCKITEM(wxID_EXIT,                 wxACCEL_CTRL, 'Q')
        STOCKITEM(wxID_CLOSE,                wxACCEL_CTRL, 'W')
        default:
            //if (id == wxXRCID_GOTO) ret.Set(wxACCEL_CTRL,'G',id);
            //else
            ret = wxGetStockAccelerator(id);
            break;
    };

    #undef STOCKITEM

    // always use wxAcceleratorEntry::IsOk on returned value !
    return ret;
}

#endif // wxUSE_ACCEL

void wxSetAcceleratorTable(wxWindow* wnd, const AcceleratorArray& array)
{
   const size_t count = array.GetCount();
   wxAcceleratorEntry* temp = new wxAcceleratorEntry[count];
   for (size_t i = 0; i < count; i++)
   {
      temp[i] = array.Item(i);
   }
   wxAcceleratorTable accel(count, temp);
   wnd->SetAcceleratorTable(accel);
   wxDELETEA(temp);
}

wxString wxMenuItem_GetText(const wxMenuItem* item)
{
   wxString str = item->GetItemLabel();
#ifdef __WXGTK__
   wxString_Replace(&str, '_', '&');
#endif
   return str;
}

#define ACCELSTR_SEP "   "

bool wxMenuItem_SetAccelText(wxMenuItem* item, const wxString& accel, bool append)
{
   wxString str = wxMenuItem_GetText(item);
   wxString ch_sep = wxT("\t");
   const int sep = str.Find(ch_sep);

   if (wxNOT_FOUND == sep)
   {
   }
   else if (append)
   {
      ch_sep = wxT(ACCELSTR_SEP);
   }
   else
   {
      str.Truncate(sep);
   }
   item->SetItemLabel(wxString::Format(wxT("%s%s%s"),
      str.wx_str(),
      ch_sep.wx_str(),
      accel.wx_str()));
   return true;
}

wxString wxGetAccelText(const wxAcceleratorEntry& accel)
{
   return wxGetAccelText(accel.GetFlags(), (enum wxKeyCode)accel.GetKeyCode());
}

void wxMenuBar_Fixup(wxMenuBar* menu, const AcceleratorArray& array)
{
   const size_t count = array.GetCount();
   for (size_t i = 0; i < count; i++)
   {
      const wxAcceleratorEntry& entry = array.Item(i);
      wxMenuItem* item = menu->FindItem(entry.GetCommand());
      if (item)
      {
         wxMenuItem_SetAccelText(item, wxGetAccelText(entry), true);
      }
   }
}

wxString wxGetStockLabelEx(wxWindowID id, bool withCodes)
{
    #define STOCKITEM(stockid, label) \
        case stockid:                 \
            stockLabel = label;       \
            break;

   wxString stockLabel;
   switch (id)
   {
     STOCKITEM(wxID_ICONIZE_FRAME,            _("&Minimize"))
     STOCKITEM(wxID_VIEW_SORTNAME,            _("&Sort by name"))
     STOCKITEM(wxID_VIEW_SORTTYPE,            _("&Sort by type"))
     STOCKITEM(wxID_VIEW_SORTDATE,            _("&Sort by date"))
     STOCKITEM(wxID_VIEW_SORTSIZE,            _("&Sort by size"))
     STOCKITEM(wxID_VIEW_LIST,                _("&List mode"))
     STOCKITEM(wxID_VIEW_DETAILS,             _("&Details mode"))
     STOCKITEM(wxID_VIEW_LARGEICONS,          _("&Large icons mode"))
     STOCKITEM(wxID_VIEW_SMALLICONS,          _("&Small icons mode"))
     STOCKITEM(wxID_SETUP          ,          _("&Setup..."))
      default:
         //if (id == wxXRCID_GOTO) stockLabel = _("&Goto...");
         //else if (id == wxXRCID_RUN) stockLabel = _("&Run");
         break;
   }
    #undef STOCKITEM
#if (wxVERSION_NUMBER >= 2800)
   if (stockLabel.IsEmpty()) stockLabel = wxGetStockLabel(id, withCodes ? wxSTOCK_WITH_MNEMONIC : 0);
#else
   if (stockLabel.IsEmpty()) stockLabel = wxGetStockLabel(id, withCodes, accelerator);
#endif
   if (!withCodes)
   {
      stockLabel = wxStripMenuCodes(stockLabel);
      wxString_RemoveEllipsis(&stockLabel);
   }
   return stockLabel;
}

