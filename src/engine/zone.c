#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"

/* gerenciamento de memória: zona, hunk e cache (zone.c do Quake, com os relatórios do GoldSrc) */

#define ZONEID 0x1D4A11
#define MINFRAGMENT 64
#define HUNK_SENTINAL 0x1DF001ED

typedef void *FileHandle_t;

typedef struct memblock_s
{
    int size;                   /* 0x00: inclui o cabeçalho */
    int tag;                    /* 0x04: 0 = livre */
    int id;                     /* 0x08: ZONEID */
    struct memblock_s *next;    /* 0x0C */
    struct memblock_s *prev;    /* 0x10 */
    int pad;                    /* 0x14 */
} memblock_t; /* 0x18 */

typedef struct
{
    int size;                   /* 0x00 */
    memblock_t blocklist;       /* 0x04 */
    memblock_t *rover;          /* 0x1C */
} memzone_t; /* 0x20 */

typedef struct
{
    int sentinal;               /* 0x00 */
    int size;                   /* 0x04: inclui o cabeçalho */
    char name[56];              /* 0x08 */
} hunk_t; /* 0x40 */

typedef struct cache_system_s
{
    int size;                   /* 0x00: inclui o cabeçalho */
    cache_user_t *user;         /* 0x04 */
    char name[36];              /* 0x08 */
    struct cache_system_s *prev, *next;         /* 0x2C */
    struct cache_system_s *lru_prev, *lru_next; /* 0x34 */
    void (*callback)(cache_user_t *user, int reason); /* 0x3C */
} cache_system_t; /* 0x40 */

typedef struct hunkdetail_s
{
    char name[60];              /* 0x00 */
    int size;                   /* 0x3C */
    struct hunkdetail_s *next;  /* 0x40 */
} hunkdetail_t; /* 0x44 */

extern int hunk_low_used;
extern int hunk_size;
extern int hunk_high_used;
extern byte *hunk_base;
extern qboolean hunk_tempactive;
extern memzone_t *mainzone;
extern int hunk_tempmark;
extern cache_system_t cache_head;
extern qboolean cache_moving;
extern int sloppy_size;
extern byte *sloppy_ptr;
extern void (*cache_callback)(cache_user_t *user, int reason);
extern cvar_t mem_dbgfile;

/* strings compartilhadas com funções que ainda estão em asm */
extern char D_003909F8[]; /* "Hunk_Check: trahsed sentinal" */
extern char D_00390A18[]; /* "Hunk_Check: bad size" */
extern char D_00390A40[]; /* "w" */
extern char D_00390A48[];
extern char D_00390A70[];
extern char D_00390A90[];
extern char D_00390AB0[];
extern char D_00390AC8[];
extern char D_00390B00[];
extern char D_00391048[]; /* D_00391048 */
extern char D_00391058[];

void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
void qsort(void *base, int num, int width, int (*compare)(const void *, const void *));
FileHandle_t FS_Open(const char *path, const char *mode);
void FS_Close(FileHandle_t file);
void func_00222500(FileHandle_t file, const char *fmt, ...);
void Q_strncpy(char *dest, const char *src, int count);
int COM_CheckParm(char *parm);
int Q_atoi(const char *str);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
void func_00226318(const char *filename);
void func_00227DB0(const char *filename);
float fptodp(float);

void Z_CheckHeap(void);
void *Z_TagMalloc(int size, int tag);
void Hunk_FreeToHighMark(int mark);
int Hunk_HighMark(void);
void *Hunk_HighAllocName(int size, char *name);
void Hunk_SubAllocReset(void);
void Cache_FreeLow(int new_low_hunk);
void Cache_FreeHigh(int new_high_hunk);
void Cache_UnlinkLRU(cache_system_t *cs);
void Cache_MakeLRU(cache_system_t *cs);
cache_system_t *Cache_TryAlloc(int size, qboolean nobottom);
void Cache_Free(cache_user_t *c);
char *CommaPrint(int in, char *out);
void Cache_Print_Models_And_Totals(void);
void Cache_Print_Sounds_And_Totals(void);
void Hunk_Print(qboolean all);
void Cache_Print(void);
void Hunk_PrintDetails(void);

