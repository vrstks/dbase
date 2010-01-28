// dbf library (sf.net/projects/dbase)
// License: zlib (opensource.org/licenses/zlib-license.php)
// TODO: memo file header/put/get

using System;
using System.Runtime.InteropServices;
using io = System.IO;

/// <summary>
/// Driverless dbf access
/// </summary>
namespace DBase
{
#region Definitions
   internal static class Const
   {
      public const int Pack = 1;
      public const int MEMO_BLOCK_SIZE = 512;
      public const int HEADER_LENGTH = 32;
      public const int FIELD_REC_LENGTH = 32;
      public const string DataTypes = "CNFDCCML";
      public const byte CPM_TEXT_TERMINATOR = 0x1A;
      public const char FIELDTERMINATOR = '\r';
      public const int FIELDTERMINATOR_LEN = 1;
      public const int MAGIC_DBASE3 = 0x03;
      public const int MAGIC_DBASE3_MEMO = 0x83;
      public const int MAGIC_DBASE4      = 0x04;
      public const int MAGIC_DBASE4_MEMO = 0x8B;
      public const int MAGIC_FOXPRO      = 0x30;
      public const string Fileext = "dbf";
      public const string FileextMemo = "dbt";

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
         StructCheck<DBF_FILEHEADER>(HEADER_LENGTH);
         StructCheck<DBT_FILEHEADER>(MEMO_BLOCK_SIZE);
         StructCheck<DBF_FILEFIELD>(FIELD_REC_LENGTH);
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
      [MarshalAs(UnmanagedType.ByValArray, SizeConst = Const.MEMO_BLOCK_SIZE - 22)]
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

   public struct DBF_FIELD_DATA
   {
      public string Name;
      public DataType Type;
      public int Length;
      public int DecCount;

