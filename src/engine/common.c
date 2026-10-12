#include "common.h"
#include <stdarg.h>
#include "engine/pmove.h"
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
extern char D_00388B48[];       /* "" */
extern int (*LittleLong)(int l);
void *func_00243DC8(sizebuf_t *buf, int length); /* SZ_GetSpace */
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteAngle(sizebuf_t *sb, float f);
void MSG_WriteHangle(sizebuf_t *sb, float f);
void MSG_StartBitWriting(sizebuf_t *buf);
void MSG_EndBitWriting(sizebuf_t *buf);
void MSG_WriteBitsUChar(byte *data, int numbits);
void MSG_WriteBitsUShort(unsigned short *data, int numbits);
void MSG_WriteBitsShort(short *data, int numbits);
void MSG_WriteBitsUInt(unsigned int *data, int numbits);
int abs(int);
extern int protocol_version;

typedef struct
{
    int nCurOutputBit;          /* 0x0 */
    byte *pOutByte;             /* 0x4 */
    sizebuf_t *pbuf;            /* 0x8 */
} bf_write_t;

typedef struct
{
    int nMsgReadCount;          /* 0x00 */
    sizebuf_t *pbuf;            /* 0x04 */
    int nBitFieldReadStartByte; /* 0x08 */
    int nBytesRead;             /* 0x0C */
    int nCurInputBit;           /* 0x10 */
    byte *pInByte;              /* 0x14 */
} bf_read_t;

extern bf_write_t D_003AC410;   /* bfwrite */
extern bf_read_t D_003AC420;    /* bfread */
void func_00242880(int nValue);  /* MSG_WriteOneBit */
extern int msg_readcount;
extern int msg_badread;
int MSG_ReadBits(int numbits);
int MSG_ReadByteBits(int numbits);
int func_00242EB0(void);          /* MSG_ReadOneBit */
int func_00243120(int numbits);   /* MSG_ReadBitsUInt? */
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

void MSG_WriteChar(sizebuf_t *sb, int c)
{
    byte *buf;

    buf = func_00243DC8(sb, 1);
    buf[0] = c;
}

void MSG_WriteByte(sizebuf_t *sb, int c)
{
    byte *buf;

    buf = func_00243DC8(sb, 1);
    buf[0] = c;
}

void MSG_WriteShort(sizebuf_t *sb, int c)
{
    byte *buf;

    buf = func_00243DC8(sb, 2);
    buf[0] = c & 0xff;
    buf[1] = c >> 8;
}

void MSG_WriteWord(sizebuf_t *sb, int c)
{
    byte *buf;

    buf = func_00243DC8(sb, 2);
    buf[0] = c & 0xff;
    buf[1] = (c >> 8) & 0xff;
}

void MSG_WriteLong(sizebuf_t *sb, int c)
{
    byte *buf;

    buf = func_00243DC8(sb, 4);
    buf[0] = c & 0xff;
    buf[1] = (c >> 8) & 0xff;
    buf[2] = (c >> 16) & 0xff;
    buf[3] = c >> 24;
}

void MSG_WriteFloat(sizebuf_t *sb, float f)
{
    union
    {
        float f;
        int l;
    } dat;

    dat.f = f;
    dat.l = LittleLong(dat.l);

    SZ_Write(sb, &dat.l, 4);
}

void MSG_WriteString(sizebuf_t *sb, const char *s)
{
    if (!s)
        SZ_Write(sb, D_00388B48, 1);
    else
        SZ_Write(sb, s, Q_strlen(s) + 1);
}

void MSG_WriteBuf(sizebuf_t *sb, int iSize, void *buf)
{
    if (!buf)
        return;

    SZ_Write(sb, buf, iSize);
}

void MSG_WriteCoord(sizebuf_t *sb, float f)
{
    MSG_WriteShort(sb, (int)(f * 8));
}

void MSG_WriteAngle(sizebuf_t *sb, float f)
{
    MSG_WriteByte(sb, (int)(f * 256 / 360) & 255);
}

