/* dlls/weapon_islave.cpp: o vortigaunt jogável do Half-Life: Decay
   (weapon_islave). Não existe no HLSDK: a garra é o Swing do crowbar.cpp e
   a carga elétrica é o ArmBeam/ZapBeam do islave.cpp, tudo a partir do
   jogador. trigger_player_islave dispara os alvos quando o jogador vira
   vortigaunt. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/skill.h"
#include "dlls/saverestore.h"
#include "dlls/gamerules.h"
#include "dlls/effects.h"
#include "include_asm.h"

#define WEAPON_ISLAVE       16

#define ISLAVE_MAX_BEAMS    8

extern "C" int cl_numlocal;
extern "C" char *strstr(const char *, const char *);
extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);

void FindHullIntersection(const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity);

extern int gmsgSlaveHud;

enum islave_e
{
    ISLAVE_IDLE1 = 0,
    ISLAVE_IDLE2,
    ISLAVE_ATTACK1HIT,
    ISLAVE_ATTACK1MISS,
    ISLAVE_ATTACK2MISS,
    ISLAVE_ATTACK2HIT,
    ISLAVE_ATTACK3MISS,
    ISLAVE_ATTACK3HIT,
    ISLAVE_CHARGE,
    ISLAVE_CHARGE_LOOP,
    ISLAVE_ZAP,
    ISLAVE_ZAP_CANCEL,
};

class CWeaponISlave : public CBasePlayerWeapon
{
public:
    void Spawn(void);
    void Precache(void);
    int iItemSlot(void) { return 1; }
    void EXPORT Smack(void);
    int GetItemInfo(ItemInfo *p);

    void PrimaryAttack(void);
    void SecondaryAttack(void);
    BOOL Deploy(void);
    void Holster(void);
    void WeaponIdle(void);

    void StartZap(void);
    void ArmBeam(int side);
    void BeamGlow(void);
    void ClearBeams(void);
    void ZapAttack(void);
    void EXPORT ZapDone(void);
    void ZapBeam(int side);
    void CancelZap(void);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    int m_iSwing;                       /* 0x8C */
    TraceResult m_trHit;                /* 0x90 */
    float m_flZapTime;                  /* 0xC8 */
    CBeam *m_pBeam[ISLAVE_MAX_BEAMS];   /* 0xCC */
    CBeam *m_pZapBeam[4];               /* 0xEC */
    int m_iBeams;                       /* 0xFC */
    float m_flUpdateBeamTime;           /* 0x100 */
    int m_iHudState;                    /* 0x104 */
};

void ISlave_DebugPrint(const char *psz, int i)
{
    ALERT(at_console, "%s %d\n", psz, i);
}

LINK_ENTITY_TO_CLASS(weapon_islave, CWeaponISlave);

