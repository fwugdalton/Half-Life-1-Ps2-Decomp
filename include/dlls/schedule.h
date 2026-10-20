#ifndef DLLS_SCHEDULE_H
#define DLLS_SCHEDULE_H

/* schedule.h do HLSDK (parcial) */

typedef enum
{
    MONSTERSTATE_NONE = 0,
    MONSTERSTATE_IDLE,
    MONSTERSTATE_COMBAT,
    MONSTERSTATE_ALERT,
    MONSTERSTATE_HUNT,
    MONSTERSTATE_PRONE,
    MONSTERSTATE_SCRIPT,
    MONSTERSTATE_PLAYDEAD,
    MONSTERSTATE_DEAD
} MONSTERSTATE;

struct Task_t
{
    int iTask;
    float flData;
};

struct Schedule_t
{
    Task_t *pTasklist;
    int cTasks;
    int iInterruptMask;
    int iSoundMask;
    const char *pName;
};

struct WayPoint_t
{
    Vector vecLocation;
    int iType;
};

#define bits_COND_NO_AMMO_LOADED    (1 << 0)
#define bits_COND_SEE_HATE          (1 << 1)
#define bits_COND_SEE_FEAR          (1 << 2)
#define bits_COND_SEE_DISLIKE       (1 << 3)
#define bits_COND_SEE_ENEMY         (1 << 4)
#define bits_COND_ENEMY_OCCLUDED    (1 << 5)
#define bits_COND_SMELL_FOOD        (1 << 6)
#define bits_COND_ENEMY_TOOFAR      (1 << 7)
#define bits_COND_LIGHT_DAMAGE      (1 << 8)
#define bits_COND_HEAVY_DAMAGE      (1 << 9)
#define bits_COND_CAN_RANGE_ATTACK1 (1 << 10)
#define bits_COND_CAN_MELEE_ATTACK1 (1 << 11)
#define bits_COND_CAN_RANGE_ATTACK2 (1 << 12)
#define bits_COND_CAN_MELEE_ATTACK2 (1 << 13)
#define bits_COND_PROVOKED          (1 << 15)
#define bits_COND_NEW_ENEMY         (1 << 16)
#define bits_COND_HEAR_SOUND        (1 << 17)
#define bits_COND_SMELL             (1 << 18)
#define bits_COND_ENEMY_FACING_ME   (1 << 19)
#define bits_COND_ENEMY_DEAD        (1 << 20)
#define bits_COND_SEE_CLIENT        (1 << 21)
#define bits_COND_SEE_NEMESIS       (1 << 22)
#define bits_COND_SPECIAL1          (1 << 28)
#define bits_COND_SPECIAL2          (1 << 29)
#define bits_COND_TASK_FAILED       (1 << 30)
#define bits_COND_SCHEDULE_DONE     (1 << 31)

#endif
