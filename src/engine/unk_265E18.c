#include "common.h"
#include "engine/cvar.h"

/* contador Geiger (no PC fica no client.dll, aqui está no engine) */

typedef float vec3_t[3];
typedef struct sfx_s sfx_t;

extern sfx_t *D_00399228, *D_00399274, *D_00399270, *D_00399224, *D_003991F8, *D_00399280;
extern vec3_t r_origin;
int D_00331D80[4] = {100, 0, 0, 0};
int D_00331D90[4] = {1, 1, -1, -1};
int geiger_range = -1;
float geiger_delay = -1;

float RandomFloat(float flLow, float flHigh);
int RandomLong(int lLow, int lHigh);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int unk);

typedef struct
{
	int x, y, width, height;
} vrect_t;

/* só os campos usados aqui */
typedef struct
{
	byte pad0[0x7C];
	vec3_t viewangles;
} refdef_t;

typedef struct
{
	byte pad0[0x318];
	vec3_t punchangle;
	byte pad324[0x3A0 - 0x324];
	float time;
	byte pad3A4[0x212C - 0x3A4];
} client_state_t;

#define VectorAdd(a, b, c) {(c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2];}

extern unsigned int D_0053D1C8;
extern float D_003995E0;
extern int D_00399620;
extern cvar_t crosshair;
extern refdef_t D_004F94D0;
extern vrect_t scr_vrect;
extern client_state_t cl[2];
extern int cl_numlocal;
extern int D_00399564;
extern int D_0032EF88, D_0032EF8C;

void AngleVectors(vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
int ScreenTransform(vec3_t point, vec3_t screen);
void func_00247998(int x, int y, int nClient);
void func_002470E0(int intermission, int nClient, int flags);
void func_002A8508(int nClient, int x, int y);
void func_002A84C8(void);
void func_002A8658(void);

void CL_UpdateGeiger(void)
{
	int pct;
	float flvol;
	sfx_t *rg[3];
	int i;

	i = 0;
	flvol = 0;

	if (cl[0].time < geiger_delay)
		return;

	geiger_delay = cl[0].time + 0.1f;

	if (geiger_range < 1000 && geiger_range > 0)
	{
		if (geiger_range > 800)
		{
			pct = 0;
		}
		else if (geiger_range > 600)
		{
			pct = 2;
			flvol = 0.4;
			rg[0] = D_00399228;
			rg[1] = D_00399228;
			i = 2;
		}
		else if (geiger_range > 500)
		{
			pct = 4;
			flvol = 0.5;
			rg[0] = D_00399228;
			rg[1] = D_00399274;
			i = 2;
		}
		else if (geiger_range > 400)
		{
			pct = 8;
			flvol = 0.6;
			rg[0] = D_00399228;
			rg[1] = D_00399274;
			rg[2] = D_00399270;
			i = 3;
		}
		else if (geiger_range > 300)
		{
			pct = 8;
			flvol = 0.7;
			rg[0] = D_00399274;
			rg[1] = D_00399270;
			rg[2] = D_00399224;
			i = 3;
		}
		else if (geiger_range > 200)
		{
			pct = 28;
			flvol = 0.78;
			rg[0] = D_00399274;
			rg[1] = D_00399270;
			rg[2] = D_00399224;
			i = 3;
		}
		else if (geiger_range > 150)
		{
			pct = 40;
			flvol = 0.80;
			rg[0] = D_00399270;
			rg[1] = D_00399224;
			rg[2] = D_003991F8;
			i = 3;
		}
		else if (geiger_range > 100)
		{
			pct = 60;
			flvol = 0.85;
			rg[0] = D_00399270;
			rg[1] = D_00399224;
			rg[2] = D_003991F8;
			i = 3;
		}
		else if (geiger_range > 75)
		{
			pct = 80;
			flvol = 0.9;
			rg[0] = D_00399224;
			rg[1] = D_003991F8;
			rg[2] = D_00399280;
			i = 3;
		}
		else if (geiger_range > 50)
		{
			pct = 90;
			flvol = 0.95;
			rg[0] = D_003991F8;
			rg[1] = D_00399280;
			i = 2;
		}
		else
		{
			pct = 95;
			flvol = 1.0;
			rg[0] = D_003991F8;
			rg[1] = D_00399280;
			i = 2;
		}

		flvol = flvol * RandomFloat(0, 0.5) + 0.25f;

		if (RandomLong(0, 127) < pct || RandomLong(0, 127) < pct)
		{
			S_StartDynamicSound(-1, 0, rg[RandomLong(0, i - 1)], r_origin, flvol, 1.0, 0, 100, 0);
		}
	}
}

void func_00266188(int nClient)
{
	vec3_t angles, forward, point, screen;
	float x, y;

	if (!D_00331D90[0])
		return;

	if (D_003995E0 == D_0053D1C8)
		return;

	D_00399620 = 1;

	if (crosshair.value != 0)
	{
		x = scr_vrect.x + scr_vrect.width / 2;
		y = scr_vrect.y + scr_vrect.height / 2;
		VectorAdd(D_004F94D0.viewangles, cl[nClient].punchangle, angles);
		AngleVectors(angles, forward, NULL, NULL);
		VectorAdd(r_origin, forward, point);
		ScreenTransform(point, screen);
		x += screen[0] * (scr_vrect.width * 0.5f) + 0.5f;
		y += screen[1] * (scr_vrect.height * 0.5f) + 0.5f;
		func_00247998((int)x, (int)y, nClient);
	}

	if (cl_numlocal > 1)
	{
		if (D_00399564)
		{
			func_002470E0(0, nClient, 0x20);
			func_002A8508(nClient, 0, -D_0032EF8C);
			func_002470E0(0, nClient, 8);
			func_002A8508(nClient, 0, D_0032EF8C);
			func_002470E0(0, nClient, 0x10);
			if (nClient == 1)
			{
				func_002A84C8();
				func_002470E0(0, 0, 0x40);
				func_002A8658();
				func_002A8508(nClient, 0, 0);
			}
		}
		else
		{
			func_002470E0(0, nClient, 4);
			func_002A8508(nClient, -D_0032EF88, 0);
			func_002470E0(0, nClient, 1);
			func_002A8508(nClient, D_0032EF88, 0);
			func_002470E0(0, nClient, 2);
			if (nClient == 1)
			{
				func_002A84C8();
				func_002470E0(0, 0, 0x40);
			}
			func_002A8508(nClient, 0, 0);
		}
	}
	else
	{
		func_002470E0(0, nClient, 7);
	}
}
