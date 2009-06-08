// dbfutil.cpp
// Copyright (c) 2007-2008 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/string.h>
#include <wx/datetime.h>
#include <wx/object.h>
#include <wx/filefn.h>
#include <wx/intl.h>
#include "dbfutil.h"

#include <zlib.h>
#include "../../../ioapi/unzip/ioapi.h"
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
   fieldname.Printf(wxT("%s_fields"), title.c_str());

   strStruct.Printf(wxT("const DBF_FIELD_INFO %s[] =\n{\n"), fieldname.c_str());
   strEnum  .Printf(wxT("enum dbf_%s\n{\n"), title.c_str());

   for (size_t i = 0; ; i++)
   {
      static const wxChar* const asz[] =
      {
         wxT("DBF_DATA_TYPE_CHAR"),
         wxT("DBF_DATA_TYPE_INTEGER"),
         wxT("DBF_DATA_TYPE_FLOAT"),
         wxT("DBF_DATA_TYPE_DATE"),
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
         wxString::Format(wxT("\"%s\""), name.c_str()).c_str(),
         asz[info.type],
         info.length,
         info.decimals
         );
      strStruct+=temp;
      name.MakeLower();
      temp.Printf(wxT("   ENUM_dbf_%s_%s,\n"), title.c_str(), name.c_str());
      strEnum+=temp;
   }
   strStruct+= wxT("};\n");
   strEnum  += wxString::Format(wxT("   ENUM_dbf_%s_enumcount\n};\n"), title.c_str());
   wxString str;
   str+=strEnum;
   str+=wxT("\n");
   str+=strStruct;
   str+=wxString::Format(wxT("C_ASSERT(ENUM_dbf_%s_enumcount == WXSIZEOF(%s));\n"), title.c_str(), fieldname.c_str());
   return str;
}

size_t dbf_getproperties(wxDBase* db, wxArrayString* as_ptr, bool header)
{
   wxArrayString as;
   wxString temp;

   if (header)
   {
      wxString dt;
      db->GetLastUpdate(&dt);

      //temp.Printf(wxT("File:\t%s"), db->GetFilename().c_str());
      //as.Add(temp);
      temp.Printf(_("Records:\t%d"), db->GetRecordCount());
      as.Add(temp);
      temp.Printf(_("Last changed:\t%s"), dt.c_str());
      as.Add(temp);
      as.Add(wxEmptyString);
      as.Add(_("Fields:"));
      as.Add(wxEmptyString);
   }
   for (size_t i = 0; ; i++)
   {
      static const wxChar* const asz[] =
      {
         wxT("Char"),
         wxT("Number"),
         wxT("Float"),
         wxT("Date"),
         wxT("Memo"),
         wxT("Logical")
      };
      C_ASSERT_(1,WXSIZEOF(asz) == DBF_DATA_TYPE_ENUMCOUNT);

      DBF_FIELD_INFO info;
      if (!db->GetFieldInfo(i, &info)) break;

      wxString name = wxConvertMB2WX(info.name);
      temp.Printf(wxT("%d\t%s:\t%s\t%s"),
         (int)i,
         name.c_str(),
         asz[info.type],
         wxString::Format(info.decimals ? wxT("%d:%d") : wxT("%d"),
            info.length,
            info.decimals).c_str()

         );
      as.Add(temp);
   }
   if (as_ptr) *as_ptr = as;
   return as.GetCount();
}

