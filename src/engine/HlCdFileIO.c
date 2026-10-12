#include "common.h"
#include "engine/cmd.h"
#include "engine/hlfileio.h"

/* leitura de arquivos do CD (engine/HlCdFileIO.c): ISO9660 e pacotes .pak lidos por setor */

typedef struct
{
    char id[4];
    int dirofs;
    int dirlen;
} dpackheader_t;

extern int com_filesize;
extern void *cache_callback;
extern void *D_00399870;            /* loadcache */
extern int D_00399874;              /* loadsize */
extern void *D_0039987C;            /* loadbuf */

int func_002CF448(char *s1, char *s2);
int func_00237128(char *name, int *size, int *lba, int *sectors);
void func_002374D8(char *name);
void CD_Read(int lba, int sectors, void *buf);
int CD_SectorCount(int size);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *ptr);
void *Mem_DbgMalloc(int size, const char *file, int line);
void *Z_Malloc(int size);
void *Hunk_AllocName(int size, char *name);
void *Hunk_TempAlloc(int size);
void *Cache_Alloc(void *c, int size, char *name);
void COM_FileBase(char *in, char *out);
void Con_Printf(const char *fmt, ...);
void Sys_Error(const char *error, ...);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);

int func_002CFF88(char *name, int *size, int *lba, int *sectors)
{
    searchpath_t *search;
    pack_t *pak;
    int i;
    int found;

    *lba = 0;
    *sectors = 0;
    *size = 0;
    found = 0;

    for (search = D_00399868; search; search = search->next)
    {
        pak = search->pack;
        if (!pak)
            continue;

        for (i = 0; i < pak->numfiles; i++)
        {
            if (!func_002CF448(name, pak->files[i].name))
            {
                *size = pak->files[i].filelen;
                *lba = pak->lba + pak->files[i].filepos / 2048;
                *sectors = *size / 2048;
                found = 1;
                if (*size & 0x7FF)
                    (*sectors)++;
                break;
            }
        }

        if (found == 1)
            break;
    }

    return found;
}

void *COM_LoadFile(char *path, int usehunk, int *pLength)
{
    char base[32];
    char name[112];
    char name2[112];
    int size, lba, sectors;
    byte *buf;
    int found;
    int len;

    buf = NULL;
    if (pLength)
        *pLength = 0;

    strcpy(name, path);
    strcat(name, ";1");
    func_002374D8(name);

    found = func_00237128(name, &size, &lba, &sectors);
    if (!found)
    {
        strcpy(name2, "VALVE\\");
        strcat(name2, name);
        found = func_00237128(name2, &size, &lba, &sectors);
        if (!found)
            found = func_002CFF88(path, &size, &lba, &sectors);
    }

    if (!found)
        com_filesize = -1;
    else
        com_filesize = size;

    len = sectors << 11;
    if (found == 1)
    {
        COM_FileBase(path, base);

        switch (usehunk)
        {
        case 0:
            buf = Z_Malloc(len);
            break;
        case 1:
            buf = Hunk_AllocName(len, base);
            break;
        case 2:
            buf = Hunk_TempAlloc(len);
            break;
        case 3:
            buf = Cache_Alloc(D_00399870, len, path);
            break;
        case 4:
            if (D_00399874 < len)
                buf = Hunk_TempAlloc(len);
            else
                buf = D_0039987C;
            break;
        case 5:
            buf = Mem_DbgMalloc(len, "engine/HlCdFileIO.c", 191);
            break;
        default:
            Sys_Error("COM_LoadFile: bad usehunk");
            break;
        }

        if (!buf)
        {
            Sys_Error("COM_LoadFile: not enough space for %s", path);
        }
        else
        {
            buf[size] = 0;
            CD_Read(lba, sectors, buf);
            if (pLength)
                *pLength = size;
        }
    }

    cache_callback = NULL;
    return buf;
}

pack_t *func_002D0368(char *packfile)
{
    int size, lba, sectors;
    dpackheader_t *header;
    packfile_t *info;
    pack_t *pack;
    int numfiles;
    int dirlba;
    int count;

    if (func_00237128(packfile, &size, &lba, &sectors) == 1)
    {
        Mem_PushTag();
        Mem_SetTag(2);
        header = func_00225D48(0x800, 0x40, "engine/HlCdFileIO.c", 242);
        Mem_PopTag();
        CD_Read(lba, 1, header);

        numfiles = (unsigned)header->dirlen / sizeof(packfile_t);
        dirlba = lba + header->dirofs / 2048;
        count = CD_SectorCount(header->dirlen);

        Mem_PushTag();
        Mem_SetTag(1);
        info = Mem_DbgMalloc(count << 11, "engine/HlCdFileIO.c", 261);
        Mem_PopTag();
        CD_Read(dirlba, count, info);
        func_00225DB8(header);

        Mem_PushTag();
        Mem_SetTag(1);
        pack = Mem_DbgMalloc(sizeof(pack_t), "engine/HlCdFileIO.c", 269);
        Mem_PopTag();
        strcpy(pack->filename, packfile);
        pack->files = info;
        pack->numfiles = numfiles;
        pack->lba = lba;
        Con_Printf("Added packfile %s (%i files)\n", packfile, numfiles);
    }
    else
        pack = NULL;
    return pack;
}

int func_002D04E8(char *name)
{
    int size, lba, sectors;

    if (!func_002CFF88(name, &size, &lba, &sectors))
        size = -1;
    return size;
}
