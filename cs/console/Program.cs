// dbf library (sf.net/projects/dbase)
// License: zlib (opensource.org/licenses/zlib-license.php)

using System;

namespace Test
{
   static class Program
   {
      private static char sep = ';';
      private static void ConsoleDump(DBase.File file)
      {
         string str = string.Empty;

         int i = 0;
         foreach (DBase.ColumnInfo col in file.Columns)
         {
            if (0 != i++) str += sep;
            str += col.Name;
         }
         Console.WriteLine(str);

         for (file.Position = 0; file.Position < file.RecordCount; file.Position++)
         {
            str = string.Empty;
            i = 0;
            foreach (DBase.ColumnInfo col in file.Columns)
            {
               if (0 != i++) str += sep;
               str += file.GetString(col);
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
         foreach (DBase.ColumnInfo col in recordset.Columns)
         {
            if (0 != i++) str += sep;
            str += col.Name;
         }
         Console.WriteLine(str);

         foreach (DBase.Record record in recordset)
         {
            str = string.Empty;
            i = 0;
            foreach (DBase.Column col in record)
            {
               if (0 != i++) str += sep;
               str += col.Data;
               //break;
            }
            Console.WriteLine("{0,5} {1}", recordset.Position, str);
            //break;
         }
      }

      private static void CreateMemo(string filename)
      {
         var columns = new DBase.ColumnInfo[]
         { 
            new DBase.ColumnInfo() { Name="MEMO", DataType=DBase.DataType.Memo, Length=10 }
         };
         var file = new DBase.File();
         if (file.Create(filename, columns))
         {
            file.AppendRecord();
            file.WriteField(columns[0], "sjov1");
            file.SaveRecord();
            file.AppendRecord();
            file.WriteField(columns[0], "sjov2");
            file.SaveRecord();

            file.Close();
         }
      }

      private static void Open(string filename)
      {
         var file = new DBase.File();
         if (file.Open(filename, System.IO.FileMode.Open))
         {
            ConsoleDump(file);
            file.Close();
         }
      }
      private static void OpenRecordset(string filename)
      {
         var recordset = new DBase.Recordset();
         if (recordset.Open(filename, System.IO.FileMode.Open))
         {
            ConsoleDump(recordset);
            recordset.Close();
         }
      }
      
      static void Main(string[] args)
      {
         //string filename = @"h:\cpcload.dbf";
         string filename = @"sjov.dbf";

         DBase.Test.CreateDatabase(filename);
         //CreateMemo(filename);
         Open(filename);
         OpenRecordset(filename);
      }
   }
}
