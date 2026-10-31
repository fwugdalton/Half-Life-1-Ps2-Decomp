---
name: hl1ps2-dlls-done-6
description: continuação da lista de .cpp das dlls feitos (item_generic em diante)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-5]]. Formato: arquivo (início .text, commit, funções que casam).

- item_generic (0x176970, 400870e, 12/12): entidade só do PS2, arquivo src/dlls/item_generic.cpp. CGenericItem : CBaseAnimating (0x54: m_lastTime 0x4C, m_iszSequenceName 0x50), sem save próprio, sem .data, .rodata 0x352420 (vtable 0x3524C8). KeyValue sequencename (vazio ou "0" vira 0)/skin ((int)atof)/body (atoi); Spawn com SOLID_NOT, MOVETYPE_NONE, effects e frame 0, StartItem se tem sequência ou spawnflags & 1; StartItem faz DROP_TO_FLOOR com ALERT(at_error,"Item %s fell out of level...") + UTIL_Remove; AnimateThink em loop; Use remove em 0.1s. Registro "117"–"119" em func_00176DF8. A função solta no fim (0x176F18, rendermode/rendercolor/renderamt/renderfx) é um membro não virtual declarado na classe e definido como `inline` depois de tudo: o gcc 2.95 emite mesmo sem uso, depois do static init. Casou de primeira.
- gauss (0x1741C8, bfda772, 22/24): versão antiga sem eventos (não existe no HLSDK; reconstruída do asm com o gauss.cpp novo e o EV_FireGauss do cl_dll/ev_hldm.cpp). Campos no CGauss (0x8C m_fInAttack, 0x90 m_flStartCharge, 0x94 m_flPlayAftershock, 0x98 m_iBalls, 0x9C m_iGlow, 0xA0 m_iBeam, 0xA4 m_iSoundState, 0xA8 m_flNextAmmoBurn, 0xAC m_fPrimaryFire), .data 0x312B38, .rodata 0x351D28 (vtables CGaussAmmo 0x351F50, CGauss 0x352138). GetFullChargeTime e a recarga do tiro 2 usam g_pGameRules->Ps2Virt07(), o StartFire usa Ps2Virt08(), o placar no fim do Fire usa IsMultiplayer()/Ps2Virt07() + cl_numlocal. Spawn põe classname "weapon_gauss". PS2: rumble, munição infinita, m_ps2ShotsFired++ no Fire e m_ps2ShotsHit++ se FPlayerCanTakeDamage, ScoreInfo no fim. Chamada de GetAutoaimVector sem variável (com variável a pilha cresce 16). Todos os floats com f. NON_MATCHING: SecondaryAttack (rumble (int)(carga*225) e pitch) e Fire (registradores e WRITE_BYTE de float); as strings só do SecondaryAttack vêm no asm, D_00351E58/88/EA0 no linker_script_extra.ld.

Próximo: gargantua.cpp (.text termina em 0x1741C8, _GLOBAL_ em 0x1741A8; .data termina em 0x312B38; .rodata termina em 0x351D28, "monster_gargantua" em 0x351D10).
