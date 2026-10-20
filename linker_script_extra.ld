/* Pass this file to the linker with the `-T linker_script_extra.ld` flag */
ENTRY(_start);

/* strings da .rodata emitidas pelo C (Host.c) e usadas também por funções INCLUDE_ASM */
D_0038C110 = 0x0038C110; /* "deathmatch" */
D_0038C5F8 = 0x0038C5F8; /* "half-life.east.won.net:27010" */
D_0038D080 = 0x0038D080; /* "\n" (keys.c) */
D_0038D150 = 0x0038D150; /* "Bad key_dest" (keys.c) */
D_0038D660 = 0x0038D660; /* "\n" (pmove.c) */
D_0038E478 = 0x0038E478; /* "Non-sprite set to glow!\n" (r_trans.c) */
/* engine/sv_phys: início */
D_0038FA20 = 0x0038FA20;
D_0038FA38 = 0x0038FA38;
/* engine/sv_phys: fim */
D_00334EAE = 0x00334EAE; /* contador de amostras do canal direito (snd_win_gbx.c) */
D_00382E30 = 0x00382E30; /* valor 0x382E30 em .rodata do engine (cai no meio do .rodata do wheelchair.cpp) */

/* strings do Gbx/MemFileSystem.cpp emitidas pelo C++ e usadas por funções INCLUDE_ASM */
D_00386F30 = 0x00386F30; /* "Gbx/MemFileSystem.cpp" */
D_00386F48 = 0x00386F48; /* "wb" */
/* Gbx/SystemSave.cpp */
D_00387238 = 0x00387238; /* "/" */
/* Gbx/GameSave.cpp: strings das cvars */
D_00387428 = 0x00387428; /* "Slot1Available" */
D_00387438 = 0x00387438; /* "0" */
D_00387440 = 0x00387440; /* "Slot2Available" */
D_00387490 = 0x00387490; /* "TempMCSave.sav" */
D_003874A0 = 0x003874A0; /* "%s%s" */
D_003874C0 = 0x003874C0; /* "%d" */
D_003874C8 = 0x003874C8; /* "NumSaves" */
D_00324C60 = 0x00324C60;
/* engine/sv_main: início */
D_0038E8F8 = 0x0038E8F8;
/* engine/sv_main: fim */
