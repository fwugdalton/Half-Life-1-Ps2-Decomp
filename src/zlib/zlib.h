/* zlib.h -- interface of the 'zlib' general purpose compression library
  version 1.1.3, July 9th, 1998

  Copyright (C) 1995-1998 Jean-loup Gailly and Mark Adler

  Trecho reduzido ao que o inflate usa; no EE, uLong e' de 64 bits.
*/

#ifndef _ZLIB_H
#define _ZLIB_H

#define ZLIB_VERSION "1.1.3"

#if defined(__STDC__) && !defined(STDC)
#  define STDC
#endif

#define OF(args)  args
#define ZEXPORTVA

#ifndef MAX_MEM_LEVEL
#  define MAX_MEM_LEVEL 9
#endif
#ifndef MAX_WBITS
#  define MAX_WBITS   15 /* 32K LZ77 window */
#endif
#define ZEXPORT
#define ZEXTERN extern
#define FAR

typedef unsigned char  Byte;
typedef unsigned int   uInt;
typedef unsigned long  uLong;

typedef Byte  FAR Bytef;
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;

typedef void const *voidpc;
typedef void FAR   *voidpf;
typedef void       *voidp;

typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
    Bytef    *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Bytef    *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: ascii or binary */
    uLong   adler;      /* adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;

#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2

#define Z_DEFLATED   8

#define Z_NULL  0

ZEXTERN int ZEXPORT deflate OF((z_streamp strm, int flush));
ZEXTERN int ZEXPORT deflateEnd OF((z_streamp strm));
ZEXTERN int ZEXPORT deflateSetDictionary OF((z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));
ZEXTERN int ZEXPORT deflateCopy OF((z_streamp dest,
                                    z_streamp source));
ZEXTERN int ZEXPORT deflateReset OF((z_streamp strm));
ZEXTERN int ZEXPORT deflateParams OF((z_streamp strm,
                                      int level,
                                      int strategy));
ZEXTERN int ZEXPORT deflateInit_ OF((z_streamp strm, int level,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT deflateInit2_ OF((z_streamp strm, int  level, int  method,
                                      int windowBits, int memLevel,
                                      int strategy, const char *version,
                                      int stream_size));
ZEXTERN uLong ZEXPORT crc32   OF((uLong crc, const Bytef *buf, uInt len));
ZEXTERN const uLongf * ZEXPORT get_crc_table    OF((void));

ZEXTERN int ZEXPORT inflate OF((z_streamp strm, int flush));
ZEXTERN int ZEXPORT inflateEnd OF((z_streamp strm));
ZEXTERN int ZEXPORT inflateSetDictionary OF((z_streamp strm,
                                             const Bytef *dictionary,
                                             uInt  dictLength));
ZEXTERN int ZEXPORT inflateSync OF((z_streamp strm));
ZEXTERN int ZEXPORT inflateReset OF((z_streamp strm));
ZEXTERN uLong ZEXPORT adler32 OF((uLong adler, const Bytef *buf, uInt len));
ZEXTERN int ZEXPORT inflateInit_ OF((z_streamp strm,
                                     const char *version, int stream_size));
ZEXTERN int ZEXPORT inflateInit2_ OF((z_streamp strm, int  windowBits,
                                      const char *version, int stream_size));
ZEXTERN int ZEXPORT inflateSyncPoint OF((z_streamp z));

#endif /* _ZLIB_H */
