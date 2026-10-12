#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"

/* vídeo do PS2 sobre a camada GL (equivale ao gl_vidnt.c do Quake) */

typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} vrect_t;

typedef struct
{
    byte pad0[0x1C];
    int width;                  /* 0x1C */
    int height;                 /* 0x20 */
    byte pad24[0x38 - 0x24];
    int conwidth;               /* 0x38 */
    int conheight;              /* 0x3C */
    byte pad40[0x50 - 0x40];
} viddef_t;                     /* 0x50 */

extern viddef_t r_viddef;
extern vrect_t D_0053D198;      /* window_rect */
extern int D_0039970C;
extern int D_003996D8;

extern char *D_00399710;        /* gl_vendor */
extern char *D_00399718;        /* gl_renderer */
extern char *D_003996E8;        /* gl_version */
extern char *D_003996F0;        /* gl_extensions */

extern void (*qglColorTableEXT)(int target, int internalformat, int width, int format, int type, void *table);
extern void (*D_003994CC)(int, float, float);        /* qglMTexCoord2fSGIS */
extern void (*D_003994C4)(int);        /* qglSelectTextureSGIS */
extern int TEXTURE0_SGIS;
extern int TEXTURE1_SGIS;
extern int D_003333D0;          /* oldtarget */
extern qboolean gl_mtexable;

/* ponteiros da camada de vídeo do PS2 */
extern void (*D_00399768)(int);
extern void (*D_0039976C)(viddef_t *);
extern char *(*D_003997BC)(int);

extern cvar_t D_00334300;       /* gl_ztrick */
extern cvar_t D_00334318;       /* vid_mode */
extern cvar_t D_00334330;       /* vid_wait */

void *qwglGetProcAddress(char *name);
int COM_CheckParm(char *parm);
int Q_atoi(char *str);
const unsigned char *func_0023B0D0(int name);  /* glGetString */
void func_0023C600(int target, int pname, float param); /* glTexParameterf */
void func_0023AD98(void);
void func_0023ADB8(void);
void glEnable(int cap);
void glAlphaFunc(int func, float ref);
void glShadeModel(int mode);
void glBlendFunc(int sfactor, int dfactor);
void glTexEnvf(int target, int pname, float param);
void GL_SelectTexture(int target);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_vidps2", D_00395228);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_vidps2", D_00395230);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_vidps2", D_00395240);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_vidps2", D_00395250);

void func_002C4E10(void)
{
    if (strstr(D_003996F0, "GL_EXT_paletted_texture"))
    {
        qglColorTableEXT = qwglGetProcAddress("glColorTableEXT");
        Con_Printf("Found paletted texture extension.\n");
    }
    else
    {
        qglColorTableEXT = NULL;
    }

    if (strstr(D_003996F0, "GL_EXT_texture_object "))
        COM_CheckParm("-gl11");
}

void func_002C4EA8(void)
{
    if (strstr(D_003996F0, "GL_ARB_multitexture "))
    {
        Con_Printf("ARB Multitexture extensions found.\n");
        D_003994CC = qwglGetProcAddress("glMultiTexCoord2fARB");
        D_003994C4 = qwglGetProcAddress("glActiveTextureARB");
        TEXTURE0_SGIS = 0x84C0;
        TEXTURE1_SGIS = 0x84C1;
        D_003333D0 = 0x84C0;
        gl_mtexable = true;
        GL_SelectTexture(0x84C0);
    }
    else
    {
        Con_Printf("NO Multitexture extensions found.\n");
    }
}

void func_002C4F60(void)
{
    D_00399710 = (char *)func_0023B0D0(0x1F00);
    Con_DPrintf("GL_VENDOR: %s\n", D_00399710);
    D_00399718 = (char *)func_0023B0D0(0x1F01);
    Con_DPrintf("GL_RENDERER: %s\n", D_00399718);
    D_003996E8 = (char *)func_0023B0D0(0x1F02);
    Con_DPrintf("GL_VERSION: %s\n", D_003996E8);
    D_003996F0 = (char *)func_0023B0D0(0x1F03);
    Con_DPrintf("GL_EXTENSIONS: %s\n", D_003996F0);

    func_002C4E10();
    func_002C4EA8();

    glEnable(0xDE1);
    glEnable(0xBC0);
    glAlphaFunc(0x205, 0);
    glShadeModel(0x1D00);

    func_0023C600(0xDE1, 0x2801, 9728.0f);
    func_0023C600(0xDE1, 0x2800, 9728.0f);
    func_0023C600(0xDE1, 0x2802, 10497.0f);
    func_0023C600(0xDE1, 0x2803, 10497.0f);

    glBlendFunc(0x302, 0x303);
    glTexEnvf(0x2300, 0x2200, 7681.0f);
}

void func_002C50C8(int *x, int *y, int *width, int *height)
{
    *x = *y = 0;
    *width = D_0053D198.right - D_0053D198.left;
    *height = D_0053D198.bottom - D_0053D198.top;
    r_viddef.width = r_viddef.conwidth = *width;
    r_viddef.height = r_viddef.conheight = *height;
    func_0023AD98();
}

void func_002C5130(void)
{
    func_0023ADB8();
    D_00399768(0);
}

void func_002C5160(void)
{
    int mode;

    mode = Q_atoi(Cmd_Argv(1));
    Con_Printf("%s\n", D_003997BC(mode));
}

int VID_Init(void)
{
    Cvar_RegisterVariable(&D_00334318);
    Cvar_RegisterVariable(&D_00334330);
    Cvar_RegisterVariable(&D_00334300);
    Cmd_AddCommand("vid_describemode", func_002C5160);
    D_0039976C(&r_viddef);
    return 1;
}

void func_002C5218(void) {
}

void func_002C5220(void) {
}

void func_002C5228(void) {
}

void func_002C5230(vrect_t *rect, int a, int b)
{
    D_0053D198 = *rect;
    D_0039970C = a;
    D_003996D8 = b;
}

void func_002C5270(viddef_t *vid)
{
    r_viddef = *vid;
}

int func_002C5330(void)
{
    return 1;
}
