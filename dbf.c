/* dbf.c */
/* Copyright (c) 2007-2009 by Troels K. All rights reserved. */
/* License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt */
/* Partially based on MFC source code by www.pablosoftwaresolutions.com 2002   */
/* Partially based on Turbo C source code by Mark Sadler.      */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include "bool.h"
#include "dbf.h"
#include "ioapi/zlib.h"
#include "ioapi/ioapi.h"
#include "minmax.h"
#include "stdint-ms.h"

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define snprintf _snprintf
#endif
#ifndef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif
#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof((array)[0]))
#endif
#define C_ASSERT_(n,e) typedef char __C_ASSERT__##n[(e)?1:-1]
#ifndef offsetof
#define offsetof(s,m)   (size_t)&(((s *)0)->m)
#endif

#define CPM_TEXT_TERMINATOR 0x1A
#define MAGIC_DBASE3      0x03
#define MAGIC_DBASE3_MEMO 0x83
#define MAGIC_DBASE4      0x04
#define MAGIC_DBASE4_MEMO 0x8B
#define MAGIC_FOXPRO      0x30

#define FIELD_REC_LENGTH  32         /*   length of field description record   */
#define HEADER_LENGTH     32         /*   length of header without field desc   */
#define MEMO_BLOCK_SIZE   512         /*  memo block size (dBase III) */
#define MAGIC_MEMO_BLOCK 0x0008FFFF

static void  strcpy_dos2host(char* buf, const char* src, size_t buf_len, enum dbf_charconv);
static void  strcpy_host2dos(char* buf, const char* src, size_t buf_len, enum dbf_charconv);
static char* strdup_host2dos(const char* src, size_t len, enum dbf_charconv, char* dup);

#pragma pack(1)
typedef struct _DBF_FILEHEADER_TIME
{
   uint8_t yy;
   uint8_t mm;
   uint8_t dd;
} DBF_FILEHEADER_TIME;

typedef struct _DBF_FILEHEADER
{
   uint8_t  version;
   DBF_FILEHEADER_TIME lastupdate;
   uint32_t recordcount;
   uint16_t   headerlength;
   uint16_t   recordlength;

   uint16_t unused_0;
   uint8_t  incomplete;
   uint8_t  crypt;

   uint8_t  unused[16];
} DBF_FILEHEADER;

typedef struct _DBT_FILEHEADER
{
   uint32_t next;
   uint8_t  unused0[4];
   char     title[8];
   uint8_t  flag;
   uint8_t  unused1[3];
   uint16_t blocksize;

   uint8_t  unused2[MEMO_BLOCK_SIZE-22];
} DBT_FILEHEADER;

typedef struct _DBF_FILEFIELD
{
   char      name[11];   // field name in ASCII zero-filled
   char      type;         // field type in ASCII
   uint8_t   unused_0[4];
   uint8_t   length;     // field length in binary
   uint8_t   deccount;   // field decimal count in binary
   uint8_t   unused_1[14];
} DBF_FILEFIELD;

typedef union _DBF_MEMO_BLOCK
{
   struct _NORMAL
   {
      uint32_t reserved; /* MAGIC_MEMO_BLOCK */
      uint32_t len;
      char     text[MEMO_BLOCK_SIZE-8];
   } normal;
   struct _HEADERLESS
   {
      char     text[MEMO_BLOCK_SIZE];
   } headerless;
} DBF_MEMO_BLOCK;

#pragma pack()

C_ASSERT_(1, sizeof(DBF_MEMO_BLOCK) == MEMO_BLOCK_SIZE );
C_ASSERT_(2, sizeof(DBF_FILEHEADER) == HEADER_LENGTH);
C_ASSERT_(3, sizeof(DBT_FILEHEADER) == MEMO_BLOCK_SIZE );
C_ASSERT_(4, sizeof(DBF_FILEFIELD ) == FIELD_REC_LENGTH);

typedef struct _DBF_MEMO_DATA
{
   void* stream;
   size_t nextfreeblock;
   DBF_MEMO_BLOCK block;
   DBT_FILEHEADER header;
} DBF_MEMO_DATA;

typedef struct _DBF_DATA
{
   size_t currentrecord;         // current record in memory
   enum dbf_charconv charconv;
   BOOL editable;

   void* stream;
   zlib_filefunc_def api;

   uint8_t version;
   size_t recordcount;
   size_t recordlength;
   size_t headerlength;
   time_t lastupdate;

   BOOL modified;            // has database contents changed?
   struct _DBF_FIELD_DATA* fieldarray;         // linked list with field structure
   size_t fieldcount;         // number of fields
   char* recorddataptr;         // pointer to current record struct

   DBF_MEMO_DATA memo;

   int  lasterror;
   char lasterrormsg[NAME_MAX];

   char* dup;
} DBF_DATA;

typedef struct _DBF_FIELD_DATA
{
   char    m_Name[12];
   enum dbf_data_type type;
   size_t m_Length;
   size_t m_DecCount;
   char*   ptr;
   uint32_t namehash;
} DBF_FIELD_DATA;

static uint32_t strhash(const char* str, BOOL case_sensitive)
{
   uint32_t hash = 5381;

   for (; *str; str++)
   {
      int c = case_sensitive ? *str : toupper(*str);
      hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
   }
   return hash;
}

int dbf_getlasterror(DBF_HANDLE handle)
{
   return handle->lasterror;
}

const char* dbf_getlasterror_str(DBF_HANDLE handle)
{
   return handle->lasterrormsg;
}

static BOOL dbf_memo_attach(DBF_HANDLE handle, void* stream)
{
   BOOL ok = (stream != NULL);

   if (ok)
   {
      handle->memo.stream = stream;
      ZSEEK(handle->api, stream, 0, ZLIB_FILEFUNC_SEEK_SET);
      ZREAD(handle->api, stream, &handle->memo.header, sizeof(handle->memo.header));
      if (0 == handle->memo.header.blocksize) handle->memo.header.blocksize = MEMO_BLOCK_SIZE; /* OpenOffice2 */
      //handle->memo.dataptr = (char*)realloc(handle->memo.dataptr, handle->memo.blocksize);
   }
   return ok;
}

