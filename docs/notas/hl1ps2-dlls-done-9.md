---
name: hl1ps2-dlls-done-9
description: continuação da lista de .cpp das dlls feitos (crossbow em diante) e próximo arquivo
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-8]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- crossbow (0x154480, 7ec56dd, 34/34): .data 0x30EE60 (m_SaveData com m_fInZoom), .rodata 0x345910 (vtables CCrossbowAmmo 0x345B80, CCrossbow 0x345D68, CCrossbowBolt 0x346040). SDK antigo (sons e animações no FireBolt/FireSniperBolt, sem eventos). PS2: PrimaryAttack soma m_ps2ShotsFired e usa Ps2Virt07 (slot 0x38) no lugar do IsMultiplayer; FireBolt vibra com func_002071C8(p, 1, RANDOM_LONG(50,200), 50), usa UTIL_SetOrigin no bolt, idle = 0.75f sem somar o tempo, punchangle.x -= 2; FIRE3 e "!HEV_AMO0" checam m_ps2InfiniteAmmo; SecondaryAttack só mexe no m_iFOV; Reload sem checar ammo_bolts; BoltTouch: pPlayer = Instance(owner) e zera se !IsPlayer() (assim casou; com ?: não), sons no CHAN_WEAPON, Killed só se !Ps2Virt07(), m_ps2ShotsHit++ se MyMonsterPointer() && FPlayerCanTakeDamage, ScoreInfo igual ao do gauss. FireSniperBolt: hitbod no tr.pHit com CHAN_BODY, cria bolt parado (SOLID_NOT) se não acertou nada. Um `TraceResult tr;` sem uso no FireBolt muda o tamanho da pilha (tirar). Registro func_00156590 é extern "C" e friend do CCrossbowBolt.

- decay_gamerules (0x151B58, e17001f, 52/52): arquivo só do PS2, nomes inventados (CHalfLifeDecay : CHalfLifeMultiplay, "HL Coopplay", 0x5C bytes; ctor 0x152E20 chamado pelo InstallGameRules como func_00152E20, definido no linker_script_extra.ld). Sem .data, .rodata 0x345440 (vtable 0x3456D8). Detalhes e truques em [[hl1ps2-dlls-decay]].

- controller_hud (0x1517C8, c2358a5, 11/11): ps2controller_hud (CControllerHud : CPointEntity, 0x38 bytes, campo sem uso em 0x30, m_iHoldTime 0x34), .data 0x30EE50 (m_SaveData), .rodata 0x3451B0 (vtable 0x345248). HudUse manda D_00317D20 ao jogador 1 com spawnflags e holdtime. Truque: o "Half-Life" vem logo depois do "target" porque o gamerules.h é incluído ANTES do cbase.h (o "" do TeamID sai depois). Precisa de `_GLOBAL_$I$ps2controller_hud` no symbol_addrs (a tabela de construtores do .data cita).

Próximo: controller.cpp (monster_alien_controller do HLSDK): .text termina em 0x1517C8 (static init 0x151748 registra monster_alien_controller, D_003442D8 e D_00344338), .rodata termina em 0x3451B0 ("monster_alien_controller" em 0x345190).
