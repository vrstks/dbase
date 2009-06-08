// streamc.h

struct zlib_filefunc_def_s;

extern void fill_filefunc(struct zlib_filefunc_def_s* api, wxOutputStream*);
extern void fill_filefunc(struct zlib_filefunc_def_s* api, wxInputStream*);