void Z_ClearZone(memzone_t *zone, int size)
{
    memblock_t *block;

    zone->blocklist.next = zone->blocklist.prev = block = (memblock_t *)((byte *)zone + sizeof(memzone_t));
    zone->blocklist.tag = 1;
    zone->blocklist.id = 0;
    zone->blocklist.size = 0;
    zone->rover = block;

    block->prev = block->next = &zone->blocklist;
    block->tag = 0;
    block->id = ZONEID;
    block->size = size - sizeof(memzone_t);
}

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_003907A0);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_003907B0);

void Z_Free(void *ptr)
{
    memblock_t *block, *other;

    if (!ptr)
        Sys_Error("Z_Free: NULL pointer");

    block = (memblock_t *)((byte *)ptr - sizeof(memblock_t));
    if (block->id != ZONEID)
        Sys_Error("Z_Free: freed a pointer without ZONEID");
    if (block->tag == 0)
        Sys_Error("Z_Free: freed a freed pointer");

    block->tag = 0;

    other = block->prev;
    if (!other->tag)
    {
        other->size += block->size;
        other->next = block->next;
        other->next->prev = other;
        if (block == mainzone->rover)
            mainzone->rover = other;
        block = other;
    }

    other = block->next;
    if (!other->tag)
    {
        block->size += other->size;
        block->next = other->next;
        block->next->prev = block;
        if (other == mainzone->rover)
            mainzone->rover = block;
    }
}

void *Z_Malloc(int size)
{
    void *buf;

    Z_CheckHeap();
    buf = Z_TagMalloc(size, 1);
    if (!buf)
        Sys_Error("Z_Malloc: failed on allocation of %i bytes", size);
    memset(buf, 0, size);

    return buf;
}

void *Z_TagMalloc(int size, int tag)
{
    int extra;
    memblock_t *start, *rover, *new, *base;

    if (!tag)
        Sys_Error("Z_TagMalloc: tried to use a 0 tag");

    size += sizeof(memblock_t);
    size += 4;
    size = (size + 7) & ~7;

    base = rover = mainzone->rover;
    start = base->prev;

    do
    {
        if (rover == start)
            return NULL;
        if (rover->tag)
            base = rover = rover->next;
        else
            rover = rover->next;
    } while (base->tag || base->size < size);

    extra = base->size - size;
    if (extra > MINFRAGMENT)
    {
        new = (memblock_t *)((byte *)base + size);
        new->size = extra;
        new->tag = 0;
        new->prev = base;
        new->id = ZONEID;
        new->next = base->next;
        new->next->prev = new;
        base->next = new;
        base->size = size;
    }

    base->tag = tag;
    mainzone->rover = base->next;
    base->id = ZONEID;
    *(int *)((byte *)base + base->size - 4) = ZONEID;

    return (void *)((byte *)base + sizeof(memblock_t));
}

void Z_Print(memzone_t *zone)
{
    memblock_t *block;

    Con_Printf("zone size: %i  location: %p\n", mainzone->size, mainzone);

    for (block = zone->blocklist.next;; block = block->next)
    {
        Con_Printf("block:%p    size:%7i    tag:%3i\n", block, block->size, block->tag);

        if (block->next == &zone->blocklist)
            break;
        if ((byte *)block + block->size != (byte *)block->next)
            Con_Printf("ERROR: block size does not touch the next block\n");
        if (block->next->prev != block)
            Con_Printf("ERROR: next block doesn't have proper back link\n");
        if (!block->tag && !block->next->tag)
            Con_Printf("ERROR: two consecutive free blocks\n");
    }
}

void Z_CheckHeap(void)
{
    memblock_t *block;

    for (block = mainzone->blocklist.next;; block = block->next)
    {
        if (block->next == &mainzone->blocklist)
            break;
        if ((byte *)block + block->size != (byte *)block->next)
            Sys_Error("Z_CheckHeap: block size does not touch the next block\n");
        if (block->next->prev != block)
            Sys_Error("Z_CheckHeap: next block doesn't have proper back link\n");
        if (!block->tag && !block->next->tag)
            Sys_Error("Z_CheckHeap: two consecutive free blocks\n");
    }
}

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_003909F8);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390A18);

