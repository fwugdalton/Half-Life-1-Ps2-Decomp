---
name: hl1ps2-matching-lessons-6
description: lições do r_studio.c: INCLUDE_RODATA duplicado, funções que retornam int, ordem de stores, confiar só na ROM
metadata:
  type: feedback
---
Continua [[hl1ps2-matching-lessons-5]].

- Quando uma função em C passa a usar uma string literal que o splat tinha deixado como INCLUDE_RODATA no .c (string compartilhada ou usada só por C), apagar essa linha INCLUDE_RODATA, senão a rodata cresce e tudo depois desloca (r_studio: +8 bytes, 10 mil bytes diferentes).
- Um "0" do difffunc logo depois de putfunc pode ser falso (função ainda vinda do .s). Só a ROM do commit.sh vale; se falhar, rodar difffunc de novo em cada função nova.
- Registrador v0 x v1 depois de uma chamada: declarar a função chamada com retorno int (tri_SpriteTexture) mudou a escolha e casou.
- Laço com `if (x) { y = i; break; }` (não `return i`) mantém o teste no topo e relê o global a cada volta, como no original.
- Stores de zero em struct: o escalonador reordena; testar permutações da ordem no fonte (func_002C3038 casou com uma ordem estranha).
- Com -ffast-math (r_studio), R_StudioEstimateInterpolant casou com `if (d > 2) return 2;` dentro do if e um `return d` compartilhado no fim (d = 1.0 no else).
- fixasm.py: cvt.w.s/sqrt.s viram .word antes do teste do nop do mtc1; fp_reads agora decodifica esses .word (senão faltava o nop do mtc1 antes de rótulo seguido de trunc). O Makefile não depende de tools/fixasm.py: dar touch no .c depois de mexer nele.
- Parâmetro ponteiro andando no laço (param no reg s* desde o prólogo): escrever `p++` no incremento do for e `*p`/`p[0]` no corpo; para o 2º ponteiro, copiar para um local (`pv = pout`) e fazer `pv++` (R_StudioSetupGlowShell).
- Ordem de declaração de arrays locais define o offset na pilha (blending antes de controller em SV_HullForStudioModel).
- Copiar a ordem de canais do SDK do HL (r,g,b lidos em 0,1,2) e as macros max/min resolveu PaletteHueReplace.
- Com -ffast-math, `x / 90` vira mul pelo recíproco; se o original tem div.s com o 90 num registro salvo, usar variável `float maxfov = 90.0f` (R_StudioCheckLOD). `(int)(x / 360) * 360.0f` quando o original faz cvt.s.w + mul.s.
- Valor gravado várias vezes sem reler (controller[0..3]): usar um int temporário novo (não um que tenha o endereço tomado, senão vai para a pilha).
- `lui sN,%hi(global)` ANTES de uma chamada cujo retorno vai para essa global = há conversão de tipo na atribuição (o gcc expande o endereço primeiro). Ex.: `gl_vendor = (char *)glGetString(...)` com glGetString retornando `const unsigned char *`; `qglX = qwglGetProcAddress(...)` com qglX ponteiro de função e retorno `void *` (gl_vidps2.c casou assim).
- Endereço global somado ao `.L` de uma chamada indireta via ponteiro de função: separar o argumento em local (`mode = Q_atoi(...); f(mode)`).
- VU0 em macro (lqc2/vmul/sqc2) = `__asm__ __volatile__` no C; cópias lq/sq de 16 bytes = `typedef int u_long128 __attribute__((mode(TI)))`.
- Strings órfãs na .rodata entre duas funções ("valve/dlls","hl.dll" no sys_ps2) = função `static __inline__` nunca chamada que usa as strings (o gcc 2.95 emite as strings mas não o código; `static` simples emite o código também).
- Store comum no fim dos dois caminhos do if (`sw zero` no delay slot do jr, executado mesmo quando o if é falso) = statement fora do if no fonte.
- Ponteiro de função chamado sem usar o retorno: se o registro do hi/lo sai trocado (v0/v1), declarar o retorno como `int` em vez de `void` (D_00399794 no sys_ps2).
- `volatile int` em global somada duas vezes seguidas sem CSE (sys_checksum no Sys_PageIn).
- unsigned long (64 bits no EE) para float chama __floatdisf (0x2DE3D0, posto no symbol_addrs).
- Tabela de switch (jtbl) sempre em endereço múltiplo de 16: se a jtbl só alinha contando a partir de outro começo de rodata, o limite de arquivo está errado (foi assim que a rede entrou no sys_ps2.c).
- NUNCA usar `git checkout <arquivo>` para desfazer teste: apaga o C não commitado. Guardar cópia no scratchpad antes.
