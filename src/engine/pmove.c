#include "common.h"
#include "engine/cvar.h"
#include "engine/model.h"
#include "engine/pmove.h"
#include "engine/server.h"

/* pmove[0].c: movimento do jogador. Num só arquivo estão a parte do motor (cvars,
 * hulls), a depuração com partículas (pm_debug) e o código compartilhado
 * (pm_shared): a string "\n" é a mesma entre PM_PrintPhysEnts e pm_shared. */

void Con_Printf(const char *fmt, ...);
void PM_CreateStuckTable(void);
void *memset(void *s, int c, unsigned int n);

extern vec3_t rgv3tStuckTable[54];
extern int rgStuckLast[][2];
void PM_CheckVelocity(int clientId);
int PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce);
int PM_FlyMove(int clientId);
qboolean PM_CheckWater(int clientId);
void PM_Accelerate(vec3_t wishdir, float wishspeed, float accel, int clientId);
void PM_AirAccelerate(vec3_t wishdir, float wishspeed, float accel, int clientId);

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
	PM_CreateStuckTable();
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
	Con_Printf("Origin: %4.2f %4.2f %4.2f\n", pmove[0].origin[0], pmove[0].origin[1], pmove[0].origin[2]);
	Con_Printf("------------------\n");

	for (i = 0; i < pmove[0].numphysent; i++)
	{
		pe = &pmove[0].physents[i];
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
void Con_DPrintf(const char *fmt, ...);

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

	if (num >= pmove[0].numphysent || num <= 0)
		return;

	pe = &pmove[0].physents[num];

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

	AngleVectors(pmove[0].angles, forward, right, up);

	VectorCopy(pmove[0].origin, origin);

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
			func_00249868(&cl_entities[pmove[0].physents[trace.ent].info]);
	}
}





void PM_CheckVelocity(int clientId)
{
	int i;

	/* limita a velocidade */
	for (i = 0; i < 3; i++)
	{
		/* vê se o valor é lixo */
		if (IS_NAN(pmove[clientId].velocity[i]))
		{
			Con_Printf("PM  Got a NaN velocity %i\n", i);
			pmove[clientId].velocity[i] = 0;
		}
		if (IS_NAN(pmove[clientId].origin[i]))
		{
			Con_Printf("PM  Got a NaN origin on %i\n", i);
			pmove[clientId].origin[i] = 0;
		}

		if (pmove[clientId].velocity[i] > movevars.maxvelocity)
		{
			Con_DPrintf("PM  Got a velocity too high on %i\n", i);
			pmove[clientId].velocity[i] = movevars.maxvelocity;
		}
		else if (pmove[clientId].velocity[i] < -movevars.maxvelocity)
		{
			Con_DPrintf("PM  Got a velocity too low on %i\n", i);
			pmove[clientId].velocity[i] = -movevars.maxvelocity;
		}
	}
}

#define STOP_EPSILON 0.1f

/* desliza no objeto atingido; devolve 1 para chão e 2 para degrau/parede */
int PM_ClipVelocity(vec3_t in, vec3_t normal, vec3_t out, float overbounce)
{
	float backoff;
	float change;
	float angle;
	int i, blocked;

	angle = normal[2];

	blocked = 0;
	if (angle > 0)
		blocked |= 0x01;
	if (!angle)
		blocked |= 0x02;

	backoff = DotProduct(in, normal) * overbounce;

	for (i = 0; i < 3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0;
	}

	return blocked;
}

void PM_AddCorrectGravity(int clientId)
{
	float ent_gravity;

	if (pmove[clientId].gravity)
		ent_gravity = pmove[clientId].gravity;
	else
		ent_gravity = 1.0f;

	pmove[clientId].velocity[2] -= ent_gravity * movevars.gravity * 0.5f * pm_frametime;
	pmove[clientId].velocity[2] += pmove[clientId].basevelocity[2] * pm_frametime;
	pmove[clientId].basevelocity[2] = 0;

	PM_CheckVelocity(clientId);
}

void PM_FixupGravityVelocity(int clientId)
{
	float ent_gravity;

	if (pmove[clientId].gravity)
		ent_gravity = pmove[clientId].gravity;
	else
		ent_gravity = 1.0f;

	pmove[clientId].velocity[2] -= ent_gravity * movevars.gravity * pm_frametime * 0.5f;

	PM_CheckVelocity(clientId);
}



/* movimento de corpo sólido que desliza ao longo de vários planos */
#ifdef NON_MATCHING
int PM_FlyMove(int clientId)
{
	int bumpcount, numbumps;
	vec3_t dir;
	float d;
	int numplanes;
	vec3_t planes[MAX_CLIP_PLANES];
	vec3_t primal_velocity, original_velocity;
	vec3_t new_velocity;
	int i, j;
	pmtrace_t trace;
	vec3_t end;
	float time_left, allFraction;
	int blocked;

	numbumps = 4;

	blocked = 0;
	numplanes = 0;
	VectorCopy(pmove[clientId].velocity, original_velocity);
	VectorCopy(pmove[clientId].velocity, primal_velocity);

	allFraction = 0;
	time_left = pm_frametime;

	for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
	{
		if (!pmove[clientId].velocity[0] && !pmove[clientId].velocity[1] && !pmove[clientId].velocity[2])
			break;

		for (i = 0; i < 3; i++)
			end[i] = pmove[clientId].origin[i] + time_left * pmove[clientId].velocity[i];

		trace = PM_PlayerTrace(pmove[clientId].origin, end, PM_NORMAL, clientId);

		allFraction += trace.fraction;

		if (trace.allsolid)
		{
			VectorCopy(vec3_origin, pmove[clientId].velocity);
			return 4;
		}

		if (trace.fraction > 0)
		{
			VectorCopy(trace.endpos, pmove[clientId].origin);
			VectorCopy(pmove[clientId].velocity, original_velocity);
			numplanes = 0;
		}

		if (trace.fraction == 1)
			break;

		PM_AddToTouched(trace, pmove[clientId].velocity, clientId);

		if (trace.plane.normal[2] > 0.7f)
			blocked |= 1; /* chão */
		if (!trace.plane.normal[2])
			blocked |= 2; /* degrau ou parede */

		time_left -= time_left * trace.fraction;

		if (numplanes >= MAX_CLIP_PLANES)
		{
			VectorCopy(vec3_origin, pmove[clientId].velocity);
			break;
		}

		VectorCopy(trace.plane.normal, planes[numplanes]);
		numplanes++;

		if (pmove[clientId].movetype == MOVETYPE_WALK &&
			((pm_onground[clientId] == -1) || (pmove[clientId].friction != 1)))
		{
			for (i = 0; i < numplanes; i++)
			{
				if (planes[i][2] > 0.7f)
				{
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1);
					VectorCopy(new_velocity, original_velocity);
				}
				else
					PM_ClipVelocity(original_velocity, planes[i], new_velocity, 1.0f + movevars.bounce * (1 - pmove[clientId].friction));
			}

			VectorCopy(new_velocity, pmove[clientId].velocity);
			VectorCopy(new_velocity, original_velocity);
		}
		else
		{
			for (i = 0; i < numplanes; i++)
			{
				PM_ClipVelocity(original_velocity, planes[i], pmove[clientId].velocity, 1);
				for (j = 0; j < numplanes; j++)
					if (j != i)
					{
						if (DotProduct(pmove[clientId].velocity, planes[j]) < 0)
							break;
					}
				if (j == numplanes)
					break;
			}

			if (i != numplanes)
			{
				;
			}
			else
			{
				if (numplanes != 2)
				{
					VectorCopy(vec3_origin, pmove[clientId].velocity);
					break;
				}
				CrossProduct(planes[0], planes[1], dir);
				d = DotProduct(dir, pmove[clientId].velocity);
				VectorScale(dir, d, pmove[clientId].velocity);
			}

			if (DotProduct(pmove[clientId].velocity, primal_velocity) <= 0)
			{
				VectorCopy(vec3_origin, pmove[clientId].velocity);
				break;
			}
		}
	}

	if (allFraction == 0)
		VectorCopy(vec3_origin, pmove[clientId].velocity);

	return blocked;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/pmove", PM_FlyMove);
