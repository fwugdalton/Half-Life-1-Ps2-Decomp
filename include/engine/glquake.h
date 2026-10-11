#ifndef ENGINE_GLQUAKE_H
#define ENGINE_GLQUAKE_H

#include "common.h"

typedef struct
{
    byte pad0[0x12];
    short texnum;               /* 0x12 */
    byte pad14[0x18 - 0x14];
    unsigned short width;       /* 0x18 */
    unsigned short height;      /* 0x1A */
    byte pad1C[0x20 - 0x1C];
    byte palette[0x400];        /* 0x20 */
    byte data[1];               /* 0x420 */
} qpic_t;

typedef struct
{
    short texnum;               /* 0x00 */
    short servercount;          /* 0x02 */
    byte pad4[0x8 - 0x4];
    char mipmap;                /* 0x08 */
    byte pad9[0x48 - 0x9];
} gltexture_t; /* 0x48 */

#define MAX_GLTEXTURES 1200

#define GL_QUADS 7
#define GL_ONE 1
#define GL_SRC_ALPHA 0x302
#define GL_DEPTH_TEST 0xB71
#define GL_ALPHA_TEST 0xBC0
#define GL_BLEND 0xBE2
#define GL_TEXTURE_2D 0xDE1
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_MODULATE 8448.0f

extern int D_00399640;          /* numgltextures */
extern gltexture_t D_004F9DD8[]; /* gltextures */
extern int currenttexture;

void GL_Bind(int texnum);

#endif
