/* dlls/tentacle_decay.cpp: o tentáculo do Decay (monster_tentacle_decay).
   É uma cópia do CTentacle do HLSDK (tentacle.cpp) com mudanças do PS2:
   segue o m_pCine em modo script, sangra quando o evento 1003 corta o
   tentáculo (BleedThink) e o DieThink solta um jato de sangue. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/soundent.h"

#define ACT_T_IDLE      1010
#define ACT_T_TAP       1020
#define ACT_T_STRIKE    1030
#define ACT_T_REARIDLE  1040

#define RANDOM_SOUND_ARRAY(array) (array)[RANDOM_LONG(0, ARRAYSIZE((array)) - 1)]
#define M_PI 3.14159265358979323846

#define TENTACLE_AE_CUT 1003    /* PS2: o tentáculo foi cortado */

class CSoundEnt : public CBaseEntity
{
public:
    static void InsertSound(int iType, const Vector &vecOrigin, int iVolume, float flDuration);
};

class CCineMonster : public CBaseMonster
{
public:
    int m_iszIdle;          /* 0x2A0 */
    int m_iszPlay;          /* 0x2A4 */
};

void UTIL_BloodStream(const Vector &origin, const Vector &direction, int color, int amount);
TraceResult UTIL_GetGlobalTrace(void);
void UTIL_EmitAmbientSound(edict_t *entity, const Vector &vecOrigin, const char *samp, float vol, float attenuation, int fFlags, int pitch);

extern "C" float cosf(float);
extern "C" float sinf(float);

class CTentacleDecay : public CBaseMonster
{
public:
    CTentacleDecay(void);

    void Spawn(void);
    void Precache(void);
    void KeyValue(KeyValueData *pkvd);

