/* dlls/warpball.cpp: env_warpball (CWarpBall), a bola de teletransporte
   alienígena do Opposing Force/Decay. Não existe no HLSDK; no PS2 ela também
   faz o papel de monstermaker: cria o monstro de "monstertype" no meio do
   efeito e conta os filhos vivos como o CMonsterMaker. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/effects.h"
#include "dlls/saverestore.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define SF_WARPBALL_REMOVE_ON_FIRE  0x0001
#define SF_WARPBALL_KILL_CENTER     0x0002

/* variante do UTIL_TraceLine em util.cpp que chama o pfnTraceLine (0x84)
   sem a flag de vidro; o nome original é desconhecido */
void UTIL_TraceLineNoEnts(const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr);
void UTIL_ScreenShake(const Vector &center, float amplitude, float frequency, float duration, float radius);
void RadiusDamage(Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType);
void DispatchSpawn(edict_t *pent);

/* o CLightning (env_beam) fica no effects.cpp */
class CLightning : public CBeam
{
public:
    void EXPORT StrikeThink(void);
    void BeamUpdateVars(void);
    static CLightning *LightningCreate(const char *pSpriteName, int width);

    int m_active;               /* 0x30 */
    int m_iszStartEntity;       /* 0x34 */
    int m_iszEndEntity;         /* 0x38 */
    float m_life;               /* 0x3C */
    int m_boltWidth;            /* 0x40 */
    int m_noiseAmplitude;       /* 0x44 */
    int m_brightness;           /* 0x48 */
    int m_speed;                /* 0x4C */
    float m_restrike;           /* 0x50 */
    int m_spriteTexture;        /* 0x54 */
    int m_iszSpriteName;        /* 0x58 */
    int m_frameStart;           /* 0x5C */
    float m_radius;             /* 0x60 */
};

class CWarpBall : public CBaseEntity
{
public:
    void Spawn(void);
    void Precache(void);
    void KeyValue(KeyValueData *pkvd);
    int Classify(void);
    void DeathNotice(entvars_t *pevChild);

    void EXPORT BallThink(void);
    void EXPORT WarpBallUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void EXPORT WarpBallThink(void);
    void MakeMonster(void);

    static CWarpBall *CreateWarpBall(const Vector &vecOrigin);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    CLightning *m_pBeams;           /* 0x30 */
    int m_iBeams;                   /* 0x34 */
    float m_flLastTime;             /* 0x38 */
    float m_flMaxFrame;             /* 0x3C */
    float m_flBeamRadius;           /* 0x40 */
    int m_iszWarpTarget;            /* 0x44 */
    float m_flWarpStart;            /* 0x48 */
    float m_flDamageDelay;          /* 0x4C */
    float m_flTargetDelay;          /* 0x50 */
    BOOL m_fPlaying;                /* 0x54 */
    BOOL m_fDamageApplied;          /* 0x58 */
    BOOL m_fBeamsCleared;           /* 0x5C */
    float m_flGround;               /* 0x60 */
    int m_iszMonsterClassname;      /* 0x64 */
    int m_iMonsterSpawnFlags;       /* 0x68 */
    int m_fMonsterCreated;          /* 0x6C */
    int m_cLiveChildren;            /* 0x70 */
    int m_iMaxLiveChildren;         /* 0x74 */
    BOOL m_fFadeChildren;           /* 0x78 */
    BOOL m_fWarpRefire;             /* 0x7C */
};

LINK_ENTITY_TO_CLASS(env_warpball, CWarpBall);

