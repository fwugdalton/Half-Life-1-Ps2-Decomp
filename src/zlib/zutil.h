/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995-1998 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#include "zlib.h"

#define local static

typedef unsigned char  uch;
typedef uch FAR uchf;
typedef unsigned short ush;
typedef ush FAR ushf;
typedef unsigned long  ulg;

#define DEF_WBITS 15
#define PRESET_DICT 0x20 /* preset dictionary flag in zlib header */

void *memcpy(void *, const void *, unsigned int);
#define zmemcpy memcpy

#define Assert(cond,msg)
#define Trace(x)
#define Tracev(x)
#define Tracevv(x)
#define Tracec(c,x)
#define Tracecv(c,x)

typedef uLong (ZEXPORT *check_func) OF((uLong check, const Bytef *buf,
                                       uInt len));
voidpf zcalloc OF((voidpf opaque, unsigned items, unsigned size));
void   zcfree  OF((voidpf opaque, voidpf ptr));

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidpf)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* _Z_UTIL_H */