      public DBF_FIELD_DATA(string _Name, DataType _Type, int _Length, int _DecCount)
      {
         Name = _Name;
         Type = _Type;
         Length = _Length;
         DecCount = _DecCount;
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
      private io.FileStream m_stream = null;
      private io.FileStream m_stream_memo = null;

      public string Filename { get; private set; }
      public string FilenameMemo { get; private set; }
      public bool IsOpen { get { return (m_stream != null); } }

      private DBF_FIELD_DATA[] _Fields;
      public int FieldCount { get { return (_Fields != null) ? _Fields.GetLength(0) : 0; } }
      public DBF_FIELD_DATA GetFieldInfo(int field) { return _Fields[field]; }
      public bool IsEditable { get { return (m_stream != null) && m_stream.CanWrite; } }
      public bool IsDirty { get; private set; }

      static File()
      {
         Const.StructCheck();
      }

      public File()
      {
         IsDirty = false;
      }
      ~File()
      {
         if (IsOpen) Close();
      }

      public bool Attach(io.FileStream stream, string filename)
      {
         bool ok = (stream != null);
         if (ok)
         {
            //!( (mode == io.FileMode.Create) || (mode == io.FileMode.CreateNew)) 

            m_stream = stream;
            Filename = filename;

            byte[] bytes = new byte[Const.HEADER_LENGTH];

            if (Const.HEADER_LENGTH == m_stream.Read(bytes, 0, Const.HEADER_LENGTH))
            {
               DBF_FILEHEADER header = Utility.PtrToStructure<DBF_FILEHEADER>(bytes);
               RecordCount = header.recordcount;
               RecordLength = header.recordlength;
               HeaderLength = header.headerlength;

               if ( (RecordCount == 0) && (RecordLength != 0) )
               {
                  RecordCount = (m_stream.Length - header.headerlength) / header.recordlength;
               }

               int fieldcount = (header.headerlength - (Const.HEADER_LENGTH + 1)) / Const.FIELD_REC_LENGTH;
               _Fields = new DBF_FIELD_DATA[fieldcount];

               bytes = new byte[Const.FIELD_REC_LENGTH];

               for (int i = 0; i < fieldcount; i++)
               {
                  m_stream.Read(bytes, 0, Const.FIELD_REC_LENGTH);
                  DBF_FILEFIELD item = Utility.PtrToStructure<DBF_FILEFIELD>(bytes);
                  DBF_FIELD_DATA field = new DBF_FIELD_DATA();
                  field.Name = item.title;
                  field.Length = item.length;
                  field.Type = (DataType)Const.DataTypes.IndexOf(item.type);
                  field.DecCount = item.deccount;
                  _Fields[i] = field;
               }
               Position = 0;
            }
         }
         return ok;
      }

      public bool AttachMemo(io.FileStream stream, string filename)
      {
         m_stream_memo = stream;
         FilenameMemo = filename;
         return true;
      }

      public void Detach(out io.FileStream stream, out io.FileStream memostream)
      {
         if (IsDirty)
         {
            m_stream.Seek(0, io.SeekOrigin.Begin);

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
            header.version = (byte)((m_stream_memo != null) ? Const.MAGIC_DBASE3_MEMO : Const.MAGIC_DBASE3);
            header.unused_0 = 0;
            for (int i = 0; i < 16; i++)
            {
               //header.unused[i] = 0;
            }
            byte[] bytes = Utility.StructureToPtr<DBF_FILEHEADER>(header);
            m_stream.Write(bytes, 0, bytes.GetLength(0));
            m_stream.Seek(HeaderLength + RecordCount * RecordLength, io.SeekOrigin.Begin);

            bytes[0] = Const.CPM_TEXT_TERMINATOR;
            m_stream.Write(bytes, 0, 1);
         }
         stream = m_stream;
         memostream = m_stream_memo;
         m_stream = null;
         Filename = string.Empty;
         IsDirty = false;
      }

      public bool Open(string filename, io.FileMode mode)
      {
         var stream = new io.FileStream(filename, mode, (mode == io.FileMode.Open)
           ? io.FileAccess.Read      /* non-exclusive */
           : io.FileAccess.ReadWrite /* exclusive     */
           );
         bool ok = (stream != null);
         if (ok)
         {
            ok = Attach(stream, filename);
         }
         return ok;
      }

      public bool Create(string filename, DBF_FIELD_DATA[] fields)
      {
         string filename_memo = string.Empty;
         bool memo = false;
         for (int i = 0; i < fields.GetLength(0); i++)
         {
            memo = memo || (fields[i].Type == DataType.Memo);
         }
         io.FileStream stream = new io.FileStream(filename, io.FileMode.Create, io.FileAccess.ReadWrite);
         io.FileStream stream_memo = null;
         bool ok = (stream != null);

         if (ok && memo)
         {
            filename_memo = io.Path.ChangeExtension(filename, DBase.Const.FileextMemo);
            stream_memo = new io.FileStream(filename_memo, io.FileMode.Create, io.FileAccess.ReadWrite);
            ok = (stream_memo != null);
         }
         if (ok)
         {
            ok = Attach(stream, filename);
            if (ok)
            {
               byte[] bytes;

               _Fields = fields;
               HeaderLength = Const.HEADER_LENGTH + Const.FIELDTERMINATOR_LEN + Const.FIELD_REC_LENGTH * FieldCount;
               IsDirty = true;
               m_stream.SetLength(HeaderLength);
               m_stream.Seek(Const.HEADER_LENGTH, io.SeekOrigin.Begin);

               RecordLength = 1;
               for (int i = 0; i < FieldCount; i++)
               {
                  DBF_FILEFIELD field = new DBF_FILEFIELD();
                  field.title = fields[i].Name;
                  field.type = Const.DataTypes[(int)fields[i].Type];
                  field.length = (byte)fields[i].Length;
                  field.deccount = (byte)fields[i].DecCount;
                  bytes = Utility.StructureToPtr<DBF_FILEFIELD>(field);
                  m_stream.Write(bytes, 0, bytes.GetLength(0));
                  RecordLength += fields[i].Length;
               }
               bytes = new byte[1];
               bytes[0] = (byte)Const.FIELDTERMINATOR;
               m_stream.Write(bytes, 0, bytes.GetLength(0));
               _RecordBuf = new byte[RecordLength];
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
         io.FileStream stream;
         io.FileStream memostream;
         Detach(out stream, out memostream);
         if (stream     != null) stream    .Close();
         if (memostream != null) memostream.Close();
      }

      private byte[] _RecordBuf = null;
      private long _Position = -1;
      public long Position
      {
         get { return _Position; }
         set
         {
            if (_Position != value)
            {
               m_stream.Seek(HeaderLength + value * RecordLength + Const.FIELDTERMINATOR_LEN, io.SeekOrigin.Begin);
               m_stream.Read(_RecordBuf, 0, RecordLength);
               _Position = value;
            }
         }
      }

      private long _NextMemoPosition = 0;

      public long RecordCount { get; private set; }
      public int RecordLength { get; private set; }
      private int HeaderLength { get; set; }

      private static char[] FieldTrim = new char[] { ' ', '\0' };

      private void ClearRecordBuf()
      {
         for (int i = 0; i < _RecordBuf.GetLength(0); i++)
         {
            _RecordBuf[0] = 0;
         }
      }
      public bool AddRecord()
      {
         _Position = RecordCount;
         m_stream.Seek(HeaderLength + _Position * RecordLength + Const.FIELDTERMINATOR_LEN, io.SeekOrigin.Begin);
         ClearRecordBuf();
         m_stream.Write(_RecordBuf, 0, _RecordBuf.GetLength(0));
         RecordCount++;
         return true;
      }

      public bool PutRecord()
      {
         m_stream.Seek(HeaderLength + _Position * RecordLength + Const.FIELDTERMINATOR_LEN, io.SeekOrigin.Begin);
         m_stream.Write(_RecordBuf, 0, _RecordBuf.GetLength(0));
         return true;
      }

      public bool PutField(int field, string str)
      {
         DBF_FIELD_DATA fielddata = _Fields[field];
         switch (fielddata.Type)
         {
            case DataType.Memo:
            {
               m_stream_memo.Seek(_NextMemoPosition * Const.MEMO_BLOCK_SIZE, io.SeekOrigin.Begin);
               str = _NextMemoPosition.ToString();
               _NextMemoPosition++;
               break;
            }
         }
         byte[] bytes = System.Text.Encoding.Default.GetBytes(str);
         int pos = GetRecordBufPos(field);
         int len = Math.Min(fielddata.Length, bytes.GetLength(0));
         Buffer.BlockCopy(bytes, 0, _RecordBuf, pos, len);
         return true;
      }

      public bool PutField(int field, int n)
      {
         return PutField(field, n.ToString());
      }

      public bool PutField(int field, DateTimeOffset date)
      {
         string str = string.Format("{0:0000}{1:00}{2:00}", date.Year, date.Month, date.Day);
         return PutField(field, str);
      }

      public bool PutField(int field, bool b)
      {
         return PutField(field, b ? "T" : "F");
      }

      public bool PutField(int field, double n)
      {
         DBF_FIELD_DATA fielddata = _Fields[field];
         string fmt = "{0:0";
         if (fielddata.DecCount != 0)
         {
            fmt += ".";
            for (int i = 0; i < fielddata.DecCount; i++)
            {
               fmt += "0";
            }
         }
         fmt += "}";
         string str = string.Format(fmt, n);
         str = str.Replace(',', '.');
         return PutField(field, str);
      }

      private int GetRecordBufPos(int field)
      {
         int pos = 0;
         for (int i = 0; i < field; i++)
         {
            pos += _Fields[i].Length;
         }
         return pos;
      }

      public string GetField(int index)
      {
         string str = string.Empty;
         int pos = 0;
         for (int i = 0; i < index; i++)
         {
            pos += _Fields[i].Length;
         }
         DBF_FIELD_DATA fielddata = _Fields[index];
         str = System.Text.Encoding.Default.GetString(_RecordBuf, pos, fielddata.Length);
         str = str.TrimEnd(FieldTrim);
         switch (fielddata.Type)
         {
            case DataType.Memo:
            {
               break;
            }
         }
         return str;
      }
   }
}