EXTERN_C DBF_HANDLE dbf_alloc(void)
{
   DBF_DATA* handle = (DBF_DATA*)malloc(sizeof(DBF_DATA));

   handle->stream          = NULL;
   handle->fieldcount      = 0;
   handle->memo.nextfreeblock = 1;
   handle->memo.stream     = NULL;
   //handle->memo.dataptr    = NULL;

   memset(&handle->memo.header, 0, sizeof(handle->memo.header));
   handle->memo.header.blocksize  = MEMO_BLOCK_SIZE;
   handle->memo.header.flag = 0x03;
   handle->memo.header.next = 1;

   handle->fieldarray     = NULL;
   handle->modified        = FALSE;
   handle->recorddataptr   = NULL;
   handle->editable        = FALSE;
   handle->currentrecord   = 0;
   handle->lastupdate      = time(NULL);
   handle->recordcount     = 0;
   handle->recordlength    = 0;
   handle->headerlength    = 0;
   handle->lasterror       = DBASE_SUCCESS;
   handle->version         = MAGIC_DBASE3;
   handle->dup             = NULL;
   *handle->lasterrormsg   = 0;
   fill_fopen_filefunc(&handle->api);
   handle->charconv = ENUM_dbf_charconv_compatible;
   return handle;
}

static BOOL sanity_check(const DBF_FILEHEADER* header)
{
   BOOL ok =    ((header->lastupdate.mm >= 1) && (header->lastupdate.mm <= 12))
             && ((header->lastupdate.dd >= 1) && (header->lastupdate.dd <= 31))
             ;
   return ok;
}

DBF_HANDLE dbf_attach(void* stream, zlib_filefunc_def* api, BOOL editable, enum dbf_charconv charconv, void* memo)
{
   DBF_HANDLE handle = NULL;
   size_t len = 0;
   DBF_FILEHEADER header;

   ZSEEK(*api, stream, 0, ZLIB_FILEFUNC_SEEK_SET);
   len+=ZREAD(*api, stream, &header, sizeof(header));

   switch (header.version)
   {
      case MAGIC_DBASE3:
      case MAGIC_DBASE3_MEMO:
      case MAGIC_DBASE4:
      case MAGIC_DBASE4_MEMO:
      case MAGIC_FOXPRO:
         // sanity check
         if (sanity_check(&header) && (len == sizeof(header)))
         {
            //const BOOL v3 = (header.version == DBASE3) || (header.version == DBASE3_MEMO);
            //const BOOL v4 = (header.version == DBASE4) || (header.version == DBASE4_MEMO);

            struct tm tm;
            handle = dbf_alloc();
            handle->stream       = stream;
            handle->api          = *api;
            handle->version      = header.version;
            handle->recordcount  = header.recordcount;
            handle->recordlength = header.recordlength;
            handle->headerlength = header.headerlength;

            if (handle->recordcount == 0)
            {
               ZSEEK(handle->api, stream, 0, ZLIB_FILEFUNC_SEEK_END);
               handle->recordcount = (size_t)(ZTELL(handle->api, stream) - header.headerlength) / header.recordlength;
            }

            tm.tm_sec   = 0;
            tm.tm_min   = 0;
            tm.tm_hour  = 0;
            tm.tm_mday  = header.lastupdate.dd;
            tm.tm_mon   = header.lastupdate.mm - 1;
            tm.tm_year  = header.lastupdate.yy;
            tm.tm_wday  = 0;
            tm.tm_yday  = 0;
            tm.tm_isdst = 0;

            handle->lastupdate = mktime(&tm);
            handle->fieldcount = (uint8_t)((handle->headerlength - (FIELD_REC_LENGTH+1)) / HEADER_LENGTH);
            handle->recorddataptr = (char*)malloc(handle->recordlength + 1); // + zeroterm.
            if (handle->recorddataptr)
            {
               size_t i;
               size_t fielddata_pos = 1;//v4 ? 1 : 1;

               ZSEEK(handle->api, stream, HEADER_LENGTH, ZLIB_FILEFUNC_SEEK_SET);

               handle->fieldarray = (DBF_FIELD*)realloc(handle->fieldarray, sizeof(DBF_FIELD) * handle->fieldcount);
               for (i = 0; i < handle->fieldcount; i++)
               {
                  DBF_FILEFIELD temp;
                  DBF_FIELD* field = handle->fieldarray + i;

                  ZREAD(handle->api, stream, &temp, sizeof(temp));

                  if (*temp.name <= ' ')
                  {
                     handle->fieldcount = i;
                     break;
                  }
                  strncpy(field->m_Name, temp.name, _countof(field->m_Name)-1);
                  field->m_Name[_countof(field->m_Name)-1] = 0;
                  field->type       = temp.deccount ? DBF_DATA_TYPE_FLOAT : dbf_gettype_int2ext(temp.type);
                  field->m_Length   = temp.length  ;
                  field->m_DecCount = temp.deccount;
                  field->ptr        = handle->recorddataptr + fielddata_pos;
                  field->namehash   = strhash(field->m_Name, FALSE);

                  fielddata_pos += field->m_Length;// + (v4 ? 1 : 0) + ((v4 && (temp.type == 'C')) ? 1 : 0);
               }
               handle->modified = FALSE;
               handle->editable = editable;
               handle->charconv = charconv;

               switch (header.version)
               {
                  case MAGIC_DBASE3_MEMO:
                  case MAGIC_DBASE4_MEMO:
                     dbf_memo_attach(handle, memo);
                     break;
                  default:
                     break;
               }

            }
            else
            {
               strncpy(handle->lasterrormsg, "Out of memory", _countof(handle->lasterrormsg));
               handle->lasterror = DBASE_OUT_OF_MEM;
               dbf_detach(&handle);
            }
         }
         break;
      //case DBASE4_MEMO:
      default:
         break;
   }
   return handle;
}

void dbf_close_memo(DBF_HANDLE handle)
{
   ZCLOSE(handle->api, handle->memo.stream);
   handle->memo.stream = NULL;
   //free(handle->memo.dataptr);
   //handle->memo.dataptr = NULL;
}

void* dbf_detach(DBF_HANDLE* handle_ptr)
{
   DBF_DATA* handle = *handle_ptr;
   void* stream = handle->stream;

   if (handle->modified)
   {
      const time_t now     = time(NULL);
      const struct tm* ptm = localtime(&now);

      DBF_FILEHEADER header;
      memset(&header, 0, sizeof(header));
      header.version = handle->version;
      header.lastupdate.dd = (uint8_t) ptm->tm_mday;
      header.lastupdate.mm = (uint8_t)(ptm->tm_mon+1);
      header.lastupdate.yy = (uint8_t) ptm->tm_year;
      header.recordcount   = (uint16_t)handle->recordcount;
      header.headerlength  = (uint16_t)handle->headerlength;
      header.recordlength  = (uint16_t)handle->recordlength;

      // set file pointer to begin of file
      ZSEEK(handle->api, handle->stream, 0, ZLIB_FILEFUNC_SEEK_SET);

      ZWRITE(handle->api, handle->stream, &header, sizeof(header));

      // set pointer to end of file
      ZSEEK(handle->api, handle->stream, 0, ZLIB_FILEFUNC_SEEK_END);
      // write eof character
//      ZWRITE(m_api, handle->stream, "\0x1a", 1);
   }

   if (handle->memo.stream) dbf_close_memo(handle);

   free(handle->dup);
   free(handle->fieldarray);
   free(handle->recorddataptr);

   free(*handle_ptr);
   *handle_ptr = NULL;
   return stream;
}

