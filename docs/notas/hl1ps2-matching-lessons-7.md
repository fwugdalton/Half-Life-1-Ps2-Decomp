---
name: hl1ps2-matching-lessons-7
description: lições do som (snd_win_gbx/snd_dma): volatile shm, delay slot, if/else que casam
metadata:
  type: feedback
  modified: 2026-10-12T06:22:22.450Z
---
Continua [[hl1ps2-matching-lessons-6]].

- `extern volatile dma_t *shm;` (como no Quake): leituras shm->x ficam ANTES do jal com nop no delay slot. O ee-gcc marca o acesso com `#.set volatile`; o tools/fixasm.py (2026-10-12) agora não deixa o gas moderno mover essas instruções para o delay slot. Sintoma: `lw a1,X(v0); jal; nop` no original e `jal; lw` no nosso.
- `r = 1; if (f() != 1) r = 0;` não reproduz `daddu s1,1 ... movn s1,zero`; o que casou foi `if (!r && cond && f() == 1) r = 1;` (r já é 0 ali).
- Teste em cadeia `beq x,1 → bloco no fim; bne x,0 → sai; caso 0 inline` = `if (x != 1) { if (x == 0) {...} } else {...}`.
- `if (a < b) r = c < d ? 2 : 1; else r = 0;` com o caso 0 inline no original = escrever `if (pos <= start) r = 0; else if (pos >= end) r = 1; else r = 2;`.
- Contadores: `sfx = &lista[n]; n++; total++;` casou; `lista[n++]` mudou a ordem dos addiu.
- Stub do splat põe INCLUDE_RODATA de strings de cvar (usadas só pela .data) entre os primeiros INCLUDE_ASM: ao trocar um bloco de funções por C, copiar esses INCLUDE_RODATA para cima, senão o link falha com D_xxx indefinido.
- String usada por C e por função ainda em asm: manter INCLUDE_RODATA e usar `extern char D_xxx[];` no C.
- Ordem dos stores segue a ordem do C quase sempre: quando só os sw/swc1 trocam de lugar, testar permutações das atribuições (tb2.py com variantes geradas por script).
- Laço `for (ch = channels; ch < &channels[N]; ch++)` dá sltu e teste inicial; o original com slt e sem teste inicial era `for (i = 0; i < N; i++) ... channels[i]`.
- Tempo com `unsigned long func_00229B88()`: `unsigned int start` dá dsll32/dsra32 ao guardar e dsll32/dsrl32 na subtração. Se cada bloco usa um registrador diferente, declarar `start` dentro de cada bloco.
- `if (!s) a; else b;` vs `if (s) b; else a;` inverte beql/bnel: testar as duas formas.
- tools/difffunc.py NÃO compara os símbolos das relocações (lui/addiu de globais diferentes passam como iguais). Usar tools/romchk.py build/src/engine/X.o Func [-v] (resolve as relocações com o build/SLUS_200.66.elf e compara com a ROM original); precisa de um build completo recente. Só a ROM do commit.sh é prova final.
- tools/permute.py src Func "linhaA|linhaB|..." [max] [-a]: testa permutações de linhas contíguas (ordem de stores); -a aplica a melhor mesmo sem chegar a 0.
- tools/chkall.sh src/engine/X.c roda romchk em todas as funções em C fora de NON_MATCHING (rodar antes do commit). tools/unwrap.py src Func desfaz o wrap NON_MATCHING para testar de novo.
- Comparação de byte unsigned: `sfx[pos] >= 3` direto dá sltiu; o original com slti veio de `(x = sfx[pos]) >= 3` com uma variável int NOVA (reusar uma existente muda os registradores).
- Constante float dobrada: `((float)p / 100) * 256` vira *2.56; o original com *0.01 e depois *256 era `fpitch = (float)p / 100;` e mais tarde `fracstep = fpitch * 256;`.
- Retorno único: quando o original guarda o resultado num s-reg e tem um só `daddu v0,sN` no fim, escrever if/else com `return sc;` no fim (não returns espalhados).