void Hunk_Check(void)
{
    hunk_t *h;

    for (h = (hunk_t *)hunk_base; (byte *)h != hunk_base + hunk_low_used;)
    {
        if (h->sentinal != HUNK_SENTINAL)
            Sys_Error(D_003909F8);
        if (h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
            Sys_Error(D_00390A18);
        h = (hunk_t *)((byte *)h + h->size);
    }
}

#ifdef NON_MATCHING
void Hunk_Print(qboolean all)
{
    hunk_t *h, *next, *endlow, *starthigh, *endhigh;
    int count, sum;
    int totalblocks;
    char name[57];
    char buf[64];
    FileHandle_t file;

    count = 0;
    sum = 0;
    totalblocks = 0;
    name[56] = 0;

    file = FS_Open("HunkReport.txt", D_00390A40);
    if (!file)
        return;

    h = (hunk_t *)hunk_base;
    endlow = (hunk_t *)(hunk_base + hunk_low_used);
    starthigh = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);
    endhigh = (hunk_t *)(hunk_base + hunk_size);

    func_00222500(file, D_00390A48, CommaPrint(hunk_size, buf));
    func_00222500(file, D_00390A70);

    while (1)
    {
        if (h == endlow)
        {
            func_00222500(file, D_00390A70);
            func_00222500(file, D_00390A90, CommaPrint(hunk_size - hunk_low_used - hunk_high_used, buf));
            func_00222500(file, D_00390A70);
            h = starthigh;
        }

        if (h == endhigh)
            break;

        if (h->sentinal != HUNK_SENTINAL)
            Sys_Error(D_003909F8);
        if (h->size < 16 || h->size + (byte *)h - hunk_base > hunk_size)
            Sys_Error(D_00390A18);

        next = (hunk_t *)((byte *)h + h->size);
        count++;
        totalblocks++;
        sum += h->size;

        memcpy(name, h->name, 56);

        if (all)
            func_00222500(file, D_00390AB0, h, CommaPrint(h->size, buf), name);

        if (next == endlow || next == endhigh || strncmp(h->name, next->name, 56))
        {
            if (!all)
                func_00222500(file, D_00390AC8, CommaPrint(sum, buf), name, count);
            count = 0;
            sum = 0;
        }

        h = next;
    }

    func_00222500(file, D_00390A70);
    func_00222500(file, D_00390B00, totalblocks);
    FS_Close(file);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/zone", Hunk_Print);
#endif

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390A40);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390A48);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390A70);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390A90);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390AB0);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390AC8);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00390B00);

void *Hunk_AllocName(int size, char *name)
{
    hunk_t *h;

    if (size < 0)
        Sys_Error("Hunk_Alloc: bad size: %i", size);

    size = sizeof(hunk_t) + ((size + 63) & ~63);

    if (hunk_size - hunk_low_used - hunk_high_used < size)
        Sys_Error("Hunk_Alloc: failed on %i bytes", size);

    h = (hunk_t *)(hunk_base + hunk_low_used);
    hunk_low_used += size;

    Cache_FreeLow(hunk_low_used);

    memset(h, 0, size);
    h->size = size;
    h->sentinal = HUNK_SENTINAL;
    Q_strncpy(h->name, name, 56);

    return (void *)(h + 1);
}

void *Hunk_Alloc(int size)
{
    return Hunk_AllocName(size, "unknown");
}

int Hunk_LowMark(void)
{
    Hunk_SubAllocReset();
    return hunk_low_used;
}

void Hunk_FreeToLowMark(int mark)
{
    if (mark < 0 || mark > hunk_low_used)
        Sys_Error("Hunk_FreeToLowMark: bad mark %i", mark);
    Hunk_SubAllocReset();
    hunk_low_used = mark;
}

int Hunk_HighMark(void)
{
    if (hunk_tempactive)
    {
        hunk_tempactive = false;
        Hunk_FreeToHighMark(hunk_tempmark);
    }

    return hunk_high_used;
}

void Hunk_FreeToHighMark(int mark)
{
    if (hunk_tempactive)
    {
        hunk_tempactive = false;
        Hunk_FreeToHighMark(hunk_tempmark);
    }
    if (mark < 0 || mark > hunk_high_used)
        Sys_Error("Hunk_FreeToHighMark: bad mark %i", mark);
    hunk_high_used = mark;
}

void *Hunk_HighAllocName(int size, char *name)
{
    hunk_t *h;

    if (size < 0)
        Sys_Error("Hunk_HighAllocName: bad size: %i", size);

    if (hunk_tempactive)
    {
        Hunk_FreeToHighMark(hunk_tempmark);
        hunk_tempactive = false;
    }

    size = sizeof(hunk_t) + ((size + 63) & ~63);

    if (hunk_size - hunk_low_used - hunk_high_used < size)
    {
        Con_Printf("Hunk_HighAlloc: failed on %i bytes\r\n", size);
        return NULL;
    }

    hunk_high_used += size;
    Cache_FreeHigh(hunk_high_used);

    h = (hunk_t *)(hunk_base + hunk_size - hunk_high_used);

    memset(h, 0, size);
    h->size = size;
    h->sentinal = HUNK_SENTINAL;
    Q_strncpy(h->name, name, 56);

    return (void *)(h + 1);
}

