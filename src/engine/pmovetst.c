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

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_0026FDD0);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_002702A8);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", PM_HullForBsp);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", PM_TestPlayerPosition);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", PM_PlayerTrace);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_00271398);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_002713E0);

INCLUDE_ASM("asm/nonmatchings/engine/pmovetst", func_002717C0);
