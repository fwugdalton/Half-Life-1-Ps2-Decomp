#ifndef DLLS_SHAKE_H
#define DLLS_SHAKE_H

/* shake.h do HLSDK */

extern int gmsgShake;

typedef struct
{
    unsigned short amplitude;   /* FIXED 4.12 */
    unsigned short duration;    /* FIXED 4.12 */
    unsigned short frequency;   /* FIXED 8.8 */
} ScreenShake;

extern int gmsgFade;

#define FFADE_IN        0x0000
#define FFADE_OUT       0x0001
#define FFADE_MODULATE  0x0002
#define FFADE_STAYOUT   0x0004

typedef struct
{
    unsigned short duration;    /* FIXED 4.12 */
    unsigned short holdTime;    /* FIXED 4.12 */
    short fadeFlags;
    byte r, g, b, a;
} ScreenFade;

#endif
