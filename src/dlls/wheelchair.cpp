/* dlls/wheelchair.cpp: o Dr. Keller na cadeira de rodas (monster_wheelchair)
   do Half-Life: Decay. É o CScientist do HLSDK com as falas "KL_", sons
   próprios e sem a seringa. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/talkmonster.h"
#include "dlls/schedule.h"
#include "dlls/defaultai.h"
#include "dlls/soundent.h"

enum
{
    SCHED_HIDE = LAST_TALKMONSTER_SCHEDULE + 1,
    SCHED_FEAR,
    SCHED_PANIC,
    SCHED_STARTLE,
    SCHED_TARGET_CHASE_SCARED,
    SCHED_TARGET_FACE_SCARED,
};

enum
{
    TASK_SAY_HEAL = LAST_TALKMONSTER_TASK + 1,
    TASK_HEAL,
    TASK_SAY_FEAR,
    TASK_RUN_PATH_SCARED,
    TASK_SCREAM,
    TASK_RANDOM_SCREAM,
    TASK_MOVE_TO_TARGET_RANGE_SCARED,
};

class CWheelchair : public CTalkMonster
{
public:
    void Spawn(void);
    void Precache(void);

    void SetYawSpeed(void);
    int Classify(void);
    void RunTask(Task_t *pTask);
    void StartTask(Task_t *pTask);
    int ObjectCaps(void) { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
    virtual int FriendNumber(int arrayNumber);
    void SetActivity(Activity newActivity);
    Activity GetStoppedActivity(void);
    int ISoundMask(void);
    void DeclineFollowing(void);

    float CoverRadius(void) { return 1200; }
    BOOL DisregardEnemy(CBaseEntity *pEnemy) { return !pEnemy->IsAlive() || (gpGlobals->time - m_fearTime) > 15; }

    BOOL CanHeal(void);
    void Heal(void);
    void Scream(void);

    Schedule_t *GetScheduleOfType(int Type);
    Schedule_t *GetSchedule(void);
    MONSTERSTATE GetIdealState(void);

    void DeathSound(void);
    void PainSound(void);

    void TalkInit(void);

    void Killed(entvars_t *pevAttacker, int iGib);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    CUSTOM_SCHEDULES;

private:
    float m_painTime;
    float m_healTime;
    float m_fearTime;
};

LINK_ENTITY_TO_CLASS(monster_wheelchair, CWheelchair);

TYPEDESCRIPTION CWheelchair::m_SaveData[] =
{
    DEFINE_FIELD(CWheelchair, m_painTime, FIELD_TIME),
    DEFINE_FIELD(CWheelchair, m_healTime, FIELD_TIME),
    DEFINE_FIELD(CWheelchair, m_fearTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CWheelchair, CTalkMonster);

Task_t tlFollow[] =
{
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_CANT_FOLLOW },
    { TASK_MOVE_TO_TARGET_RANGE, (float)128 },
};

Schedule_t slFollow[] =
{
    {
        tlFollow,
        ARRAYSIZE(tlFollow),
        bits_COND_NEW_ENEMY |
        bits_COND_LIGHT_DAMAGE |
        bits_COND_HEAVY_DAMAGE |
        bits_COND_HEAR_SOUND,
        bits_SOUND_COMBAT |
        bits_SOUND_DANGER,
        "Follow"
    },
};

Task_t tlFollowScared[] =
{
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_TARGET_CHASE },
    { TASK_MOVE_TO_TARGET_RANGE_SCARED, (float)128 },
};

Schedule_t slFollowScared[] =
{
    {
        tlFollowScared,
        ARRAYSIZE(tlFollowScared),
        bits_COND_NEW_ENEMY |
        bits_COND_HEAR_SOUND |
        bits_COND_LIGHT_DAMAGE |
        bits_COND_HEAVY_DAMAGE,
        bits_SOUND_DANGER,
        "FollowScared"
    },
};

Task_t tlFaceTargetScared[] =
{
    { TASK_FACE_TARGET, (float)0 },
    { TASK_SET_ACTIVITY, (float)ACT_CROUCHIDLE },
    { TASK_SET_SCHEDULE, (float)SCHED_TARGET_CHASE_SCARED },
};

Schedule_t slFaceTargetScared[] =
{
    {
        tlFaceTargetScared,
        ARRAYSIZE(tlFaceTargetScared),
        bits_COND_HEAR_SOUND |
        bits_COND_NEW_ENEMY,
        bits_SOUND_DANGER,
        "FaceTargetScared"
    },
};

Task_t tlStopFollowing[] =
{
    { TASK_CANT_FOLLOW, (float)0 },
};

Schedule_t slStopFollowing[] =
{
    {
        tlStopFollowing,
        ARRAYSIZE(tlStopFollowing),
        0,
        0,
        "StopFollowing"
    },
};

Task_t tlHeal[] =
{
    { TASK_MOVE_TO_TARGET_RANGE, (float)50 },
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_TARGET_CHASE },
    { TASK_FACE_IDEAL, (float)0 },
    { TASK_SAY_HEAL, (float)0 },
    { TASK_PLAY_SEQUENCE_FACE_TARGET, (float)ACT_ARM },
    { TASK_HEAL, (float)0 },
    { TASK_PLAY_SEQUENCE_FACE_TARGET, (float)ACT_DISARM },
};

Schedule_t slHeal[] =
{
    {
        tlHeal,
        ARRAYSIZE(tlHeal),
        0,
        0,
        "Heal"
    },
};

Task_t tlFaceTarget[] =
{
    { TASK_STOP_MOVING, (float)0 },
    { TASK_FACE_TARGET, (float)0 },
    { TASK_SET_ACTIVITY, (float)ACT_IDLE },
    { TASK_SET_SCHEDULE, (float)SCHED_TARGET_CHASE },
};

Schedule_t slFaceTarget[] =
{
    {
        tlFaceTarget,
        ARRAYSIZE(tlFaceTarget),
        bits_COND_CLIENT_PUSH |
        bits_COND_NEW_ENEMY |
        bits_COND_HEAR_SOUND,
        bits_SOUND_COMBAT |
        bits_SOUND_DANGER,
        "FaceTarget"
    },
};

Task_t tlSciPanic[] =
{
    { TASK_STOP_MOVING, (float)0 },
    { TASK_FACE_ENEMY, (float)0 },
    { TASK_SCREAM, (float)0 },
    { TASK_PLAY_SEQUENCE_FACE_ENEMY, (float)ACT_EXCITED },
    { TASK_SET_ACTIVITY, (float)ACT_IDLE },
};

Schedule_t slSciPanic[] =
{
    {
        tlSciPanic,
        ARRAYSIZE(tlSciPanic),
        0,
        0,
        "SciPanic"
    },
};

Task_t tlIdleSciStand[] =
{
    { TASK_STOP_MOVING, 0 },
    { TASK_SET_ACTIVITY, (float)ACT_IDLE },
    { TASK_WAIT, (float)2 },
    { TASK_TLK_HEADRESET, (float)0 },
};

Schedule_t slIdleSciStand[] =
{
    {
        tlIdleSciStand,
        ARRAYSIZE(tlIdleSciStand),
        bits_COND_NEW_ENEMY |
        bits_COND_LIGHT_DAMAGE |
        bits_COND_HEAVY_DAMAGE |
        bits_COND_HEAR_SOUND |
        bits_COND_SMELL |
        bits_COND_CLIENT_PUSH |
        bits_COND_PROVOKED,

        bits_SOUND_COMBAT |
        bits_SOUND_DANGER |
        bits_SOUND_MEAT |
        bits_SOUND_CARCASS |
        bits_SOUND_GARBAGE,
        "IdleSciStand"

    },
};

Task_t tlScientistCover[] =
{
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_PANIC },
    { TASK_STOP_MOVING, (float)0 },
    { TASK_FIND_COVER_FROM_ENEMY, (float)0 },
    { TASK_RUN_PATH_SCARED, (float)0 },
    { TASK_TURN_LEFT, (float)179 },
    { TASK_SET_SCHEDULE, (float)SCHED_HIDE },
};

Schedule_t slScientistCover[] =
{
    {
        tlScientistCover,
        ARRAYSIZE(tlScientistCover),
        bits_COND_NEW_ENEMY,
        0,
        "ScientistCover"
    },
};

Task_t tlScientistHide[] =
{
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_PANIC },
    { TASK_STOP_MOVING, (float)0 },
    { TASK_PLAY_SEQUENCE, (float)ACT_CROUCH },
    { TASK_SET_ACTIVITY, (float)ACT_CROUCHIDLE },
    { TASK_WAIT_RANDOM, (float)10.0 },
};

Schedule_t slScientistHide[] =
{
    {
        tlScientistHide,
        ARRAYSIZE(tlScientistHide),
        bits_COND_NEW_ENEMY |
        bits_COND_HEAR_SOUND |
        bits_COND_SEE_ENEMY |
        bits_COND_SEE_HATE |
        bits_COND_SEE_FEAR |
        bits_COND_SEE_DISLIKE,
        bits_SOUND_DANGER,
        "ScientistHide"
    },
};

Task_t tlScientistStartle[] =
{
    { TASK_SET_FAIL_SCHEDULE, (float)SCHED_PANIC },
    { TASK_RANDOM_SCREAM, (float)0.3 },
    { TASK_STOP_MOVING, (float)0 },
    { TASK_PLAY_SEQUENCE_FACE_ENEMY, (float)ACT_CROUCH },
    { TASK_RANDOM_SCREAM, (float)0.1 },
    { TASK_PLAY_SEQUENCE_FACE_ENEMY, (float)ACT_CROUCHIDLE },
    { TASK_WAIT_RANDOM, (float)1.0 },
};

Schedule_t slScientistStartle[] =
{
    {
        tlScientistStartle,
        ARRAYSIZE(tlScientistStartle),
        bits_COND_NEW_ENEMY |
        bits_COND_SEE_ENEMY |
        bits_COND_SEE_HATE |
        bits_COND_SEE_FEAR |
        bits_COND_SEE_DISLIKE,
        0,
        "ScientistStartle"
    },
};

Task_t tlFear[] =
{
    { TASK_STOP_MOVING, (float)0 },
    { TASK_FACE_ENEMY, (float)0 },
    { TASK_SAY_FEAR, (float)0 },
};

Schedule_t slFear[] =
{
    {
        tlFear,
        ARRAYSIZE(tlFear),
        bits_COND_NEW_ENEMY,
        0,
        "Fear"
    },
};

DEFINE_CUSTOM_SCHEDULES(CWheelchair)
{
    slFollow,
    slFaceTarget,
    slIdleSciStand,
    slFear,
    slScientistCover,
    slScientistHide,
    slScientistStartle,
    slHeal,
    slStopFollowing,
    slSciPanic,
    slFollowScared,
    slFaceTargetScared,
};

IMPLEMENT_CUSTOM_SCHEDULES(CWheelchair, CTalkMonster);

void CWheelchair::DeclineFollowing(void)
{
    Talk(10);
    m_hTalkTarget = m_hEnemy;
    PlaySentence("KL_POK", 2, VOL_NORM, ATTN_NORM);
}

void CWheelchair::Scream(void)
{
    if (FOkToSpeak())
    {
        Talk(10);
        m_hTalkTarget = m_hEnemy;
        PlaySentence("KL_SCREAM", RANDOM_FLOAT(3, 6), VOL_NORM, ATTN_NORM);
    }
}

Activity CWheelchair::GetStoppedActivity(void)
{
    if (m_hEnemy != NULL)
        return ACT_EXCITED;
    return CTalkMonster::GetStoppedActivity();
}

void CWheelchair::StartTask(Task_t *pTask)
{
    switch (pTask->iTask)
    {
    case TASK_SAY_HEAL:
        Talk(2);
        m_hTalkTarget = m_hTargetEnt;
        PlaySentence("KL_HEAL", 2, VOL_NORM, ATTN_IDLE);
        TaskComplete();
        break;

    case TASK_SCREAM:
        Scream();
        TaskComplete();
        break;

    case TASK_RANDOM_SCREAM:
        if (RANDOM_FLOAT(0, 1) < pTask->flData)
            Scream();
        TaskComplete();
        break;

    case TASK_SAY_FEAR:
        if (FOkToSpeak())
        {
            Talk(2);
            m_hTalkTarget = m_hEnemy;
            if (m_hEnemy->IsPlayer())
                PlaySentence("KL_PLFEAR", 5, VOL_NORM, ATTN_NORM);
            else
                PlaySentence("KL_FEAR", 5, VOL_NORM, ATTN_NORM);
        }
        TaskComplete();
        break;

    case TASK_HEAL:
        m_IdealActivity = ACT_MELEE_ATTACK1;
        break;

    case TASK_RUN_PATH_SCARED:
        m_movementActivity = ACT_RUN_SCARED;
        break;

    case TASK_MOVE_TO_TARGET_RANGE_SCARED:
    {
        if ((m_hTargetEnt->pev->origin - pev->origin).Length() < 1)
            TaskComplete();
        else
        {
            m_vecMoveGoal = m_hTargetEnt->pev->origin;
            if (!MoveToTarget(ACT_WALK_SCARED, 0.5))
                TaskFail();
        }
    }
    break;

    default:
        CTalkMonster::StartTask(pTask);
        break;
    }
}

void CWheelchair::RunTask(Task_t *pTask)
{
    switch (pTask->iTask)
    {
    case TASK_RUN_PATH_SCARED:
        if (MovementIsComplete())
            TaskComplete();
        if (RANDOM_LONG(0, 31) < 8)
            Scream();
        break;

    case TASK_MOVE_TO_TARGET_RANGE_SCARED:
    {
        if (RANDOM_LONG(0, 63) < 8)
            Scream();

        if (m_hEnemy == NULL)
        {
            TaskFail();
        }
        else
        {
            float distance;

            distance = (m_vecMoveGoal - pev->origin).Length2D();
            if ((distance < pTask->flData) || (m_vecMoveGoal - m_hTargetEnt->pev->origin).Length() > pTask->flData * 0.5f)
            {
                m_vecMoveGoal = m_hTargetEnt->pev->origin;
                distance = (m_vecMoveGoal - pev->origin).Length2D();
                FRefreshRoute();
            }

            if (distance < pTask->flData)
            {
                TaskComplete();
                RouteClear();
            }
            else if (distance < 190 && m_movementActivity != ACT_WALK_SCARED)
                m_movementActivity = ACT_WALK_SCARED;
            else if (distance >= 270 && m_movementActivity != ACT_RUN_SCARED)
                m_movementActivity = ACT_RUN_SCARED;
        }
    }
    break;

    case TASK_HEAL:
        if (m_fSequenceFinished)
        {
            TaskComplete();
        }
        else
        {
            if (TargetDistance() > 90)
                TaskComplete();
            pev->ideal_yaw = UTIL_VecToYaw(m_hTargetEnt->pev->origin - pev->origin);
            ChangeYaw(pev->yaw_speed);
        }
        break;

    default:
        CTalkMonster::RunTask(pTask);
        break;
    }
}

int CWheelchair::Classify(void)
{
    return CLASS_HUMAN_PASSIVE;
}

void CWheelchair::SetYawSpeed(void)
{
    int ys;

    ys = 90;

    switch (m_Activity)
    {
    case ACT_IDLE:
        ys = 120;
        break;
    case ACT_WALK:
        ys = 180;
        break;
    case ACT_RUN:
        ys = 150;
        break;
    case ACT_TURN_LEFT:
    case ACT_TURN_RIGHT:
        ys = 120;
        break;
    }

    pev->yaw_speed = ys;
}

void CWheelchair::Spawn(void)
{
    Precache();

    SET_MODEL(ENT(pev), "models/wheelchair_sci.dol");
    UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

    pev->solid = SOLID_SLIDEBOX;
    pev->movetype = MOVETYPE_STEP;
    m_bloodColor = BLOOD_COLOR_RED;
    pev->health = gSkillData.scientistHealth * 2;
    pev->view_ofs = Vector(0, 0, 50);
    m_flFieldOfView = VIEW_FIELD_WIDE;
    m_MonsterState = MONSTERSTATE_NONE;

    m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE;

    pev->skin = 0;
    pev->body = 0;

    MonsterInit();

    if (!(pev->spawnflags & (SF_MONSTER_PREDISASTER | 0x10000)))
        SetUse(&CTalkMonster::FollowerUse);
    else
        SetUse(NULL);
}

void CWheelchair::Precache(void)
{
    PRECACHE_MODEL("models/wheelchair_sci.dol");
    PRECACHE_SOUND("wheelchair/kl_pain1.wav");
    PRECACHE_SOUND("wheelchair/kl_pain2.wav");
    PRECACHE_SOUND("wheelchair/kl_pain3.wav");
    PRECACHE_SOUND("wheelchair/kl_pain4.wav");
    PRECACHE_SOUND("wheelchair/kl_pain5.wav");
    PRECACHE_SOUND("wheelchair/wheelchair_walk.wav");
    PRECACHE_SOUND("wheelchair/wheelchair_jog.wav");
    PRECACHE_SOUND("wheelchair/wheelchair_run.wav");

    TalkInit();
    CTalkMonster::Precache();
}

void CWheelchair::TalkInit()
{
    CTalkMonster::TalkInit();

    m_szFriends[0] = "monster_scientist";
    m_szFriends[1] = "monster_sitting_scientist";
    m_szFriends[2] = "monster_barney";

    m_szGrp[TLK_ANSWER] = "KL_ANSWER";
    m_szGrp[TLK_QUESTION] = "KL_QUESTION";
    m_szGrp[TLK_IDLE] = "KL_IDLE";
    m_szGrp[TLK_STARE] = "KL_STARE";
    m_szGrp[TLK_USE] = "KL_OK";
    m_szGrp[TLK_UNUSE] = "KL_WAIT";
    m_szGrp[TLK_STOP] = "KL_STOP";
    m_szGrp[TLK_NOSHOOT] = "KL_SCARED";
    m_szGrp[TLK_HELLO] = "KL_HELLO";

    m_szGrp[TLK_PLHURT1] = "!KL_CUREA";
    m_szGrp[TLK_PLHURT2] = "!KL_CUREB";
    m_szGrp[TLK_PLHURT3] = "!KL_CUREC";

    m_szGrp[TLK_PHELLO] = "KL_PHELLO";
    m_szGrp[TLK_PIDLE] = "KL_PIDLE";
    m_szGrp[TLK_PQUESTION] = "KL_PQUEST";
    m_szGrp[TLK_SMELL] = "KL_SMELL";

    m_szGrp[TLK_WOUND] = "KL_WOUND";
    m_szGrp[TLK_MORTAL] = "KL_MORTAL";

    m_voicePitch = 100;
}

int CWheelchair::ISoundMask(void)
{
    return bits_SOUND_WORLD |
           bits_SOUND_COMBAT |
           bits_SOUND_DANGER |
           bits_SOUND_PLAYER;
}

void CWheelchair::PainSound(void)
{
    if (gpGlobals->time < m_painTime)
        return;

    m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);

    switch (RANDOM_LONG(0, 4))
    {
    case 0: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "wheelchair/kl_pain1.wav", 1, ATTN_NORM, 0, 100); break;
    case 1: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "wheelchair/kl_pain2.wav", 1, ATTN_NORM, 0, 100); break;
    case 2: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "wheelchair/kl_pain3.wav", 1, ATTN_NORM, 0, 100); break;
    case 3: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "wheelchair/kl_pain4.wav", 1, ATTN_NORM, 0, 100); break;
    case 4: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "wheelchair/kl_pain5.wav", 1, ATTN_NORM, 0, 100); break;
    }
}

void CWheelchair::DeathSound(void)
{
    PainSound();
}

void CWheelchair::Killed(entvars_t *pevAttacker, int iGib)
{
    SetUse(NULL);
    CTalkMonster::Killed(pevAttacker, iGib);
}

void CWheelchair::SetActivity(Activity newActivity)
{
    int iSequence;

    iSequence = LookupActivity(newActivity);

    if (iSequence == ACTIVITY_NOT_AVAILABLE)
        newActivity = ACT_IDLE;
    CTalkMonster::SetActivity(newActivity);
}

Schedule_t *CWheelchair::GetScheduleOfType(int Type)
{
    Schedule_t *psched;

    switch (Type)
    {
    case SCHED_TARGET_FACE:
        psched = CTalkMonster::GetScheduleOfType(Type);

        if (psched == slIdleStand)
            return slFaceTarget;
        else
            return psched;

    case SCHED_TARGET_CHASE:
        return slFollow;

    case SCHED_CANT_FOLLOW:
        return slStopFollowing;

    case SCHED_PANIC:
        return slSciPanic;

    case SCHED_TARGET_CHASE_SCARED:
        return slFollowScared;

    case SCHED_TARGET_FACE_SCARED:
        return slFaceTargetScared;

    case SCHED_IDLE_STAND:
        psched = CTalkMonster::GetScheduleOfType(Type);

        if (psched == slIdleStand)
            return slIdleSciStand;
        else
            return psched;

    case SCHED_HIDE:
        return slScientistHide;

    case SCHED_STARTLE:
        return slScientistStartle;

    case SCHED_FEAR:
        return slFear;
    }

    return CTalkMonster::GetScheduleOfType(Type);
}

inline BOOL CBaseMonster::HasConditions(int iConditions)
{
    if (m_afConditions & iConditions)
        return TRUE;
    return FALSE;
}

Schedule_t *CWheelchair::GetSchedule(void)
{
    CBaseEntity *pEnemy = m_hEnemy;

    if (HasConditions(bits_COND_HEAR_SOUND))
    {
        CSound *pSound;
        pSound = PBestSound();

        if (pSound && (pSound->m_iType & bits_SOUND_DANGER))
            return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
    }

    switch (m_MonsterState)
    {
    case MONSTERSTATE_ALERT:
    case MONSTERSTATE_IDLE:
        if (pEnemy)
        {
            if (HasConditions(bits_COND_SEE_ENEMY))
                m_fearTime = gpGlobals->time;
            else if (DisregardEnemy(pEnemy))
            {
                m_hEnemy = NULL;
                pEnemy = NULL;
            }
        }

        if (HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
        {
            return GetScheduleOfType(SCHED_SMALL_FLINCH);
        }

        if (HasConditions(bits_COND_HEAR_SOUND))
        {
            CSound *pSound;
            pSound = PBestSound();

            if (pSound)
            {
                if (pSound->m_iType & (bits_SOUND_DANGER | bits_SOUND_COMBAT))
                {
                    if (gpGlobals->time - m_fearTime > 3)
                    {
                        m_fearTime = gpGlobals->time;
                        return GetScheduleOfType(SCHED_STARTLE);
                    }
                }
            }
        }

        if (IsFollowing())
        {
            if (!m_hTargetEnt->IsAlive())
            {
                StopFollowing(FALSE);
                break;
            }

            int relationship = R_NO;

            if (pEnemy != NULL)
                relationship = IRelationship(pEnemy);

            if (relationship != R_DL && relationship != R_HT)
            {
                if (TargetDistance() <= 128)
                {
                    if (CanHeal())
                        return slHeal;
                    if (HasConditions(bits_COND_CLIENT_PUSH))
                        return GetScheduleOfType(SCHED_MOVE_AWAY_FOLLOW);
                }
                return GetScheduleOfType(SCHED_TARGET_FACE);
            }
            else
            {
                if (HasConditions(bits_COND_NEW_ENEMY))
                    return GetScheduleOfType(SCHED_FEAR);
                return GetScheduleOfType(SCHED_TARGET_FACE_SCARED);
            }
        }

        if (HasConditions(bits_COND_CLIENT_PUSH))
            return GetScheduleOfType(SCHED_MOVE_AWAY);

        TrySmellTalk();
        break;
    case MONSTERSTATE_COMBAT:
        if (HasConditions(bits_COND_NEW_ENEMY))
            return slFear;
        if (HasConditions(bits_COND_SEE_ENEMY))
            return slScientistCover;

        if (HasConditions(bits_COND_HEAR_SOUND))
            return slTakeCoverFromBestSound;

        return slScientistCover;
        break;
    }

    return CTalkMonster::GetSchedule();
}

MONSTERSTATE CWheelchair::GetIdealState(void)
{
    switch (m_MonsterState)
    {
    case MONSTERSTATE_ALERT:
    case MONSTERSTATE_IDLE:
        if (HasConditions(bits_COND_NEW_ENEMY))
        {
            if (IsFollowing())
            {
                int relationship = IRelationship(m_hEnemy);
                if (relationship != R_FR || relationship != R_HT && !HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
                {
                    m_IdealMonsterState = MONSTERSTATE_ALERT;
                    return m_IdealMonsterState;
                }
                StopFollowing(TRUE);
            }
        }
        else if (HasConditions(bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE))
        {
            if (IsFollowing())
                StopFollowing(TRUE);
        }
        break;

    case MONSTERSTATE_COMBAT:
    {
        CBaseEntity *pEnemy = m_hEnemy;
        if (pEnemy != NULL)
        {
            if (DisregardEnemy(pEnemy))
            {
                m_IdealMonsterState = MONSTERSTATE_ALERT;
                m_hEnemy = NULL;
                return m_IdealMonsterState;
            }
            if (m_hTargetEnt != NULL)
            {
                m_IdealMonsterState = MONSTERSTATE_ALERT;
                return m_IdealMonsterState;
            }

            if (HasConditions(bits_COND_SEE_ENEMY))
            {
                m_fearTime = gpGlobals->time;
                m_IdealMonsterState = MONSTERSTATE_COMBAT;
                return m_IdealMonsterState;
            }
        }
    }
    break;
    }

    return CTalkMonster::GetIdealState();
}

BOOL CWheelchair::CanHeal(void)
{
    if ((m_healTime > gpGlobals->time) || (m_hTargetEnt == NULL) || (m_hTargetEnt->pev->health > (m_hTargetEnt->pev->max_health * 0.5f)))
        return FALSE;

    return TRUE;
}

void CWheelchair::Heal(void)
{
    if (!CanHeal())
        return;

    Vector target = m_hTargetEnt->pev->origin - pev->origin;
    if (target.Length() > 100)
        return;

    m_hTargetEnt->TakeHealth(gSkillData.scientistHeal, DMG_GENERIC);
    m_healTime = gpGlobals->time + 60;
}

int CWheelchair::FriendNumber(int arrayNumber)
{
    static int array[3] = { 1, 2, 0 };
    if (arrayNumber < 3)
        return array[arrayNumber];
    return arrayNumber;
}

/* registro dos ponteiros de método usados neste arquivo (chamado por
   func_00239870 no MethodPointerTable.cpp) */
extern "C" int func_00239810(const char *name, USEPTR pfn);

extern "C" void func_002142F8(void)
{
    func_00239810("345", static_cast<USEPTR>(&CTalkMonster::FollowerUse));
}
