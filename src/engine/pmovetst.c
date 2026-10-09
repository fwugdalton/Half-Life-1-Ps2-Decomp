#include "common.h"
#include "engine/model.h"
#include "engine/pmove.h"
#include "engine/server.h"

extern hull_t box_hull;
extern dclipnode_t box_clipnodes[6];
extern mplane_t box_planes[6];

void PM_InitBoxHull(void)
{
	int i;
	int side;

	box_hull.clipnodes = box_clipnodes;
	box_hull.planes = box_planes;
	box_hull.firstclipnode = 0;
	box_hull.lastclipnode = 5;

	for (i = 0; i < 6; i++)
	{
		box_clipnodes[i].planenum = i;

		side = i & 1;

		box_clipnodes[i].children[side] = CONTENTS_EMPTY;
		if (i != 5)
			box_clipnodes[i].children[side ^ 1] = i + 1;
		else
			box_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;

		box_planes[i].type = i >> 1;
		box_planes[i].normal[i >> 1] = 1;
	}
}

hull_t *PM_HullForBox(vec3_t mins, vec3_t maxs)
{
	box_planes[0].dist = 2249.0f;
	box_planes[0].dist = maxs[0];
	box_planes[1].dist = mins[0];
	box_planes[2].dist = maxs[1];
	box_planes[3].dist = mins[1];
	box_planes[4].dist = maxs[2];
	box_planes[5].dist = mins[2];

	return &box_hull;
}

int PM_HullPointContents(hull_t *hull, int num, vec3_t p)
{
	float d;
	dclipnode_t *node;
	mplane_t *plane;

	if (hull->firstclipnode >= hull->lastclipnode)
		return -1;

	while (num >= 0)
	{
		if (num < hull->firstclipnode || num > hull->lastclipnode)
			Sys_Error("PM_HullPointContents: bad node number");

		node = hull->clipnodes + num;
		plane = hull->planes + node->planenum;

		d = DotProduct(plane->normal, p) - plane->dist;
		if (d < 0)
			num = node->children[1];
		else
			num = node->children[0];
	}

	return num;
}

int PM_LinkContents(vec3_t p, int *pIndex, int clientId)
{
	int i;
	physent_t *pe;
	model_t *model;
	hull_t *hull;
	int num;
	vec3_t test;

	for (i = 1; i < pmove[clientId].numphysent; i++)
	{
		pe = &pmove[clientId].physents[i];
		if (pe->solid)
			continue;
		model = pe->model;
		if (!model)
			continue;

		hull = model->hulls;
		num = hull->firstclipnode;
		test[0] = p[0] - pe->origin[0];
		test[1] = p[1] - pe->origin[1];
		test[2] = p[2] - pe->origin[2];

		if (PM_HullPointContents(hull, num, test) != CONTENTS_EMPTY)
		{
			if (pIndex)
				*pIndex = pe->info;
			return pe->skin;
		}
	}

	return CONTENTS_EMPTY;
}

