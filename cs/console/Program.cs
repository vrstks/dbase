// dbf library (sf.net/projects/dbase)
// License: zlib (opensource.org/licenses/zlib-license.php)

using System;

namespace Test
{
   static class Program
   {
      private static char sep = ';';
      private static void ConsoleDump(FlatDatabase.DBase.File file)
      {
         string str = string.Empty;

         int i = 0;
         foreach (FlatDatabase.ColumnInfo col in file.Columns)
         {
            if (0 != i++) str += sep;
            str += col.Name;
         }
         Console.WriteLine(str);

         for (file.Position = 0; file.Position < file.RecordCount; file.Position++)
         {
            str = string.Empty;
            i = 0;
            foreach (FlatDatabase.ColumnInfo col in file.Columns)
            {
               if (0 != i++) str += sep;
               str += file.GetString(col);
               //break;
            }
            Console.WriteLine("{0,5} {1}", file.Position, str);
            //break;
         }
      }

      private static void ConsoleDump(FlatDatabase.DBase.Recordset recordset)
      {
         string str = string.Empty;

         int i = 0;
         foreach (FlatDatabase.ColumnInfo col in recordset.Columns)
         {
            if (0 != i++) str += sep;
            str += col.Name;
         }
         Console.WriteLine(str);

         foreach (FlatDatabase.DBase.Record record in recordset)
         {
            str = string.Empty;
            i = 0;
            foreach (FlatDatabase.DBase.Column col in record)
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
         var columns = new FlatDatabase.ColumnInfo[]
         { 
            new FlatDatabase.ColumnInfo() { Name="MEMO", DataType=typeof(string), Length=1024 }
         };
         var file = new FlatDatabase.DBase.File();
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
         var file = new FlatDatabase.DBase.File();
         if (file.Open(filename, System.IO.FileMode.Open))
         {
            ConsoleDump(file);
            file.Close();
         }
      }
      private static void OpenRecordset(string filename)
      {
         var recordset = new FlatDatabase.DBase.Recordset();
         if (recordset.Open(filename, System.IO.FileMode.Open))
         {
            ConsoleDump(recordset);
            recordset.Close();
         }
      }
      
      static void Main(string[] args)
      {
         //string filename = @"h:\cpcload.dbf";
        //string filename = @"E:\nctest\viewer\db\FOXUSER.DBF";
        string filename = @"E:\nctest\viewer\db\FakturaH.DBF";
         //string filename = @"memo.dbf";
        //string filename = @"E:\nctest\viewer\db\i_mon_ch.dbf";

         //FlatDatabase.DBase.Test.CreateDatabase(filename);
         //CreateMemo(filename);
         Open(filename);
         //OpenRecordset(filename);
      }
   }
}
