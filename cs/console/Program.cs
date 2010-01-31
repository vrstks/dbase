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
      private static char sep = ';';
      private static void ConsoleDump(DBase.File file)
      {
         string str = string.Empty;

         int i = 0;
         foreach (FieldInfo field in file.Fields)
         {
            if (0 != i++) str += sep;
            str += field.Name;
         }
         Console.WriteLine(str);

         for (file.Position = 0; file.Position < file.RecordCount; file.Position++)
         {
            str = string.Empty;
            i = 0;
            foreach (FieldInfo field in file.Fields)
            {
               if (0 != i++) str += sep;
               str += file.GetString(field);
               //break;
            }
            Console.WriteLine("{0,5} {1}", file.Position, str);
            //break;
         }
      }

      private static void ConsoleDump(DBase.Recordset recordset)
      {
         string str = string.Empty;

         int i = 0;
         foreach (FieldInfo field in recordset.Fields)
         {
            if (0 != i++) str += sep;
            str += field.Name;
         }
         Console.WriteLine(str);

         foreach (DBase.Record record in recordset)
         {
            str = string.Empty;
            i = 0;
            foreach (Field field in record)
            {
               if (0 != i++) str += sep;
               str += field.Data;
               //break;
            }
            Console.WriteLine("{0,5} {1}", recordset.Position, str);
            //break;
         }
      }

      private static void Create(string filename)
      {
         var file = new DBase.File();

         FieldInfo[] fields = new FieldInfo[]
         { 
            new FieldInfo("TITLE", DataType.Char, 4),
            new FieldInfo("INTEGER", DataType.Integer, 10),
            new FieldInfo("BOOLEAN", DataType.Boolean, 1),
            new FieldInfo("DATE", DataType.Date, 8),
            new FieldInfo("FLOAT", DataType.Float, 10, 5),
            new FieldInfo("MEMO", DataType.Memo, 10),
         };

         if (file.Create(filename, fields))
         {
            file.AppendRecord();
            file.WriteField(fields[0], "sjov");
            file.WriteField(fields[1], 10);
            file.WriteField(fields[2], true);
            file.WriteField(fields[3], DateTimeOffset.UtcNow);
            file.WriteField(fields[4], 11.1);
            file.WriteField(fields[5], "memo");
            file.SaveRecord();

            file.AppendRecord();
            file.WriteField(fields[0], "sjov");
            file.WriteField(fields[1], 10);
            file.WriteField(fields[2], true);
            file.WriteField(fields[3], DateTimeOffset.UtcNow);
            file.WriteField(fields[4], 11.1);
            file.WriteField(fields[5], "memo");
            file.SaveRecord();

            file.Close();
         }
      }

      private static void CreateMemo(string filename)
      {
         var file = new DBase.File();

         FieldInfo[] fields = new FieldInfo[]
         { 
            new FieldInfo("MEMO", DataType.Memo, 10),
         };

         if (file.Create(filename, fields))
         {
            file.AppendRecord();
            file.WriteField(fields[0], "sjov1");
            file.SaveRecord();
            file.AppendRecord();
            file.WriteField(fields[0], "sjov2");
            file.SaveRecord();

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
      private static void OpenRecordset(string filename)
      {
         var recordset = new DBase.Recordset();
         if (recordset.Open(filename, io.FileMode.Open))
         {
            ConsoleDump(recordset);
            recordset.Close();
         }
      }
      
      static void Main(string[] args)
      {
         //string filename = @"h:\cpcload.dbf";
         string filename = @"sjov.dbf";

         //Create(filename);
         CreateMemo(filename);
         Open(filename);
         //OpenRecordset(filename);
      }
   }
}
