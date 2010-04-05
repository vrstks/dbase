// dbfutil.cpp
// Copyright (c) 2007-2010 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/datetime.h"
#include "wx/object.h"
#include "wx/filefn.h"
#include "wx/intl.h"
#include "wx/filename.h"

#include "dbfutil.h"
#include "wxext.h"

#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "../../bool.h"
#include "../../dbf.h"
#include "../../dbf.hpp"
#include "../../dbf.inl"
#include "../../dbf_wx.h"
#include "../../dbf_wx.inl"

#define C_ASSERT_(n,e) typedef char __C_ASSERT__##n[(e)?1:-1]

wxString dbf_getstruct_c(const wxString& tablename, wxDBase* db)
{
   const wxString title = tablename;
   wxString strStruct;
   wxString strEnum;
/*
   wxString title;
   wxFileName::SplitPath(db->GetFilename(), NULL, &title, NULL);
*/
   wxString fieldname;
   fieldname.Printf(wxT("%s_fields"), title.wx_str());

   strStruct.Printf(wxT("const DBF_FIELD_INFO %s[] =\n{\n"), fieldname.wx_str());
   strEnum  .Printf(wxT("enum dbf_%s\n{\n"), title.wx_str());

   for (size_t i = 0; ; i++)
   {
      static const wxChar* const asz[] =
      {
         wxT("DBF_DATA_TYPE_CHAR"),
         wxT("DBF_DATA_TYPE_INTEGER"),
         wxT("DBF_DATA_TYPE_FLOAT"),
         wxT("DBF_DATA_TYPE_DATE"),
         wxT("DBF_DATA_TYPE_TIME"),
         wxT("DBF_DATA_TYPE_DATETIME"),
         wxT("DBF_DATA_TYPE_MEMO"),
         wxT("DBF_DATA_TYPE_BOOLEAN")
      };
      C_ASSERT_(1,WXSIZEOF(asz) == DBF_DATA_TYPE_ENUMCOUNT);

      DBF_FIELD_INFO info;
      if (!db->GetFieldInfo(i, &info)) break;
      wxString temp;
      wxString name = wxConvertMB2WX(info.name);
      temp.Printf(wxT("   { %s, %-21s,%3d,%3d },\n"),
         //-(int)(WXSIZEOF(info.name)+1),
         wxString::Format(wxT("\"%s\""), name.wx_str()).wx_str(),
         asz[info.type],
         info.length,
         info.decimals
         );
      strStruct+=temp;
      name.MakeLower();
      temp.Printf(wxT("   ENUM_dbf_%s_%s,\n"), title.wx_str(), name.wx_str());
      strEnum+=temp;
   }
   strStruct+= wxT("};\n");
   strEnum  += wxString::Format(wxT("   ENUM_dbf_%s_enumcount\n};\n"), title.wx_str());
   wxString str;
   str+=strEnum;
   str+=wxT("\n");
   str+=strStruct;
   str+=wxString::Format(wxT("C_ASSERT(ENUM_dbf_%s_enumcount == WXSIZEOF(%s));\n"), title.wx_str(), fieldname.wx_str());
   return str;
}

size_t dbf_getproperties(wxDBase* db, wxArrayString* as_ptr, bool header)
{
   wxArrayString as;
   wxString temp;

   if (header)
   {
      DBF_INFO info;
      wxDateTime dt;
      db->GetInfo(&info, &dt);

      wxString name = wxConvertMB2WX(info.tablename);

      temp.Printf(_("Table:\t%s"), name.wx_str());  as.Add(temp);
      temp.Printf(_("Version:\t%X"), info.version); as.Add(temp);
      temp.Printf(_("Memo file:\t%s"), info.memo ? _("Yes") : _("No")); as.Add(temp);
      temp.Printf(_("Record count:\t%d"), info.recordcount); as.Add(temp);
      temp.Printf(_("Modified:\t%s"), info.modified ? _("Yes") : _("No")); as.Add(temp);
      if (dt.IsValid())
      {
         temp.Printf(_("Last changed:\t%s"), dt.Format(wxT("%x")).wx_str()); as.Add(temp);
      }
      temp.Printf(_("Field count:\t%d"), info.fieldcount); as.Add(temp);
   }
   for (size_t i = 0; ; i++)
   {
      static const wxChar* const asz[] =
      {
         wxT("Char"),
         wxT("Number"),
         wxT("Float"),
         wxT("Date"),
         wxT("Time"),
         wxT("DateTime"),
         wxT("Memo"),
         wxT("Logical")
      };
      C_ASSERT_(1,WXSIZEOF(asz) == DBF_DATA_TYPE_ENUMCOUNT);

      DBF_FIELD_INFO info;
      if (!db->GetFieldInfo(i, &info)) break;

      wxString name = wxConvertMB2WX(info.name);
      temp.Printf(wxT("%d\t%s:\t%s\t%s"),
         (int)i,
         name.wx_str(),
         asz[info.type],
         wxString::Format(info.decimals ? wxT("%d:%d") : wxT("%d"),
            info.length,
            info.decimals).wx_str()

         );
      as.Add(temp);
   }
   if (as_ptr)
   {
      wxJoin(as_ptr, as);
   }
   return as.GetCount();
}

