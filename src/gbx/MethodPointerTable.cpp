/* Gbx: tabela de ponteiros de métodos (registro das funções do jogo pelo nome) */

#include "common.h"

extern "C" {

/* uma entrada da tabela (0x18 bytes) */
struct MethodEntry {
    int data[6];
};

/* a tabela global; a base (0x328 bytes) é a classe de 0x229378 */
struct MethodPointerTable {
    char base[0x328];
    void *vtable;
    MethodEntry *entries;
    int count;
    int max;
};

void *Mem_DbgMalloc(int size, const char *file, int line);
void func_00229378(MethodPointerTable *t);
void func_00229398(MethodPointerTable *t, int flags);
int func_002293C8(MethodPointerTable *t, void *a, void *b);
int func_002294D0(MethodPointerTable *t, void *a);
int func_00229570(MethodPointerTable *t, void *a);
int func_00239AB0(void *a);
int func_00239B00(void *a, void *b);
MethodPointerTable *func_00239B28(MethodPointerTable *t);
void func_00239B68(MethodPointerTable *t, int flags);
void func_00124218(void);
void func_00126E20(void);
void func_0012CEE8(void);
void func_0012D328(void);
void func_0012FAA8(void);
void func_00135828(void);
void func_00136B48(void);
void func_00138510(void);
void func_0013B8D0(void);
void func_0013E308(void);
void func_001410E0(void);
void func_001446D8(void);
void func_0014D4C0(void);
void func_001514B0(void);
void func_00151A48(void);
void func_00154318(void);
void func_00156590(void);
void func_001579B0(void);
void func_0015A3E8(void);
void func_00160740(void);
void func_00164370(void);
void func_00164C68(void);
void func_001672C8(void);
void func_001698A8(void);
void func_00173CC8(void);
void func_00176DF8(void);
void func_00179AF8(void);
void func_0017E008(void);
void func_0017EA78(void);
void func_00180298(void);
void func_001810C0(void);
void func_00183FE0(void);
void func_00186AA0(void);
void func_0018B940(void);
void func_0018D298(void);
void func_0018EBC8(void);
void func_0018FDD8(void);
void func_00194ED8(void);
void func_00197848(void);
void func_0019A4F8(void);
void func_0019B0D0(void);
void func_0019F2F0(void);
void func_001A0CC8(void);
void func_001A1680(void);
void func_001A89C8(void);
void func_001A9B50(void);
void func_001B3290(void);
void func_001B9480(void);
void func_001BC1B0(void);
void func_001C2A38(void);
void func_001D2BF0(void);
void func_001D73C8(void);
void func_001D9630(void);
void func_001DAEC0(void);
void func_001E0020(void);
void func_001E2928(void);
void func_001E6A80(void);
void func_001EA230(void);
void func_001EE138(void);
void func_001EFB20(void);
void func_001F5C18(void);
void func_001F7ED0(void);
void func_001FF5A8(void);
void func_00202840(void);
void func_00206300(void);
void func_0020F1D0(void);
void func_0020C260(void);
void func_00212630(void);
void func_002142F8(void);

extern MethodPointerTable D_003A95D0;

int func_002397D0(void *a, void *b) {
    return func_00239B00(a, b);
}

int func_002397F0(void *a, void *b) {
    return func_00239B00(a, b);
}

int func_00239810(void *a, void *b) {
    return func_00239B00(a, b);
}

int func_00239830(void *a, void *b) {
    return func_00239B00(a, b);
}

int func_00239850(void *a) {
    return func_00239AB0(a);
}

/* registra as funções de cada arquivo do jogo */
void func_00239870(void) {
    func_00124218();
    func_00126E20();
    func_0012CEE8();
    func_0012D328();
    func_0012FAA8();
    func_00135828();
    func_00136B48();
    func_00138510();
    func_0013B8D0();
    func_0013E308();
    func_001410E0();
    func_001446D8();
    func_0014D4C0();
    func_001514B0();
    func_00151A48();
    func_00154318();
    func_00156590();
    func_001579B0();
    func_0015A3E8();
    func_00160740();
    func_00164370();
    func_00164C68();
    func_001672C8();
    func_001698A8();
    func_00173CC8();
    func_00176DF8();
    func_00179AF8();
    func_0017E008();
    func_0017EA78();
    func_00180298();
    func_001810C0();
    func_00183FE0();
    func_00186AA0();
    func_0018B940();
    func_0018D298();
    func_0018EBC8();
    func_0018FDD8();
    func_00194ED8();
    func_00197848();
    func_0019A4F8();
    func_0019B0D0();
    func_0019F2F0();
    func_001A0CC8();
    func_001A1680();
    func_001A89C8();
    func_001A9B50();
    func_001B3290();
    func_001B9480();
    func_001BC1B0();
    func_001C2A38();
    func_001D2BF0();
    func_001D73C8();
    func_001D9630();
    func_001DAEC0();
    func_001E0020();
    func_001E2928();
    func_001E6A80();
    func_001EA230();
    func_001EE138();
    func_001EFB20();
    func_001F5C18();
    func_001F7ED0();
    func_001FF5A8();
    func_00202840();
    func_00206300();
    func_0020F1D0();
    func_0020C260();
    func_00212630();
    func_002142F8();
}

int func_00239AB0(void *a) {
    return func_00229570(&D_003A95D0, a);
}

int func_00239AD8(void *a) {
    return func_002294D0(&D_003A95D0, a);
}

#ifdef NON_MATCHING /* o lui e o daddu a1 saem trocados */
int func_00239B00(void *a, void *b) {
    return func_002293C8(&D_003A95D0, a, b);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MethodPointerTable", func_00239B00);
#endif

extern char D_00388810[];

MethodPointerTable *func_00239B28(MethodPointerTable *t) {
    func_00229378(t);
    t->entries = 0;
    t->vtable = D_00388810;
    t->count = 0;
    t->max = 0;
    return t;
}

void func_00239B68(MethodPointerTable *t, int flags) {
    t->vtable = D_00388810;
    func_00229398(t, flags);
}

MethodEntry *func_00239B90(MethodPointerTable *t) {
    if (t->count == t->max) {
        t->entries = (MethodEntry *)Mem_DbgMalloc(50 * sizeof(MethodEntry), "Gbx/MethodPointerTable.cpp", 0xBC);
        t->max = 50;
        t->count = 0;
    }
    return &t->entries[t->count++];
}

__asm__(".section .rodata\n.balign 8\n.section .text");
INCLUDE_RODATA("asm/nonmatchings/gbx/MethodPointerTable", D_00388810);

/* construção e destruição do objeto global */
void func_00239C00(int init, int priority) {
    if (priority == 0xFFFF) {
        if (init) {
            func_00239B28(&D_003A95D0);
        } else {
            func_00239B68(&D_003A95D0, 2);
        }
    }
}

void func_00239C40(void) {
    func_00239C00(1, 0xFFFF);
}

void func_00239C60(void) {
    func_00239C00(0, 0xFFFF);
}
}
