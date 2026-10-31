---
name: hl1ps2-dlls-healthkit
description: healthkit.cpp (kit médico, func_healthcharger e o carregador novo do PS2 CWallHealthNew/Canister): faixas, NON_MATCHING e truques
metadata:
  type: project
---
healthkit.cpp feito em 2026-10-31 (commit 7f15f06), 36/38: texto 0x184280–0x186E08, .data 0x313468 (3 m_SaveData), .rodata 0x359CE0–0x35A850. Flag -fmove-all-movables (sem ela o FindPlayer não içava 96.0/1.0 para f20/f21).

PS2 (nomes reais vêm das strings do save):
- CHealthKit::MyTouch: em vez do deadflag, `if (!gEvilImpulse101 && (pPlayer->m_afPhysicsFlags & 0x40)) return FALSE;`.
- CWallHealth::Use e CWallHealthNew::Use: com a flag 0x40 do jogador chama TakeDamage(pev, pev, 1, DMG_GENERIC) em vez de TakeHealth.
- CWallHealthNewCanister : CBaseAnimating (item_healthchargercan, m_fDraining 0x4C, m_fSettling 0x50; modelo health_charger_both.dol, sequências still/slosh/to_rest, SetJuice = SetBoneController(0, i * 0.1f + -11)). Create(Vector, Vector) estático com vetores por valor.
- CWallHealthNew : CBaseToggle (item_healthcharger, 16 campos de 0xD0 a 0x114 + m_fRestored 0x118 fora do save, posto em 1 no Restore). Braço e olho seguem o jogador (FindPlayer a 96 unidades com traço, SetIdealYaws, bone controllers 0 e 1), SetLights manda 3 TE_ELIGHT (28) mas ninguém chama. Registro func_00186AA0: "143" a "148".

NON_MATCHING (INLINE_INCLUDE_ASM; o Spawn é a função-chave da vtable): CWallHealthNew::Spawn e Recharge. No alvo `m_iJuice = gSkillData.healthchargerCapacity` sai trunc+mfc1+sw; o nosso sai swc1 (no CWallHealth do mesmo arquivo o alvo usa swc1). Mesmo problema do islave/maprules.

Truques:
- cadeia `a = b = c = 0`: as gravações saem da direita para a esquerda; ordem do alvo 0x108,0x100,0x10C,0x104 veio de `m_flOffTime = m_flIdealEyeYaw = m_flCurrentEyeYaw = m_flIdealArmYaw = m_flCurrentArmYaw = 0`.
- o alvo relê um campo recém-zerado num registrador salvo e usa no frame: `m_flIdealArmYaw = 0; float flYaw = m_flIdealArmYaw; SetBoneController(0, flYaw); ... pev->frame = flYaw;`.
- TraceResult e vetores declarados no topo da função (antes do laço) para a pilha casar.
- hgrunt.o passou a precisar de `.rodata=16` no Makefile (a .rodata do healthkit acaba em 0x35A843).
- UTIL_TraceLine de 6 args (com ignore_glass) declarado localmente, como no hgrunt.
- os D_ que o asm NON_MATCHING usa (D_00359DD8, D_00359E90, D_00359FB0, D_00359FD0) vão no linker_script_extra.ld.

Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-done-3]].