void *Hunk_TempAlloc(int size)
{
    void *buf;

    size = (size + 63) & ~63;

    if (hunk_tempactive)
    {
        Hunk_FreeToHighMark(hunk_tempmark);
        hunk_tempactive = false;
    }

    hunk_tempmark = Hunk_HighMark();
    buf = Hunk_HighAllocName(size, "temp");
    hunk_tempactive = true;

    return buf;
}

void *Hunk_SubAllocName(int size)
{
    void *buf;

    if (size > sloppy_size || !sloppy_ptr)
    {
        sloppy_size = 0x1000;
        if (size > sloppy_size)
            sloppy_size = size;
        sloppy_ptr = Hunk_AllocName(sloppy_size, "sloppy");
    }

    buf = sloppy_ptr;
    sloppy_ptr += size;
    sloppy_size -= size;

    return buf;
}

void Hunk_SubAllocReset(void)
{
    sloppy_ptr = NULL;
    sloppy_size = 0;
}

void Cache_Move(cache_system_t *c)
{
    cache_system_t *new;

    cache_moving = true;

    new = Cache_TryAlloc(c->size, true);
    if (new)
    {
        if (c->callback)
            c->callback(c->user, 0);

        memcpy(new + 1, c + 1, c->size - sizeof(cache_system_t));
        new->user = c->user;
        memcpy(new->name, c->name, sizeof(new->name));
        Cache_Free(c->user);
        new->callback = c->callback;
        c->callback = NULL;
        new->user->data = (void *)(new + 1);

        if (new->callback)
            new->callback(new->user, 1);
    }
    else
    {
        Cache_Free(c->user);
    }

    cache_moving = false;
}

void Cache_FreeLow(int new_low_hunk)
{
    cache_system_t *c;

    while (1)
    {
        c = cache_head.next;
        if (c == &cache_head)
            return;
        if ((byte *)c >= hunk_base + new_low_hunk)
            return;
        Cache_Move(c);
    }
}

void Cache_FreeHigh(int new_high_hunk)
{
    cache_system_t *c, *prev;

    prev = NULL;
    while (1)
    {
        c = cache_head.prev;
        if (c == &cache_head)
            return;
        if ((byte *)c + c->size <= hunk_base + hunk_size - new_high_hunk)
            return;
        if (c == prev)
        {
            Cache_Free(c->user);
        }
        else
        {
            Cache_Move(c);
            prev = c;
        }
    }
}

void Cache_UnlinkLRU(cache_system_t *cs)
{
    if (!cs->lru_next || !cs->lru_prev)
        Sys_Error("Cache_UnlinkLRU: NULL link");

    cs->lru_next->lru_prev = cs->lru_prev;
    cs->lru_prev->lru_next = cs->lru_next;

    cs->lru_prev = cs->lru_next = NULL;
}

void Cache_MakeLRU(cache_system_t *cs)
{
    if (cs->lru_next || cs->lru_prev)
        Sys_Error("Cache_MakeLRU: active link");

    cache_head.lru_next->lru_prev = cs;
    cs->lru_next = cache_head.lru_next;
    cs->lru_prev = &cache_head;
    cache_head.lru_next = cs;
}

cache_system_t *Cache_TryAlloc(int size, qboolean nobottom)
{
    cache_system_t *cs, *new;

    if (!nobottom && cache_head.prev == &cache_head)
    {
        if (hunk_size - hunk_high_used - hunk_low_used < size)
            Sys_Error("Cache_TryAlloc: %i is greater then free hunk", size);

        new = (cache_system_t *)(hunk_base + hunk_low_used);
        memset(new, 0, sizeof(*new));
        new->size = size;

        cache_head.prev = cache_head.next = new;
        new->prev = new->next = &cache_head;

        Cache_MakeLRU(new);
        return new;
    }

    new = (cache_system_t *)(hunk_base + hunk_low_used);
    cs = cache_head.next;

    do
    {
        if (!nobottom || cs != cache_head.next)
        {
            if ((byte *)cs - (byte *)new >= size)
            {
                memset(new, 0, sizeof(*new));
                new->size = size;

                new->next = cs;
                new->prev = cs->prev;
                cs->prev->next = new;
                cs->prev = new;

                Cache_MakeLRU(new);
                return new;
            }
        }

        new = (cache_system_t *)((byte *)cs + cs->size);
        cs = cs->next;
    } while (cs != &cache_head);

    if (hunk_base + hunk_size - hunk_high_used - (byte *)new >= size)
    {
        memset(new, 0, sizeof(*new));
        new->size = size;

        new->next = &cache_head;
        new->prev = cache_head.prev;
        cache_head.prev->next = new;
        cache_head.prev = new;

        Cache_MakeLRU(new);
        return new;
    }

    return NULL;
}

