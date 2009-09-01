/* d4base.h */
/* CodeBase compatibility layer */
/* Copyright (c) 2007-2009 by Troels K. All rights reserved. */
/* License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt */

#ifndef __D4BASE_H__
#define __D4BASE_H__

#ifndef __DBF_H__
   #include "dbf.h"
#endif

#ifndef EXTERN_C   
   #ifdef __cplusplus
      #define EXTERN_C    extern "C"
   #else
      #define EXTERN_C    extern
   #endif
#endif

#define FIELD struct _DBF_FIELD_DATA

EXTERN_C int  d4init(void);
EXTERN_C int  d4init_undo(void);
EXTERN_C const TCHAR* d4name(void);
EXTERN_C int  d4deleted(void);
EXTERN_C int  d4select(int handle);
EXTERN_C int  d4close(void);
EXTERN_C int  d4create(const TCHAR*, struct zlib_filefunc_def_s*, int, const FIELD *, int);
EXTERN_C int  d4use2(const TCHAR* file, struct zlib_filefunc_def_s*, int readonly);
EXTERN_C int  d4pack(void);
EXTERN_C int  d4zap(long start_rec, long end_rec);
EXTERN_C int  d4eof(void);
EXTERN_C int  d4bof(void);
EXTERN_C long d4recno(void);
EXTERN_C long d4reccount(void);
EXTERN_C int  d4append(void);
EXTERN_C int  d4append_blank(void);
EXTERN_C int  d4write(long rec_num);
EXTERN_C int  d4skip(long num_skip);
EXTERN_C unsigned char d4language_get(void);
EXTERN_C void d4language_set(unsigned char);

EXTERN_C void f4r_str( long field_ref, char* str);
EXTERN_C char f4type(long field_ref);
EXTERN_C int  f4isnull(long field_ref);
EXTERN_C long f4ref(const char* field_name);
EXTERN_C int  f4width(long field_ref);
EXTERN_C long f4j_ref(int);
EXTERN_C char* f4name(long);
EXTERN_C long f4long(long f_ref);
EXTERN_C int  f4num_fields(void);
EXTERN_C void f4r_long(long, long);
EXTERN_C int  f4true(long field_ref);
EXTERN_C double f4double(long);
EXTERN_C void f4r_double(long, double);
EXTERN_C void f4r_char( long f_ref, char chr );
EXTERN_C char* f4str(long field_ref);

EXTERN_C int  m4read( long field_ref, long rec_num, char *str, int str_len);
EXTERN_C int  m4write(long field_ref, long rec_num, const char *str, int str_len );

EXTERN_C int  x4go(long record_number);
EXTERN_C int  x4top(void);
EXTERN_C int  x4bottom(void);
EXTERN_C int  x4filter(int (*filter_routine)(void* user), void* user);
EXTERN_C int  x4skip(long num_records);

#endif /* __D4BASE_H__ */
