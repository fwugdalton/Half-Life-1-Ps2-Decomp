---
name: hl1ps2-dlls-done-12
description: continuação da lista de .cpp das dlls feitos (bloater, bigmomma, barney, barnacle, apache, animation) e próximo arquivo (animating.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-11]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- bloater (0x13BE98, b5110dc, 17/17): sem .data, .rodata 0x33F788–0x33FBE8 (o bmodels.o ganhou `--set-section-alignment .rodata=16`). HLSDK puro com floater.dol; os 4 sons vazios são funções de 8 bytes.

- bigmomma (0x138728, 7913a6f, 55/55): .data 0x30E478, .rodata 0x33E7F0–0x33F781. Mudanças do PS2: BIG_MAXCHILDREN 7 (era 20); Touch do morteiro com RANDOM_LONG(90,110); sqrt→sqrtf; VecCheckSplatToss sem os locais vecScale/vecTemp (pilha 32 bytes menor); o 2º trace do VecCheckSplatToss e o do Touch são UTIL_TraceLineNoEnts. Registro func_0013B8D0: "027" CBMortar::Animate (entre o MortarSpray/Touch e as cópias de GetClassPtr).
- ATENÇÃO: o cmpfunc não acusa alvo de jal diferente (UTIL_TraceLine vs UTIL_TraceLineNoEnts tem o mesmo código de chamada); só o build completo pega. Quando o ROM falhar com poucos bytes, ver o jal com objdump no .elf e `od -An -tx4 -j OFF -N4 SLUS_200.66.rom` (não há xxd).

- barney (0x136CC0, 8252aee, 37/37): .data 0x30E340, .rodata 0x33DC48–0x33E7E4 (bigmomma.o ganhou .rodata=16). Precisa de DLLS_MONSTERS_CPP (HasConditions inline). Mudanças do PS2: arma por SetBodygroup(1, x) em vez de pev->body (saque e coldre usam ambos 1; Killed testa GetBodygroup(1) < 2 e põe 2); pev->effects |= EF_MUZZLEFLASH; Spawn só faz SetUse(FollowerUse) se !(spawnflags & 0x10000), senão SetUse(NULL); CheckRangeAttack1 usa UTIL_TraceLineNoEnts. Registro func_00138510: use "026" FollowerUse.

- barnacle (0x135BF0, 6bd6ecf, 17/17): .data 0x30E2E0, .rodata 0x33D698. cos/sin→cosf/sinf e M_PI float; CGib::SpawnRandomGibs tem 4 args (fBig=0). PS2: TongueTouchEnt não pega jogador com m_pActiveItem->m_iId == 16 (escrito como um `||` só). Registro func_00136B48: "024" BarnacleThink, "025" WaitTillDead.

- apache (0x131F00, 94d0df4, 33/35): .data 0x30E1E8, .rodata 0x33CB10–0x33D697. PS2: IgniteThink sem EF_LIGHT; Killed dispara m_iszTriggerTarget se m_iTriggerCondition == 4 e zera; Flight sem o cálculo de flVol. NON_MATCHING: Flight (pitch, igual osprey) e TraceAttack (trunc+swc1); D_0033CC78 no linker_script_extra.ld. Registro func_00135828 "015".."023" com friend.
- ATENÇÃO: o regex do sufixo f não pega literais sem dígito antes do ponto (`.965`): procure com `grep -nP '(?<![\w.])\.\d+(?![\w.f])'`.

- animation (0x131148, 9d7ec58, 14/16): .data 0x30DF80 (activity_map), .rodata 0x33C510–0x33CB02 (apache.o ganhou .rodata=16). Não incluir util.h (as inline põem "classname"/"targetname"/"target" na .rodata): macros do g_engfuncs locais. PS2: GetSequenceInfo usa sqrtf; função nova GetMaxAttachment (0x131CB8, nome inventado); Set/GetBodygroup com a tabela de LOD do studiohdr (0xF4 índice, 0xF8 e 0xF9 bytes; igual func_002C2B68 do r_studio), cada ramo do SetBodygroup completo, laço com `if (i == iValue) break;` e `total += 1 + p[...].unk00`. NON_MATCHING: SetController e SetBlending (agendamento depois do cvt.w.s do setting).

Próximo: animating.cpp (CBaseAnimating; Save 0x12FC80 … HandleAnimEvent 0x131140); conferir o início (func_0012FC60 e anteriores) e as faixas de .data/.rodata antes de 0x30DF80/0x33C510.
