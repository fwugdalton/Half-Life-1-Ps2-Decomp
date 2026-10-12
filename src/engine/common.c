#include "common.h"
#include <stdarg.h>
#include "engine/cmd.h"
#include "engine/server.h"

extern char D_0046DED0[];       /* serverinfo */
extern unsigned char D_00397D81[]; /* _ctype_ + 1 */
void *memset(void *, int, unsigned int);
int sprintf(char *, const char *, ...);
int vsprintf(char *, const char *, va_list);
char *strcat(char *, const char *);
char *strncpy(char *, const char *, unsigned int);
extern char D_003A9D90[128];     /* COM_BinPrintf: szReturn */
extern char D_003A9E10[1024];    /* COM_ExplainDisconnection: string */
extern char D_0053D718[256];     /* gszDisconnectReason */
extern qboolean D_0033451C;     /* gfExtendedError */
extern char D_00388B40[];       /* "%s" */
#define isupper(c) (D_00397D81[(int)(c)] & 1)
#define tolower(c) (isupper(c) ? (c) + 32 : (c))

char *Info_Serverinfo(void)
{
    return D_0046DED0;
}

void ClearLink(link_t *l)
{
    l->prev = l->next = l;
}

void RemoveLink(link_t *l)
{
    l->next->prev = l->prev;
    l->prev->next = l->next;
}

void InsertLinkBefore(link_t *l, link_t *before)
{
    l->next = before;
    l->prev = before->prev;
    l->prev->next = l;
    l->next->prev = l;
}

/* InsertLinkAfter */
void func_002415D8(link_t *l, link_t *after)
{
    l->next = after->next;
    l->prev = after;
    after->next = l;
    l->next->prev = l;
}

void Q_strcpy(char *dest, const char *src)
{
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest++ = 0;
}

void Q_strncpy(char *dest, const char *src, int count)
{
    while (*src && count--)
    {
        *dest++ = *src++;
    }
    if (count)
        *dest++ = 0;
}

int Q_strlen(const char *str)
{
    int count;

    count = 0;
    while (str[count])
        count++;

    return count;
}

/* Q_strrchr */
char *func_002416A8(char *s, char c)
{
    int len = Q_strlen(s);
    s += len;
    while (len--)
        if (*--s == c)
            return s;
    return 0;
}

void Q_strcat(char *dest, const char *src)
{
    dest += Q_strlen(dest);
    Q_strcpy(dest, src);
}

int Q_strcmp(const char *s1, const char *s2)
{
    while (1)
    {
        if (*s1 != *s2)
            return -1;
        if (!*s1)
            return 0;
        s1++;
        s2++;
    }
    return -1;
}

int Q_strncmp(const char *s1, const char *s2, int count)
{
    while (1)
    {
        if (!count--)
            return 0;
        if (*s1 != *s2)
            return -1;
        if (!*s1)
            return 0;
        s1++;
        s2++;
    }
    return -1;
}

int Q_strncasecmp(const char *s1, const char *s2, int n)
{
    int c1, c2;

    while (1)
    {
        c1 = *s1++;
        c2 = *s2++;

        if (!n--)
            return 0;

        if (c1 != c2)
        {
            if (c1 >= 'a' && c1 <= 'z')
                c1 -= ('a' - 'A');
            if (c2 >= 'a' && c2 <= 'z')
                c2 -= ('a' - 'A');
            if (c1 != c2)
                return -1;
        }
        if (!c1)
            return 0;
    }
    return -1;
}

int Q_strcasecmp(const char *s1, const char *s2)
{
    return Q_strncasecmp(s1, s2, 99999);
}

int Q_atoi(const char *str)
{
    int val;
    int sign;
    int c;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else
        sign = 1;

    val = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
        while (1)
        {
            c = *str++;
            if (c >= '0' && c <= '9')
                val = (val << 4) + c - '0';
            else if (c >= 'a' && c <= 'f')
                val = (val << 4) + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                val = (val << 4) + c - 'A' + 10;
            else
                return val * sign;
        }
    }

    if (str[0] == '\'')
    {
        return sign * str[1];
    }

    while (1)
    {
        c = *str++;
        if (c < '0' || c > '9')
            return val * sign;
        val = val * 10 + c - '0';
    }

    return 0;
}

float Q_atof(const char *str)
{
    float val;
    int sign;
    int c;
    int decimal, total;

    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else
        sign = 1;

    val = 0;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        str += 2;
        while (1)
        {
            c = *str++;
            if (c >= '0' && c <= '9')
                val = (val * 16) + c - '0';
            else if (c >= 'a' && c <= 'f')
                val = (val * 16) + c - 'a' + 10;
            else if (c >= 'A' && c <= 'F')
                val = (val * 16) + c - 'A' + 10;
            else
                return val * sign;
        }
    }

    if (str[0] == '\'')
    {
        return sign * str[1];
    }

    decimal = -1;
    total = 0;
    while (1)
    {
        c = *str++;
        if (c == '.')
        {
            decimal = total;
            continue;
        }
        if (c < '0' || c > '9')
            break;
        val = val * 10 + c - '0';
        total++;
    }

    if (decimal == -1)
        return val * sign;
    while (total > decimal)
    {
        val /= 10;
        total--;
    }

    return val * sign;
}

char *Q_strlwr(char *s)
{
    int c, l;

    while (*s)
    {
        c = *s;
        l = c + 32;
        if (!isupper(c))
            l = c;
        *s = l;
        s++;
    }
    return s;
}

