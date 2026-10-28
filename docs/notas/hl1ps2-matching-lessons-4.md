---
name: hl1ps2-matching-lessons-4
description: lições do gl_rsurf.c (li.d no fixasm, a = b = expr, índices em vez de ponteiros)
metadata:
  type: project
---
Anterior: [[hl1ps2-matching-lessons-3]].
- `tools/fixasm.py` agora expande `li.d $gpr, val` como o ee-as (ori imm16 + dsll32/dsll, menor shift onde cabe), mas só quando os 48 bits de baixo do double são zero (1.0, 0.5, 256); senão (264.0, 0.6f) vai para a .rodata, emitido depois do .end da função (lights.cpp). Funções double (pow/dpmul/dpadd/dptoli) já têm nome em symbol_addrs.
- Stores que saem na ordem "campo A depois campo B": `surf->dlightbits = surf->cached_dlight = surf->dlightbits & X;` casou; `&=` + atribuição separada sempre invertia (R_BuildLightMap).
- Laço interno que anda num ponteiro de 4 em 4 com `bl += 16` hoisted: escrever `((unsigned int *)bl)[k]` e `bl++` no fim do laço externo, não um ponteiro local.
- Load e resultado no mesmo registrador (`lw a2; sra a2,a2,8`): `t = bl->r; t = t >> 8;` em duas linhas.
- Temporário de luminância num registrador próprio: usar uma variável separada (`lum`), não reaproveitar `t`.
- Condição `c.eq.s` + `bc1t` para o bloco "else": inverter o if (`if (x != 0) A else B`) até o desvio bater.
- `fabs` em gl_rsurf é macro ternária; onde o original tem `abs.s` usar `__builtin_fabsf(x)`.
- Ponteiro base somado à esquerda (`addu v0,s6,v0`): usar ponteiro temporário `r_pedge = &pedges[i]` (BuildSurfaceDisplayList).
- Strings iguais usadas por duas funções (-lm_1.. em func_002A4180/GL_BuildLightmaps): bloco `__asm__` com labels D_... + `extern char D_...[]`.
- Jump table que começa alinhada em 16 depois de código convertido: `__asm__(".section .rodata\n.align 4\n.section .text")`.
- Ordem de cálculo vs ordem das stores: gerar permutações dos statements com itertools e testar no trybody (resolveu func_002A4180, R_DecalVertsClip, BuildSurfaceDisplayList).
- Constante pi/360 do original é 0x3C0EFA36: escrever `3.141593f / 360.0f` (M_PI dá ...35); 360/pi = `360.0f / M_PI` (0x42E52EE0).
- `switch` com 2 casos que gera `sltiu x,3` extra: falta um `case` que caiu no default (R_PreDrawViewModel tinha `case mod_brush:` vazio) e o índice é unsigned.
- Mesma chamada em vários ramos (CalcFov) fundida no fim pelo cross-jump: escrever uma chamada por ramo, não variáveis + uma chamada.
- memcpy só vira lq/sq inline com protótipo `unsigned int` no tamanho (`int` impede o builtin).
- Store depois de uma chamada que aparece no delay slot do jal: no fonte o store vem antes da chamada.
- `lh x; andi 0xff; sra 8` (sem virar lhu + sll16/sra24): escrever `int code = f->x;` e usar `(code & 0xFF)` e `((short)code >> 8)` (gl_draw.c, fontes). Sem o cast o gcc troca o lh por lhu.
- Load de cvar que o scheduler sobe antes dos stores em `p->`: usar `cvar.value` (struct cvar_t extern) e não um `float` extern solto; o membro de struct cria dependência de alias (R_TracerParticles, tracerlength = D_00331B70).
- Testar permutações da ordem dos stores com itertools + trybody resolve rápido diffs só de scheduling.
- Constantes (5.0f, 0x6F, pt_*) carregadas dentro do laço no alvo = arquivo sem -fmove-all-movables; içadas para s-reg/f-reg = com a flag. Funções vizinhas com flags diferentes indicam fronteira de arquivo (r_part.c / r_part2.c em 0x2B15D0).
- Zeros extras no fim do .rodata de um arquivo = jump table do gcc (sempre `.align 4`) que deixou a seção sem múltiplo de 16: pôr `__asm__(".section .rodata\n.align 4\n.section .text");` no FIM do arquivo (o objcopy força .rodata=8, então .align no início não vale). Não é sinal de fronteira de arquivo.
