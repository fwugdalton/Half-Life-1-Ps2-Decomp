#include "common.h"
#include "engine/cvar.h"
#include "engine/model.h"
#include "engine/pmove.h"

/* pmove.c: movimento do jogador. Num só arquivo estão a parte do motor (cvars,
 * hulls), a depuração com partículas (pm_debug) e o código compartilhado
 * (pm_shared): a string "\n" é a mesma entre PM_PrintPhysEnts e pm_shared. */

void Con_Printf(const char *fmt, ...);
void PM_Init(void);

cvar_t cl_showclip = { "cl_showclip", "0" };
cvar_t cl_printclip = { "cl_printclip", "0" };
cvar_t pm_nostudio = { "pm_nostudio", "0" };
cvar_t pm_nocomplex = { "pm_nocomplex", "0" };
cvar_t pm_worldonly = { "pm_worldonly", "0" };
cvar_t pm_pushfix = { "pm_pushfix", "0" };
cvar_t pm_nostucktouch = { "pm_nostucktouch", "0" };

vec3_t player_mins[3] = { { -16, -16, -36 }, { -16, -16, -18 }, { 0, 0, 0 } };
vec3_t player_maxs[3] = { { 16, 16, 36 }, { 16, 16, 18 }, { 0, 0, 0 } };

void Pmove_Init(void)
{
	PM_InitBoxHull();
	PM_Init();
}

float PM_SplineFraction(float value, float scale)
{
	float valueSquared;

	value = scale * value;
	valueSquared = value * value;

	return 3 * valueSquared - 2 * valueSquared * value;
}

char *PM_NameForContents(int contents)
{
	switch (contents)
	{
	case CONTENTS_EMPTY:
		return "EMPTY";
	case CONTENTS_SOLID:
		return "SOLID";
	case CONTENTS_WATER:
		return "WATER";
	case CONTENTS_SLIME:
		return "SLIME";
	case CONTENTS_LAVA:
		return "LAVA";
	case CONTENTS_SKY:
		return "SKY";
	case CONTENTS_ORIGIN:
		return "ORIGIN";
	case CONTENTS_CLIP:
		return "CLIP";
	case CONTENTS_CURRENT_0:
		return "C0";
	case CONTENTS_CURRENT_90:
		return "C90";
	case CONTENTS_CURRENT_180:
		return "C180";
	case CONTENTS_CURRENT_270:
		return "C270";
	case CONTENTS_CURRENT_UP:
		return "CUP";
	case CONTENTS_CURRENT_DOWN:
		return "CDOWN";
	case CONTENTS_TRANSLUCENT:
		return "TRANSLUCENT";
	case CONTENTS_LADDER:
		return "LADDER";
	default:
		return "UNKNOWN";
	}
}

void PM_PrintPhysEnts(void)
{
	int i;
	physent_t *pe;

	Con_Printf("===== PMOVE ======\n");
	Con_Printf("Origin: %4.2f %4.2f %4.2f\n", pmove.origin[0], pmove.origin[1], pmove.origin[2]);
	Con_Printf("------------------\n");

	for (i = 0; i < pmove.numphysent; i++)
	{
		pe = &pmove.physents[i];
		Con_Printf("Ent:  %i\n", i);
		Con_Printf("Org:  %4.2f %4.2f %4.2f\n", pe->origin[0], pe->origin[1], pe->origin[2]);
		Con_Printf("Ang:  %4.2f %4.2f %4.2f\n", pe->angles[0], pe->angles[1], pe->angles[2]);
	}

	Con_Printf("\n");
}

typedef struct cl_entity_s
{
	byte unk_000[0x1A0];
} cl_entity_t;

extern cl_entity_t *cl_entities;

void func_00249CE8(vec3_t org, int color, float life, int zpos, int zvel);
void func_00249868(cl_entity_t *ent);

/* expande as caixas de depuração por essa quantidade de unidades */
#define BOX_GAP 0.0f

static int PM_boxpnt[6][4] =
{
	{ 0, 4, 6, 2 }, /* +X */
	{ 0, 1, 5, 4 }, /* +Y */
	{ 0, 2, 3, 1 }, /* +Z */
	{ 7, 5, 1, 3 }, /* -X */
	{ 7, 3, 2, 6 }, /* -Y */
	{ 7, 6, 4, 5 }, /* -Z */
};

void PM_ParticleLine(vec3_t start, vec3_t end, int pcolor, float life, float vert)
{
	float linestep = 2.0f;
	float curdist;
	float len;
	vec3_t curpos;
	vec3_t diff;
	int i;

	VectorSubtract(end, start, diff);

	len = VectorNormalize(diff);

	curdist = 0;
	while (curdist <= len)
	{
		for (i = 0; i < 3; i++)
			curpos[i] = start[i] + curdist * diff[i];

		func_00249CE8(curpos, pcolor, life, 0, vert);

		curdist += linestep;
	}
}

