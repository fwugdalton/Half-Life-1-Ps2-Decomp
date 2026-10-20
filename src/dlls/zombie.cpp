/* dlls/zombie.cpp: o zumbi (CZombie) do HLSDK.
   No PS2 há listas curtas de sons (2 em vez de 3) usadas quando
   g_pGameRules->IsMultiplayer(). */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/schedule.h"
#include "dlls/gamerules.h"

#define ZOMBIE_AE_ATTACK_RIGHT      0x01
#define ZOMBIE_AE_ATTACK_LEFT       0x02
#define ZOMBIE_AE_ATTACK_BOTH       0x03

#define ZOMBIE_FLINCH_DELAY         2

class CZombie : public CBaseMonster
{
public:
    void Spawn(void);
    void Precache(void);
    void SetYawSpeed(void);
    int Classify(void);
    void HandleAnimEvent(MonsterEvent_t *pEvent);
    int IgnoreConditions(void);

    float m_flNextFlinch;                   /* 0x2A0 */

    void PainSound(void);
    void AlertSound(void);
    void IdleSound(void);
    void AttackSound(void);

    static const char *pAttackSounds[];
    static const char *pIdleSounds[];
    static const char *pAlertSounds[];
    static const char *pAlertSoundsShort[];
    static const char *pPainSounds[];
    static const char *pAttackHitSounds[];
    static const char *pAttackHitSoundsShort[];
    static const char *pAttackMissSounds[];

    BOOL CheckRangeAttack1(float flDot, float flDist) { return FALSE; }
    BOOL CheckRangeAttack2(float flDot, float flDist) { return FALSE; }
    int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
};

LINK_ENTITY_TO_CLASS(monster_zombie, CZombie);

const char *CZombie::pAttackHitSounds[] =
{
    "zombie/claw_strike1.wav",
    "zombie/claw_strike2.wav",
    "zombie/claw_strike3.wav",
};

const char *CZombie::pAttackHitSoundsShort[] =
{
    "zombie/claw_strike1.wav",
    "zombie/claw_strike3.wav",
};

const char *CZombie::pAttackMissSounds[] =
{
    "zombie/claw_miss1.wav",
    "zombie/claw_miss2.wav",
};

const char *CZombie::pAttackSounds[] =
{
    "zombie/zo_attack1.wav",
    "zombie/zo_attack2.wav",
};

const char *CZombie::pIdleSounds[] =
{
    "zombie/zo_idle1.wav",
    "zombie/zo_idle2.wav",
    "zombie/zo_idle3.wav",
    "zombie/zo_idle4.wav",
};

const char *CZombie::pAlertSounds[] =
{
    "zombie/zo_alert10.wav",
    "zombie/zo_alert20.wav",
    "zombie/zo_alert30.wav",
};

const char *CZombie::pAlertSoundsShort[] =
{
    "zombie/zo_alert10.wav",
    "zombie/zo_alert30.wav",
};

const char *CZombie::pPainSounds[] =
{
    "zombie/zo_pain1.wav",
    "zombie/zo_pain2.wav",
};

int CZombie::Classify(void)
{
    return CLASS_ALIEN_MONSTER;
}

void CZombie::SetYawSpeed(void)
{
    int ys;

    ys = 120;

    pev->yaw_speed = ys;
}

int CZombie::TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType)
{
    if (bitsDamageType == DMG_BULLET)
    {
        Vector vecDir = pev->origin - (pevInflictor->absmin + pevInflictor->absmax) * 0.5;
        vecDir = vecDir.Normalize();
        float flForce = DamageForce(flDamage);
        pev->velocity = pev->velocity + vecDir * flForce;
        flDamage *= 0.3f;
    }

    if (IsAlive())
        PainSound();
    return CBaseMonster::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

void CZombie::PainSound(void)
{
    int pitch = 95 + RANDOM_LONG(0, 9);

    if (RANDOM_LONG(0, 5) < 2)
        EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pPainSounds[RANDOM_LONG(0, ARRAYSIZE(pPainSounds) - 1)], 1.0, ATTN_NORM, 0, pitch);
}

void CZombie::AlertSound(void)
{
    int pitch = 95 + RANDOM_LONG(0, 9);

    if (g_pGameRules->IsMultiplayer())
        EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pAlertSoundsShort[RANDOM_LONG(0, ARRAYSIZE(pAlertSoundsShort) - 1)], 1.0, ATTN_NORM, 0, pitch);
    else
        EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pAlertSounds[RANDOM_LONG(0, ARRAYSIZE(pAlertSounds) - 1)], 1.0, ATTN_NORM, 0, pitch);
}

