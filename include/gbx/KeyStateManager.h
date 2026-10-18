#ifndef GBX_KEYSTATEMANAGER_H
#define GBX_KEYSTATEMANAGER_H

/* Gbx/KeyStateManager: estado de teclas/botões (solto, pressionado, segurado, liberado) */

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

#endif
