// wxext.cpp
// Copyright (c) 2007-2011 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"
#include <wx/stdpaths.h>

#include "wxext.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(AcceleratorArray)
WX_DEFINE_OBJARRAY(FileNameArray)

IMPLEMENT_CLASS(wxViewEx, wxView)

bool wxWindow_Toggle(wxWindow* wnd)
{
   const bool show = !wnd->IsShown();
   wnd->Show(show);
   wxWindow* parent = wnd->GetParent();
   if (wxIS_KIND_OF(parent, wxFrame))
   {
      wxStaticCast(parent, wxFrame)->SendSizeEvent(); // needed when mdi
   }
   else
   {
      parent->Layout(); // needed when non-mdi
   }
   return show;
}

void wxFrame_ToggleFullScreen(wxFrame* wnd, long style)
{
   wnd->ShowFullScreen(!wnd->IsFullScreen(), style);
}

void wxFrame_OnUpdateFullScreen(wxFrame* wnd, wxUpdateUIEvent& event)
{
   event.Check(wnd->IsFullScreen());
}

bool wxGetExeFolder(wxFileName* filename)
{
   filename->Assign(wxStandardPaths::Get().GetExecutablePath());
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
   const wxString fullname = wxTheApp->GetAppName().Lower() + wxT(".xrc");
   wxFileName filename;

   wxGetExeFolder(&filename);
   filename.SetFullName(fullname);
   if (!filename.FileExists())
   {
      wxFileName dev;

      ::wxGetDevFolder(&dev);
      dev.AppendDir(wxT("src"));
      dev.AppendDir(wxT("res"));
      dev.SetFullName(fullname);
      if (dev.FileExists())
      {
         filename = dev;
      }
   }
   wxXmlResource::Get()->InitAllHandlers();
   return wxXmlResource::Get()->Load(filename.GetFullPath());
}

