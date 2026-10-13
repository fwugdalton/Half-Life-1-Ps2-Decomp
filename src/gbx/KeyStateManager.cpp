/* Gbx/KeyStateManager.cpp -- estado de teclas/botões (solto, pressionado, segurado, liberado) */

#include "common.h"

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
}

struct KeyState {
    int state;
    int data;
};

class KeyStateManager {
public:
    KeyStateManager();
    ~KeyStateManager();
    void Init(unsigned int count);
    void Shutdown();
    void SetData(unsigned int key, int data);

    KeyState *m_states;
    unsigned int m_count;
    unsigned char m_initialized;
};

KeyStateManager::KeyStateManager()
{
    m_initialized = 0;
}

KeyStateManager::~KeyStateManager()
{
}

INCLUDE_RODATA("asm/nonmatchings/gbx/KeyStateManager", D_00386EA8);

void KeyStateManager::Init(unsigned int count)
{
    unsigned int i;

    m_count = count;
    m_states = (KeyState *)Mem_DbgMalloc(count * sizeof(KeyState), "Gbx/KeyStateManager.cpp", 0x27);
    for (i = 0; i < count; i++) {
        m_states[i].state = 0;
        m_states[i].data = 0;
    }
    m_initialized = 1;
}

void KeyStateManager::Shutdown()
{
    Mem_Free(m_states);
    m_initialized = 0;
}

void KeyStateManager::SetData(unsigned int key, int data)
{
    m_states[key].data = data;
}

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_00229DF8);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_00229E60);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_00229EB0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_00229FB0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A000);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A028);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A048);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A0B0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A108);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A1A0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A280);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A2E0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A4B0);

INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A4C0);