    int Save(CSave &save);
    int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    // Don't allow the tentacle to go across transitions!!!
    virtual int ObjectCaps(void) { return CBaseMonster::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

    void SetObjectCollisionBox(void)
    {
        pev->absmin = pev->origin + Vector(-400, -400, 0);
        pev->absmax = pev->origin + Vector(400, 400, 850);
    }

    void EXPORT Cycle(void);
    void EXPORT CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void EXPORT Start(void);
    void EXPORT DieThink(void);
    void EXPORT BleedThink(void);

    void EXPORT Test(void);

    void EXPORT HitTouch(CBaseEntity *pOther);

    float HearingSensitivity(void) { return 2.0; };

    int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    void HandleAnimEvent(MonsterEvent_t *pEvent);
    void Killed(entvars_t *pevAttacker, int iGib);

    MONSTERSTATE GetIdealState(void) { return MONSTERSTATE_IDLE; };
    int CanPlaySequence(BOOL fDisregardState) { return TRUE; };

    int Classify(void);

    int Level(float dz);
    int MyLevel(void);
    float MyHeight(void);

    float m_flInitialYaw;           /* 0x2A0 */
    int m_iGoalAnim;                /* 0x2A4 */
    int m_iLevel;                   /* 0x2A8 */
    int m_iDir;                     /* 0x2AC */
    float m_flFramerateAdj;         /* 0x2B0 */
    float m_flSoundYaw;             /* 0x2B4 */
    int m_iSoundLevel;              /* 0x2B8 */
    float m_flSoundTime;            /* 0x2BC */
    float m_flSoundRadius;          /* 0x2C0 */
    int m_iHitDmg;                  /* 0x2C4 */
    float m_flHitTime;              /* 0x2C8 */

    float m_flTapRadius;            /* 0x2CC */

    float m_flNextSong;             /* 0x2D0 */

    float m_flMaxYaw;               /* 0x2D4 */
    int m_iTapSound;                /* 0x2D8 */

    Vector m_vecPrevSound;          /* 0x2DC */
    float m_flPrevSoundTime;        /* 0x2E8 */
    BOOL m_fScriptTurn;             /* 0x2EC: PS2, ainda precisa virar para o m_pCine */

    static const char *pHitSilo[];
    static const char *pHitDirt[];
    static const char *pHitWater[];
};

/* static int g_fFlySound, g_fSquirmSound (.bss) */
extern "C" int D_003990C0;
extern "C" int D_003990C4;
#define g_fFlySound     D_003990C0
#define g_fSquirmSound  D_003990C4

LINK_ENTITY_TO_CLASS(monster_tentacle_decay, CTentacleDecay);

// stike sounds
#define TE_NONE -1
#define TE_SILO 0
#define TE_DIRT 1
#define TE_WATER 2

const char *CTentacleDecay::pHitSilo[] =
{
    "tentacle/te_strike1.wav",
    "tentacle/te_strike2.wav",
};

const char *CTentacleDecay::pHitDirt[] =
{
    "player/pl_dirt1.wav",
    "player/pl_dirt2.wav",
    "player/pl_dirt3.wav",
    "player/pl_dirt4.wav",
};

const char *CTentacleDecay::pHitWater[] =
{
    "player/pl_slosh1.wav",
    "player/pl_slosh2.wav",
    "player/pl_slosh3.wav",
    "player/pl_slosh4.wav",
};

TYPEDESCRIPTION CTentacleDecay::m_SaveData[] =
{
    DEFINE_FIELD(CTentacleDecay, m_flInitialYaw, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_iGoalAnim, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_iLevel, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_iDir, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_flFramerateAdj, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_flSoundYaw, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_iSoundLevel, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_flSoundTime, FIELD_TIME),
    DEFINE_FIELD(CTentacleDecay, m_flSoundRadius, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_iHitDmg, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_flHitTime, FIELD_TIME),
    DEFINE_FIELD(CTentacleDecay, m_flTapRadius, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_flNextSong, FIELD_TIME),
    DEFINE_FIELD(CTentacleDecay, m_iTapSound, FIELD_INTEGER),
    DEFINE_FIELD(CTentacleDecay, m_flMaxYaw, FIELD_FLOAT),
    DEFINE_FIELD(CTentacleDecay, m_vecPrevSound, FIELD_POSITION_VECTOR),
    DEFINE_FIELD(CTentacleDecay, m_flPrevSoundTime, FIELD_TIME),
};
IMPLEMENT_SAVERESTORE(CTentacleDecay, CBaseMonster);

// animation sequence aliases
typedef enum
{
    TENTACLE_ANIM_Pit_Idle,

    TENTACLE_ANIM_rise_to_Temp1,
    TENTACLE_ANIM_Temp1_to_Floor,
    TENTACLE_ANIM_Floor_Idle,
    TENTACLE_ANIM_Floor_Fidget_Pissed,
    TENTACLE_ANIM_Floor_Fidget_SmallRise,
    TENTACLE_ANIM_Floor_Fidget_Wave,
    TENTACLE_ANIM_Floor_Strike,
    TENTACLE_ANIM_Floor_Tap,
    TENTACLE_ANIM_Floor_Rotate,
    TENTACLE_ANIM_Floor_Rear,
    TENTACLE_ANIM_Floor_Rear_Idle,
    TENTACLE_ANIM_Floor_to_Lev1,

    TENTACLE_ANIM_Lev1_Idle,
    TENTACLE_ANIM_Lev1_Fidget_Claw,
    TENTACLE_ANIM_Lev1_Fidget_Shake,
    TENTACLE_ANIM_Lev1_Fidget_Snap,
    TENTACLE_ANIM_Lev1_Strike,
    TENTACLE_ANIM_Lev1_Tap,
    TENTACLE_ANIM_Lev1_Rotate,
    TENTACLE_ANIM_Lev1_Rear,
    TENTACLE_ANIM_Lev1_Rear_Idle,
    TENTACLE_ANIM_Lev1_to_Lev2,

    TENTACLE_ANIM_Lev2_Idle,
    TENTACLE_ANIM_Lev2_Fidget_Shake,
    TENTACLE_ANIM_Lev2_Fidget_Swing,
    TENTACLE_ANIM_Lev2_Fidget_Tut,
    TENTACLE_ANIM_Lev2_Strike,
    TENTACLE_ANIM_Lev2_Tap,
    TENTACLE_ANIM_Lev2_Rotate,
    TENTACLE_ANIM_Lev2_Rear,
    TENTACLE_ANIM_Lev2_Rear_Idle,
    TENTACLE_ANIM_Lev2_to_Lev3,

    TENTACLE_ANIM_Lev3_Idle,
    TENTACLE_ANIM_Lev3_Fidget_Shake,
    TENTACLE_ANIM_Lev3_Fidget_Side,
    TENTACLE_ANIM_Lev3_Fidget_Swipe,
    TENTACLE_ANIM_Lev3_Strike,
    TENTACLE_ANIM_Lev3_Tap,
    TENTACLE_ANIM_Lev3_Rotate,
    TENTACLE_ANIM_Lev3_Rear,
    TENTACLE_ANIM_Lev3_Rear_Idle,

    TENTACLE_ANIM_Lev1_Door_reach,

    TENTACLE_ANIM_Lev3_to_Engine,
    TENTACLE_ANIM_Engine_Idle,
    TENTACLE_ANIM_Engine_Sway,
    TENTACLE_ANIM_Engine_Swat,
    TENTACLE_ANIM_Engine_Bob,
    TENTACLE_ANIM_Engine_Death1,
    TENTACLE_ANIM_Engine_Death2,
    TENTACLE_ANIM_Engine_Death3,

    TENTACLE_ANIM_none
} TENTACLE_ANIM;

//=========================================================
// Classify - indicates this monster's place in the
// relationship table.
//=========================================================
int CTentacleDecay::Classify(void)
{
    return CLASS_ALIEN_MONSTER;
}

//
// Tentacle Spawn
//
void CTentacleDecay::Spawn(void)
{
    Precache();

    pev->solid = SOLID_BBOX;
    pev->movetype = MOVETYPE_FLY;
    pev->effects = 0;
    pev->health = 75;
    pev->sequence = 0;

    SET_MODEL(ENT(pev), "models/tentacle2.dol");
    UTIL_SetSize(pev, Vector(-32, -32, 0), Vector(32, 32, 64));

    pev->takedamage = DAMAGE_YES;
    pev->flags |= FL_MONSTER;

    m_bloodColor = BLOOD_COLOR_GREEN;

    SetThink(&CTentacleDecay::Start);
    SetTouch(&CTentacleDecay::HitTouch);
    SetUse(&CTentacleDecay::CommandUse);

    pev->nextthink = gpGlobals->time + 0.2f;

    ResetSequenceInfo();
    m_iDir = 1;

    pev->yaw_speed = 18;
    m_flInitialYaw = pev->angles.y;
    pev->ideal_yaw = m_flInitialYaw;

    g_fFlySound = FALSE;
    g_fSquirmSound = FALSE;

    m_iHitDmg = 20;

    if (m_flMaxYaw <= 0)
        m_flMaxYaw = 65;

    m_MonsterState = MONSTERSTATE_IDLE;

    // SetThink( Test );
    UTIL_SetOrigin(pev, pev->origin);

    m_fScriptTurn = TRUE;
}

void CTentacleDecay::Precache(void)
{
    PRECACHE_MODEL("models/tentacle2.dol");

    PRECACHE_SOUND("ambience/flies.wav");
    PRECACHE_SOUND("ambience/squirm2.wav");

    PRECACHE_SOUND("tentacle/te_alert1.wav");
    PRECACHE_SOUND("tentacle/te_alert2.wav");
    PRECACHE_SOUND("tentacle/te_flies1.wav");
    PRECACHE_SOUND("tentacle/te_move1.wav");
    PRECACHE_SOUND("tentacle/te_move2.wav");
    PRECACHE_SOUND("tentacle/te_roar1.wav");
    PRECACHE_SOUND("tentacle/te_roar2.wav");
    PRECACHE_SOUND("tentacle/te_search1.wav");
    PRECACHE_SOUND("tentacle/te_search2.wav");
    PRECACHE_SOUND("tentacle/te_sing1.wav");
    PRECACHE_SOUND("tentacle/te_sing2.wav");
    PRECACHE_SOUND("tentacle/te_squirm2.wav");
    PRECACHE_SOUND("tentacle/te_strike1.wav");
    PRECACHE_SOUND("tentacle/te_strike2.wav");
    PRECACHE_SOUND("tentacle/te_swing1.wav");
    PRECACHE_SOUND("tentacle/te_swing2.wav");

    switch (m_iTapSound)
    {
    case TE_SILO:
        PRECACHE_SOUND_ARRAY(pHitSilo);
        break;
    case TE_NONE:
        PRECACHE_SOUND_ARRAY(pHitSilo);
        PRECACHE_SOUND_ARRAY(pHitDirt);
        PRECACHE_SOUND_ARRAY(pHitWater);
        break;
    case TE_DIRT:
        PRECACHE_SOUND_ARRAY(pHitDirt);
        break;
    case TE_WATER:
        PRECACHE_SOUND_ARRAY(pHitWater);
        break;
    }
}

CTentacleDecay::CTentacleDecay(void)
{
    m_flMaxYaw = 65;
    m_iTapSound = 0;
}

void CTentacleDecay::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "sweeparc"))
    {
        m_flMaxYaw = atof(pkvd->szValue) / 2.0;
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "sound"))
    {
        m_iTapSound = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else
        CBaseMonster::KeyValue(pkvd);
}

