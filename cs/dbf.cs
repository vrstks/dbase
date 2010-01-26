// dbf.cs
// Copyright (c) 2010 by Troels K. All rights reserved.
// License: zlib

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
      public const int FIELDDATAOFFSET = 1;
      public const int MAGIC_DBASE3 = 0x03;
      public const int MAGIC_DBASE3_MEMO = 0x83;
      public const int MAGIC_DBASE4      = 0x04;
      public const int MAGIC_DBASE4_MEMO = 0x8B;
      public const int MAGIC_FOXPRO      = 0x30;

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
      private io.BinaryReader m_reader = null;
      private io.BinaryWriter m_writer = null;

      public string Filename { get; private set; }
      public static string Fileext { get { return "dbf"; } }
      public static string FileextMemo { get { return "dbt"; } }
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
            m_reader = new io.BinaryReader(m_stream);
            if (m_stream.CanWrite)
            {
               m_writer = new io.BinaryWriter(m_stream);
            }
            
            byte[] bytes = m_reader.ReadBytes(Const.HEADER_LENGTH);
            if (Const.HEADER_LENGTH == bytes.GetLength(0))
            {
               DBF_FILEHEADER header = Utility.PtrToStructure<DBF_FILEHEADER>(bytes);
               Recordcount = (int)header.recordcount;
               RecordLength = header.recordlength;
               HeaderLength = header.headerlength;

               int fieldcount = (header.headerlength - (Const.HEADER_LENGTH + 1)) / Const.FIELD_REC_LENGTH;
               _Fields = new DBF_FIELD_DATA[fieldcount];
               for (int i = 0; i < fieldcount; i++)
               {
                  bytes = m_reader.ReadBytes(Const.FIELD_REC_LENGTH);
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

      public io.FileStream Detach()
      {
         if (IsDirty)
         {
            m_stream.Seek(0, io.SeekOrigin.Begin);

            //byte[] bytes = new byte[Const.HEADER_LENGTH];
            //DBF_FILEHEADER header = Utility.StructureToPtr<DBF_FILEHEADER>(header);
            
            DBF_FILEHEADER header = new DBF_FILEHEADER();
            header.headerlength = (ushort)HeaderLength;
            header.crypt = 0;
            header.incomplete = 0;

            DateTimeOffset now = DateTimeOffset.UtcNow;

            header.lastupdate.yy = (byte)(now.Year - 1900);
            header.lastupdate.mm = (byte)now.Month;
            header.lastupdate.dd = (byte)now.Day;
            header.recordcount = (ushort)Recordcount;
            header.recordlength = 0;
            header.version = Const.MAGIC_DBASE3;
            header.unused_0 = 0;
            for (int i = 0; i < 16; i++)
            {
               //header.unused[i] = 0;
            }
            byte[] bytes = Utility.StructureToPtr<DBF_FILEHEADER>(header);
            m_writer.Write(bytes);
         }
         io.FileStream stream = m_stream;
         m_stream = null;
         Filename = string.Empty;
         m_reader = null;
         m_writer = null;
         IsDirty = false;
         return stream;
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
         var stream = new io.FileStream(filename, io.FileMode.Create, io.FileAccess.ReadWrite);
         bool ok = (stream != null);
         if (ok)
         {
            ok = Attach(stream, filename);
            if (ok)
            {
               _Fields = fields;
               HeaderLength = Const.HEADER_LENGTH + Const.FIELDDATAOFFSET + Const.FIELD_REC_LENGTH * FieldCount;
               IsDirty = true;
               m_stream.SetLength(HeaderLength);
               long pos = m_stream.Seek(Const.HEADER_LENGTH, io.SeekOrigin.Begin);

               for (int i = 0; i < FieldCount; i++)
               {
                  DBF_FILEFIELD field = new DBF_FILEFIELD();
                  field.title = fields[i].Name;
                  field.type = Const.DataTypes[(int)fields[i].Type];
                  field.length = (byte)fields[i].Length;
                  field.deccount = (byte)fields[i].DecCount;
                  byte[] bytes = Utility.StructureToPtr<DBF_FILEFIELD>(field);
                  m_writer.Write(bytes);
               }
            }
         }
         return ok;
      }

      public void Close()
      {
         Detach().Close();
      }

      private byte[] _Record = null;
      private int _Position = -1;
      public int Position
      {
         get { return _Position; }
         set
         {
            if (_Position != value)
            {
               m_stream.Seek(HeaderLength + value * RecordLength + Const.FIELDDATAOFFSET, io.SeekOrigin.Begin);
               _Record = m_reader.ReadBytes(RecordLength);
               _Position = value;
            }
         }
      }

      public int Recordcount { get; private set; }
      public int RecordLength { get; private set; }
      private int HeaderLength { get; set; }

      private static char[] FieldTrim = new char[] { ' ', '\0' };

      public string GetField(int index)
      {
         string str = string.Empty;
         DBF_FIELD_DATA field;
         int pos = 0;
         for (int i = 0; i < index; i++)
         {
            field = _Fields[i];
            pos += field.Length;
         }
         field = _Fields[index];
         /*
         byte[] bytes = new byte[field.Length + 1];
         Buffer.BlockCopy(_Record, pos, bytes, 0, field.Length); // faster than Array.Copy
         bytes[field.Length] = 0;
         str = System.Text.Encoding.Default.GetString(bytes);
         */
         str = System.Text.Encoding.Default.GetString(_Record, pos, field.Length);
         str = str.TrimEnd(FieldTrim);
         /*
         switch (field.Type)
         {
            case DataType.DBF_DATA_TYPE_CHAR:
               str = System.Text.Encoding.Default.GetString(_Record, pos, field.Length);
               break;
            case DataType.DBF_DATA_TYPE_INTEGER:

               break;
            default:
               Console.Write("");
               break;
         }
         */
         return str;
      }
   }
}
