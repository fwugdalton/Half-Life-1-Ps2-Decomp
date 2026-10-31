---
name: hl1ps2-dlls-decay
description: decay_gamerules.cpp (co-op do Decay no PS2, sem fonte): campos, funções, registro de funções externas e truques que casaram
metadata:
  type: project
---
dlls/decay_gamerules.cpp feito em 2026-10-31 (commit e17001f), 52/52 casam de primeira quase tudo. É uma cópia do coopplay_gamerules.cpp (mesmas funções de respawn/pontos/queda) mais o Decay:
- Campos 0x1C..0x58: m_DisableDeath* (0x1C/0x20), unsigned char m_rgPlayers[2] (ENTINDEX dos jogadores 1/2, 0x24), m_iSwitchState 1/2 (0x28), m_iNumPlayers 0x2C, m_iNumAIPlayers 0x30, m_fUseSpawnSpot 0x34, Vector origem/ângulos 0x38/0x44, m_fAIPlayersCreated 0x50, m_flAIPlayerTime 0x54, m_fUpdateBodies 0x58.
- Funções: AddPlayer (devolve 1/2 → m_ps2ClientIndex), Ps2Virt64 (jogador pelo índice 1/2), CreateAIPlayer (CREATE_FAKE_CLIENT "Colette"/"R-4913" com mp_coopslave D_00310060, ClientConnect=func_00147160, init da IA func_00126F80), SwapPlayers (troca tudo entre dois CBasePlayer), CoopSwitch (cl_numlocal==1), PlayerKilled dispara "decay_player1_dead"/"decay_player2_dead" com Use(USE_TOGGLE).
- Lanterna do player.cpp: func_001CC800 FlashlightIsOn, 001CC810 TurnOn, 001CC928 TurnOff. pev->effects &= ~256 nos EHANDLE 0x77C/0x79C do jogador; pfnPs2Unk1E0(1,0).
- Registro func_00154318 aponta para funções de outro arquivo ainda em asm: dei nomes C++ (Ps2AIThink__11CBasePlayer=0x1270E0, Ps2AITouch__11CBasePlayerP11CBaseEntity=0x12BFB8) no symbol_addrs.txt e declarei no player.h; o glabel do asm passa a definir o símbolo.
- Truques: `int i;` no topo do Think servindo aos dois laços; `CBasePlayerItem *pItem;` no topo do SwapPlayers e reaproveitado na troca do m_pActiveItem/m_pLastItem (registradores das cópias por memcpy só casaram assim); arrays temporários (szModel[1024], itens, munição) em blocos separados dividem o mesmo slot da pilha; `pszMap + ( strlen( pszMap ) - 5 )`; `WRITE_SHORT( (short)pev->frags )` gera sll/sra; GetPlayerSpawnSpot com `if (!m_fUseSpawnSpot) return base;` primeiro.
Ver [[hl1ps2-dlls-done-9]] e [[hl1ps2-dlls-focus]].
