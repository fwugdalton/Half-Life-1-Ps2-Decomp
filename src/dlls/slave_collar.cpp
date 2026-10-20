/* dlls/slave_collar.cpp: item_slave_collar do Half-Life: Decay. Não existe
   no HLSDK: é a carga elétrica do islave.cpp (ArmBeam, BeamGlow, WackBeam,
   ZapBeam, ClearBeams) presa num modelo parado, ligada e desligada pelo
   Use. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/skill.h"
#include "dlls/saverestore.h"
#include "dlls/effects.h"
#include "include_asm.h"

#define COLLAR_MAX_BEAMS    12

/* sem NON_MATCHING o StartCharge é asm: "debris/zap4.wav" vira um símbolo
   (D_003747D8) definido junto dele, para a string sair na mesma posição do .rodata */
#ifdef NON_MATCHING
#define ZAP4_WAV "debris/zap4.wav"
#else
extern "C" const char D_003747D8[];
#define ZAP4_WAV D_003747D8
#endif

extern "C" int cl_numlocal;

void UTIL_TraceLineNoEnts(const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr);

class CISlaveCollar : public CBaseToggle
{
public:
    void Spawn(void);
    void Precache(void);
    void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

    void StartCharge(void);
    void EXPORT ZapThink(void);
    void EXPORT ZapAgainThink(void);
    void EXPORT RechargeThink(void);

    void ArmBeam(int side);
    void BeamGlow(void);
    void WackBeam(int side, CBaseEntity *pEntity);
    void ZapBeam(int side);
    void ClearBeams(void);
    void ClearArmBeams(void);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    CBeam *m_pBeam[COLLAR_MAX_BEAMS];   /* 0xD0 */
    int m_iBeams;                       /* 0x100 */
    float m_flNextUse;                  /* 0x104 */
    BOOL m_fCollarOn;                   /* 0x108 */
};

LINK_ENTITY_TO_CLASS(item_slave_collar, CISlaveCollar);

TYPEDESCRIPTION CISlaveCollar::m_SaveData[] =
{
    DEFINE_ARRAY(CISlaveCollar, m_pBeam, FIELD_CLASSPTR, COLLAR_MAX_BEAMS),
    DEFINE_FIELD(CISlaveCollar, m_iBeams, FIELD_INTEGER),
    DEFINE_FIELD(CISlaveCollar, m_flNextUse, FIELD_TIME),
    DEFINE_FIELD(CISlaveCollar, m_fCollarOn, FIELD_BOOLEAN),
};

IMPLEMENT_SAVERESTORE(CISlaveCollar, CBaseToggle);

void CISlaveCollar::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
    if (m_flNextUse > gpGlobals->time)
        return;

    if ((pev->spawnflags & 1) && m_fCollarOn)
    {
        ClearBeams();
        m_fCollarOn = FALSE;
        SetThink(NULL);
        pev->nextthink = gpGlobals->time;
    }
    else
    {
        StartCharge();
        SetThink(&CISlaveCollar::ZapThink);
        pev->nextthink = gpGlobals->time + 2.0f;
        m_fCollarOn = TRUE;
    }
}

#ifdef NON_MATCHING
/* só o agendamento: o alvo carrega o pfnWriteByte depois da divisão 20 / framerate */
void CISlaveCollar::StartCharge(void)
{
    UTIL_MakeAimVectors(pev->angles);

    if (cl_numlocal == 1 && m_iBeams == 0)
    {
        Vector vecSrc = pev->origin + gpGlobals->v_forward * 2;
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecSrc);
        WRITE_BYTE(TE_DLIGHT);
        WRITE_COORD(vecSrc.x);
        WRITE_COORD(vecSrc.y);
        WRITE_COORD(vecSrc.z);
        WRITE_BYTE(12);
        WRITE_BYTE(255);
        WRITE_BYTE(180);
        WRITE_BYTE(96);
        int iLife = 20 / pev->framerate;
        WRITE_BYTE(iLife);
        WRITE_BYTE(0);
        MESSAGE_END();
    }

    ArmBeam(1);
    ArmBeam(2);
    ArmBeam(3);
    BeamGlow();

    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, ZAP4_WAV, 1, ATTN_NORM, 0, 100 + m_iBeams * 10);
}
#else
__asm__(".section .rodata\n.align 3\n.globl D_003747D8\nD_003747D8:\n.asciz \"debris/zap4.wav\"\n.previous\n");
INCLUDE_ASM("asm/nonmatchings/dlls/slave_collar", StartCharge__13CISlaveCollar);
#endif

