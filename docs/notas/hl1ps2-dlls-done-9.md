---
name: hl1ps2-dlls-done-9
description: continuação da lista de .cpp das dlls feitos (crossbow em diante) e próximo arquivo
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-8]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- crossbow (0x154480, 7ec56dd, 34/34): .data 0x30EE60 (m_SaveData com m_fInZoom), .rodata 0x345910 (vtables CCrossbowAmmo 0x345B80, CCrossbow 0x345D68, CCrossbowBolt 0x346040). SDK antigo (sons e animações no FireBolt/FireSniperBolt, sem eventos). PS2: PrimaryAttack soma m_ps2ShotsFired e usa Ps2Virt07 (slot 0x38) no lugar do IsMultiplayer; FireBolt vibra com func_002071C8(p, 1, RANDOM_LONG(50,200), 50), usa UTIL_SetOrigin no bolt, idle = 0.75f sem somar o tempo, punchangle.x -= 2; FIRE3 e "!HEV_AMO0" checam m_ps2InfiniteAmmo; SecondaryAttack só mexe no m_iFOV; Reload sem checar ammo_bolts; BoltTouch: pPlayer = Instance(owner) e zera se !IsPlayer() (assim casou; com ?: não), sons no CHAN_WEAPON, Killed só se !Ps2Virt07(), m_ps2ShotsHit++ se MyMonsterPointer() && FPlayerCanTakeDamage, ScoreInfo igual ao do gauss. FireSniperBolt: hitbod no tr.pHit com CHAN_BODY, cria bolt parado (SOLID_NOT) se não acertou nada. Um `TraceResult tr;` sem uso no FireBolt muda o tamanho da pilha (tirar). Registro func_00156590 é extern "C" e friend do CCrossbowBolt.

Próximo: o arquivo do PS2 antes do crossbow (.text termina em 0x154480; .rodata 0x345440–0x345910 com "HL", "R-4913", "Colette", "dm_slave", "coopsidekick", "coopswitch", "game_player_equip", "decay_player1_dead", registro "068"/"069"). Antes dele vem o do "ps2controller_hud" (CControllerHud, .rodata 0x3451B0–0x345440, .data com "m_iHoldTime" em 0x30EE50).