TYPEDESCRIPTION CWarpBall::m_SaveData[] =
{
    DEFINE_FIELD(CWarpBall, m_pBeams, FIELD_CLASSPTR),
    DEFINE_FIELD(CWarpBall, m_iBeams, FIELD_INTEGER),
    DEFINE_FIELD(CWarpBall, m_flLastTime, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_flMaxFrame, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_flBeamRadius, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_iszWarpTarget, FIELD_STRING),
    DEFINE_FIELD(CWarpBall, m_flWarpStart, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_flDamageDelay, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_flTargetDelay, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_fPlaying, FIELD_BOOLEAN),
    DEFINE_FIELD(CWarpBall, m_fDamageApplied, FIELD_BOOLEAN),
    DEFINE_FIELD(CWarpBall, m_fBeamsCleared, FIELD_BOOLEAN),
    DEFINE_FIELD(CWarpBall, m_flGround, FIELD_FLOAT),
    DEFINE_FIELD(CWarpBall, m_iszMonsterClassname, FIELD_STRING),
    DEFINE_FIELD(CWarpBall, m_iMonsterSpawnFlags, FIELD_INTEGER),
    DEFINE_FIELD(CWarpBall, m_fMonsterCreated, FIELD_INTEGER),
    DEFINE_FIELD(CWarpBall, m_cLiveChildren, FIELD_INTEGER),
    DEFINE_FIELD(CWarpBall, m_iMaxLiveChildren, FIELD_INTEGER),
    DEFINE_FIELD(CWarpBall, m_fFadeChildren, FIELD_BOOLEAN),
    DEFINE_FIELD(CWarpBall, m_fWarpRefire, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CWarpBall, CBaseEntity);

CWarpBall *CWarpBall::CreateWarpBall(const Vector &vecOrigin)
{
    CWarpBall *pWarpBall = GetClassPtr((CWarpBall *)NULL);

    UTIL_SetOrigin(pWarpBall->pev, vecOrigin);
    pWarpBall->pev->classname = MAKE_STRING("env_warpball");
    pWarpBall->Spawn();

    return pWarpBall;
}

void CWarpBall::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "radius"))
    {
        m_flBeamRadius = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "warp_target"))
    {
        m_iszWarpTarget = ALLOC_STRING(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "damage_delay"))
    {
        m_flDamageDelay = atof(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "monstertype"))
    {
        if (pkvd->szValue && *pkvd->szValue)
            m_iszMonsterClassname = ALLOC_STRING(pkvd->szValue);
        else
            m_iszMonsterClassname = 0;
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "monsterspawnflags"))
    {
        m_iMonsterSpawnFlags = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "maxlivechildren"))
    {
        m_iMaxLiveChildren = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else
        CBaseEntity::KeyValue(pkvd);
}

void CWarpBall::Spawn(void)
{
    Precache();

    pev->movetype = MOVETYPE_NONE;
    pev->solid = SOLID_NOT;
    UTIL_SetOrigin(pev, pev->origin);
    UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

    pev->rendermode = kRenderGlow;
    pev->renderamt = 255;
    pev->renderfx = kRenderFxNoDissipation;
    pev->framerate = 10;

    SetUse(&CWarpBall::WarpBallUse);

    if ((pev->spawnflags & SF_WARPBALL_KILL_CENTER) && m_flDamageDelay == 0)
        m_flDamageDelay = 1.0f;
}

void CWarpBall::BallThink(void)
{
    pev->frame += pev->framerate * (gpGlobals->time - m_flLastTime);

    if (pev->frame > m_flMaxFrame)
    {
        SET_MODEL(ENT(pev), "");
        SetThink(NULL);

        if (pev->spawnflags & SF_WARPBALL_REMOVE_ON_FIRE)
            UTIL_Remove(this);

        m_fPlaying = FALSE;

        if (m_fWarpRefire)
        {
            SetThink(&CWarpBall::WarpBallThink);
            pev->nextthink = gpGlobals->time + RANDOM_FLOAT(2, 4);
        }
        return;
    }

    float flMonsterDelay = max(m_flDamageDelay + 0.2f, 1.0f);

    if (m_iszMonsterClassname && !m_fMonsterCreated && gpGlobals->time - m_flWarpStart >= flMonsterDelay)
    {
        MakeMonster();
        m_fMonsterCreated = TRUE;
    }

    if (pev->frame >= m_flMaxFrame - 4)
    {
        m_pBeams->SetThink(NULL);
        m_pBeams->pev->nextthink = gpGlobals->time;
    }

    pev->nextthink = gpGlobals->time + 0.1f;
    m_flLastTime = gpGlobals->time;
}