wxArtID wxID2ArtID(int wx_id)
{
   wxArtID str;
   static const struct _MAP
   {
      int id;
#if (wxVERSION_NUMBER >= 2902)
      const char* id_art;
#else
      const wxChar* id_art;
#endif
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

static wxString wxGetAccelText(int flags, int keyCode)
{
   // wxAcceleratorEntry.ToString() produces silly text
   wxString str;
#if (wxVERSION_NUMBER >= 2901)
   const wxChar sep = '+'; // the new wx default
#else
   const wxChar sep = '-'; // the old annoying wx default
#endif

   if (flags & wxACCEL_CTRL)
   {
      if (str.Length()) str+=sep;
      str+=_("Ctrl");
   }
   if (flags & wxACCEL_ALT)
   {
      if (str.Length()) str+=sep;
      str+=_("Alt");
   }
   if (flags & wxACCEL_SHIFT)
   {
      if (str.Length()) str+=sep;
      str+=_("Shift");
   }
   if (str.Length()) str+=sep;
   switch (keyCode)
   {
      case WXK_INSERT         : str+=_("Insert" ); break;
      case WXK_PAGEUP         : str+=_("PgUp"   ); break;
      case WXK_PAGEDOWN       : str+=_("PgDn"   ); break;
      case WXK_HOME           : str+=_("Home"   ); break;
      case WXK_END            : str+=_("End"    ); break;
      case WXK_RETURN         : str+=_("Return" ); break;
      case WXK_DELETE         : str+=_("Del"    ); break;
      case WXK_NUMPAD_ADD     : str+=_("Num+"   ); break;
      case WXK_NUMPAD_SUBTRACT: str+=_("Num-"   ); break;
      default:
         if ( (keyCode >= WXK_F1) && (keyCode <= WXK_F24) )
         {
            str+=wxString::Format(wxT("F%d"), keyCode - WXK_F1 + 1);
         }
         else
         {
            wxASSERT(keyCode >= ' ');
            str+=(wxChar)keyCode;
         }
         break;
   }
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

bool wxListCtrl_EndEditLabel(wxListCtrl* wnd, bool cancel)
{
#ifdef __WXMSW__
   #if (wxVERSION_NUMBER >= 2901)
      return wnd->EndEditLabel(cancel);
   #else
      HWND hwnd = ListView_GetEditControl((HWND)wnd->GetHWND());
      bool ok = (hwnd != NULL);
      if (ok)
      {
      #ifdef ListView_CancelEditLabel
         if (cancel && (wxApp::GetComCtl32Version() >= 600))
         {
            ListView_CancelEditLabel((HWND)wnd->GetHWND());
            return ok;
         }
      #endif
         if (::IsWindowVisible(hwnd))
         {
            ::SendMessage(hwnd, WM_KEYDOWN, cancel ? VK_ESCAPE : VK_RETURN, 0);
         }
         else
         {
            ::SendMessage(hwnd, WM_CLOSE, 0, 0);
         }
      }
      return ok;
   #endif
#else
   return false;
#endif
}

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
    #if (wxVERSION_NUMBER < 2903)
        STOCKITEM(wxID_PRINT,                wxACCEL_CTRL,'P')
        STOCKITEM(wxID_UNDO,                 wxACCEL_CTRL,'Z')
    #endif
        STOCKITEM(wxID_PREVIEW,              wxACCEL_CTRL | wxACCEL_SHIFT,'P')
        STOCKITEM(wxID_SAVEAS,               wxACCEL_CTRL | wxACCEL_SHIFT,'S')
        STOCKITEM(wxID_SELECTALL,            wxACCEL_CTRL,'A')
        STOCKITEM(wxID_REDO,                 wxACCEL_CTRL,'Y')
        //STOCKITEM(wxID_PREFERENCES,          wxACCEL_CTRL,'T')
        STOCKITEM(wxID_ICONIZE_FRAME,        wxACCEL_ALT,WXK_FULLSCREEN)
        STOCKITEM(wxID_REFRESH,              wxACCEL_NORMAL, WXK_F5)
        STOCKITEM(wxID_PROPERTIES,           wxACCEL_ALT,WXK_RETURN)
        STOCKITEM(wxID_BACKWARD,             wxACCEL_ALT , WXK_LEFT)
        STOCKITEM(wxID_FORWARD,              wxACCEL_ALT , WXK_RIGHT)
        STOCKITEM(wxID_HELP,                 wxACCEL_NORMAL, WXK_HELP)
        STOCKITEM(wxID_REPLACE,              wxACCEL_CTRL,'H')
        STOCKITEM(wxID_EXIT,                 wxACCEL_CTRL, 'Q')
        STOCKITEM(wxID_CLOSE,                wxACCEL_CTRL, 'W')
        STOCKITEM(wxID_CLEAR,                wxACCEL_NORMAL, WXK_DELETE)
        STOCKITEM(wxID_ABOUT,                wxACCEL_SHIFT, WXK_HELP)
        default:
            ret = wxGetStockAccelerator(id);
            break;
    }

    #undef STOCKITEM


#if (wxVERSION_NUMBER >= 2902)
    wxASSERT(ret.IsOk());
#else
    // trac.wxwidgets.org/ticket/12444
    // trac.wxwidgets.org/ticket/12445
#endif
    return ret;
}

#endif // wxUSE_ACCEL

void wxSetAcceleratorTable(wxWindow* wnd, const AcceleratorArray& array)
{
   size_t count = array.GetCount();
   wxAcceleratorEntry* temp = new wxAcceleratorEntry[count];

   for (size_t i = 0; i < count; i++)
   {
      temp[i] = array.Item(i);
   }
   wxAcceleratorTable accel((int)count, temp);
   wnd->SetAcceleratorTable(accel);
   wxDELETEA(temp);
}

wxString wxMenuItem_GetText(const wxMenuItem* item)
{
   wxString str = item->GetItemLabel();
#ifdef __WXGTK__
   str.Replace(wxString('_'), wxString('&'));
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

static wxString wxGetAccelText(const wxAcceleratorEntry& accel)
{
   return wxGetAccelText(accel.GetFlags(), (enum wxKeyCode)accel.GetKeyCode());
}

void wxMenu_SetAccelText(wxMenuBar* menu, const AcceleratorArray& array)
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

wxString wxToolBarTool_MakeShortHelp(const wxString& rstr, const AcceleratorArray& accel, int id)
{
   wxString str = rstr;
   if (accel.GetCount() && str.Length())
   {
      wxString strAccel;

      for (size_t i = 0; i < accel.GetCount(); i++)
      {
         const wxAcceleratorEntry& element = accel.Item(i);
         if (element.GetCommand() == id)
         {
            if (strAccel.Length()) strAccel+=wxT(ACCELSTR_SEP);
            strAccel+=wxGetAccelText(element);
         }
      }
      if (strAccel.Length())
      {
         str+=wxString::Format(wxT(" (%s)"), strAccel.wx_str());
      }
   }
   return str;
}

wxString wxGetStockLabelEx(wxWindowID id, long flags)
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
   if (stockLabel.Length())
   {
       if ( !(flags & wxSTOCK_WITH_MNEMONIC) )
       {
           stockLabel = wxStripMenuCodes(stockLabel);
       }
#if (wxVERSION_NUMBER >= 2901)
       if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
       {
           wxString baseLabel;
           if ( stockLabel.EndsWith(wxT("..."), &baseLabel) )
               stockLabel = baseLabel;
       }
#else 
       // handled below
#endif
   }
   else
   {
      stockLabel = wxGetStockLabel(id, flags);
   }
#if (wxVERSION_NUMBER < 2901)
   if (flags & wxSTOCK_WITHOUT_ELLIPSIS)
   {
      wxString baseLabel;
      if ( stockLabel.EndsWith(wxT("..."), &baseLabel) )
         stockLabel = baseLabel;
   }
#endif
   return stockLabel;
}

/////////////////////////////////////////////////////////////////////////////
// wxRecentFileList

wxRecentFileList::wxRecentFileList(wxFileHistory* fileHistory) : m_fileHistory(fileHistory)
{
}

wxMenuItem* wxRecentFileList::GetSubMenu(wxMenuBar* menubar, wxMenu** submenu) const
{
   wxMenuItem* item = menubar->FindItem(m_fileHistory->GetBaseId(), submenu);
   wxASSERT(item);
   return item;
}

void wxRecentFileList::Attach(wxMenuBar* menubar)
{
   wxMenu* submenu;
   wxMenuItem* item = GetSubMenu(menubar, &submenu);
   submenu->Delete(item);
   m_fileHistory->UseMenu(submenu);
   m_fileHistory->AddFilesToMenu(submenu);
}

bool wxRecentFileList::Detach(wxMenuBar* menubar)
{
   wxMenu* submenu;
   bool ok = (NULL != GetSubMenu(menubar, &submenu));
   if (ok)
   {
      m_fileHistory->RemoveMenu(submenu);
   }
   return ok;
}

void wxRecentFileList::Attach(wxFrame* frame)
{
   Attach(frame->GetMenuBar());
}

bool wxRecentFileList::Detach(wxFrame* frame)
{
   return Detach(frame->GetMenuBar());
}

void wxRecentFileList::Load(wxConfigBase* config)
{
   if (NULL == config) config = wxConfigBase::Get();
   config->SetPath(wxT("MRU"));
   m_fileHistory->Load(*config);
   config->SetPath(wxT("/"));
}

void wxRecentFileList::Save(wxConfigBase* config)
{
   if (NULL == config) config = wxConfigBase::Get();
   config->SetPath(wxT("MRU"));
   m_fileHistory->Save(*config);
   config->SetPath(wxT("/"));
}

wxFileHistory* wxRecentFileList::GetImplementation() const
{
   return m_fileHistory;
}

bool wxRecentFileList::GetFile(size_t index, wxFileName* str) const
{
   const wxFileHistory* impl = GetImplementation();
   bool ok = (index < impl->GetCount());
   if (ok)
   {
      str->operator=(impl->GetHistoryFile(index));
   }
   return ok;
}

wxView* wxDocument_GetCurrentView(const wxDocument* doc)
{
   wxView* view = doc->GetDocumentManager()->GetCurrentView();

   return (view && (view->GetDocument() == doc)) ? view : doc->GetFirstView();
}

void wxDocument_Info(const wxDocument* doc, wxArrayString* as)
{
   const wxString fmt = wxT("%s:\t%s");
   wxView* active_view = wxDocument_GetCurrentView(doc);
   wxView* view  = active_view ? active_view : doc->GetFirstView();
   wxWindow* frame = view ? view->GetFrame() : NULL;;

   as->Add(wxString::Format(fmt, wxT("Doc class"), doc->GetClassInfo()->GetClassName()));
   if (view)
   {
      as->Add(wxString::Format(fmt, wxT("View class"), view->GetClassInfo()->GetClassName()));
   }
   as->Add(wxString::Format(fmt, wxT("Frame class"), frame ? frame->GetClassInfo()->GetClassName() : wxEmptyString));
   if (frame)
   {
      as->Add(wxString::Format(fmt, wxT("Frame label"), frame->GetLabel().wx_str()));
   }
   as->Add(wxString::Format(fmt, wxT("GetFilename"), doc->GetFilename().wx_str()));
   as->Add(wxString::Format(fmt, wxT("GetTitle"), doc->GetTitle().wx_str()));
   as->Add(wxString::Format(fmt, wxT("GetUserReadableName"), doc->GetUserReadableName().wx_str()));
   as->Add(wxString::Format(fmt, wxT("IsModified"), doc->IsModified() ? _("Yes") : _("No")));
   as->Add(wxString::Format(fmt, wxT("GetDocumentSaved"), doc->GetDocumentSaved() ? _("Yes") : _("No")));
   as->Add(wxString::Format(fmt, wxT("GetDocumentName"), doc->GetDocumentName().wx_str()));
}

void wxFrame_SetInitialPosition(wxFrame* wnd, const wxPoint& pos, const wxSize& size, int margin_pct)
{
   if (size == wxDefaultSize)
   {
      wxRect rect = wxGetClientDisplayRect();
      wxSize size(
         (rect.width  * (100 - margin_pct*2))/100, 
         (rect.height * (100 - margin_pct*2))/100);
         
      wnd->SetSize(size);
   }
   if (pos == wxDefaultPosition)
   {
      wnd->Center();
   }
}

void wxJoin(wxArrayString* dst, const wxArrayString& src)
{
   for (wxArrayString::const_iterator it = src.begin();
        it != src.end();
        it++)
   {
      dst->Add(*it);
   }
}

#define HASBIT(value, bit)      (((value) & (bit)) != 0)

/*static*/ bool wxClipboardHelper::Set(wxDataObject* def, wxDataObject* primary)
{
#if wxUSE_DATAOBJ && wxUSE_CLIPBOARD
    wxClipboard* clipboard = wxTheClipboard;
    bool was_open = clipboard->IsOpened();
    bool ok = was_open || clipboard->Open();
    
    if (ok)
    {
        if (def)
        {
            clipboard->UsePrimarySelection(false);
            ok = clipboard->SetData(def);
            if (ok)
            {
                def = NULL;
            }
        }
    #ifndef __WXMSW__
        if (primary)
        {
            clipboard->UsePrimarySelection(true);
            ok = clipboard->SetData(primary);
            clipboard->UsePrimarySelection(false);
            if (ok)
            {
                primary = NULL;
            }
        }
    #endif // __WXMSW__
        if (!was_open)
        {
            clipboard->Close();
        }
        //clipboard->Flush(); // else emu and wxc is freezing
    }
    delete def;
    delete primary;
    return ok;
#else
    return false;
#endif
}

/*static*/ bool wxClipboardHelper::SetText(const wxString& str, Type clip_type)
{
#if wxUSE_DATAOBJ && wxUSE_CLIPBOARD
    return Set(HASBIT(clip_type, Default) ? new wxTextDataObject(str) : NULL, 
               HASBIT(clip_type, Primary) ? new wxTextDataObject(str) : NULL);
#else
    return false;
#endif
}

/////////////////////////////////////////////////////////////////////////////
