#ifndef ENGINE_CL_ENTITY_H
#define ENGINE_CL_ENTITY_H

#include "engine/mathlib.h"
#include "engine/entity_state.h"

struct model_s;


/* cl_entity_t do PS2, 0x1A0 bytes. Layout parcial (r_trans.c, pmove.c e pr_cmds.c ainda têm versões locais). */
typedef struct cl_entity_s
{
    byte controller[4];         /* 0x00 */
    byte blending[2];           /* 0x04 */
    byte prevseqblending[2];    /* 0x06 */
    byte prevcontroller[4];     /* 0x08 */
    byte prevblending[2];       /* 0x0C */
    byte mouthopen;             /* 0x0E */
    byte padF;
    int rendermode;             /* 0x10 */
    int renderamt;              /* 0x14 */
    int renderfx;               /* 0x18 */
    int movetype;               /* 0x1C */
    int body;                   /* 0x20: body do viewmodel (svc_weaponanim) */
    int sequence;               /* 0x24 */
    int prevsequence;           /* 0x28 */
    int index;                  /* 0x2C */
    int colormap;               /* 0x30 */
    int skin;                   /* 0x34 */
    int visframe;               /* 0x38 */
    byte pad3C[0x44 - 0x3C];
    int aiment;                 /* 0x44 */
    byte pad48[0x50 - 0x48];
    struct model_s *model;      /* 0x50 */
    struct efrag_s *efrag;      /* 0x54 */
    void *topnode;              /* 0x58 */
    float framerate;            /* 0x5C */
    float frame;                /* 0x60 */
    byte pad64[0x68 - 0x64];
    float animtime;             /* 0x68 */
    float scale;                /* 0x6C */
    byte pad70[0x74 - 0x70];
    float prevanimtime;         /* 0x74 */
    float prevframe;            /* 0x78 */
    float sequencetime;         /* 0x7C */
    byte pad80[0x84 - 0x80];
    vec3_t origin;              /* 0x84 */
    vec3_t angles;              /* 0x90 */
    vec3_t prevorigin;          /* 0x9C */
    vec3_t prevangles;          /* 0xA8 */
    vec3_t attachment[4];       /* 0xB4 */
    color24 rendercolor;        /* 0xE4 */
    byte padE7;
    unsigned int cvFloorColor[4]; /* 0xE8: colorVec */
    entity_state_t baseline;    /* 0xF8 */
    byte pad18C[0x194 - 0x18C];
    int unk194;                 /* 0x194: bits 0x100/0x200 = viewmodel de qual jogador */
    int trivial_accept;         /* 0x198 */
    byte pad19C[0x1A0 - 0x19C];
} cl_entity_t; /* 0x1A0 */

#endif
