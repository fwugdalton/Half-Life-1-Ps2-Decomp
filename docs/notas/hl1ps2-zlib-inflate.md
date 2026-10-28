---
name: hl1ps2-zlib-inflate
description: zlib 1.1.3 inteiro em src/zlib (faixas, flags, alinhamentos, 4 funções NON_MATCHING)
metadata:
  type: project
  modified: 2026-10-12T21:53:23.682Z
---

zlib 1.1.3 inflate separado em src/zlib/ (commit 2e5bfc9, 2026-10-12):
- .text: infblock 0x222538, infcodes 0x223320, inffast 0x223BC0, inflate 0x223F98, inftrees 0x224928, infutil 0x225248; código Gbx volta em 0x2253C8 (cod/1253C8).
- .data: inftrees 0x326F40 (fixed_bl/bd/tl/td, gerados da ROM em inffixed.h), infutil 0x328048 (inflate_mask).
- .rodata: 0x385C40 infblock, 0x385D40 infcodes, 0x385DA8 inffast, 0x385DE0 inflate, 0x385EA0 inftrees (copyright + cplens...).
- Resto do zlib (commits 278e975, 654e3b9): adler32 0x217280 (-fmove-all-movables), crc32 0x219B20 (crc_table na rodata 0x3850F8), deflate 0x219D28..0x21BC78 (.data 0x325080 = my_version, rodata 0x3858F8), trees 0x232740 (rodata 0x3879D0, .data 0x328990), uncompr 0x2349D8, Gbx/Zutil.c 0x234AB0 (z_errmsg .data 0x3289D8 alinhada em 8; zcalloc = Mem_DbgMalloc(n, "Gbx/Zutil.c", 73) + memset; zcfree = Mem_Free).
- Fonte oficial 1.1.3 baixável de raw.githubusercontent.com/madler/zlib/v1.1.3/<arquivo> (o resto do github dá 403). Usado direto em adler32/crc32/deflate/trees/uncompr.
- libgcc: __umoddi3=0x2DEAE8, __muldi3=0x2DE4B0.

Pegadinhas:
- O ee-gcc liga strict aliasing no -O2; o zlib foi compilado com -fno-strict-aliasing (regra no Makefile para src/zlib/%.o). Sem isso inflate/inflate_blocks/inflate_codes não casam.
- Arquivos com tabela de salto (infblock, infcodes, inflate) precisam de .rodata alinhada em 16 (SECTALIGN_C no Makefile).
- INCLUDE_ASM depois de tabelas static const deixa o gcc achando que está na .rodata: as strings da função seguinte vão parar na .text. Correção: `__asm__(".section .rodata");` logo após o INCLUDE_ASM (ver inftrees.c).
- deflate_fast e deflate_slow também NON_MATCHING (o original iça &s->dyn_ltree e as tabelas para s*; -fmove-all-movables iça demais).
- inflate_fast e huft_build: o fonte oficial 1.1.3 (conferido em raw.githubusercontent.com/madler/zlib/v1.1.3) não casa com nenhuma flag; ficam NON_MATCHING.
- Conferir com tools/cmpfunc.py arquivo.o (lista OK/XX por função); um ck.sh deu 0 falso nesta sessão.

Ver [[hl1ps2-file-status-10]] e [[hl1ps2-compiler-flags]].
