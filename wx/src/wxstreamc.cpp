// streamc.cpp
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include <wx/wx.h>
#include <wx/stream.h>
#include "../../ioapi/zlib.h"
#include "../../ioapi/ioapi.h"
#include "wxstreamc.h"

static long out_fseek(
   voidpf WXUNUSED(opaque),
   voidpf stream,
   ZOFF_T offset,
   int origin)
{
   wxOutputStream* out = (wxOutputStream*)stream;
   return (wxInvalidOffset != out->SeekO(offset, (wxSeekMode)origin)) ? 0 : -1;
}

static uLong fwrite_impl(
   voidpf WXUNUSED(opaque),
   voidpf stream,
   const void* buf,
   uLong size)
{
   wxOutputStream* out = (wxOutputStream*)stream;
   return out->Write(buf, size).LastWrite();
}

static int fclose_impl(
   voidpf WXUNUSED(opaque),
   voidpf WXUNUSED(stream))
{
   return 0;
}

static ZPOS_T out_tell(voidpf WXUNUSED(opaque), voidpf stream)
{
   wxOutputStream* out = (wxOutputStream*)stream;
   return out->TellO();
}

void fill_filefunc(struct zlib_filefunc_def_s* api, wxOutputStream* WXUNUSED(stream))
{
   api->zopen_file  = NULL;
   api->zread_file  = NULL;
   api->zwrite_file = fwrite_impl;
   api->ztell_file  = out_tell;
   api->zseek_file  = out_fseek;
   api->zclose_file = fclose_impl;
   api->zerror_file = NULL;
   api->opaque      = NULL;
}

static long in_fseek(
   voidpf WXUNUSED(opaque),
   voidpf stream,
   ZOFF_T offset,
   int origin)
{
   wxInputStream* in = (wxInputStream*)stream;
   return (wxInvalidOffset != in->SeekI(offset, (wxSeekMode)origin)) ? 0 : -1;
}

static uLong fread_impl(
   voidpf WXUNUSED(opaque),
   voidpf stream,
   void* buf,
   uLong size)
{
   wxInputStream* in = (wxInputStream*)stream;
   return in->Read(buf, size).LastRead();
}

static ZPOS_T in_tell(voidpf WXUNUSED(opaque), voidpf stream)
{
   wxInputStream* in = (wxInputStream*)stream;
   return in->TellI();
}

void fill_filefunc(struct zlib_filefunc_def_s* api, wxInputStream* WXUNUSED(stream))
{
   api->zopen_file  = NULL;
   api->zread_file  = fread_impl;
   api->zwrite_file = NULL;
   api->ztell_file  = in_tell;
   api->zseek_file  = in_fseek;
   api->zclose_file = fclose_impl;
   api->zerror_file = NULL;
   api->opaque      = NULL;
}