void MSG_WriteHangle(sizebuf_t *sb, float f)
{
    MSG_WriteShort(sb, (int)(f * 65536 / 360) & 65535);
}

/* MSG_WriteDeltaUsercmd */
void func_00242150(sizebuf_t *buf, usercmd_t *to, usercmd_t *from)
{
    int bits;
    int i;

    bits = 0;
    for (i = 0; i < 3; i++)
        if (to->viewangles[i] != from->viewangles[i])
            bits |= 1 << i;
    if (to->forwardmove != from->forwardmove)
        bits |= 8;
    if (to->sidemove != from->sidemove)
        bits |= 0x10;
    if (to->upmove != from->upmove)
        bits |= 0x20;
    if (to->buttons != from->buttons)
        bits |= 0x40;
    if (to->impulse != from->impulse)
        bits |= 0x80;

    MSG_WriteByte(buf, bits);

    if (bits & 1)
        MSG_WriteHangle(buf, to->viewangles[0]);
    if (bits & 2)
        MSG_WriteHangle(buf, to->viewangles[1]);
    if (bits & 4)
        MSG_WriteAngle(buf, to->viewangles[2]);
    if (bits & 8)
        MSG_WriteFloat(buf, to->forwardmove);
    if (bits & 0x10)
        MSG_WriteFloat(buf, to->sidemove);
    if (bits & 0x20)
        MSG_WriteFloat(buf, to->upmove);
    if (bits & 0x40)
        MSG_WriteShort(buf, to->buttons);
    if (bits & 0x80)
        MSG_WriteByte(buf, to->impulse);
    MSG_WriteByte(buf, to->lightlevel);
    MSG_WriteByte(buf, to->msec);
}

/* MSG_WriteDeltaUsercmd com movimentos em short */
void func_00242320(sizebuf_t *buf, usercmd_t *to, usercmd_t *from)
{
    int bits;
    int i;

    bits = 0;
    for (i = 0; i < 3; i++)
        if (to->viewangles[i] != from->viewangles[i])
            bits |= 1 << i;
    if (to->forwardmove != from->forwardmove)
        bits |= 8;
    if (to->sidemove != from->sidemove)
        bits |= 0x10;
    if (to->upmove != from->upmove)
        bits |= 0x20;
    if (to->buttons != from->buttons)
        bits |= 0x40;
    if (to->impulse != from->impulse)
        bits |= 0x80;

    MSG_WriteByte(buf, bits);

    if (bits & 1)
        MSG_WriteHangle(buf, to->viewangles[0]);
    if (bits & 2)
        MSG_WriteHangle(buf, to->viewangles[1]);
    if (bits & 4)
        MSG_WriteAngle(buf, to->viewangles[2]);
    if (bits & 8)
        MSG_WriteShort(buf, (int)to->forwardmove);
    if (bits & 0x10)
        MSG_WriteShort(buf, (int)to->sidemove);
    if (bits & 0x20)
        MSG_WriteShort(buf, (int)to->upmove);
    if (bits & 0x40)
        MSG_WriteShort(buf, to->buttons);
    if (bits & 0x80)
        MSG_WriteByte(buf, to->impulse);
    MSG_WriteByte(buf, to->lightlevel);
    MSG_WriteByte(buf, to->msec);
}

