#ifndef DLLS_BASEMONSTER_H
#define DLLS_BASEMONSTER_H

/* basemonster.h do HLSDK. Virtuais novas: Look = slot 60 ... StopFollowing = 126,
   mais uma extra do PS2 no slot 127. Campos conferidos pelos offsets do zombie. */

#include "dlls/activity.h"

class CSound;
class CCineMonster;

#define ROUTE_SIZE          8
#define MAX_OLD_ENEMIES     4
#define CDMG_TIMEBASED      8

class CBaseMonster : public CBaseToggle
{
private:
    int m_afConditions;                     /* 0xD0 */

public:
    typedef enum
    {
        SCRIPT_PLAYING = 0,
        SCRIPT_WAIT,
        SCRIPT_CLEANUP,
        SCRIPT_WALK_TO_MARK,
        SCRIPT_RUN_TO_MARK,
    } SCRIPTSTATE;

    EHANDLE m_hEnemy;                       /* 0xD4 */
    EHANDLE m_hTargetEnt;                   /* 0xDC */
    EHANDLE m_hOldEnemy[MAX_OLD_ENEMIES];   /* 0xE4 */
    Vector m_vecOldEnemy[MAX_OLD_ENEMIES];  /* 0x104 */

    float m_flFieldOfView;                  /* 0x134 */
    float m_flWaitFinished;                 /* 0x138 */
    float m_flMoveWaitFinished;             /* 0x13C */

    Activity m_Activity;                    /* 0x140 */
    Activity m_IdealActivity;               /* 0x144 */

    int m_LastHitGroup;                     /* 0x148 */

    MONSTERSTATE m_MonsterState;            /* 0x14C */
    MONSTERSTATE m_IdealMonsterState;       /* 0x150 */

    int m_iTaskStatus;                      /* 0x154 */
    Schedule_t *m_pSchedule;                /* 0x158 */
    int m_iScheduleIndex;                   /* 0x15C */

    WayPoint_t m_Route[ROUTE_SIZE];         /* 0x160 */
    int m_movementGoal;                     /* 0x1E0 */
    int m_iRouteIndex;                      /* 0x1E4 */
    float m_moveWaitTime;                   /* 0x1E8 */

    Vector m_vecMoveGoal;                   /* 0x1EC */
    Activity m_movementActivity;            /* 0x1F8 */

    int m_iAudibleList;                     /* 0x1FC */
    int m_afSoundTypes;                     /* 0x200 */

    Vector m_vecLastPosition;               /* 0x204 */

    int m_iHintNode;                        /* 0x210 */

    int m_afMemory;                         /* 0x214 */

    int m_iMaxHealth;                       /* 0x218 */

    Vector m_vecEnemyLKP;                   /* 0x21C */

    int m_cAmmoLoaded;                      /* 0x228 */

    int m_afCapability;                     /* 0x22C */

    float m_flNextAttack;                   /* 0x230 */

    int m_bitsDamageType;                   /* 0x234 */
    BYTE m_rgbTimeBasedDamage[CDMG_TIMEBASED]; /* 0x238 */

    int m_lastDamageAmount;                 /* 0x240 */

    int m_bloodColor;                       /* 0x244 */

    virtual int BloodColor(void) { return m_bloodColor; }
    virtual CBaseMonster *MyMonsterPointer(void) { return this; }

    int m_failSchedule;                     /* 0x248 */

    float m_flHungryTime;                   /* 0x24C */
    float m_flDistTooFar;                   /* 0x250 */
    float m_flDistLook;                     /* 0x254 */

    int m_iTriggerCondition;                /* 0x258 */
    string_t m_iszTriggerTarget;            /* 0x25C */

    Vector m_HackedGunPos;                  /* 0x260 */

    SCRIPTSTATE m_scriptState;              /* 0x26C */
    CCineMonster *m_pCine;                  /* 0x270 */

