// dbfutil.cpp
// Copyright (c) 2007-2012 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#include "dbfutil.h"
#include "wx/ext/wx.h"

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
   //wxString title = wxFileName(db->GetFilename()).GetName();
   wxString strStruct;
   wxString strEnum;
   wxString fieldname;

   fieldname.Printf(wxT("%s_fields"), title.wx_str());

   strStruct.Printf(wxT("const DBF_FIELD_INFO %s[] =\n{\n"), fieldname.wx_str());
   strEnum  .Printf(wxT("enum dbf_%s\n{\n"), title.wx_str());

   for (dbf_uint i = 0; ; i++)
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
      wxString temp;
      wxString name;
      
      if (!db->GetFieldInfo(i, &info))
      {
          break;
      }

      name = wxConvertMB2WX(info.name);
      temp.Printf(wxT("   { %s, %-21s,%3d,%3d },\n"),
         //-(int)(WXSIZEOF(info.name)+1),
         wxString::Format(wxT("\"%s\""), name.wx_str()).wx_str(),
         asz[info.type],
         info.length,
         info.decimals
         );
      strStruct+=temp;
      name.MakeLower();
      temp.Printf(wxT("   dbf_%s_%s,\n"), title.wx_str(), name.wx_str());
      strEnum+=temp;
   }
   strStruct+= wxT("};\n");
   strEnum  += wxString::Format(wxT("   dbf_%s_enumcount\n};\n"), title.wx_str());
   wxString str;
   str+=strEnum;
   str+=wxT("\n");
   str+=strStruct;
   str+=wxString::Format(wxT("C_ASSERT(dbf_%s_enumcount == WXSIZEOF(%s));\n"), title.wx_str(), fieldname.wx_str());
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
      wxString name;
      wxString format;
      
      db->GetInfo(&info, &dt);
      name = wxConvertMB2WX(info.tablename);
      format = wxConvertMB2WX(info.format);

      temp.Printf(_("Table name:\t%s"), name.wx_str());  as.push_back(temp);
      temp.Printf(_("Format:\t%s"), format.wx_str()); as.push_back(temp);
      temp.Printf(_("Memo file:\t%s"), info.memo ? _("Yes") : _("No")); as.push_back(temp);
      temp.Printf(_("Record count:\t%d"), info.recordcount); as.push_back(temp);
      temp.Printf(_("Modified:\t%s"), info.modified ? _("Yes") : _("No")); as.push_back(temp);
      if (dt.IsValid())
      {
         temp.Printf(_("Last changed:\t%s"), dt.Format(wxT("%x")).wx_str()); as.push_back(temp);
      }
      temp.Printf(_("Field count:\t%d"), info.fieldcount); as.push_back(temp);
   }
   for (dbf_uint i = 0; ; i++)
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
      wxString name;

      if (!db->GetFieldInfo(i, &info))
      {
          break;
      }

      name = wxConvertMB2WX(info.name);
      temp.Printf(wxT("%d\t%s:\t%s\t%s"),
         (int)i,
         name.wx_str(),
         asz[info.type],
         info.decimals 
            ? wxString::Format(wxT("%d:%d"), info.length, info.decimals).wx_str()
            : wxString::Format(wxT("%d"), info.length).wx_str()
         );
      as.push_back(temp);
   }
   if (as_ptr)
   {
      wxJoin(as_ptr, as);
   }
   return as.size();
}