/* MSG_WriteDeltaUsercmd em bits */
void func_00242508(usercmd_t *to, usercmd_t *from)
{
    byte bits;
    unsigned short us;
    byte uc;
    short s;
    int i;

    bits = 0;
    for (i = 0; i < 3; i++)
        if (to->viewangles[i] != from->viewangles[i])
            bits |= 1 << i;
    if (to->forwardmove != from->forwardmove)
        bits |= 8;
    if (to->sidemove != from->sidemove)
        bits |= 0x10;
    if (to->upmove != from->upmove)
        bits |= 0x20;
    if (to->buttons != from->buttons)
        bits |= 0x40;
    if (to->impulse != from->impulse)
        bits |= 0x80;

    MSG_WriteBitsUChar(&bits, 8);

    if (bits & 1)
    {
        us = (int)(to->viewangles[0] * 1024 / 360) & 1023;
        MSG_WriteBitsUShort(&us, 10);
    }
    if (bits & 2)
    {
        us = (int)(to->viewangles[1] * 1024 / 360) & 1023;
        MSG_WriteBitsUShort(&us, 10);
    }
    if (bits & 4)
    {
        uc = (int)(to->viewangles[2] * 64 / 360) & 63;
        MSG_WriteBitsUChar(&uc, 6);
    }
    if (bits & 8)
    {
        s = (int)(to->forwardmove / 4);
        MSG_WriteBitsShort(&s, 8);
    }
    if (bits & 0x10)
    {
        s = (int)(to->sidemove / 4);
        MSG_WriteBitsShort(&s, 8);
    }
    if (bits & 0x20)
    {
        s = (int)(to->upmove / 4);
        MSG_WriteBitsShort(&s, 8);
    }
    if (bits & 0x40)
    {
        us = to->buttons;
        MSG_WriteBitsUShort(&us, 16);
    }
    if (bits & 0x80)
        MSG_WriteBitsUChar(&to->impulse, 8);
    uc = to->lightlevel >> 2;
    MSG_WriteBitsUChar(&uc, 6);
    uc = to->msec;
    MSG_WriteBitsUChar(&uc, 8);
}

/* MSG_WriteUsercmd */
void func_00242800(sizebuf_t *buf, usercmd_t *to, usercmd_t *from)
{
    if (protocol_version == 35)
        func_00242150(buf, to, from);
    else
        func_00242320(buf, to, from);
}

/* COM_BitOpsInit */
void func_00242840(void)
{
    memset(&D_003AC410, 0, sizeof(D_003AC410));
    memset(&D_003AC420, 0, sizeof(D_003AC420));
}

/* MSG_WriteOneBit */
void func_00242880(int nValue)
{
    int iShift;

    if (!D_003AC410.pbuf->overflowed)
    {
        if (D_003AC410.nCurOutputBit >= 8)
        {
            func_00243DC8(D_003AC410.pbuf, 1);
            D_003AC410.nCurOutputBit = 0;
            D_003AC410.pOutByte++;
        }

        if (!D_003AC410.pbuf->overflowed)
        {
            iShift = 7 - D_003AC410.nCurOutputBit;
            if (nValue)
                *D_003AC410.pOutByte |= 1 << iShift;
            else
                *D_003AC410.pOutByte &= ~(1 << iShift);

            D_003AC410.nCurOutputBit++;
        }
    }
}

void MSG_StartBitWriting(sizebuf_t *buf)
{
    D_003AC410.nCurOutputBit = 0;
    D_003AC410.pOutByte = &buf->data[buf->cursize];
    D_003AC410.pbuf = buf;
}

void MSG_EndBitWriting(sizebuf_t *buf)
{
    int i;
    int nBitsLeft;
    byte mask;

    if (!D_003AC410.pbuf->overflowed)
    {
        mask = 0xFF;
        nBitsLeft = 8 - D_003AC410.nCurOutputBit;
        for (i = 0; i < nBitsLeft; i++)
            mask &= ~(1 << i);
        *D_003AC410.pOutByte &= mask;
        func_00243DC8(D_003AC410.pbuf, 1);
        D_003AC410.nCurOutputBit = 0;
        D_003AC410.pOutByte = NULL;
        D_003AC410.pbuf = NULL;
    }
}

void MSG_WriteBitsUChar(byte *data, int numbits)
{
    while (numbits-- > 0)
        func_00242880(*data & (1 << numbits));
}

void MSG_WriteBitsUShort(unsigned short *data, int numbits)
{
    while (numbits-- > 0)
        func_00242880(*data & (1 << numbits));
}

void MSG_WriteBitsUInt(unsigned int *data, int numbits)
{
    while (numbits-- > 0)
        func_00242880(*data & (1 << numbits));
}

