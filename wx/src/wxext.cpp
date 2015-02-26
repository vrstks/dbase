// wxext.cpp
// Copyright (c) 2007-2015 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include <wx/html/htmprint.h>

#include <algorithm>

#include "wx/ext/trunk.h"
#include "wx/ext/wx.h"

IMPLEMENT_CLASS(wxViewEx, wxView)
IMPLEMENT_CLASS(wxFileLoadedHint, wxObject)

//static
const std::string XmlWriter::EndTagLeftChars = "</";
//static
const std::string XmlWriter::DefaultEncoding = "utf8";
//static
const std::string XmlWriter::DefaultTabString = std::string(3, ' ');


//static
const std::string HtmlTextWriter::EndTagLeftChars = "</";
//static
const std::string HtmlTextWriter::DefaultEncoding = "utf8";
//static
const std::string HtmlTextWriter::DefaultTabString = std::string(3, ' ');

HtmlTextWriter::HtmlTextWriter() : TextWriter()
{
    m_encoding = DefaultEncoding, m_tabString = DefaultTabString;
    m_open = true;
}

//static
std::string HtmlTextWriter::Escape(const std::string& data)
{
    std::string buffer;
    buffer.reserve(data.size());
    for(std::string::const_iterator it = data.begin(); it != data.end(); it++)
    {
        switch(*it)
        {
            case '&':  buffer.append("&amp;");  break;
            case '\"': buffer.append("&quot;"); break;
            case '\'': buffer.append("&apos;"); break;
            case '<':  buffer.append("&lt;");   break;
            case '>':  buffer.append("&gt;");   break;
            case ' ':  buffer.append("&nbsp;"); break;
            default:   buffer.append(1, *it);   break;
        }
    }
    return buffer;
}

bool wxRemoveFile(wxFFile* file)
{
    bool ok = true;

    if (file->IsOpened())
    {
        wxString filename = file->GetName();

        file->Close();
        wxLogNull no_log;
        ok = wxRemoveFile(filename);
    }
    return ok;
}

bool wxWindow_Toggle(wxWindow* wnd)
{
   wxWindow* parent = wnd->GetParent();
   const bool show = !wnd->IsShown();

   wnd->Show(show);
   if (wxIS_KIND_OF(parent, wxFrame))
      wxStaticCast(parent, wxFrame)->SendSizeEvent(); // needed when mdi
   else
      parent->Layout(); // needed when non-mdi
   return show;
}

void wxFrame_ClonePosition(wxFrame* wnd, wxWindow* otherwindow /*= NULL*/)
{
    otherwindow = otherwindow ? wxGetTopLevelParent(otherwindow) : wxTheApp->GetTopWindow();
    wxFrame* topframe = wxStaticCast(otherwindow, wxFrame);

    if (topframe->IsMaximized())
        wnd->Maximize();
    else if (topframe->IsFullScreen())
        wnd->Maximize();
    else
    {
        wxRect rect = topframe->GetScreenRect();

        wnd->SetSize(rect);
    }
}

void wxFrame_ToggleFullScreen(wxFrame* wnd, long style)
{
   wnd->ShowFullScreen(!wnd->IsFullScreen(), style);
}

void wxFrame_OnUpdateFullScreen(wxFrame* wnd, wxUpdateUIEvent& event)
{
   event.Check(wnd->IsFullScreen());
}

//static
const wxString wxXmlResourceHelper::FileExt = wxT("xrc");
//static
const wxString wxXmlResourceHelper::FileExtCompressed = wxT("xrs");
//static
const wxString wxXmlResourceHelper::DefaultFolder = wxT("xmlres");

//static
void wxXmlResourceHelper::Init()
{
   wxXmlResource::Get()->InitAllHandlers();
}

