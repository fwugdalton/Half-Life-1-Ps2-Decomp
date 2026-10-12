#include "common.h"

/* mapa de bits dos botões do controle (atribuído conforme o tipo do controle) */

void Host_WriteConfiguration(void);

extern int D_00399350;
extern int D_00399354;
extern int D_00399358;
extern int D_0039935C;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern int D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern int D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394;
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern int D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern int D_003993B4;
extern int D_003993B8;
extern int D_003993BC;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993C8;
extern int D_003993CC;
extern int D_003993D0;
extern int D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;

void func_00277B58(void)
{
    Host_WriteConfiguration();
}

#ifdef NON_MATCHING
void func_00277B78(int type)
{
    if (type == 0x23)
    {
        D_00399394 = 0x1;
        D_00399354 = 0x2;
        D_00399350 = 0x4;
        D_003993BC = 0x8;
        D_003993A0 = 0x10;
        D_0039939C = 0x20;
        D_003993A8 = 0x40;
        D_00399368 = 0x80;
        D_003993C0 = 0x100;
        D_00399370 = 0x200;
        D_0039935C = 0x400;
        D_003993C8 = 0x800;
        D_003993DC = 0x1000;
        D_00399388 = 0x2000;
        D_003993D0 = 0x4000;
        D_0039938C = 0x8000;
        D_00399360 = 0x10000;
        D_003993B8 = 0x20000;
        D_00399390 = 0x40000;
        D_003993AC = 0x80000;
        D_003993E0 = 0x100000;
        D_00399364 = 0x200000;
        D_003993C4 = 0x400000;
        D_0039937C = 0x800000;
        D_003993B0 = 0x1000000;
        D_00399384 = 0x2000000;
        D_00399378 = 0x4000000;
    }
    else
    {
        D_00399394 = 0x10;
        D_00399354 = 0x1;
        D_003993A0 = 0x400;
        D_0039939C = 0x2;
        D_003993A8 = 0x1000000;
        D_00399368 = 0x1000;
        D_003993C0 = 0x40;
        D_00399374 = 0x20;
        D_00399370 = 0x8000000;
        D_00399398 = 0x80;
        D_003993CC = 0x100;
        D_003993D8 = 0x200;
        D_003993DC = 0x10000000;
        D_00399384 = 0x800;
        D_003993D0 = 0x4000000;
        D_0039938C = 0x2000;
        D_0039937C = 0x4000;
        D_00399360 = 0x400000;
        D_003993B8 = 0x10000;
        D_00399390 = 0x20000;
        D_003993AC = 0x40000;
        D_003993E0 = 0x80000;
        D_00399364 = 0x100000;
        D_003993C4 = 0x200000;
        D_003993B0 = 0x2000000;
        D_00399378 = 0x8;
        D_003993E4 = 0x8000;
        D_003993D4 = 0x1;
        D_00399388 = 0x4;
        D_003993A4 = 0x2;
        D_003993B4 = 0x4;
        D_00399358 = 0x800000;
        D_0039936C = 0x8;
        D_00399380 = 0x10;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/unk_277B58", func_00277B78);
#endif
