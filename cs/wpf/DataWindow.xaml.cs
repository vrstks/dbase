using System;
using System.Data;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace Test
{
   /// <summary>
   /// Interaction logic for DataWindow.xaml
   /// </summary>
   public partial class DataWindow : Window
   {
      public DataWindow()
      {
         InitializeComponent();
         
         //string filename = @"h:\cpcload.dbf";
         string filename = "sjov.dbf"; DBase.Test.CreateDatabase(filename);
         var recordset = new DBase.Recordset();
         recordset.Open(filename, System.IO.FileMode.Open);
         //var table = new MyDataTable(recordset);
         Title = string.Format("{0} [{1}] - dbf library", System.IO.Path.GetDirectoryName(recordset.Filename), recordset.TableName);

         var gridview = new GridView();

         foreach (DBase.ColumnInfo col in recordset.Columns)
         {
            var gvcolumn = new GridViewColumn();
            gvcolumn.Header = col.Name;
            gvcolumn.DisplayMemberBinding = new Binding(col.Name); // makes wpf look for a property of this name - which doesn't exist
            gridview.Columns.Add(gvcolumn);
         }

         list.View = gridview;

         var bind = new Binding();
         bind.Source = recordset;

         list.ItemsSource = recordset;
         //list.DataContext = new ObservableCollection<DBase.Recordset>(recordset);
         list.SetBinding(ListView.ItemsSourceProperty, bind);
      }
   }

   // not used
   class MyDataTable : DataTable
   {
      public MyDataTable(DBase.Recordset recordset) : base(recordset.TableName)
      {
         foreach (DBase.ColumnInfo col in recordset.Columns)
         {
            var column = new DataColumn(col.Name);
            column.DataType = col.GetType();
            Columns.Add(column);
         }
      }
   }
}
