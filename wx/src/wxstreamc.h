// streamc.h
// Copyright (c) 2007-2009 by Troels K. All rights reserved.
// License: wxWindows Library Licence, Version 3.1 - see LICENSE.txt

struct zlib_filefunc_def_s;

extern void fill_filefunc(struct zlib_filefunc_def_s* api, wxOutputStream*);
extern void fill_filefunc(struct zlib_filefunc_def_s* api, wxInputStream*);

