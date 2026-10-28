---
name: hl1ps2-matching-lessons
description: Lições de matching do ee-gcc 2.95 aprendidas no decomp do HL1 PS2 (padrões de C que reproduzem o asm)
metadata:
  type: project
---
- Laço independente → variável própria (`j`), senão o contador vira registrador salvo.
- `for` com `return` dentro não é rotacionado: usar `break` e testar depois (Master_AddServer).
- if/else: a ordem no fonte decide o layout (Valve escreve `if (!fp) {...} else {...}`).
- Mesma variável para valor intermediário e final (Host_Speeds: `fps = soma; if (fps<1) fps=100; else fps=1000/fps`).
- va_start depois de calls (strlen antes) evita registrador salvo.
- `int size; if (...) size=a; else size=b; return size;` (Draw_DecalSize).
- Função `__inline__` o gcc 2.95 emite no fim do arquivo: tornar normal e duplicar o corpo no chamador (Key_HistoryUp em Key_Console).
- cmpfunc mascara relocações: depois de casar, conferir a ROM inteira (ordem de stores, Key_Escape_f).
- switch com índice unsigned; ponteiro copiado para variável local antes do laço (Host_Init `pb = pal`).

- Inteiros/ponteiros inicializados com 0 vão para .data no gcc 2.95 (dá para migrar .data com `int x = 0;`). `unsigned long` é 64 bits no EE: usar `unsigned int` (RandomLong). Comparação com constante double gera li.d inválido: fazer cast `(float)`.
- fixasm: o ee-as não preenche o delay slot de um desvio quando a instrução anterior recebeu o nop do mtc1.
- Permuter (decomp-permuter, tools/permuter_setup.py) resolveu troca de registradores em ED_ReserveLeafs só mudando a ordem de um store; vale rodar em segundo plano (-j3).
- cache_user_t fica em include/common.h.
- O protótipo importa: declarar uma função externa como `void` quando ela retorna `int` muda a alocação de registradores (S_LoadStreamSound casou só com `int func_002CFF88`). Se o permuter achar score 0 com mudança no protótipo, copiar a mudança.
- Divisão por sizeof → variável `unsigned int` às vezes; testar signedness das locais.
- Teste duplicado no binário (`if (a<b) b=a;` duas vezes, S_LoadStreamChunk) vem de macro: repetir o if.
- `if (x != a) { if (cond) s=1; else s=div; } else s=1;` reproduziu o layout de S_LoadSound.
- Ordem de stores em struct segue o fonte (PF_MessageBegin_I: cursize antes de overflowed).
- `pmove` é `playermove_t pmove[]` (0x173D4 cada, um por jogador local); funções levam clientId como ÚLTIMO argumento. Layout em include/engine/pmove.h.
- pmove.c compila com -ffast-math (Makefile por arquivo): `?:` vira max.s, sqrt.s inline, `0 - x` vira neg.s.
- String compartilhada entre funções = mesmo arquivo .c (gcc 2.95 não junta strings entre arquivos).
- Static local em .bss vira extern global (senão desloca a .bss); static inicializada em .data pode ficar local.
- fixasm extra: nop do mtc1 antes de rótulo alinhado (mtc1 vira .word), sem delay slot após l.s seguido de uso do FPR, `la` vira lui/addiu.
- tools/putfunc.py src frag.c troca INCLUDE_ASM pelas funções do fragmento (e renomeia func_X pelo symbol_addrs). Depois de renomear rodar `make split`. Script de verificação da ROM: make split + touch src/engine/*.c + make + `cmp -l`.

Continua em [[hl1ps2-matching-lessons-2]]. Ver [[hl1ps2-next-steps]].
