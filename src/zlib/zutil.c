/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 * Versão da Gearbox (Gbx/Zutil.c): zcalloc/zcfree usam o heap do engine.
 */

#include "zutil.h"

struct internal_state      {int dummy;}; /* for buggy compilers */

void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);

const char *z_errmsg[10] = {
"need dictionary",     /* Z_NEED_DICT       2  */
"stream end",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"file error",          /* Z_ERRNO         (-1) */
"stream error",        /* Z_STREAM_ERROR  (-2) */
"data error",          /* Z_DATA_ERROR    (-3) */
"insufficient memory", /* Z_MEM_ERROR     (-4) */
"buffer error",        /* Z_BUF_ERROR     (-5) */
"incompatible version",/* Z_VERSION_ERROR (-6) */
""};


const char * ZEXPORT zlibVersion()
{
    return ZLIB_VERSION;
}

/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const char * ZEXPORT zError(err)
    int err;
{
    return ERR_MSG(err);
}


/* o original usa __FILE__/__LINE__ de Gbx/Zutil.c (linha 73) */
voidpf zcalloc (opaque, items, size)
    voidpf opaque;
    unsigned items;
    unsigned size;
{
    voidpf p = Mem_DbgMalloc(items * size, "Gbx/Zutil.c", 73);
    zmemzero(p, items * size);
    return p;
}

void  zcfree (opaque, ptr)
    voidpf opaque;
    voidpf ptr;
{
    Mem_Free(ptr);
}
