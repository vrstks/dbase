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
         string filename = "sjov.dbf";
         DBase.Test.CreateDatabase(filename);
         var recordset = new DBase.Recordset();
         recordset.Open(filename, System.IO.FileMode.Open);
         var table = new MyDataTable(recordset);
         Title = string.Format("{0} [{1}] - dbf library", System.IO.Path.GetDirectoryName(recordset.Filename), table.TableName);

         var gridview = new GridView();

         foreach (DBase.FieldInfo field in recordset.Fields)
         {
            var gvcolumn = new GridViewColumn();
            gvcolumn.Header = field.Name;
            gvcolumn.DisplayMemberBinding = new Binding(field.Name);
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

   class MyDataTable : DataTable
   {
      public MyDataTable(DBase.Recordset recordset) : base(recordset.TableName)
      {
         foreach (DBase.FieldInfo field in recordset.Fields)
         {
            var column = new DataColumn(field.Name);
            column.DataType = field.GetType();
            Columns.Add(column);
         }
      }
   }
}
