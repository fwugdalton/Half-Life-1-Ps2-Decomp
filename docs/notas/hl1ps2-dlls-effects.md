---
name: hl1ps2-dlls-effects
description: effects.cpp do PS2 (faixas, diferenças do HLSDK, env_extremefunnel, ordem de vtables/inlines) e lições
metadata:
  type: project
---
effects.cpp (commit 27983de, 207/212): .text 0x15A6C0–0x161BB8, .data 0x30F810 (m_SaveData), .rodata 0x347560–0x349EA0. Base: effects.cpp do HLSDK com floats `f` e .spz/.dol.

Diferenças do PS2: UTIL_TraceLineNoEnts em vez de UTIL_TraceLine; CBubbling::Spawn usa `int speed = fabsf(pev->speed)`; CSprite::Spawn sem o hack de angles; StrikeThink do CLightning faz MESSAGE_BEGIN antes do teste de SF_BEAM_RING; CLaser::StrikeThink só traça se `!g_pGameRules->IsMultiplayer()`; CBlood::Use sem o `? 70`; CFade::Use testa pActivator; env_funnel com spawnflag 2 (repetível) não se remove; CLightning::LightningCreate (como BeamCreate, classname "env_beam"); classe nova CEnvExtremeFunnel : CBaseEntity (tamanho 0x44, TE 128, três thinks, `pev->nextthink = gpGlobals->time += m_flDelay`); registro func_00160740 ("082"–"104", sem "100").

Lições:
- O gcc emite no fim todas as inline das classes com vtable aqui, na ordem em que os corpos foram definidos. O CGib (ObjectCaps em 0x161500) tem que vir antes do #include do effects.h; a classe CLightning foi definida antes da CBubbling para o ServerSide sair antes do ObjectCaps da CBubbling.
- Tirar uma inline virtual da classe (definir fora) muda a ordem das vtables no .rodata; o datacmp não vê (ignora relocações), só o build completo.
- A string do ALERT do StrikeThink vem no asm NON_MATCHING: o corpo C vai no `#else`.
- GetClassPtr<CGib>, ObjectCaps__4CGib e _vt$4CGib daqui são cópias weak (a ROM chama as de 0x14D598); não pôr no symbol_addrs.
- include/dlls/effects.h agora é o do HLSDK inteiro (+ BEAM_F* do customentity.h).