TYPEDESCRIPTION CWeaponISlave::m_SaveData[] =
{
    DEFINE_FIELD(CWeaponISlave, m_iSwing, FIELD_INTEGER),
    DEFINE_FIELD(CWeaponISlave, m_flZapTime, FIELD_TIME),
    DEFINE_FIELD(CWeaponISlave, m_flUpdateBeamTime, FIELD_TIME),
    DEFINE_FIELD(CWeaponISlave, m_iHudState, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CWeaponISlave, CBasePlayerWeapon);

const char *pGibberishSounds[] =
{
    "scientist/gibberish01.wav",
    "scientist/gibberish02.wav",
    "scientist/gibberish03.wav",
};

void CWeaponISlave::Spawn(void)
{
    Precache();
    m_iId = WEAPON_ISLAVE;
    SET_MODEL(ENT(pev), "models/w_crowbar.dol");
    m_iClip = -1;
    m_flUpdateBeamTime = 0;
    m_iHudState = 0;

    FallInit();
}

void CWeaponISlave::Precache(void)
{
    PRECACHE_MODEL("models/v_slave.dol");
    PRECACHE_MODEL("models/w_crowbar.dol");
    PRECACHE_MODEL("models/p_crowbar.dol");
    PRECACHE_MODEL("sprites/lgtning.spz");
    PRECACHE_SOUND("debris/zap1.wav");
    PRECACHE_SOUND("debris/zap4.wav");
    PRECACHE_SOUND("zombie/claw_strike1.wav");
    PRECACHE_SOUND("zombie/claw_strike2.wav");
    PRECACHE_SOUND("zombie/claw_strike3.wav");
    PRECACHE_SOUND("zombie/claw_miss1.wav");
    PRECACHE_SOUND("zombie/claw_miss2.wav");
    PRECACHE_SOUND("aslave/slv_word5.wav");
    PRECACHE_SOUND("aslave/slv_word7.wav");
    PRECACHE_SOUND("aslave/slv_word3.wav");
    PRECACHE_SOUND("aslave/slv_word4.wav");
    PRECACHE_SOUND("scientist/gibberish01.wav");
    PRECACHE_SOUND("scientist/gibberish02.wav");
    PRECACHE_SOUND("scientist/gibberish03.wav");

    int i;
    for (i = 0; i < ISLAVE_MAX_BEAMS; i++)
    {
        if (m_pBeam[i])
            m_pBeam[i] = NULL;
    }
    m_iBeams = 0;

    int j;
    for (j = 0; j < 4; j++)
    {
        if (m_pZapBeam[j])
            m_pZapBeam[j] = NULL;
    }
}

/* chamado pelo EMIT_SOUND_DYN: com o jogador de vortigaunt, as falas dos
   cientistas e dos seguranças viram algaravia */
BOOL ISlave_EmitSound(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch)
{
    if (!sample)
        return FALSE;

    CBaseEntity *pPlayer = CBaseEntity::Instance(INDEXENT(1));
    if (!pPlayer)
        return FALSE;

    if (!pPlayer->IsPlayer())
        return FALSE;

    if (!(((CBasePlayer *)pPlayer)->m_afPhysicsFlags & PFLAG_PS2_NOPICKUP))
        return FALSE;

    if (!strstr(sample, "scientist") && !strstr(sample, "!SC") && !strstr(sample, "barney") && !strstr(sample, "!BA"))
        return FALSE;

    if (strstr(sample, "pain"))
        return FALSE;

    if (strstr(sample, "scream"))
        return FALSE;

    EMIT_SOUND_DYN1(entity, channel, pGibberishSounds[RANDOM_LONG(0, 2)], volume, attenuation, flags, pitch);
    return TRUE;
}

int CWeaponISlave::GetItemInfo(ItemInfo *p)
{
    p->pszName = STRING(pev->classname);
    p->pszAmmo1 = NULL;
    p->iMaxAmmo1 = -1;
    p->pszAmmo2 = NULL;
    p->iMaxAmmo2 = -1;
    p->iMaxClip = WEAPON_NOCLIP;
    p->iSlot = 0;
    p->iPosition = 2;
    p->iId = WEAPON_ISLAVE;
    p->iWeight = -1;
    return 1;
}

BOOL CWeaponISlave::Deploy(void)
{
    m_pPlayer->m_iFOV = 120;
    m_pPlayer->m_afPhysicsFlags |= PFLAG_PS2_NOPICKUP;
    m_pPlayer->m_afPhysicsFlags |= PFLAG_DUCKING;
    m_iHudState = 0;

    MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
    WRITE_BYTE(m_iHudState);
    MESSAGE_END();

    return DefaultDeploy("models/v_slave.dol", NULL, ISLAVE_IDLE1, "crowbar");
}

void CWeaponISlave::Holster(void)
{
    if (gEvilImpulse101)
    {
        m_pPlayer->m_afPhysicsFlags &= ~PFLAG_PS2_NOPICKUP;
        m_pPlayer->m_afPhysicsFlags &= ~PFLAG_DUCKING;
        m_pPlayer->m_iFOV = 90;
    }

    m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;
    ClearBeams();
    m_flZapTime = 0;
    m_iHudState = 0;

    MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
    WRITE_BYTE(m_iHudState);
    MESSAGE_END();

    m_pPlayer->pev->viewmodel = 0;
    m_pPlayer->pev->weaponmodel = 0;
}

#ifdef NON_MATCHING
/* o alvo converte 13.0f em int em tempo de execução (trunc.w.s); aqui vira li 13 */
void CWeaponISlave::StartZap(void)
{
    const float flTime = 1.3f;

    m_flZapTime = gpGlobals->time + flTime;
    SendWeaponAnim(ISLAVE_CHARGE);

    Vector vecSrc = m_pPlayer->GetGunPosition();

    if (cl_numlocal == 1)
    {
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
        WRITE_BYTE(TE_DLIGHT);
        WRITE_COORD(vecSrc.x);
        WRITE_COORD(vecSrc.y);
        WRITE_COORD(vecSrc.z);
        WRITE_BYTE(12);
        WRITE_BYTE(255);
        WRITE_BYTE(180);
        WRITE_BYTE(96);
        WRITE_BYTE(flTime * 10);
        WRITE_BYTE(0);
        MESSAGE_END();
    }

    m_iHudState = 2;

    MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
    WRITE_BYTE(m_iHudState);
    MESSAGE_END();

    m_flUpdateBeamTime = gpGlobals->time;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/weapon_islave", StartZap__13CWeaponISlave);
#endif

void CWeaponISlave::ArmBeam(int side)
{
    TraceResult tr;
    float flDist = 1.0f;

    if (m_iBeams >= ISLAVE_MAX_BEAMS)
        return;

    UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);
    Vector vecSrc = m_pPlayer->GetGunPosition() + gpGlobals->v_up * 36 + gpGlobals->v_right * side * 16 + gpGlobals->v_forward * 32;

    for (int i = 0; i < 3; i++)
    {
        Vector vecAim = gpGlobals->v_right * side * RANDOM_FLOAT(0, 1) + gpGlobals->v_up * RANDOM_FLOAT(-1, 1);
        TraceResult tr1;
        UTIL_TraceLine(vecSrc, vecSrc + vecAim * 512, dont_ignore_monsters, ENT(pev), &tr1);
        if (flDist > tr1.flFraction)
        {
            tr = tr1;
            flDist = tr.flFraction;
        }
    }

    if (flDist == 1.0f)
        return;

    m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spz", 30);
    if (!m_pBeam[m_iBeams])
        return;

    m_pBeam[m_iBeams]->PointEntInit(tr.vecEndPos, m_pPlayer->entindex());
    m_pBeam[m_iBeams]->SetEndAttachment(side < 0 ? 2 : 1);
    m_pBeam[m_iBeams]->SetColor(96, 128, 16);
    m_pBeam[m_iBeams]->SetBrightness(64);
    m_pBeam[m_iBeams]->SetNoise(80);
    m_pBeam[m_iBeams]->pev->spawnflags |= SF_BEAM_TEMPORARY;
    m_iBeams++;
}

void CWeaponISlave::BeamGlow(void)
{
    int b = m_iBeams * 32;
    if (b > 255)
        b = 255;

    for (int i = 0; i < m_iBeams; i++)
    {
        if (m_pBeam[i]->GetBrightness() != 255)
        {
            m_pBeam[i]->SetBrightness(b);
        }
    }
}

void CWeaponISlave::ClearBeams(void)
{
    int i;
    for (i = 0; i < ISLAVE_MAX_BEAMS; i++)
    {
        if (m_pBeam[i])
        {
            UTIL_Remove(m_pBeam[i]);
            m_pBeam[i] = NULL;
        }
    }
    m_iBeams = 0;
    pev->skin = 0;

    for (i = 0; i < 4; i++)
    {
        if (m_pZapBeam[i])
        {
            UTIL_Remove(m_pZapBeam[i]);
            m_pZapBeam[i] = NULL;
        }
    }

    STOP_SOUND(ENT(pev), CHAN_WEAPON, "debris/zap4.wav");
}

void CWeaponISlave::ZapAttack(void)
{
    SendWeaponAnim(ISLAVE_ZAP);
    m_flNextSecondaryAttack = gpGlobals->time + 1.0f;
    m_flTimeWeaponIdle = gpGlobals->time + 5.0f;

    ZapBeam(1);
    ZapBeam(-1);

    for (int i = 0; i < ISLAVE_MAX_BEAMS; i++)
    {
        if (m_pBeam[i])
        {
            UTIL_Remove(m_pBeam[i]);
            m_pBeam[i] = NULL;
        }
    }
    m_iBeams = 0;
    pev->skin = 0;
    m_flZapTime = 0;

    SetThink(&CWeaponISlave::ZapDone);
    pev->nextthink = gpGlobals->time + 0.3f;
}

void CWeaponISlave::ZapDone(void)
{
    m_iHudState = 0;
    ClearBeams();
}

Vector ISlave_SpreadVector(Vector vecDir, Vector vecSpread)
{
    Vector vecForward, vecRight, vecUp;
    float x, y, z;

    g_engfuncs.pfnAngleVectors(vecDir, vecForward, vecRight, vecUp);

    do
    {
        x = RANDOM_FLOAT(-0.5f, 0.5f) + RANDOM_FLOAT(-0.5f, 0.5f);
        y = RANDOM_FLOAT(-0.5f, 0.5f) + RANDOM_FLOAT(-0.5f, 0.5f);
        z = x * x + y * y;
    } while (z > 1);

    Vector vecResult = vecDir + x * vecSpread.x * vecRight + y * vecSpread.y * vecUp;
    return vecResult;
}

void CWeaponISlave::ZapBeam(int side)
{
    Vector vecSrc, vecAngles;
    TraceResult tr;
    int iBeam = side > 0;

    vecSrc = m_pPlayer->GetGunPosition();
    vecAngles = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
    UTIL_MakeVectors(vecAngles);

    Vector vecAim = gpGlobals->v_forward;
    if (side < 0)
        vecAim = ISlave_SpreadVector(vecAim, VECTOR_CONE_5DEGREES);

    UTIL_TraceLine(vecSrc, vecSrc + vecAim * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

    m_pZapBeam[iBeam] = CBeam::BeamCreate("sprites/lgtning.spz", 50);
    if (!m_pZapBeam[iBeam])
        return;

    float flCharge = (1.3f - __builtin_fabsf(m_flZapTime - gpGlobals->time)) * (1 / 1.3f);
    if (flCharge > 1.0f || m_iHudState == 3)
        flCharge = 1.0f;

    float flDamage = gSkillData.plrDmgISlaveZap * flCharge;
    float flBrightness = flCharge * 255;

    m_pZapBeam[iBeam]->PointEntInit(tr.vecEndPos, m_pPlayer->entindex());
    m_pZapBeam[iBeam]->SetEndAttachment(side < 0 ? 2 : 1);
    m_pZapBeam[iBeam]->SetColor(180, 255, 96);
    m_pZapBeam[iBeam]->SetBrightness(flBrightness);
    m_pZapBeam[iBeam]->SetNoise(20);
    m_pZapBeam[iBeam]->pev->spawnflags |= SF_BEAM_TEMPORARY;
    m_pPlayer->m_ps2ShotsFired++;

    CBaseEntity *pEntity;

    if (flCharge >= 0.95f)
    {
        /* carga cheia: um segundo raio mais aberto */
        TraceResult tr2;
        Vector vecAim2 = ISlave_SpreadVector(gpGlobals->v_forward, VECTOR_CONE_15DEGREES);
        UTIL_TraceLine(vecSrc, vecSrc + vecAim2 * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr2);

        pEntity = CBaseEntity::Instance(tr2.pHit);
        m_pPlayer->m_ps2ShotsFired++;
        if (pEntity != NULL && pEntity->pev->takedamage && !m_pPlayer->pev->deadflag)
        {
            ClearMultiDamage();
            pEntity->TraceAttack(m_pPlayer->pev, flDamage * 0.5f, gpGlobals->v_forward, &tr, DMG_SHOCK);
            ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

            if (!pEntity->IsPlayer() && !m_pPlayer->pev->deadflag)
                m_pPlayer->TakeHealth((100 - m_pPlayer->pev->health) * 0.1f, DMG_GENERIC);

            if (g_pGameRules->FPlayerCanTakeDamage(m_pPlayer, pEntity))
                m_pPlayer->m_ps2ShotsHit++;
        }

        m_pZapBeam[iBeam + 2] = CBeam::BeamCreate("sprites/lgtning.spz", 65);
        if (!m_pZapBeam[iBeam])
            return;

        m_pZapBeam[iBeam + 2]->PointEntInit(tr2.vecEndPos, m_pPlayer->entindex());
        m_pZapBeam[iBeam + 2]->SetEndAttachment(side < 0 ? 2 : 1);
        m_pZapBeam[iBeam + 2]->SetColor(130, 255, 70);
        m_pZapBeam[iBeam + 2]->SetBrightness(255);
        m_pZapBeam[iBeam + 2]->SetNoise(30);
        m_pZapBeam[iBeam + 2]->pev->spawnflags |= SF_BEAM_TEMPORARY;

        UTIL_DecalTrace(&tr2, RANDOM_LONG(28, 32));
    }

    pEntity = CBaseEntity::Instance(tr.pHit);
    if (pEntity != NULL && pEntity->pev->takedamage && !m_pPlayer->pev->deadflag)
    {
        ClearMultiDamage();
        pEntity->TraceAttack(m_pPlayer->pev, flDamage * 0.5f, gpGlobals->v_forward, &tr, DMG_SHOCK);
        ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

        if (!pEntity->IsPlayer() && !m_pPlayer->pev->deadflag)
            m_pPlayer->TakeHealth((100 - m_pPlayer->pev->health) * 0.1f, DMG_GENERIC);

        if (g_pGameRules->FPlayerCanTakeDamage(m_pPlayer, pEntity))
            m_pPlayer->m_ps2ShotsHit++;
    }

    if (g_pGameRules->IsMultiplayer() || g_pGameRules->Ps2Virt07())
    {
        if (g_pGameRules->IsMultiplayer() && cl_numlocal == 1)
        {
            MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
            WRITE_BYTE(m_pPlayer->m_ps2ClientIndex);
            WRITE_SHORT(m_pPlayer->pev->frags);
            WRITE_SHORT(m_pPlayer->m_iDeaths);
            WRITE_SHORT(m_pPlayer->m_ps2Stat5F8);
            WRITE_SHORT(m_pPlayer->m_ps2ShotsFired);
            WRITE_SHORT(m_pPlayer->m_ps2ShotsHit);
            MESSAGE_END();
        }
        else
        {
            MESSAGE_BEGIN(MSG_ALL, gmsgScoreInfo);
            WRITE_BYTE(ENTINDEX(m_pPlayer->edict()));
            WRITE_SHORT(m_pPlayer->pev->frags);
            WRITE_SHORT(m_pPlayer->m_iDeaths);
            WRITE_SHORT(m_pPlayer->m_ps2Stat5F8);
            WRITE_SHORT(m_pPlayer->m_ps2ShotsFired);
            WRITE_SHORT(m_pPlayer->m_ps2ShotsHit);
            MESSAGE_END();
        }
    }

    UTIL_DecalTrace(&tr, RANDOM_LONG(28, 32));
}

void CWeaponISlave::CancelZap(void)
{
    m_flNextSecondaryAttack = gpGlobals->time + 1.5f;
    m_flZapTime = 0;
    m_iHudState = 0;

    MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
    WRITE_BYTE(m_iHudState);
    MESSAGE_END();

    SendWeaponAnim(ISLAVE_ZAP_CANCEL);
    ClearBeams();
}

#ifdef NON_MATCHING
/* só o int do volume: o alvo faz trunc + mfc1 + sw, aqui sai swc1 direto */
void CWeaponISlave::PrimaryAttack(void)
{
    TraceResult tr;

    UTIL_MakeVectors(m_pPlayer->pev->v_angle);
    Vector vecSrc = m_pPlayer->GetGunPosition();
    Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

    ClearBeams();
    if (m_iHudState)
    {
        m_iHudState = 0;
        MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
        WRITE_BYTE(m_iHudState);
        MESSAGE_END();
    }
    m_flZapTime = 0;

    UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr);

    if (tr.flFraction >= 1.0f)
    {
        UTIL_TraceHull(vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT(m_pPlayer->pev), &tr);
        if (tr.flFraction < 1.0f)
        {
            CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
            if (!pHit || pHit->IsBSPModel())
                FindHullIntersection(vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, ENT(m_pPlayer->pev));
            vecEnd = tr.vecEndPos;
        }
    }

    if (tr.flFraction >= 1.0f)
    {
        /* errou */
        m_flNextPrimaryAttack = gpGlobals->time + 0.6f;

        switch (m_iSwing)
        {
        case 0:
            SendWeaponAnim(ISLAVE_ATTACK1MISS);
            break;
        case 1:
            SendWeaponAnim(ISLAVE_ATTACK2MISS);
            break;
        case 2:
            SendWeaponAnim(ISLAVE_ATTACK3MISS);
            m_flNextPrimaryAttack = gpGlobals->time + 0.75f;
            break;
        default:
            SendWeaponAnim(ISLAVE_ATTACK1MISS);
            m_iSwing = 0;
            break;
        }

        switch (RANDOM_LONG(0, 1))
        {
        case 0:
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "zombie/claw_miss1.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 15));
            break;
        case 1:
            EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "zombie/claw_miss2.wav", 1, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 15));
            break;
        }
    }
    else
    {
        m_flNextPrimaryAttack = gpGlobals->time + 0.4f;

        CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
        if (!pEntity)
            return;

        switch (m_iSwing)
        {
        case 0:
            SendWeaponAnim(ISLAVE_ATTACK1HIT);
            break;
        case 1:
            SendWeaponAnim(ISLAVE_ATTACK2HIT);
            break;
        case 2:
            SendWeaponAnim(ISLAVE_ATTACK3HIT);
            m_flNextPrimaryAttack = gpGlobals->time + 0.45f;
            break;
        default:
            SendWeaponAnim(ISLAVE_ATTACK1HIT);
            m_iSwing = 0;
            break;
        }

        ClearMultiDamage();
        pEntity->TraceAttack(m_pPlayer->pev, gSkillData.plrDmgISlave, gpGlobals->v_forward, &tr, DMG_CLUB);
        ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

        if (!pEntity->IsPlayer() && pEntity->pev->takedamage && !m_pPlayer->pev->deadflag)
            m_pPlayer->TakeHealth((100 - m_pPlayer->pev->health) * 0.1f, DMG_GENERIC);

        float flVol = 1.0f;
        int fHitWorld = TRUE;

        if (pEntity)
        {
            if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
            {
                switch (RANDOM_LONG(0, 2))
                {
                case 0:
                    EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike1.wav", flVol, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                    break;
                case 1:
                    EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike2.wav", flVol, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                    break;
                case 2:
                    EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike3.wav", flVol, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                    break;
                }
                fHitWorld = FALSE;
            }
        }

        if (fHitWorld)
        {
            switch (RANDOM_LONG(0, 2))
            {
            case 0:
                EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike1.wav", 1, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                break;
            case 1:
                EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike2.wav", 1, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                break;
            case 2:
                EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "zombie/claw_strike3.wav", 1, ATTN_NORM, 0, 98 + RANDOM_LONG(0, 3));
                break;
            }
        }

        m_trHit = tr;
        SetThink(&CWeaponISlave::Smack);
        pev->nextthink = gpGlobals->time + 0.2f;

        int iVolume = flVol * 512;
        m_pPlayer->m_iWeaponVolume = iVolume;
    }

    m_pPlayer->SetAnimation(PLAYER_ATTACK1);

    m_iSwing++;
    if (m_iSwing >= 3)
        m_iSwing = 0;

    m_flNextPrimaryAttack = gpGlobals->time + 0.3f;
}
#else
/* a constante do ponteiro para Smack vai para o .rodata quando o PrimaryAttack
   é compilado; esta cópia em seção descartada a coloca no mesmo lugar */
