---
name: hl1ps2-cldll-5
description: geiger, flashlight e death.cpp do cl_dll (faixas, campos do CHud, truque da rodata em asm com force_not_migration) e próximo arquivo
metadata:
  type: project
---
Feitos em 2026-11-01 (continuação de [[hl1ps2-cldll-4]]):
- geiger.cpp (ce9341c, 7/8, texto 0x118398–0x1187E8, .rodata 0x3390B0, vtable 0x3390D0): CHudGeiger em gHUD+0x1B8. Draw sai se !(flags & 0x24), sem srand, som por gEngfuncs+0x64 (pfnPlaySoundPs2(sz, vol, clientId)). Draw NON_MATCHING (ROM usa 3 regs salvos, o meu 4).
- flashlight.cpp (e17fe0d, 10/10, texto 0x117D50–0x118398, .rodata 0x339020, vtable 0x339070): CHudFlashlight em gHUD+0x21C (0x40 bytes). Draw: !(flags & 0xA), m_iPs2Flags & 6, & 0x40, m_iPs2Unk20E4 >= 0 saem; cor do HUD (m_iColorR/G/B) no lugar do YELLOWISH; x -8, y +24. `/100.0f` casa (o 100.0 dava 16 instruções diferentes).
- death.cpp (8ed91af, 7/9, texto 0x117300–0x117D50, .rodata 0x338F00, vtable 0x338FE0, .data 0x30CBF8 DEATHNOTICE_DISPLAY_TIME=6 + rgDeathNoticeList[2][5] = {0} em 0x30CC00): CHudDeathNotice em gHUD+0x1CC0. Item de 0x54 bytes (szKiller[32], szVictim[32], iId, iSuicide, iTeamKill, flDisplayTime, iClient 0x50). InitHUDData(void) não sobrescreve o virtual; Reset(int) faz o memset. Sem cores, sem gViewPort, sem "killed a"; nomes vêm de gHUD[c].m_Scoreboard.m_PlayerInfoList; CVAR_CREATE só com clientId 0 e flags 0; y = 44 + 20*i; x -8.
- Draw (só ordem de 3 slots de pilha) e MsgFunc (alocação) NON_MATCHING. Como o asm do MsgFunc usa as strings, a .rodata 0x338F00–0x338FE0 inteira vai num `__asm__(".section .rodata" ... .word ...)` no topo, com `extern "C" const char D_xxx[]` para as strings do Init/VidInit (HOOK_MESSAGE escrito à mão). Precisa `D_xxx = 0x...; // force_not_migration:True` no symbol_addrs para cada string, senão o splat migra e dá "already defined".
- vtable = 8 bytes antes do 1º pfn (entradas de 8 bytes; Init em vt+0xC). Ordem: Init, VidInit, Draw, Think, Reset, InitHUDData; 0x114E48/0x114840 são as cópias de Think/InitHUDData do CHudBase usadas por todos.
- Próximo: o arquivo antes de 0x117300 (cópias fracas em 0x1172F0/0x1172F8; funções desde ~0x116xxx; ver a .rodata antes de 0x338F00).