void dbf_close(DBF_HANDLE* handle)
{
   zlib_filefunc_def api = (*handle)->api;
   void* stream = dbf_detach(handle);

   if (stream) ZCLOSE(api, stream);
}

void dbf_getmemofilename(const char* file_dbf, char* buf, size_t buf_len)
{
   char name[PATH_MAX];
   size_t len;

   strncpy(name, file_dbf, sizeof(name));
   len = strlen(name)-1;
   if (len) switch(name[len])
   {
      case 'F':
         name[len] = 'T';
         break;
      default:
      case 'f':
         name[len] = 't';
         break;
   }
   strncpy(buf, name, buf_len);
}

DBF_HANDLE dbf_open(const char* file, zlib_filefunc_def* api, BOOL editable, enum dbf_charconv charconv)
{
   DBF_HANDLE handle = NULL;
   void* stream = NULL;
   zlib_filefunc_def temp;

   if (api == NULL)
   {
      fill_fopen_filefunc(&temp);
      api = &temp;
   }
   stream = (*api->zopen_file)(api->opaque, file, ZLIB_FILEFUNC_MODE_EXISTING | (editable ? ZLIB_FILEFUNC_MODE_WRITE : ZLIB_FILEFUNC_MODE_READ));
   if (stream)
   {
      void* memostream = NULL;
      char temp[PATH_MAX];

      dbf_getmemofilename(file, temp, _countof(temp));
      memostream = (*api->zopen_file)(api->opaque, temp, ZLIB_FILEFUNC_MODE_EXISTING | ZLIB_FILEFUNC_MODE_WRITE);
      handle = dbf_attach(stream, api, editable, charconv, memostream);
      if (handle)
      {
         switch (handle->version)
         {
            case MAGIC_DBASE3_MEMO:
            case MAGIC_DBASE4_MEMO:
               break;
            default:
               if (memostream) ZCLOSE(*api, memostream);
               break;
         }
      }
      else
      {
         ZCLOSE(*api, stream);
         if (memostream) ZCLOSE(*api, memostream);
      }
   }
   else
   {
       //strncpy(handle->lasterrormsg, "Unable to open file", _countof(handle->lasterrormsg));
      //handle->lasterror = DBASE_NO_FILE;
   }
   return handle;
}

size_t dbf_getposition(DBF_HANDLE handle)
{
   return handle->currentrecord;
}

size_t dbf_getrecordcount(DBF_HANDLE handle)
{
   return handle->recordcount;
}

size_t dbf_getfieldcount(DBF_HANDLE handle)
{
   return handle->fieldcount;
}

/* Remove all trailing and leading spaces.                  */
static char* Trim(char *str, char trimchar)
{
   char* end;

   while (*str == trimchar)
   {
      strcpy(str, str + 1);
   }
   end = str + strlen(str) - 1;
   while ( (end >= str) && (*end == trimchar) )
   {
      *(end--) = 0;
   }
   return str;
}

time_t dbf_getlastupdate(DBF_HANDLE handle)
{
   return handle->lastupdate;
}

BOOL dbf_isvaliddate(const char *buf)
{
   int year, month, day;
   BOOL ok = (3 == sscanf(buf,"%04d%02d%02d", &year, &month, &day));

   if (ok)
   {
      ok = (month >= 1) && (month <= 12) && (day >= 1) && (day <= 31);
   }
   return ok;
}

BOOL dbf_setposition(DBF_HANDLE handle, size_t record)
{
   BOOL ok = (record < handle->recordcount);

   if (ok)
   {
      ok = (0 == ZSEEK(handle->api, handle->stream, handle->headerlength + (record - 0) * handle->recordlength, ZLIB_FILEFUNC_SEEK_SET));
      if (ok) ok = (handle->recordlength == ZREAD(handle->api, handle->stream, handle->recorddataptr, handle->recordlength));
      if (ok)
      {
         handle->currentrecord = record;
         handle->lasterror = DBASE_SUCCESS;
      }
      else
      {
         //DWORD dw = GetLastError();
         strncpy(handle->lasterrormsg, "Invalid record", _countof(handle->lasterrormsg));
         handle->lasterror = DBASE_INVALID_RECORD;
      }
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid record", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_RECORD;
   }
   return ok;
}

BOOL dbf_putrecord(DBF_HANDLE handle, size_t record)
{
   BOOL ok = (record < handle->recordcount);

   if (ok)
   {
      ZSEEK (handle->api, handle->stream, handle->headerlength + (record - 0) * handle->recordlength, ZLIB_FILEFUNC_SEEK_SET);
      ZWRITE(handle->api, handle->stream, handle->recorddataptr, handle->recordlength);
      handle->modified = TRUE;
      handle->currentrecord = record;
      handle->lasterror = DBASE_SUCCESS;
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid record", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_RECORD;
   }
   return ok;
}

BOOL dbf_isrecorddeleted(DBF_HANDLE handle)
{
   return (*handle->recorddataptr == '*');
}

BOOL dbf_deleterecord(DBF_HANDLE handle, BOOL do_delete)
{
   BOOL ok = (NULL != handle->recorddataptr);

   if (ok)
   {
      *handle->recorddataptr = (char)(do_delete ? '*' : ' ');

      handle->modified = TRUE;
      ok = dbf_putrecord(handle, dbf_getposition(handle));
   }
   else
   {
      handle->lasterror = DBASE_INVALID_RECORD;
   }
   return ok;
}

