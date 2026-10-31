---
name: hl1ps2-dlls-done-10
description: continuação da lista de .cpp das dlls feitos (client em diante), lições do client.cpp e próximo arquivo (cbase.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-9]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- client (0x1470F8, 08a9916, 25/25): sem .data, .rodata 0x343338–0x343C53. É o client.cpp do SDK ANTIGO: a tabela de funções (gFunctionTable em 0x30EBE8, no .data do cbase) não tem ServerDeactivate, então depois do ServerActivate vêm PreThink, PostThink, StartFrame, ParmsNewLevel (vazio), ParmsChangeLevel, GetGameDescription, PlayerCustomization, Spectator* e uma entrada nova do PS2 (Ps2PlayerCommand(a,b,c) = g_pGameRules->Ps2Virt63). Host_Say antigo (laço com UTIL_FindEntityByClassname "player"); dentro do laço o WRITE_BYTE usa ENTINDEX(client->edict()), não pEntity (sem isso os registros s0/s1 trocam). Mudanças do PS2: ClientDisconnect sem o "" nem o if netname; ClientPutInServer só SetCustomDecalFrames(-1)+Spawn; ClientCommand tem "goto x y z" (UTIL_SetOrigin, sem trapaça); ServerActivate sem LinkUserMessages; StartFrame roda Ps2AIThink nos jogadores FL_FAKECLIENT quando cl_numlocal < 2; ClientPrecache só precachea "models/player.dol" e pl_long_jump.wav se !IsMultiplayer. customization_t declarado local (type é byte em 0x44, nUserData2 em 0x5C).
  - player.h ganhou m_fNextSuicideTime (0x2F8), PreThink/PostThink (slots 130/131, antes Ps2Virt130/131), SetCustomDecalFrames 0x1D0798, DropPlayerItem 0x1D07C0, SelectItem 0x1CBE78, SelectLastItem 0x1CBFA8.
  - decay_gamerules chamava func_00147160: agora declara `BOOL ClientConnect(...)` em C++.
  - O .rodata do client termina em 0x343C53 e o do combat começa em 0x343C60: foi preciso `--set-section-alignment .rodata=16` no combat.o (Makefile). Sintoma de alinhamento errado: o ROM falha com %lo de vtables deslocados de 0x10 bem mais à frente (ver o .map).
  - D_00343430 ("use") é citado por outro .rodata em asm: definido no linker_script_extra.ld.

- cbase (0x144E98, 4e9ca2f, 91/91): .data 0x30EBE8 (gFunctionTable de 33 entradas, gTouchDisabled 0x30EC6C, m_SaveData 0x30EC70), .rodata 0x342FF0 (vtable CBaseEntity 0x343158). HLSDK quase igual; GetEntityAPI sem GetEntityAPI2 (versão 138), DispatchRestore lê o location com func_00236B48 (int desalinhado). O gcc 2.95 solta TODAS as funções inline de CBaseEntity (virtuais ou não) no fim do .text do arquivo da vtable, na ordem da classe: por isso HasTarget teve de virar inline no cbase.h. O construtor implícito de entvars_t sai fora de linha antes do DispatchRestore: resolvido com `entvars_s();` sob `#ifdef ENTVARS_CTOR_OUTOFLINE` no extdll.h e `entvars_s::entvars_s() {}` no cbase.cpp. _vt$11CBaseEntity = 0x343158 e D_00343030 definidos.

Próximo: buttons.cpp (as cópias weak de GetClassPtr em 0x144B20.. e as funções pequenas 0x144E18–0x144E78 são o fim dele). .text termina em 0x144E98, .rodata termina em 0x342FF0, .data termina em 0x30EBE8.