int CTentacleDecay::Level(float dz)
{
    if (dz < 216)
        return 0;
    if (dz < 408)
        return 1;
    if (dz < 600)
        return 2;
    return 3;
}

float CTentacleDecay::MyHeight(void)
{
    switch (MyLevel())
    {
    case 1:
        return 256;
    case 2:
        return 448;
    case 3:
        return 640;
    }
    return 0;
}

int CTentacleDecay::MyLevel(void)
{
    switch (pev->sequence)
    {
    case TENTACLE_ANIM_Pit_Idle:
        return -1;

    case TENTACLE_ANIM_rise_to_Temp1:
    case TENTACLE_ANIM_Temp1_to_Floor:
    case TENTACLE_ANIM_Floor_to_Lev1:
        return 0;

    case TENTACLE_ANIM_Floor_Idle:
    case TENTACLE_ANIM_Floor_Fidget_Pissed:
    case TENTACLE_ANIM_Floor_Fidget_SmallRise:
    case TENTACLE_ANIM_Floor_Fidget_Wave:
    case TENTACLE_ANIM_Floor_Strike:
    case TENTACLE_ANIM_Floor_Tap:
    case TENTACLE_ANIM_Floor_Rotate:
    case TENTACLE_ANIM_Floor_Rear:
    case TENTACLE_ANIM_Floor_Rear_Idle:
        return 0;

    case TENTACLE_ANIM_Lev1_Idle:
    case TENTACLE_ANIM_Lev1_Fidget_Claw:
    case TENTACLE_ANIM_Lev1_Fidget_Shake:
    case TENTACLE_ANIM_Lev1_Fidget_Snap:
    case TENTACLE_ANIM_Lev1_Strike:
    case TENTACLE_ANIM_Lev1_Tap:
    case TENTACLE_ANIM_Lev1_Rotate:
    case TENTACLE_ANIM_Lev1_Rear:
    case TENTACLE_ANIM_Lev1_Rear_Idle:
        return 1;

    case TENTACLE_ANIM_Lev1_to_Lev2:
        return 1;

    case TENTACLE_ANIM_Lev2_Idle:
    case TENTACLE_ANIM_Lev2_Fidget_Shake:
    case TENTACLE_ANIM_Lev2_Fidget_Swing:
    case TENTACLE_ANIM_Lev2_Fidget_Tut:
    case TENTACLE_ANIM_Lev2_Strike:
    case TENTACLE_ANIM_Lev2_Tap:
    case TENTACLE_ANIM_Lev2_Rotate:
    case TENTACLE_ANIM_Lev2_Rear:
    case TENTACLE_ANIM_Lev2_Rear_Idle:
        return 2;

    case TENTACLE_ANIM_Lev2_to_Lev3:
        return 2;

    case TENTACLE_ANIM_Lev3_Idle:
    case TENTACLE_ANIM_Lev3_Fidget_Shake:
    case TENTACLE_ANIM_Lev3_Fidget_Side:
    case TENTACLE_ANIM_Lev3_Fidget_Swipe:
    case TENTACLE_ANIM_Lev3_Strike:
    case TENTACLE_ANIM_Lev3_Tap:
    case TENTACLE_ANIM_Lev3_Rotate:
    case TENTACLE_ANIM_Lev3_Rear:
    case TENTACLE_ANIM_Lev3_Rear_Idle:
        return 3;

    case TENTACLE_ANIM_Lev1_Door_reach:
        return -1;
    }
    return -1;
}

