/* stubs do sistema de arquivos */

#include "common.h"
#include <stdarg.h>

void *FS_Open(const char *path, const char *mode)
{
    return (void *)0xDEADBEEF;
}

void FS_Close(void *f)
{
}

int func_002224D0(void)
{
    return 0;
}

int func_002224D8(void)
{
    return 0;
}

int func_002224E0(void)
{
    return 1;
}

int func_002224E8(void)
{
    return 0;
}

int func_002224F0(void)
{
    return 0;
}

int func_002224F8(void)
{
    return 0;
}

int func_00222500(void *f, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    va_end(ap);
    return 0;
}
