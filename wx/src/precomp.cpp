// precomp.cpp
// Copyright (c) 2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

#include "precomp.h"

#ifdef _MSC_VER
#if wxUSE_UNICODE
#define MODE "Unicode"
#else
#define MODE "Ansi"
#endif

#pragma message("Compiling using wx "wxVERSION_NUM_DOT_STRING" "MODE"...")
#endif