void Cache_Flush(void)
{
    while (cache_head.next != &cache_head)
        Cache_Free(cache_head.next->user);
}

int CacheSystemCompare(const void *ppcs1, const void *ppcs2)
{
    cache_system_t *pcs1 = *(cache_system_t **)ppcs1;
    cache_system_t *pcs2 = *(cache_system_t **)ppcs2;

    return strcasecmp(pcs1->name, pcs2->name);
}

#ifdef NON_MATCHING
void Cache_Print(void)
{
    cache_system_t *cd;
    cache_system_t *SortList[512];
    int i, j;
    char buf[64];
    FileHandle_t file;

    i = 0;

    file = FS_Open("HLCache Report.txt", D_00390A40);
    if (!file)
        return;

    memset(SortList, sizeof(SortList), 0);

    func_00222500(file, "\r\nCACHE:\r\n");

    for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
        SortList[i++] = cd;

    qsort(SortList, i, sizeof(cache_system_t *), CacheSystemCompare);

    for (j = 0; j < i; j++)
        func_00222500(file, "%16.16s : %-16s\r\n", CommaPrint(SortList[j]->size, buf), SortList[j]->name);

    FS_Close(file);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/zone", Cache_Print);
#endif

qboolean ComparePath1(char *path1, char *path2)
{
    while (*path1 != '/' && *path1 != '\\' && *path1)
    {
        if (*path1 != *path2)
            return false;
        path1++;
        path2++;
    }

    return true;
}

char *CommaPrint(int in, char *out)
{
    char outbuf[64];
    char temp[64];
    int len, i;

    memset(outbuf, 0, 50);

    while (in)
    {
        int val = in % 1000;
        in = in / 1000;
        strcpy(temp, outbuf);
        sprintf(outbuf, ",%03i%s", val, temp);
    }

    len = strlen(outbuf);
    for (i = 0; i < len; i++)
    {
        if (outbuf[i] != '0' && outbuf[i] != ',')
            break;
    }

    if (i == len)
        strcpy(out, "0");
    else
        strcpy(out, &outbuf[i]);

    return out;
}

void Cache_Report(void)
{
    Con_DPrintf("%4.1f megabyte data cache\r\n", (hunk_size - hunk_high_used - hunk_low_used) / (float)(1024 * 1024));
}

void Cache_Compact(void)
{
}

void Cache_Init(void)
{
    cache_head.next = cache_head.prev = &cache_head;
    cache_head.lru_next = cache_head.lru_prev = &cache_head;
}

void Cache_Free(cache_user_t *c)
{
    cache_system_t *cs;

    if (!c->data)
        Sys_Error("Cache_Free: not allocated");

    cs = ((cache_system_t *)c->data) - 1;

    if (!cache_moving && cs->callback)
    {
        cs->callback(cs->user, 2);
        cs->callback = NULL;
    }

    cs->prev->next = cs->next;
    cs->next->prev = cs->prev;
    cs->next = cs->prev = NULL;

    c->data = NULL;

    Cache_UnlinkLRU(cs);
}

int Cache_TotalUsed(void)
{
    cache_system_t *cd;
    int Total = 0;

    for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
        Total += cd->size;

    return Total;
}

void *Cache_Check(cache_user_t *c)
{
    cache_system_t *cs;

    if (!c->data)
        return NULL;

    cs = ((cache_system_t *)c->data) - 1;

    Cache_UnlinkLRU(cs);
    Cache_MakeLRU(cs);

    return c->data;
}

void *Cache_Alloc(cache_user_t *c, int size, char *name)
{
    cache_system_t *cs;

    if (c->data)
        Sys_Error("Cache_Alloc: already allocated");

    if (size <= 0)
        Sys_Error("Cache_Alloc: size %i", size);

    size = (size + sizeof(cache_system_t) + 15) & ~15;

    while (1)
    {
        cs = Cache_TryAlloc(size, false);
        if (cs)
        {
            strncpy(cs->name, name, sizeof(cs->name) - 1);
            c->data = (void *)(cs + 1);
            cs->callback = cache_callback;
            cs->user = c;
            break;
        }

        if (cache_head.lru_prev == &cache_head)
            Sys_Error("Cache_Alloc: out of memory");

        Cache_Free(cache_head.lru_prev->user);
    }

    return Cache_Check(c);
}

#ifdef NON_MATCHING
void Hunk_PrintDetails(void)
{
    hunk_t *h, *endlow, *starthigh, *endhigh;
    hunkdetail_t *list, *p, *next;
    char buf[64];
    FileHandle_t file;

    list = NULL;

    file = FS_Open(mem_dbgfile.string, D_00390A40);

    h = (hunk_t *)hunk_base;
    endhigh = (hunk_t *)(hunk_base + hunk_size);
    endlow = (hunk_t *)(hunk_base + hunk_low_used);
    starthigh = (hunk_t *)((byte *)endhigh - hunk_high_used);

    if (!file)
        return;

    func_00222500(file, "\r\n\r\nHunk:\r\n\r\n");

    if (h == endlow)
        h = starthigh;

    while (h != endhigh)
    {
        for (p = list; p; p = p->next)
        {
            if (!strncmp(p->name, h->name, 56))
                break;
        }

        if (p)
        {
            p->size += h->size;
        }
        else
        {
            p = Mem_DbgMalloc(sizeof(hunkdetail_t), "engine/Zone.c", 1209);
            strncpy(p->name, h->name, 56);
            p->name[56] = 0;
            p->size = h->size;
            p->next = list;
            list = p;
        }

        h = (hunk_t *)((byte *)h + h->size);
        if (h == endlow)
            h = starthigh;
    }

    if (list)
    {
        p = list;
        do
        {
            next = p;
            func_00222500(file, "%16.16s : %16s \r\n", CommaPrint(p->size, buf), p->name);
            p = p->next;
            Mem_Free(next);
        } while (p);
    }

    FS_Close(file);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/zone", Hunk_PrintDetails);
#endif

void Cache_Print_Details(void)
{
    Con_Printf("\n\nCache:\n\n");
    Cache_Print();
    Cache_Print_Sounds_And_Totals();
    Cache_Print_Models_And_Totals();
    Cache_Report();
}

void Zone_Print_f(void)
{
    Con_Printf("\n\nZone:\n\n");
    Z_Print(mainzone);
}

void Mem_Summary(void)
{
    int cachesize;
    char buf[64];
    FileHandle_t file;

    file = FS_Open("MemSummary.txt", D_00390A40);
    cachesize = Cache_TotalUsed();
    if (!file)
        return;

    func_00222500(file, "MEMORY SUMMARY:\r\n------------------------------------\r\n");
    func_00222500(file, "\tTotal memory available:    %s\r\n", CommaPrint(hunk_size, buf));
    func_00222500(file, "\tTotal used in low hunk:    %s\r\n", CommaPrint(hunk_low_used, buf));
    func_00222500(file, "\tTotal used in high hunk:   %s\r\n", CommaPrint(hunk_high_used, buf));
    func_00222500(file, "\tTotal cache space:         %s\r\n", CommaPrint(hunk_size - hunk_low_used - hunk_high_used, buf));
    func_00222500(file, "\tTotal cache used:          %s\r\n", CommaPrint(cachesize, buf));
    func_00222500(file, "\tTotal memory available:    %s\r\n", CommaPrint(hunk_size - hunk_low_used - hunk_high_used - cachesize, buf));
    func_00222500(file, "------------------------------------\r\n\r\n");
    FS_Close(file);
}

void Hunk_Print_f(void)
{
    Hunk_Print(false);
}

void Hunk_PrintAll_f(void)
{
    Hunk_Print(true);
}

void Mem_PrintAll_f(void)
{
    FileHandle_t file;

    func_00226318("Membank Report.txt");
    func_00227DB0("Misc Membank Report.txt");
    Mem_Summary();

    file = FS_Open(mem_dbgfile.string, D_00390A40);
    if (!file)
        return;

    func_00222500(file, "DETAILS:\r\n------------------------------------\r\n");
    FS_Close(file);

    Hunk_Print(false);
    Hunk_PrintDetails();
    Cache_Print_Details();
}

void Cache_PrintSounds_f(void)
{
    Cache_Print_Sounds_And_Totals();
}

void Cache_PrintModels_f(void)
{
    Cache_Print_Models_And_Totals();
}

void Memory_Init(void *buf, int size)
{
    int p;
    int zonesize = 0x20000;

    hunk_base = buf;
    hunk_size = size;
    hunk_low_used = 0;
    hunk_high_used = 0;

    Cache_Init();

    p = COM_CheckParm("-zone");
    if (p)
    {
        if (p < com_argc - 1)
            zonesize = Q_atoi(com_argv[p + 1]) * 1024;
        else
            Sys_Error("Memory_Init: you must specify a size in KB after -zone");
    }

    mainzone = Hunk_AllocName(zonesize, "zone");
    Z_ClearZone(mainzone, zonesize);

    Cmd_AddCommand("flush", Cache_Flush);
    Cmd_AddCommand("memhunk", Hunk_PrintDetails);
    Cmd_AddCommand("memcache", Cache_Print_Details);
    Cmd_AddCommand("memzone", Zone_Print_f);
    Cmd_AddCommand("memprint", Hunk_Print_f);
    Cmd_AddCommand("memprintall", Hunk_PrintAll_f);
    Cmd_AddCommand("memall", Mem_PrintAll_f);
    Cmd_AddCommand("memsound", Cache_PrintSounds_f);
    Cmd_AddCommand("memmodels", Cache_PrintModels_f);
    Cvar_RegisterVariable(&mem_dbgfile);
}

void Cache_Print_Models_And_Totals(void)
{
    char buf[64];
    cache_system_t *cd;
    cache_system_t *SortList[512];
    int i, j;
    int totalbytes;
    FileHandle_t file;

    i = 0;
    totalbytes = 0;

    file = FS_Open("ModelCache.txt", D_00390A40);
    if (!file)
        return;

    memset(SortList, sizeof(SortList), 0);

    for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
    {
        if (strstr(cd->name, ".dol"))
            SortList[i++] = cd;
    }

    qsort(SortList, i, sizeof(cache_system_t *), CacheSystemCompare);

    func_00222500(file, "\r\nCACHED MODELS:\r\n");

    for (j = 0; j < i; j++)
    {
        func_00222500(file, D_00391048, CommaPrint(SortList[j]->size, buf), SortList[j]->name);
        totalbytes += SortList[j]->size;
    }

    func_00222500(file, D_00391058, CommaPrint(totalbytes, buf));
    FS_Close(file);
}

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00391048);

INCLUDE_RODATA("asm/nonmatchings/engine/zone", D_00391058);

#ifdef NON_MATCHING
void Cache_Print_Sounds_And_Totals(void)
{
    char buf[64];
    cache_system_t *cd;
    cache_system_t *SortList[512];
    int i, j;
    int totalsndbytes, subtotal;
    FileHandle_t file;

    i = 0;
    totalsndbytes = 0;
    subtotal = 0;

    file = FS_Open("SoundCache.txt", D_00390A40);
    if (!file)
        return;

    memset(SortList, sizeof(SortList), 0);

    for (cd = cache_head.next; cd != &cache_head; cd = cd->next)
    {
        if (strstr(cd->name, ".wav"))
            SortList[i++] = cd;
    }

    qsort(SortList, i, sizeof(cache_system_t *), CacheSystemCompare);

    if (!file)
        return;

    func_00222500(file, "\r\nCACHED SOUNDS:\r\n");

    for (j = 0; j < i; j++)
    {
        func_00222500(file, D_00391048, CommaPrint(SortList[j]->size, buf), SortList[j]->name);
        totalsndbytes += SortList[j]->size;

        if (j + 1 == i || !ComparePath1(SortList[j]->name, SortList[j + 1]->name))
        {
            func_00222500(file, "\tTotal Bytes used: %s\r\n", CommaPrint(totalsndbytes - subtotal, buf));
            subtotal = totalsndbytes;
        }
    }

    func_00222500(file, "Total bytes in cache used by sound:  %s\r\n", CommaPrint(totalsndbytes, buf));
    FS_Close(file);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/zone", Cache_Print_Sounds_And_Totals);
#endif
