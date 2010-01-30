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
         string str = string.Empty;

         int i = 0;
         foreach (FieldInfo field in file.Fields)
         {
            if (0 != i++) str += ",";
            str += field.Name;
         }
         Console.WriteLine(str);

         for (file.Position = 0; file.Position < file.RecordCount; file.Position++)
         {
            str = string.Empty;
            i = 0;
            foreach (FieldInfo field in file.Fields)
            {
               if (0 != i++) str += ",";
               str += file.GetField(field);
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
            if (0 != i++) str += ",";
            str += field.Name;
         }
         Console.WriteLine(str);

         foreach (DBase.Record record in recordset)
         {
            str = string.Empty;
            i = 0;
            foreach (Field field in record)
            {
               if (0 != i++) str += ",";
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
            new FieldInfo("TITLE", DataType.Char, 4, 0),
            new FieldInfo("INTEGER", DataType.Integer, 10, 0),
            new FieldInfo("BOOLEAN", DataType.Boolean, 1, 0),
            new FieldInfo("DATE", DataType.Date, 8, 0),
            new FieldInfo("FLOAT", DataType.Float, 10, 5),
            //new FieldInfo("MEMO", DataType.Memo, 10, 0),
         };

         if (file.Create(filename, fields))
         {
            file.AddRecord();
            file.PutField(fields[0], "sjov");
            file.PutField(fields[1], 10);
            file.PutField(fields[2], true);
            file.PutField(fields[3], DateTimeOffset.UtcNow);
            file.PutField(fields[4], 11.1);
            //file.PutField(fields[5], "memo");
            file.PutRecord();

            file.AddRecord();
            file.PutField(fields[0], "sjov");
            file.PutField(fields[1], 10);
            file.PutField(fields[2], true);
            file.PutField(fields[3], DateTimeOffset.UtcNow);
            file.PutField(fields[4], 11.1);
            //file.PutField(fields[5], "memo");
            file.PutRecord();

            file.Close();
         }
      }

      private static void CreateMemo(string filename)
      {
         var file = new DBase.File();

         FieldInfo[] fields = new FieldInfo[]
         { 
            /*
            new FieldInfo("TITLE", DataType.Char, 4, 0),
            new FieldInfo("INTEGER", DataType.Integer, 10, 0),
            new FieldInfo("BOOLEAN", DataType.Boolean, 1, 0),
            new FieldInfo("DATE", DataType.Date, 8, 0),
            new FieldInfo("FLOAT", DataType.Float, 10, 5),
            */
            new FieldInfo("MEMO", DataType.Memo, 10, 0),
         };

         if (file.Create(filename, fields))
         {
            file.AddRecord();
            file.PutField(fields[0], "sjov");
            /*file.PutField(fields[1], 10);
            file.PutField(fields[2], true);
            file.PutField(fields[3], DateTimeOffset.UtcNow);
            file.PutField(fields[4], 11.1);
            file.PutField(fields[5], "memo");
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

         Create(filename);
         //CreateMemo(filename);
         Open(filename);
         OpenRecordset(filename);
      }
   }
}
