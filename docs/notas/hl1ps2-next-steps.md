---
name: hl1ps2-next-steps
description: Receita de build/matching, pegadinhas do ee-gcc e estado do decomp do HL1 PS2 (atualizado 2026-10-10)
metadata:
  type: project
  modified: 2026-10-09T17:16:51.819Z
---

Compilador CONFIRMADO: ee-gcc 2.95.2 SCE build 274, `-O2 -G0`, via wibo. Repo na nuvem: /home/claude/hl1ps2 (some com o container); backup `git bundle` em /mnt/project-files/hl1ps2/hl1ps2.bundle (atualizar após cada commit).

Pipeline: gcc -S → tools/fixasm.py → gas moderno → objcopy. fixasm: move→daddu, .rdata→.rodata, break 0,7, cvt.w.s→.word; sem delay slot após mflo/mfhi, sdl/sdr (penúltima) ou macro uld/usd/ulw/usw; nop após mtc1/li.s se a próxima lê o mesmo par de FPR.
- fixasm (2026-10-10): mtc1/li.s seguido de rótulo cuja instrução não lê o registrador vira .word (o gas poria nop; o ee-as não).
- SEMPRE `source ../.venv/bin/activate` antes de `make split` (sem o venv o split apaga asm/ e falha).
- Strings compartilhadas entre função INCLUDE_ASM e C: o splat não leva para o .s. Emitir com `__asm__(".section .rodata ...")` no `#else` ou símbolo absoluto em linker_script_extra.ld (ver Host.c). Static local de função envolvida vira global no `#else`.
- tools: cmpfunc, difffunc, trybody (variantes), applybody, wrap/unwrap (NON_MATCHING), permuter_setup. Nunca `pkill -f` nem `kill $(ps|grep padrão)` no mesmo comando que contém o padrão (mata o shell). permuter_setup usa as flags do Makefile desde 2026-10-10 (antes ignorava -ffast-math).

Lições de matching em [[hl1ps2-matching-lessons]].

Estado por arquivo em [[hl1ps2-file-status]].

Dispositivo: decomp/ no PC do Dalton está desatualizado; entregar o bundle por attached_outputs. Ver [[hl1ps2-project-setup]].
