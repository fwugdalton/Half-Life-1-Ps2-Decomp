/* Gbx/KeyStateManager.cpp -- estado de teclas/botões (solto, pressionado, segurado, liberado) */

#include "common.h"

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
extern void (*D_00399084)(int data, int down);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
unsigned long func_00229B88(void);
void func_00234B58(void);
void func_00234BE0(unsigned char *info);
int func_00234C28(int port, unsigned char *report);
void func_00234CB8(int);
extern unsigned char D_003A6480[];
extern unsigned char D_003A6500[];
extern volatile int D_00334FC0;
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

extern "C" {
extern KeyStateManager *D_003281F8;
extern unsigned long D_00328200;
void func_0022A4C0(void);
}

/* versão para os botões do mouse USB (5 botões) */
class MouseStateManager : public KeyStateManager {
public:
    MouseStateManager();
    ~MouseStateManager();
    void Press(unsigned int key);
    void Release(unsigned int key);
    void Clear();
    void Dispatch();
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

MouseStateManager::MouseStateManager()
{
}

MouseStateManager::~MouseStateManager()
{
}

void MouseStateManager::Press(unsigned int key)
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

#ifdef NON_MATCHING
/* só a ordem do último sw/jr (o original não preenche o delay slot do case KEY_DOWN) */
void MouseStateManager::Release(unsigned int key)
{
    switch (m_states[key].state) {
    case KEY_UP:
        break;
    case KEY_PRESSED:
        m_states[key].state = KEY_DOWN;
        break;
    case KEY_DOWN:
        m_states[key].state = KEY_DOWN;
        break;
    case KEY_RELEASED:
        m_states[key].state = KEY_DOWN;
        break;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", Release__17MouseStateManagerUi);
#endif

void MouseStateManager::Clear()
{
    unsigned int i;

    for (i = 0; i < m_count; i++) {
        switch (m_states[i].state) {
        case KEY_UP:
            break;
        case KEY_PRESSED:
        case KEY_DOWN:
        case KEY_RELEASED:
            m_states[i].state = KEY_UP;
            D_00399084(m_states[i].data, 0);
            break;
        }
    }
}

void MouseStateManager::Dispatch()
{
    unsigned int i;

    for (i = 0; i < m_count; i++) {
        switch (m_states[i].state) {
        case KEY_UP:
            break;
        case KEY_PRESSED:
            m_states[i].state = KEY_RELEASED;
            D_00399084(m_states[i].data, 1);
            break;
        case KEY_DOWN:
            m_states[i].state = KEY_UP;
            D_00399084(m_states[i].data, 0);
            break;
        case KEY_RELEASED:
            m_states[i].state = KEY_RELEASED;
            break;
        }
    }
}

extern "C" void func_0022A280(void)
{
    func_00234B58();
    Mem_PushTag();
    Mem_SetTag(1);
    D_003281F8 = new KeyStateManager;
    D_003281F8->Init(0xEB);
    Mem_PopTag();
    func_0022A4C0();
}

#ifdef NON_MATCHING
/* o original guarda a constante 1 em s7 e refaz o lui de D_00334FC0; aqui o gcc iça o endereço */
/* lê os relatórios dos teclados USB e atualiza o estado das teclas */
extern "C" void func_0022A2E0(int first)
{
    int i;
    int j;
    unsigned char key;

    if (first == 1) {
        func_00234BE0(D_003A6480);
        func_00234CB8(0);
    }
    D_00334FC0 = 0;
    for (i = 0; i < D_003A6480[0]; i++) {
        if (D_003A6480[i + 2] == 0) {
            continue;
        }
        D_00334FC0 = 1;
        if (func_00234C28(i, D_003A6500) != 0) {
            continue;
        }
        func_00234CB8(0);
        if (D_003A6500[1] == 0) {
            continue;
        }
        if ((D_003A6500[2] & 0x01) || (D_003A6500[2] & 0x10)) {
            D_003281F8->Press(0xE9);
        }
        if (D_003A6500[2] & 0x22) {
            D_003281F8->Press(0xEA);
        }
        if (D_003A6500[2] & 0x44) {
            D_003281F8->Press(0xE8);
        }
        for (j = 2; j < D_003A6500[1]; j++) {
            key = D_003A6500[j + 2];
            if (key >= 4) {
                D_003281F8->Press(key);
            }
        }
        D_003281F8->Dispatch();
    }
    if (D_003281F8->AnyDown() == 1) {
        D_00328200 = func_00229B88();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/KeyStateManager", func_0022A2E0);
#endif

extern "C" unsigned long func_0022A4B0(void)
{
    return D_00328200;
}

void func_0022A4C0(void)
{
    D_003281F8->SetData(0, 0);
    D_003281F8->SetData(1, 0);
    D_003281F8->SetData(2, 0);
    D_003281F8->SetData(3, 0);
    D_003281F8->SetData(4, 'a');
    D_003281F8->SetData(5, 'b');
    D_003281F8->SetData(6, 'c');
    D_003281F8->SetData(7, 'd');
    D_003281F8->SetData(8, 'e');
    D_003281F8->SetData(9, 'f');
    D_003281F8->SetData(10, 'g');
    D_003281F8->SetData(11, 'h');
    D_003281F8->SetData(12, 'i');
    D_003281F8->SetData(13, 'j');
    D_003281F8->SetData(14, 'k');
    D_003281F8->SetData(15, 'l');
    D_003281F8->SetData(16, 'm');
    D_003281F8->SetData(17, 'n');
    D_003281F8->SetData(18, 'o');
    D_003281F8->SetData(19, 'p');
    D_003281F8->SetData(20, 'q');
    D_003281F8->SetData(21, 'r');
    D_003281F8->SetData(22, 's');
    D_003281F8->SetData(23, 't');
    D_003281F8->SetData(24, 'u');
    D_003281F8->SetData(25, 'v');
    D_003281F8->SetData(26, 'w');
    D_003281F8->SetData(27, 'x');
    D_003281F8->SetData(28, 'y');
    D_003281F8->SetData(29, 'z');
    D_003281F8->SetData(30, '1');
    D_003281F8->SetData(31, '2');
    D_003281F8->SetData(32, '3');
    D_003281F8->SetData(33, '4');
    D_003281F8->SetData(34, '5');
    D_003281F8->SetData(35, '6');
    D_003281F8->SetData(36, '7');
    D_003281F8->SetData(37, '8');
    D_003281F8->SetData(38, '9');
    D_003281F8->SetData(39, '0');
    D_003281F8->SetData(40, 0xd);
    D_003281F8->SetData(41, 0x1b);
    D_003281F8->SetData(42, 0x7f);
    D_003281F8->SetData(43, 9);
    D_003281F8->SetData(44, ' ');
    D_003281F8->SetData(45, '-');
    D_003281F8->SetData(46, '=');
    D_003281F8->SetData(47, '[');
    D_003281F8->SetData(48, ']');
    D_003281F8->SetData(49, 0x5c);
    D_003281F8->SetData(50, 0);
    D_003281F8->SetData(51, ';');
    D_003281F8->SetData(52, 0x27);
    D_003281F8->SetData(53, '`');
    D_003281F8->SetData(54, ',');
    D_003281F8->SetData(55, '.');
    D_003281F8->SetData(56, '/');
    D_003281F8->SetData(57, 0);
    D_003281F8->SetData(58, 0x87);
    D_003281F8->SetData(59, 0x88);
    D_003281F8->SetData(60, 0x89);
    D_003281F8->SetData(61, 0x8a);
    D_003281F8->SetData(62, 0x8b);
    D_003281F8->SetData(63, 0x8c);
    D_003281F8->SetData(64, 0x8d);
    D_003281F8->SetData(65, 0x8e);
    D_003281F8->SetData(66, 0x8f);
    D_003281F8->SetData(67, 0x90);
    D_003281F8->SetData(68, 0x91);
    D_003281F8->SetData(69, 0x92);
    D_003281F8->SetData(70, 0);
    D_003281F8->SetData(71, 0);
    D_003281F8->SetData(72, 0xff);
    D_003281F8->SetData(73, 0x93);
    D_003281F8->SetData(74, 0x97);
    D_003281F8->SetData(75, 0x96);
    D_003281F8->SetData(76, 0x94);
    D_003281F8->SetData(77, 0x98);
    D_003281F8->SetData(78, 0x95);
    D_003281F8->SetData(79, 0x83);
    D_003281F8->SetData(80, 0x82);
    D_003281F8->SetData(81, 0x81);
    D_003281F8->SetData(82, 0x80);
    D_003281F8->SetData(83, 0);
    D_003281F8->SetData(84, '/');
    D_003281F8->SetData(85, '*');
    D_003281F8->SetData(86, '-');
    D_003281F8->SetData(87, '+');
    D_003281F8->SetData(88, 0xd);
    D_003281F8->SetData(89, 0x98);
    D_003281F8->SetData(90, 0x81);
    D_003281F8->SetData(91, 0x95);
    D_003281F8->SetData(92, 0x82);
    D_003281F8->SetData(93, '5');
    D_003281F8->SetData(94, 0x83);
    D_003281F8->SetData(95, 0x97);
    D_003281F8->SetData(96, 0x80);
    D_003281F8->SetData(97, 0x96);
    D_003281F8->SetData(98, 0x93);
    D_003281F8->SetData(99, 0x94);
    D_003281F8->SetData(233, 0x85);
    D_003281F8->SetData(234, 0x86);
    D_003281F8->SetData(232, 0x84);
}

