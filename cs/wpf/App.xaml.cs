using System;
using System.Data;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;

namespace wpf
{
	/// <summary>
	/// Interaction logic for App.xaml
	/// </summary>

	public partial class App : System.Windows.Application
	{
	}

   class Program : Window
   {
      [STAThread]
      static void Main(string[] args)
      {
         TableWindow tablewindow = new TableWindow();
         new Application().Run(tablewindow);
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
   public class TableWindow : Window
   {
      public TableWindow()
      {
         //Create DataTable and put some data in it
         string filename = @"h:\cpcload.dbf";
         var recordset = new DBase.Recordset();
         recordset.Open(filename, System.IO.FileMode.Open);
         var table = new MyDataTable(recordset);
         
         var listview = new ListView();
         var gridview = new GridView();

         foreach (DBase.FieldInfo field in recordset.Fields)
         {
            var gvcolumn = new GridViewColumn();
            gvcolumn.Header = field.Name;
            gvcolumn.DisplayMemberBinding = new Binding(field.Name);
            gridview.Columns.Add(gvcolumn);
         }

         listview.View = gridview;

         var bind = new Binding();
         bind.Source = recordset;

         //listview.ItemsSource = recordset;
         listview.DataContext = table;
         listview.SetBinding(ListView.ItemsSourceProperty, bind);

         Content = listview;
      }
   }
}
