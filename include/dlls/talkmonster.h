#ifndef DLLS_TALKMONSTER_H
#define DLLS_TALKMONSTER_H

/* talkmonster.h do HLSDK. CTalkMonster vai de 0x2A0 a 0x310. */

#ifndef MONSTERS_H
#include "dlls/monsters.h"
#endif

#define TALKRANGE_MIN 500.0

#define TLK_STARE_DIST  128
#define TLK_CFRIENDS    3

typedef enum
{
    TLK_ANSWER = 0,
    TLK_QUESTION,
    TLK_IDLE,
    TLK_STARE,
    TLK_USE,
    TLK_UNUSE,
    TLK_STOP,
    TLK_NOSHOOT,
    TLK_HELLO,
    TLK_PHELLO,
    TLK_PIDLE,
    TLK_PQUESTION,
    TLK_PLHURT1,
    TLK_PLHURT2,
    TLK_PLHURT3,
    TLK_SMELL,
    TLK_WOUND,
    TLK_MORTAL,

    TLK_CGROUPS,
} TALKGROUPNAMES;

enum
{
    SCHED_CANT_FOLLOW = LAST_COMMON_SCHEDULE + 1,
    SCHED_MOVE_AWAY,
    SCHED_MOVE_AWAY_FOLLOW,
    SCHED_MOVE_AWAY_FAIL,

    LAST_TALKMONSTER_SCHEDULE,
};

enum
{
    TASK_CANT_FOLLOW = LAST_COMMON_TASK + 1,
    TASK_MOVE_AWAY_PATH,
    TASK_WALK_PATH_FOR_UNITS,

    TASK_TLK_RESPOND,
    TASK_TLK_SPEAK,
    TASK_TLK_HELLO,
    TASK_TLK_HEADRESET,
    TASK_TLK_STOPSHOOTING,
    TASK_TLK_STARE,
    TASK_TLK_LOOK_AT_CLIENT,
    TASK_TLK_CLIENT_STARE,
    TASK_TLK_EYECONTACT,
    TASK_TLK_IDEALYAW,
    TASK_FACE_PLAYER,

    LAST_TALKMONSTER_TASK,
};

class CTalkMonster : public CBaseMonster
{
public:
    void TalkInit(void);
    CBaseEntity *FindNearestFriend(BOOL fPlayer);
    float TargetDistance(void);
    void StopTalking(void) { SentenceStop(); }

    void Precache(void);
    int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    void Touch(CBaseEntity *pOther);
    void Killed(entvars_t *pevAttacker, int iGib);
    int IRelationship(CBaseEntity *pTarget);
    virtual int CanPlaySentence(BOOL fDisregardState);
    virtual void PlaySentence(const char *pszSentence, float duration, float volume, float attenuation);
    void PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener);
    void KeyValue(KeyValueData *pkvd);

    void SetActivity(Activity newActivity);
    Schedule_t *GetScheduleOfType(int Type);
    void StartTask(Task_t *pTask);
    void RunTask(Task_t *pTask);
    void HandleAnimEvent(MonsterEvent_t *pEvent);
    void PrescheduleThink(void);

    int GetVoice(void);
    void IdleRespond(void);
    int FIdleSpeak(void);
    int FIdleStare(void);
    int FIdleHello(void);
    void IdleHeadTurn(Vector &vecFriend);
    int FOkToSpeak(void);
    void TrySmellTalk(void);
    CBaseEntity *EnumFriends(CBaseEntity *pentPrevious, int listNumber, BOOL bTrace);
    void AlertFriends(void);
    void ShutUpFriends(void);
    BOOL IsTalking(void);
    void Talk(float flDuration);

    BOOL CanFollow(void);
    BOOL IsFollowing(void) { return m_hTargetEnt != NULL && m_hTargetEnt->IsPlayer(); }
    void StopFollowing(BOOL clearSchedule);
    void StartFollowing(CBaseEntity *pLeader);
    virtual void DeclineFollowing(void) { }
    void LimitFollowers(CBaseEntity *pPlayer, int maxFollowers);

    void EXPORT FollowerUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

    virtual void SetAnswerQuestion(CTalkMonster *pSpeaker);
    virtual int FriendNumber(int arrayNumber) { return arrayNumber; }

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    static char *m_szFriends[TLK_CFRIENDS];
    static float g_talkWaitTime;

    int m_bitsSaid;                     /* 0x2A0 */
    int m_nSpeak;                       /* 0x2A4 */
    int m_voicePitch;                   /* 0x2A8 */
    const char *m_szGrp[TLK_CGROUPS];   /* 0x2AC */
    float m_useTime;                    /* 0x2F4 */
    int m_iszUse;                       /* 0x2F8 */
    int m_iszUnUse;                     /* 0x2FC */
    float m_flLastSaidSmelled;          /* 0x300 */
    float m_flStopTalkTime;             /* 0x304 */
    EHANDLE m_hTalkTarget;              /* 0x308 */

    CUSTOM_SCHEDULES;
};

#endif
