/* dlls/scientist.cpp: os cientistas (monster_scientist,
   monster_scientist_dead e monster_sitting_scientist). Versão do PS2 com
   cabeças por bodygroup, seringa por bodygroup e um campo m_coverTime. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/talkmonster.h"
#include "dlls/schedule.h"
#include "dlls/defaultai.h"
#include "dlls/soundent.h"

#define bit_saidHelloPlayer     (1<<3)
#define bits_MEMORY_PROVOKED    (1<<0)

int SENTENCEG_PlayRndSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch);

#define NUM_SCIENTIST_HEADS 4

enum { HEAD_GLASSES = 0, HEAD_EINSTEIN = 1, HEAD_LUTHER = 2, HEAD_SLICK = 3 };

#define SCIENTIST_AE_HEAL       (1)
#define SCIENTIST_AE_NEEDLEON   (2)
#define SCIENTIST_AE_NEEDLEOFF  (3)

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

class CScientist : public CTalkMonster
{
public:
    void Spawn(void);
    void Precache(void);

    void SetYawSpeed(void);
    int Classify(void);
    void RunTask(Task_t *pTask);
    void StartTask(Task_t *pTask);
    void HandleAnimEvent(MonsterEvent_t *pEvent);
    int ObjectCaps(void) { return CTalkMonster::ObjectCaps() | FCAP_IMPULSE_USE; }
    virtual int FriendNumber(int arrayNumber);
    void SetActivity(Activity newActivity);
    Activity GetStoppedActivity(void);
    int ISoundMask(void);
    void DeclineFollowing(void);

    float CoverRadius(void) { return 512; }
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

    int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    void Killed(entvars_t *pevAttacker, int iGib);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    CUSTOM_SCHEDULES;

private:
    float m_painTime;
    float m_healTime;
    float m_fearTime;
    float m_coverTime;
};

LINK_ENTITY_TO_CLASS(monster_scientist, CScientist);

TYPEDESCRIPTION CScientist::m_SaveData[] =
{
    DEFINE_FIELD(CScientist, m_painTime, FIELD_TIME),
    DEFINE_FIELD(CScientist, m_healTime, FIELD_TIME),
    DEFINE_FIELD(CScientist, m_fearTime, FIELD_TIME),
    DEFINE_FIELD(CScientist, m_coverTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE(CScientist, CTalkMonster);

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

DEFINE_CUSTOM_SCHEDULES(CScientist)
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

IMPLEMENT_CUSTOM_SCHEDULES(CScientist, CTalkMonster);

void CScientist::DeclineFollowing(void)
{
    Talk(10);
    m_hTalkTarget = m_hEnemy;
    PlaySentence("SC_POK", 2, VOL_NORM, ATTN_NORM);
}

void CScientist::Scream(void)
{
    if (FOkToSpeak())
    {
        Talk(10);
        m_hTalkTarget = m_hEnemy;
        PlaySentence("SC_SCREAM", RANDOM_FLOAT(3, 6), VOL_NORM, ATTN_NORM);
    }
}

Activity CScientist::GetStoppedActivity(void)
{
    if (m_hEnemy != NULL)
        return ACT_EXCITED;
    return CTalkMonster::GetStoppedActivity();
}

void CScientist::StartTask(Task_t *pTask)
{
    switch (pTask->iTask)
    {
    case TASK_SAY_HEAL:
        Talk(2);
        m_hTalkTarget = m_hTargetEnt;
        PlaySentence("SC_HEAL", 2, VOL_NORM, ATTN_IDLE);
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
                PlaySentence("SC_PLFEAR", 5, VOL_NORM, ATTN_NORM);
            else
                PlaySentence("SC_FEAR", 5, VOL_NORM, ATTN_NORM);
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

void CScientist::RunTask(Task_t *pTask)
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

int CScientist::Classify(void)
{
    return CLASS_HUMAN_PASSIVE;
}

void CScientist::SetYawSpeed(void)
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

void CScientist::HandleAnimEvent(MonsterEvent_t *pEvent)
{
    switch (pEvent->event)
    {
    case SCIENTIST_AE_HEAL:
        Heal();
        break;
    case SCIENTIST_AE_NEEDLEON:
        SetBodygroup(2, 1);
        break;
    case SCIENTIST_AE_NEEDLEOFF:
        SetBodygroup(2, 0);
        break;

    default:
        CTalkMonster::HandleAnimEvent(pEvent);
    }
}

void CScientist::Spawn(void)
{
    Precache();

    SET_MODEL(ENT(pev), "models/scientist.dol");
    UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

    pev->solid = SOLID_SLIDEBOX;
    pev->movetype = MOVETYPE_STEP;
    m_bloodColor = BLOOD_COLOR_RED;
    pev->health = gSkillData.scientistHealth;
    pev->view_ofs = Vector(0, 0, 50);
    m_flFieldOfView = VIEW_FIELD_WIDE;
    m_MonsterState = MONSTERSTATE_NONE;

    m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_OPEN_DOORS | bits_CAP_AUTO_DOORS | bits_CAP_USE;

    pev->skin = 0;

    if (pev->body == -1)
        SetBodygroup(1, RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1));
    else
        SetBodygroup(1, pev->body);

    if (GetBodygroup(1) == HEAD_LUTHER)
        pev->skin = 1;

    MonsterInit();

    if (!(pev->spawnflags & 0x10000))
        SetUse(&CTalkMonster::FollowerUse);
    else
        SetUse(NULL);
}

void CScientist::Precache(void)
{
    PRECACHE_MODEL("models/scientist.dol");
    PRECACHE_SOUND("scientist/sci_pain1.wav");
    PRECACHE_SOUND("scientist/sci_pain2.wav");
    PRECACHE_SOUND("scientist/sci_pain3.wav");
    PRECACHE_SOUND("scientist/sci_pain4.wav");
    PRECACHE_SOUND("scientist/sci_pain5.wav");

    TalkInit();
    CTalkMonster::Precache();
}

void CScientist::TalkInit()
{
    CTalkMonster::TalkInit();

    m_szFriends[0] = "monster_scientist";
    m_szFriends[1] = "monster_sitting_scientist";
    m_szFriends[2] = "monster_barney";

    m_szGrp[TLK_ANSWER] = "SC_ANSWER";
    m_szGrp[TLK_QUESTION] = "SC_QUESTION";
    m_szGrp[TLK_IDLE] = "SC_IDLE";
    m_szGrp[TLK_STARE] = "SC_STARE";
    m_szGrp[TLK_USE] = "SC_OK";
    m_szGrp[TLK_UNUSE] = "SC_WAIT";
    m_szGrp[TLK_STOP] = "SC_STOP";
    m_szGrp[TLK_NOSHOOT] = "SC_SCARED";
    m_szGrp[TLK_HELLO] = "SC_HELLO";

    m_szGrp[TLK_PLHURT1] = "!SC_CUREA";
    m_szGrp[TLK_PLHURT2] = "!SC_CUREB";
    m_szGrp[TLK_PLHURT3] = "!SC_CUREC";

    m_szGrp[TLK_PHELLO] = "SC_PHELLO";
    m_szGrp[TLK_PIDLE] = "SC_PIDLE";
    m_szGrp[TLK_PQUESTION] = "SC_PQUEST";
    m_szGrp[TLK_SMELL] = "SC_SMELL";

    m_szGrp[TLK_WOUND] = "SC_WOUND";
    m_szGrp[TLK_MORTAL] = "SC_MORTAL";

    if ((pev->body >= 0 && pev->body < 25) || (pev->body >= 100 && pev->body < 125))
        m_voicePitch = 105;
    if ((pev->body >= 25 && pev->body < 50) || (pev->body >= 125 && pev->body < 150))
        m_voicePitch = 100;
    if ((pev->body >= 50 && pev->body < 75) || (pev->body >= 150 && pev->body < 175))
        m_voicePitch = 95;
    if ((pev->body >= 75 && pev->body <= 100) || (pev->body >= 175 && pev->body < 200))
        m_voicePitch = 100;
}

int CScientist::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
    if (pevInflictor && pevInflictor->flags & FL_CLIENT)
    {
        Remember(bits_MEMORY_PROVOKED);
        StopFollowing(TRUE);
    }

    return CTalkMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

int CScientist::ISoundMask(void)
{
    return bits_SOUND_WORLD |
           bits_SOUND_COMBAT |
           bits_SOUND_DANGER |
           bits_SOUND_PLAYER;
}

void CScientist::PainSound(void)
{
    if (gpGlobals->time < m_painTime)
        return;

    m_painTime = gpGlobals->time + RANDOM_FLOAT(0.5, 0.75);

    switch (RANDOM_LONG(0, 4))
    {
    case 0: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "scientist/sci_pain1.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
    case 1: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "scientist/sci_pain2.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
    case 2: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "scientist/sci_pain3.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
    case 3: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "scientist/sci_pain4.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
    case 4: EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "scientist/sci_pain5.wav", 1, ATTN_NORM, 0, GetVoicePitch()); break;
    }
}

void CScientist::DeathSound(void)
{
    PainSound();
}

void CScientist::Killed(entvars_t *pevAttacker, int iGib)
{
    SetUse(NULL);
    CTalkMonster::Killed(pevAttacker, iGib);
}

void CScientist::SetActivity(Activity newActivity)
{
    int iSequence;

    iSequence = LookupActivity(newActivity);

    if (iSequence == ACTIVITY_NOT_AVAILABLE)
        newActivity = ACT_IDLE;
    CTalkMonster::SetActivity(newActivity);
}

Schedule_t *CScientist::GetScheduleOfType(int Type)
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

Schedule_t *CScientist::GetSchedule(void)
{
    CBaseEntity *pEnemy = m_hEnemy;

    if (HasConditions(bits_COND_HEAR_SOUND))
    {
        CSound *pSound;
        pSound = PBestSound();

        if (pSound && (pSound->m_iType & bits_SOUND_DANGER))
        {
            if (gpGlobals->time - m_coverTime > 3)
            {
                m_coverTime = gpGlobals->time;
                return GetScheduleOfType(SCHED_TAKE_COVER_FROM_BEST_SOUND);
            }
        }
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

        if (gpGlobals->time - m_coverTime < 3)
        {
            if (gpGlobals->time - m_fearTime > 3)
            {
                m_fearTime = gpGlobals->time;
                return GetScheduleOfType(SCHED_STARTLE);
            }
            return GetScheduleOfType(SCHED_ALERT_STAND);
        }

        if (HasConditions(bits_COND_SEE_ENEMY))
        {
            m_coverTime = gpGlobals->time;
            return slScientistCover;
        }

        if (HasConditions(bits_COND_HEAR_SOUND))
        {
            m_coverTime = gpGlobals->time;
            return slTakeCoverFromBestSound;
        }

        m_coverTime = gpGlobals->time;
        return slScientistCover;
        break;
    }

    return CTalkMonster::GetSchedule();
}

MONSTERSTATE CScientist::GetIdealState(void)
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

BOOL CScientist::CanHeal(void)
{
    if ((m_healTime > gpGlobals->time) || (m_hTargetEnt == NULL) || (m_hTargetEnt->pev->health > (m_hTargetEnt->pev->max_health * 0.5f)))
        return FALSE;

    return TRUE;
}

void CScientist::Heal(void)
{
    if (!CanHeal())
        return;

    Vector target = m_hTargetEnt->pev->origin - pev->origin;
    if (target.Length() > 100)
        return;

    m_hTargetEnt->TakeHealth(gSkillData.scientistHeal, DMG_GENERIC);
    m_healTime = gpGlobals->time + 60;
}

int CScientist::FriendNumber(int arrayNumber)
{
    static int array[3] = { 1, 2, 0 };
    if (arrayNumber < 3)
        return array[arrayNumber];
    return arrayNumber;
}

class CDeadScientist : public CBaseMonster
{
public:
    void Spawn(void);
    int Classify(void) { return CLASS_HUMAN_PASSIVE; }

    void KeyValue(KeyValueData *pkvd);
    int m_iPose;
    static char *m_szPoses[7];
};
char *CDeadScientist::m_szPoses[] = { "lying_on_back", "lying_on_stomach", "dead_sitting", "dead_hang", "dead_table1", "dead_table2", "dead_table3" };

void CDeadScientist::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "pose"))
    {
        m_iPose = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else
        CBaseMonster::KeyValue(pkvd);
}
LINK_ENTITY_TO_CLASS(monster_scientist_dead, CDeadScientist);

void CDeadScientist::Spawn()
{
    PRECACHE_MODEL("models/scientist.dol");
    SET_MODEL(ENT(pev), "models/scientist.dol");

    pev->effects = 0;
    pev->sequence = 0;
    pev->health = 8;

    m_bloodColor = BLOOD_COLOR_RED;

    if (pev->body == -1)
        SetBodygroup(1, RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1));
    else
        SetBodygroup(1, pev->body);

    if (GetBodygroup(1) == HEAD_LUTHER)
        pev->skin = 1;
    else
        pev->skin = 0;

    pev->sequence = LookupSequence(m_szPoses[m_iPose]);
    if (pev->sequence == -1)
    {
        ALERT(at_console, "Dead scientist with bad pose\n");
    }

    MonsterInitDead();
}

class CSittingScientist : public CScientist
{
public:
    void Spawn(void);
    void Precache(void);

    void EXPORT SittingThink(void);
    int Classify(void);
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    virtual void SetAnswerQuestion(CTalkMonster *pSpeaker);
    int FriendNumber(int arrayNumber);

    int FIdleSpeak(void);
    int m_baseSequence;
    int m_headTurn;
    float m_flResponseDelay;
};

LINK_ENTITY_TO_CLASS(monster_sitting_scientist, CSittingScientist);
TYPEDESCRIPTION CSittingScientist::m_SaveData[] =
{
    DEFINE_FIELD(CSittingScientist, m_headTurn, FIELD_INTEGER),
    DEFINE_FIELD(CSittingScientist, m_flResponseDelay, FIELD_FLOAT),
};

IMPLEMENT_SAVERESTORE(CSittingScientist, CScientist);

typedef enum
{
    SITTING_ANIM_sitlookleft,
    SITTING_ANIM_sitlookright,
    SITTING_ANIM_sitscared,
    SITTING_ANIM_sitting2,
    SITTING_ANIM_sitting3
} SITTING_ANIM;

void CSittingScientist::Spawn()
{
    PRECACHE_MODEL("models/scientist.dol");
    SET_MODEL(ENT(pev), "models/scientist.dol");
    Precache();
    InitBoneControllers();

    UTIL_SetSize(pev, Vector(-14, -14, 0), Vector(14, 14, 36));

    pev->solid = SOLID_SLIDEBOX;
    pev->movetype = MOVETYPE_STEP;
    pev->effects = 0;
    pev->health = 50;

    m_bloodColor = BLOOD_COLOR_RED;
    m_flFieldOfView = VIEW_FIELD_WIDE;

    m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD;

    SetBits(pev->spawnflags, SF_MONSTER_PREDISASTER);

    if (pev->body == -1)
        SetBodygroup(1, RANDOM_LONG(0, NUM_SCIENTIST_HEADS - 1));
    else
        SetBodygroup(1, pev->body);

    if (GetBodygroup(1) == HEAD_LUTHER)
        pev->skin = 1;
    else
        pev->skin = 0;

    m_baseSequence = LookupSequence("sitlookleft");
    pev->sequence = m_baseSequence + RANDOM_LONG(0, 4);
    ResetSequenceInfo();

    SetThink(&CSittingScientist::SittingThink);
    pev->nextthink = gpGlobals->time + 0.1f;

    DROP_TO_FLOOR(ENT(pev));
}

void CSittingScientist::Precache(void)
{
    m_baseSequence = LookupSequence("sitlookleft");
    TalkInit();
}

int CSittingScientist::Classify(void)
{
    return CLASS_HUMAN_PASSIVE;
}

int CSittingScientist::FriendNumber(int arrayNumber)
{
    static int array[3] = { 2, 1, 0 };
    if (arrayNumber < 3)
        return array[arrayNumber];
    return arrayNumber;
}

void CSittingScientist::SittingThink(void)
{
    CBaseEntity *pent;

    StudioFrameAdvance();

    if (FIdleHello())
    {
        pent = FindNearestFriend(TRUE);
        if (pent)
        {
            float yaw = VecToYaw(pent->pev->origin - pev->origin) - pev->angles.y;

            if (yaw > 180) yaw -= 360;
            if (yaw < -180) yaw += 360;

            if (yaw > 0)
                pev->sequence = m_baseSequence + SITTING_ANIM_sitlookleft;
            else
                pev->sequence = m_baseSequence + SITTING_ANIM_sitlookright;

            ResetSequenceInfo();
            pev->frame = 0;
            SetBoneController(0, 0);
        }
    }
    else if (m_fSequenceFinished)
    {
        int i = RANDOM_LONG(0, 99);
        m_headTurn = 0;

        if (m_flResponseDelay && gpGlobals->time > m_flResponseDelay)
        {
            IdleRespond();
            pev->sequence = m_baseSequence + SITTING_ANIM_sitscared;
            m_flResponseDelay = 0;
        }
        else if (i < 30)
        {
            pev->sequence = m_baseSequence + SITTING_ANIM_sitting3;

            if (!FBitSet(m_bitsSaid, bit_saidHelloPlayer))
                pent = FindNearestFriend(TRUE);
            else
                pent = FindNearestFriend(FALSE);

            if (!FIdleSpeak() || !pent)
            {
                m_headTurn = RANDOM_LONG(0, 8) * 10 - 40;
                pev->sequence = m_baseSequence + SITTING_ANIM_sitting3;
            }
            else
            {
                float yaw = VecToYaw(pent->pev->origin - pev->origin) - pev->angles.y;

                if (yaw > 180) yaw -= 360;
                if (yaw < -180) yaw += 360;

                if (yaw > 0)
                    pev->sequence = m_baseSequence + SITTING_ANIM_sitlookleft;
                else
                    pev->sequence = m_baseSequence + SITTING_ANIM_sitlookright;
            }
        }
        else if (i < 60)
        {
            pev->sequence = m_baseSequence + SITTING_ANIM_sitting3;
            m_headTurn = RANDOM_LONG(0, 8) * 10 - 40;
            if (RANDOM_LONG(0, 99) < 5)
            {
                FIdleSpeak();
            }
        }
        else if (i < 80)
        {
            pev->sequence = m_baseSequence + SITTING_ANIM_sitting2;
        }
        else if (i < 100)
        {
            pev->sequence = m_baseSequence + SITTING_ANIM_sitscared;
        }

        ResetSequenceInfo();
        pev->frame = 0;
        SetBoneController(0, m_headTurn);
    }
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CSittingScientist::SetAnswerQuestion(CTalkMonster *pSpeaker)
{
    m_flResponseDelay = gpGlobals->time + RANDOM_FLOAT(6, 8);
    m_hTalkTarget = (CBaseMonster *)pSpeaker;
}

int CSittingScientist::FIdleSpeak(void)
{
    int pitch;

    if (!FOkToSpeak())
        return FALSE;

    CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(4.8f, 5.2f);

    pitch = GetVoicePitch();

    CBaseEntity *pentFriend = FindNearestFriend(FALSE);

    if (pentFriend && RANDOM_LONG(0, 1))
    {
        CTalkMonster *pTalkMonster = GetClassPtr((CTalkMonster *)pentFriend->pev);
        pTalkMonster->SetAnswerQuestion(this);

        IdleHeadTurn(pentFriend->pev->origin);
        SENTENCEG_PlayRndSz(ENT(pev), m_szGrp[TLK_PQUESTION], 1.0, ATTN_IDLE, 0, pitch);
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(4.8f, 5.2f);
        return TRUE;
    }

    if (RANDOM_LONG(0, 1))
    {
        SENTENCEG_PlayRndSz(ENT(pev), m_szGrp[TLK_PIDLE], 1.0, ATTN_IDLE, 0, pitch);
        CTalkMonster::g_talkWaitTime = gpGlobals->time + RANDOM_FLOAT(4.8f, 5.2f);
        return TRUE;
    }

    CTalkMonster::g_talkWaitTime = 0;
    return FALSE;
}

/* registro dos ponteiros de método usados neste arquivo (chamado por
   func_00239870 no MethodPointerTable.cpp) */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" int func_00239810(const char *name, USEPTR pfn);

extern "C" void func_001E0020(void)
{
    func_002397D0("252", static_cast<BASEPTR>(&CSittingScientist::SittingThink));
    func_00239810("253", static_cast<USEPTR>(&CTalkMonster::FollowerUse));
}
