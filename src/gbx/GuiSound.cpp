/* Gbx: sons da interface tocados direto no SPU2 (47 vozes, GUIDATA\GUISOUND.PAK) */

#include "common.h"

extern "C" {
#include "engine/decals.h"

/* bloco alocado na memória do SPU2 (SPUMemory) */
struct SpuBlock {
    int addr;
    int size;
};

struct GuiVoice {
    int free;
    int voice;
    int core;
    char name[0xC];
    SpuBlock block;     /* 0x18 */
    int size;
    int rate;
};

void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, const char *path);
void func_00235198(pakfile_t *pak);
int strcasecmp(const char *a, const char *b);
char *strcpy(char *dst, const char *src);
void func_002D6260(void);
int func_002D6320(const void *src, void *dst, int size, int mode);
int func_002D63A0(int a, int cmd, ...);
void func_002EF8E0(int a);
void func_002F3A80(void);
void *func_002F3B08(int size);
void func_002F3B78(void *p);
SpuBlock func_002320A0(int size);
void func_00232150(SpuBlock *b);
int func_00231B30(const char *name, int rate, int size, char *data);
void func_00231C68(int idx);
void func_00231DC8(const void *src, int addr, int size);
void func_00231E70(int idx);
int func_00231F60(void);

extern int D_00328980;
extern GuiVoice D_003A7840[47];

void func_00231908(void) {
    GuiVoice *v;
    int i;

    func_002D6260();
    func_002D63A0(1, 0x8000, 0);
    for (i = 0; i < 2; i++) {
        func_002D63A0(1, 0x8010, i | 0x980, 0x3FFF);
        func_002D63A0(1, 0x8010, i | 0xA80, 0x3FFF);
    }
    func_002D63A0(1, 0x8010, 0xF80, 0);
    func_002D63A0(1, 0x8010, 0x1080, 0);
    func_002F3A80();
    v = D_003A7840;
    for (i = 0; i < 47; i++, v++) {
        v->free = 1;
        if (i < 24) {
            v->voice = i;
            v->core = 0;
        } else {
            v->core = 1;
            v->voice = i - 24;
        }
    }
    D_00328980 = 1;
}

void func_002319F8(void) {
    pakfile_t pak;
    int i;

    i = 0;
    Mem_PushTag();
    Mem_SetTag(2);
    Pak_Init(&pak);
    pak.inMemory = 1;
    Pak_Load(&pak, "GUIDATA\\GUISOUND.PAK;1");
    Mem_PopTag();
    for (; i < pak.count; i++) {
        func_00231B30(pak.lumps[i].name, 44100, pak.lumps[i].size, (char *)pak.data + pak.lumps[i].filepos);
    }
    func_00235198(&pak);
}

void func_00231A98(const char *name) {
    int i;

    for (i = 0; i < 47; i++) {
        if (D_003A7840[i].free == 0 && strcasecmp(D_003A7840[i].name, name) == 0) {
            func_00231C68(i);
            break;
        }
    }
}

int func_00231B30(const char *name, int rate, int size, char *data) {
    GuiVoice *v;
    int idx;

    data += 0x30;
    size -= 0x30;
    idx = func_00231F60();
    v = &D_003A7840[idx];
    if (name != 0) {
        strcpy(v->name, name);
    } else {
        strcpy(v->name, "");
    }
    v->rate = rate;
    v->size = size;
    v->free = 0;
    v->block = func_002320A0(size);
    func_00231DC8(data, v->block.addr, size);
    func_00231E70(idx);
    return idx;
}

void func_00231C18(int idx) {
    GuiVoice *v;

    v = &D_003A7840[idx];
    if (v->free == 0) {
        func_00232150(&D_003A7840[idx].block);
        v->free = 1;
    }
}

void func_00231C68(int idx) {
    func_002D63A0(1, 0x8030, D_003A7840[idx].core | 0x1500, 1 << D_003A7840[idx].voice);
}

void func_00231CB8(float vol) {
    int v;
    int i;
    int p;

    v = (int)(vol * 16383.0f);
    for (i = 0; i < 47; i++) {
        if (D_003A7840[i].free == 0) {
            p = D_003A7840[i].core | (D_003A7840[i].voice << 1);
            func_002D63A0(1, 0x8010, p, v);
            func_002D63A0(1, 0x8010, p | 0x100, v);
        }
    }
}

void func_00231D68(float vol) {
    int v;

    v = (int)(vol * 16383.0f);
    func_002D63A0(1, 0x8010, 0x2F, v);
    func_002D63A0(1, 0x8010, 0x12F, v);
}

void func_00231DC8(const void *src, int addr, int size) {
    void *buf;

    func_002EF8E0(0);
    buf = func_002F3B08(size);
    func_002D6320(src, buf, size, 1);
    func_002D63A0(1, 0x80D0, 1, 0, buf, addr, size);
    func_002D63A0(1, 0x80F0, 1, 1);
    func_002F3B78(buf);
}

void func_00231E70(int idx) {
    int p;

    p = D_003A7840[idx].core | (D_003A7840[idx].voice << 1);
    func_002D63A0(1, 0x8010, p, 0x1EFF);
    func_002D63A0(1, 0x8010, p | 0x100, 0x1EFF);
    func_002D63A0(1, 0x8010, p | 0x200, ((unsigned)D_003A7840[idx].rate << 12) / 48000);
    func_002D63A0(1, 0x8050, p | 0x2040, D_003A7840[idx].block.addr);
    func_002D63A0(1, 0x8010, p | 0x300, 0xF);
    func_002D63A0(1, 0x8010, p | 0x400, 0x1FC0);
}

int func_00231F60(void) {
    int i;
    int ret;

    ret = -1;
    for (i = 0; i < 47; i++) {
        if (D_003A7840[i].free == 1) {
            ret = i;
            break;
        }
    }
    return ret;
}

int func_00231FC0(int *idx) {
    return (unsigned)*idx < 47;
}

}
