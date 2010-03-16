using System;
using System.Data;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Globalization;

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
         string filename = "sjov.dbf"; FlatDatabase.DBase.Test.CreateDatabase(filename);
         var recordset = new FlatDatabase.DBase.Recordset();
         recordset.Open(filename, System.IO.FileMode.Open);
         //var table = new MyDataTable(recordset);
         Title = string.Format("{0} [{1}] - dbf library", System.IO.Path.GetDirectoryName(recordset.Filename), recordset.TableName);

         var gridview = new GridView();

         Binding bind;
         if (true) foreach (FlatDatabase.ColumnInfo col in recordset.Columns)
         {
            bind = new Binding();

            //bind.Source = recordset;
            bind.Converter = new PropertyObjectConverterCol(col);
            bind.ConverterParameter = col;
            //bind.Mode = BindingMode.OneWay;
            
            var gvcolumn = new GridViewColumn();
            gvcolumn.Header = col.Name;
            gvcolumn.DisplayMemberBinding = bind;// new Binding(/*col.Name*/); // makes wpf look for a property of this name - which doesn't exist
            gridview.Columns.Add(gvcolumn);
         }

         list.View = gridview;

         list.ItemsSource = recordset;
         //list.DataContext = table;
         //list.DataContext = new ObservableCollection<DBase.Recordset>(recordset);
         //bind = new Binding();
         //bind.Source = recordset;
         //bind.Converter = new PropertyObjectConverter();
         //bind.ConverterParameter = recordset;
         //bind.Mode = BindingMode.OneWay;
         //list.SetBinding(ListView.ItemsSourceProperty, bind);
      }
   }

   // not used
   class MyDataTable : DataTable
   {
      public FlatDatabase.DBase.Recordset Recordset { get; private set; }
      public MyDataTable(FlatDatabase.DBase.Recordset recordset) : base(recordset.TableName)
      {
         Recordset = recordset;
         foreach (FlatDatabase.ColumnInfo col in recordset.Columns)
         {
            var column = new DataColumn(col.Name);
            column.DataType = col.DataType;
            Columns.Add(column);
         }
      }
   }

   public class PropertyObjectConverterCol : IValueConverter
   {
      FlatDatabase.ColumnInfo _Col;
      public PropertyObjectConverterCol(FlatDatabase.ColumnInfo col)
      {
         _Col = col;
      }
      public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
      {
         //var recordset = value as DBase.Recordset;
         //var table = value as MyDataTable;
         var record = value as FlatDatabase.DBase.Record;
         var col = parameter as FlatDatabase.ColumnInfo;
         long pos = record.Recordset.Position;
         //return "col";
         //long pos = recordset.Position;
         return pos.ToString();
      }
      public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
      {
         throw new NotImplementedException();
      }
   }
   /*
   public class PropertyObjectConverter : IValueConverter
   {
      public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
      {
         DataTable datatable = value as DataTable;
         DBase.Recordset recordset = parameter as DBase.Recordset;
         //ShownInList obj = (ShownInList)value;  

         //string key = (string)parameter;  
         //return obj.Properties.ContainsKey(key) ? obj.Properties[key].Value :  null;  
         return "sjov";
      }

      public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
      {
         throw new NotImplementedException();
      }
   }
   */
}