void ISlave_PrimaryAttackRodata(CWeaponISlave *p) __attribute__((section(".islave_discard")));
void ISlave_PrimaryAttackRodata(CWeaponISlave *p)
{
    p->m_pfnThink = static_cast<BASEPTR>(&CWeaponISlave::Smack);
}
INCLUDE_ASM("asm/nonmatchings/dlls/weapon_islave", PrimaryAttack__13CWeaponISlave);
#endif

void CWeaponISlave::SecondaryAttack(void)
{
    if (m_flZapTime == 0)
    {
        StartZap();
        return;
    }

    if (m_flZapTime != 0 && m_flZapTime <= gpGlobals->time)
    {
        if (m_iHudState != 1)
        {
            CancelZap();
            return;
        }

        m_iHudState = 3;
        MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
        WRITE_BYTE(m_iHudState);
        MESSAGE_END();

        m_flZapTime = gpGlobals->time + 0.5f;
        SendWeaponAnim(ISLAVE_CHARGE_LOOP);
    }

    m_pPlayer->pev->velocity.x = 0;
    m_pPlayer->pev->velocity.y = 0;

    if (m_iHudState == 2 && m_flZapTime - gpGlobals->time <= 0.65f)
    {
        m_iHudState = 1;
        MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
        WRITE_BYTE(m_iHudState);
        MESSAGE_END();
    }

    if (m_flUpdateBeamTime < gpGlobals->time)
    {
        ArmBeam(1);
        ArmBeam(-1);
        BeamGlow();

        EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "debris/zap4.wav", 1, ATTN_NORM, 0, 100 + m_iBeams * 10);
        func_002071C8(m_pPlayer, 1, RANDOM_LONG(125, 175), RANDOM_LONG(25, 35));
        m_flUpdateBeamTime = gpGlobals->time + 0.15f;
    }

    m_pPlayer->SetAnimation(PLAYER_PS2_ATTACK2);
}

