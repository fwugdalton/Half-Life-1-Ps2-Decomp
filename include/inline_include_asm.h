#ifndef INLINE_INCLUDE_ASM_H
#define INLINE_INCLUDE_ASM_H

/* Troca o corpo de uma função C/C++ pelo .s do asm, mantendo a função no fonte: o
   tools/fixasm.py acha o comentário "#INLINE_INCLUDE_ASM pasta/nome.s" dentro da função
   `nome` e põe o .include no lugar do código do gcc. Serve para a cópia fora de linha de
   uma função inline e para a função-chave de uma classe (sem ela o gcc não emite a vtable).
   O splat acha o nome por causa do "INCLUDE_ASM(" no nome da macro.
   (o include/include_asm.h é gerado pelo splat, por isso a macro fica aqui) */
#if !defined(M2CTX) && !defined(PERMUTER)
#define INLINE_INCLUDE_ASM(FOLDER, NAME) __asm__("#INLINE_INCLUDE_ASM " FOLDER "/" #NAME ".s")
#else
#define INLINE_INCLUDE_ASM(FOLDER, NAME)
#endif

#endif /* INLINE_INCLUDE_ASM_H */
