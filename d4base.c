/* d4base.c */
/* CodeBase compatibility layer */
/* Copyright (c) 2007-2012 by Troels K. All rights reserved. */
/* License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt */

#include <stdlib.h>
#include <time.h>
#include <tchar.h>

#ifdef USE_CODEBASE
#else

#include "ioapi/zlib.h"
#include "ioapi/ioapi.h"
#include "bool.h"
#include "dbf.h"
#include "d4base.h"

static DBF_HANDLE MOD_handle = NULL;

int d4use2(const TCHAR* file, struct zlib_filefunc_def_s* api, int readonly)
{
   return 0;
}

int d4close(void)
{
   return 0;
}

int d4init(void)
{
   return 0;
}

int d4init_undo(void)
{
   return 0;
}

int d4zap(long start_rec, long end_rec)
{
   return 0;
}

int d4select(int handle)
{
   MOD_handle = (DBF_HANDLE)handle;
   return 0;
}

long d4reccount(void)
{
   return 0;
}

long d4recno(void)
{
   return 0;
}

int d4eof(void)
{
   return 0;
}

int d4append(void)
{
   return 0;
}

int d4append_blank(void)
{
   return 0;
}

int d4deleted(void)
{
   return 0;
}

int d4write(long rec_num)
{
   return 0;
}

int d4skip(long num_skip)
{
   return 0;
}

void f4r_str(long field_ref, char* str)
{
}

long f4long(long field_ref)
{
   return 0;
}

void f4r_char(long field_ref, char chr)
{
}

char* f4str(long field_ref)
{
   return 0;
}

char f4type(long field_ref)
{
   return 0;
}

long f4ref(const char* field_name)
{
   return 0;
}

int m4write(long field_ref, long rec_num, const char *str, int str_len )
{
   return 0;
}

int m4read( long field_ref, long rec_num, char *str, int str_len)
{
   return 0;
}

int f4true(long field_ref)
{
   return 0;
}

int x4filter(int (* filter_routine)(void* user), void* user)
{
   return 0;
}

int x4top(void)
{
   return 0;
}

int x4bottom(void)
{
   return 0;
}

int x4go(long record_number)
{
   return 0;
}

int x4skip(long num_records)
{
   return 0;
}

#endif
