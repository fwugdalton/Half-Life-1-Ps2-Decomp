---
name: hl1ps2-pure-asm-done
description: as últimas funções do jogo que estavam só em asm (HL1Input, GsEnv, SmallMembank, HL1Compress) e os truques que casaram
metadata:
  type: project
  modified: 2026-11-03T20:24:08.124Z
---
Em 2026-11-03 sobravam 12 funções do Gbx sem corpo em C. Estado agora:
- HL1Input.cpp: init estático da tabela de controles (func_0021D7A8, D868, D888) casa. A tabela D_003A6200 fica no .bss em asm (um .bss em C mudaria os endereços), então o init foi escrito à mão: ctor/dtor chamados pelos nomes do gcc (`void __15KeyStateManager(KeyStateManager *)`, `_$_15KeyStateManager(p, 2)` dentro do extern "C"). Laço do destrutor com `if (D_003A6200[0]) { p = D_003A6200[0] + 8; while (p != D_003A6200[0]) {...} }` (sem variável base).
- GsEnv.c: func_00239DB8 (limpeza, como sceGsSetDefClear), func_00239F28 (registros de desenho, como sceGsSetDefDrawEnv) e func_0023A4E8 (dois buffers + tag GIF) casam. A ordem dos dois sd de cada qword decide: tools/handoff/storeorder.py faz a busca gulosa. sceGifTag do gl.h ganhou REGS0..REGS15 de 4 bits; a tag é zerada com `*(u_long128 *)&tag = 0`.
- SmallMembank.cpp: func_00228A18 e func_002286D8 em C, NON_MATCHING (3 e ~77 instruções). Truque: o tamanho total `h->size + (int)((char *)(h + 1) + h->tagSize - (char *)h)` faz o gcc somar tagSize+0x20 antes (como a ROM). O laço "Data[" só casa com goto (o for é rotacionado). O Membank.cpp (func_00227300 e func_00226FC0) ganhou as mesmas mudanças: 36→3 e 146→77.
- HL1Compress.cpp: func_0021E880 (codificador das entidades: 4 abre, 1 mudou, 2 apagado, 3 novo, 5 fim; procura no dicionário a entidade com menor custo começando em i) em C, NON_MATCHING. Tamanho igual, diferença de registradores (best no registrador, não na pilha).
- Ficam em asm (escritas à mão no original): gl.c func_0023AA18 (sync; sync.p; j) e func_0023AB00 (bc0t), TexConv.c func_00241500.
- tools/handoff/score.py: diferença por alinhamento (difflib); usar quando o cmpfunc dá centenas porque uma instrução a mais desloca tudo.
Commits bef2d5e, c67f997, fdb29c8, 1bab214. Ver [[hl1ps2-nonmatching-revisit]].