/* COM_Nibble */
unsigned char func_00241BC0(char c)
{
    if ((c >= '0') && (c <= '9'))
        return (unsigned char)(c - '0');
    if ((c >= 'A') && (c <= 'F'))
        return (unsigned char)(c - 'A' + 0x0a);
    if ((c >= 'a') && (c <= 'f'))
        return (unsigned char)(c - 'a' + 0x0a);
    return '0';
}

/* COM_HexConvert */
void func_00241C18(const char *pszInput, int nInputLength, unsigned char *pOutput)
{
    unsigned char *p;
    int i;
    const char *pIn;

    p = pOutput;
    for (i = 0; i < nInputLength; i += 2)
    {
        pIn = &pszInput[i];
        *p = func_00241BC0(pIn[0]) << 4 | func_00241BC0(pIn[1]);
        p++;
    }
}

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AA8);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AB8);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AC0);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AD0);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AE0);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AE8);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388AF8);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B08);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B10);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B18);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B20);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B30);

/* COM_BinPrintf */
char *func_00241CA0(unsigned char *buf, int nLen)
{
    unsigned char c;
    char szChunk[10];
    int i;

    memset(D_003A9D90, 0, sizeof(D_003A9D90));
    for (i = 0; i < nLen; i++)
    {
        c = (unsigned char)buf[i];
        sprintf(szChunk, "%02x", c);
        strcat(D_003A9D90, szChunk);
    }
    return D_003A9D90;
}

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B40);

void COM_ExplainDisconnection(qboolean bPrint, char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    vsprintf(D_003A9E10, fmt, argptr);
    va_end(argptr);

    strncpy(D_0053D718, D_003A9E10, 255);
    D_0053D718[255] = 0;
    D_0033451C = 1;
    if (bPrint)
        Con_Printf(D_00388B40, D_0053D718);
}

/* ShortSwap */
short func_00241DE0(short l)
{
    byte b1, b2;

    b1 = l & 255;
    b2 = (l >> 8) & 255;

    return (b1 << 8) + b2;
}

/* ShortNoSwap */
short func_00241E00(short l)
{
    return l;
}

/* LongSwap */
int func_00241E10(int l)
{
    byte b1, b2, b3, b4;

    b1 = l & 255;
    b2 = (l >> 8) & 255;
    b3 = (l >> 16) & 255;
    b4 = (l >> 24) & 255;

    return ((int)b1 << 24) + ((int)b2 << 16) + ((int)b3 << 8) + b4;
}

/* LongNoSwap */
int func_00241E40(int l)
{
    return l;
}

/* FloatSwap */
float func_00241E48(float f)
{
    union
    {
        float f;
        byte b[4];
    } dat1, dat2;

    dat1.f = f;
    dat2.b[0] = dat1.b[3];
    dat2.b[1] = dat1.b[2];
    dat2.b[2] = dat1.b[1];
    dat2.b[3] = dat1.b[0];
    return dat2.f;
}

/* FloatNoSwap */
float func_00241EB8(float f)
{
    return f;
}

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteChar);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteByte);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteShort);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteWord);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteLong);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteFloat);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteString);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteBuf);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteCoord);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteAngle);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteHangle);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242150);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242320);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242508);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242800);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242840);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242880);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_StartBitWriting);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_EndBitWriting);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteBitsUChar);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteBitsUShort);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteBitsUInt);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242B58);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_WriteBitsShort);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242C28);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242C90);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242D60);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_StartBitReading);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_EndBitReading);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242EB0);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadByteBits);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00242FC8);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadBits);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243120);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243190);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243278);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadSBits);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243318);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_BeginReading);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243370);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadByte);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadShort);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243460);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadLong);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadFloat);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadBuf);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadString);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadStringLine);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadCoord);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243770);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_002437A8);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_002437E0);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243930);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243AA0);

INCLUDE_ASM("asm/nonmatchings/engine/common", MSG_ReadDeltaUsercmd);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388B48);

INCLUDE_ASM("asm/nonmatchings/engine/common", SZ_Alloc);

INCLUDE_ASM("asm/nonmatchings/engine/common", SZ_Clear);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243DC8);

INCLUDE_ASM("asm/nonmatchings/engine/common", SZ_Write);

INCLUDE_ASM("asm/nonmatchings/engine/common", SZ_Print);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243F68);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00243FA0);

INCLUDE_ASM("asm/nonmatchings/engine/common", COM_FileBase);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244138);

INCLUDE_ASM("asm/nonmatchings/engine/common", COM_Parse);

INCLUDE_ASM("asm/nonmatchings/engine/common", COM_CheckParm);

void func_00244418(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244420);

INCLUDE_ASM("asm/nonmatchings/engine/common", COM_Init);

INCLUDE_ASM("asm/nonmatchings/engine/common", va);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_002447B8);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244848);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244880);

void COM_ClearCustomizationList(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244920);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244928);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244948);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244950);

INCLUDE_ASM("asm/nonmatchings/engine/common", Info_ValueForKey);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388C40);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244AC8);

INCLUDE_ASM("asm/nonmatchings/engine/common", Info_RemovePrefixedKeys);

INCLUDE_RODATA("asm/nonmatchings/engine/common", D_00388C68);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244CF0);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244D88);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00244EE0);

INCLUDE_ASM("asm/nonmatchings/engine/common", Info_SetValueForKey);

INCLUDE_ASM("asm/nonmatchings/engine/common", Info_Print);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00245310);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00245408);

INCLUDE_ASM("asm/nonmatchings/engine/common", func_00245420);

INCLUDE_ASM("asm/nonmatchings/engine/common", PM_CalcRoll);
