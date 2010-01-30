﻿// dbf library (sf.net/projects/dbase)
// License: zlib (opensource.org/licenses/zlib-license.php)

using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Globalization;
using System.IO;

/// <summary>
/// Driverless dbf access
/// </summary>
namespace DBase
{
#region Definitions
   internal static class Const
   {
      public const int Pack = 1;
      public const int MemoBlockLen = 512;
      public static int HeaderLen { get; private set; }
      public static int FieldLen { get; private set; }
      public const string DataTypes = "CNFDCCML";
      public const char CPM_TEXT_TERMINATOR = '\x1A';
      public const char FieldTerminator = '\r';
      public const int FieldTerminatorLen = 1;
      public const char FieldFillChar = ' ';
      public const int MAGIC_DBASE3 = 0x03;
      public const int MAGIC_DBASE3_MEMO = 0x83;
      public const int MAGIC_DBASE4      = 0x04;
      public const int MAGIC_DBASE4_MEMO = 0x8B;
      public const int MAGIC_FOXPRO      = 0x30;
      public const int EnumeratorDefault = -1;
      public const string Fileext = "dbf";
      public const string FileextMemo = "dbt";

      static Const()
      {
         HeaderLen = Marshal.SizeOf(typeof(DBF_FILEHEADER));
         FieldLen  = Marshal.SizeOf(typeof(DBF_FILEFIELD));
      }

      public static void StructCheck<T>(int size_desired)
      {
         int size = Marshal.SizeOf(typeof(T));
         if (size != size_desired)
         {
            throw new Exception(string.Format("Size of struct {0} is wrong ({1} != {2})", typeof(T), size, size_desired));
         }
      }

      public static void StructCheck()
      {
         StructCheck<DBF_FILEHEADER_TIME>(3);
         StructCheck<DBF_FILEHEADER>(32);
         StructCheck<DBT_FILEHEADER>(MemoBlockLen);
         StructCheck<DBF_FILEFIELD>(32);
      }
   }

   [StructLayout(LayoutKind.Sequential, Pack = Const.Pack)]
   internal struct DBF_FILEHEADER_TIME
   {
      public Byte yy;
      public Byte mm;
      public Byte dd;
   }

   [StructLayout(LayoutKind.Sequential, Pack = Const.Pack)]
   internal struct DBF_FILEHEADER
   {
      public Byte version;
      public DBF_FILEHEADER_TIME lastupdate;
      public UInt32 recordcount;
      public UInt16 headerlength;
      public UInt16 recordlength;
      public UInt16 unused_0;
      public Byte incomplete;
      public Byte crypt;

      [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
      public Byte[] unused;
   }

   [StructLayout(LayoutKind.Sequential, Pack = Const.Pack)]
   internal struct DBT_FILEHEADER
   {
      public UInt32 next;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
      public Byte[] unused0;
      [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 8)]
      public string title;
      public Byte flag;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
      public Byte[] unused1;
      public UInt16 blocksize;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = Const.MemoBlockLen - 22)]
      public Byte[] unused2;
   }

   [StructLayout(LayoutKind.Sequential, Pack = Const.Pack)]
   internal struct DBF_FILEFIELD
   {
      [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 11)]
      public string title;
      public char type;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = 4)]
      public Byte[] unused0;
      public Byte length;
      public Byte deccount;
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = 14)]
      public Byte[] unused1;
   }

   public enum DataType
   {
      Char,
      Integer,
      Float,
      Date,
      Time,  /* non standard */
      DateTime, /* non standard */
      Memo,
      Boolean,
      EnumCount,
      Logical = Boolean,
      Unknown = -1,
      Any = -2
   }

   public class FieldInfo
   {
      public string Name;
      public DataType Type;
      public int Length;
      public int DecCount;
      public int Position;

      public FieldInfo(string name, DataType type, int length)
      {
         Name = name;
         Type = type;
         Length = length;
         DecCount = 0;
      }
      public FieldInfo(string name, DataType type, int length, int deccount)
      {
         Name = name;
         Type = type;
         Length = length;
         DecCount = deccount;
      }
   }
#endregion Definitions

