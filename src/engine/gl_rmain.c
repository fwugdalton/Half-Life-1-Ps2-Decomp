#include "common.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/cvar.h"
#include "engine/render.h"

#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_TEXTURE_2D 0xDE1
#define GL_DEPTH_TEST 0xB71
#define GL_CULL_FACE 0xB44
#define GL_BLEND 0xBE2
#define GL_ALPHA_TEST 0xBC0
#define GL_POLYGON 9
#define GL_LEQUAL 0x203

typedef struct
{
    int width;
    int height;
} viddef_t;

extern viddef_t vid;

void glEnable(int cap);
void glDisable(int cap);
void glMatrixMode(int mode);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadIdentity(void);
void glFrustum(float left, float right, float bottom, float top, float zNear, float zFar);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
void glViewport(int x, int y, int width, int height);
void glDepthRange(float zNear, float zFar);
void glDepthFunc(int func);
void glBegin(int mode);
void glEnd(void);
void glVertex2f(float x, float y);
void glColor3f(float r, float g, float b);
void glColor4f(float r, float g, float b, float a);
float tanf(float x);
void glTranslatef(float x, float y, float z);
void glRotatef(float angle, float x, float y, float z);
float atanf(float x);

extern mplane_t frustum[4];
extern vec3_t r_origin, vpn, vright, vup;
extern vec3_t D_00551760[], D_00553DB0[], D_00551788[], D_00551718[];

#define GL_FOG 0xB60
#define STAT_HEALTH 0
#define PLANE_ANYZ 5
#define DEG2RAD(a) ((a) * (3.141593f / 180.0f))

void *memcpy(void *, const void *, unsigned int);
void *memset(void *, int, unsigned int);
extern float D_003332E0[]; /* fov por jogador */
float CalcFov(float fov_x, float width, float height);
int SignbitsForPlane(mplane_t *out);
void RotatePointAroundVector(vec3_t dst, vec3_t dir, vec3_t point, float degrees);

extern cl_entity_t *currententity;
extern qboolean cam_thirdperson;
extern cvar_t r_drawentities;
extern int D_00333290;
extern float D_00333474;
extern float D_00331694;
extern int D_00399588;
int R_StudioDrawModel(int flags, int clientId, int unk);
extern cvar_t D_003334F8;

typedef struct
{
    unsigned int r, g, b, a;
} colorVec;

colorVec R_LightPoint(vec3_t p);
extern int cl_numvisedicts[];
extern cl_entity_t *cl_visedicts[];
extern int cl_updatemask[];
extern float r_blend;
extern vec3_t r_entorigin;
void AddTEntity(cl_entity_t *e, int clientId);
void R_DrawBrushModel(cl_entity_t *e, int clientId);
void func_0023AFC8(int);
int R_StudioDrawPlayer(int flags, player_state_t *pplayer, int clientId);
float *R_GetAttachmentPoint(int entity, int attachment, int clientId);
void R_DrawSpriteModel(cl_entity_t *e, int clientId);
#define GL_FOG_START 0xB63
#define GL_FOG_END 0xB64
extern float D_003337D4;
extern float D_00334E6C;
extern cvar_t r_fullbright;
extern int r_framecount;
extern mleaf_t *r_viewleaf[], *r_oldviewleaf[];
extern int D_00332D18[4];
extern float D_0033387C;
extern cvar_t D_00333888;
extern int D_00399590;
extern int D_00399554;
void Cvar_Set(char *var_name, char *value);
void R_AnimateLight(void);
mleaf_t *Mod_PointInLeaf(vec3_t p, model_t *model);
void func_0023C3D8(int r, int g, int b);
void func_0023C3A0(int pname, float param);
void func_0028CA58(void);
#define GL_MODELVIEW_MATRIX 0xBA6
#define GL_PROJECTION_MATRIX 0xBA7
extern float D_00333298[], D_003332A0[];
extern cvar_t D_00556058[], D_0055A378[];
extern float D_003338AC, D_0033384C, D_00333834, D_00331384;
extern int D_0032EFB0, D_0032EFB4, D_0032EFB8, D_0032EFBC;
extern int D_0032EFC0, D_0032EFC4, D_0032EFC8, D_0032EFCC;
extern int D_0032EFF0, D_0032EFF4, D_0032EFF8, D_0032EFFC;
extern int D_0032F000, D_0032F004, D_0032F008, D_0032F00C;
extern float D_004A8B58;
extern float D_00414118[16]; /* r_projection_matrix */
extern float D_004F93D8[16]; /* r_world_matrix */
extern float D_004F9430[16];
extern float D_003337A4;
void glGetFloatv(int pname, float *params);
void MYgluPerspective(float fovy, float aspect, float zNear, float zFar);
#define GL_QUADS 7
#define GL_ZERO 0
#define GL_SRC_COLOR 0x300
#define GL_SRC_ALPHA 0x302
#define GL_ONE_MINUS_SRC_ALPHA 0x303
extern int D_00399644;
extern int D_00399558, D_00399570;
int V_FadeAlpha(int clientId);
void GL_Bind(int texnum);
void glBlendFunc(int sfactor, int dfactor);
void glColor4ubv(byte *v);