void CISlaveCollar::ZapThink(void)
{
    ClearBeams();
    UTIL_MakeAimVectors(pev->angles);

    ZapBeam(1);
    ZapBeam(2);
    ZapBeam(3);

    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "hassault/hw_shoot1.wav", 1, ATTN_NORM, 0, RANDOM_LONG(130, 160));

    SetThink(&CISlaveCollar::RechargeThink);
    pev->nextthink = gpGlobals->time + 2.0f;
}

void CISlaveCollar::ZapAgainThink(void)
{
    ClearArmBeams();

    EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "hassault/hw_shoot1.wav", 1, ATTN_NORM, 0, RANDOM_LONG(130, 160));

    SetThink(&CISlaveCollar::RechargeThink);
    pev->nextthink = gpGlobals->time + 2.0f;
}

void CISlaveCollar::RechargeThink(void)
{
    if (pev->spawnflags & 1)
    {
        StartCharge();
        SetThink(&CISlaveCollar::ZapAgainThink);
        pev->nextthink = gpGlobals->time + 2.0f;
    }
    else
    {
        ClearBeams();
        m_fCollarOn = FALSE;
    }
}

void CISlaveCollar::Spawn(void)
{
    Precache();

    SET_MODEL(ENT(pev), "models/collar_test.dol");
    UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 72));

    pev->solid = SOLID_BBOX;
    pev->movetype = MOVETYPE_NONE;
    pev->effects = 0;

    DROP_TO_FLOOR(ENT(pev));

    m_fCollarOn = FALSE;
}

void CISlaveCollar::Precache(void)
{
    PRECACHE_MODEL("models/collar_test.dol");
    PRECACHE_MODEL("sprites/lgtning.spz");
    PRECACHE_SOUND("debris/zap1.wav");
    PRECACHE_SOUND((char *)ZAP4_WAV);
    PRECACHE_SOUND("weapons/electro4.wav");
    PRECACHE_SOUND("hassault/hw_shoot1.wav");

    UTIL_PrecacheOther("test_effect");
}

void CISlaveCollar::ArmBeam(int side)
{
    TraceResult tr;
    Vector vecSrc, vecAngles;
    float flDist = 1.0f;

    if (m_iBeams >= COLLAR_MAX_BEAMS)
        return;

    UTIL_MakeAimVectors(pev->angles);
    vecSrc = pev->origin + gpGlobals->v_up * 36 + gpGlobals->v_right * side * 16 + gpGlobals->v_forward * 32;

    for (int i = 0; i < 3; i++)
    {
        Vector vecAim = gpGlobals->v_right * side * RANDOM_FLOAT(0, 1) + gpGlobals->v_up * RANDOM_FLOAT(-1, 1);
        TraceResult tr1;
        UTIL_TraceLineNoEnts(vecSrc, vecSrc + vecAim * 512, dont_ignore_monsters, ENT(pev), &tr1);
        if (flDist > tr1.flFraction)
        {
            tr = tr1;
            flDist = tr.flFraction;
        }
    }

    if (flDist == 1.0f)
        return;

    DecalGunshot(&tr, BULLET_PLAYER_CROWBAR);

    m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spz", 30);
    if (!m_pBeam[m_iBeams])
        return;

    GetAttachment(side - 1, vecSrc, vecAngles);
    m_pBeam[m_iBeams]->PointsInit(tr.vecEndPos, vecSrc);
    m_pBeam[m_iBeams]->SetColor(96, 128, 16);
    m_pBeam[m_iBeams]->SetBrightness(64);
    m_pBeam[m_iBeams]->SetNoise(80);
    m_iBeams++;
}

void CISlaveCollar::BeamGlow(void)
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