void CWeaponISlave::WeaponIdle(void)
{
    m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

    if (m_flZapTime != 0)
    {
        ZapAttack();
        m_iHudState = 0;
        MESSAGE_BEGIN(MSG_ONE, gmsgSlaveHud, NULL, ENT(m_pPlayer->pev));
        WRITE_BYTE(m_iHudState);
        MESSAGE_END();
        return;
    }

    if (m_flTimeWeaponIdle > gpGlobals->time)
        return;

    switch (RANDOM_LONG(0, 3))
    {
    case 0:
        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "aslave/slv_word5.wav", RANDOM_FLOAT(0.3f, ATTN_NORM), ATTN_NORM);
        break;
    case 1:
        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "aslave/slv_word7.wav", RANDOM_FLOAT(0.3f, ATTN_NORM), ATTN_NORM);
        break;
    case 2:
        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "aslave/slv_word3.wav", RANDOM_FLOAT(0.3f, ATTN_NORM), ATTN_NORM);
        break;
    case 3:
        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "aslave/slv_word4.wav", RANDOM_FLOAT(0.3f, ATTN_NORM), ATTN_NORM);
        break;
    }

    if (RANDOM_LONG(0, 1) == 0)
    {
        SendWeaponAnim(ISLAVE_IDLE1);
        m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(8, 15);
    }
    else
    {
        SendWeaponAnim(ISLAVE_IDLE2);
        m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10, 20);
    }
}