/* MSG_WriteBitsChar */
void func_00242B58(char *data, int numbits)
{
    byte b;

    if (*data < 0)
        func_00242880(1);
    else
        func_00242880(0);
    b = abs(*data);
    MSG_WriteBitsUChar(&b, numbits - 1);
}

void MSG_WriteBitsShort(short *data, int numbits)
{
    unsigned short s;

    if (*data < 0)
        func_00242880(1);
    else
        func_00242880(0);
    s = abs(*data);
    MSG_WriteBitsUShort(&s, numbits - 1);
}

/* MSG_WriteBitsInt */
void func_00242C28(int *data, int numbits)
{
    unsigned int i;

    if (*data < 0)
        func_00242880(1);
    else
        func_00242880(0);
    i = abs(*data);
    MSG_WriteBitsUInt(&i, numbits - 1);
}

/* MSG_WriteBitAngle */
void func_00242C90(float fAngle, int numbits)
{
    unsigned short s;
    char c;
    unsigned int u;
    unsigned int shift;
    unsigned int mask;
    int d;

    shift = 1 << numbits;
    mask = shift - 1;
    d = (int)(fAngle * shift) / 360;
    d &= mask;

    if (numbits > 16)
    {
        u = d;
        MSG_WriteBitsUInt(&u, numbits);
    }
    else if (numbits > 8)
    {
        s = d;
        MSG_WriteBitsUShort(&s, numbits);
    }
    else
    {
        c = d;
        MSG_WriteBitsUChar(&c, numbits);
    }
}

/* MSG_ReadBitAngle */
/* MSG_ReadBitAngle */
float func_00242D60(int numbits)
{
    float fReturn;
    float shift;

    shift = (float)(1 << numbits);

    if (numbits > 16)
        fReturn = func_00243120(numbits) * (360.0f / shift);
    else if (numbits > 8)
        fReturn = (short)MSG_ReadBits(numbits) * (360.0f / shift);
    else
        fReturn = (char)MSG_ReadByteBits(numbits) * (360.0f / shift);

    return fReturn;
}

void MSG_StartBitReading(sizebuf_t *buf)
{
    D_003AC420.nBitFieldReadStartByte = msg_readcount;
    D_003AC420.nCurInputBit = 0;
    D_003AC420.nBytesRead = 0;
    D_003AC420.nMsgReadCount = msg_readcount;
    D_003AC420.pInByte = &buf->data[msg_readcount];
    D_003AC420.pbuf = buf;
    D_003AC420.nMsgReadCount = msg_readcount + 1;

    if (D_003AC420.nMsgReadCount > buf->cursize)
        msg_badread = 1;
}

void MSG_EndBitReading(sizebuf_t *buf)
{
    if (D_003AC420.nMsgReadCount > buf->cursize)
        msg_badread = 1;

    D_003AC420.nBitFieldReadStartByte = 0;
    D_003AC420.nCurInputBit = 0;
    D_003AC420.nBytesRead = 0;
    D_003AC420.pInByte = NULL;
    msg_readcount = D_003AC420.nMsgReadCount;
    D_003AC420.pbuf = NULL;
}

/* MSG_ReadOneBit */
int func_00242EB0(void)
{
    int nValue;
    byte mask;
    if (msg_badread)
        return 1;

    if (D_003AC420.nCurInputBit >= 8)
    {
        D_003AC420.nMsgReadCount++;
        D_003AC420.nBytesRead++;
        D_003AC420.pInByte++;
        D_003AC420.nCurInputBit = 0;
    }

    if (D_003AC420.nMsgReadCount > D_003AC420.pbuf->cursize)
    {
        msg_badread = 1;
        return 1;
    }

    mask = 1 << (7 - D_003AC420.nCurInputBit);
    nValue = *D_003AC420.pInByte & mask;
    D_003AC420.nCurInputBit++;
    return nValue != 0;
}

int MSG_ReadByteBits(int numbits)
{
    byte result;

    result = 0;
    while (numbits-- > 0)
    {
        if (func_00242EB0())
            result |= 1 << numbits;
    }
    return result;
}

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
