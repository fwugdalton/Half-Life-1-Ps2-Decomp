/* Gbx: música do CD (faixas .VAG tocadas pelo IOP) e o comando "cd" */

#include "common.h"
#include "engine/cmd.h"

/* argumentos do RPC 400 */
typedef struct {
    signed char track;
    char loop;
    short pad;
    int lba;
    int sectors;
    int size;
} MusicArgs;

int func_00237128(const char *name, int *size, int *lba, int *sectors);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(void *cd);
int atoi(const char *s);

extern char D_0032EF40[];       /* cliente RPC do IOP */
extern MusicArgs D_003A9580;
extern MusicArgs D_003A95C0;
extern int D_004A9294;

void func_00239318(int track, int loop) {
    char name[0xD0];

    if (track < 2) {
        return;
    }
    sprintf(name, "MUSIC\\TRACK%d.VAG;1", track);
    if (func_00237128(name, &D_003A9580.size, &D_003A9580.lba, &D_003A9580.sectors) == 1) {
        D_003A9580.track = track;
        D_003A9580.loop = loop;
        while (func_002F2890(D_0032EF40) != 0) {
        }
        func_002F26A0(D_0032EF40, 400, 0, &D_003A9580, 0x10, 0, 0, 0, 0);
    }
}

void func_002393D8(const char *file, int loop) {
    char name[0xD0];

    sprintf(name, "MUSIC\\%s.VAG;1", file);
    if (func_00237128(name, &D_003A95C0.size, &D_003A95C0.lba, &D_003A95C0.sectors) == 1) {
        D_003A95C0.loop = loop;
        D_003A95C0.track = -1;
        while (func_002F2890(D_0032EF40) != 0) {
        }
        func_002F26A0(D_0032EF40, 400, 0, &D_003A95C0, 0x10, 0, 0, 0, 0);
    }
}

void func_00239488(void) {
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 401, 0, 0, 0, 0, 0, 0, 0);
    D_004A9294 = 0;
}

void func_002394F0(void) {
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 402, 0, 0, 0, 0, 0, 0, 0);
}

void func_00239550(void) {
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 403, 0, 0, 0, 0, 0, 0, 0);
}

void func_002395D8(void);

void func_002395B0(void) {
    Cmd_AddCommand("cd", func_002395D8);
}

void func_002395D8(void) {
    char *command;

    if (Cmd_Argc() < 2) {
        return;
    }
    command = Cmd_Argv(1);
    if (strcasecmp(command, "on") == 0) {
    }
    if (strcasecmp(command, "off") == 0) {
        func_00239488();
    }
    if (strcasecmp(command, "reset") == 0) {
    }
    if (strcasecmp(command, "remap") == 0) {
    }
    if (strcasecmp(command, "close") == 0) {
    }
    if (strcasecmp(command, "play") == 0) {
        func_00239318(atoi(Cmd_Argv(2)), 0);
    }
    if (strcasecmp(command, "loop") == 0) {
        func_00239318(atoi(Cmd_Argv(2)), 1);
    }
    if (strcasecmp(command, "playfile") == 0) {
        func_002393D8(Cmd_Argv(2), 0);
    }
    if (strcasecmp(command, "loopfile") == 0) {
        func_002393D8(Cmd_Argv(2), 1);
    }
    if (strcasecmp(command, "stop") == 0) {
        func_00239488();
    }
    if (strcasecmp(command, "pause") == 0) {
        func_00239488();
    }
    if (strcasecmp(command, "gbxpause") == 0) {
        func_002394F0();
    }
    if (strcasecmp(command, "resume") == 0) {
        func_00239550();
    }
    if (strcasecmp(command, "eject") == 0) {
    }
    if (strcasecmp(command, "info") == 0) {
    }
}
