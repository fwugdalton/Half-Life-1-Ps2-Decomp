#ifndef DLLS_CBASE_H
#define DLLS_CBASE_H

/* cbase.h do HLSDK (versão antiga, sem os campos de munição do cliente).
   O gcc 2.95 põe o vptr depois dos campos da primeira classe com virtuais:
   CBaseEntity tem pev..m_pfnBlocked (0x00..0x2C) e o vptr em 0x2C.
   Entradas da vtable: 8 bytes {short delta; short index; void *pfn}, a 1ª é vazia. */

#define FCAP_CUSTOMSAVE         0x00000001
#define FCAP_ACROSS_TRANSITION  0x00000002
#define FCAP_MUST_SPAWN         0x00000004
#define FCAP_DONT_SAVE          0x80000000
#define FCAP_IMPULSE_USE        0x00000008
#define FCAP_CONTINUOUS_USE     0x00000010
#define FCAP_ONOFF_USE          0x00000020
#define FCAP_DIRECTIONAL_USE    0x00000040
#define FCAP_MASTER             0x00000080
#define FCAP_FORCE_TRANSITION   0x00000080

#include "dlls/saverestore.h"
#include "dlls/schedule.h"
#include "dlls/monsterevent.h"

#define EXPORT

typedef void (*ENTITYINIT)(entvars_t *);

extern "C" void func_0021D8A8(const char *key, int value);   /* registra uma entidade no hash (Gbx) */

/* LINK_ENTITY_TO_CLASS do PS2: sem GetProcAddress, cada arquivo registra suas
   entidades num objeto estático */
class CEntityLink
{
public:
    CEntityLink(const char *name, ENTITYINIT pfn) { func_0021D8A8(name, (int)pfn); }
};

#define LINK_ENTITY_TO_CLASS(mapClassName, DLLClassName) \
    extern "C" void mapClassName(entvars_t *pev); \
    void mapClassName(entvars_t *pev) { GetClassPtr((DLLClassName *)pev); } \
    static CEntityLink mapClassName##_link(#mapClassName, mapClassName);

class CBaseEntity;
class CBaseMonster;
class CBasePlayerItem;
class CSquadMonster;

#define SF_NORESPAWN (1 << 30)

class EHANDLE
{
private:
    edict_t *m_pent;
    int m_serialnumber;
public:
    edict_t *Get(void);
    edict_t *Set(edict_t *pent);

    operator int();

    operator CBaseEntity *();

    CBaseEntity *operator=(CBaseEntity *pEntity);
    CBaseEntity *operator->();
};

typedef enum { USE_OFF = 0, USE_ON = 1, USE_SET = 2, USE_TOGGLE = 3 } USE_TYPE;

typedef void (CBaseEntity::*BASEPTR)(void);
typedef void (CBaseEntity::*ENTITYFUNCPTR)(CBaseEntity *pOther);
typedef void (CBaseEntity::*USEPTR)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

class CBaseEntity
{
public:
    entvars_t *pev;                     /* 0x00 */
    CBaseEntity *m_pGoalEnt;            /* 0x04 */
    CBaseEntity *m_pLink;               /* 0x08 */

    virtual void Spawn(void) { return; }
    virtual void Precache(void) { return; }
    virtual void KeyValue(KeyValueData *pkvd) { pkvd->fHandled = FALSE; }
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    virtual int ObjectCaps(void) { return FCAP_ACROSS_TRANSITION; }
    virtual void Activate(void) { }

    virtual void SetObjectCollisionBox(void);

    virtual int Classify(void) { return CLASS_NONE; }
    virtual void DeathNotice(entvars_t *pevChild) { }

    static TYPEDESCRIPTION m_SaveData[];

    virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
    virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    virtual int TakeHealth(float flHealth, int bitsDamageType);
    virtual void Killed(entvars_t *pevAttacker, int iGib);
    virtual int BloodColor(void) { return DONT_BLEED; }
    virtual void TraceBleed(float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
    virtual BOOL IsTriggered(CBaseEntity *pActivator) { return TRUE; }
    virtual CBaseMonster *MyMonsterPointer(void) { return NULL; }
    virtual CSquadMonster *MySquadMonsterPointer(void) { return NULL; }
    virtual int GetToggleState(void) { return TS_AT_TOP; }
    virtual void AddPoints(int score, BOOL bAllowNegativeScore) { }
    virtual void AddPointsToTeam(int score, BOOL bAllowNegativeScore) { }
    virtual BOOL AddPlayerItem(CBasePlayerItem *pItem) { return 0; }
    virtual BOOL RemovePlayerItem(CBasePlayerItem *pItem) { return 0; }
    virtual int GiveAmmo(int iAmount, char *szName, int iMax) { return -1; }
    virtual float GetDelay(void) { return 0; }
    virtual int IsMoving(void) { return pev->velocity != g_vecZero; }
    virtual void OverrideReset(void) { }
    virtual int DamageDecal(int bitsDamageType);
    virtual void SetToggleState(int state) { }
    virtual void StartSneaking(void) { }
    virtual void StopSneaking(void) { }
    virtual BOOL OnControls(entvars_t *pev) { return FALSE; }
    virtual BOOL IsSneaking(void) { return FALSE; }
    virtual BOOL IsAlive(void) { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
    virtual BOOL IsBSPModel(void) { return pev->solid == SOLID_BSP || pev->movetype == MOVETYPE_PUSHSTEP; }
    virtual BOOL ReflectGauss(void) { return (IsBSPModel() && !pev->takedamage); }
    virtual BOOL HasTarget(string_t targetname);
    virtual BOOL IsInWorld(void);
    virtual BOOL IsPlayer(void) { return FALSE; }
    virtual BOOL IsNetClient(void) { return FALSE; }
    virtual const char *TeamID(void) { return ""; }

    virtual CBaseEntity *GetNextTarget(void);

    /* Think/Touch/Use/Blocked chamam os ponteiros para membro */
    virtual void Think(void) { if (m_pfnThink) (this->*m_pfnThink)(); }
    virtual void Touch(CBaseEntity *pOther) { if (m_pfnTouch) (this->*m_pfnTouch)(pOther); }
    virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
    {
        if (m_pfnUse)
            (this->*m_pfnUse)(pActivator, pCaller, useType, value);
    }
    virtual void Blocked(CBaseEntity *pOther) { if (m_pfnBlocked) (this->*m_pfnBlocked)(pOther); }

    void *operator new(unsigned int stAllocateBlock, entvars_t *pev)
    {
        return (void *)ALLOC_PRIVATE(ENT(pev), stAllocateBlock);
    }

    void UpdateOnRemove(void);

    void EXPORT SUB_Remove(void);
    void EXPORT SUB_DoNothing(void);
    void EXPORT SUB_StartFadeOut(void);
    void SUB_StartFadeOutLater(void); /* PS2: igual, mas o fade começa em 5 s */
    void EXPORT SUB_FadeOut(void);
    void EXPORT SUB_FadeOutFast(void); /* PS2: SUB_FadeOut com passo 21 */
    void EXPORT SUB_CallUseToggle(void) { this->Use(this, this, USE_TOGGLE, 0); }
    int ShouldToggle(USE_TYPE useType, BOOL currentState);
    void FireBullets(unsigned int cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq = 4, int iDamage = 0, entvars_t *pevAttacker = NULL);

    virtual CBaseEntity *Respawn(void) { return NULL; }

    void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
    BOOL Intersects(CBaseEntity *pOther);
    void MakeDormant(void);
    int IsDormant(void);
    BOOL IsLockedByMaster(void) { return FALSE; }

    static CBaseEntity *Instance(edict_t *pent)
    {
        if (!pent)
            pent = ENT(0);
        CBaseEntity *pEnt = (CBaseEntity *)GET_PRIVATE(pent);
        return pEnt;
    }

    static CBaseEntity *Instance(entvars_t *pev) { return Instance(ENT(pev)); }
    static CBaseEntity *Instance(int eoffset) { return Instance(ENT(eoffset)); }

    CBaseMonster *GetMonsterPointer(entvars_t *pevMonster)
    {
        CBaseEntity *pEntity = Instance(pevMonster);
        if (pEntity)
            return pEntity->MyMonsterPointer();
        return NULL;
    }
    CBaseMonster *GetMonsterPointer(edict_t *pentMonster)
    {
        CBaseEntity *pEntity = Instance(pentMonster);
        if (pEntity)
            return pEntity->MyMonsterPointer();
        return NULL;
    }

    static CBaseEntity *Create(char *szName, const Vector &vecOrigin, const Vector &vecAngles, edict_t *pentOwner = NULL);

    virtual void UpdateOwner(void) { return; }
    virtual BOOL FBecomeProne(void) { return FALSE; }
    edict_t *edict() { return ENT(pev); }
    EOFFSET eoffset() { return OFFSET(pev); }
    int entindex() { return ENTINDEX(edict()); }

    virtual Vector Center() { return (pev->absmax + pev->absmin) * 0.5; }
    virtual Vector EyePosition() { return pev->origin + pev->view_ofs; }
    virtual Vector EarPosition() { return pev->origin + pev->view_ofs; }
    virtual Vector BodyTarget(const Vector &posSrc) { return Center(); }

    virtual int Illumination() { return GETENTITYILLUM(ENT(pev)); }

    virtual BOOL FVisible(CBaseEntity *pEntity);
    virtual BOOL FVisible(const Vector &vecOrigin);

    virtual void Ps2Unk58(void) { }     /* virtual extra do PS2 (vazia em todas as classes vistas) */

    BASEPTR m_pfnThink;                 /* 0x0C */
    ENTITYFUNCPTR m_pfnTouch;           /* 0x14 */
    USEPTR m_pfnUse;                    /* 0x1C */
    ENTITYFUNCPTR m_pfnBlocked;         /* 0x24 */
    /* vptr em 0x2C */
};

#define SetThink(a) m_pfnThink = static_cast<void (CBaseEntity::*)(void)>(a)
#define SetTouch(a) m_pfnTouch = static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a)
#define SetUse(a) m_pfnUse = static_cast<void (CBaseEntity::*)(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)>(a)
#define SetBlocked(a) m_pfnBlocked = static_cast<void (CBaseEntity::*)(CBaseEntity *)>(a)

class CPointEntity : public CBaseEntity
{
public:
    void Spawn(void);
    virtual int ObjectCaps(void) { return CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

typedef struct locksounds
{
    string_t sLockedSound;
    string_t sLockedSentence;
    string_t sUnlockedSound;
    string_t sUnlockedSentence;
    int iLockedSentence;
    int iUnlockedSentence;
    float flwaitSound;
    float flwaitSentence;
    BYTE bEOFLocked;
    BYTE bEOFUnlocked;
} locksound_t;

class CBaseDelay : public CBaseEntity
{
public:
    float m_flDelay;                    /* 0x30 */
    int m_iszKillTarget;                /* 0x34 */

    virtual void KeyValue(KeyValueData *pkvd);
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];
    void SUB_UseTargets(CBaseEntity *pActivator, USE_TYPE useType, float value);
    void EXPORT DelayThink(void);
};

class CBaseAnimating : public CBaseDelay
{
public:
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];

    virtual void HandleAnimEvent(MonsterEvent_t *pEvent) { return; }

    float StudioFrameAdvance(float flInterval = 0.0);
    int GetSequenceFlags(void);
    int LookupActivity(int activity);
    int LookupActivityHeaviest(int activity);
    int LookupSequence(const char *label);
    void ResetSequenceInfo();
    void DispatchAnimEvents(float flFutureInterval = 0.1);
    float SetBoneController(int iController, float flValue);
    void InitBoneControllers(void);
    float SetBlending(int iBlender, float flValue);
    void GetBonePosition(int iBone, Vector &origin, Vector &angles);
    void GetAutomovement(Vector &origin, Vector &angles, float flInterval = 0.1);
    int FindTransition(int iEndingSequence, int iGoalSequence, int *piDir);
    void GetAttachment(int iAttachment, Vector &origin, Vector &angles);
    void SetBodygroup(int iGroup, int iValue);
    int GetBodygroup(int iGroup);
    int ExtractBbox(int sequence, float *mins, float *maxs);
    void SetSequenceBox(void);

    float m_flFrameRate;                /* 0x38 */
    float m_flGroundSpeed;              /* 0x3C */
    float m_flLastEventCheck;           /* 0x40 */
    BOOL m_fSequenceFinished;           /* 0x44 */
    BOOL m_fSequenceLoops;              /* 0x48 */
};

class CBaseToggle : public CBaseAnimating
{
public:
    void KeyValue(KeyValueData *pkvd);

    TOGGLE_STATE m_toggle_state;        /* 0x4C */
    float m_flActivateFinished;         /* 0x50 */
    float m_flMoveDistance;             /* 0x54 */
    float m_flWait;                     /* 0x58 */
    float m_flLip;                      /* 0x5C */
    float m_flTWidth;                   /* 0x60 */
    float m_flTLength;                  /* 0x64 */

    Vector m_vecPosition1;              /* 0x68 */
    Vector m_vecPosition2;              /* 0x74 */
    Vector m_vecAngle1;                 /* 0x80 */
    Vector m_vecAngle2;                 /* 0x8C */

    int m_cTriggersLeft;                /* 0x98 */
    float m_flHeight;                   /* 0x9C */
    EHANDLE m_hActivator;               /* 0xA0 */
    void (CBaseToggle::*m_pfnCallWhenMoveDone)(void); /* 0xA8 */
    Vector m_vecFinalDest;              /* 0xB0 */
    Vector m_vecFinalAngle;             /* 0xBC */

    int m_bitsDamageInflict;            /* 0xC8 */

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];

    virtual int GetToggleState(void) { return m_toggle_state; }
    virtual float GetDelay(void) { return m_flWait; }

    void LinearMove(Vector vecDest, float flSpeed);
    void EXPORT LinearMoveDone(void);
    void AngularMove(Vector vecDestAngle, float flSpeed);
    void EXPORT AngularMoveDone(void);
    BOOL IsLockedByMaster(void);

    static float AxisValue(int flags, const Vector &angles);
    static void AxisDir(entvars_t *pev);
    static float AxisDelta(int flags, const Vector &angle1, const Vector &angle2);

    string_t m_sMaster;                 /* 0xCC */
};

#define SetMoveDone(a) m_pfnCallWhenMoveDone = static_cast<void (CBaseToggle::*)(void)>(a)

template <class T> T *GetClassPtr(T *a)
{
    entvars_t *pev = (entvars_t *)a;

    if (pev == NULL)
        pev = VARS(CREATE_ENTITY());

    a = (T *)GET_PRIVATE(ENT(pev));

    if (a == NULL)
    {
        a = new (pev) T;
        a->pev = pev;
    }
    return a;
}

#endif
