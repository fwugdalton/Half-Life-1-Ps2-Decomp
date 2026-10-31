---
name: hl1ps2-dlls-done-5
description: continuação da lista de .cpp das dlls feitos (glock em diante)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-4]]. Formato: arquivo (início do .text, commit, funções casadas): detalhes.

- glock (0x179DA8, ca810f5, 21/21): sem .data, .rodata 0x353420. Versão antiga do SDK (OLD_WEAPONS: SendWeaponAnim, EjectBrass com m_pPlayer->pev->origin, EMIT_SOUND_DYN pl_gun3 no servidor, punchangle -2), sem silenciador (só o ramo NORMAL_GUN) e sem eventos/m_usFireGlock (classe com 0x90 bytes, m_iShell em 0x8C). PS2: rumble func_002071C8(m_pPlayer,1,120,50) antes do FireBullets, munição infinita (m_ps2InfiniteAmmo) no aviso !HEV_AMO0; Reload com iResult = 0, DefaultReload(17, RELOAD) com pente vazio e DefaultReload(18, RELOAD_NOT_EMPTY) só se m_iClip != 17. Casou de primeira. Um ponteiro falso D_00353730 do gl_rmisc foi para linker_script_extra.ld.
- globals (0x179D58, e8cdf59, 2/2): .data 0x312C18 (layout casou com a ordem do SDK: g_vecAttackDir, g_vecZero, g_Language, depois os outros), .rodata 0x3533F0 (strings inline). g_vecZero sem const (util.h declara sem) e com "= Vector(0,0,0)", que gera o static init 0x179D58 + _GLOBAL_$I$g_vecAttackDir 0x179D88. D_00312C44/48 (g_iSkillLevel/gDisplayTitle, usados por nome D_ em hassassin/singleplay/hgrunt) foram para linker_script_extra.ld.
- ggrenade (0x177C58, 5b35f9d, 25/27): sem .data, .rodata 0x352E18 (vtable 0x352FD8). Incluir skill.h e não gamerules.h (senão entra "Half-Life" no .rodata). PS2: Explode faz rumble (func_002071C8(p,1,(int)(dmg*1.7f),250)) nos jogadores 1 e 2 (CBaseEntity::Instance(INDEXENT(n)), IsPlayer, distância < 400); Spawn zera m_fRegisteredSound e depois m_iPs2Unk2A4; DropPickedUp/ThrowPickedUp novos (velocity, BounceTouch, origin+1 no Drop, flags &= ~FL_ONGROUND, effects &= ~EF_NODRAW, MOVETYPE_BOUNCE, SOLID_BBOX, owner, dmgtime, TumbleThink, nextthink+0.1, sequence RANDOM_LONG(3,6), framerate 1, frame 0, m_iPs2Unk2A4 = 0). BloodColor é `inline` fora da classe (sai depois do static init). Registro "120"–"130" em func_00179AF8. Explode e Smoke NON_MATCHING (conversão float->int agendada antes dos outros argumentos, igual rpg/maprules): o asm do Explode já traz as strings dos debris; a constante do Smoke e "spark_shower" saem de uma função em seção descartada (D_00352E90/98 no linker_script_extra.ld).