void PM_DrawRectangle(vec3_t tl, vec3_t bl, vec3_t tr, vec3_t br, int pcolor, float life)
{
	PM_ParticleLine(tl, bl, pcolor, life, 0);
	PM_ParticleLine(bl, br, pcolor, life, 0);
	PM_ParticleLine(br, tr, pcolor, life, 0);
	PM_ParticleLine(tr, tl, pcolor, life, 0);
}

void PM_DrawPhysEntBBox(int num, int pcolor, float life)
{
	physent_t *pe;
	vec3_t org;
	int j;
	vec3_t tmp;
	vec3_t p[8];
	float gap = BOX_GAP;

	if (num >= pmove.numphysent || num <= 0)
		return;

	pe = &pmove.physents[num];

	if (pe->model)
	{
		VectorCopy(pe->origin, org);

		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pe->model->mins[0] - gap : pe->model->maxs[0] + gap;
			tmp[1] = (j & 2) ? pe->model->mins[1] - gap : pe->model->maxs[1] + gap;
			tmp[2] = (j & 4) ? pe->model->mins[2] - gap : pe->model->maxs[2] + gap;

			VectorCopy(tmp, p[j]);
		}

		/* se a caixa deve ser girada, gira */
		if (pe->angles[0] || pe->angles[1] || pe->angles[2])
		{
			vec3_t forward, right, up;

			AngleVectorsTranspose(pe->angles, forward, right, up);
			for (j = 0; j < 8; j++)
			{
				VectorCopy(p[j], tmp);
				p[j][0] = DotProduct(tmp, forward);
				p[j][1] = DotProduct(tmp, right);
				p[j][2] = DotProduct(tmp, up);
			}
		}

		/* desloca pela origem da entidade */
		for (j = 0; j < 8; j++)
			VectorAdd(p[j], org, p[j]);

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}
	}
	else
	{
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pe->mins[0] : pe->maxs[0];
			tmp[1] = (j & 2) ? pe->mins[1] : pe->maxs[1];
			tmp[2] = (j & 4) ? pe->mins[2] : pe->maxs[2];

			VectorAdd(tmp, pe->origin, tmp);
			VectorCopy(tmp, p[j]);
		}

		for (j = 0; j < 6; j++)
		{
			PM_DrawRectangle(
				p[PM_boxpnt[j][1]],
				p[PM_boxpnt[j][0]],
				p[PM_boxpnt[j][2]],
				p[PM_boxpnt[j][3]],
				pcolor, life);
		}
	}
}

void PM_DrawBBox(vec3_t mins, vec3_t maxs, vec3_t origin, int pcolor, float life)
{
	int j;
	vec3_t tmp;
	vec3_t p[8];
	float gap = BOX_GAP;

	for (j = 0; j < 8; j++)
	{
		tmp[0] = (j & 1) ? mins[0] - gap : maxs[0] + gap;
		tmp[1] = (j & 2) ? mins[1] - gap : maxs[1] + gap;
		tmp[2] = (j & 4) ? mins[2] - gap : maxs[2] + gap;

		VectorAdd(tmp, origin, tmp);
		VectorCopy(tmp, p[j]);
	}

	for (j = 0; j < 6; j++)
	{
		PM_DrawRectangle(
			p[PM_boxpnt[j][1]],
			p[PM_boxpnt[j][0]],
			p[PM_boxpnt[j][2]],
			p[PM_boxpnt[j][3]],
			pcolor, life);
	}
}

void PM_ViewEntity(void)
{
	vec3_t forward, right, up;
	float raydist = 256.0f;
	vec3_t origin;
	vec3_t end;
	int i;
	pmtrace_t trace;
	int pcolor = 77;

	if (!cl_showclip.value)
		return;

	AngleVectors(pmove.angles, forward, right, up);

	VectorCopy(pmove.origin, origin);

	for (i = 0; i < 3; i++)
		end[i] = origin[i] + raydist * forward[i];

	trace = PM_PlayerTrace(origin, end, PM_STUDIO_BOX, 0);

	if (trace.ent > 0) /* não é o mundo */
		pcolor = 111;

	/* desenha o hull ou a caixa */
	if (trace.ent > 0)
	{
		PM_DrawPhysEntBBox(trace.ent, pcolor, 0.3f);

		if (cl_printclip.value)
			func_00249868(&cl_entities[pmove.physents[trace.ent].info]);
	}
}

/* direções dos seis eixos, usadas por func_0026C758 */
vec3_t D_003322A0[6] =
{
	{ 1, 0, 0 }, { 0, 1, 0 }, { -1, 0, 0 },
	{ 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 },
};

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026A800);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026A940);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026AA10);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026AAC0);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026AB50);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026B290);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026B848);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026BAA8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026BB90);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026BC98);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026C0C8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026C288);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026C448);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026C5D0);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026C758);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", PM_Init);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026CD60);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026CDE8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026CE08);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026D2C0);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026D5B8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026D700);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026D7E8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026D888);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026DA48);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026DF08);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026E060);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026E160);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026E6C8);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026E818);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026ED00);

INCLUDE_ASM("asm/nonmatchings/engine/pmove", func_0026F490);
