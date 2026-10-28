---
name: hl1ps2-dlls-squeak
description: squeakgrenade.cpp (snark) feito: faixas, diferenças do PS2, gSkillData em 0x318618 e slot 7 do CGameRules
metadata:
  type: project
---

squeakgrenade.cpp feito em 2026-10-20, commit 0347efb, 26 de 26 funções casam.
- Faixas: texto 0x1EC6A0–0x1EE388, .data 0x3237C8–0x323830 (m_flNextBounceSoundTime + m_SaveData), .rodata 0x376240–0x376B5D (vtables CSqueak 0x376460, CSqueakGrenade 0x376738).
- Versão antiga do HLSDK (gpGlobals->time, sem eventos, sem AngleVectors). PS2: PrimaryAttack chama SendWeaponAnim(SQUEAK_THROW) antes do SetAnimation, trace de +16 a +64, flFraction > 0.25; WeaponIdle começa com m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES) sem usar o resultado; SuperBounceTouch sem o teste FL_WORLDBRUSH e usa o slot 7 do CGameRules (Ps2Virt07, que é o IsMultiplayer do HLSDK); HuntThink usa `==` no MOVETYPE_FLY e não tem TraceResult local.
- gSkillData passou para 0x318618 (agruntHealth 0x31861C); skill.h ganhou `float ps2Unk[2]` depois do bigmomma (posição exata a ver; todos os campos usados até agora mantiveram o endereço absoluto).
- soundent.h agora declara CSoundEnt (só InsertSound); as cópias locais em tentacle*.cpp foram tiradas.
- Registro de think "268"–"271" em func_001EE138 (271 é touch, func_002397F0).

squadmonster.cpp feito em 2026-10-20, 27 de 27 casam. Texto 0x1EB048–0x1EC6A0, .data 0x323778 (m_SaveData), .rodata 0x375D38 (vtable 0x375E38). include/dlls/squadmonster.h criado.
- PS2: CSquadMonster::TakeDamage novo (se flDamage >= pev->max_health, avisa os membros sem inimigo com UTIL_TraceLineNoEnts; senão m_IdealMonsterState = HUNT). Truques: `Vector vecDir; vecDir = a - b;` (atribuição, não inicialização) e o if invertido (`!= 1.0f` primeiro).
- Conditions inline no arquivo todo (bloco no topo). As inline MySquadLeader/MySquadMember/InSquad/IsLeader saem no fim mesmo sem uso (o gcc as emite sozinho).
- CPlane (plane.cpp): ctor 0x1BD368, InitializePlane 0x1BD378, PointInFront 0x1BD3D0.

spectator.cpp feito em 2026-10-20, casou de primeira. Texto 0x1EAE30–0x1EB048, .data 0x323774 (pGoal estático, 4 bytes: precisa `.data=4` no Makefile), .rodata 0x375AD0 (vtable 0x375B58). FL_SPECTATOR (1<<26) entrou no util.h. Lembrete: mudar OBJCOPY_EXTRA no Makefile não refaz o .o; apagar o .o antes do full2.sh.

soundent.cpp feito em 2026-10-20, tudo casa. Texto 0x1EA608–0x1EAE30, .data 0x323770 (pSoundEnt = NULL), .rodata 0x375778 (vtable 0x3758E0). soundent.h agora é o do HLSDK inteiro. PS2: ISoundsInList com `int iThisSound = 0;`. GetClassPtr<CSoundEnt> forte fica em 0x18D4D8 (outro TU); a cópia weak 0x1EAD28 não tem símbolo.
