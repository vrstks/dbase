// Program.cs
// Copyright (c) 2010 by Troels K. All rights reserved.
// License: zlib

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

         for (file.Position = 0; file.Position < file.Recordcount; file.Position++)
         {
            str = string.Empty;
            for (i = 0; i < file.FieldCount; i++)
            {
               if (i != 0) str += ",";
               str+=file.GetField(i);
               //break;
            }
            Console.WriteLine("{0,5} {1}", i, str);
            //break;
         }
      }

      static void Create(DBase.File file)
      {
      }

      static void Main(string[] args)
      {
         //string filename = @"h:\cpcload.dbf";
         string filename = @"sjov.dbf";
         var file = new DBase.File();

         DBF_FIELD_DATA[] fields = new DBF_FIELD_DATA[]
         { 
            new DBF_FIELD_DATA("TITLE", DataType.Char, 10, 0),
            new DBF_FIELD_DATA("VALUE", DataType.Integer, 10, 0)
         };

         if (file.Create(filename, fields))
         {
            file.Close();
         }

         if (file.Open(filename, io.FileMode.Open))
         {
            ConsoleDump(file);
            file.Close();
         }
      }
   }
}
