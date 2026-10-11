#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
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

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmisc", R_Init);

void func_002B0278(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmisc", func_002B0280);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmisc", func_002B0530);

void func_002B05F8(void) {
}