#region Convert
   public static class Convert
   {
      public static class Time
      {
         /// <summary>
         /// time_t/__time64_t is Unix UTC time, seconds since 1970.1.1
         /// </summary>
         public static readonly DateTime OffsetTimeT = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);
         public static Int64 TimeT(DateTimeOffset time)
         {
            Int64 time_t = System.Convert.ToInt64((time - OffsetTimeT).TotalSeconds);
            return time_t;
         }
         public static DateTimeOffset TimeT(Int64 time_t, int ms)
         {
            DateTimeOffset time = OffsetTimeT.AddSeconds(time_t);
            return time.AddMilliseconds(ms);
         }
      }
   }
#endregion Convert

#region Utility
   public interface IOpenClose
   {
      bool IsOpen { get; }
      void Close();
   }

   public static class Utility
   {
      public static T PtrToStructure<T>(byte[] bytes)
      {
         GCHandle pinned = GCHandle.Alloc(bytes, GCHandleType.Pinned);
         T structure = (T)Marshal.PtrToStructure(pinned.AddrOfPinnedObject(), typeof(T));
         pinned.Free();
         return structure;
      }

      public static byte[] StructureToPtr<T>(T anything)
      {
         int rawsize = Marshal.SizeOf(typeof(T));
         byte[] rawdata = new byte[rawsize];
         GCHandle handle = GCHandle.Alloc(rawdata, GCHandleType.Pinned);
         Marshal.StructureToPtr(anything, handle.AddrOfPinnedObject(), false);
         handle.Free();
         return rawdata;
      }
   }
