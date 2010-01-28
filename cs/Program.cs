// Program.cs
// Copyright (c) 2010 by Troels K. All rights reserved.
// Project website: sf.net/projects/dbase
// License: zlib (opensource.org/licenses/zlib-license.php)

using System;
using io = System.IO;

namespace DBase.Test
{
   static class Program
   {
      private static void ConsoleDump(DBase.File file)
      {
         int i;
         string str = string.Empty;

         for (i = 0; i < file.FieldCount; i++)
         {
            if (i != 0) str += ",";
            str += file.GetFieldInfo(i).Name;
         }
         Console.WriteLine(str);

         for (file.Position = 0; file.Position < file.RecordCount; file.Position++)
         {
            str = string.Empty;
            for (i = 0; i < file.FieldCount; i++)
            {
               if (i != 0) str += ",";
               str += file.GetField(i);
               //break;
            }
            Console.WriteLine("{0,5} {1}", i, str);
            //break;
         }
      }

      private static void Create(string filename)
      {
         var file = new DBase.File();

         DBF_FIELD_DATA[] fields = new DBF_FIELD_DATA[]
         { 
            new DBF_FIELD_DATA("TITLE", DataType.Char, 4, 0),
            new DBF_FIELD_DATA("INTEGER", DataType.Integer, 10, 0),
            new DBF_FIELD_DATA("BOOLEAN", DataType.Boolean, 1, 0),
            new DBF_FIELD_DATA("DATE", DataType.Date, 8, 0),
            new DBF_FIELD_DATA("FLOAT", DataType.Float, 10, 5),
            //new DBF_FIELD_DATA("MEMO", DataType.Memo, 10, 0),
         };

         if (file.Create(filename, fields))
         {
            file.AddRecord();
            file.PutField(0, "sjov");
            file.PutField(1, 10);
            file.PutField(2, true);
            file.PutField(3, DateTimeOffset.UtcNow);
            file.PutField(4, 11.1);
            //file.PutField(5, "memo");
            file.PutRecord();
            file.Close();
         }
      }

      private static void CreateMemo(string filename)
      {
         var file = new DBase.File();

         DBF_FIELD_DATA[] fields = new DBF_FIELD_DATA[]
         { 
            /*
            new DBF_FIELD_DATA("TITLE", DataType.Char, 4, 0),
            new DBF_FIELD_DATA("INTEGER", DataType.Integer, 10, 0),
            new DBF_FIELD_DATA("BOOLEAN", DataType.Boolean, 1, 0),
            new DBF_FIELD_DATA("DATE", DataType.Date, 8, 0),
            new DBF_FIELD_DATA("FLOAT", DataType.Float, 10, 5),
            */
            new DBF_FIELD_DATA("MEMO", DataType.Memo, 10, 0),
         };

         if (file.Create(filename, fields))
         {
            file.AddRecord();
            file.PutField(0, "sjov");
            /*file.PutField(1, 10);
            file.PutField(2, true);
            file.PutField(3, DateTimeOffset.UtcNow);
            file.PutField(4, 11.1);
            file.PutField(5, "memo");
            */
            file.PutRecord();

            file.Close();
         }
      }

      private static void Open(string filename)
      {
         var file = new DBase.File();
         if (file.Open(filename, io.FileMode.Open))
         {
            ConsoleDump(file);
            file.Close();
         }
      }
      
      static void Main(string[] args)
      {
         //string filename = @"h:\cpcload.dbf";
         string filename = @"sjov.dbf";

         Create(filename);
         //CreateMemo(filename);
         Open(filename);

      }
   }
}