#endif

void PM_WalkMove(int clientId)
{
	int clip;
	int oldonground;
	int i;
	vec3_t wishvel;
	float spd;
	float fmove, smove;
	vec3_t wishdir;
	float wishspeed;
	vec3_t dest, start;
	vec3_t original, originalvel;
	vec3_t down, downvel;
	float downdist, updist;
	pmtrace_t trace;

	fmove = pmove[clientId].cmd.forwardmove;
	smove = pmove[clientId].cmd.sidemove;

	pm_forward[2] = 0;
	pm_right[2] = 0;

	VectorNormalize(pm_forward);
	VectorNormalize(pm_right);

	for (i = 0; i < 2; i++)
		wishvel[i] = pm_forward[i] * fmove + pm_right[i] * smove;

	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	/* limita à velocidade máxima do servidor */
	if (wishspeed > pmove[clientId].maxspeed)
	{
		VectorScale(wishvel, pmove[clientId].maxspeed / wishspeed, wishvel);
		wishspeed = pmove[clientId].maxspeed;
	}

	pmove[clientId].velocity[2] = 0;
	PM_Accelerate(wishdir, wishspeed, movevars.accelerate, clientId);
	pmove[clientId].velocity[2] = 0;

	VectorAdd(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

	spd = Length(pmove[clientId].velocity);

	if (spd < 1.0f)
	{
		VectorCopy(vec3_origin, pmove[clientId].velocity);
		return;
	}

	oldonground = pm_onground[clientId];

	/* primeiro tenta ir direto ao destino */
	dest[0] = pmove[clientId].origin[0] + pmove[clientId].velocity[0] * pm_frametime;
	dest[1] = pmove[clientId].origin[1] + pmove[clientId].velocity[1] * pm_frametime;
	dest[2] = pmove[clientId].origin[2];

	VectorCopy(dest, start);
	trace = PM_PlayerTrace(pmove[clientId].origin, dest, PM_NORMAL, clientId);

	if (trace.fraction == 1)
	{
		VectorCopy(trace.endpos, pmove[clientId].origin);
		return;
	}

	if (oldonground == -1 && pm_waterlevel[clientId] == 0)
		return;

	if (pmove[clientId].waterjumptime)
		return;

	VectorCopy(pmove[clientId].origin, original);
	VectorCopy(pmove[clientId].velocity, originalvel);

	clip = PM_FlyMove(clientId);

	VectorCopy(pmove[clientId].origin, down);
	VectorCopy(pmove[clientId].velocity, downvel);

	VectorCopy(original, pmove[clientId].origin);
	VectorCopy(originalvel, pmove[clientId].velocity);

	/* começa um degrau acima */
	VectorCopy(pmove[clientId].origin, dest);
	dest[2] += movevars.stepsize;

	trace = PM_PlayerTrace(pmove[clientId].origin, dest, PM_NORMAL, clientId);

	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, pmove[clientId].origin);
	}

	clip = PM_FlyMove(clientId);

	/* agora desce de volta a altura do degrau */
	VectorCopy(pmove[clientId].origin, dest);
	dest[2] -= movevars.stepsize;

	trace = PM_PlayerTrace(pmove[clientId].origin, dest, PM_NORMAL, clientId);

	if (trace.plane.normal[2] < 0.7f)
		goto usedown;

	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, pmove[clientId].origin);
	}

	VectorCopy(pmove[clientId].origin, pm_up);

	/* qual dos dois foi mais longe */
	downdist = (down[0] - original[0]) * (down[0] - original[0])
		+ (down[1] - original[1]) * (down[1] - original[1]);
	updist = (pm_up[0] - original[0]) * (pm_up[0] - original[0])
		+ (pm_up[1] - original[1]) * (pm_up[1] - original[1]);

	if (downdist > updist)
	{
usedown:
		VectorCopy(down, pmove[clientId].origin);
		VectorCopy(downvel, pmove[clientId].velocity);
	}
	else
		pmove[clientId].velocity[2] = downvel[2];
}

