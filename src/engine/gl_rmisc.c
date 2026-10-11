#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/model.h"
#include "engine/glquake.h"

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A08);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A10);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A20);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A28);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A38);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A48);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A58);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A68);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A78);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A80);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394A90);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AA0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AB0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AC0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AD0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AE0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AE8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394AF0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B00);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B10);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B18);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B28);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B38);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B48);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B58);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B68);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B78);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B88);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394B98);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394BA8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394BC0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394BD8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394BE0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394BF0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C00);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C08);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C18);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C28);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C30);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C38);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C40);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C50);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C60);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C70);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C78);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C88);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394C98);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CA8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CB8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CC8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CD8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CE0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CF0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394CF8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D00);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D08);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D18);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D28);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D30);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D40);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D48);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D58);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rmisc", D_00394D60);

extern char *D_00399710; /* gl_vendor */
extern char *D_00399718; /* gl_renderer */
extern char *D_003996E8; /* gl_version */
extern char *D_003996F0; /* gl_extensions */
extern int texture_extension_number;
extern int D_00399540;            /* particletexture */
extern byte D_00414160[8][8][4];  /* dados da textura de partícula */
extern byte D_003338B8[8][8];     /* dottexture */
void glTexEnvf(int target, int pname, float param);
void func_0023C600(int target, int pname, float param); /* glTexParameterf */
void glTexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, void *pixels);

void func_002AFE50(void)
{
    if (D_00399710)
        Con_Printf("GL Vendor: %s\n", D_00399710);
    if (D_00399718)
        Con_Printf("GL Renderer: %s\n", D_00399718);
    if (D_003996E8)
        Con_Printf("GL Version: %s\n", D_003996E8);
    if (D_003996F0)
        Con_Printf("GL Extensions: %s\n", D_003996F0);
}

void func_002AFED0(void)
{
    int x, y;

    D_00399540 = texture_extension_number++;
    GL_Bind(D_00399540);

    for (x = 0; x < 8; x++)
    {
        for (y = 0; y < 8; y++)
        {
            D_00414160[y][x][0] = 255;
            D_00414160[y][x][1] = 255;
            D_00414160[y][x][2] = 255;
            D_00414160[y][x][3] = D_003338B8[x][y] * 255;
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, 4, 8, 8, 0, 0x1908, 0x1401, D_00414160);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 9729.0f);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 9729.0f);
}

extern cvar_t D_00333438, r_fullbright, r_decals, mp_decals, r_drawentities, D_00333468, D_00333510;
extern cvar_t r_wateralpha, r_dynamic, r_novis, D_00333498, D_00333480, D_003335D0, D_003335A0;
extern cvar_t D_003334F8, D_00333570, r_traceglow, D_003335B8, D_003335E8, D_00333600, D_00333618;
extern cvar_t D_00333630, D_00333660, D_00333678, D_003336D8, gl_polyoffset, D_00333708, D_00333690;
extern cvar_t D_003336A8, gl_wateramp, gl_overbright, gl_zmax, gl_alphamin, D_00333798, gl_monolights;
extern cvar_t D_003337C8, D_003337E0, D_003337F8, D_00333810, D_00333828, D_00333840, D_00333858;
extern cvar_t D_00333870, D_00333888, D_003338A0;
extern char D_00394A80[]; /* "r_decals" */
void Cvar_SetValue(char *var_name, float value);
void func_002B0530(void);
void func_002B0E20(void);
void func_002B0758(void);