void CWeaponISlave::Smack(void)
{
    CBaseEntity *pEntity = CBaseEntity::Instance(m_trHit.pHit);

    if (pEntity)
    {
        if (pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE)
            func_002071C8(m_pPlayer, 1, 120, 50);
        else
            func_002071C8(m_pPlayer, 1, 170, 25);
    }
    else if (m_trHit.flFraction != 1.0f)
    {
        func_002071C8(m_pPlayer, 1, 120, 50);
    }
}

class CTriggerPlayerISlave : public CBaseEntity
{
public:
    void Spawn(void);
    void EXPORT PlayerISlaveUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void EXPORT PlayerISlaveThink(void);
};

void CTriggerPlayerISlave::Spawn(void)
{
    pev->movetype = MOVETYPE_NONE;
    pev->solid = SOLID_NOT;
    pev->takedamage = DAMAGE_NO;
    pev->effects = EF_NODRAW;
    SetUse(&CTriggerPlayerISlave::PlayerISlaveUse);
}

void CTriggerPlayerISlave::PlayerISlaveUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    SetThink(&CTriggerPlayerISlave::PlayerISlaveThink);
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CTriggerPlayerISlave::PlayerISlaveThink(void)
{
    CBaseEntity *pPlayer = CBaseEntity::Instance(INDEXENT(1));

    if (pPlayer && pPlayer->IsPlayer())
    {
        if (((CBasePlayer *)pPlayer)->m_afPhysicsFlags & PFLAG_PS2_NOPICKUP)
            SUB_UseTargets(this, USE_ON, 0);
        return;
    }

    pev->nextthink = gpGlobals->time + 0.3f;
}

LINK_ENTITY_TO_CLASS(trigger_player_islave, CTriggerPlayerISlave);

extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0020F1D0(void)
{
    func_002397D0("333", static_cast<BASEPTR>(&CWeaponISlave::Smack));
    func_002397D0("334", static_cast<BASEPTR>(&CWeaponISlave::ZapDone));
    func_002397D0("450", static_cast<BASEPTR>(&CTriggerPlayerISlave::PlayerISlaveThink));
    func_00239810("451", static_cast<USEPTR>(&CTriggerPlayerISlave::PlayerISlaveUse));
}