int PM_PointContents(vec3_t p, int clientId)
{
	int cont;
	int truecont;
	hull_t *hull;

	hull = &pmove[clientId].physents[0].model->hulls[0];
	cont = PM_HullPointContents(hull, hull->firstclipnode, p);

	if (cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
		cont = CONTENTS_WATER;

	if (cont != CONTENTS_SOLID)
	{
		truecont = PM_LinkContents(p, NULL, clientId);
		if (truecont != CONTENTS_EMPTY)
			return truecont;
	}

	return cont;
}

int PM_WaterEntity(vec3_t p, int clientId)
{
	int cont;
	int entityIndex;
	hull_t *hull;

	entityIndex = -1;
	hull = &pmove[clientId].physents[0].model->hulls[0];
	cont = PM_HullPointContents(hull, hull->firstclipnode, p);

	if (cont < CONTENTS_SOLID)
		entityIndex = 0;

	if (cont != CONTENTS_SOLID)
		PM_LinkContents(p, &entityIndex, clientId);

	return entityIndex;
}

int PM_TruePointContents(vec3_t p, int clientId)
{
	hull_t *hull;

	hull = &pmove[clientId].physents[0].model->hulls[0];
	if (hull)
		return PM_HullPointContents(hull, hull->firstclipnode, p);

	return CONTENTS_EMPTY;
}

#define DIST_EPSILON (0.03125f)

qboolean PM_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, pmtrace_t *trace)
{
	dclipnode_t *node;
	mplane_t *plane;
	float t1, t2;
	float frac;
	int i;
	vec3_t mid;
	int side;
	float midf;

	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = 0;
			if (num == CONTENTS_EMPTY)
				trace->inopen = 1;
			else
				trace->inwater = 1;
		}
		else
			trace->startsolid = 1;
		return 1;
	}

	if (hull->firstclipnode >= hull->lastclipnode)
	{
		trace->allsolid = 0;
		trace->inopen = 1;
		return 1;
	}

	if (num < hull->firstclipnode || num > hull->lastclipnode)
		Sys_Error("PM_RecursiveHullCheck: bad node number");

	node = hull->clipnodes + num;
	plane = hull->planes + node->planenum;

	if (plane->type < 3)
	{
		t1 = p1[plane->type] - plane->dist;
		t2 = p2[plane->type] - plane->dist;
	}
	else
	{
		t1 = DotProduct(plane->normal, p1) - plane->dist;
		t2 = DotProduct(plane->normal, p2) - plane->dist;
	}

	if (t1 >= 0 && t2 >= 0)
		return PM_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return PM_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);

	if (t1 < 0)
		frac = (t1 + DIST_EPSILON) / (t1 - t2);
	else
		frac = (t1 - DIST_EPSILON) / (t1 - t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;

	midf = p1f + (p2f - p1f) * frac;
	for (i = 0; i < 3; i++)
		mid[i] = p1[i] + frac * (p2[i] - p1[i]);

	side = (t1 < 0);

	if (!PM_RecursiveHullCheck(hull, node->children[side], p1f, midf, p1, mid, trace))
		return 0;

	if (PM_HullPointContents(hull, node->children[side ^ 1], mid) != CONTENTS_SOLID)
		return PM_RecursiveHullCheck(hull, node->children[side ^ 1], midf, p2f, mid, p2, trace);

	if (trace->allsolid)
		return 0;

	if (!side)
	{
		VectorCopy(plane->normal, trace->plane.normal);
		trace->plane.dist = plane->dist;
	}
	else
	{
		VectorSubtract(vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.dist = -plane->dist;
	}

	while (PM_HullPointContents(hull, hull->firstclipnode, mid) == CONTENTS_SOLID)
	{
		frac -= 0.05f;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy(mid, trace->endpos);
			Con_DPrintf("Trace backed up past 0.0.\n");
			return 0;
		}
		midf = p1f + (p2f - p1f) * frac;
		for (i = 0; i < 3; i++)
			mid[i] = p1[i] + frac * (p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy(mid, trace->endpos);

	return 0;
}

hull_t *R_StudioHull(model_t *pModel, float frame, int sequence, vec3_t angles, vec3_t origin, vec3_t size, byte *pcontroller, byte *pblending, int *pNumHulls);

hull_t *PM_HullForStudioModel(model_t *pModel, vec3_t offset, float frame, int sequence, vec3_t angles, vec3_t origin, byte *pcontroller, byte *pblending, int *pNumHulls, int clientId)
{
	vec3_t size;

	VectorSubtract(player_maxs[pmove[clientId].usehull], player_mins[pmove[clientId].usehull], size);
	VectorScale(size, 0.5f, size);
	VectorCopy(vec3_origin, offset);
	return R_StudioHull(pModel, frame, sequence, angles, origin, size, pcontroller, pblending, pNumHulls);
}

hull_t *PM_HullForBsp(physent_t *pe, vec3_t offset, int clientId)
{
	hull_t *hull;

	switch (pmove[clientId].usehull)
	{
	case 0:
		hull = &pe->model->hulls[1];
		break;
	case 1:
		hull = &pe->model->hulls[3];
		break;
	case 2:
		hull = &pe->model->hulls[0];
		break;
	default:
		hull = NULL;
		break;
	}

	VectorSubtract(hull->clip_mins, player_mins[pmove[clientId].usehull], offset);
	VectorAdd(offset, pe->origin, offset);
	return hull;
}

#define SOLID_BSP 4
#define STUDIO_TRACE_HITBOX 0x200

extern int g_contentsresult;

#ifdef NON_MATCHING
int PM_TestPlayerPosition(vec3_t pos, int clientId)
{
	int i, j;
	physent_t *pe;
	hull_t *hull;
	vec3_t mins, maxs, test;
	vec3_t offset;
	int numhulls;
	int rotated;

	pm_stucktrace = PM_PlayerTrace(pmove[clientId].origin, pmove[clientId].origin, PM_NORMAL, clientId);

	for (i = 0; i < pmove[clientId].numphysent; i++)
	{
		pe = &pmove[clientId].physents[i];

		if (pe->model && !pe->solid && pe->skin)
			continue;

		VectorCopy(pe->origin, offset);
		numhulls = 1;

		if (pe->model)
		{
			hull = PM_HullForBsp(pe, offset, clientId);
		}
		else if (pe->studiomodel && pe->studiomodel->type == mod_studio &&
		         ((pe->studiomodel->flags & STUDIO_TRACE_HITBOX) || pmove[clientId].usehull == 2))
		{
			hull = PM_HullForStudioModel(pe->studiomodel, offset, pe->frame, pe->sequence, pe->angles, pe->origin,
			                             pe->controller, pe->blending, &numhulls, clientId);
		}
		else
		{
			VectorSubtract(pe->mins, player_maxs[pmove[clientId].usehull], mins);
			VectorSubtract(pe->maxs, player_mins[pmove[clientId].usehull], maxs);
			hull = PM_HullForBox(mins, maxs);
		}

		VectorSubtract(pos, offset, test);

		if (pe->solid == SOLID_BSP && (pe->angles[0] != 0 || pe->angles[1] != 0 || pe->angles[2] != 0))
			rotated = 1;
		else
			rotated = 0;

		if (rotated)
		{
			vec3_t forward, right, up;
			vec3_t temp;

			AngleVectors(pe->angles, forward, right, up);

			VectorCopy(test, temp);
			test[0] = DotProduct(temp, forward);
			test[1] = -DotProduct(temp, right);
			test[2] = DotProduct(temp, up);
		}

		if (numhulls == 1)
		{
			g_contentsresult = PM_HullPointContents(hull, hull->firstclipnode, test);
			if (g_contentsresult == CONTENTS_SOLID)
				return i;
		}
		else
		{
			for (j = 0; j < numhulls; j++)
			{
				g_contentsresult = PM_HullPointContents(&hull[j], hull[j].firstclipnode, test);
				if (g_contentsresult == CONTENTS_SOLID)
					return i;
			}
		}
	}

	return -1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", PM_TestPlayerPosition);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", PM_PlayerTrace);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_00271398);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_002713E0);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_002717C0);
