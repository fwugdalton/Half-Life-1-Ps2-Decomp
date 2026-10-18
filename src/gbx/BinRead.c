/* Gbx: leitura de valores desalinhados e printf para o console do IOP */

#include "common.h"
#include <stdarg.h>

void *memcpy(void *dst, const void *src, int n);
int vsprintf(char *buf, const char *fmt, va_list ap);
void func_002F1680(const char *fmt, ...);

int func_00236B48(void *p) {
    int v;

    memcpy(&v, p, 4);
    return v;
}

float func_00236B70(void *p) {
    float v;

    memcpy(&v, p, 4);
    return v;
}

void func_00236B98(const char *fmt, ...) {
    char buf[0x400];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);
    func_002F1680("%s", buf);
}
