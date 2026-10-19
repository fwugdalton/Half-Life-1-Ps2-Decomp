/* Gbx: cvars dos cheats e o modo do disco */

#include "common.h"

void Cvar_RegisterVariable(void *var);

extern char D_0032F010[];       /* GravityCheat */
extern char D_0032F028[];
extern char D_0032F040[];
extern char D_0032F058[];
extern char D_0032F070[];
extern char D_0032F088[];
extern char D_0032F0A0[];
extern char D_0032F0B8[];
extern char D_0032F0D0[];
extern char D_0032F0E8[];
extern char D_0032F100[];
extern char D_0032F118[];
extern char D_0032F130[];
extern char D_0032F148[];
extern int D_0032F15C;

void func_00239C80(void) {
    Cvar_RegisterVariable(D_0032F010);
    Cvar_RegisterVariable(D_0032F028);
    Cvar_RegisterVariable(D_0032F040);
    Cvar_RegisterVariable(D_0032F058);
    Cvar_RegisterVariable(D_0032F070);
    Cvar_RegisterVariable(D_0032F088);
    Cvar_RegisterVariable(D_0032F0A0);
    Cvar_RegisterVariable(D_0032F0B8);
    Cvar_RegisterVariable(D_0032F0D0);
    Cvar_RegisterVariable(D_0032F0E8);
    Cvar_RegisterVariable(D_0032F100);
    Cvar_RegisterVariable(D_0032F118);
    Cvar_RegisterVariable(D_0032F130);
    Cvar_RegisterVariable(D_0032F148);
}

int func_00239D40(void) {
    return D_0032F15C;
}

void func_00239D50(int mode) {
    D_0032F15C = mode;
}

int func_00239D60(void) {
    switch (D_0032F15C) {
    case 0:
        return 1;
    case 1:
        return 2;
    }
    return 1;
}
