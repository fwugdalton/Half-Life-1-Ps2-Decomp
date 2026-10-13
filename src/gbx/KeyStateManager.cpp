/* Gbx/KeyStateManager.cpp -- estado de teclas/botões (solto, pressionado, segurado, liberado) */

#include "common.h"

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
extern void (*D_00399084)(int data, int down);
}

enum {
    KEY_UP,
    KEY_PRESSED,
    KEY_DOWN,
    KEY_RELEASED
};

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
    void Press(unsigned int key);
    void Update();
    void Dispatch();
    int AnyDown();

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

void KeyStateManager::Press(unsigned int key)
{
    switch (m_states[key].state) {
    case KEY_UP:
        m_states[key].state = KEY_PRESSED;
        break;
    case KEY_PRESSED:
        m_states[key].state = KEY_PRESSED;
        break;
    case KEY_DOWN:
        m_states[key].state = KEY_RELEASED;
        break;
    case KEY_RELEASED:
        m_states[key].state = KEY_RELEASED;
        break;
    }
}

void KeyStateManager::Update()
{
    unsigned int i;

    for (i = 0; i < m_count; i++) {
        if (m_states[i].state == KEY_DOWN) {
            m_states[i].state = KEY_RELEASED;
        }
    }
}

void KeyStateManager::Dispatch()
{
    unsigned int i;

    for (i = 0; i < m_count; i++) {
        switch (m_states[i].state) {
        case KEY_UP:
            break;
        case KEY_PRESSED:
            m_states[i].state = KEY_DOWN;
            D_00399084(m_states[i].data, 1);
            break;
        case KEY_DOWN:
            m_states[i].state = KEY_UP;
            D_00399084(m_states[i].data, 0);
            break;
        case KEY_RELEASED:
            m_states[i].state = KEY_DOWN;
            D_00399084(m_states[i].data, 1);
            break;
        }
    }
}

int KeyStateManager::AnyDown()
{
    unsigned int i;
    int down = 0;

    for (i = 0; i < m_count; i++) {
        switch (m_states[i].state) {
        case KEY_PRESSED:
        case KEY_DOWN:
        case KEY_RELEASED:
            down = 1;
            break;
        default:
            down = 0;
            break;
        }
        if (down == 1) {
            break;
        }
    }
    return down;
}

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