void PM_Friction(int clientId)
{
	float *vel;
	float speed, newspeed, control;
	float friction;
	float drop;
	vec3_t newvel;

	/* no pulo da água não há atrito */
	if (pmove[clientId].waterjumptime)
		return;

	vel = pmove[clientId].velocity;

	speed = sqrtf(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);

	if (speed < 0.1f)
		return;

	drop = 0;

	/* atrito do chão */
	if (pm_onground[clientId] != -1)
	{
		vec3_t start, stop;
		pmtrace_t trace;

		start[0] = stop[0] = pmove[clientId].origin[0] + vel[0] / speed * 16;
		start[1] = stop[1] = pmove[clientId].origin[1] + vel[1] / speed * 16;
		start[2] = pmove[clientId].origin[2] + player_mins[pmove[clientId].usehull][2];
		stop[2] = start[2] - 34;

		trace = PM_PlayerTrace(start, stop, PM_NORMAL, clientId);

		if (trace.fraction == 1.0f)
			friction = movevars.friction * movevars.edgefriction;
		else
			friction = movevars.friction;

		friction *= pmove[clientId].friction;

		control = (speed < movevars.stopspeed) ? movevars.stopspeed : speed;
		drop += control * friction * pm_frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	newspeed /= speed;

	newvel[0] = vel[0] * newspeed;
	newvel[1] = vel[1] * newspeed;
	newvel[2] = vel[2] * newspeed;

	VectorCopy(newvel, pmove[clientId].velocity);
}

void PM_Accelerate(vec3_t wishdir, float wishspeed, float accel, int clientId)
{
	int i;
	float addspeed, accelspeed, currentspeed;

	/* mortos não aceleram */
	if (pmove[clientId].dead)
		return;

	if (pmove[clientId].waterjumptime)
		return;

	currentspeed = DotProduct(pmove[clientId].velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;

	accelspeed = accel * pm_frametime * wishspeed * pmove[clientId].friction;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove[clientId].velocity[i] += accelspeed * wishdir[i];
}

void PM_AirAccelerate(vec3_t wishdir, float wishspeed, float accel, int clientId)
{
	int i;
	float addspeed, accelspeed, currentspeed, wishspd = wishspeed;

	if (pmove[clientId].dead)
		return;
	if (pmove[clientId].waterjumptime)
		return;

	if (wishspd > 30)
		wishspd = 30;

	currentspeed = DotProduct(pmove[clientId].velocity, wishdir);
	addspeed = wishspd - currentspeed;
	if (addspeed <= 0)
		return;

	accelspeed = accel * wishspeed * pm_frametime * pmove[clientId].friction;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove[clientId].velocity[i] += accelspeed * wishdir[i];
}

void PM_WaterMove(int clientId)
{
	int i;
	vec3_t wishvel;
	float wishspeed;
	vec3_t wishdir;
	vec3_t start, dest;
	vec3_t temp;
	pmtrace_t trace;

	float speed, newspeed, addspeed, accelspeed;

	/* intenção do jogador */
	for (i = 0; i < 3; i++)
		wishvel[i] = pm_forward[i] * pmove[clientId].cmd.forwardmove + pm_right[i] * pmove[clientId].cmd.sidemove;

	/* afunda se não houver outro movimento */
	if (!pmove[clientId].cmd.forwardmove && !pmove[clientId].cmd.sidemove && !pmove[clientId].cmd.upmove)
		wishvel[2] -= 60;
	else
		wishvel[2] += pmove[clientId].cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if (wishspeed > pmove[clientId].maxspeed)
	{
		VectorScale(wishvel, pmove[clientId].maxspeed / wishspeed, wishvel);
		wishspeed = pmove[clientId].maxspeed;
	}
	wishspeed *= 0.8f;

	VectorAdd(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

	/* atrito da água */
	VectorCopy(pmove[clientId].velocity, temp);
	speed = VectorNormalize(temp);
	if (speed)
	{
		newspeed = speed - pm_frametime * speed * movevars.friction * pmove[clientId].friction;

		if (newspeed < 0)
			newspeed = 0;
		VectorScale(pmove[clientId].velocity, newspeed / speed, pmove[clientId].velocity);
	}
	else
		newspeed = 0;

	/* aceleração na água */
	if (wishspeed < 0.1f)
		return;

	addspeed = wishspeed - newspeed;
	if (addspeed > 0)
	{
		VectorNormalize(wishvel);
		accelspeed = movevars.accelerate * wishspeed * pm_frametime * pmove[clientId].friction;
		if (accelspeed > addspeed)
			accelspeed = addspeed;

		for (i = 0; i < 3; i++)
			pmove[clientId].velocity[i] += accelspeed * wishvel[i];
	}

	/* supõe degrau ou rampa: desce a partir da altura do degrau */
	VectorMA(pmove[clientId].origin, pm_frametime, pmove[clientId].velocity, dest);
	VectorCopy(dest, start);
	start[2] += movevars.stepsize + 1;
	trace = PM_PlayerTrace(start, dest, PM_NORMAL, clientId);
	if (!trace.startsolid && !trace.allsolid)
	{
		VectorCopy(trace.endpos, pmove[clientId].origin);
		return;
	}

	PM_FlyMove(clientId);
}

void PM_AirMove(int clientId)
{
	int i;
	vec3_t wishvel;
	float fmove, smove;
	vec3_t wishdir;
	float wishspeed;

	fmove = pmove[clientId].cmd.forwardmove;
	smove = pmove[clientId].cmd.sidemove;

	pm_forward[2] = 0;
	pm_right[2] = 0;

	VectorNormalize(pm_forward);
	VectorNormalize(pm_right);

	for (i = 0; i < 2; i++)
	{
		wishvel[i] = pm_forward[i] * fmove + pm_right[i] * smove;
	}
	wishvel[2] = 0;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if (wishspeed > pmove[clientId].maxspeed)
	{
		VectorScale(wishvel, pmove[clientId].maxspeed / wishspeed, wishvel);
		wishspeed = pmove[clientId].maxspeed;
	}

	PM_AirAccelerate(wishdir, wishspeed, movevars.airaccelerate, clientId);

	VectorAdd(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

	PM_FlyMove(clientId);
}

void PM_CatagorizePosition(int clientId)
{
	vec3_t point;
	pmtrace_t tr;

	PM_CheckWater(clientId);

	point[0] = pmove[clientId].origin[0];
	point[1] = pmove[clientId].origin[1];
	point[2] = pmove[clientId].origin[2] - 2;

	if (pmove[clientId].velocity[2] > 180) /* subindo rápido: não está no chão */
	{
		pm_onground[clientId] = -1;
	}
	else
	{
		tr = PM_PlayerTrace(pmove[clientId].origin, point, PM_NORMAL, clientId);
		if (tr.plane.normal[2] < 0.7f)
			pm_onground[clientId] = -1; /* íngreme demais */
		else
			pm_onground[clientId] = tr.ent;

		if (pm_onground[clientId] != -1)
		{
			pmove[clientId].waterjumptime = 0;
			if (pm_waterlevel[clientId] < 2 && !tr.startsolid && !tr.allsolid)
				VectorCopy(tr.endpos, pmove[clientId].origin);
		}

		if (tr.ent > 0)
		{
			PM_AddToTouched(tr, pmove[clientId].velocity, clientId);
		}
	}
}

void PM_Jump(int clientId)
{
	if (pmove[clientId].dead)
	{
		pmove[clientId].oldbuttons |= IN_JUMP; /* não pula de novo até soltar */
		return;
	}

	if (pmove[clientId].waterjumptime)
	{
		pmove[clientId].waterjumptime -= pm_frametime;
		if (pmove[clientId].waterjumptime < 0)
			pmove[clientId].waterjumptime = 0;
		return;
	}

	/* nadando, não pulando */
	if (pm_waterlevel[clientId] >= 2)
	{
		pm_onground[clientId] = -1;

		if (pm_watertype[clientId] == CONTENTS_WATER)
			pmove[clientId].velocity[2] = 100;
		else if (pm_watertype[clientId] == CONTENTS_SLIME)
			pmove[clientId].velocity[2] = 80;
		else
			pmove[clientId].velocity[2] = 50;
		return;
	}

	if (pm_onground[clientId] == -1)
	{
		pmove[clientId].oldbuttons |= IN_JUMP;
		return;
	}

	if (pmove[clientId].oldbuttons & IN_JUMP)
		return; /* sem pula-pula */

	pm_onground[clientId] = -1;

	if (pmove[clientId].usehull == 1)
		pmove[clientId].velocity[2] += sqrtf(2 * 800 * 45.0f);
	else
		pmove[clientId].velocity[2] += sqrtf(2 * 800 * 45.0f);

	pmove[clientId].oldbuttons |= IN_JUMP;
}

void PM_CheckWaterJump(int clientId)
{
	vec3_t spot;
	int cont;
	vec3_t flatforward;

	if (pmove[clientId].waterjumptime)
		return;

	/* não pula para fora se acabou de entrar */
	if (pmove[clientId].velocity[2] < -180)
		return;

	/* vê se está perto de uma borda */
	flatforward[0] = pm_forward[0];
	flatforward[1] = pm_forward[1];
	flatforward[2] = 0;
	VectorNormalize(flatforward);

	VectorMA(pmove[clientId].origin, 24, flatforward, spot);
	spot[2] += 8;
	cont = PM_PointContents(spot, clientId);
	if (cont != CONTENTS_SOLID)
		return;
	spot[2] += 24;
	cont = PM_PointContents(spot, clientId);
	if (cont != CONTENTS_EMPTY)
		return;

	/* pula para fora da água */
	VectorScale(pm_forward, 200, pmove[clientId].velocity);
	pmove[clientId].velocity[2] = 225;
	pmove[clientId].waterjumptime = 2;
	pmove[clientId].oldbuttons |= IN_JUMP;
}

qboolean PM_CheckWater(int clientId)
{
	vec3_t point;
	int cont;
	int truecont;
	float height;
	float heightover2;

	/* um ponto logo acima dos pés */
	point[0] = pmove[clientId].origin[0] + (player_mins[pmove[clientId].usehull][0] + player_maxs[pmove[clientId].usehull][0]) * 0.5f;
	point[1] = pmove[clientId].origin[1] + (player_mins[pmove[clientId].usehull][1] + player_maxs[pmove[clientId].usehull][1]) * 0.5f;
	point[2] = pmove[clientId].origin[2] + player_mins[pmove[clientId].usehull][2] + 1;

	pm_waterlevel[clientId] = 0;
	pm_watertype[clientId] = CONTENTS_EMPTY;

	cont = PM_PointContents(point, clientId);
	if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
	{
		truecont = PM_TruePointContents(point, clientId);

		pm_watertype[clientId] = cont;
		pm_waterlevel[clientId] = 1;

		height = (player_mins[pmove[clientId].usehull][2] + player_maxs[pmove[clientId].usehull][2]);
		heightover2 = height * 0.5f;

		point[2] = pmove[clientId].origin[2] + heightover2;
		cont = PM_PointContents(point, clientId);
		if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
		{
			pm_waterlevel[clientId] = 2;

			point[2] = pmove[clientId].origin[2] + pmove[clientId].view_ofs[2];

			cont = PM_PointContents(point, clientId);
			if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
				pm_waterlevel[clientId] = 3;
		}

		/* correnteza: quanto mais fundo, mais forte */
		if ((truecont <= CONTENTS_CURRENT_0) && (truecont >= CONTENTS_CURRENT_DOWN))
		{
			static vec3_t current_table[] =
			{
				{ 1, 0, 0 }, { 0, 1, 0 }, { -1, 0, 0 },
				{ 0, -1, 0 }, { 0, 0, 1 }, { 0, 0, -1 }
			};

			VectorMA(pmove[clientId].basevelocity, 50.0f * pm_waterlevel[clientId], current_table[CONTENTS_CURRENT_0 - truecont], pmove[clientId].basevelocity);
		}
	}

	return pm_waterlevel[clientId] > 1;
}

void PM_CreateStuckTable(void)
{
	float x, y, z;
	int idx;
	int i;
	float zi[3];

	memset(rgv3tStuckTable, 0, 54 * sizeof(vec3_t));

	idx = 0;
	/* movimentos pequenos */
	x = y = 0;
	/* em z */
	for (z = -0.124f; z <= 0.124f; z += 0.124f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	x = z = 0;
	/* em y */
	for (y = -0.124f; y <= 0.124f; y += 0.124f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	y = z = 0;
	/* em x */
	for (x = -0.124f; x <= 0.124f; x += 0.124f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	/* os demais, em vários eixos */
	for (x = -0.124f; x <= 0.124f; x += 0.248f)
	{
		for (y = -0.124f; y <= 0.124f; y += 0.248f)
		{
			for (z = -0.124f; z <= 0.124f; z += 0.248f)
			{
				rgv3tStuckTable[idx][0] = x;
				rgv3tStuckTable[idx][1] = y;
				rgv3tStuckTable[idx][2] = z;
				idx++;
			}
		}
	}

	/* movimentos grandes */
	x = y = 0;
	zi[0] = 0.0f;
	zi[1] = 1.0f;
	zi[2] = 6.0f;

	for (i = 0; i < 3; i++)
	{
		z = zi[i];
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	x = z = 0;

	for (y = -2.0f; y <= 2.0f; y += 2.0f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}
	y = z = 0;
	for (x = -2.0f; x <= 2.0f; x += 2.0f)
	{
		rgv3tStuckTable[idx][0] = x;
		rgv3tStuckTable[idx][1] = y;
		rgv3tStuckTable[idx][2] = z;
		idx++;
	}

	for (i = 0; i < 3; i++)
	{
		z = zi[i];
		for (x = -2.0f; x <= 2.0f; x += 2.0f)
		{
			for (y = -2.0f; y <= 2.0f; y += 2.0f)
			{
				rgv3tStuckTable[idx][0] = x;
				rgv3tStuckTable[idx][1] = y;
				rgv3tStuckTable[idx][2] = z;
				idx++;
			}
		}
	}
}

int PM_GetRandomStuckOffsets(int nIndex, int server, vec3_t offset)
{
	int idx;

	idx = rgStuckLast[nIndex][server]++;

	VectorCopy(rgv3tStuckTable[idx % 54], offset);

	return (idx % 54);
}

void PM_ResetStuckOffsets(int nIndex, int server)
{
	rgStuckLast[nIndex][server] = 0;
}

extern float pm_stucktime[3];
extern float rgStuckCheckTime[][2];
extern pmtrace_t pm_stucktrace;
extern edict_t *sv_player;
extern byte *D_003329F4;
extern void (*gEntityInterface_pfnTouch)(edict_t *pentTouched, edict_t *pentOther);
float Sys_FloatTime(void);
int PM_TestPlayerPosition(vec3_t pos, int clientId);
edict_t *EDICT_NUM(int n);
void SV_SetGlobalTrace(trace_t *ptrace);

int PM_CheckStuck(int clientId)
{
	vec3_t base;
	vec3_t offset;
	vec3_t test;
	int hitent;
	int idx;
	float fTime;
	int i;
	int info;
	edict_t *ent;
	trace_t trace;
	vec3_t oldvel;

	pm_stucktime[0] = Sys_FloatTime();
	hitent = PM_TestPlayerPosition(pmove[clientId].origin, clientId);
	pm_stucktime[1] = Sys_FloatTime();
	if (hitent == -1)
	{
		PM_ResetStuckOffsets(pmove[clientId].player_index, pmove[clientId].server);
		return 0;
	}

	VectorCopy(pmove[clientId].origin, base);

	if (!pmove[clientId].server)
	{
		if (hitent == 0 || pmove[clientId].physents[hitent].model)
		{
			int nReps = 0;
			PM_ResetStuckOffsets(pmove[clientId].player_index, pmove[clientId].server);
			do
			{
				i = PM_GetRandomStuckOffsets(pmove[clientId].player_index, pmove[clientId].server, offset);
				VectorAdd(base, offset, test);
				if (PM_TestPlayerPosition(test, clientId) == -1)
				{
					PM_ResetStuckOffsets(pmove[clientId].player_index, pmove[clientId].server);
					VectorCopy(test, pmove[clientId].origin);
					return 0;
				}
				nReps++;
			} while (nReps < 54);
		}
	}

	idx = pmove[clientId].server ? 0 : 1;
	fTime = Sys_FloatTime();
	if (rgStuckCheckTime[pmove[clientId].player_index][idx] >= fTime - 0.05f)
		return 1;
	rgStuckCheckTime[pmove[clientId].player_index][idx] = fTime;

	if (pmove[clientId].server && !pmove[clientId].spectator && !pm_nostucktouch.value)
	{
		info = pmove[clientId].physents[hitent].info;
		ent = EDICT_NUM(info);
		if (!(ent->v.flags & FL_SPECTATOR) && !((D_003329F4[info / 8] >> (info % 8)) & 1))
		{
			trace.allsolid = pm_stucktrace.allsolid;
			trace.startsolid = pm_stucktrace.startsolid;
			trace.inopen = pm_stucktrace.inopen;
			trace.inwater = pm_stucktrace.inwater;
			trace.fraction = pm_stucktrace.fraction;
			VectorCopy(pm_stucktrace.endpos, trace.endpos);
			VectorCopy(pm_stucktrace.plane.normal, trace.plane.normal);
			trace.plane.dist = pm_stucktrace.plane.dist;
			trace.ent = ent;
			trace.hitgroup = pm_stucktrace.hitgroup;
			VectorCopy(sv_player->v.velocity, oldvel);
			VectorCopy(pmove[clientId].velocity, sv_player->v.velocity);
			SV_SetGlobalTrace(&trace);
			gEntityInterface_pfnTouch(ent, sv_player);
			VectorCopy(oldvel, sv_player->v.velocity);
		}
	}

	i = PM_GetRandomStuckOffsets(pmove[clientId].player_index, pmove[clientId].server, offset);
	VectorAdd(base, offset, test);
	if (PM_TestPlayerPosition(test, clientId) == -1)
	{
		PM_ResetStuckOffsets(pmove[clientId].player_index, pmove[clientId].server);
		if (i >= 27)
			VectorCopy(test, pmove[clientId].origin);
		return 0;
	}
	pm_stucktime[2] = Sys_FloatTime();
	return 1;
}

void PM_SpectatorMove(int clientId)
{
	float speed, drop, friction, control, newspeed;
	float currentspeed, addspeed, accelspeed;
	int i;
	vec3_t wishvel;
	float fmove, smove;
	vec3_t wishdir;
	float wishspeed;

	speed = Length(pmove[clientId].velocity);
	if (speed < 1)
	{
		VectorCopy(vec3_origin, pmove[clientId].velocity);
	}
	else
	{
		drop = 0;
		friction = movevars.friction * 1.5f;
		control = speed < movevars.stopspeed ? movevars.stopspeed : speed;
		drop += control * friction * pm_frametime;
		newspeed = speed - drop;
		if (newspeed < 0)
			newspeed = 0;
		newspeed /= speed;
		VectorScale(pmove[clientId].velocity, newspeed, pmove[clientId].velocity);
	}

	fmove = pmove[clientId].cmd.forwardmove;
	smove = pmove[clientId].cmd.sidemove;
	VectorNormalize(pm_forward);
	VectorNormalize(pm_right);

	for (i = 0; i < 3; i++)
		wishvel[i] = pm_forward[i] * fmove + pm_right[i] * smove;
	wishvel[2] += pmove[clientId].cmd.upmove;

	VectorCopy(wishvel, wishdir);
	wishspeed = VectorNormalize(wishdir);

	if (wishspeed > movevars.spectatormaxspeed)
	{
		VectorScale(wishvel, movevars.spectatormaxspeed / wishspeed, wishvel);
		wishspeed = movevars.spectatormaxspeed;
	}

	currentspeed = DotProduct(pmove[clientId].velocity, wishdir);
	addspeed = wishspeed - currentspeed;
	if (addspeed <= 0)
		return;
	accelspeed = movevars.accelerate * pm_frametime * wishspeed;
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	for (i = 0; i < 3; i++)
		pmove[clientId].velocity[i] += accelspeed * wishdir[i];

	VectorMA(pmove[clientId].origin, pm_frametime, pmove[clientId].velocity, pmove[clientId].origin);
}

void PM_NoClip(int clientId)
{
	int i;
	vec3_t wishvel;
	float fmove, smove;

	fmove = pmove[clientId].cmd.forwardmove;
	smove = pmove[clientId].cmd.sidemove;

	VectorNormalize(pm_forward);
	VectorNormalize(pm_right);

	for (i = 0; i < 3; i++)
		wishvel[i] = pm_forward[i] * fmove + pm_right[i] * smove;
	wishvel[2] += pmove[clientId].cmd.upmove;

	VectorMA(pmove[clientId].origin, pm_frametime, wishvel, pmove[clientId].origin);

	VectorCopy(vec3_origin, pmove[clientId].velocity);
}

void PM_WaterJump(int clientId)
{
	if (pmove[clientId].waterjumptime > 2)
		pmove[clientId].waterjumptime = 2;

	if (!pmove[clientId].waterjumptime)
		return;

	pmove[clientId].waterjumptime -= pm_frametime;
	if (pmove[clientId].waterjumptime <= 0 || !pm_waterlevel[clientId])
	{
		pmove[clientId].waterjumptime = 0;
		pmove[clientId].flags &= ~FL_WATERJUMP;
	}

	pmove[clientId].velocity[0] = pmove[clientId].movedir[0];
	pmove[clientId].velocity[1] = pmove[clientId].movedir[1];
}

void PM_AddGravity(int clientId)
{
	float ent_gravity;

	if (pmove[clientId].gravity)
		ent_gravity = pmove[clientId].gravity;
	else
		ent_gravity = 1.0f;

	pmove[clientId].velocity[2] -= (ent_gravity * movevars.gravity * pm_frametime);
	pmove[clientId].velocity[2] += pmove[clientId].basevelocity[2] * pm_frametime;
	pmove[clientId].basevelocity[2] = 0;
	PM_CheckVelocity(clientId);
}

pmtrace_t PM_PushEntity(vec3_t push, int clientId)
{
	pmtrace_t trace;
	vec3_t end;

	VectorAdd(pmove[clientId].origin, push, end);

	trace = PM_PlayerTrace(pmove[clientId].origin, end, PM_NORMAL, clientId);

	VectorCopy(trace.endpos, pmove[clientId].origin);

	if (trace.fraction < 1.0f && !trace.allsolid)
		PM_AddToTouched(trace, pmove[clientId].velocity, clientId);

	return trace;
}

void PM_Physics_Toss(int clientId)
{
	pmtrace_t trace;
	vec3_t move;
	float backoff;

	PM_CheckWater(clientId);

	if (pmove[clientId].velocity[2] > 0)
		pm_onground[clientId] = -1;

	if (pm_onground[clientId] != -1)
	{
		if (VectorCompare(pmove[clientId].basevelocity, vec3_origin) &&
		    VectorCompare(pmove[clientId].velocity, vec3_origin))
			return;
	}

	PM_CheckVelocity(clientId);

	if (pmove[clientId].movetype != MOVETYPE_FLY &&
	    pmove[clientId].movetype != MOVETYPE_BOUNCEMISSILE &&
	    pmove[clientId].movetype != MOVETYPE_FLYMISSILE)
		PM_AddGravity(clientId);

	VectorAdd(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

	PM_CheckVelocity(clientId);
	VectorScale(pmove[clientId].velocity, pm_frametime, move);
	VectorSubtract(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

	trace = PM_PushEntity(move, clientId);

	PM_CheckVelocity(clientId);

	if (trace.allsolid)
	{
		pm_onground[clientId] = trace.ent;
		VectorCopy(vec3_origin, pmove[clientId].velocity);
		return;
	}

	if (trace.fraction == 1)
	{
		PM_CheckWater(clientId);
		return;
	}

	if (pmove[clientId].movetype == MOVETYPE_BOUNCE)
		backoff = 2.0f - pmove[clientId].friction;
	else if (pmove[clientId].movetype == MOVETYPE_BOUNCEMISSILE)
		backoff = 2.0f;
	else
		backoff = 1;

	PM_ClipVelocity(pmove[clientId].velocity, trace.plane.normal, pmove[clientId].velocity, backoff);

	if (trace.plane.normal[2] > 0.7f)
	{
		float vel;
		vec3_t base;

		VectorCopy(vec3_origin, base);
		if (pmove[clientId].velocity[2] < movevars.gravity * pm_frametime)
		{
			pm_onground[clientId] = trace.ent;
			pmove[clientId].velocity[2] = 0;
		}

		vel = DotProduct(pmove[clientId].velocity, pmove[clientId].velocity);

		if (vel < (30 * 30) || (pmove[clientId].movetype != MOVETYPE_BOUNCE && pmove[clientId].movetype != MOVETYPE_BOUNCEMISSILE))
		{
			pm_onground[clientId] = trace.ent;
			VectorCopy(vec3_origin, pmove[clientId].velocity);
		}
		else
		{
			VectorScale(pmove[clientId].velocity, (1.0f - trace.fraction) * pm_frametime * 0.9f, move);
			trace = PM_PushEntity(move, clientId);
		}
		VectorSubtract(pmove[clientId].velocity, base, pmove[clientId].velocity);
	}

	PM_CheckWater(clientId);
}

hull_t *PM_HullForBsp(physent_t *pe, vec3_t offset, int clientId);
int PM_HullPointContents(hull_t *hull, int num, vec3_t p);
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);

physent_t *PM_Ladder(int clientId)
{
	int i;
	physent_t *pe;
	hull_t *hull;
	int num;
	vec3_t test;

	for (i = 0; i < pmove[clientId].nummoveent; i++)
	{
		pe = &pmove[clientId].moveents[i];

		if (pe->model && pe->model->type == mod_brush && pe->skin == CONTENTS_LADDER)
		{
			hull = PM_HullForBsp(pe, test, clientId);
			num = hull->firstclipnode;

			VectorSubtract(pmove[clientId].origin, test, test);

			if (PM_HullPointContents(hull, num, test) == CONTENTS_EMPTY)
				continue;

			return pe;
		}
	}

	return NULL;
}

float PM_TraceModel(physent_t *pEnt, float *start, float *end, trace_t *trace, int clientId)
{
	hull_t *pHull;
	int saveHull;
	vec3_t start_l, end_l;
	vec3_t offset;

	saveHull = pmove[clientId].usehull;
	pmove[clientId].usehull = 2;
	pHull = PM_HullForBsp(pEnt, offset, clientId);
	pmove[clientId].usehull = saveHull;

	VectorSubtract(start, offset, start_l);
	VectorSubtract(end, offset, end_l);

	SV_RecursiveHullCheck(pHull, pHull->firstclipnode, 0, 1, start_l, end_l, trace);

	return trace->fraction;
}

#define MAX_CLIMB_SPEED 200

extern int D_003316B0[];
extern int D_003316B8[];
extern float D_00399158[];
extern float D_00399148[];

void PM_LadderMove(physent_t *pLadder, int clientId)
{
	vec3_t ladderCenter;
	trace_t trace;
	qboolean onFloor;
	vec3_t floor;

	if (pmove[clientId].movetype == MOVETYPE_NOCLIP)
		return;

	VectorAdd(pLadder->model->mins, pLadder->model->maxs, ladderCenter);
	VectorScale(ladderCenter, 0.5f, ladderCenter);

	pmove[clientId].movetype = MOVETYPE_FLY;

	VectorCopy(pmove[clientId].origin, floor);
	floor[2] += player_mins[pmove[clientId].usehull][2] - 1;

	if (PM_PointContents(floor, clientId) == CONTENTS_SOLID)
		onFloor = 1;
	else
		onFloor = 0;

	pmove[clientId].gravity = 0;
	PM_TraceModel(pLadder, pmove[clientId].origin, ladderCenter, &trace, clientId);
	if (trace.fraction != 1.0f)
	{
		float forward = 0, right = 0;
		vec3_t vpn, v_right;

		AngleVectors(pmove[clientId].angles, vpn, v_right, NULL);
		if (pmove[clientId].cmd.buttons & IN_BACK)
			forward -= (D_003316B0[clientId] == 1) ? D_00399158[clientId] * MAX_CLIMB_SPEED : MAX_CLIMB_SPEED;
		if (pmove[clientId].cmd.buttons & IN_FORWARD)
			forward += (D_003316B0[clientId] == 1) ? D_00399158[clientId] * MAX_CLIMB_SPEED : MAX_CLIMB_SPEED;
		if (pmove[clientId].cmd.buttons & IN_MOVELEFT)
			right -= (D_003316B8[clientId] == 1) ? D_00399148[clientId] * 50 : 50;
		if (pmove[clientId].cmd.buttons & IN_MOVERIGHT)
			right += (D_003316B8[clientId] == 1) ? D_00399148[clientId] * 50 : 50;

		if (pmove[clientId].cmd.buttons & IN_JUMP)
		{
			pmove[clientId].movetype = MOVETYPE_WALK;
			VectorScale(trace.plane.normal, 270, pmove[clientId].velocity);
		}
		else
		{
			if (forward != 0 || right != 0)
			{
				vec3_t velocity, perp, cross, lateral, tmp;
				float normal;

				VectorScale(vpn, forward, velocity);
				VectorMA(velocity, right, v_right, velocity);

				VectorCopy(vec3_origin, tmp);
				tmp[2] = 1;
				CrossProduct(tmp, trace.plane.normal, perp);
				VectorNormalize(perp);

				normal = DotProduct(velocity, trace.plane.normal);
				VectorScale(trace.plane.normal, normal, cross);

				VectorSubtract(velocity, cross, lateral);

				CrossProduct(trace.plane.normal, perp, tmp);
				VectorMA(lateral, -normal, tmp, pmove[clientId].velocity);
				if (onFloor && normal > 0)
				{
					VectorMA(pmove[clientId].velocity, MAX_CLIMB_SPEED, trace.plane.normal, pmove[clientId].velocity);
				}
			}
			else
			{
				VectorCopy(vec3_origin, pmove[clientId].velocity);
			}
		}
	}
}

void PM_FixPlayerCrouchStuck(int clientId)
{
	int i;
	vec3_t test;

	if (PM_TestPlayerPosition(pmove[clientId].origin, clientId) == -1)
		return;

	VectorCopy(pmove[clientId].origin, test);
	for (i = 0; i < 18; i++)
	{
		pmove[clientId].origin[2] += 1;
		if (PM_TestPlayerPosition(pmove[clientId].origin, clientId) == -1)
		{
			Con_Printf(" fix %i", i);
			return;
		}
	}

	Con_Printf(" stuck");
	VectorCopy(test, pmove[clientId].origin);
}

#define PLAYER_DUCKING_MULTIPLIER 0.333f
#define TIME_TO_DUCK 0.4f
#define VEC_DUCK_VIEW 12
#define VEC_VIEW 28

void PM_Duck(int clientId)
{
	int i;
	float time;
	float duckFraction;

	int buttonsChanged = (pmove[clientId].oldbuttons ^ pmove[clientId].cmd.buttons);
	int nButtonPressed = buttonsChanged & pmove[clientId].cmd.buttons;

	int duckchange = buttonsChanged & IN_DUCK ? 1 : 0;
	int duckpressed = nButtonPressed & IN_DUCK ? 1 : 0;

	Con_Printf("btn %i bInDuck %i FL_DUCKING %i ofs %.2f h %i ch %i pr %i",
	           pmove[clientId].cmd.buttons & IN_DUCK ? 1 : 0,
	           pmove[clientId].bInDuck ? 1 : 0,
	           pmove[clientId].flags & FL_DUCKING ? 1 : 0,
	           pmove[clientId].view_ofs[2],
	           pmove[clientId].usehull,
	           duckchange,
	           duckpressed);

	if ((pmove[clientId].cmd.buttons & IN_DUCK) || pmove[clientId].bInDuck || (pmove[clientId].flags & FL_DUCKING))
	{
		if (pmove[clientId].cmd.buttons & IN_DUCK)
		{
			pmove[clientId].cmd.forwardmove *= PLAYER_DUCKING_MULTIPLIER;
			pmove[clientId].cmd.sidemove *= PLAYER_DUCKING_MULTIPLIER;
			pmove[clientId].cmd.upmove *= PLAYER_DUCKING_MULTIPLIER;

			if ((nButtonPressed & IN_DUCK) && !(pmove[clientId].flags & FL_DUCKING))
			{
				pmove[clientId].flDuckTime = pmove[clientId].time;
				pmove[clientId].bInDuck = 1;
				Con_Printf("(new)");
			}

			time = pmove[clientId].time - pmove[clientId].flDuckTime;
			time *= 0.001f;
			Con_Printf("e %.2f", time);

			if (pmove[clientId].bInDuck)
			{
				if (time >= TIME_TO_DUCK || pm_onground[clientId] == -1)
				{
					if (pm_onground[clientId] != -1)
					{
						for (i = 0; i < 3; i++)
							pmove[clientId].origin[i] -= (player_mins[1][i] - player_mins[0][i]);
						PM_FixPlayerCrouchStuck(clientId);
					}
					pmove[clientId].usehull = 1;
					pmove[clientId].view_ofs[2] = VEC_DUCK_VIEW;
					pmove[clientId].flags |= FL_DUCKING;
					pmove[clientId].bInDuck = 0;
				}
				else
				{
					float fMore = 0;

					if (pmove[clientId].server)
						fMore = 18;

					duckFraction = PM_SplineFraction(time, (1.0f / TIME_TO_DUCK));
					pmove[clientId].view_ofs[2] = ((VEC_DUCK_VIEW - fMore) * duckFraction) + (VEC_VIEW * (1 - duckFraction));
				}
			}
		}
		else
		{
			pmtrace_t trace;
			vec3_t newOrigin;

			VectorCopy(pmove[clientId].origin, newOrigin);

			if (pm_onground[clientId] != -1)
			{
				for (i = 0; i < 3; i++)
					newOrigin[i] += (player_mins[1][i] - player_mins[0][i]);
			}

			trace = PM_PlayerTrace(newOrigin, newOrigin, PM_NORMAL, clientId);

			if (!trace.startsolid)
			{
				pmove[clientId].flags &= ~FL_DUCKING;
				pmove[clientId].bInDuck = 0;
				pmove[clientId].view_ofs[2] = VEC_VIEW;
				pmove[clientId].usehull = 0;
				VectorCopy(newOrigin, pmove[clientId].origin);
			}
			else
			{
				Con_Printf("Stuck on up\n");
			}
		}
	}

	Con_Printf("\n");
}

extern cvar_t cl_dumpents;
extern vec3_t pm_sv_origin;
extern vec3_t pm_cl_origin;

void PM_PlayerMove(qboolean server, int clientId)
{
	physent_t *pLadder = NULL;

	pmove[clientId].server = server;

	pm_frametime = pmove[clientId].cmd.msec * 0.001f;

	pmove[clientId].numtouch = 0;

	AngleVectors(pmove[clientId].angles, pm_forward, pm_right, pm_up);

	if (!server && cl_dumpents.value)
	{
		Cvar_SetValue("cl_dumpents", 0);
		PM_PrintPhysEnts();
	}

	if (pmove[clientId].spectator)
	{
		PM_SpectatorMove(clientId);
		return;
	}

	if (pmove[clientId].movetype != MOVETYPE_NOCLIP)
	{
		if (PM_CheckStuck(clientId))
			return;
	}

	PM_CatagorizePosition(clientId);

	if (pmove[clientId].server)
	{
		VectorCopy(pmove[clientId].origin, pm_sv_origin);
	}
	else
	{
		VectorCopy(pmove[clientId].origin, pm_cl_origin);
	}

	if (!pmove[clientId].dead && !(pmove[clientId].flags & FL_ONTRAIN))
	{
		pLadder = PM_Ladder(clientId);
		if (pLadder)
		{
			PM_LadderMove(pLadder, clientId);
		}
		else if (pmove[clientId].movetype != MOVETYPE_WALK &&
		         pmove[clientId].movetype != MOVETYPE_NOCLIP)
		{
			pmove[clientId].movetype = MOVETYPE_WALK;
		}
	}

	if (pm_onground[clientId] != -1 && (pmove[clientId].cmd.buttons & IN_USE))
		VectorScale(pmove[clientId].velocity, 0.3f, pmove[clientId].velocity);

	switch (pmove[clientId].movetype)
	{
	default:
		Con_DPrintf("Bogus pmove[clientId] player movetype %i on (%i) 0=cl 1=sv\n", pmove[clientId].movetype, pmove[clientId].server);
		break;

	case MOVETYPE_NONE:
		break;

	case MOVETYPE_NOCLIP:
		PM_NoClip(clientId);
		break;

	case MOVETYPE_TOSS:
	case MOVETYPE_BOUNCE:
		PM_Physics_Toss(clientId);
		break;

	case MOVETYPE_FLY:
		PM_CheckWater(clientId);

		if (pmove[clientId].cmd.buttons & IN_JUMP)
		{
			if (!pLadder)
				PM_Jump(clientId);
		}
		else
		{
			pmove[clientId].oldbuttons &= ~IN_JUMP;
		}

		VectorAdd(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);
		PM_FlyMove(clientId);
		VectorSubtract(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);
		break;

	case MOVETYPE_WALK:
		if (!PM_CheckWater(clientId) && !pmove[clientId].waterjumptime)
			PM_AddCorrectGravity(clientId);

		PM_CheckVelocity(clientId);

		PM_CatagorizePosition(clientId);

		if (pmove[clientId].waterjumptime)
		{
			PM_WaterJump(clientId);
			PM_FlyMove(clientId);
			return;
		}

		if (pm_waterlevel[clientId] >= 2)
		{
			if (pm_waterlevel[clientId] == 2 && !pmove[clientId].waterjumptime)
				PM_CheckWaterJump(clientId);

			if (pmove[clientId].velocity[2] < 0 && pmove[clientId].waterjumptime)
				pmove[clientId].waterjumptime = 0;

			if (pmove[clientId].cmd.buttons & IN_JUMP)
				PM_Jump(clientId);
			else
				pmove[clientId].oldbuttons &= ~IN_JUMP;

			PM_WaterMove(clientId);

			VectorSubtract(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);
		}
		else
		{
			if (pm_onground[clientId] != -1)
			{
				pmove[clientId].velocity[2] = 0;
				PM_Friction(clientId);
			}

			if (pmove[clientId].cmd.buttons & IN_JUMP)
			{
				if (!pLadder)
					PM_Jump(clientId);
			}
			else
			{
				pmove[clientId].oldbuttons &= ~IN_JUMP;
			}

			PM_CheckVelocity(clientId);

			PM_CatagorizePosition(clientId);

			if (pm_onground[clientId] != -1)
				PM_WalkMove(clientId);
			else
				PM_AirMove(clientId);

			VectorSubtract(pmove[clientId].velocity, pmove[clientId].basevelocity, pmove[clientId].velocity);

			PM_CheckVelocity(clientId);

			if (!PM_CheckWater(clientId) && !pmove[clientId].waterjumptime)
				PM_FixupGravityVelocity(clientId);

			PM_CatagorizePosition(clientId);

			if (pm_onground[clientId] != -1)
				pmove[clientId].velocity[2] = 0;
		}
		break;
	}
}

void SV_Impact(edict_t *e1, edict_t *e2, trace_t *ptrace);

qboolean PM_AddToTouched(pmtrace_t tr, vec3_t impactvelocity, int clientId)
{
	int i;

	for (i = 0; i < pmove[clientId].numtouch; i++)
	{
		if (pmove[clientId].touchindex[i].ent == tr.ent)
			break;
	}
	if (i != pmove[clientId].numtouch)
		return 0;

	if (pm_pushfix.value)
	{
		if (pmove[clientId].server)
		{
			trace_t trace;
			edict_t *ent;
			vec3_t oldvel;

			trace.allsolid = tr.allsolid;
			trace.startsolid = tr.startsolid;
			trace.inopen = tr.inopen;
			trace.inwater = tr.inwater;
			trace.fraction = tr.fraction;
			VectorCopy(tr.endpos, trace.endpos);
			VectorCopy(tr.plane.normal, trace.plane.normal);
			trace.plane.dist = tr.plane.dist;
			ent = EDICT_NUM(pmove[clientId].physents[pmove[clientId].touchindex[i].ent].info);
			trace.ent = ent;
			trace.hitgroup = tr.hitgroup;

			VectorCopy(pmove[clientId].velocity, oldvel);
			VectorCopy(impactvelocity, sv_player->v.velocity);
			if (!(ent->v.flags & FL_SPECTATOR) && !pmove[clientId].spectator)
				SV_Impact(ent, sv_player, &trace);
			VectorSubtract(sv_player->v.velocity, oldvel, tr.deltavelocity);
			VectorCopy(oldvel, pmove[clientId].velocity);
		}
		else
		{
			VectorCopy(vec3_origin, tr.deltavelocity);
		}
	}
	else
	{
		VectorCopy(impactvelocity, tr.deltavelocity);
	}

	if (pmove[clientId].numtouch >= MAX_PHYSENTS)
		Con_DPrintf("Too many entities were touched!\n");

	pmove[clientId].touchindex[pmove[clientId].numtouch++] = tr;
	return 1;
}
