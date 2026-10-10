#include "common.h"

INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", ResampleSfx);
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038E4E0\nD_0038E4E0: .asciz \"sound\"\n"
        ".align 3\n.globl D_0038E4E8\nD_0038E4E8: .asciz \"/\"\n"
        ".section .text\n");
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", S_LoadSound);
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038E508\nD_0038E508: .asciz \"%s is a stereo sample\\n\"\n"
        ".align 3\n.globl D_0038E520\nD_0038E520: .asciz \"%s is a 16 bit sample\\n\"\n"
        ".section .text\n");
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", S_LoadSoundFromMemory);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", S_LoadStreamChunk);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", S_LoadStreamSound);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", GetLittleShort);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", GetLittleLong);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", FindNextChunk);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", FindChunk);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", DumpChunks);
INCLUDE_ASM("asm/nonmatchings/engine/snd_mem", GetWavinfo);