#endregion Utility

   /// <summary>
   /// dbf file class
   /// </summary>
   public class File : IOpenClose
   {
#region Memo
      private class MemoFile
      {
         public FileStream _Stream = null;
         public string Title;
         public long Next = 1;
         private byte[] _Buf = new byte[Const.MemoBlockLen];
         public static string CreateFileName(string filename)
         {
            return Path.ChangeExtension(filename, DBase.Const.FileextMemo);
         }

         public string Read(long pos)
         {
            _Stream.Seek(pos * Const.MemoBlockLen, SeekOrigin.Begin);
            string str = string.Empty;
            for (;;)
            {
               int val = _Stream.ReadByte();
               if (val == -1) break;
               char c = (char)val;
               if (c == Const.CPM_TEXT_TERMINATOR) break;
               str += c;
            }
            return str;
         }

         public long Write(string str)
         {
            long pos = Next;
            _Stream.Seek(pos * Const.MemoBlockLen, SeekOrigin.Begin);
            byte[] bytes = System.Text.Encoding.Default.GetBytes(str + Const.CPM_TEXT_TERMINATOR + Const.CPM_TEXT_TERMINATOR);
            _Stream.Write(bytes, 0, bytes.GetLength(0));
            Next +=    (bytes.GetLength(0) / Const.MemoBlockLen)
                   + (((bytes.GetLength(0) % Const.MemoBlockLen) != 0) ? 1 : 0);
            return pos;
         }

         public bool ReadHeader()
         {
            int len = Marshal.SizeOf(typeof(DBT_FILEHEADER));
            byte[] bytes = new byte[len];

            bool ok = (len == _Stream.Read(bytes, 0, len));
            if (ok)
            {
               DBT_FILEHEADER header = Utility.PtrToStructure<DBT_FILEHEADER>(bytes);
               Next = header.next;
            }
            return ok;
         }

         public void WriteHeader()
         {
            _Stream.Seek(0, SeekOrigin.Begin);
            string title = Title;
            if (title.Length > 8) title = title.Substring(0, 8);
            DBT_FILEHEADER header = new DBT_FILEHEADER();
            header.next = (uint)Next;
            header.title = title;
            header.flag = 0;
            header.blocksize = Const.MemoBlockLen;
            byte[] bytes = Utility.StructureToPtr<DBT_FILEHEADER>(header);
            _Stream.Write(bytes, 0, bytes.GetLength(0));
         }
      }
#endregion Memo
      private MemoFile _MemoFile = new MemoFile();
      private FileStream _Stream = null;

      public string Filename { get; private set; }
      public bool IsOpen { get { return (_Stream != null); } }
      public bool HasMemo { get { return (_MemoFile._Stream != null); } }

      public List<FieldInfo> Fields { get; private set; }

      public bool IsEditable { get { return (_Stream != null) && _Stream.CanWrite; } }
      public bool IsDirty { get; private set; }

      static File()
      {
         Const.StructCheck();
      }

      public File()
      {
         IsDirty = false;
         Fields = new List<FieldInfo>();
      }
      ~File()
      {
         if (IsOpen) Close();
      }

      public bool Attach(FileStream stream, string filename)
      {
         bool ok = (stream != null);
         if (ok)
         {
            _Stream = stream;
            Filename = filename;

            int len_header = Marshal.SizeOf(typeof(DBF_FILEHEADER));
            int len_field = Marshal.SizeOf(typeof(DBF_FILEFIELD));

            byte[] bytes = new byte[len_header];

            if (len_header == StreamRead(bytes))
            {
               DBF_FILEHEADER header = Utility.PtrToStructure<DBF_FILEHEADER>(bytes);
               RecordCount = header.recordcount;
               RecordLength = header.recordlength;
               HeaderLength = header.headerlength;

               if ( (RecordCount == 0) && (RecordLength != 0) )
               {
                  RecordCount = (_Stream.Length - header.headerlength) / header.recordlength;
               }

               int fieldcount = (header.headerlength - (len_header + 1)) / len_field;

               bytes = new byte[len_field];

               for (int i = 0; i < fieldcount; i++)
               {
                  StreamRead(bytes);
                  DBF_FILEFIELD item = Utility.PtrToStructure<DBF_FILEFIELD>(bytes);
                  var type = (DataType)Const.DataTypes.IndexOf(item.type);
                  var field = new FieldInfo(item.title, type, item.length, item.deccount);
                  Fields.Add(field);
               }
            }
         }
         return ok;
      }

      public bool AttachMemo(FileStream stream, string filename)
      {
         _MemoFile._Stream = stream;
         _MemoFile.Title = Path.GetFileNameWithoutExtension(filename);
         return _MemoFile.ReadHeader();
      }

      public void Detach(out FileStream stream, out FileStream memostream)
      {
         if (IsDirty)
         {
            StreamSeek(0);

            DBF_FILEHEADER header = new DBF_FILEHEADER();
            
            header.headerlength = (ushort)HeaderLength;
            header.crypt = 0;
            header.incomplete = 0;

            DateTimeOffset now = DateTimeOffset.UtcNow;

            header.lastupdate.yy = (byte)(now.Year - 1900);
            header.lastupdate.mm = (byte)now.Month;
            header.lastupdate.dd = (byte)now.Day;
            header.recordcount = (ushort)RecordCount;
            header.recordlength = (ushort)RecordLength;
            header.version = (byte)(HasMemo ? Const.MAGIC_DBASE3_MEMO : Const.MAGIC_DBASE3);
            header.unused_0 = 0;
            for (int i = 0; i < 16; i++)
            {
               //header.unused[i] = 0;
            }
            byte[] bytes = Utility.StructureToPtr<DBF_FILEHEADER>(header);
            StreamWrite(bytes);
            StreamSeek(HeaderLength + RecordCount * RecordLength);

            bytes = new byte[] { (byte)Const.CPM_TEXT_TERMINATOR };
            StreamWrite(bytes);

            if (HasMemo)
            {
               _MemoFile.WriteHeader();
            }
         }
         stream = _Stream;
         memostream = _MemoFile._Stream;
         _Stream = _MemoFile._Stream = null;
         Filename = string.Empty;
         IsDirty = false;
      }

      public bool Open(string filename, FileMode mode)
      {
         FileAccess access = (mode == FileMode.Open)
           ? FileAccess.Read      // non-exclusive
           : FileAccess.ReadWrite; // exclusive
         var stream = new FileStream(filename, mode, access);
         bool ok = (stream != null);
         if (ok)
         {
            ok = Attach(stream, filename);
            if (ok)
            {
               bool memo = false;
               foreach (FieldInfo item in Fields)
               {
                  memo = memo || (item.Type == DataType.Memo);
               }
               if (memo)
               {
                  string filename_memo = MemoFile.CreateFileName(filename);
                  var stream_memo = new FileStream(filename_memo, mode, access);
                  ok = (stream_memo != null);
                  if (ok)
                  {
                     ok = AttachMemo(stream_memo, filename_memo);
                  }
                  if (!ok)
                  {
                     Close();
                  }
               }
            }
         }
         return ok;
      }

      public bool Create(string filename, FieldInfo[] array)
      {
         var list = new List<FieldInfo>();
         foreach (FieldInfo item in array)
         {
            list.Add(item);
         }
         return Create(filename, list);
      }

      public bool Create(string filename, List<FieldInfo> fields)
      {
         FileMode mode = FileMode.Create;
         FileAccess access = FileAccess.ReadWrite; // exclusive
         string filename_memo = string.Empty;
         bool memo = false;
         foreach (FieldInfo item in fields)
         {
            memo = memo || (item.Type == DataType.Memo);
         }
         FileStream stream = new FileStream(filename, mode, access);
         FileStream stream_memo = null;
         bool ok = (stream != null);

         if (ok && memo)
         {
            filename_memo = MemoFile.CreateFileName(filename);
            stream_memo = new FileStream(filename_memo, mode, access);
            ok = (stream_memo != null);
         }
         if (ok)
         {
            byte[] bytes;

            ok = Attach(stream, filename);
            if (ok)
            {
               HeaderLength = Const.HeaderLen + Const.FieldTerminatorLen + Const.FieldLen * fields.Count;
               IsDirty = true;
               _Stream.SetLength(HeaderLength);
               StreamSeek(Const.HeaderLen);
               Fields = fields;

               RecordLength = 1;
               foreach (FieldInfo item in fields)
               {
                  DBF_FILEFIELD field = new DBF_FILEFIELD();
                  field.title = item.Name;
                  field.type = Const.DataTypes[(int)item.Type];
                  field.length = (byte)item.Length;
                  field.deccount = (byte)item.DecCount;
                  bytes = Utility.StructureToPtr<DBF_FILEFIELD>(field);
                  StreamWrite(bytes);
                  RecordLength += item.Length;
               }
               bytes = new byte[] { (byte)Const.FieldTerminator };
               StreamWrite(bytes);
            }
            if (stream_memo != null)
            {
               AttachMemo(stream_memo, filename_memo);
            }
         }
         return ok;
      }

      public void Close()
      {
         FileStream stream;
         FileStream memostream;
         Detach(out stream, out memostream);
         if (stream     != null) stream    .Close();
         if (memostream != null) memostream.Close();
         Fields.Clear();
         RecordCount = 0;
         RecordLength = 0;
         _Position = Const.EnumeratorDefault;
      }

      private int StreamRead(byte[] bytes)
      {
         return _Stream.Read(bytes, 0, bytes.GetLength(0));
      }
      private void StreamWrite(byte[] bytes)
      {
         _Stream.Write(bytes, 0, bytes.GetLength(0));
      }
      private long StreamSeek(long offset)
      {
         return _Stream.Seek(offset, SeekOrigin.Begin);
      }      

      private string _RecordBuf = null;
      private long _Position = Const.EnumeratorDefault;
      public long Position
      {
         get
         { 
            return _Position;
         }
         set
         {
            if (_Position != value)
            {
               _Position = value;
               ReadRecord();
            }
         }
      }

      public long RecordCount { get; private set; }
      public int RecordLength { get; private set; }
      private int HeaderLength { get; set; }

      public bool AppendRecord()
      {
         _Position = RecordCount;
         _RecordBuf = new string(Const.FieldFillChar, RecordLength);
         RecordCount++;
         return SaveRecord();
      }

      public bool SaveRecord()
      {
         StreamSeek(HeaderLength + _Position * RecordLength + Const.FieldTerminatorLen);
         byte[] bytes = System.Text.Encoding.Default.GetBytes(_RecordBuf);
         StreamWrite(bytes);
         return true;
      }

      private bool ReadRecord()
      {
         StreamSeek(HeaderLength + _Position * RecordLength + Const.FieldTerminatorLen);
         byte[] bytes = new byte[RecordLength];
         StreamRead(bytes);
         _RecordBuf = System.Text.Encoding.Default.GetString(bytes, 0, RecordLength);
         return true;
      }

      public bool WriteField(FieldInfo field, string str)
      {
         switch (field.Type)
         {
            case DataType.Memo:
               str = _MemoFile.Write(str).ToString();
               break;
         }
         int pos = GetRecordBufPos(field);
         string temp = str;
         while (temp.Length < field.Length) temp += Const.FieldFillChar;
         _RecordBuf = _RecordBuf.Remove(pos, field.Length);
         _RecordBuf = _RecordBuf.Insert(pos, temp);
         return true;
      }

      public bool WriteField(FieldInfo field, int n)
      {
         return WriteField(field, n.ToString());
      }

      public bool WriteField(FieldInfo field, DateTimeOffset date)
      {
         string str = string.Format("{0:0000}{1:00}{2:00}", date.Year, date.Month, date.Day);
         return WriteField(field, str);
      }

      public bool WriteField(FieldInfo field, bool b)
      {
         return WriteField(field, b ? "T" : "F");
      }

      public bool WriteField(FieldInfo field, double n)
      {
         string fmt = "{0:0";
         if (field.DecCount != 0)
         {
            fmt += "." + new string('0', field.DecCount);
         }
         fmt += "}";
         string str = string.Format(fmt, n);
         str = str.Replace(',', '.');
         return WriteField(field, str);
      }

      private int GetRecordBufPos(FieldInfo field)
      {
         int pos = 0;
         foreach (FieldInfo item in Fields)
         {
            if (item == field) break;
            pos += item.Length;
         }
         return pos;
      }

      private static char[] _FieldDataTrim = new char[] { Const.FieldFillChar, '\0' };
      public string GetString(FieldInfo field)
      {
         int pos = GetRecordBufPos(field);
         string str = _RecordBuf.Substring(pos, field.Length);
         str = str.TrimEnd(_FieldDataTrim);
         switch (field.Type)
         {
            case DataType.Memo:
               str = _MemoFile.Read(long.Parse(str));
               break;
         }
         return str;
      }

      public object GetData(FieldInfo field)
      {
         object obj = null;
         string str = GetString(field);
         switch (field.Type)
         {
            case DataType.Char:
               obj = str;
               break;
            case DataType.Integer:
            {
               long n = 0;
               long.TryParse(str, out n);
               obj = n;
               break;
            }
            case DataType.Float:
            {
               double n = 0;
               double.TryParse(str, System.Globalization.NumberStyles.Any, CultureInfo.InvariantCulture, out n);
               obj = n;
               break;
            }
            case DataType.Date:
            {
               DateTimeOffset n = new DateTimeOffset(
                  int.Parse(str.Substring(0, 4)), 
                  int.Parse(str.Substring(4, 2)), 
                  int.Parse(str.Substring(6, 2)),
                  0,0,0, DateTimeOffset.Now.Offset
                  );
               obj = n;
               break;
            }
            case DataType.Boolean:
            {
               bool b = (-1 != str.IndexOfAny("YyTt".ToCharArray()));
               obj = b;
               break;
            }
            case DataType.Memo:
            {
               obj = str;
               break;
            }
         }
         return obj;
      }
   }

   #region Recordset
   // Adds a richer "syntactical sugar" api to File, supporting foreach
   public class Recordset : File, IEnumerable, IEnumerator
   {
      private Record _Record;

      public Recordset()
      {
         _Record = new Record(this);
      }

      #region IEnumerable Members
      public IEnumerator GetEnumerator()
      {
         return this;
      }
      #endregion

      #region IEnumerator Members
      public object Current { get { return _Record; } }

      public bool MoveNext()
      {
         _Record.Reset();
         Position++;
         return (Position < RecordCount);
      }

      public void Reset()
      {
         Position = Const.EnumeratorDefault;
      }
      #endregion
   }

   public class Record : IEnumerable, IEnumerator
   {
      public Recordset Recordset { get; private set; }
      private Field _Field;
      public int FieldPosition { get; set; }
      public Record(Recordset recordset)
      {
         Recordset = recordset;
         _Field = new Field(this);
         Reset();
      }
      
      #region IEnumerable Members
      public IEnumerator GetEnumerator()
      {
         return this;
      }
      #endregion

      #region IEnumerator Members
      public object Current { get { return _Field; } }

      public bool MoveNext()
      {
         FieldPosition++;
         return (FieldPosition < Recordset.Fields.Count);
      }

      public void Reset()
      {
         FieldPosition = Const.EnumeratorDefault;
      }
      #endregion
   }

   public class Field
   {
      public Record Record { get; private set; }
      public Field(Record record)
      {
         Record = record;
      }
      public object Data
      { 
         get { return Record.Recordset.GetData(Record.Recordset.Fields[Record.FieldPosition]); } 
      }
      public string String
      { 
         get { return Record.Recordset.GetString(Record.Recordset.Fields[Record.FieldPosition]); } 
      }
      public double Float
      {
         get { return (double)Data; }
      }
      public int Integer
      {
         get { return (int)Data; }
      }
      public DateTimeOffset DateTimeOffset
      {
         get { return (DateTimeOffset)Data; }
      }
      public DateTime DateTime
      {
         get { return DateTimeOffset.DateTime; }
      }
      public bool Boolean
      {
         get { return (bool)Data; }
      }
   }

   #endregion Recordset
}
