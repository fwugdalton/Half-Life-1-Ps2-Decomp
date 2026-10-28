---
name: hl1ps2-dlls-triggers
description: triggers.cpp (faixas, entidades novas do PS2/Decay, NON_MATCHING) e lições (cross-jump, lb, weak copies, strings mescladas)
metadata:
  type: project
---
dlls/triggers.cpp feito em 2026-10-20 (commit 86168db), tudo casa exceto Grade e CRenderFxManager::Use (NON_MATCHING via INCLUDE_ASM).
- Faixas: texto 0x1F8190–0x200ED8 (rom 0xF8190), .data 0x323FC0 (yaml 0x223FC0), .rodata 0x379F08 (yaml 0x279F08).
- Entidades novas do PS2/Decay: trigger_playerfreeze, trigger_random, trigger_bit, trigger_bitcounter, trigger_newhurt, trigger_savecampaign, trigger_enddecay (estatísticas de nota do Decay), e mudanças em TeleportTouch, GravityTouch (cvar D_00332638), CTriggerCamera (m_ps2ViewSet/m_ps2Restored).
- Lições:
  - cmpfunc não confere alvo de jal: olhar as linhas CONFLITO do resolveundef (ex.: `(CBaseEntity *)handle` em vez de operator int).
  - laço com duas chamadas `p=FIND(NULL); while(p&&..){..; p=FIND(p);}` vira uma só (cross-jumping do gcc 2.95).
  - `lb` em vez de `lbu`: guardar o char numa variável local antes.
  - func_00228EE0 (sprintf) deve retornar int, senão mudam registradores.
  - construtor de CBasePlayer só sai fora de linha com os Vector soltos no player.h.
  - template/inline weak: a 1ª cópia na ROM leva o nome (GetClassPtr<CPointEntity>=0x160948, <CBaseDelay>=0x1EFDA0, <CBasePlayer>=0x148748); as cópias locais deste arquivo ficam sem nome e as chamadas vão para a 1ª.
  - estáticos .bss viram `extern "C"` D_ (senão `objcopy -R .bss` quebra); D_003241F0 definido depois do último IMPLEMENT_SAVERESTORE para cair no fim do .data.
  - o gcc mescla strings iguais no mesmo arquivo: com Use em asm, "env_render" é definido por `__asm__(".section .rodata ...D_0037A070...")` logo depois do INCLUDE_ASM e o LINK_ENTITY_TO_CLASS é escrito à mão com CEntityLink usando o símbolo (como no tripmine).
  - Grade: o ee-as tira o nop entre `mfc1 v0` e `mtc1 v0` no fim; regra no fixasm.py quebrava sv_user.c (lá há nop), então ficou NON_MATCHING.
  - CGameRules Ps2Virt08 (slot 8) = IsDeathmatch-like; SF_CORNER_FIREONCE = 4.
Ver [[hl1ps2-dlls]].
