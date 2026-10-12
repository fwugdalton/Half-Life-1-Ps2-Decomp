#include "common.h"
#include "engine/cmd.h"
#include "engine/hlfileio.h"

/* sistema de arquivos do HL1 PS2 (engine/HlFileIO.c): caminhos de busca, pacotes e diretórios */


typedef struct
{
    char *basedir;
    char *cachedir;
} quakeparms_t;

extern quakeparms_t host_parms;
extern char com_gamedir[];
extern char D_00555F00[];           /* com_cachedir */
extern void *cache_callback;
extern void *D_00399870;            /* loadcache */
extern int D_00399874;              /* loadsize */
extern void *D_0039987C;            /* loadbuf */
extern char D_00465A78[8];          /* exten de COM_FileExtension */
extern unsigned char D_00397D81[];  /* tabela de ctype */

#define isupper(c) (D_00397D81[(int)(c)] & 1)
#define tolower(c) (isupper(c) ? (c) + 32 : (c))

int COM_CheckParm(char *parm);
char *Info_Serverinfo(void);
void func_00244EE0(char *s, char *key, char *value, int maxsize);
void Con_Printf(const char *fmt, ...);
void Sys_Error(const char *error, ...);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
int strlen(const char *);
int strcmp(const char *, const char *);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, int);
int sprintf(char *, const char *, ...);
void *COM_LoadFile(char *path, int usehunk, int *pLength);
pack_t *func_002D0368(char *packfile);
void func_002CFDD0(searchpath_t *search);
void func_002CFE28(int bIsBase, char *pszBaseDir, char *pszDir);
char *func_002CFD30(char *in);

void COM_FixSlashes(char *pname)
{
    while (*pname)
    {
        if (*pname == '\\')
            *pname = '/';
        pname++;
    }
}

