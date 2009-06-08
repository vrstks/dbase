/* stdint.h adapted for MS compilers */

#ifndef _MSC_VER
   /* Use unpatched gcc version */
   #include <stdint.h>
#else

#ifndef _STDINT_H
#define _STDINT_H

typedef signed char    int8_t;
typedef unsigned char  uint8_t;

typedef short          int16_t;
typedef unsigned short uint16_t;

#ifdef _WIN32
   typedef int              int32_t;
   typedef unsigned         uint32_t;
   
   typedef __int64          int64_t;
   typedef unsigned __int64 uint64_t;
#else
   typedef long          int32_t;
   typedef unsigned long uint32_t;
#endif

/* 7.18.2  Limits of specified-width integer types */
#if !defined ( __cplusplus) || defined (__STDC_LIMIT_MACROS)

/* 7.18.2.1  Limits of exact-width integer types */
#define INT8_MIN (-128) 
#define INT16_MIN (-32768)
#define INT32_MIN (-2147483647 - 1)
#ifdef _WIN32
#define INT64_MIN  (-9223372036854775807LL - 1)
#endif

#define INT8_MAX 127
#define INT16_MAX 32767
#define INT32_MAX 2147483647
#ifdef _WIN32
#define INT64_MAX 9223372036854775807LL
#endif

#define UINT8_MAX 0xff /* 255U */
#define UINT16_MAX 0xffff /* 65535U */
#define UINT32_MAX 0xffffffff  /* 4294967295U */
#ifdef _WIN32
#define UINT64_MAX 0xffffffffffffffffULL /* 18446744073709551615ULL */
#endif

#endif /* !defined ( __cplusplus) || defined __STDC_LIMIT_MACROS */


#endif /* _STDINT_H */
#endif /* !_MSC_VER */
