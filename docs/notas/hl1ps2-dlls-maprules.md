---
name: hl1ps2-dlls-maprules
description: maprules.cpp (game_*): diferenças do PS2, conversões float->int que não casam e a macro INLINE_INCLUDE_ASM
metadata:
  type: project
---
maprules.cpp feito em 2026-10-25 (commit 75137fa): texto 0x19B2A0–0x19D2C0, .data 0x3146B0, .rodata 0x360EE8. 83/88 casam.

Diferenças do PS2: EquipPlayer testa `if (!pEntity) return;` e, para "item_battery", soma gSkillData.batteryCapacity em pev->armorvalue (min 100) em vez de GiveNamedItem; os outros itens ficam entre `g_pGameRules->Ps2Virt65(FALSE/TRUE)` (slot 65, 0x208; gamerules.h agora `Ps2Virt65(BOOL)`). UTIL_StripToken tem 2 argumentos.

Não casou (NON_MATCHING): CGameScore::Use, CGameCounter::Spawn/Use/HitLimit, EquipPlayer. No alvo a conversão float->int (trunc.w.s) sai em ordem de fonte e com outro registrador temporário (f2 em vez de f1; no HitLimit o pev fica em a1 e o load de health desce para depois do 1º mfc1). Testei ordem dos operandos, variáveis locais, flags (-fno-schedule-insns, -fmove-all-movables, -g...) sem sucesso. EquipPlayer: o alvo não junta os dois this+0x34 (um vai para a pilha).

Truques novos:
- `INLINE_INCLUDE_ASM(pasta, nome)` (include/inline_include_asm.h; o include/include_asm.h é gerado pelo splat e é sobrescrito no `make split`): dentro do corpo de uma função, o tools/fixasm.py troca a função inteira pelo .include do .s. Usei para a cópia fora de linha de uma inline (HitLimit) e para a função-chave da classe (CGameCounter::Spawn), que precisa continuar em C para o gcc emitir a vtable. O splat gera o .s porque o nome tem "INCLUDE_ASM(".
- string usada só pela função em asm ("item_battery") vem no .s do nonmatchings e cai no lugar certo do .rodata; não precisa de dummy (com dummy ela sai duas vezes).
- fixasm.py também tira a definição de objeto zerado com nome D_XXXXXXXX (array com construtor que fica no .bss do asm, ex. gTankSpread do mtank.cpp com `asm("D_003A3B88")`).
