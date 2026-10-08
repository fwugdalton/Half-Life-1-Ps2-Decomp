#ifndef STDARG_H
#define STDARG_H

/* ee-gcc (EABI, single-float): va_list e um ponteiro para a area de registradores salvos */
typedef char *va_list;

/* igual ao va-mips.h do gcc 2.95 para EABI: aponta para a area onde o prologo salvou $a0-$t3 */
#define va_start(ap, last) \
    (ap = (va_list)__builtin_next_arg(last) - \
          (__builtin_args_info(2) < 8 ? (8 - __builtin_args_info(2)) * 8 : 0))
#define va_end(ap) ((void)0)

#endif /* STDARG_H */