void R_Init(void)
{
    Cmd_AddCommand("timerefresh", func_002B0530);
    Cmd_AddCommand("pointfile", func_002B0E20);
    Cmd_AddCommand("gl_dump", func_002AFE50);
    Cvar_RegisterVariable(&D_00333438);
    Cvar_RegisterVariable(&r_fullbright);
    Cvar_RegisterVariable(&r_decals);
    Cvar_RegisterVariable(&mp_decals);
    Cvar_SetValue(D_00394A80, 1024);
    Cvar_RegisterVariable(&r_drawentities);
    Cvar_RegisterVariable(&D_00333468);
    Cvar_RegisterVariable(&D_00333510);
    Cvar_RegisterVariable(&r_wateralpha);
    Cvar_RegisterVariable(&r_dynamic);
    Cvar_RegisterVariable(&r_novis);
    Cvar_RegisterVariable(&D_00333498);
    Cvar_RegisterVariable(&D_00333480);
    Cvar_RegisterVariable(&D_003335D0);
    Cvar_RegisterVariable(&D_003335A0);
    Cvar_RegisterVariable(&D_003334F8);
    Cvar_RegisterVariable(&D_00333570);
    Cvar_RegisterVariable(&r_traceglow);
    Cvar_RegisterVariable(&D_003335B8);
    Cvar_RegisterVariable(&D_003335E8);
    Cvar_RegisterVariable(&D_00333600);
    Cvar_RegisterVariable(&D_00333618);
    Cvar_RegisterVariable(&D_00333630);
    Cvar_RegisterVariable(&D_00333660);
    Cvar_RegisterVariable(&D_00333678);
    Cvar_RegisterVariable(&D_003336D8);
    Cvar_RegisterVariable(&gl_polyoffset);
    Cvar_RegisterVariable(&D_00333708);
    Cvar_RegisterVariable(&D_00333690);
    Cvar_RegisterVariable(&D_003336A8);
    Cvar_RegisterVariable(&gl_wateramp);
    Cvar_RegisterVariable(&gl_overbright);
    Cvar_RegisterVariable(&gl_zmax);
    Cvar_RegisterVariable(&gl_alphamin);
    Cvar_RegisterVariable(&D_00333798);
    Cvar_RegisterVariable(&gl_monolights);
    Cvar_RegisterVariable(&D_003337C8);
    Cvar_RegisterVariable(&D_003337E0);
    Cvar_RegisterVariable(&D_003337F8);
    Cvar_RegisterVariable(&D_00333810);
    Cvar_RegisterVariable(&D_00333828);
    Cvar_RegisterVariable(&D_00333840);
    Cvar_RegisterVariable(&D_00333858);
    Cvar_RegisterVariable(&D_00333870);
    Cvar_RegisterVariable(&D_00333888);
    Cvar_RegisterVariable(&D_003338A0);
    func_002B0758();
    func_002AFED0();
}

void func_002B0278(void) {
}

extern int d_lightstylevalue[];
extern cl_entity_t D_004F9238; /* r_worldentity */
extern mleaf_t *r_viewleaf[2];
extern int D_00399520;  /* skytexturenum */
extern int D_00399550;  /* mirrortexturenum */
extern float D_003338F8; /* valor salvo de r_decals */
extern cl_entity_t *cl_entities[2];
void func_002A67E0(void);
void func_002C3038(void);
void func_002B0CD0(int clientId);
void R_ClearDecals(void);
void V_ResetScreenEffects(int clientId);
void GL_BuildLightmaps(int clientId);
void R_LoadSkys(void);
int func_00228C00(char *name, model_t *model);
void GL_UnloadTextures(void);
void *memset(void *s, int c, unsigned int n);

void func_002B0280(int clientId)
{
    int i;

    for (i = 0; i < 256; i++)
        d_lightstylevalue[i] = 264;

    func_002A67E0();
    func_002C3038();

    memset(&D_004F9238, 0, sizeof(D_004F9238));
    D_004F9238.model = cl[clientId].worldmodel;

    for (i = 0; i < cl[clientId].worldmodel->numleafs; i++)
        cl[clientId].worldmodel->leafs[i].efrags = NULL;

    r_viewleaf[clientId] = NULL;
    func_002B0CD0(clientId);
    R_ClearDecals();
    V_ResetScreenEffects(clientId);

    if (!clientId)
        GL_BuildLightmaps(0);

    D_00399520 = -1;
    D_00399550 = -1;

    for (i = 0; i < cl[clientId].worldmodel->numtextures; i++)
    {
        if (!Q_strncmp(cl[clientId].worldmodel->textures[i].name, "sky", 3))
            D_00399520 = i;
        if (!Q_strncmp(cl[clientId].worldmodel->textures[i].name, "window02_1", 10))
            D_00399550 = i;
        cl[clientId].worldmodel->textures[i].texturechain = NULL;
    }

    if (!clientId)
        R_LoadSkys();

    if (D_003338F8 < 0)
        D_003338F8 = r_decals.value;

    if (func_00228C00("maps/ht12fubar.bs2", cl[clientId].worldmodel) == 1)
        r_decals.value = 20;
    else
        r_decals.value = D_003338F8;

    cl_entities[clientId]->scale = gl_wateramp.value;
    GL_UnloadTextures();
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmisc", func_002B0530);

void func_002B05F8(void) {
}
