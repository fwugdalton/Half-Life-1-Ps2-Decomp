---
name: hl1ps2-dlls-islave
description: weapon_islave.cpp (arma vortigaunt do Decay): faixas, NON_MATCHING com INCLUDE_ASM em C++, truque da seção descartada para rodata
metadata:
  type: project
  modified: 2026-10-20T06:03:22.817Z
---
weapon_islave.cpp feito em 2026-10-20 (commit be01c86). Texto 0x20C3A0–0x20F3C8, .data 0x324AE0, .rodata 0x381350 (+8 bytes de padding em cod/281B18). StartZap e PrimaryAttack são NON_MATCHING (INCLUDE_ASM de asm/nonmatchings/dlls/weapon_islave/), o resto casa.

Lições:
- ordem dos includes muda o .rodata: aqui gamerules.h ("Half-Life") vem ANTES de effects.h (constantes de ponteiro para membro).
- vetor espalhado: `vecAim = ISlave_SpreadVector(vecAim, cone)` (reusar a variável) casou o ZapBeam.
- dois laços seguidos que zeram ponteiros: usar variáveis distintas (i e j) em `for`, não reaproveitar i.
- StartZap: o alvo faz trunc.w.s em tempo de execução de 13.0f (vida do DLIGHT); toda variante dobra para `li 13`. PrimaryAttack: o alvo faz trunc+mfc1+sw do m_iWeaponVolume, o nosso sai swc1.
- NON_MATCHING em .cpp: as strings usadas só pela função em asm somem do .rodata; aqui todas já estavam no Precache. Os rótulos D_ que o asm referencia vão para linker_script_extra.ld.
- constante de ponteiro para membro (SetThink(&Smack)) que só a função em asm gerava: função dummy com `__attribute__((section(".islave_discard")))` no #else faz o gcc emitir a constante no lugar certo; o /DISCARD/ do linker joga o código fora.
- resolveundef com função NON_MATCHING mais curta dá lixo (deslocamento); mapear offsets por função (nm + pares nome=endereço) ignorando as não casadas.
- FindHullIntersection = 0x156B90, UTIL_TraceHull = 0x207F40, CBeam::BeamCreate 0x15ADD0, PointEntInit 0x15B038, SetAnimation 0x1C4BA0.

warpball.cpp (env_warpball, CWarpBall : CBaseEntity tamanho 0x80, também cria monstro como o monstermaker) feito em 2026-10-20 (commit df33958): texto 0x20B330, .data 0x3249A0, .rodata 0x380E90; só MakeMonster NON_MATCHING (troca v0/v1). Lições: cor setada campo a campo (pev->rendercolor.x = ...) e m_pBeams->SetColor(); string usada duas vezes num bloco veio de uma variável local (const char *psz = "...") declarada antes do if; o splat migra para o .s em nonmatchings a rodata usada só pela função (string do ALERT), então não precisa de truque; 0x207F18 é outro UTIL_TraceLine de 5 args (chamei de UTIL_TraceLine2); CLightning (effects.cpp): LightningCreate 0x15CE30, BeamUpdateVars 0x15CB80, StrikeThink 0x15BBD8; RadiusDamage global 0x14B4E0, UTIL_ScreenShake 0x207258, DispatchSpawn 0x144F58.

Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-weapons]].
