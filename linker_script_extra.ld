/* Pass this file to the linker with the `-T linker_script_extra.ld` flag */
ENTRY(_start);

/* strings da .rodata emitidas pelo C (Host.c) e usadas também por funções INCLUDE_ASM */
D_0038C110 = 0x0038C110; /* "deathmatch" */
D_0038C5F8 = 0x0038C5F8; /* "half-life.east.won.net:27010" */