void CWarpBall::Precache(void)
{
    PRECACHE_MODEL("sprites/warpball.spz");
    PRECACHE_MODEL("sprites/lgtning.spz");
    PRECACHE_SOUND("debris/alien_teleport.wav");

    if (m_iszMonsterClassname)
        UTIL_PrecacheOther(STRING(m_iszMonsterClassname));
}

void CWarpBall::WarpBallUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    WarpBallThink();
}

void CWarpBall::WarpBallThink(void)
{
    if (m_fPlaying)
        return;

    if (m_iMaxLiveChildren > 0 && m_cLiveChildren >= m_iMaxLiveChildren)
        return;

    if (m_iszWarpTarget)
    {
        edict_t *pTarget = FIND_ENTITY_BY_TARGETNAME(NULL, STRING(m_iszWarpTarget));
        if (pTarget)
            UTIL_SetOrigin(pev, pTarget->v.origin);
    }

    if (!(pev->spawnflags & SF_WARPBALL_KILL_CENTER))
    {
        if (m_flGround == 0)
        {
            TraceResult tr;
            UTIL_TraceLineNoEnts(pev->origin, pev->origin - Vector(0, 0, 2048), ignore_monsters, ENT(pev), &tr);
            m_flGround = tr.vecEndPos.z;
        }

        Vector mins = pev->origin - Vector(34, 34, 0);
        Vector maxs = pev->origin + Vector(34, 34, 0);
        maxs.z = pev->origin.z;
        mins.z = m_flGround;

        CBaseEntity *pList[2];
        if (UTIL_EntitiesInBox(pList, 2, mins, maxs, FL_CLIENT | FL_MONSTER))
        {
            SetThink(&CWarpBall::WarpBallThink);
            pev->nextthink = gpGlobals->time + RANDOM_FLOAT(2, 4);
            return;
        }
    }

    m_fWarpRefire = FALSE;

    SET_MODEL(ENT(pev), "sprites/warpball.spz");
    m_flMaxFrame = (float)MODEL_FRAMES(pev->modelindex) - 1;
    pev->rendercolor.x = 77;
    pev->rendercolor.y = 210;
    pev->rendercolor.z = 130;
    pev->scale = 1.2f;
    pev->frame = 0;

    const char *pszBeamSprite = "sprites/lgtning.spz";
    if (!m_pBeams)
    {
        m_pBeams = CLightning::LightningCreate(pszBeamSprite, 18);
        if (m_pBeams)
        {
            m_pBeams->m_iszSpriteName = MAKE_STRING(pszBeamSprite);
            m_pBeams->pev->origin = pev->origin;
            UTIL_SetOrigin(m_pBeams->pev, pev->origin);

            if (g_pGameRules->IsMultiplayer())
                m_pBeams->m_restrike = -0.3f;
            else
                m_pBeams->m_restrike = -0.5f;

            m_pBeams->m_noiseAmplitude = 65;
            m_pBeams->m_boltWidth = 18;
            m_pBeams->m_life = 0.5f;
            m_pBeams->SetColor(0, 255, 0);
            m_pBeams->pev->spawnflags |= SF_BEAM_SPARKEND;
            m_pBeams->pev->spawnflags |= SF_BEAM_TOGGLE;
            m_pBeams->m_radius = m_flBeamRadius;
            m_pBeams->m_iszStartEntity = pev->targetname;
            m_pBeams->BeamUpdateVars();
        }
    }

    if (m_pBeams)
    {
        m_pBeams->pev->solid = SOLID_NOT;
        m_pBeams->Precache();
        m_pBeams->SetThink(&CLightning::StrikeThink);
        m_pBeams->pev->nextthink = gpGlobals->time + 0.1f;
    }

    SetThink(&CWarpBall::BallThink);
    pev->nextthink = gpGlobals->time + 0.1f;
    m_flLastTime = gpGlobals->time;
    m_fBeamsCleared = FALSE;
    m_fPlaying = TRUE;

    if ((pev->spawnflags & SF_WARPBALL_KILL_CENTER) && m_flDamageDelay == 0)
    {
        RadiusDamage(pev->origin, pev, pev, 300, 48, CLASS_NONE, DMG_SHOCK);
        m_fDamageApplied = TRUE;
    }
    else
        m_fDamageApplied = FALSE;

    SUB_UseTargets(this, USE_TOGGLE, 0);
    UTIL_ScreenShake(pev->origin, 4, 100, 2, 1000);
    m_flWarpStart = gpGlobals->time;
    EMIT_SOUND(ENT(pev), CHAN_WEAPON, "debris/alien_teleport.wav", 1, ATTN_NORM);
    m_fMonsterCreated = FALSE;
}