#ifdef NON_MATCHING
int func_002CF448(char *s1, char *s2)
{
    int c1; char c2;

    do
    {
        c1 = *s1;
        c2 = *s2;

        if (!((c1 == '/' && c2 == '\\') || (c1 == '\\' && c2 == '/')))
        {
            if (tolower(c1) != tolower(c2))
                return -1;
        }
    } while (*s1++ && *s2++);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/HlFileIO", func_002CF448);
#endif

void func_002CF500(void)
{
    searchpath_t *s;

    Con_Printf("Current search path:\n");
    for (s = D_00399868; s; s = s->next)
    {
        if (s->bIsBase)
            Con_Printf("(d) ");

        if (s->pack)
            Con_Printf("%s (%i files)\n", s->pack->filename, s->pack->numfiles);
        else
            Con_Printf("%s\n", s->filename);
    }
}

byte *COM_LoadHunkFile(char *path)
{
    return COM_LoadFile(path, 1, NULL);
}

void *COM_LoadFileForMe(char *path, int *pLength)
{
    return COM_LoadFile(path, 2, pLength);
}

void *COM_LoadFileCached(char *path, void *cu, void *callback)
{
    D_00399870 = cu;
    cache_callback = callback;
    return COM_LoadFile(path, 3, NULL);
}

void *COM_LoadStackFile(char *path, void *buffer, int bufsize)
{
    D_0039987C = buffer;
    D_00399874 = bufsize;
    return COM_LoadFile(path, 4, NULL);
}

void func_002CF640(void)
{
    searchpath_t *s, *next;

    for (s = D_00399868; s; s = next)
    {
        next = s->next;
        func_002CFDD0(s);
    }
    D_00399868 = NULL;
}

void func_002CF690(void)
{
    searchpath_t *s, *next;

    for (s = D_00399868; s; s = next)
    {
        next = s->next;
        func_002CFDD0(s);
    }
    D_00399868 = NULL;

    func_002CFE28(1, "", "VALVE");
}

static __inline__ void func_RemoveNonBase(void)
{
    searchpath_t *s, *next, *list;

    list = NULL;
    for (s = D_00399868; s; s = next)
    {
        next = s->next;
        if (s->bIsBase)
        {
            s->next = list;
            list = s;
        }
        else
            func_002CFDD0(s);
    }

    D_00399868 = NULL;
    for (s = list; s; s = next)
    {
        next = s->next;
        s->next = D_00399868;
        D_00399868 = s;
    }
}

void func_002CF6F8(char *pszDir)
{

    func_RemoveNonBase();

    func_002CFE28(0, host_parms.basedir, pszDir);
}

void func_002CF7B8(void)
{
    char szGameDir[260];

    func_RemoveNonBase();

    sprintf(szGameDir, "%s/%s", host_parms.basedir, "VALVE");
    func_00244EE0(Info_Serverinfo(), "*gamedir", "VALVE", 512);
    strcpy(com_gamedir, szGameDir);
}

void func_002CF8B0(void)
{
    int i, j;
    char basedir[260];
    searchpath_t *search;

    i = COM_CheckParm("-basedir");
    if (i && i < com_argc - 1)
        strcpy(basedir, com_argv[i + 1]);
    else
        strcpy(basedir, host_parms.basedir);

    j = strlen(basedir);
    if (j > 0)
    {
        if (basedir[j - 1] == '\\' || basedir[j - 1] == '/')
            basedir[j - 1] = 0;
    }

    i = COM_CheckParm("-cachedir");
    if (i && i < com_argc - 1)
    {
        if (com_argv[i + 1][0] == '-')
            D_00555F00[0] = 0;
        else
            strcpy(D_00555F00, com_argv[i + 1]);
    }
    else if (host_parms.cachedir)
        strcpy(D_00555F00, host_parms.cachedir);
    else
        D_00555F00[0] = 0;

    func_002CFE28(1, basedir, "VALVE");

    i = COM_CheckParm("-game");
    if (i && i < com_argc - 1)
        func_002CFE28(0, basedir, com_argv[i + 1]);

    i = COM_CheckParm("-path");
    if (i)
    {
        D_00399868 = NULL;
        while (++i < com_argc)
        {
            if (!com_argv[i] || com_argv[i][0] == '+' || com_argv[i][0] == '-')
                break;

            search = Mem_DbgMalloc(sizeof(searchpath_t), "engine/HlFileIO.c", 342);
            memset(search, 0, sizeof(searchpath_t));
            if (!strcmp(func_002CFD30(com_argv[i]), "pak"))
            {
                search->pack = func_002D0368(com_argv[i]);
                if (!search->pack)
                    Sys_Error("Couldn't load packfile: %s", com_argv[i]);
            }
            else
                strcpy(search->filename, com_argv[i]);
            search->next = D_00399868;
            D_00399868 = search;
        }
    }
}

void func_002CFB90(char *pszSubString)
{
    char szExt[128];
    char szBase[128];
    int nSubStringLen;
    searchpath_t *search;
    pack_t *pak;
    int i;

    nSubStringLen = 0;
    if (pszSubString && *pszSubString)
        nSubStringLen = strlen(pszSubString);

    for (search = D_00399868; search; search = search->next)
    {
        Con_Printf("\nDir:  %s\n", search->directory);
        Con_Printf("-------------\n");

        pak = search->pack;
        if (!pak)
            continue;

        for (i = 0; i < pak->numfiles; i++)
        {
            if (strncasecmp("maps/", pak->files[i].name, 4))
                continue;
            if (strcasecmp(szExt, ".bs2"))
                continue;
            if (nSubStringLen)
            {
                if (!strncasecmp(szBase, pszSubString, nSubStringLen))
                    Con_Printf("(pak) %s\n", pak->files[i].name + 5);
            }
            else
                Con_Printf("(pak) %s\n", pak->files[i].name + 5);
        }
    }
}

void func_002CFD08(void *buffer)
{
    if (buffer)
        Mem_Free(buffer);
}

int func_002CFD28(void)
{
    return 0;
}

char *func_002CFD30(char *in)
{
    int i;

    while (*in && *in != '.')
        in++;
    if (!*in)
        return "";
    in++;
    for (i = 0; i < 7 && *in; i++, in++)
        D_00465A78[i] = *in;
    D_00465A78[i] = 0;
    return D_00465A78;
}

void func_002CFDD0(searchpath_t *search)
{
    if (!search)
        return;

    if (search->pack)
    {
        if (search->pack->files)
            Mem_Free(search->pack->files);
        Mem_Free(search->pack);
    }
    Mem_Free(search);
}

void func_002CFE28(int bIsBase, char *pszBaseDir, char *pszDir)
{
    char pakfile[260];
    char szGameDir[260];
    searchpath_t *search;
    pack_t *pak;

    sprintf(szGameDir, "%s/%s", pszBaseDir, pszDir);
    func_00244EE0(Info_Serverinfo(), "*gamedir", pszDir, 512);
    strcpy(com_gamedir, szGameDir);

    sprintf(pakfile, "%s\\PAK%i.PAK;1", pszDir, 0);
    pak = func_002D0368(pakfile);
    if (pak)
    {
        search = Mem_DbgMalloc(sizeof(searchpath_t), "engine/HlFileIO.c", 505);
        memset(search, 0, sizeof(searchpath_t));
        strcpy(search->directory, pszDir);
        search->pack = pak;
        search->next = D_00399868;
        search->bIsBase = bIsBase;
        D_00399868 = search;
    }

    search = Mem_DbgMalloc(sizeof(searchpath_t), "engine/HlFileIO.c", 518);
    memset(search, 0, sizeof(searchpath_t));
    strcpy(search->directory, pszDir);
    search->bIsBase = bIsBase;
    strcpy(search->filename, szGameDir);
    search->next = D_00399868;
    D_00399868 = search;
}
