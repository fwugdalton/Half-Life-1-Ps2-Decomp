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

#define MOVETYPE_WALK 3

#define MAX_CLIP_PLANES 5

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

typedef struct movevars_s
{
	float gravity;                  /* 0x00 */
	float stopspeed;                /* 0x04 */
	float maxspeed;                 /* 0x08 */
	float spectatormaxspeed;        /* 0x0C */
	float accelerate;               /* 0x10 */
	float airaccelerate;            /* 0x14 */
	float wateraccelerate;          /* 0x18 */
	float friction;                 /* 0x1C */
	float edgefriction;             /* 0x20 */
	float waterfriction;            /* 0x24 */
	float entgravity;               /* 0x28 */
	float bounce;                   /* 0x2C */
	float stepsize;                 /* 0x30 */
	float maxvelocity;              /* 0x34 */
} movevars_t;

/* no PS2 há um playermove_t por jogador local (tela dividida): pmove[clientId] */
typedef struct playermove_s
{
	byte unk_00[0x0C];
	vec3_t origin;                  /* 0x0C */
	vec3_t angles;                  /* 0x18 */
	vec3_t velocity;                /* 0x24 */
	vec3_t unk_30;
	vec3_t basevelocity;            /* 0x3C */
	byte unk_48[0x60 - 0x48];
	int usehull;                    /* 0x60 */
	float gravity;                  /* 0x64 */
	float friction;                 /* 0x68 */
	int oldbuttons;                 /* 0x6C */
	float waterjumptime;            /* 0x70 */
	qboolean dead;                  /* 0x74 */
	int unk_78;
	int movetype;                   /* 0x7C */
	byte unk_80[0x84 - 0x80];
	int numphysent;                 /* 0x84 */
	physent_t physents[MAX_PHYSENTS]; /* 0x88 */
	byte unk_CEC8[0xD470 - 0xCEC8];
	int numtouch;                   /* 0xD470 */
	pmtrace_t touchindex[MAX_PHYSENTS]; /* 0xD474 */
} playermove_t; /* 0x173D4 */

extern playermove_t pmove[];
extern movevars_t movevars;
extern float pm_frametime;

extern vec3_t player_mins[3];
extern vec3_t player_maxs[3];

void PM_InitBoxHull(void);
pmtrace_t PM_PlayerTrace(vec3_t start, vec3_t end, int traceFlags, int clientId);
qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity, int clientId);

#endif /* ENGINE_PMOVE_H */