void CTentacleDecay::Test(void)
{
    pev->sequence = TENTACLE_ANIM_Floor_Strike;
    entvars_t *pevTest = pev;
    pevTest->framerate = 0;
    StudioFrameAdvance(pevTest->framerate);
    pev->nextthink = gpGlobals->time + 0.1f;
}

inline void CBaseMonster::ClearConditions(int iConditions)
{
    m_afConditions &= ~iConditions;
}

inline BOOL CBaseMonster::HasConditions(int iConditions)
{
    if (m_afConditions & iConditions)
        return TRUE;
    return FALSE;
}

//
// TentacleThink
//
void CTentacleDecay::Cycle(void)
{
    pev->nextthink = gpGlobals->time + 0.1f;

    if (m_MonsterState == MONSTERSTATE_SCRIPT || m_IdealMonsterState == MONSTERSTATE_SCRIPT)
    {
        if (m_fScriptTurn)
        {
            pev->ideal_yaw = m_pCine->pev->angles.y;
            if (pev->angles.y == pev->ideal_yaw)
            {
                pev->sequence = LookupSequence(STRING(m_pCine->m_iszPlay));
                pev->frame = 0;
                ResetSequenceInfo();
                m_fScriptTurn = FALSE;
            }
        }
        ClearConditions(IgnoreConditions());
        DispatchAnimEvents();
        StudioFrameAdvance();
        ChangeYaw(pev->yaw_speed);
        return;
    }

    m_fScriptTurn = TRUE;

    DispatchAnimEvents();
    StudioFrameAdvance();

    ChangeYaw(pev->yaw_speed);

    CSound *pSound;

    Listen();

    // Listen will set this if there's something in my sound list
    if (HasConditions(bits_COND_HEAR_SOUND))
        pSound = PBestSound();
    else
        pSound = NULL;

    if (pSound)
    {
        Vector vecDir;
        if (gpGlobals->time - m_flPrevSoundTime < 0.5f)
        {
            float dt = gpGlobals->time - m_flPrevSoundTime;
            vecDir = pSound->m_vecOrigin + (pSound->m_vecOrigin - m_vecPrevSound) / dt - pev->origin;
        }
        else
        {
            vecDir = pSound->m_vecOrigin - pev->origin;
        }
        m_flPrevSoundTime = gpGlobals->time;
        m_vecPrevSound = pSound->m_vecOrigin;

        m_flSoundYaw = UTIL_VecToYaw(vecDir) - m_flInitialYaw;
        m_iSoundLevel = Level(vecDir.z);

        if (m_flSoundYaw < -180)
            m_flSoundYaw += 360;
        if (m_flSoundYaw > 180)
            m_flSoundYaw -= 360;

        if (m_flSoundTime < gpGlobals->time)
        {
            // play "I hear new something" sound
            char *sound;

            switch (RANDOM_LONG(0, 1))
            {
            case 0: sound = "tentacle/te_alert1.wav"; break;
            case 1: sound = "tentacle/te_alert2.wav"; break;
            }
        }
        m_flSoundTime = gpGlobals->time + RANDOM_FLOAT(5.0, 10.0);
    }

    // clip ideal_yaw
    float dy = m_flSoundYaw;
    switch (pev->sequence)
    {
    case TENTACLE_ANIM_Floor_Rear:
    case TENTACLE_ANIM_Floor_Rear_Idle:
    case TENTACLE_ANIM_Lev1_Rear:
    case TENTACLE_ANIM_Lev1_Rear_Idle:
    case TENTACLE_ANIM_Lev2_Rear:
    case TENTACLE_ANIM_Lev2_Rear_Idle:
    case TENTACLE_ANIM_Lev3_Rear:
    case TENTACLE_ANIM_Lev3_Rear_Idle:
        if (dy < 0 && dy > -m_flMaxYaw)
            dy = -m_flMaxYaw;
        if (dy > 0 && dy < m_flMaxYaw)
            dy = m_flMaxYaw;
        break;
    default:
        if (dy < -m_flMaxYaw)
            dy = -m_flMaxYaw;
        if (dy > m_flMaxYaw)
            dy = m_flMaxYaw;
    }
    pev->ideal_yaw = m_flInitialYaw + dy;

    if (m_fSequenceFinished)
    {
        if (pev->health <= 1)
        {
            m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
            if (pev->sequence == TENTACLE_ANIM_Pit_Idle)
            {
                pev->health = 75;
            }
        }
        else if (m_flSoundTime > gpGlobals->time)
        {
            if (m_flSoundYaw >= -(m_flMaxYaw + 30) && m_flSoundYaw <= (m_flMaxYaw + 30))
            {
                // strike
                m_iGoalAnim = LookupActivity(ACT_T_STRIKE + m_iSoundLevel);
            }
            else if (m_flSoundYaw >= -m_flMaxYaw * 2 && m_flSoundYaw <= m_flMaxYaw * 2)
            {
                // tap
                m_iGoalAnim = LookupActivity(ACT_T_TAP + m_iSoundLevel);
            }
            else
            {
                // go into rear idle
                m_iGoalAnim = LookupActivity(ACT_T_REARIDLE + m_iSoundLevel);
            }
        }
        else if (pev->sequence == TENTACLE_ANIM_Pit_Idle)
        {
            // stay in pit until hear noise
            m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
        }
        else if (pev->sequence == m_iGoalAnim)
        {
            if (MyLevel() >= 0 && gpGlobals->time < m_flSoundTime)
            {
                if (RANDOM_LONG(0, 9) < m_flSoundTime - gpGlobals->time)
                {
                    // continue stike
                    m_iGoalAnim = LookupActivity(ACT_T_STRIKE + m_iSoundLevel);
                }
                else
                {
                    // tap
                    m_iGoalAnim = LookupActivity(ACT_T_TAP + m_iSoundLevel);
                }
            }
            else if (MyLevel() < 0)
            {
                m_iGoalAnim = LookupActivity(ACT_T_IDLE + 0);
            }
            else
            {
                if (m_flNextSong < gpGlobals->time)
                {
                    // play "I hear new something" sound
                    char *sound;

                    switch (RANDOM_LONG(0, 1))
                    {
                    case 0: sound = "tentacle/te_sing1.wav"; break;
                    default: sound = "tentacle/te_sing2.wav"; break;
                    }

                    EMIT_SOUND(ENT(pev), CHAN_VOICE, sound, 1.0, ATTN_NORM);

                    m_flNextSong = gpGlobals->time + RANDOM_FLOAT(10, 20);
                }

                if (RANDOM_LONG(0, 15) == 0)
                {
                    // idle on new level
                    m_iGoalAnim = LookupActivity(ACT_T_IDLE + RANDOM_LONG(0, 3));
                }
                else if (RANDOM_LONG(0, 3) == 0)
                {
                    // tap
                    m_iGoalAnim = LookupActivity(ACT_T_TAP + MyLevel());
                }
                else
                {
                    // idle
                    m_iGoalAnim = LookupActivity(ACT_T_IDLE + MyLevel());
                }
            }
            if (m_flSoundYaw < 0)
                m_flSoundYaw += RANDOM_FLOAT(2, 8);
            else
                m_flSoundYaw -= RANDOM_FLOAT(2, 8);
        }

        pev->sequence = FindTransition(pev->sequence, m_iGoalAnim, &m_iDir);

        if (m_iDir > 0)
        {
            pev->frame = 0;
        }
        else
        {
            m_iDir = -1; // just to safe
            pev->frame = 255;
        }
        ResetSequenceInfo();

        m_flFramerateAdj = RANDOM_FLOAT(-0.2f, 0.2f);
        pev->framerate = m_iDir * 1.0f + m_flFramerateAdj;

        switch (pev->sequence)
        {
        case TENTACLE_ANIM_Floor_Tap:
        case TENTACLE_ANIM_Lev1_Tap:
        case TENTACLE_ANIM_Lev2_Tap:
        case TENTACLE_ANIM_Lev3_Tap:
            {
                Vector vecSrc;
                UTIL_MakeVectors(pev->angles);

                TraceResult tr1, tr2;

                vecSrc = pev->origin + Vector(0, 0, MyHeight() - 4);
                UTIL_TraceLine(vecSrc, vecSrc + gpGlobals->v_forward * 512, ignore_monsters, ENT(pev), &tr1);

                vecSrc = pev->origin + Vector(0, 0, MyHeight() + 8);
                UTIL_TraceLine(vecSrc, vecSrc + gpGlobals->v_forward * 512, ignore_monsters, ENT(pev), &tr2);

                m_flTapRadius = SetBlending(0, RANDOM_FLOAT(tr1.flFraction * 512, tr2.flFraction * 512));
            }
            break;
        default:
            m_flTapRadius = 336; // 400 - 64
            break;
        }
        pev->view_ofs.z = MyHeight();
    }

    if (m_flPrevSoundTime + 2.0f > gpGlobals->time)
    {
        // 1.5 normal speed if hears sounds
        pev->framerate = m_iDir * 1.5f + m_flFramerateAdj;
    }
    else if (m_flPrevSoundTime + 5.0f > gpGlobals->time)
    {
        // slowdown to normal
        pev->framerate = m_iDir + m_iDir * (5 - (gpGlobals->time - m_flPrevSoundTime)) / 2 + m_flFramerateAdj;
    }
}