bool wxCreateFileFromMemory(const void* buf, size_t buf_len, const wxFileName& filename, wxFFile* file_ptr)
{
    wxFFile file;
    bool ok = filename.FileExists();

    if (ok)
    {
         // file exists
        ok = file.Open(filename.GetFullPath(), wxT("rb"));
        if (ok)
        {
            ok = (wxFileOffset(buf_len) == file.Length());
            if (!ok)
            {
                // exists but wrong length
                file.Close();
                wxRemoveFile(filename.GetFullPath());
            }
        }
    }
    if (!ok)
    {
        // create file
        ok =  file.Open(filename.GetFullPath(), wxT("wb"))
           && (buf_len == file.Write(buf, buf_len))
           && file.Flush();
    }

    if (ok && file_ptr)
    {
        file_ptr->Attach(file.fp(), file.GetName());
        file.Detach();
    }
    return ok;
}

//static
bool wxXmlResourceHelper::LoadFromMemory(const void* buf, size_t buf_len, const wxString& fullname, wxFFile* file_ptr)
{
    wxFileName filename(wxFileName::GetTempDir(), fullname);
    bool ok;

    filename.AppendDir(filename.GetName()); // subfolder named after file title
    ok = filename.DirExists() || filename.Mkdir();
    if (ok)
        ok = wxCreateFileFromMemory(buf, buf_len, filename, file_ptr);
    if (ok)
        ok = LoadFromFile(filename);
    return ok;
}

//static
bool wxXmlResourceHelper::LoadFromFile(const wxFileName& filename)
{
   return wxXmlResource::Get()->Load(filename.GetFullPath());
}

//static
bool wxXmlResourceHelper::LoadFromFile(const char* srcmodule, const wxString& fullname)
{
    wxFileName filename(wxString::FromAscii(srcmodule));

    filename.RemoveLastDir();
    filename.AppendDir(DefaultFolder);
    filename.SetFullName(fullname);
    return LoadFromFile(filename);
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
      if (!str.empty()) str+=sep;
      str+=_("Ctrl");
   }
   if (flags & wxACCEL_ALT)
   {
      if (!str.empty()) str+=sep;
      str+=_("Alt");
   }
   if (flags & wxACCEL_SHIFT)
   {
      if (!str.empty()) str+=sep;
      str+=_("Shift");
   }
   if (!str.empty()) str+=sep;
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

#if wxUSE_ACCEL

/*static*/
wxAcceleratorEntry wxAcceleratorHelper::GetStockAccelerator(wxWindowID id)
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

wxString wxMenuItem_GetText(const wxMenuItem* item)
{
   wxString str = item->GetItemLabel();
#ifdef __WXGTK__
   str.Replace(wxString('_'), wxString('&'));
#endif
   return str;
}

#define ACCELSTR_SEP "   "

/*static*/
bool wxAcceleratorHelper::SetAccelText(wxMenuItem* item, const wxString& accel, bool append)
{
   wxString str = wxMenuItem_GetText(item);
   wxString ch_sep = wxT("\t");
   const int sep = str.Find(ch_sep);

   if (wxNOT_FOUND == sep)
   {
   }
   else if (append)
      ch_sep = wxT(ACCELSTR_SEP);
   else
      str.Truncate(sep);
   item->SetItemLabel(wxString::Format(wxT("%s%s%s"),
      str.wx_str(),
      ch_sep.wx_str(),
      accel.wx_str()));
   return true;
}

static wxString wxGetAccelText(const wxAcceleratorEntry& accel)
{
   return wxGetAccelText(accel.GetFlags(), accel.GetKeyCode());
}

/*static*/
void wxAcceleratorHelper::SetAccelText(wxMenuBar* menu, const wxAcceleratorVector& array)
{
    for (wxAcceleratorVector::const_iterator it = array.begin(); it != array.end(); it++)
    {
        const wxAcceleratorEntry& entry = *it;
        wxMenuItem* item = menu->FindItem(entry.GetCommand());

        if (item)
            wxAcceleratorHelper::SetAccelText(item, wxGetAccelText(entry), true);
    }
}

