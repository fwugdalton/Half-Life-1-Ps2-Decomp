/* Gbx: tela de "disco não encontrado" (GUIs/DiscFail.pak) enquanto o CD não volta */

#include "common.h"

extern "C" {
#include "engine/decals.h"

/* ee_sema_t */
struct SemaParam {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
};

/* objeto de GUI (0x64 bytes) */
struct Gui {
    int pad[0x19];
};

int func_002EF660(SemaParam *p);
int func_002EF680(int sema);
int func_002EF6A0(int sema);
void func_002EF8E0(int a);
void func_0010EC68(Gui *gui);
void func_0010EC78(Gui *gui, int flags);
void func_0010EE50(Gui *gui, pakfile_t *pak, const char *name, int a);
void func_0010F0B0(Gui *gui);
void func_0010F1E0(Gui *gui);
void func_0010F2B8(Gui *gui);
int func_0010F390(Gui *gui);
void func_00225438(int a);
void func_00225458(void);
void func_00225490(void);
void func_002254A8(void);
void func_00225590(Gui *gui);
void func_002255B0(void);
void func_002255E8(void);
void Pak_Init(pakfile_t *pak);
void func_00235038(pakfile_t *pak, byte *data, int size);
void func_00235198(pakfile_t *pak);
void func_00235EA0(const char *name, byte **data, int *size);
int func_00235F18(void);
int func_002D3220(int a, int b, void *out, void *cmd);
int func_002D3A98(int a);
int func_002D4000(int a);
int func_00236E68(void);

extern int D_00328B1C;
extern int D_003A8180[];
extern int D_003A8980;
extern int D_003A8984;
extern int D_003A8988;

void func_00236BF8(void) {
    SemaParam p;

    p.init_count = 1;
    p.max_count = 1;
    p.option = 0;
    func_002EF8E0(0);
    D_003A8980 = func_002EF660(&p);
    D_003A8984 = func_002EF660(&p);
    p.max_count = 1;
    p.init_count = 0;
    p.option = 0;
    func_002EF8E0(0);
    D_00328B1C = func_002EF660(&p);
    D_003A8988 = 0;
}

void func_00236C78(void) {
    pakfile_t pak;
    Gui gui;
    byte *data;
    int size;

    func_002EF6A0(D_003A8980);
    if (func_00235F18() == 1) {
        func_0010EC68(&gui);
        func_002EF6A0(D_003A8984);
        D_003A8988 = 1;
        func_002EF680(D_003A8984);
        func_00225458();
        func_00225438(1);
        func_00235EA0("GUIs/DiscFail.pak", &data, &size);
        Pak_Init(&pak);
        func_00235038(&pak, data, size);
        func_0010EE50(&gui, &pak, "DiscFail.txt", 0);
        func_002255B0();
        func_00225590(&gui);
        do {
            func_002254A8();
            func_0010F0B0(&gui);
            func_0010F2B8(&gui);
            func_0010F1E0(&gui);
            if (func_0010F390(&gui) != 0) {
                break;
            }
        } while (func_00236E68() == 0);
        func_002255E8();
        func_00235198(&pak);
        func_00225490();
        func_002EF6A0(D_003A8984);
        D_003A8988 = 0;
        func_002EF680(D_003A8984);
        func_002EF680(D_00328B1C);
        func_0010EC78(&gui, 2);
    }
    func_002EF680(D_003A8980);
}

int func_00236E28(void) {
    int r;

    func_002EF6A0(D_003A8984);
    r = D_003A8988;
    func_002EF680(D_003A8984);
    return r;
}

#ifdef NON_MATCHING
/* o original guarda a constante 1 em s0; aqui o gcc reaproveita o retorno de func_002D3220 */
int func_00236E68(void) {
    unsigned char cmd[0x10];
    int ret;

    ret = 0;
    if (func_002D4000(1) == 2) {
        cmd[0] = 5;
        cmd[1] = 1;
        cmd[2] = 0;
        if (func_002D3220(0x10, 1, D_003A8180, cmd) == 1) {
            if (func_002D3A98(0) == 0) {
                ret = 1;
            }
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/DiscFail", func_00236E68);
#endif
}