void CTentacleDecay::CommandUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    switch (useType)
    {
    case USE_OFF:
        pev->takedamage = DAMAGE_NO;
        SetThink(&CTentacleDecay::DieThink);
        m_iGoalAnim = TENTACLE_ANIM_Engine_Death1;
        break;
    case USE_ON:
        if (pActivator)
        {
            CSoundEnt::InsertSound(bits_SOUND_WORLD, pActivator->pev->origin, 1024, 1.0);
        }
        break;
    case USE_SET:
        break;
    case USE_TOGGLE:
        pev->takedamage = DAMAGE_NO;
        SetThink(&CTentacleDecay::DieThink);
        m_iGoalAnim = TENTACLE_ANIM_Engine_Idle;
        break;
    }
}

void CTentacleDecay::DieThink(void)
{
    pev->nextthink = gpGlobals->time + 0.1f;

    DispatchAnimEvents();
    StudioFrameAdvance();

    ChangeYaw(24);

    Vector vecSrc, vecAngles;
    GetAttachment(1, vecSrc, vecAngles);
    UTIL_MakeVectors(vecAngles);

    MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
        WRITE_BYTE(TE_BLOODSTREAM);
        WRITE_COORD(vecSrc.x);
        WRITE_COORD(vecSrc.y);
        WRITE_COORD(vecSrc.z);
        WRITE_COORD(gpGlobals->v_forward.x);
        WRITE_COORD(gpGlobals->v_forward.y);
        WRITE_COORD(gpGlobals->v_forward.z);
        WRITE_BYTE(70);
        WRITE_BYTE(25);
    MESSAGE_END();

    if (m_fSequenceFinished)
    {
        if (pev->sequence == m_iGoalAnim)
        {
            switch (m_iGoalAnim)
            {
            case TENTACLE_ANIM_Engine_Idle:
            case TENTACLE_ANIM_Engine_Sway:
            case TENTACLE_ANIM_Engine_Swat:
            case TENTACLE_ANIM_Engine_Bob:
                m_iGoalAnim = TENTACLE_ANIM_Engine_Sway + RANDOM_LONG(0, 2);
                break;
            case TENTACLE_ANIM_Engine_Death1:
            case TENTACLE_ANIM_Engine_Death2:
            case TENTACLE_ANIM_Engine_Death3:
                UTIL_Remove(this);
                return;
            }
        }

        pev->sequence = FindTransition(pev->sequence, m_iGoalAnim, &m_iDir);

        if (m_iDir > 0)
        {
            pev->frame = 0;
        }
        else
        {
            pev->frame = 255;
        }
        ResetSequenceInfo();

        float dy;
        switch (pev->sequence)
        {
        case TENTACLE_ANIM_Floor_Rear:
        case TENTACLE_ANIM_Floor_Rear_Idle:
        case TENTACLE_ANIM_Lev1_Rear:
        case TENTACLE_ANIM_Lev1_Rear_Idle:
        case TENTACLE_ANIM_Lev2_Rear:
        case TENTACLE_ANIM_Lev2_Rear_Idle:
        case TENTACLE_ANIM_Lev3_Rear:
        case TENTACLE_ANIM_Lev3_Rear_Idle:
        case TENTACLE_ANIM_Engine_Idle:
        case TENTACLE_ANIM_Engine_Sway:
        case TENTACLE_ANIM_Engine_Swat:
        case TENTACLE_ANIM_Engine_Bob:
        case TENTACLE_ANIM_Engine_Death1:
        case TENTACLE_ANIM_Engine_Death2:
        case TENTACLE_ANIM_Engine_Death3:
            pev->framerate = RANDOM_FLOAT(m_iDir - 0.2f, m_iDir + 0.2f);
            dy = 180;
            break;
        default:
            pev->framerate = 1.5;
            dy = 0;
            break;
        }
        pev->ideal_yaw = m_flInitialYaw + dy;
    }
}

