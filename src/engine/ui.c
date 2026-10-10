#include "common.h"
#include "engine/cmd.h"

/* interface (VGUI) desativada no PS2: só sobra a fila de eventos e uns stubs */

typedef struct uirect_s
{
    int x, y, width, height;
    struct uirect_s *pnext;
} uirect_t;

typedef struct uievent_s
{
    int flags;                  /* 0x00: 1 = tecla, 2 = mouse */
    int x;                      /* 0x04 */
    int y;                      /* 0x08 */
    int key;                    /* 0x0C */
    struct uievent_s *next;     /* 0x10 */
} uievent_t; /* 0x14 */

extern uievent_t *ui_queue;

void *memset(void *, int, unsigned int);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
void func_002ACC08(uirect_t *rect, int color);

void UI_DrawRect(uirect_t *rect, int color)
{
    uirect_t r;

    r.x = rect->x;
    r.y = rect->y;
    r.width = rect->width;
    r.height = 1;
    func_002ACC08(&r, color);

    r.y = rect->y + rect->height;
    func_002ACC08(&r, color);

    r.x = rect->x;
    r.y = rect->y;
    r.width = 1;
    r.height = rect->height;
    func_002ACC08(&r, color);

    r.x = rect->x + rect->width;
    func_002ACC08(&r, color);
}

void UI_QueueEvent(uievent_t *ev)
{
    uievent_t *p;

    for (p = ui_queue; p && p->next; p = p->next)
        ;

    if (p)
        p->next = ev;
    else
        ui_queue = ev;
}

void UI_QueueMouseEvent(int x, int y)
{
    uievent_t *ev;

    ev = Mem_DbgMalloc(sizeof(uievent_t), "engine/ui.c", 88);
    memset(ev, 0, sizeof(uievent_t));
    ev->flags |= 2;
    ev->x = x;
    ev->y = y;
    ev->key = 0;
    ev->next = NULL;
    UI_QueueEvent(ev);
}

void func_002943F8(void)
{
}

void UI_Disabled_f(void)
{
    Con_Printf("UI disabled\n");
}

void func_00294420(void)
{
}

uievent_t *UI_GetEvent(void)
{
    uievent_t *ev;

    ev = ui_queue;
    if (!ev)
        return NULL;

    ui_queue = ev->next;
    return ev;
}

void UI_FreeEvent(uievent_t *ev)
{
    Mem_Free(ev);
}

void UI_QueueKeyEvent(int key)
{
    uievent_t *ev;

    ev = Mem_DbgMalloc(sizeof(uievent_t), "engine/ui.c", 176);
    memset(ev, 0, sizeof(uievent_t));
    ev->flags |= 1;
    ev->key = key;
    ev->x = 0;
    ev->y = 0;
    ev->next = NULL;
    UI_QueueEvent(ev);
}

void func_002944E8(void *p)
{
    Mem_Free(p);
}

void func_00294508(void)
{
}

void func_00294510(void)
{
}