int CWarpBall::Classify(void)
{
    return CLASS_NONE;
}

#ifdef NON_MATCHING
/* só a troca de v0/v1 nas leituras de pev depois do DispatchSpawn */
void CWarpBall::MakeMonster(void)
{
    edict_t *pent;
    entvars_t *pevCreate;

    if (m_iMaxLiveChildren > 0 && m_cLiveChildren >= m_iMaxLiveChildren)
        return;

    if (m_flGround == 0)
    {
        TraceResult tr;
        UTIL_TraceLineNoEnts(pev->origin, pev->origin - Vector(0, 0, 2048), ignore_monsters, ENT(pev), &tr);
        m_flGround = tr.vecEndPos.z;
    }

    Vector mins = pev->origin - Vector(34, 34, 0);
    Vector maxs = pev->origin + Vector(34, 34, 0);
    maxs.z = pev->origin.z;
    mins.z = m_flGround;

    CBaseEntity *pList[2];
    int count = UTIL_EntitiesInBox(pList, 2, mins, maxs, FL_CLIENT | FL_MONSTER);
    if (count)
    {
        if (pev->spawnflags & SF_WARPBALL_KILL_CENTER)
        {
            for (int i = 0; i < count; i++)
                pList[i]->TakeDamage(pev, pev, 300, DMG_SHOCK);
        }
        else
        {
            m_fWarpRefire = TRUE;
            return;
        }
    }

    pent = CREATE_NAMED_ENTITY(m_iszMonsterClassname);

    if (FNullEnt(pent))
    {
        ALERT(at_console, "NULL Ent in MonsterMaker!\n");
        return;
    }

    pevCreate = VARS(pent);
    pevCreate->origin = pev->origin;
    pevCreate->angles = pev->angles;
    pevCreate->spawnflags = m_iMonsterSpawnFlags;
    SetBits(pevCreate->spawnflags, SF_MONSTER_FALL_TO_GROUND);

    DispatchSpawn(ENT(pevCreate));
    pevCreate->owner = edict();

    if (!FStringNull(pev->netname))
        pevCreate->targetname = pev->netname;

    m_cLiveChildren++;
}
#else
/* o asm traz junto a string do ALERT (o splat a migrou para o .s) */
INCLUDE_ASM("asm/nonmatchings/dlls/warpball", MakeMonster__9CWarpBall);
#endif

void CWarpBall::DeathNotice(entvars_t *pevChild)
{
    m_cLiveChildren--;

    if (!m_fFadeChildren)
        pevChild->owner = NULL;
}

extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0020C260(void)
{
    func_002397D0("335", static_cast<BASEPTR>(&CWarpBall::BallThink));
    func_00239810("336", static_cast<USEPTR>(&CWarpBall::WarpBallUse));
    func_002397D0("400", static_cast<BASEPTR>(&CWarpBall::WarpBallThink));
}
