#ifndef COMMON_H
#define COMMON_H

#ifndef INCLUDE_ASM_USE_MACRO_INC
#define INCLUDE_ASM_USE_MACRO_INC 1
#endif
#include "include_asm.h"

typedef unsigned char byte;
typedef int qboolean;
#define false 0
#define true 1

#ifndef NULL
#define NULL ((void *)0)
#endif

typedef struct cache_user_s
{
    void *data;
} cache_user_t;

#endif /* COMMON_H */