wxString wxToolBarTool_MakeShortHelp(const wxString& rstr, const wxAcceleratorVector& accel, int id)
{
   wxString str = rstr;

   if ((!accel.empty()) && (!str.empty()))
   {
      wxString strAccel;

      for (wxAcceleratorVector::const_iterator it = accel.begin(); it != accel.end(); it++)
      {
         const wxAcceleratorEntry& element = *it;

         if (element.GetCommand() == id)
         {
            if (!strAccel.empty())
                strAccel+=wxT(ACCELSTR_SEP);
            strAccel+=wxGetAccelText(element);
         }
      }
      if (!strAccel.empty())
         str+=wxString::Format(wxT(" (%s)"), strAccel.wx_str());
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
   if (!stockLabel.empty())
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

wxRecentFileList::wxRecentFileList(size_t maxFiles, wxWindowID idBase)
{
    m_fileHistory = m_DeleteMe = new wxFileHistory(maxFiles, idBase);
}

wxRecentFileList::wxRecentFileList(wxFileHistory* fileHistory) : m_fileHistory(fileHistory), m_DeleteMe(NULL)
{
}

wxRecentFileList::~wxRecentFileList()
{
    delete m_DeleteMe;
}

void wxRecentFileList::Attach(wxMenuBar* menubar)
{
    wxMenu* submenu;
    wxMenuItem* item = menubar->FindItem(m_fileHistory->GetBaseId(), &submenu);

    submenu->Delete(item);
    m_fileHistory->UseMenu(submenu);
    m_fileHistory->AddFilesToMenu(submenu);
}

bool wxRecentFileList::Detach(wxMenuBar* menubar)
{
    const wxList& list = m_fileHistory->GetMenus();

    for (wxList::const_iterator it = list.begin();
         it != list.end();
         it++)
    {
        wxMenu* menu = wxStaticCast(*it, wxMenu);

        if (menu->GetMenuBar() == menubar)
        {
            m_fileHistory->RemoveMenu(menu);
            return true;
        }
    }
    wxASSERT(false);
    return false;
}

void wxRecentFileList::Attach(wxFrame* frame)
{
    Attach(frame->GetMenuBar());
}

bool wxRecentFileList::Detach(wxFrame* frame)
{
    return Detach(frame->GetMenuBar());
}

void wxRecentFileList::Load(wxConfigBase* config, const wxString& configPath)
{
    if (NULL == config)
        config = wxConfigBase::Get();
    config->SetPath(configPath.empty() ? wxT("MRU") : configPath);
    m_fileHistory->Load(*config);
    config->SetPath(wxT("/"));
}

void wxRecentFileList::Save(wxConfigBase* config, const wxString& configPath)
{
    if (NULL == config)
        config = wxConfigBase::Get();
    config->SetPath(configPath.empty() ? wxT("MRU") : configPath);
    m_fileHistory->Save(*config);
    config->SetPath(wxT("/"));
}

int wxRecentFileList::IndexFromID(wxWindowID id) const
{
    const wxFileHistory* impl = GetImplementation();

    return (   (id >= impl->GetBaseId())
            && (id < (impl->GetBaseId() + impl->GetMaxFiles()))
           )
            ? id - impl->GetBaseId()
            : wxNOT_FOUND;
}

void wxRecentFileList::AddFileToHistory(const wxString& file)
{
    GetImplementation()->AddFileToHistory(file);
}

bool wxRecentFileList::GetFile(size_t index, wxString* str) const
{
    const wxFileHistory* impl = GetImplementation();
    bool ok = (index < impl->GetCount());

    if (ok && str)
        *str = impl->GetHistoryFile(index);
    return ok;
}

#if (wxVERSION_NUMBER < 2905)
// static
wxDocVector wxTrunkDocView::GetDocumentsVector(const wxDocManager& docManager)
{
    wxDocVector docs;
    const wxList& list = wxConstCast(&docManager, wxDocManager)->GetDocuments();

    for (wxList::const_iterator it = list.begin(); it != list.end(); it++)
        docs.push_back(wxStaticCast(*it, wxDocument));
    return docs;
}
#endif

// static
wxView* wxDocViewHelpers::GetCurrentView(const wxDocument& rdoc)
{
    const wxDocument* doc = &rdoc;
    wxView* view = doc->GetDocumentManager()->GetCurrentView();

    return (view && (view->GetDocument() == doc)) ? view : doc->GetFirstView();
}

// static
void wxDocViewHelpers::GetInfo(const wxDocument& doc, wxArrayString* as)
{
   const wxString fmt = wxT("%s:\t%s");
   wxView* active_view = wxDocViewHelpers::GetCurrentView(doc);
   wxView* view  = active_view ? active_view : doc.GetFirstView();
   wxWindow* frame = view ? view->GetFrame() : NULL;;

   as->push_back(wxString::Format(fmt, wxT("Doc class"), doc.GetClassInfo()->GetClassName()));
   if (view)
      as->push_back(wxString::Format(fmt, wxT("View class"), view->GetClassInfo()->GetClassName()));
   as->push_back(wxString::Format(fmt, wxT("Frame class"), frame ? frame->GetClassInfo()->GetClassName() : wxEmptyString));
   if (frame)
      as->push_back(wxString::Format(fmt, wxT("Frame label"), frame->GetLabel().wx_str()));
   as->push_back(wxString::Format(fmt, wxT("GetFilename"), doc.GetFilename().wx_str()));
   as->push_back(wxString::Format(fmt, wxT("GetTitle"), doc.GetTitle().wx_str()));
   as->push_back(wxString::Format(fmt, wxT("GetUserReadableName"), doc.GetUserReadableName().wx_str()));
   as->push_back(wxString::Format(fmt, wxT("IsModified"), doc.IsModified() ? _("Yes") : _("No")));
   as->push_back(wxString::Format(fmt, wxT("GetDocumentSaved"), doc.GetDocumentSaved() ? _("Yes") : _("No")));
   as->push_back(wxString::Format(fmt, wxT("GetDocumentName"), doc.GetDocumentName().wx_str()));
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
      wnd->Center();
}

void wxJoin(wxArrayString* dst, const wxArrayString& src)
{
   for (wxArrayString::const_iterator it = src.begin();
        it != src.end();
        it++)
   {
      dst->push_back(*it);
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
                def = NULL;
        }
    #ifndef __WXMSW__
        if (primary)
        {
            clipboard->UsePrimarySelection(true);
            ok = clipboard->SetData(primary);
            clipboard->UsePrimarySelection(false);
            if (ok)
                primary = NULL;
        }
    #endif // __WXMSW__
        if (!was_open)
            clipboard->Close();
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
// MDIWindowMenuEvtHandler

MDIWindowMenuEvtHandler::MDIWindowMenuEvtHandler(wxMDIParentFrame* wnd) : wxEvtHandler(), m_target_wnd(NULL)
{
    wxMenu* windowMenu = wnd->GetWindowMenu();

    if (windowMenu)
    {
        windowMenu->AppendSeparator();
        windowMenu->Append(wxID_CLOSE, _("Cl&ose"), _("Close window"));
        windowMenu->Append(wxID_CLOSE_ALL, _("Close A&ll"), _("Close all open windows"));
        wnd->PushEventHandler(this);
        m_target_wnd = wnd;
    }
}

MDIWindowMenuEvtHandler::~MDIWindowMenuEvtHandler()
{
    if (m_target_wnd)
        m_target_wnd->RemoveEventHandler(this);
}

BEGIN_EVENT_TABLE(MDIWindowMenuEvtHandler, wxEvtHandler)
    EVT_MENU(wxID_CLOSE_ALL, MDIWindowMenuEvtHandler::OnCloseAll)
    EVT_UPDATE_UI(wxID_CLOSE, MDIWindowMenuEvtHandler::OnUpdateNeedWindow)
    EVT_UPDATE_UI(wxID_CLOSE_ALL, MDIWindowMenuEvtHandler::OnUpdateNeedWindow)
END_EVENT_TABLE()

void MDIWindowMenuEvtHandler::OnUpdateNeedWindow(wxUpdateUIEvent& event)
{
    event.Enable(m_target_wnd->GetActiveChild() != NULL);
}

void MDIWindowMenuEvtHandler::OnCloseAll(wxCommandEvent&)
{
    const wxWindowList list = m_target_wnd->GetChildren(); // make a copy of the window list

    for (wxWindowList::const_iterator i = list.begin();
         i != list.end();
         i++)
    {
        if (wxDynamicCast(*i, wxMDIChildFrame))
        {
            if (!(*i)->Close())
            {
                // Close was vetoed
                break;
            }
        }
    }
}

wxStdDialogButtonSizer* wxCreateStdDialogButtonSizer(wxWindow* parent, long flags)
{
    wxStdDialogButtonSizer* buttonpane = new wxStdDialogButtonSizer();

    if ((flags & wxOK) && (flags & wxCANCEL))
    {
        buttonpane->AddButton(new wxButton(parent, wxID_OK));
        buttonpane->AddButton(new wxButton(parent, wxID_CANCEL));
        buttonpane->GetAffirmativeButton()->SetDefault();
    }
    else if (flags & wxOK)
    {
        buttonpane->AddButton(new wxButton(parent, wxID_OK));
        buttonpane->GetAffirmativeButton()->SetDefault();
    }
    else if (flags & wxCANCEL)
    {
        buttonpane->AddButton(new wxButton(parent, wxID_CANCEL, _("Cl&ose")));
        buttonpane->GetCancelButton()->SetDefault();
    }
    if (flags & wxAPPLY)
        buttonpane->AddButton(new wxButton(parent, wxID_APPLY, _("&Apply")));
    buttonpane->Realize();

    //parent->GetSizer()->Add(new wxStaticLine(parent), 0, wxEXPAND | wxALL, 5); // separator
    parent->GetSizer()->Add(buttonpane, 0, wxEXPAND | wxLEFT | wxBOTTOM, 5);

    return buttonpane;
}

#if (wxVERSION_NUMBER < 2900)
static unsigned char wxColourBase_AlphaBlend(unsigned char fg, unsigned char bg, double alpha)
{
    double result = bg + (alpha * (fg - bg));
    result = wxMax(result,   0.0);
    result = wxMin(result, 255.0);
    return (unsigned char)result;
}

static void wxColourBase_ChangeLightness(unsigned char* r, unsigned char* g, unsigned char* b, int ialpha)
{
    if (ialpha == 100) return;

    // ialpha is 0..200 where 0 is completely black
    // and 200 is completely white and 100 is the same
    // convert that to normal alpha 0.0 - 1.0
    ialpha = wxMax(ialpha,   0);
    ialpha = wxMin(ialpha, 200);
    double alpha = ((double)(ialpha - 100.0))/100.0;

    unsigned char bg;
    if (ialpha > 100)
    {
        // blend with white
        bg = 255;
        alpha = 1.0 - alpha;  // 0 = transparent fg; 1 = opaque fg
    }
    else
    {
        // blend with black
        bg = 0;
        alpha = 1.0 + alpha;  // 0 = transparent fg; 1 = opaque fg
    }

    *r = wxColourBase_AlphaBlend(*r, bg, alpha);
    *g = wxColourBase_AlphaBlend(*g, bg, alpha);
    *b = wxColourBase_AlphaBlend(*b, bg, alpha);
}

static wxColour wxColourBase_ChangeLightness(const wxColour& colour, int ialpha)
{
    wxByte r = colour.Red();
    wxByte g = colour.Green();
    wxByte b = colour.Blue();
    wxColourBase_ChangeLightness(&r, &g, &b, ialpha);
    return wxColour(r,g,b);
}
#endif

IMPLEMENT_DYNAMIC_CLASS(wxTrunkListView, wxListView)

#if (wxVERSION_NUMBER < 2905)
void wxTrunkListView::EnableAlternateRowColours(bool enable)
{
    if (enable)
    {
        // Determine the alternate rows colour automatically from the
        // background colour.
        const wxColour bgColour = GetBackgroundColour();

        // Depending on the background, alternate row color
        // will be 3% more dark or 50% brighter.
    #if (wxVERSION_NUMBER >= 2900)
        int alpha = (bgColour.GetRGB() > 0x808080) ? 97 : 150;
        m_alternateRowColour.SetBackgroundColour(bgColour.ChangeLightness(alpha));
    #else
        int alpha = ((bgColour.Red() | (bgColour.Green() << 8) | (bgColour.Blue() << 16)) > 0x808080) ? 97 : 150;
        m_alternateRowColour.SetBackgroundColour(wxColourBase_ChangeLightness(bgColour, alpha));
    #endif
    }
    else
    {
        m_alternateRowColour.SetBackgroundColour(wxColour());
    }
}

wxListItemAttr* wxTrunkListView::OnGetItemAttr(long row) const
{
    return (m_alternateRowColour.GetBackgroundColour().IsOk() && (row % 2))
        ? wxConstCast(&m_alternateRowColour, wxListItemAttr)
        : base::OnGetItemAttr(row);
}
#endif

#if (wxVERSION_NUMBER < 2900)
bool wxTrunkListView::GetSubItemRect( long row, long col, wxRect& rect, int code) const
{
    bool ok = base::GetItemRect(row, rect, code);

    if (ok)
    {
      for (int i = 0/*, count = ctrl.GetColumnCount()*/; ; i++)
      {
         const int width = GetColumnWidth(i);

         if (i < col)
            rect.x+=width;
         else if (i == col)
         {
            rect.width = width;
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
#endif

long wxTrunkListView::HitTest(const wxPoint& point, int* flags, long* col) const
{
    int rFlags;
    const long row = base::HitTest(point, rFlags);

    if (col && (row != wxNOT_FOUND))
    {
      const int offset = GetScrollPos(wxHORIZONTAL);
      int pos = 0;

      *col = -1;
      for (int i = 0, count = GetColumnCount(); i < count; i++)
      {
         pos += GetColumnWidth(i);
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

bool wxTrunkListView::EndEditLabel(bool cancel)
{
#ifdef __WXMSW__
    #if (wxVERSION_NUMBER >= 2901)
        return base::EndEditLabel(cancel);
    #else
      HWND hwnd = ListView_GetEditControl((HWND)GetHWND());
      bool ok = (hwnd != NULL);
      if (ok)
      {
      #ifdef ListView_CancelEditLabel
         if (cancel && (wxApp::GetComCtl32Version() >= 600))
         {
            ListView_CancelEditLabel((HWND)GetHWND());
            return ok;
         }
      #endif
         if (::IsWindowVisible(hwnd))
            ::SendMessage(hwnd, WM_KEYDOWN, cancel ? VK_ESCAPE : VK_RETURN, 0);
         else
            ::SendMessage(hwnd, WM_CLOSE, 0, 0);
      }
      return ok;
    #endif
#else
    return false;
#endif
}

void wxModalTextDialog(wxWindow* parent, const wxString& text, const wxString& caption)
{
    wxTextEntryDialog dialog(parent, wxEmptyString, caption.empty() ? wxGetTextFromUserPromptStr : caption, text, wxOK | wxTE_MULTILINE);
    dialog.ShowModal();
}

void HtmlTextParagraph::Render(HtmlTextWriter* writer_ptr) const
{
    HtmlTextWriter& writer = *writer_ptr;
    writer.WriteFullBeginTag("p");
    if (Weight == HtmlTextWeight_Bold)
        writer.WriteFullBeginTag("b");
    writer.Write(Text);
    if (Weight == HtmlTextWeight_Bold)
        writer.WriteEndTag("b");
    writer.WriteEndTag("p");
    writer.WriteBreak();
    writer.WriteLine(); 
}

void HtmlTable::Render(HtmlTextWriter* writer_ptr) const
{
    char str[80];
    HtmlTextWriter& writer = *writer_ptr;

    writer.WriteBeginTag("table");
        _snprintf(str, WXSIZEOF(str), "%d", Border);
        writer.WriteAttribute("border", str);
        writer.Write(wxHtmlTextWriter::TagRightChar); writer.WriteLine(); writer.IncreaseIndent();
        writer.WriteFullBeginTag("tbody"); writer.WriteLine(); writer.IncreaseIndent();

            for (HtmlTableVector::const_iterator it_row = List.begin(); it_row != List.end(); it_row++)
            {
                writer.WriteFullBeginTag("tr"); writer.WriteLine(); writer.IncreaseIndent();
                size_t column = 0;
                for (std_string_vector::const_iterator it_col = it_row->ColumnTextArray.begin(); it_col != it_row->ColumnTextArray.end(); it_col++)
                {
                    writer.WriteBeginTag("td");

                    if ( (it_col == it_row->ColumnTextArray.begin()) && (PercentArray.size() > column))
                    {
                        _snprintf(str, WXSIZEOF(str), " width=\"%d%%\"", PercentArray[column]);
                        writer.Write(str);
                    }
                    if (it_row->ColumnAttribute.size() > column)
                        writer.Write(std::string(" ") + it_row->ColumnAttribute[column]);
                    writer.Write(wxHtmlTextWriter::TagRightChar);
                    writer.Write(*it_col);
                    writer.WriteEndTag("td"); writer.WriteLine();
                    column++;
                }
                writer.DecreaseIndent();
                writer.WriteEndTag("tr"); writer.WriteLine();
            }
        writer.DecreaseIndent();
        writer.WriteEndTag("tbody"); writer.WriteLine(); writer.DecreaseIndent();
    writer.WriteEndTag("table"); writer.WriteLine();
}

wxHtmlTableWriter::~wxHtmlTableWriter()
{
    for (HtmlItemVector::iterator it = List.begin(); it != List.end(); it++)
        delete *it;
}

void wxHtmlTableWriter::SaveFile(wxOutputStream* stream, std::string encoding)
{
    wxHtmlTextWriter writer(stream);
    writer.SetEncoding(encoding);
    
    writer.WriteFullBeginTag("html"); writer.WriteLine(); writer.IncreaseIndent();
        writer.WriteFullBeginTag("head"); writer.WriteLine(); writer.IncreaseIndent();
            writer.WriteBeginTag("meta"); 
                writer.WriteAttribute("content", std::string("text/html; charset=") + writer.GetEncoding());
                writer.Write(wxHtmlTextWriter::TagRightChar); writer.WriteLine();
                writer.WriteFullBeginTag("title");
                    writer.Write(Title); 
                writer.WriteEndTag("title"); writer.WriteLine();
                writer.DecreaseIndent();
        writer.WriteEndTag("head"); writer.WriteLine();
        writer.WriteFullBeginTag("body"); writer.WriteLine(); writer.IncreaseIndent();
            for (HtmlItemVector::const_iterator it = List.begin(); it != List.end(); it++)
                (*it)->Render(&writer);
        writer.DecreaseIndent();
        writer.WriteEndTag("body"); writer.WriteLine(); writer.DecreaseIndent();
    writer.WriteEndTag("html"); writer.WriteLine(); 
    writer.Close();
}

/////////////////////////////////////////////////////////////////////////////
