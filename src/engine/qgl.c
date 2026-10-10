#include "common.h"

int strcmp(const char *, const char *);
void func_0023C450(void);

void *qwglGetProcAddress(const char *name)
{
    if (!strcmp(name, "glColorTableEXT"))
        return (void *)func_0023C450;

    return NULL;
}