void R_SetupFrame(int clientId);
void R_SetFrustum(int clientId);
void R_SetupGL(int clientId);
void R_MarkLeaves(int clientId);
void R_DrawWorld(int clientId);
void func_002CD9F8(int clientId);
void R_DrawEntitiesOnList(int clientId);
void R_DrawTEntitiesOnList(int clientId);
void R_DrawStudioTEntities(int clientId);
void func_002AB188(void);
void GL_DisableMultitexture(void);
void func_002B3A10(int clientId);

extern int D_00399564;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern int D_0032EF78, D_0032EF7C, D_0032EF80, D_0032EF84;
extern int D_0032EF90, D_0032EF94, D_0032EF98, D_0032EF9C;
extern int D_0032EFA0, D_0032EFA4, D_0032EFA8, D_0032EFAC;
extern int D_0032EFD0, D_0032EFD4, D_0032EFD8, D_0032EFDC;
extern int D_0032EFE0, D_0032EFE4, D_0032EFE8, D_0032EFEC;
extern float gldepthmax; /* gldepthmax */
extern float gldepthmin; /* gldepthmin */

void func_002A8750(void);

void func_002A8358(int i)
{
    if (D_00399564)
    {
        if (cl_numlocal > 1)
        {
            switch (i)
            {
            case 0:
                glViewport(D_0032EFD0, D_0032EFD4, D_0032EFD8, D_0032EFDC);
                break;
            case 1:
                glViewport(D_0032EFE0, D_0032EFE4, D_0032EFE8, D_0032EFEC);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }
    else
    {
        if (cl_numlocal > 1)
        {
            switch (i)
            {
            case 0:
                glViewport(D_0032EF90, D_0032EF94, D_0032EF98, D_0032EF9C);
                break;
            case 1:
                glViewport(D_0032EFA0, D_0032EFA4, D_0032EFA8, D_0032EFAC);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }
    func_002A8750();
}

void func_002A84C8(void)
{
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
}

void func_002A8508(int i, int x, int y)
{
    if (D_00399564)
    {
        switch (i)
        {
        case 0:
            glViewport(D_0032EFD0 + x, D_0032EFD4 + y, D_0032EFD8, D_0032EFDC);
            break;
        case 1:
            glViewport(D_0032EFE0 + x, D_0032EFE4 + y, D_0032EFE8, D_0032EFEC);
            break;
        }
    }
    else
    {
        switch (i)
        {
        case 0:
            glViewport(D_0032EF90 + x, D_0032EF94 + y, D_0032EF98, D_0032EF9C);
            break;
        case 1:
            glViewport(D_0032EFA0 + x, D_0032EFA4 + y, D_0032EFA8, D_0032EFAC);
            break;
        }
    }
}

void func_002A8610(void)
{
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
    func_002A8750();
}

void func_002A8658(void)
{
    glDisable(GL_TEXTURE_2D);
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2f(254, 0);
    glVertex2f(258, 0);
    glVertex2f(258, 448);
    glVertex2f(254, 448);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void func_002A8708(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void func_002A8750(void)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(D_0032EF68, D_0032EF68 + vid.width, D_0032EF6C + D_0032EF74, D_0032EF6C, -99999, 99999);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glColor4f(1, 1, 1, 1);
}

void ProjectPointOnPlane(vec3_t dst, vec3_t p, vec3_t normal)
{
    float d;
    vec3_t n;
    float inv_denom;

    inv_denom = 1.0f / DotProduct(normal, normal);

    d = DotProduct(normal, p) * inv_denom;

    n[0] = normal[0] * inv_denom;
    n[1] = normal[1] * inv_denom;
    n[2] = normal[2] * inv_denom;

    dst[0] = p[0] - d * n[0];
    dst[1] = p[1] - d * n[1];
    dst[2] = p[2] - d * n[2];
}

void PerpendicularVector(vec3_t dst, vec3_t src)
{
    int pos;
    int i;
    float minelem = 1.0f;
    vec3_t tempvec;

    for (pos = 0, i = 0; i < 3; i++)
    {
        if (__builtin_fabsf(src[i]) < minelem)
        {
            pos = i;
            minelem = __builtin_fabsf(src[i]);
        }
    }
    tempvec[0] = tempvec[1] = tempvec[2] = 0.0f;
    tempvec[pos] = 1.0f;

    ProjectPointOnPlane(dst, tempvec, src);

    VectorNormalize(dst);
}

void RotatePointAroundVector(vec3_t dst, vec3_t dir, vec3_t point, float degrees)
{
    float m[3][3];
    float im[3][3];
    float zrot[3][3];
    float tmpmat[3][3];
    float rot[3][3];
    int i;
    vec3_t vr, vup, vf;

    vf[0] = dir[0];
    vf[1] = dir[1];
    vf[2] = dir[2];

    PerpendicularVector(vr, dir);
    CrossProduct(vr, vf, vup);

    m[0][0] = vr[0];
    m[1][0] = vr[1];
    m[2][0] = vr[2];

    m[0][1] = vup[0];
    m[1][1] = vup[1];
    m[2][1] = vup[2];

    m[0][2] = vf[0];
    m[1][2] = vf[1];
    m[2][2] = vf[2];

    memcpy(im, m, sizeof(im));

    im[0][1] = m[1][0];
    im[0][2] = m[2][0];
    im[1][0] = m[0][1];
    im[1][2] = m[2][1];
    im[2][0] = m[0][2];
    im[2][1] = m[1][2];

    memset(zrot, 0, sizeof(zrot));
    zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0f;

    zrot[0][0] = cosf(DEG2RAD(degrees));
    zrot[0][1] = sinf(DEG2RAD(degrees));
    zrot[1][0] = -sinf(DEG2RAD(degrees));
    zrot[1][1] = cosf(DEG2RAD(degrees));

    R_ConcatRotations(m, zrot, tmpmat);
    R_ConcatRotations(tmpmat, im, rot);

    for (i = 0; i < 3; i++)
    {
        dst[i] = rot[i][0] * point[0] + rot[i][1] * point[1] + rot[i][2] * point[2];
    }
}

qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (BoxOnPlaneSide(mins, maxs, &frustum[i]) == 2)
            return true;
    }
    return false;
}

qboolean func_002A8C20(vec3_t mins, vec3_t maxs)
{
    int i;
    qboolean inside = true;

    for (i = 0; i < 4; i++)
    {
        if (BoxOnPlaneSide(mins, maxs, &frustum[i]) != 1)
        {
            inside = false;
            break;
        }
    }
    return inside;
}

void R_RotateForEntity(cl_entity_t *e)
{
    int i;
    vec3_t angles;
    vec3_t modelpos;

    VectorCopy(e->origin, modelpos);
    angles[2] = e->angles[2];
    angles[0] = e->angles[0];
    angles[1] = e->angles[1];

    if (e->movetype != 0)
    {
        float f;
        float d;

        f = 0;
        if (cl[0].time < e->animtime + 0.2f && e->animtime != e->prevanimtime)
            f = (cl[0].time - e->animtime) / (e->animtime - e->prevanimtime);

        for (i = 0; i < 3; i++)
        {
            modelpos[i] -= (e->prevorigin[i] - e->origin[i]) * f;
        }

        if (f > 0 && f < 1.5f)
        {
            for (i = 0; i < 3; i++)
            {
                d = e->prevangles[i] - e->angles[i];
                if (d > 180)
                    d -= 360;
                else if (d < -180)
                    d += 360;

                angles[i] += d * (1 - f);
            }
        }
    }

    glTranslatef(modelpos[0], modelpos[1], modelpos[2]);
    glRotatef(angles[1], 0, 0, 1);
    glRotatef(angles[0], 0, 1, 0);
    glRotatef(angles[2], 1, 0, 0);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", R_DrawSpriteModel);

#ifdef NON_MATCHING
void R_DrawEntitiesOnList(int clientId)
{
    int i, j;
    float *pAttachment;

    if (!r_drawentities.value)
        return;

    for (i = 0; i < cl_numvisedicts[clientId]; i++)
    {
        currententity = &cl_visedicts[clientId][i];

        if (currententity->rendermode != 0)
        {
            AddTEntity(currententity, clientId);
            continue;
        }

        if (currententity->model->type == mod_brush)
            R_DrawBrushModel(currententity, clientId);
    }

    if (cl[clientId].waterlevel < 3)
        func_0023AFC8(2);

    for (i = 0; i < cl_numvisedicts[clientId]; i++)
    {
        currententity = &cl_visedicts[clientId][i];

        if (currententity->rendermode != 0)
            continue;

        if (currententity->model->type != mod_studio)
            continue;

        if (currententity->index > 0 && currententity->index <= cl[clientId].maxclients)
        {
            R_StudioDrawPlayer(3, &cl[clientId].frames[cl[clientId].parsecount & cl_updatemask[clientId]].playerstate[currententity->index - 1], clientId);
        }
        else if (currententity->movetype == 12)
        {
            for (j = 0; j < cl_numvisedicts[clientId]; j++)
            {
                if (cl_visedicts[clientId][j].index == currententity->aiment)
                {
                    currententity = &cl_visedicts[clientId][j];
                    if (currententity->index > 0 && currententity->index <= cl[clientId].maxclients)
                        R_StudioDrawPlayer(0, &cl[clientId].frames[cl[clientId].parsecount & cl_updatemask[clientId]].playerstate[currententity->index - 1], clientId);
                    else
                        R_StudioDrawModel(0, clientId, 1);
                    currententity = &cl_visedicts[clientId][i];
                    R_StudioDrawModel(3, clientId, 1);
                    break;
                }
            }
        }
        else
        {
            R_StudioDrawModel(3, clientId, 1);
        }
    }

    r_blend = 1.0f;
    for (i = 0; i < cl_numvisedicts[clientId]; i++)
    {
        currententity = &cl_visedicts[clientId][i];

        if (currententity->rendermode != 0)
            continue;

        if (currententity->model->type == mod_sprite)
        {
            if (currententity->body)
            {
                pAttachment = R_GetAttachmentPoint(currententity->skin, currententity->body, clientId);
                VectorCopy(pAttachment, r_entorigin);
            }
            else
            {
                VectorCopy(currententity->origin, r_entorigin);
            }
            R_DrawSpriteModel(currententity, clientId);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", R_DrawEntitiesOnList);
#endif

void R_DrawViewModel(int clientId)
{
    colorVec c;
    float oldval;

    D_00333290 = 1;
    currententity = &cl[clientId].viewent;

    if (!D_00333474 || cam_thirdperson || D_00331694 || D_00399588 || !r_drawentities.value
        || cl[clientId].stats[STAT_HEALTH] <= 0 || !currententity->model
        || cl[clientId].viewentity > cl[clientId].maxclients || cl[clientId].paused)
    {
        c = R_LightPoint(currententity->origin);
        cl[clientId].light_level = (c.r + c.g + c.b) / 3;
        D_00333290 = 0;
        return;
    }

    if (currententity->model->type == mod_studio)
    {
        glDepthRange(gldepthmin, gldepthmin + 0.25f * (gldepthmax - gldepthmin));

        if (!cl[clientId].weaponstarttime)
            cl[clientId].weaponstarttime = cl[clientId].time;

        currententity->sequence = cl[clientId].weaponsequence;
        currententity->framerate = 1;
        currententity->frame = 0;
        currententity->animtime = cl[clientId].weaponstarttime;
        currententity->colormap = (byte)cl[clientId].players[cl[clientId].playernum].topcolor;
        currententity->colormap |= (byte)cl[clientId].players[cl[clientId].playernum].bottomcolor << 8;
        cl[clientId].light_level = 128;

        oldval = D_003334F8.value;
        D_003334F8.value = 0;
        R_StudioDrawModel(1, clientId, 0);
        D_003334F8.value = oldval;
        glDepthRange(gldepthmin, gldepthmax);
    }
    D_00333290 = 0;
}

#ifdef NON_MATCHING
void R_PreDrawViewModel(int clientId)
{
    currententity = &cl[clientId].viewent;
    D_00333290 = 1;

    if (D_00333474 && !cam_thirdperson && !D_00331694 && !D_00399588 && r_drawentities.value
        && cl[clientId].stats[STAT_HEALTH] > 0 && currententity->model
        && cl[clientId].viewentity <= cl[clientId].maxclients && !cl[clientId].paused)
    {
        switch ((unsigned)currententity->model->type)
        {
        case mod_brush:
            break;
        case mod_alias:
            break;
        case mod_studio:
            if (!cl[clientId].weaponstarttime)
                cl[clientId].weaponstarttime = cl[clientId].time;
            currententity->sequence = cl[clientId].weaponsequence;
            currententity->frame = 0;
            currententity->framerate = 1;
            currententity->animtime = cl[clientId].weaponstarttime;
            R_StudioDrawModel(2, clientId, 1);
            break;
        }
    }
    D_00333290 = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", R_PreDrawViewModel);
#endif

void R_ScreenFade(int clientId)
{
    int alpha;
    byte color[4];

    alpha = V_FadeAlpha(clientId);
    if (!alpha)
        return;

    GL_DisableMultitexture();
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    GL_Bind(D_00399644);

    if (cl[clientId].sf.fadeFlags & FFADE_MODULATE)
    {
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
        color[0] = (cl[clientId].sf.fader * alpha + 255 * (255 - alpha)) >> 8;
        color[1] = (cl[clientId].sf.fadeg * alpha + 255 * (255 - alpha)) >> 8;
        color[2] = (cl[clientId].sf.fadeb * alpha + 255 * (255 - alpha)) >> 8;
        color[3] = 255;
    }
    else
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        color[0] = cl[clientId].sf.fader;
        color[1] = cl[clientId].sf.fadeg;
        color[2] = cl[clientId].sf.fadeb;
        color[3] = alpha;
    }

    if (D_00399564)
    {
        if (cl_numlocal > 1)
        {
            switch (clientId)
            {
            case 0:
                glViewport(D_0032EFF0, D_0032EFF4, D_0032EFF8, D_0032EFFC);
                break;
            case 1:
                glViewport(D_0032F000, D_0032F004, D_0032F008, D_0032F00C);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }
    else
    {
        if (cl_numlocal > 1)
        {
            switch (clientId)
            {
            case 0:
                glViewport(D_0032EFB0, D_0032EFB4, D_0032EFB8, D_0032EFBC);
                break;
            case 1:
                glViewport(D_0032EFC0, D_0032EFC4, D_0032EFC8, D_0032EFCC);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, 1, 1, 0, -99999, 99999);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor4ubv(color);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glVertex2f(0, 0);
    glVertex2f(0, 1);
    glVertex2f(1, 1);
    glVertex2f(1, 0);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_ALPHA_TEST);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

int SignbitsForPlane(mplane_t *out)
{
    int bits, j;

    bits = 0;
    for (j = 0; j < 3; j++)
    {
        if (out->normal[j] < 0)
            bits |= 1 << j;
    }
    return bits;
}

void R_SetFrustum(int clientId)
{
    int i;
    float fov_x, fov_y;

    fov_x = D_003332E0[clientId];

    if (D_00399564)
    {
        if (cl_numlocal > 1)
            fov_y = CalcFov(fov_x, 8, 9);
        else
            fov_y = CalcFov(fov_x, 16, 9);
    }
    else
    {
        if (cl_numlocal > 1)
            fov_y = CalcFov(fov_x, 4, 1.5f);
        else
            fov_y = CalcFov(fov_x, 4, 3);
    }

    fov_x /= 2;
    fov_y /= 2;
    RotatePointAroundVector(frustum[0].normal, vup, vpn, -(90 - fov_x));
    RotatePointAroundVector(frustum[1].normal, vup, vpn, 90 - fov_x);
    RotatePointAroundVector(frustum[2].normal, vright, vpn, 90 - fov_y);
    RotatePointAroundVector(frustum[3].normal, vright, vpn, -(90 - fov_y));

    for (i = 0; i < 4; i++)
    {
        frustum[i].type = PLANE_ANYZ;
        frustum[i].dist = DotProduct(r_origin, frustum[i].normal);
        frustum[i].signbits = SignbitsForPlane(&frustum[i]);
    }
}

void R_SetupFrame(int clientId)
{
    if (D_003337D4)
        D_00399564 = 1;
    else
        D_00399564 = 0;

    if (cl[clientId].maxclients > 1)
    {
        if (r_fullbright.value)
            Cvar_Set("r_fullbright", "1");
        if (D_00334E6C)
            Cvar_Set("snd_show", "0");
        if (D_00331694)
            Cvar_Set("chase_active", "0");
    }

    R_AnimateLight();

    r_framecount++;

    VectorCopy(r_refdef.vieworg, r_origin);

    AngleVectors(r_refdef.viewangles, vpn, vright, vup);

    r_oldviewleaf[clientId] = r_viewleaf[clientId];
    r_viewleaf[clientId] = Mod_PointInLeaf(r_origin, cl[clientId].worldmodel);

    if (cl[clientId].waterlevel > 2)
    {
        unsigned int density;

        density = D_00332D18[3] * 4;
        func_0023C3D8((byte)D_00332D18[0], (byte)D_00332D18[1], (byte)D_00332D18[2]);
        func_0023C3A0(GL_FOG_START, D_0033387C);
        func_0023C3A0(GL_FOG_END, D_00333888.value - density);
        glEnable(GL_FOG);
        D_00399590 = 1;
        func_0023AFC8(3);
    }
    else
    {
        D_00399590 = 0;
        func_0023AFC8(1);
        glDisable(GL_FOG);
    }

    func_0028CA58();
    D_00399554 = 0;
}

void MYgluPerspective(float fovy, float aspect, float zNear, float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * tanf(fovy * (3.141593f / 360.0f));
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

float CalcFov(float fov_x, float width, float height)
{
    float a;
    float x;

    if (fov_x < 1 || fov_x > 179)
        fov_x = 90;

    x = width / tanf(fov_x * (3.141593f / 360.0f));

    a = atanf(height / x);

    a = a * (360.0f / M_PI);

    return a;
}

void R_SetupGL(int clientId)
{
    float screenaspect;
    float fov_y;
    int i, j, k;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (D_003332E0[clientId] < 85)
    {
        D_00333298[clientId] = D_00556058[clientId].value * D_003338AC;
        D_00331384 = D_0033384C;
        D_003332A0[clientId] = D_0055A378[clientId].value * D_003338AC;
    }
    else
    {
        D_00331384 = D_00333834;
        D_00333298[clientId] = D_00556058[clientId].value;
        D_003332A0[clientId] = D_0055A378[clientId].value;
    }

    if (D_00399564)
    {
        if (cl_numlocal > 1)
        {
            switch (clientId)
            {
            case 0:
                glViewport(D_0032EFF0, D_0032EFF4, D_0032EFF8, D_0032EFFC);
                break;
            case 1:
                glViewport(D_0032F000, D_0032F004, D_0032F008, D_0032F00C);
                break;
            }
            screenaspect = 8.0f / 9.0f;
            fov_y = CalcFov(D_003332E0[clientId], 8, 9);
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
            screenaspect = 16.0f / 9.0f;
            fov_y = CalcFov(D_003332E0[clientId], 16, 9);
        }
    }
    else
    {
        if (cl_numlocal > 1)
        {
            switch (clientId)
            {
            case 0:
                glViewport(D_0032EFB0, D_0032EFB4, D_0032EFB8, D_0032EFBC);
                break;
            case 1:
                glViewport(D_0032EFC0, D_0032EFC4, D_0032EFC8, D_0032EFCC);
                break;
            }
            screenaspect = 8.0f / 3.0f;
            fov_y = CalcFov(D_003332E0[clientId], 4, 1.5f);
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
            screenaspect = 4.0f / 3.0f;
            fov_y = CalcFov(D_003332E0[clientId], 4, 3);
        }
    }

    MYgluPerspective(fov_y, screenaspect, 4, D_004A8B58);

    glGetFloatv(GL_PROJECTION_MATRIX, D_00414118);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-90, 1, 0, 0);
    glRotatef(90, 0, 0, 1);
    glRotatef(-r_refdef.viewangles[2], 1, 0, 0);
    glRotatef(-r_refdef.viewangles[0], 0, 1, 0);
    glRotatef(-r_refdef.viewangles[1], 0, 0, 1);
    glTranslatef(-r_refdef.vieworg[0], -r_refdef.vieworg[1], -r_refdef.vieworg[2]);

    glGetFloatv(GL_MODELVIEW_MATRIX, D_004F93D8);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_DEPTH_TEST);

    if (D_003337A4)
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                D_004F9430[i * 4 + j] = 0;
                for (k = 0; k < 4; k++)
                    D_004F9430[i * 4 + j] += D_004F93D8[k * 4 + i] * D_00414118[j * 4 + k];
            }
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            for (j = 0; j < 4; j++)
            {
                D_004F9430[i * 4 + j] = 0;
                for (k = 0; k < 4; k++)
                    D_004F9430[i * 4 + j] += D_004F93D8[i * 4 + k] * D_00414118[k * 4 + j];
            }
        }
    }
}

void R_RenderScene(int clientId)
{
    R_SetupFrame(clientId);

    VectorCopy(r_origin, D_00551760[clientId]);
    VectorCopy(vpn, D_00553DB0[clientId]);
    VectorCopy(vright, D_00551788[clientId]);
    VectorCopy(vup, D_00551718[clientId]);

    R_SetFrustum(clientId);
    R_SetupGL(clientId);
    R_MarkLeaves(clientId);
    R_DrawWorld(clientId);
    func_002CD9F8(clientId);
    R_DrawEntitiesOnList(clientId);
    R_DrawTEntitiesOnList(clientId);
    glDisable(GL_FOG);
    R_DrawStudioTEntities(clientId);
    func_002CD9F8(clientId);
    func_002AB188();
    GL_DisableMultitexture();
    func_002B3A10(clientId);
}

void func_002AAD48(void)
{
    gldepthmin = 0;
    gldepthmax = 1;
    glDepthFunc(GL_LEQUAL);
    glDepthRange(gldepthmin, gldepthmax);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", R_RenderView);

void func_002AB0B8(int a, int b, int c, ...)
{
}
