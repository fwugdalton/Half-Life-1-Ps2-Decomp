---
name: hl1ps2-dlls-focus
description: focus_emitter.cpp (item_focusemitter/info_emittertarget do Decay, sem fonte): layout, truques e como escrever um arquivo sem SDK
metadata:
  type: project
---
dlls/focus_emitter.cpp feito em 2026-10-31 (commit cf87a0a), 21/22 funções casam. Não existe no HLSDK: nomes de arquivo, classes (CEmitterTarget : CBaseToggle, tamanho 0xD0; CFocusEmitter : CBaseMonster, 0x304), métodos e campos são inventados; a ordem saiu do .text, os tipos do asm.
- Faixas: .text 0x165CA8–0x167630, sem .data, .rodata 0x34B3C8–0x34BB58 (vtables CFocusEmitter 0x34B568 com 0x408 bytes, CEmitterTarget 0x34B970). Os 8 bytes 0x34BB58–60 são padding: func_break.o ganhou .rodata=16 no Makefile.
- Como descobrir a estrutura sem fonte: a vtable diz quais virtuais a classe sobrescreve; o tamanho vem do ALLOC_PRIVATE no GetClassPtr; o __static_initialization (func_0021D8A8) dá os LINK_ENTITY_TO_CLASS; os ponteiros {0xffff0000, fn} na .rodata dão os SetThink; a ordem das strings da .rodata confirma a ordem das funções.
- Campos 0x2A0..0x300: EHANDLE alvo, 2 CBeam*, yaw/pitch (ideal, atual, delta), contador, flags deploy/ativo/pausa, iszDeployTarget/DeathTarget (KeyValue com `if (szValue && *szValue) ALLOC_STRING else 0`), m_iState 0..3, índice do fexplo, próximo tempo de mira.
- Truques: no Spawn a ordem dos stores só casou com `a = b = 0` invertido (`m_flYaw = m_flIdealYaw = 0`, `pev->health = pev->max_health = x`) e os zeros finais em ordem estranha (o último comando sai primeiro); FStrEq(STRING(pev->classname), ...) em vez de FClassnameIs (registradores); `Vector vecDir;` declarado antes dos outros e atribuído depois (cópia via pilha); EHANDLE com `(CBaseEntity *)h` para chamar __opP (o cmpfile não vê, só o cmp da ROM).
- TraceAttack NON_MATCHING: a ROM lê m_fActive (0x2D4) à toa num delay slot; nenhuma variante reproduziu. A string "monster_alienflyer" (D_0034B520) foi para o linker_script_extra.ld.
- Registro func_001672C8: 107 DeployThink, 108 FallThink, 110 TrackThink.
Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-done-7]].