void CZombie::IdleSound(void)
{
    EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pIdleSounds[RANDOM_LONG(0, ARRAYSIZE(pIdleSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}

void CZombie::AttackSound(void)
{
    EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, pAttackSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
}

void CZombie::HandleAnimEvent(MonsterEvent_t *pEvent)
{
    switch (pEvent->event)
    {
    case ZOMBIE_AE_ATTACK_RIGHT:
    {
        CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.zombieDmgOneSlash, DMG_SLASH);
        if (pHurt)
        {
            if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
            {
                pHurt->pev->punchangle.z = -18;
                pHurt->pev->punchangle.x = 5;
                pHurt->pev->velocity = pHurt->pev->velocity - gpGlobals->v_right * 100;
            }
            if (g_pGameRules->IsMultiplayer())
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSoundsShort[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSoundsShort) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
            else
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
        }
        else
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackMissSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));

        if (RANDOM_LONG(0, 1))
            AttackSound();
    }
    break;

    case ZOMBIE_AE_ATTACK_LEFT:
    {
        CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.zombieDmgOneSlash, DMG_SLASH);
        if (pHurt)
        {
            if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
            {
                pHurt->pev->punchangle.z = 18;
                pHurt->pev->punchangle.x = 5;
                pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_right * 100;
            }
            if (g_pGameRules->IsMultiplayer())
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSoundsShort[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSoundsShort) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
            else
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
        }
        else
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackMissSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));

        if (RANDOM_LONG(0, 1))
            AttackSound();
    }
    break;

    case ZOMBIE_AE_ATTACK_BOTH:
    {
        CBaseEntity *pHurt = CheckTraceHullAttack(70, gSkillData.zombieDmgBothSlash, DMG_SLASH);
        if (pHurt)
        {
            if (pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
            {
                pHurt->pev->punchangle.x = 5;
                pHurt->pev->velocity = pHurt->pev->velocity + gpGlobals->v_forward * -100;
            }
            if (g_pGameRules->IsMultiplayer())
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSoundsShort[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSoundsShort) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
            else
                EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackHitSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackHitSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));
        }
        else
            EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, pAttackMissSounds[RANDOM_LONG(0, ARRAYSIZE(pAttackMissSounds) - 1)], 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-5, 5));

        if (RANDOM_LONG(0, 1))
            AttackSound();
    }
    break;

    default:
        CBaseMonster::HandleAnimEvent(pEvent);
        break;
    }
}

void CZombie::Spawn()
{
    Precache();

    SET_MODEL(ENT(pev), "models/zombie.dol");
    UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

    pev->solid = SOLID_SLIDEBOX;
    pev->movetype = MOVETYPE_STEP;
    m_bloodColor = BLOOD_COLOR_GREEN;
    pev->health = gSkillData.zombieHealth;
    pev->view_ofs = VEC_VIEW;
    m_flFieldOfView = 0.5;
    m_MonsterState = MONSTERSTATE_NONE;
    m_afCapability = bits_CAP_DOORS_GROUP;

    MonsterInit();
}

void CZombie::Precache()
{
    int i;

    PRECACHE_MODEL("models/zombie.dol");

    if (g_pGameRules->IsMultiplayer())
    {
        for (i = 0; i < ARRAYSIZE(pAttackHitSoundsShort); i++)
            PRECACHE_SOUND((char *)pAttackHitSoundsShort[i]);
    }
    else
    {
        for (i = 0; i < ARRAYSIZE(pAttackHitSounds); i++)
            PRECACHE_SOUND((char *)pAttackHitSounds[i]);
    }

    for (i = 0; i < ARRAYSIZE(pAttackMissSounds); i++)
        PRECACHE_SOUND((char *)pAttackMissSounds[i]);

    for (i = 0; i < ARRAYSIZE(pAttackSounds); i++)
        PRECACHE_SOUND((char *)pAttackSounds[i]);

    for (i = 0; i < ARRAYSIZE(pIdleSounds); i++)
        PRECACHE_SOUND((char *)pIdleSounds[i]);

    if (g_pGameRules->IsMultiplayer())
    {
        for (i = 0; i < ARRAYSIZE(pAlertSoundsShort); i++)
            PRECACHE_SOUND((char *)pAlertSoundsShort[i]);
    }
    else
    {
        for (i = 0; i < ARRAYSIZE(pAlertSounds); i++)
            PRECACHE_SOUND((char *)pAlertSounds[i]);
    }

    for (i = 0; i < ARRAYSIZE(pPainSounds); i++)
        PRECACHE_SOUND((char *)pPainSounds[i]);
}

int CZombie::IgnoreConditions(void)
{
    int iIgnore = CBaseMonster::IgnoreConditions();

    if ((m_Activity == ACT_MELEE_ATTACK1) || (m_Activity == ACT_MELEE_ATTACK1))
    {
        if (m_flNextFlinch >= gpGlobals->time)
            iIgnore |= (bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE);
    }

    if ((m_Activity == ACT_SMALL_FLINCH) || (m_Activity == ACT_BIG_FLINCH))
    {
        if (m_flNextFlinch < gpGlobals->time)
            m_flNextFlinch = gpGlobals->time + ZOMBIE_FLINCH_DELAY;
    }

    return iIgnore;
}
