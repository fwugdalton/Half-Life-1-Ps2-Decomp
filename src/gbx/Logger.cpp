/* Logger da Gearbox: grava logger.txt (host0:) com hora e indentação ENTER/LEAVE */

#include "common.h"
#include <stdarg.h>

extern "C" {
int vsprintf(char *buf, const char *fmt, va_list ap);
char *strcat(char *dst, const char *src);
void *memset(void *dst, int c, unsigned int n);
void *FS_Open(const char *name, const char *mode);
void FS_Close(void *f);
int func_00222500(void *f, const char *fmt, ...); /* FS_FPrintf */
unsigned long func_00229B88(void);

void func_0022ACE0(const char *fmt, ...);

int D_00328208 = 1;
void *D_0032820C = 0;
int D_00328210 = 0;
int D_00328214 = 1;
}

extern "C" void func_0022AB50(const char *fmt, ...)
{
    char msg[0x800];
    char line[0x800] = "ENTER  - ";
    va_list ap;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);
    strcat(line, msg);
    func_0022ACE0(line);
    D_00328210++;
}

extern "C" void func_0022AC18(const char *fmt, ...)
{
    char msg[0x800];
    char line[0x800] = "LEAVE  - ";
    va_list ap;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);
    strcat(line, msg);
    D_00328210--;
    func_0022ACE0(line);
}

extern "C" void func_0022ACE0(const char *fmt, ...)
{
    char msg[0x800];
    va_list ap;
    int i;

    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);
    if (D_0032820C == 0) {
        if (D_00328214 == 1) {
            D_00328214 = 0;
            D_0032820C = FS_Open("logger.txt", "w");
        } else {
            D_0032820C = FS_Open("logger.txt", "a");
        }
    }
    func_00222500(D_0032820C, "[%10d] -- ", func_00229B88());
    for (i = 0; i < D_00328210; i++) {
        func_00222500(D_0032820C, "\t");
    }
    func_00222500(D_0032820C, msg);
    func_00222500(D_0032820C, "\n");
}

extern "C" void func_0022AE18(void)
{
    FS_Close(D_0032820C);
    D_0032820C = 0;
}
