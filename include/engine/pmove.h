#ifndef ENGINE_PMOVE_H
#define ENGINE_PMOVE_H

#include "common.h"
#include "engine/mathlib.h"

#define MAX_PHYSENTS 600

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2
#define CONTENTS_WATER -3
#define CONTENTS_SLIME -4
#define CONTENTS_LAVA -5
#define CONTENTS_SKY -6
#define CONTENTS_ORIGIN -7
#define CONTENTS_CLIP -8
#define CONTENTS_CURRENT_0 -9
#define CONTENTS_CURRENT_90 -10
#define CONTENTS_CURRENT_180 -11
#define CONTENTS_CURRENT_270 -12
#define CONTENTS_CURRENT_UP -13
#define CONTENTS_CURRENT_DOWN -14
#define CONTENTS_TRANSLUCENT -15
#define CONTENTS_LADDER -16

#define PM_NORMAL 0
#define PM_STUDIO_IGNORE 1
#define PM_STUDIO_BOX 2

/* layouts parciais: só os campos usados até agora, nos offsets do binário */
typedef struct physent_s
{
	vec3_t origin;                  /* 0x00 */
	struct model_s *model;          /* 0x0C: só para modelos bsp */
	struct model_s *studiomodel;    /* 0x10 */
	vec3_t mins, maxs;              /* 0x14, 0x20: só para não-bsp */
	int info;                       /* 0x2C: índice em cl_entities/edicts */
	vec3_t angles;                  /* 0x30 */
	byte unk_3C[0x58 - 0x3C];
} physent_t; /* 0x58 */

typedef struct
{
	vec3_t normal;
	float dist;
} pmplane_t;

typedef struct pmtrace_s
{
	qboolean allsolid;              /* 0x00 */
	qboolean startsolid;            /* 0x04 */
	qboolean inopen, inwater;       /* 0x08, 0x0C */
	float fraction;                 /* 0x10 */
	vec3_t endpos;                  /* 0x14 */
	pmplane_t plane;                /* 0x20 */
	int ent;                        /* 0x30 */
	vec3_t deltavelocity;           /* 0x34 */
	int hitgroup;                   /* 0x40 */
} pmtrace_t; /* 0x44 */

typedef struct playermove_s
{
	byte unk_00[0x0C];
	vec3_t origin;                  /* 0x0C */
	vec3_t angles;                  /* 0x18 */
	byte unk_24[0x84 - 0x24];
	int numphysent;                 /* 0x84 */
	physent_t physents[MAX_PHYSENTS]; /* 0x88 */
} playermove_t;

extern playermove_t pmove;

extern vec3_t player_mins[3];
extern vec3_t player_maxs[3];

void PM_InitBoxHull(void);
pmtrace_t PM_PlayerTrace(vec3_t start, vec3_t end, int traceFlags, int ignore_pe);

#endif /* ENGINE_PMOVE_H */