    int m_ps2Unk274[11];                    /* 0x274: campos extras do PS2 (a ver) */

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];

    void KeyValue(KeyValueData *pkvd);

    virtual void Look(int iDistance);
    virtual void RunAI(void);
    void Listen(void);

    virtual BOOL IsAlive(void) { return (pev->deadflag != DEAD_DEAD); }
    virtual BOOL ShouldFadeOnDeath(void);

    virtual float ChangeYaw(int speed);
    float VecToYaw(Vector vecDir);
    float FlYawDiff(void);

    float DamageForce(float damage);

    virtual void MonsterThink(void);
    void EXPORT CallMonsterThink(void) { this->MonsterThink(); }
    virtual int IRelationship(CBaseEntity *pTarget);
    virtual void MonsterInit(void);
    virtual void MonsterInitDead(void);
    virtual void BecomeDead(void);
    void EXPORT CorpseFallThink(void);

    void EXPORT MonsterInitThink(void);
    virtual void StartMonster(void);
    virtual CBaseEntity *BestVisibleEnemy(void);
    virtual BOOL FInViewCone(CBaseEntity *pEntity);
    virtual BOOL FInViewCone(Vector *pOrigin);
    virtual void HandleAnimEvent(MonsterEvent_t *pEvent);

    virtual int CheckLocalMove(const Vector &vecStart, const Vector &vecEnd, CBaseEntity *pTarget, float *pflDist);
    virtual void Move(float flInterval = 0.1);
    virtual void MoveExecute(CBaseEntity *pTargetEnt, const Vector &vecDir, float flInterval);
    virtual BOOL ShouldAdvanceRoute(float flWaypointDist);

    virtual Activity GetStoppedActivity(void) { return ACT_IDLE; }
    virtual void Stop(void) { m_IdealActivity = GetStoppedActivity(); }

    inline void StopAnimation(void) { pev->framerate = 0; }

    virtual BOOL CheckRangeAttack1(float flDot, float flDist);
    virtual BOOL CheckRangeAttack2(float flDot, float flDist);
    virtual BOOL CheckMeleeAttack1(float flDot, float flDist);
    virtual BOOL CheckMeleeAttack2(float flDot, float flDist);

    BOOL FHaveSchedule(void);
    BOOL FScheduleValid(void);
    void ClearSchedule(void);
    BOOL FScheduleDone(void);
    void ChangeSchedule(Schedule_t *pNewSchedule);
    void NextScheduledTask(void);
    Schedule_t *ScheduleInList(const char *pName, Schedule_t **pList, int listCount);

    virtual Schedule_t *ScheduleFromName(const char *pName);
    static Schedule_t *m_scheduleList[];

    void MaintainSchedule(void);
    virtual void StartTask(Task_t *pTask);
    virtual void RunTask(Task_t *pTask);
    virtual Schedule_t *GetScheduleOfType(int Type);
    virtual Schedule_t *GetSchedule(void);
    virtual void ScheduleChange(void) { }
    virtual int CanPlaySequence(BOOL fDisregardState, int interruptLevel);
    virtual int CanPlaySentence(BOOL fDisregardState) { return IsAlive(); }
    virtual void PlaySentence(const char *pszSentence, float duration, float volume, float attenuation);
    virtual void PlayScriptedSentence(const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener);

    virtual void SentenceStop(void);

    Task_t *GetTask(void);
    virtual MONSTERSTATE GetIdealState(void);
    virtual void SetActivity(Activity NewActivity);
    void SetSequenceByName(char *szSequence);
    void SetState(MONSTERSTATE State);
    virtual void ReportAIState(void);

    void CheckAttacks(CBaseEntity *pTarget, float flDist);
    virtual int CheckEnemy(CBaseEntity *pEnemy);
    void PushEnemy(CBaseEntity *pEnemy, Vector &vecLastKnownPos);
    BOOL PopEnemy(void);

    BOOL FGetNodeRoute(Vector vecDest);

    inline void TaskComplete(void) { if (!HasConditions(bits_COND_TASK_FAILED)) m_iTaskStatus = 2; }
    void MovementComplete(void);
    inline void TaskFail(void) { SetConditions(bits_COND_TASK_FAILED); }
    inline void TaskBegin(void) { m_iTaskStatus = 1; }
    int TaskIsRunning(void);
    inline int TaskIsComplete(void) { return (m_iTaskStatus == 2); }
    inline int MovementIsComplete(void) { return (m_movementGoal == 0); }

    int IScheduleFlags(void);
    BOOL FRefreshRoute(void);
    BOOL FRouteClear(void);
    void RouteSimplify(CBaseEntity *pTargetEnt);
    void AdvanceRoute(float distance);
    virtual BOOL FTriangulate(const Vector &vecStart, const Vector &vecEnd, float flDist, CBaseEntity *pTargetEnt, Vector *pApex);
    void MakeIdealYaw(Vector vecTarget);
    virtual void SetYawSpeed(void) { return; }
    BOOL BuildRoute(const Vector &vecGoal, int iMoveFlag, CBaseEntity *pTarget);
    virtual BOOL BuildNearestRoute(Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist);
    int RouteClassify(int iMoveFlag);
    void InsertWaypoint(Vector vecLocation, int afMoveFlags);

    BOOL FindLateralCover(const Vector &vecThreat, const Vector &vecViewOffset);
    virtual BOOL FindCover(Vector vecThreat, Vector vecViewOffset, float flMinDist, float flMaxDist);
    virtual BOOL FValidateCover(const Vector &vecCoverLocation) { return TRUE; }
    virtual float CoverRadius(void) { return 384; }

    virtual BOOL FCanCheckAttacks(void);
    virtual void CheckAmmo(void) { return; }
    virtual int IgnoreConditions(void);

    inline void SetConditions(int iConditions) { m_afConditions |= iConditions; }
    inline void ClearConditions(int iConditions) { m_afConditions &= ~iConditions; }
    inline BOOL HasConditions(int iConditions)
    {
        if (m_afConditions & iConditions)
            return TRUE;
        return FALSE;
    }
    inline BOOL HasAllConditions(int iConditions)
    {
        if ((m_afConditions & iConditions) == iConditions)
            return TRUE;
        return FALSE;
    }

    virtual BOOL FValidateHintType(short sHint);
    int FindHintNode(void);
    virtual BOOL FCanActiveIdle(void);
    void SetTurnActivity(void);
    float FLSoundVolume(CSound *pSound);

    BOOL MoveToNode(Activity movementAct, float waitTime, const Vector &goal);
    BOOL MoveToTarget(Activity movementAct, float waitTime);
    BOOL MoveToLocation(Activity movementAct, float waitTime, const Vector &goal);
    BOOL MoveToEnemy(Activity movementAct, float waitTime);

    float OpenDoorAndWait(entvars_t *pevDoor);

    virtual int ISoundMask(void);
    virtual CSound *PBestSound(void);
    virtual CSound *PBestScent(void);
    virtual float HearingSensitivity(void) { return 1.0; }

    BOOL FBecomeProne(void);
    virtual void BarnacleVictimBitten(entvars_t *pevBarnacle);
    virtual void BarnacleVictimReleased(void);

    void SetEyePosition(void);

    BOOL FShouldEat(void);
    void Eat(float flFullDuration);

    CBaseEntity *CheckTraceHullAttack(float flDist, int iDamage, int iDmgType);
    BOOL FacingIdeal(void);

    BOOL FCheckAITrigger(void);

    BOOL NoFriendlyFire(void);

    BOOL BBoxFlat(void);

    virtual void PrescheduleThink(void) { return; }

    BOOL GetEnemy(void);
    void MakeDamageBloodDecal(int cCount, float flNoise, TraceResult *ptr, const Vector &vecDir);
    void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);

    float UpdateTarget(entvars_t *pevTarget);
    virtual Activity GetDeathActivity(void);
    Activity GetSmallFlinchActivity(void);
    virtual void Killed(entvars_t *pevAttacker, int iGib);
    virtual void GibMonster(void);
    BOOL ShouldGibMonster(int iGib);
    void CallGibMonster(void);
    virtual BOOL HasHumanGibs(void);
    virtual BOOL HasAlienGibs(void);
    virtual void FadeMonster(void);

    Vector ShootAtEnemy(const Vector &shootOrigin);
    virtual Vector BodyTarget(const Vector &posSrc) { return Center() * 0.75 + EyePosition() * 0.25; }

    virtual Vector GetGunPosition(void);

    virtual int TakeHealth(float flHealth, int bitsDamageType);
    virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    int DeadTakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

    void RadiusDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType);
    void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int iClassIgnore, int bitsDamageType);
    virtual int IsMoving(void) { return m_movementGoal != 0; }

    void RouteClear(void);
    void RouteNew(void);

    virtual void DeathSound(void) { return; }
    virtual void AlertSound(void) { return; }
    virtual void IdleSound(void) { return; }
    virtual void PainSound(void) { return; }

    virtual void StopFollowing(BOOL clearSchedule) { }

    virtual void Ps2Unk127(void);       /* virtual extra do PS2 (Ps2Unk127__12CBaseMonster) */

    inline void Remember(int iMemory) { m_afMemory |= iMemory; }
    inline void Forget(int iMemory) { m_afMemory &= ~iMemory; }
    inline BOOL HasMemory(int iMemory)
    {
        if (m_afMemory & iMemory)
            return TRUE;
        return FALSE;
    }
    inline BOOL HasAllMemories(int iMemory)
    {
        if ((m_afMemory & iMemory) == iMemory)
            return TRUE;
        return FALSE;
    }

    BOOL ExitScriptedSequence();
    BOOL CineCleanup();

    CBaseEntity *DropItem(char *pszItemName, const Vector &vecPos, const Vector &vecAng);
};

#endif