void CISlaveCollar::WackBeam(int side, CBaseEntity *pEntity)
{
    if (m_iBeams >= COLLAR_MAX_BEAMS)
        return;

    if (pEntity == NULL)
        return;

    m_pBeam[m_iBeams] = CBeam::BeamCreate("sprites/lgtning.spz", 30);
    if (!m_pBeam[m_iBeams])
        return;

    Vector vecSrc, vecAngles;
    GetAttachment(side - 1, vecSrc, vecAngles);
    m_pBeam[m_iBeams]->PointsInit(pEntity->Center(), vecSrc);
    m_pBeam[m_iBeams]->SetColor(180, 255, 96);
    m_pBeam[m_iBeams]->SetBrightness(255);
    m_pBeam[m_iBeams]->SetNoise(80);
    m_iBeams++;
}

void CISlaveCollar::ZapBeam(int side)
{
    Vector vecSrc, vecAim;
    TraceResult tr;
    CBaseEntity *pEntity;

    if (m_iBeams >= COLLAR_MAX_BEAMS)
        return;

    float deflection = 0.01f;

    UTIL_MakeVectors(pev->angles);
    vecSrc = pev->origin + gpGlobals->v_up * 36;
    vecAim = gpGlobals->v_forward;
    vecAim = vecAim + gpGlobals->v_right * side * RANDOM_FLOAT(0, deflection) + gpGlobals->v_up * RANDOM_FLOAT(-deflection, deflection);
    UTIL_TraceLine(vecSrc, vecSrc + vecAim * 1024, dont_ignore_monsters, ENT(pev), &tr);

    pEntity = CBaseEntity::Instance(tr.pHit);
    if (pEntity != NULL && pEntity->pev->takedamage)
    {
        ClearMultiDamage();
        pEntity->TraceAttack(pev, gSkillData.slaveDmgZap, vecAim, &tr, DMG_SHOCK);
        pEntity->TakeDamage(pev, pev, gSkillData.slaveDmgZap, DMG_SHOCK);
    }

    UTIL_TraceLine(vecSrc, vecSrc + vecAim * 1024, ignore_monsters, ENT(pev), &tr);

    m_pBeam[COLLAR_MAX_BEAMS - side] = CBeam::BeamCreate("sprites/lgtning.spz", 50);
    if (!m_pBeam[COLLAR_MAX_BEAMS - side])
        return;

    GetAttachment(side - 1, vecSrc, vecAim);
    m_pBeam[COLLAR_MAX_BEAMS - side]->PointsInit(tr.vecEndPos, vecSrc);
    m_pBeam[COLLAR_MAX_BEAMS - side]->SetColor(180, 255, 96);
    m_pBeam[COLLAR_MAX_BEAMS - side]->SetBrightness(255);
    m_pBeam[COLLAR_MAX_BEAMS - side]->SetNoise(20);

    UTIL_EmitAmbientSound(ENT(pev), tr.vecEndPos, "weapons/electro4.wav", 0.5f, ATTN_NORM, 0, RANDOM_LONG(140, 160));
}

void CISlaveCollar::ClearBeams(void)
{
    for (int i = 0; i < COLLAR_MAX_BEAMS; i++)
    {
        if (m_pBeam[i])
        {
            UTIL_Remove(m_pBeam[i]);
            m_pBeam[i] = NULL;
        }
    }
    m_iBeams = 0;

    STOP_SOUND(ENT(pev), CHAN_WEAPON, ZAP4_WAV);
}

void CISlaveCollar::ClearArmBeams(void)
{
    for (int i = 0; i < COLLAR_MAX_BEAMS - 3; i++)
    {
        if (m_pBeam[i])
        {
            UTIL_Remove(m_pBeam[i]);
            m_pBeam[i] = NULL;
        }
    }
    m_iBeams = 0;

    STOP_SOUND(ENT(pev), CHAN_WEAPON, ZAP4_WAV);
}

extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_001E6A80(void)
{
    func_002397D0("259", static_cast<BASEPTR>(&CISlaveCollar::ZapThink));
    func_002397D0("260", static_cast<BASEPTR>(&CISlaveCollar::ZapAgainThink));
    func_002397D0("261", static_cast<BASEPTR>(&CISlaveCollar::RechargeThink));
}