/* PS2: depois de cortado, o toco sangra até a animação acabar */
void CTentacleDecay::BleedThink(void)
{
    DispatchAnimEvents();
    StudioFrameAdvance();

    ChangeYaw(24);

    Vector vecSrc, vecAngles;
    GetAttachment(1, vecSrc, vecAngles);
    UTIL_MakeVectors(vecAngles);

    vecAngles = gpGlobals->v_right;
    UTIL_BloodStream(vecSrc, vecAngles, BLOOD_COLOR_GREEN, 75);

    if (!m_fSequenceFinished)
        pev->nextthink = gpGlobals->time + 0.1f;
    else
        pev->nextthink = gpGlobals->time + 2.0f;
}

void CTentacleDecay::HandleAnimEvent(MonsterEvent_t *pEvent)
{
    char *sound;

    switch (pEvent->event)
    {
    case 1: // bang
        {
            Vector vecSrc, vecAngles;
            GetAttachment(0, vecSrc, vecAngles);

            switch (m_iTapSound)
            {
            case TE_SILO:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitSilo), 1.0, ATTN_NORM, 0, 100);
                break;
            case TE_NONE:
                break;
            case TE_DIRT:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitDirt), 1.0, ATTN_NORM, 0, 100);
                break;
            case TE_WATER:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitWater), 1.0, ATTN_NORM, 0, 100);
                break;
            }
            gpGlobals->force_retouch++;
        }
        break;

    case 3: // start killing swing
        m_iHitDmg = 200;
        break;

    case 4: // end killing swing
        m_iHitDmg = 25;
        break;

    case 5: // just "whoosh" sound
        break;

    case 2: // tap scrape
    case 6: // light tap
        {
            Vector vecSrc = pev->origin + m_flTapRadius * Vector(cosf(pev->angles.y * ((float)M_PI / 180.0f)), sinf(pev->angles.y * ((float)M_PI / 180.0f)), 0.0);

            vecSrc.z += MyHeight();

            float flVol = RANDOM_FLOAT(0.3, 0.5);

            switch (m_iTapSound)
            {
            case TE_SILO:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitSilo), flVol, ATTN_NORM, 0, 100);
                break;
            case TE_NONE:
                break;
            case TE_DIRT:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitDirt), flVol, ATTN_NORM, 0, 100);
                break;
            case TE_WATER:
                UTIL_EmitAmbientSound(ENT(pev), vecSrc, RANDOM_SOUND_ARRAY(pHitWater), flVol, ATTN_NORM, 0, 100);
                break;
            }
        }
        break;

    case 7: // roar
        switch (RANDOM_LONG(0, 1))
        {
        case 0: sound = "tentacle/te_roar1.wav"; break;
        default: sound = "tentacle/te_roar2.wav"; break;
        }

        UTIL_EmitAmbientSound(ENT(pev), pev->origin + Vector(0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
        break;

    case 8: // search
        switch (RANDOM_LONG(0, 1))
        {
        case 0: sound = "tentacle/te_search1.wav"; break;
        default: sound = "tentacle/te_search2.wav"; break;
        }

        UTIL_EmitAmbientSound(ENT(pev), pev->origin + Vector(0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
        break;

    case 9: // swing
        switch (RANDOM_LONG(0, 1))
        {
        case 0: sound = "tentacle/te_move1.wav"; break;
        default: sound = "tentacle/te_move2.wav"; break;
        }

        UTIL_EmitAmbientSound(ENT(pev), pev->origin + Vector(0, 0, MyHeight()), sound, 1.0, ATTN_NORM, 0, 100);
        break;

    case TENTACLE_AE_CUT:
        SetBodygroup(1, 1);
        SetThink(&CTentacleDecay::BleedThink);
        SetTouch(NULL);
        break;

    default:
        CBaseMonster::HandleAnimEvent(pEvent);
    }
}

//
// TentacleStart
//
void CTentacleDecay::Start(void)
{
    SetThink(&CTentacleDecay::Cycle);

    if (!g_fFlySound)
    {
        EMIT_SOUND(ENT(pev), CHAN_BODY, "ambience/flies.wav", 1, ATTN_NORM);
        g_fFlySound = TRUE;
    }
    else if (!g_fSquirmSound)
    {
        EMIT_SOUND(ENT(pev), CHAN_BODY, "ambience/squirm2.wav", 1, ATTN_NORM);
        g_fSquirmSound = TRUE;
    }

    pev->nextthink = gpGlobals->time + 0.1f;
}

void CTentacleDecay::HitTouch(CBaseEntity *pOther)
{
    TraceResult tr = UTIL_GetGlobalTrace();

    if (pOther->pev->modelindex == pev->modelindex)
        return;

    if (m_flHitTime > gpGlobals->time)
        return;

    // only look at the ones where the player hit me
    if (tr.pHit == NULL || tr.pHit->v.modelindex != pev->modelindex)
        return;

    if (tr.iHitgroup >= 3)
    {
        pOther->TakeDamage(pev, pev, m_iHitDmg, DMG_CRUSH);
    }
    else if (tr.iHitgroup != 0)
    {
        pOther->TakeDamage(pev, pev, 20, DMG_CRUSH);
    }
    else
    {
        return; // Huh?
    }

    m_flHitTime = gpGlobals->time + 0.5f;
}

int CTentacleDecay::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
    if (flDamage > pev->health)
    {
        pev->health = 1;
    }
    else
    {
        pev->health -= flDamage;
    }
    return 1;
}

void CTentacleDecay::Killed(entvars_t *pevAttacker, int iGib)
{
    m_iGoalAnim = TENTACLE_ANIM_Pit_Idle;
    return;
}

extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_001F7ED0(void)
{
    func_002397D0("280", static_cast<BASEPTR>(&CTentacleDecay::BleedThink));
    func_002397D0("281", static_cast<BASEPTR>(&CTentacleDecay::DieThink));
    func_002397D0("282", static_cast<BASEPTR>(&CTentacleDecay::Cycle));
    func_002397D0("283", static_cast<BASEPTR>(&CTentacleDecay::Start));
    func_002397F0("284", static_cast<ENTITYFUNCPTR>(&CTentacleDecay::HitTouch));
    func_00239810("285", static_cast<USEPTR>(&CTentacleDecay::CommandUse));
}
