#include "common.h"

int Q_strncasecmp(char *s1, char *s2, int n);
int atoi(const char *);

extern char *D_00331450;        /* date = __DATE__ */
extern char *D_00331458[12];    /* mon */
extern char D_00331488[12];     /* mond */
extern int D_00331494;          /* b */

int build_number(void)
{
    int m = 0;
    int d = 0;
    int y = 0;

    if (D_00331494 != 0)
        return D_00331494;

    for (m = 0; m < 11; m++)
    {
        if (Q_strncasecmp(&D_00331450[0], D_00331458[m], 3) == 0)
            break;
        d += D_00331488[m];
    }

    d += atoi(&D_00331450[4]) - 1;
    y = atoi(&D_00331450[7]);
    D_00331494 = d + (int)((y - 1901) * 365.25f);
    if (((y % 4) == 0) && m > 1)
        D_00331494 += 1;
    D_00331494 -= 34995;
    return D_00331494;
}

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388DE0);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388DF0);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388DF8);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E00);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E08);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E10);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E18);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E20);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E28);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E30);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E38);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E40);

INCLUDE_RODATA("asm/nonmatchings/engine/build", D_00388E48);