BOOL dbf_addrecord(DBF_HANDLE handle)
{
   BOOL ok;
   memset(handle->recorddataptr, ' ', handle->recordlength);
   ZSEEK(handle->api, handle->stream, handle->headerlength + handle->recordcount * handle->recordlength, ZLIB_FILEFUNC_SEEK_SET);
   ok = (handle->recordlength == ZWRITE(handle->api, handle->stream, handle->recorddataptr, handle->recordlength));
   if (ok)
   {
      handle->currentrecord = handle->recordcount++;
      handle->modified = TRUE;
      handle->lasterror = DBASE_SUCCESS;
   }
   else
   {
      strncpy(handle->lasterrormsg, "Error while adding new record", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_RECORD;
   }
   return ok;
}

BOOL dbf_insertrecord(DBF_HANDLE handle, size_t index)
{
   char buf[255];

   BOOL ok = (index <= handle->recordcount);
   if (ok)
   {
      size_t i, j, total = handle->recordcount+1;

      /* add new record at the end of file */
      dbf_addrecord(handle);

      /* move all records one place */
      for (i = total; i > index; i--)
      {
          for (j = 0; j < handle->fieldcount; j++)
          {
            const DBF_FIELD* field;
            dbf_setposition(handle, i-1);

            field = dbf_getfieldptr(handle, j);
            dbf_getfield(handle, field, buf, sizeof(buf), DBF_DATA_TYPE_ANY);
            dbf_setposition(handle, i);
            dbf_putfield(handle, field, buf);
            dbf_putrecord(handle, i);
          }
      }
      handle->modified = TRUE;
      handle->lasterror = DBASE_SUCCESS;
   }
   else
   {
      handle->lasterror = DBASE_INVALID_RECORD;
   }
   return ok;
}

const DBF_FIELD* dbf_getfieldptr(DBF_HANDLE handle, size_t index)
{
   const DBF_FIELD* field = NULL;

   if (index < handle->fieldcount)
   {
      field = handle->fieldarray + index;
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid field pointer", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_FIELDNO;
   }
   return field;
}

int dbf_findfield(DBF_HANDLE handle, const char* fieldname)
{
   size_t i;
   const uint32_t namehash = strhash(fieldname, FALSE);

   for (i = 0; i < handle->fieldcount; i++)
   {
      const DBF_FIELD* temp = handle->fieldarray + i;
      if (   (namehash == temp->namehash)
          && (0 == strcasecmp(temp->m_Name, fieldname)))
      {
         return (int)i;
      }
   }
   strncpy(handle->lasterrormsg, "Invalid field pointer", _countof(handle->lasterrormsg));
   handle->lasterror = DBASE_INVALID_FIELDNO;
   return -1;
}

const DBF_FIELD* dbf_getfieldptr_name(DBF_HANDLE handle, const char* fieldname)
{
   const int index = dbf_findfield(handle, fieldname);

   return (index != -1) ? (handle->fieldarray + index) : NULL;
}

BOOL dbf_isnull(DBF_HANDLE handle, const DBF_FIELD* field)
{
   size_t i;

   for (i = 0; i < field->m_Length; i++)
   {
      if (field->ptr[i] != ' ') return FALSE;
   }
   return TRUE;
}

static int dotnormalize(char* str, char dot, size_t len)
{
   size_t i;

   if (0 == len) len = strlen(str);
   for (i = 0; i < len; i++) switch (str[i])
   {
      case ',':
      case '.':
         str[i] = (char)(dot ? dot : *localeconv()->decimal_point);
         return (int)i;
   }
   return -1;
}

BOOL dbf_putfield(DBF_HANDLE handle, const DBF_FIELD* field, const char* buf)
{
   BOOL ok = field && handle->editable;

   if (ok)
   {
      const size_t buf_len = strlen(buf);
      char* dup = handle->dup = strdup_host2dos(buf, buf_len, handle->charconv, handle->dup);
      // check for correct type
      switch (field->type)
      {
         case DBF_DATA_TYPE_FLOAT:
         case DBF_DATA_TYPE_INTEGER:
         //case DBF_DATA_TYPE_MEMO:
         {
            char* ptr = dup;
            Trim(ptr, ' ');
            while (*ptr)
            {
               if (strchr("+-.,0123456789", *ptr))
               {
                  ptr++;
               }
               else
               {
                  strncpy(handle->lasterrormsg, "Invalid type (not a FLOAT/NUMERIC)", _countof(handle->lasterrormsg));
                  handle->lasterror = DBASE_INVALID_DATA;
                  ok = FALSE;
                  break;
               }
            }
            break;
         }
         case DBF_DATA_TYPE_DATE:
            if(!dbf_isvaliddate(dup))
            {
               strncpy(handle->lasterrormsg, "Invalid type (not a DATE)", _countof(handle->lasterrormsg));
               handle->lasterror = DBASE_INVALID_DATA;
               ok = FALSE;
            }
            break;
         case DBF_DATA_TYPE_BOOLEAN:
            if (strchr("YyNnFfTt?", *dup))
            {
            }
            else
            {
               strncpy(handle->lasterrormsg, "Invalid type (not a LOGICAL)", _countof(handle->lasterrormsg));
               handle->lasterror = DBASE_INVALID_DATA;
               ok = FALSE;
            }
            break;
      }
      if (ok)
      {
         switch (field->type)
         {
            case DBF_DATA_TYPE_INTEGER:
               snprintf(dup, buf_len + 1, "%d", atoi(dup)); // normalize
               memset(field->ptr, ' ', field->m_Length);
               strncpy(field->ptr, dup, field->m_Length);
               break;
            case DBF_DATA_TYPE_MEMO:
            {
               const int n = atoi(field->ptr);
               size_t block = (size_t)n;
               ok = (NULL != handle->memo.stream);
               if (ok) switch (n)
               {
                  case -1:
                     break;
                  case 0:
                     block = handle->memo.header.next;
                     if (0 == block)
                     {
                        size_t filelen;

                        ZSEEK(handle->api, handle->memo.stream, 0, ZLIB_FILEFUNC_SEEK_END);
                        filelen = (size_t)ZTELL(handle->api, handle->memo.stream);
                        block =  (filelen / handle->memo.header.blocksize)
                              + ((filelen % handle->memo.header.blocksize) ? 1 : 0);
                     }
                     handle->memo.header.next = block + 1;
                     ZSEEK (handle->api, handle->memo.stream, 0, ZLIB_FILEFUNC_SEEK_SET);
                     ZWRITE(handle->api, handle->memo.stream, &handle->memo.header, sizeof(handle->memo.header));
                     // fall through
                  default:
                  {
                     DBF_MEMO_BLOCK* temp = &handle->memo.block;
                     char* text = temp->normal.text;

                     ZSEEK(handle->api, handle->memo.stream, block * handle->memo.header.blocksize, ZLIB_FILEFUNC_SEEK_SET);
                     temp->normal.len = min(handle->memo.header.blocksize - offsetof(DBF_MEMO_BLOCK, normal.text), buf_len);
                     temp->normal.reserved = MAGIC_MEMO_BLOCK;
                     memset(text, ' ', temp->normal.len);
                     strncpy(text, dup, temp->normal.len);
                     ok = (sizeof(*temp) == ZWRITE(handle->api, handle->memo.stream, temp, sizeof(*temp)));
                     if (ok)
                     {
                        memset(field->ptr, ' ', field->m_Length);
                        snprintf(field->ptr, field->m_Length, "%08d", (int)block);
                     }
                     break;
                  }
               }
               break;
            }
            case DBF_DATA_TYPE_FLOAT:
            {
               int sep;
               size_t i;

               dotnormalize(dup, 0, buf_len);
               i = snprintf(field->ptr, field->m_Length, "%f", atof(dup));
               sep = dotnormalize(field->ptr, '.', field->m_Length);
               if (sep != -1)
               {
                  int pad = field->m_DecCount - (i - sep);
                  for (; (pad >= 0) && (i < field->m_Length); i++, pad--)
                  {
                     field->ptr[i] = '0';
                  }
               }

               for (; i < field->m_Length; i++)
               {
                  field->ptr[i] = ' ';
               }
               break;
            }
            default:
            {
               size_t nLength = min(buf_len, field->m_Length);
               memset(field->ptr, ' ', field->m_Length);
               strncpy(field->ptr, dup, nLength);
               break;
            }
         }
      }
   }
   else
   {
      strncpy(handle->lasterrormsg, "Not a valid field index", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_FIELDNO;
   }
   return ok;
}

BOOL dbf_putfield_bool(DBF_HANDLE handle, const DBF_FIELD* field, BOOL b)
{
   return dbf_putfield(handle, field, b ? "T" : "F");
}

BOOL dbf_move_prepare(DBF_HANDLE handle)
{
   BOOL ok = TRUE;

   if (handle->modified)
   {
      //ok = dbf_putrecord(handle, handle->currentrecord);
   }
   return ok;
}

static BOOL dbf_getfield_date(DBF_HANDLE handle, const DBF_FIELD* field, int* year, int* month, int* mday)
{
   BOOL ok = field && (field->type == DBF_DATA_TYPE_DATE);

   if (ok)
   {
      long temp;
      ok = dbf_getfield_numeric(handle, field, &temp);
      if (ok) ok = (temp != 0);
      if (ok)
      {
         if (mday ) *mday  =  (temp / 1    ) % 100;
         if (month) *month = ((temp / 100  ) % 100) - 1;
         if (year ) *year  =  (temp / 10000);
      }
   }
   return ok;
}

BOOL dbf_getfield_time(DBF_HANDLE handle, const DBF_FIELD* field, time_t* utc_ptr, int* ms_ptr)
{
   BOOL ok = (field != NULL);

   if (ok)
   {
      switch (field->type)
      {
         case DBF_DATA_TYPE_DATE:
         {
            int year, month, mday;
            ok = dbf_getfield_date(handle, field, &year, &month, &mday);
            if (ok)
            {
               struct tm tm;
               time_t temp;

               tm.tm_sec = 0;
               tm.tm_min = 0;
               tm.tm_hour = 0;
               tm.tm_mday = mday;
               tm.tm_mon  = month;
               tm.tm_year = year - 1900;
               tm.tm_wday = 0;
               tm.tm_yday = 0;
               tm.tm_isdst = 0;

               temp = mktime(&tm);
               ok = (-1 != temp);
               if (ok)
               {
                  if (utc_ptr) *utc_ptr = temp;
                  if (ms_ptr ) *ms_ptr  = 0;
               }
            }
            break;
         }
         case DBF_DATA_TYPE_INTEGER:
         case DBF_DATA_TYPE_MEMO:
         {
            long temp;
            ok = dbf_getfield_numeric(handle, field, &temp);
            if (ok)
            {
               if (utc_ptr) *utc_ptr = temp;
               if (ms_ptr ) *ms_ptr  = 0;
            }
            break;
         }
         case DBF_DATA_TYPE_FLOAT:
         {
            double temp;
            ok = dbf_getfield_float(handle, field, &temp);
            if (ok)
            {
               if (utc_ptr) *utc_ptr = (int)floor(temp);
               if (ms_ptr ) *ms_ptr  = (int)((temp - floor(temp)) * 1000.0);
            }
            break;
         }
         default:
            ok = FALSE;
            break;
      }
   }
   return ok;
}

BOOL dbf_putfield_time(DBF_HANDLE handle, const DBF_FIELD* field, time_t utc, int ms)
{
   BOOL ok = (field != NULL);

   if (ok) switch (field->type)
   {
      case DBF_DATA_TYPE_DATE:
      {
         struct tm* tm = localtime(&utc);
         ok = (tm != NULL);
         if (ok) ok = dbf_putfield_tm(handle, field, tm, ms);
         break;
      }
      case DBF_DATA_TYPE_INTEGER:
      case DBF_DATA_TYPE_MEMO:
         ok = dbf_putfield_numeric(handle, field, (long)utc);
         break;
      case DBF_DATA_TYPE_FLOAT:
         ok = dbf_putfield_float(handle, field, ((double)utc) + ((double)ms)/1000);
         break;
      default:
         ok = FALSE;
         break;
   }
   return ok;
}

BOOL dbf_getfield_tm(DBF_HANDLE handle, const DBF_FIELD* field, struct tm* tm, int* ms)
{
   BOOL ok = (field != NULL);

   if (ok)
   {
      switch (field->type)
      {
         case DBF_DATA_TYPE_DATE:
         {
            int year, month, mday;
            ok = dbf_getfield_date(handle, field, &year, &month, &mday);
            if (ok)
            {
               if (tm)
               {
                  tm->tm_sec = 0;
                  tm->tm_min = 0;
                  tm->tm_hour = 0;
                  tm->tm_mday = mday;
                  tm->tm_mon  = month;
                  tm->tm_year = year - 1900;
                  tm->tm_wday = 0;
                  tm->tm_yday = 0;
                  tm->tm_isdst = 0;
               }
               if (ms) *ms = 0;
            }
            break;
         }
         case DBF_DATA_TYPE_INTEGER:
         case DBF_DATA_TYPE_MEMO:
         case DBF_DATA_TYPE_FLOAT:
         {
            time_t utc;
            ok = dbf_getfield_time(handle, field, &utc, ms);
            if (ok)
            {
               const struct tm* temp = localtime(&utc);
               ok = (temp != NULL);
               if (ok)
               {
                  if (tm) *tm = *temp;
                  if (ms) *ms = 0;
               }
            }
            break;
         }
         default:
            ok = FALSE;
            break;
      }
   }
   return ok;
}

BOOL dbf_putfield_tm(DBF_HANDLE handle, const DBF_FIELD* field, const struct tm* tm, int ms)
{
   BOOL ok = tm && field;

   if (ok) switch (field->type)
   {
      case DBF_DATA_TYPE_DATE:
      {
         char sz[80];
         snprintf(sz, sizeof(sz), "%04d%02d%02d",
            tm->tm_year + ((tm->tm_year < 1900) ? 1900 : 0),
            tm->tm_mon + 1,
            tm->tm_mday);
         ok = dbf_putfield(handle, field, sz);
         break;
      }
      case DBF_DATA_TYPE_TIME: /* non-standard */
      {
         char sz[80];
         snprintf(sz, sizeof(sz), "%02d%02d%02d%03d",
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec,
            ms % 1000);
         ok = dbf_putfield(handle, field, sz);
         break;
      }
      case DBF_DATA_TYPE_DATETIME: /* non-standard */
      {
         char sz[80];
         snprintf(sz, sizeof(sz), "%04d%02d%02d%02d%02d%02d%03d",
            tm->tm_year + ((tm->tm_year < 1900) ? 1900 : 0),
            tm->tm_mon + 1,
            tm->tm_mday,
            tm->tm_hour,
            tm->tm_min,
            tm->tm_sec,
            ms % 1000);
         ok = dbf_putfield(handle, field, sz);
         break;
      }
      case DBF_DATA_TYPE_INTEGER:
      case DBF_DATA_TYPE_MEMO:
      case DBF_DATA_TYPE_FLOAT:
      {
         const time_t utc = mktime((struct tm*)tm); // unconst due to bad prototype in MSVC7
         ok = (-1 != utc);
         if (ok) ok = dbf_putfield_time(handle, field, utc, ms);
         break;
      }
      default:
         ok = FALSE;
         break;
   }
   return ok;
}

BOOL dbf_getfield_float(DBF_HANDLE handle, const DBF_FIELD* field, double* d)
{
   char buf[21];
   BOOL ok = dbf_getfield(handle, field, buf, sizeof(buf), DBF_DATA_TYPE_FLOAT);

   if (ok)
   {
      dotnormalize(buf, 0, 0);
      if (d) *d = atof(buf);
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid type (not a FLOAT)", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_BAD_FORMAT;
   }
   return ok;
}

BOOL dbf_getfield_bool(DBF_HANDLE handle, const DBF_FIELD* field, BOOL* b)
{
   char buf;
   BOOL ok = dbf_getfield(handle, field, &buf, sizeof(buf), DBF_DATA_TYPE_BOOLEAN);

   if (ok)
   {
      if(b) *b = (strchr("YyTt", buf) != NULL);
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid type (not a LOGICAL)", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_BAD_FORMAT;
   }
   return ok;
}

BOOL dbf_putfield_float(DBF_HANDLE handle, const DBF_FIELD* field, double value)
{
   BOOL ok = (field != NULL);
   if (ok)
   {
      char* buf = (char*)malloc(field->m_Length);

      snprintf(buf, field->m_Length, "%g", value);
      ok = dbf_putfield(handle, field, buf);
      free(buf);
   }
   return ok;
}

BOOL dbf_putfield_numeric(DBF_HANDLE handle, const DBF_FIELD* field, long value)
{
   char buf[19];

   snprintf(buf, _countof(buf), "%ld", value);
   return dbf_putfield(handle, field, buf);
}

static const char mod_type_int[] = "CNFDCCML";
C_ASSERT_(5, (sizeof(mod_type_int) - 1) == DBF_DATA_TYPE_ENUMCOUNT);

char dbf_gettype_ext2int(enum dbf_data_type type)
{
   char n;

   switch (type)
   {
      case DBF_DATA_TYPE_UNKNOWN:
         n = 0;
         break;
      case DBF_DATA_TYPE_ANY:
         n = '?';
         break;
      default:
         n = mod_type_int[type];
         break;
   }
   return n;
}

enum dbf_data_type dbf_gettype_int2ext(char type)
{
   const char* p = strchr(mod_type_int, type);

   return p ? (enum dbf_data_type)(p - mod_type_int) : DBF_DATA_TYPE_UNKNOWN;
}

enum dbf_data_type dbf_getfield_type(DBF_HANDLE handle, const DBF_FIELD* field)
{
   return field ? field->type : DBF_DATA_TYPE_UNKNOWN;
}

BOOL dbf_getfield_info(DBF_HANDLE handle, size_t index, DBF_FIELD_INFO* info)
{
   BOOL ok = (index < handle->fieldcount);

   if (ok && info)
   {
      const DBF_FIELD* field = handle->fieldarray + index;

      //strncpy(info->name, field->m_Name, sizeof(info->name));
      strcpy_dos2host(info->name, field->m_Name, sizeof(info->name), handle->charconv);
      info->length   = field->m_Length;
      info->type     = field->type;
      info->decimals = field->m_DecCount;
   }
   return ok;
}

const char* dbf_gettypetext(enum dbf_data_type index)
{
   const char* const asz[] =
   {
      "CHAR",
      "NUMERIC",
      "FLOAT",
      "DATE",
      "CHAR",
      "CHAR",
      "MEMO",
      "LOGICAL"
   };
   C_ASSERT_(2, _countof(asz) == DBF_DATA_TYPE_ENUMCOUNT);
   return (index < DBF_DATA_TYPE_ENUMCOUNT) ? asz[index] : NULL;
}

BOOL dbf_memo_create(DBF_HANDLE handle, void* stream)
{
   BOOL ok = dbf_memo_attach(handle, stream);

   if (ok)
   {
      size_t i;
      ZSEEK(handle->api, handle->memo.stream, 0, ZLIB_FILEFUNC_SEEK_SET);

      handle->memo.nextfreeblock = 1;

      // first 4 bytes is next block
      ZWRITE(handle->api, handle->memo.stream, &handle->memo.nextfreeblock, sizeof(handle->memo.nextfreeblock));

      // reserved space
      for (i = 4; i < 512; i++)
      {
         ZWRITE(handle->api, handle->memo.stream, "\0", 1);
      }
      ZSEEK (handle->api, handle->memo.stream, 16, ZLIB_FILEFUNC_SEEK_SET);
      ZWRITE(handle->api, handle->memo.stream, "\0x03", 1);

      //handle->memo.dataptr = (char*)malloc(handle->memo.blocksize);
      //ok = (handle->memo.dataptr != NULL);
      if (ok)
      {
         *handle->lasterrormsg = 0;
         handle->lasterror = DBASE_SUCCESS;
      }
      else
      {
         ZCLOSE(handle->api, handle->memo.stream);
         handle->memo.stream = NULL;
         strncpy(handle->lasterrormsg, "Out of memory", _countof(handle->lasterrormsg));
         handle->lasterror = DBASE_OUT_OF_MEM;
      }
   }
   else
   {
      *handle->lasterrormsg = 0;
      handle->lasterror = DBASE_NO_FILE;
   }
   return ok;
}

BOOL dbf_memo_open(DBF_HANDLE handle, void* stream)
{
   BOOL ok = dbf_memo_attach(handle, stream);

   if (ok)
   {
      ZSEEK(handle->api, handle->memo.stream, 0, ZLIB_FILEFUNC_SEEK_SET);
      ZREAD(handle->api, handle->memo.stream, &handle->memo.nextfreeblock, sizeof(handle->memo.nextfreeblock));
      //handle->memo.dataptr = (char *)malloc(handle->memo.blocksize);
      //ok = (handle->memo.dataptr != NULL);
      if (!ok)
      {
         ZCLOSE(handle->api, handle->memo.stream);
         handle->memo.stream = NULL;
           strncpy(handle->lasterrormsg, "Out of memory", _countof(handle->lasterrormsg));
         handle->lasterror = DBASE_OUT_OF_MEM;
      }
   }
   else
   {
      *handle->lasterrormsg = 0;
      handle->lasterror = DBASE_NO_FILE;
   }
   return ok;
}

static int find_memo(const DBF_FIELD_INFO* array, size_t array_count)
{
   size_t i;

   for (i = 0; i < array_count; i++)
   {
      if (array[i].type == DBF_DATA_TYPE_MEMO)
      {
         return (int)i;
      }
   }
   return -1;
}

DBF_HANDLE dbf_create(const char* filename, const DBF_FIELD_INFO* array, size_t array_count, enum dbf_charconv charconv)
{
   DBF_HANDLE handle = NULL;
   void* stream;
   void* memo = NULL;
   zlib_filefunc_def api;
   fill_fopen_filefunc(&api);
   stream = (*api.zopen_file)(api.opaque, filename, ZLIB_FILEFUNC_MODE_CREATE | ZLIB_FILEFUNC_MODE_WRITE);

   if (stream && (-1 != find_memo(array, array_count)))
   {
      char temp[PATH_MAX];
      dbf_getmemofilename(filename, temp, _countof(temp));
      memo = (*api.zopen_file)(api.opaque, temp, ZLIB_FILEFUNC_MODE_CREATE | ZLIB_FILEFUNC_MODE_WRITE);
      if (NULL == memo)
      {
         ZCLOSE(api, stream);
         stream = NULL;
      }
   }
   if (stream)
   {
      handle = dbf_create_attach(stream, &api, array, array_count, charconv, memo);
      if (handle)
      {
         dbf_close(&handle); // ioapi quirk - change mode to ZLIB_FILEFUNC_MODE_EXISTING | ZLIB_FILEFUNC_MODE_WRITE
         handle = dbf_open(filename, &api, TRUE, charconv);
      }
      else
      {
         ZCLOSE(api, stream);
      }
   }
   return handle;
}

DBF_HANDLE dbf_create_attach(void* stream, zlib_filefunc_def* api,
                             const DBF_FIELD_INFO* array, size_t array_count,
                             enum dbf_charconv charconv, void* memo)
{
   DBF_HANDLE handle = NULL;
   const time_t now = time(NULL);
   const struct tm* ptm = localtime(&now);
   size_t i;
   DBF_FILEHEADER header;
   DBF_FIELD* fieldarray;
   char* recorddataptr;

   memset(&header, 0, sizeof(header));
   header.version = (uint8_t)(memo ? MAGIC_DBASE3_MEMO : MAGIC_DBASE3);
   header.lastupdate.dd = (uint8_t)(ptm->tm_mday );
   header.lastupdate.mm = (uint8_t)(ptm->tm_mon+1);
   header.lastupdate.yy = (uint8_t)(ptm->tm_year );
   header.recordcount   = 0;
   header.headerlength  = 0;
   header.recordlength  = 1;

   fieldarray = (DBF_FIELD*)malloc(array_count*sizeof(DBF_FIELD));
   for (i = 0; i < array_count; i++)
   {
      const DBF_FIELD_INFO* src = array + i;
            DBF_FIELD     * dst = fieldarray + i;
      const size_t len = min(src->length, UCHAR_MAX);

      //dst->index = i;
      strncpy(dst->m_Name, src->name, sizeof(dst->m_Name));
      dst->type         = src->type;
      dst->m_DecCount   = (uint8_t)src->decimals;
      dst->m_Length     = (uint8_t)len;
      dst->ptr = NULL;
      header.recordlength = (uint16_t)(header.recordlength + src->length);
   }
   recorddataptr = (char*)malloc(header.recordlength);
   header.lastupdate.dd = (uint8_t) ptm->tm_mday;
   header.lastupdate.mm = (uint8_t)(ptm->tm_mon+1);
   header.lastupdate.yy = (uint8_t) ptm->tm_year;

   ZSEEK(*api, stream, 0, ZLIB_FILEFUNC_SEEK_SET);
   ZWRITE(*api, stream, &header, sizeof(header));

   header.headerlength = (uint16_t)(HEADER_LENGTH + (array_count * FIELD_REC_LENGTH) + 1);
   header.recordlength = 1;

   for (i = 0; i < array_count; i++)
   {
      DBF_FIELD* field = fieldarray + i;
      DBF_FILEFIELD temp;

      memset(&temp, 0, sizeof(temp));

      strcpy_host2dos(temp.name, field->m_Name, sizeof(temp.name), charconv);

      //strncpy(temp.name, field->m_Name, sizeof(temp.name));

      temp.type     = dbf_gettype_ext2int(field->type);
      temp.length   = (uint8_t)field->m_Length;
      temp.deccount = (uint8_t)field->m_DecCount;
      field->ptr = recorddataptr + header.recordlength;
      field->namehash   = strhash(field->m_Name, FALSE);

      ZWRITE(*api, stream, &temp, sizeof(temp));
      header.recordlength = (uint16_t)(header.recordlength + field->m_Length);
   }
   ZWRITE(*api, stream, "\xd", 1); // crucial - reqd by OpenOffice 2
   ZSEEK (*api, stream, 0, ZLIB_FILEFUNC_SEEK_SET);
   ZWRITE(*api, stream, &header, sizeof(header));
   ZSEEK (*api, stream, 0, ZLIB_FILEFUNC_SEEK_END);
   ZWRITE(*api, stream, "\0x0d", 1);
   ZWRITE(*api, stream, "\0x1a", 1);

   handle = dbf_alloc();
   handle->api          = *api;
   handle->stream       = stream;
   handle->version      = header.version;
   handle->charconv     = charconv;
   handle->editable     = TRUE;
   handle->fieldarray   = fieldarray;
   handle->fieldcount   = array_count;
   handle->recorddataptr = recorddataptr;
   handle->recordlength = header.recordlength;
   handle->headerlength = header.headerlength;
   return handle;
}

BOOL dbf_copy(DBF_HANDLE handle,
              void* stream,
              void* stream_memo,
              zlib_filefunc_def* api,
              BOOL include_records,
              BOOL include_deleted_records)
{
   BOOL ok = TRUE;

   return ok;
}

void dbf_getfileapi(DBF_HANDLE handle, zlib_filefunc_def* api)
{
   *api = handle->api;
}

void* dbf_getmemofile(DBF_HANDLE handle)
{
   return handle->memo.stream;
}

BOOL dbf_iseditable(DBF_HANDLE handle)
{
   return handle->editable;
}

BOOL dbf_ismodified(DBF_HANDLE handle)
{
   return handle->modified;
}

BOOL dbf_getversion(DBF_HANDLE handle)
{
   return handle->version;
}

size_t dbf_getfield(DBF_HANDLE handle, const DBF_FIELD* field, char* buf, size_t buf_len, enum dbf_data_type type)
{
   BOOL trim = TRUE;
   size_t len = 0;

   if (field)
   {
      if ( (field->type == type) || (type == DBF_DATA_TYPE_ANY))
      {
         switch (field->type)
         {
            case DBF_DATA_TYPE_MEMO:
            {
               const char term = field->ptr[field->m_Length];
               int n;

               field->ptr[field->m_Length] = 0;
               n = atoi(field->ptr);
               field->ptr[field->m_Length] = term;
               //sscanf(field->ptr, "%10d", &n);
               if (handle->memo.stream && (n >= 1))
               {
                  const size_t block = (size_t)n;
                  size_t read;
                  DBF_MEMO_BLOCK* temp = &handle->memo.block;

                  //if (block > 42900000)
                  //   _asm int 3
                  ZSEEK(handle->api, handle->memo.stream, block * handle->memo.header.blocksize, ZLIB_FILEFUNC_SEEK_SET);
                  read = ZREAD(handle->api, handle->memo.stream, temp, sizeof(*temp));
                  if (read)
                  {
                     const char* text;
                     size_t i;

                     switch (temp->normal.reserved)
                     {
                        case MAGIC_MEMO_BLOCK:
                        //case MAGIC_MEMO_BLOCK:
                           text = temp->normal.text;
                           len = read - offsetof(DBF_MEMO_BLOCK, normal.text);
                           len = min(len, handle->memo.block.normal.len);
                           break;
                        default:
                           text = temp->headerless.text;
                           len = read;
                           break;
                     }
                     for (i = 0; i < len; i++) switch (text[i])
                     {
                        case CPM_TEXT_TERMINATOR:
                           //if (temp->normal.reserved == MAGIC_MEMO_BLOCK) break;
                        case 0:
                           len = i;
                           break;
                     }
                     for (; trim && len && (text[len-1] == ' '); len--)
                     {
                     }
                     if (buf)
                     {
                        len = min(len, buf_len - (buf_len ? 1 : 0));
                        strcpy_dos2host(buf, text, len, handle->charconv);
                     }
                  }
                  else
                  {
                      strncpy(handle->lasterrormsg, "An error occured while reading from memo file", _countof(handle->lasterrormsg));
                     handle->lasterror = DBASE_READ_ERROR;
                  }
               }
               else
               {
                  //len+=snprintf(buf, buf_len, "%d", n);
                  strncpy(handle->lasterrormsg, "No memo data available", _countof(handle->lasterrormsg));
                  handle->lasterror = DBASE_NO_MEMO_DATA;
               }
               break;
            }
            default:
               for (len = field->m_Length; trim && len && (field->ptr[len-1] == ' '); len--)
               {
               }
               if (buf)
               {
                  if (field->type == DBF_DATA_TYPE_FLOAT)
                  {
                     strncpy(buf, field->ptr, len);
                     dotnormalize(buf, 0, len);
                  }
                  else
                  {
                     strcpy_dos2host(buf, field->ptr, len, handle->charconv);
                  }
               }
               break;
         }
      }
      else
      {
         strncpy(handle->lasterrormsg, "Unexpected field type", _countof(handle->lasterrormsg));
         handle->lasterror = DBASE_INVALID_FIELDNO;
      }
   }
   else
   {
      strncpy(handle->lasterrormsg, "Invalid field index", _countof(handle->lasterrormsg));
      handle->lasterror = DBASE_INVALID_FIELDNO;
   }
   if (buf && (len < buf_len) && buf_len)
   {
      size_t i;

      buf[len] = 0;
      for (i = 0; len && (' ' == *buf); i++, len--)
      {
         strcpy(buf, buf + 1);
      }
   }
   return len;
}

BOOL dbf_getfield_numeric(DBF_HANDLE handle, const DBF_FIELD* field, long* n)
{
   char buf[19];
   BOOL ok =
      (   dbf_getfield(handle, field, buf, sizeof(buf), DBF_DATA_TYPE_INTEGER)
       || dbf_getfield(handle, field, buf, sizeof(buf), DBF_DATA_TYPE_DATE   ));
   if (ok)
   {
      *n = atol(buf);
   }
   else
   {
      ok = dbf_getfield(handle, field, buf, sizeof(buf), DBF_DATA_TYPE_MEMO); // get field data, check for MEMO type
      if (ok)
      {
         *n = atol(buf);
      }
      else
      {
         strncpy(handle->lasterrormsg, "Invalid field type (not a NUMERIC)", _countof(handle->lasterrormsg));
         handle->lasterror = DBASE_INVALID_FIELDNO;
      }
   }
   return ok;
}

#ifdef _WIN32
#include <windows.h> // OemToCharBuffA
#endif

static void strcpy_dos2host(char* buf, const char* src, size_t buf_len, enum dbf_charconv mode)
{
   switch (mode)
   {
      case ENUM_dbf_charconv_oem_host:
      #ifdef _WIN32
         OemToCharBuffA(src, buf, buf_len);
         break;
      #endif
      case ENUM_dbf_charconv_off:
      default:
         strncpy(buf, src, buf_len);
         break;
   }
}

static void strcpy_host2dos(char* buf, const char* src, size_t buf_len, enum dbf_charconv mode)
{
   switch (mode)
   {
      case ENUM_dbf_charconv_oem_host:
      #ifdef _WIN32
         CharToOemBuffA(src, buf, buf_len);
         break;
      #endif
      case ENUM_dbf_charconv_off:
      default:
         strncpy(buf, src, buf_len);
         break;
   }
}

static char* strdup_host2dos(const char* src, size_t len, enum dbf_charconv mode, char* dup)
{
   dup = (char*)realloc(dup, (1 + len)*sizeof(char));
   if (len) strcpy_host2dos(dup, src, len + 1, mode);
   else *dup = 0;
   return dup;
}

