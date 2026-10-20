/* dlls/weapons.cpp: código das armas que todas usam (CBasePlayerItem,
   CBasePlayerWeapon, CBasePlayerAmmo e CWeaponBox). É o weapons.cpp do
   HLSDK antigo (sem previsão no cliente) com as mudanças do PS2: lista de
   pré-carga por fase no W_Precache, "player_index" para o multiplayer em
   tela dividida, baterias na weaponbox e munição infinita. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/decals.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/saverestore.h"
#include "dlls/gamerules.h"

#define NOT_USED 255

extern "C" int cl_numlocal;
extern "C" void func_00113D08(int iPlayer, float flTime, float flDelay);

DLL_GLOBAL const char *g_pModelNameLaser = "sprites/laserbeam.spz";

static int giAmmoIndex = 0;

extern Vector g_vecAttackDir;

int MaxAmmoCarry(int iszName)
{
    for (int i = 0; i < MAX_WEAPONS; i++)
    {
        if (CBasePlayerItem::ItemInfoArray[i].pszAmmo1 && !strcmp(STRING(iszName), CBasePlayerItem::ItemInfoArray[i].pszAmmo1))
            return CBasePlayerItem::ItemInfoArray[i].iMaxAmmo1;
        if (CBasePlayerItem::ItemInfoArray[i].pszAmmo2 && !strcmp(STRING(iszName), CBasePlayerItem::ItemInfoArray[i].pszAmmo2))
            return CBasePlayerItem::ItemInfoArray[i].iMaxAmmo2;
    }

    ALERT(at_console, "MaxAmmoCarry() doesn't recognize '%s'!\n", STRING(iszName));
    return -1;
}

void ClearMultiDamage(void)
{
    gMultiDamage.pEntity = NULL;
    gMultiDamage.amount = 0;
    gMultiDamage.type = 0;
}

void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker)
{
    if (!gMultiDamage.pEntity)
        return;

    gMultiDamage.pEntity->TakeDamage(pevInflictor, pevAttacker, gMultiDamage.amount, gMultiDamage.type);
}

void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType)
{
    if (!pEntity)
        return;

    gMultiDamage.type |= bitsDamageType;

    if (pEntity != gMultiDamage.pEntity)
    {
        ApplyMultiDamage(pevInflictor, pevInflictor);
        gMultiDamage.pEntity = pEntity;
        gMultiDamage.amount = 0;
    }

    gMultiDamage.amount += flDamage;
}

void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage)
{
    UTIL_BloodDrips(vecSpot, g_vecAttackDir, bloodColor, (int)flDamage);
}

int DamageDecal(CBaseEntity *pEntity, int bitsDamageType)
{
    if (!pEntity)
        return (DECAL_GUNSHOT1 + RANDOM_LONG(0, 4));

    return pEntity->DamageDecal(bitsDamageType);
}

void DecalGunshot(TraceResult *pTrace, int iBulletType)
{
    if (!UTIL_IsValidEntity(pTrace->pHit))
        return;

    if (VARS(pTrace->pHit)->solid == SOLID_BSP || VARS(pTrace->pHit)->movetype == MOVETYPE_PUSHSTEP)
    {
        CBaseEntity *pEntity = NULL;

        if (!FNullEnt(pTrace->pHit))
            pEntity = CBaseEntity::Instance(pTrace->pHit);

        switch (iBulletType)
        {
        case BULLET_PLAYER_9MM:
        case BULLET_MONSTER_9MM:
        case BULLET_PLAYER_MP5:
        case BULLET_MONSTER_MP5:
        case BULLET_PLAYER_BUCKSHOT:
        case BULLET_PLAYER_357:
        default:
            UTIL_GunshotDecalTrace(pTrace, DamageDecal(pEntity, DMG_BULLET));
            break;
        case BULLET_MONSTER_12MM:
            UTIL_GunshotDecalTrace(pTrace, DamageDecal(pEntity, DMG_BULLET));
            break;
        case BULLET_PLAYER_CROWBAR:
            UTIL_DecalTrace(pTrace, DamageDecal(pEntity, DMG_CLUB));
            break;
        }
    }
}

void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype)
{
    if (cl_numlocal == 1)
    {
        MESSAGE_BEGIN(MSG_PVS, SVC_TEMPENTITY, vecOrigin);
            WRITE_BYTE(TE_MODEL);
            WRITE_COORD(vecOrigin.x);
            WRITE_COORD(vecOrigin.y);
            WRITE_COORD(vecOrigin.z);
            WRITE_COORD(vecVelocity.x);
            WRITE_COORD(vecVelocity.y);
            WRITE_COORD(vecVelocity.z);
            WRITE_ANGLE(rotation);
            WRITE_SHORT(model);
            WRITE_BYTE(soundtype);
            WRITE_BYTE(25);
        MESSAGE_END();
    }
}

void AddAmmoNameToAmmoRegistry(const char *szAmmoname)
{
    for (int i = 0; i < MAX_AMMO_SLOTS; i++)
    {
        if (!CBasePlayerItem::AmmoInfoArray[i].pszName)
            continue;

        if (stricmp(CBasePlayerItem::AmmoInfoArray[i].pszName, szAmmoname) == 0)
            return;
    }

    giAmmoIndex++;
    if (giAmmoIndex >= MAX_AMMO_SLOTS)
        giAmmoIndex = 0;

    CBasePlayerItem::AmmoInfoArray[giAmmoIndex].pszName = szAmmoname;
    CBasePlayerItem::AmmoInfoArray[giAmmoIndex].iId = giAmmoIndex;
}

void UTIL_PrecacheOtherWeapon(const char *szClassname)
{
    edict_t *pent;

    pent = CREATE_NAMED_ENTITY(MAKE_STRING(szClassname));
    if (FNullEnt(pent))
    {
        ALERT(at_console, "NULL Ent in UTIL_PrecacheOtherWeapon\n");
        return;
    }

    CBaseEntity *pEntity = CBaseEntity::Instance(VARS(pent));

    if (pEntity)
    {
        ItemInfo II;
        pEntity->Precache();
        memset(&II, 0, sizeof II);
        if (((CBasePlayerItem *)pEntity)->GetItemInfo(&II))
        {
            CBasePlayerItem::ItemInfoArray[II.iId] = II;

            if (II.pszAmmo1 && *II.pszAmmo1)
                AddAmmoNameToAmmoRegistry(II.pszAmmo1);

            if (II.pszAmmo2 && *II.pszAmmo2)
                AddAmmoNameToAmmoRegistry(II.pszAmmo2);

            memset(&II, 0, sizeof II);
        }
    }

    REMOVE_ENTITY(pent);
}

/* PS2: a fase pode mandar a lista do que pré-carregar (nomes em pares) */
void W_Precache(string_t *pszList1, string_t *pszList2, int count)
{
    memset(CBasePlayerItem::ItemInfoArray, 0, sizeof(CBasePlayerItem::ItemInfoArray));
    memset(CBasePlayerItem::AmmoInfoArray, 0, sizeof(CBasePlayerItem::AmmoInfoArray));
    giAmmoIndex = 0;

    UTIL_PrecacheOther("item_battery");

    if (pszList1)
    {
        for (int i = 0; i < count && pszList1[i]; i++)
        {
            if (!strncmp(STRING(pszList1[i]), "weapon_", 7))
                UTIL_PrecacheOtherWeapon(STRING(pszList1[i]));
            else
                UTIL_PrecacheOther(STRING(pszList1[i]));

            if (pszList2[i])
            {
                if (!strncmp(STRING(pszList2[i]), "weapon_", 7))
                    UTIL_PrecacheOtherWeapon(STRING(pszList2[i]));
                else
                    UTIL_PrecacheOther(STRING(pszList2[i]));
            }
        }
    }
    else
    {
        if (!g_pGameRules->IsMultiplayer() && !g_pGameRules->Ps2Virt07())
            UTIL_PrecacheOther("item_suit");
        UTIL_PrecacheOther("item_longjump");

        UTIL_PrecacheOtherWeapon("weapon_shotgun");
        UTIL_PrecacheOther("ammo_buckshot");

        UTIL_PrecacheOtherWeapon("weapon_crowbar");

        UTIL_PrecacheOtherWeapon("weapon_9mmhandgun");
        UTIL_PrecacheOther("ammo_9mmclip");

        UTIL_PrecacheOtherWeapon("weapon_9mmAR");
        UTIL_PrecacheOther("ammo_9mmAR");
        UTIL_PrecacheOther("ammo_ARgrenades");

        UTIL_PrecacheOtherWeapon("weapon_islave");

        UTIL_PrecacheOtherWeapon("weapon_357");
        UTIL_PrecacheOther("ammo_357");

        UTIL_PrecacheOtherWeapon("weapon_gauss");
        UTIL_PrecacheOther("ammo_gaussclip");

        UTIL_PrecacheOtherWeapon("weapon_rpg");
        UTIL_PrecacheOther("ammo_rpgclip");

        UTIL_PrecacheOtherWeapon("weapon_crossbow");
        UTIL_PrecacheOther("ammo_crossbow");

        UTIL_PrecacheOtherWeapon("weapon_egon");

        UTIL_PrecacheOtherWeapon("weapon_tripmine");

        UTIL_PrecacheOtherWeapon("weapon_satchel");

        UTIL_PrecacheOtherWeapon("weapon_handgrenade");

        UTIL_PrecacheOtherWeapon("weapon_snark");

        UTIL_PrecacheOtherWeapon("weapon_hornetgun");
    }

    if (g_pGameRules->Ps2Virt08() || g_pGameRules->IsMultiplayer())
        UTIL_PrecacheOther("weaponbox");

    g_sModelIndexFireball = PRECACHE_MODEL("sprites/zerogxplode.spz");
    g_sModelIndexWExplosion = PRECACHE_MODEL("sprites/WXplo1.spz");
    g_sModelIndexSmoke = PRECACHE_MODEL("sprites/steam1.spz");
    g_sModelIndexBubbles = PRECACHE_MODEL("sprites/bubble.spz");
    g_sModelIndexBloodSpray = PRECACHE_MODEL("sprites/bloodspray.spz");
    g_sModelIndexBloodDrop = PRECACHE_MODEL("sprites/blood.spz");

    g_sModelIndexLaser = PRECACHE_MODEL((char *)g_pModelNameLaser);
    g_sModelIndexLaserDot = PRECACHE_MODEL("sprites/laserdot.spz");

    PRECACHE_MODEL("models/grenade.dol");
    PRECACHE_MODEL("sprites/explode1.spz");

    PRECACHE_SOUND("weapons/debris1.wav");
    PRECACHE_SOUND("weapons/debris2.wav");
    PRECACHE_SOUND("weapons/debris3.wav");

    PRECACHE_SOUND("weapons/grenade_hit1.wav");
    PRECACHE_SOUND("weapons/grenade_hit2.wav");
    PRECACHE_SOUND("weapons/grenade_hit3.wav");

    PRECACHE_SOUND("weapons/bullet_hit1.wav");
    PRECACHE_SOUND("weapons/bullet_hit2.wav");

    PRECACHE_SOUND("items/weapondrop1.wav");

    PRECACHE_MODEL("sprites/bracket.spz");
    UTIL_PrecacheOther("targeting_spot");
    UTIL_PrecacheOther("useable_graphic");
}

TYPEDESCRIPTION CBasePlayerItem::m_SaveData[] =
{
    DEFINE_FIELD(CBasePlayerItem, m_pPlayer, FIELD_CLASSPTR),
    DEFINE_FIELD(CBasePlayerItem, m_pNext, FIELD_CLASSPTR),
    DEFINE_FIELD(CBasePlayerItem, m_iId, FIELD_INTEGER),
    DEFINE_FIELD(CBasePlayerItem, m_nPlayerIdx, FIELD_INTEGER),
};
IMPLEMENT_SAVERESTORE(CBasePlayerItem, CBaseAnimating);

TYPEDESCRIPTION CBasePlayerWeapon::m_SaveData[] =
{
    DEFINE_FIELD(CBasePlayerWeapon, m_flNextPrimaryAttack, FIELD_TIME),
    DEFINE_FIELD(CBasePlayerWeapon, m_flNextSecondaryAttack, FIELD_TIME),
    DEFINE_FIELD(CBasePlayerWeapon, m_flTimeWeaponIdle, FIELD_TIME),
    DEFINE_FIELD(CBasePlayerWeapon, m_iPrimaryAmmoType, FIELD_INTEGER),
    DEFINE_FIELD(CBasePlayerWeapon, m_iSecondaryAmmoType, FIELD_INTEGER),
    DEFINE_FIELD(CBasePlayerWeapon, m_iClip, FIELD_INTEGER),
    DEFINE_FIELD(CBasePlayerWeapon, m_iDefaultAmmo, FIELD_INTEGER),
};
IMPLEMENT_SAVERESTORE(CBasePlayerWeapon, CBasePlayerItem);

void CBasePlayerItem::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "player_index"))
    {
        m_nPlayerIdx = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else
        CBaseAnimating::KeyValue(pkvd);
}

void CBasePlayerItem::SetObjectCollisionBox(void)
{
    pev->absmin = pev->origin + Vector(-24, -24, 0);
    pev->absmax = pev->origin + Vector(24, 24, 16);
}

void CBasePlayerItem::FallInit(void)
{
    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_BBOX;

    UTIL_SetOrigin(pev, pev->origin);
    UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

    SetTouch(&CBasePlayerItem::DefaultTouch);
    if (!(pev->spawnflags & 1))
        SetThink(&CBasePlayerItem::FallThink);
    else
    {
        SetThink(&CBasePlayerItem::Materialize);
        pev->movetype = MOVETYPE_NONE;
    }

    pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayerItem::FallThink(void)
{
    pev->nextthink = gpGlobals->time + 0.1f;

    if (pev->flags & FL_ONGROUND)
    {
        if (!FNullEnt(pev->owner))
        {
            int pitch = 95 + RANDOM_LONG(0, 29);
            EMIT_SOUND_DYN(ENT(pev), CHAN_VOICE, "items/weapondrop1.wav", 1, ATTN_NORM, 0, pitch);
        }

        pev->angles.x = 0;
        pev->angles.z = 0;

        Materialize();
    }
}

void CBasePlayerItem::Materialize(void)
{
    if (pev->effects & EF_NODRAW)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150);
        pev->effects &= ~EF_NODRAW;
        pev->effects |= EF_MUZZLEFLASH;
    }

    pev->solid = SOLID_TRIGGER;

    UTIL_SetOrigin(pev, pev->origin);
    SetTouch(&CBasePlayerItem::DefaultTouch);
    SetThink(NULL);
}

void CBasePlayerItem::AttemptToMaterialize(void)
{
    float time = g_pGameRules->FlWeaponTryRespawn(this);

    if (time == 0)
    {
        Materialize();
        return;
    }

    pev->nextthink = gpGlobals->time + time;
}

void CBasePlayerItem::CheckRespawn(void)
{
    switch (g_pGameRules->WeaponShouldRespawn(this))
    {
    case GR_WEAPON_RESPAWN_YES:
        Respawn();
        break;
    case GR_WEAPON_RESPAWN_NO:
        return;
        break;
    }
}

CBaseEntity *CBasePlayerItem::Respawn(void)
{
    CBaseEntity *pNewWeapon = CBaseEntity::Create((char *)STRING(pev->classname), g_pGameRules->VecWeaponRespawnSpot(this), pev->angles, pev->owner);

    if (pNewWeapon)
    {
        pNewWeapon->pev->effects |= EF_NODRAW;
        pNewWeapon->SetTouch(NULL);
        pNewWeapon->SetThink(&CBasePlayerItem::AttemptToMaterialize);

        if (!(pev->spawnflags & 1))
            DROP_TO_FLOOR(ENT(pev));

        pNewWeapon->pev->nextthink = g_pGameRules->FlWeaponRespawnTime(this);
    }
    else
    {
        ALERT(at_console, "Respawn failed to create %s!\n", STRING(pev->classname));
    }

    return pNewWeapon;
}

void CBasePlayerItem::DefaultTouch(CBaseEntity *pOther)
{
    if (!pOther->IsPlayer())
        return;

    CBasePlayer *pPlayer = (CBasePlayer *)pOther;

    if (!gEvilImpulse101 && (pPlayer->m_afPhysicsFlags & PFLAG_PS2_NOPICKUP))
        return;

    if (!g_pGameRules->CanHavePlayerItem(pPlayer, this))
    {
        if (gEvilImpulse101)
            UTIL_Remove(this);
        return;
    }

    if (g_pGameRules->IsMultiplayer() && m_nPlayerIdx && g_pGameRules->Ps2Virt64(m_nPlayerIdx) != pOther)
        return;

    if (pOther->AddPlayerItem(this))
    {
        AttachToPlayer(pPlayer);
        EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM);
    }

    SUB_UseTargets(pOther, USE_TOGGLE, 0);
}

void CBasePlayerWeapon::ItemPostFrame(void)
{
    if ((m_fInReload) && (m_pPlayer->m_flNextAttack <= gpGlobals->time))
    {
        int j = min(iMaxClip() - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

        if (!m_pPlayer->m_ps2InfiniteAmmo)
        {
            m_iClip += j;
            m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= j;
        }
        else
            m_iClip += iMaxClip() - m_iClip;

        m_fInReload = FALSE;
    }

    if ((m_pPlayer->pev->button & IN_ATTACK2) && (m_flNextSecondaryAttack <= gpGlobals->time))
    {
        if (pszAmmo2() && !m_pPlayer->m_rgAmmo[SecondaryAmmoIndex()])
        {
            m_fFireOnEmpty = TRUE;
        }

        SecondaryAttack();
        m_pPlayer->pev->button &= ~IN_ATTACK2;
    }
    else if ((m_pPlayer->pev->button & IN_ATTACK) && (m_flNextPrimaryAttack <= gpGlobals->time))
    {
        if ((m_iClip == 0 && pszAmmo1()) || (iMaxClip() == -1 && !m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()]))
        {
            m_fFireOnEmpty = TRUE;
        }

        PrimaryAttack();
    }
    else if (m_pPlayer->pev->button & IN_RELOAD && iMaxClip() != WEAPON_NOCLIP && !m_fInReload)
    {
        Reload();
    }
    else if (!(m_pPlayer->pev->button & (IN_ATTACK | IN_ATTACK2)))
    {
        m_fFireOnEmpty = FALSE;

        if (!IsUseable() && m_flNextPrimaryAttack < gpGlobals->time)
        {
            if (!(iFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY) && g_pGameRules->GetNextBestWeapon(m_pPlayer, this))
            {
                m_flNextPrimaryAttack = gpGlobals->time + 0.3f;
                return;
            }
        }
        else
        {
            if (m_iClip == 0 && !(iFlags() & ITEM_FLAG_NOAUTORELOAD) && m_flNextPrimaryAttack < gpGlobals->time)
            {
                Reload();
                return;
            }
        }

        WeaponIdle();
        return;
    }

    if (ShouldWeaponIdle())
    {
        WeaponIdle();
    }
}

void CBasePlayerItem::DestroyItem(void)
{
    if (m_pPlayer)
    {
        m_pPlayer->RemovePlayerItem(this);
    }

    Kill();
}

int CBasePlayerItem::AddToPlayer(CBasePlayer *pPlayer)
{
    m_pPlayer = pPlayer;

    return TRUE;
}

void CBasePlayerItem::Drop(void)
{
    SetTouch(NULL);
    SetThink(&CBaseEntity::SUB_Remove);
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayerItem::Kill(void)
{
    SetTouch(NULL);
    SetThink(&CBaseEntity::SUB_Remove);
    pev->nextthink = gpGlobals->time + 0.1f;
}

void CBasePlayerItem::Holster(void)
{
    m_pPlayer->pev->viewmodel = 0;
    m_pPlayer->pev->weaponmodel = 0;
}

void CBasePlayerItem::AttachToPlayer(CBasePlayer *pPlayer)
{
    pev->movetype = MOVETYPE_FOLLOW;
    pev->solid = SOLID_NOT;
    pev->aiment = pPlayer->edict();
    pev->effects = EF_NODRAW;
    pev->modelindex = 0;
    pev->model = iStringNull;
    pev->owner = pPlayer->edict();
    pev->nextthink = gpGlobals->time + 0.1f;
    SetTouch(NULL);
}

int CBasePlayerWeapon::AddDuplicate(CBasePlayerItem *pOriginal)
{
    if (m_iDefaultAmmo)
    {
        return ExtractAmmo((CBasePlayerWeapon *)pOriginal);
    }
    else
    {
        return ExtractClipAmmo((CBasePlayerWeapon *)pOriginal);
    }
}

int CBasePlayerWeapon::AddToPlayer(CBasePlayer *pPlayer)
{
    int bResult = CBasePlayerItem::AddToPlayer(pPlayer);

    pPlayer->pev->weapons |= (1 << m_iId);

    if (!m_iPrimaryAmmoType)
    {
        m_iPrimaryAmmoType = pPlayer->GetAmmoIndex(pszAmmo1());
        m_iSecondaryAmmoType = pPlayer->GetAmmoIndex(pszAmmo2());
    }

    if (bResult)
        return AddWeapon();
    return FALSE;
}

int CBasePlayerWeapon::UpdateClientData(CBasePlayer *pPlayer)
{
    int state = 0;

    if (pPlayer->m_pActiveItem == this)
    {
        if (cl_numlocal >= 2)
        {
            if (pPlayer->m_fOnTarget)
                state = WEAPON_IS_ONTARGET;
            else
                state = 1;
        }
        else
            state = pPlayer->m_fOnTarget;
    }

    if (!pPlayer->m_fWeapon ||
        pPlayer->m_pActiveItem != pPlayer->m_pClientActiveItem ||
        m_iClip != m_iClientClip ||
        state != m_iClientWeaponState ||
        pPlayer->m_iFOV != pPlayer->m_iClientFOV)
    {
        if (!(m_pPlayer->pev->flags & FL_FAKECLIENT))
        {
            MESSAGE_BEGIN(MSG_ONE, gmsgCurWeapon, NULL, ENT(pPlayer->pev));
                WRITE_BYTE(state);
                WRITE_BYTE(m_iId);
                WRITE_BYTE(m_iClip);
            MESSAGE_END();
        }

        m_iClientClip = m_iClip;
        m_iClientWeaponState = state;
        pPlayer->m_fWeapon = TRUE;
    }

    if (m_pNext)
        m_pNext->UpdateClientData(pPlayer);

    return 1;
}

void CBasePlayerWeapon::SendWeaponAnim(int iAnim)
{
    if (m_pPlayer->pev->flags & FL_FAKECLIENT)
        return;

    MESSAGE_BEGIN(MSG_ONE, SVC_WEAPONANIM, NULL, ENT(m_pPlayer->pev));
        WRITE_BYTE(iAnim);
        WRITE_BYTE(pev->body);
    MESSAGE_END();
}

BOOL CBasePlayerWeapon::AddPrimaryAmmo(int iCount, char *szName, int iMaxClip, int iMaxCarry)
{
    int iIdAmmo;

    if (iMaxClip < 1)
    {
        m_iClip = -1;
        iIdAmmo = m_pPlayer->GiveAmmo(iCount, szName, iMaxCarry);
    }
    else if (m_iClip == 0)
    {
        int i;
        i = min(m_iClip + iCount, iMaxClip) - m_iClip;
        m_iClip += i;
        iIdAmmo = m_pPlayer->GiveAmmo(iCount - i, szName, iMaxCarry);
    }
    else
    {
        iIdAmmo = m_pPlayer->GiveAmmo(iCount, szName, iMaxCarry);
    }

    if (iIdAmmo > 0)
    {
        m_iPrimaryAmmoType = iIdAmmo;
        if (m_pPlayer->HasPlayerItem(this))
        {
            EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
        }
    }

    return iIdAmmo > 0 ? TRUE : FALSE;
}

BOOL CBasePlayerWeapon::AddSecondaryAmmo(int iCount, char *szName, int iMax)
{
    int iIdAmmo;

    iIdAmmo = m_pPlayer->GiveAmmo(iCount, szName, iMax);

    if (iIdAmmo > 0)
    {
        m_iSecondaryAmmoType = iIdAmmo;
        EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
    }
    return iIdAmmo > 0 ? TRUE : FALSE;
}

BOOL CBasePlayerWeapon::IsUseable(void)
{
    if (m_iClip <= 0)
    {
        if (m_pPlayer->m_rgAmmo[PrimaryAmmoIndex()] <= 0 && iMaxAmmo1() != -1)
        {
            return FALSE;
        }
    }

    return TRUE;
}

BOOL CBasePlayerWeapon::CanDeploy(void)
{
    BOOL bHasAmmo = 0;

    if (!pszAmmo1())
    {
        return TRUE;
    }

    if (pszAmmo1())
    {
        bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] != 0);
    }
    if (pszAmmo2())
    {
        bHasAmmo |= (m_pPlayer->m_rgAmmo[m_iSecondaryAmmoType] != 0);
    }
    if (m_iClip > 0)
    {
        bHasAmmo |= 1;
    }
    if (!bHasAmmo)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CBasePlayerWeapon::DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt)
{
    if (!CanDeploy())
        return FALSE;

    m_pPlayer->pev->viewmodel = szViewModel ? MAKE_STRING(szViewModel) : 0;
    m_pPlayer->pev->weaponmodel = szWeaponModel ? MAKE_STRING(szWeaponModel) : 0;
    strcpy(m_pPlayer->m_szAnimExtention, szAnimExt);
    SendWeaponAnim(iAnim);

    m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;
    m_flTimeWeaponIdle = gpGlobals->time + 1.0f;
    return TRUE;
}

int CBasePlayerWeapon::DefaultReload(int iClipSize, int iAnim, float fDelay)
{
    if (!m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
        return FALSE;

    int j = min(iClipSize - m_iClip, m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]);

    if (j == 0)
        return FALSE;

    m_pPlayer->m_flNextAttack = gpGlobals->time + fDelay;

    SendWeaponAnim(iAnim);

    globalvars_t *pGlobals = gpGlobals;
    m_fInReload = TRUE;

    m_flTimeWeaponIdle = pGlobals->time + 3;
    func_00113D08(ENTINDEX(m_pPlayer->edict()) - 1, pGlobals->time, fDelay);
    return TRUE;
}

BOOL CBasePlayerWeapon::PlayEmptySound(void)
{
    if (m_iPlayEmptySound)
    {
        EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8f, ATTN_NORM);
        m_iPlayEmptySound = 0;
        return 0;
    }
    return 0;
}

void CBasePlayerWeapon::ResetEmptySound(void)
{
    m_iPlayEmptySound = 1;
}

int CBasePlayerWeapon::PrimaryAmmoIndex(void)
{
    return m_iPrimaryAmmoType;
}

int CBasePlayerWeapon::SecondaryAmmoIndex(void)
{
    return -1;
}

void CBasePlayerWeapon::Holster(void)
{
    m_fInReload = FALSE;
    m_pPlayer->pev->viewmodel = 0;
    m_pPlayer->pev->weaponmodel = 0;
}

void CBasePlayerAmmo::Spawn(void)
{
    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_TRIGGER;
    UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 16));
    UTIL_SetOrigin(pev, pev->origin);

    SetTouch(&CBasePlayerAmmo::DefaultTouch);
}

CBaseEntity *CBasePlayerAmmo::Respawn(void)
{
    pev->effects |= EF_NODRAW;
    SetTouch(NULL);

    UTIL_SetOrigin(pev, g_pGameRules->VecAmmoRespawnSpot(this));

    SetThink(&CBasePlayerAmmo::Materialize);
    pev->nextthink = g_pGameRules->FlAmmoRespawnTime(this);

    return this;
}

void CBasePlayerAmmo::Materialize(void)
{
    if (pev->effects & EF_NODRAW)
    {
        EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "items/suitchargeok1.wav", 1, ATTN_NORM, 0, 150);
        pev->effects &= ~EF_NODRAW;
        pev->effects |= EF_MUZZLEFLASH;
    }

    SetTouch(&CBasePlayerAmmo::DefaultTouch);
}

void CBasePlayerAmmo::DefaultTouch(CBaseEntity *pOther)
{
    if (!pOther->IsPlayer())
    {
        return;
    }

    if (!gEvilImpulse101 && (((CBasePlayer *)pOther)->m_afPhysicsFlags & PFLAG_PS2_NOPICKUP))
        return;

    if (AddAmmo(pOther))
    {
        if (g_pGameRules->AmmoShouldRespawn(this) == GR_AMMO_RESPAWN_YES)
        {
            Respawn();
        }
        else
        {
            SetTouch(NULL);
            SetThink(&CBaseEntity::SUB_Remove);
            pev->nextthink = gpGlobals->time + 0.1f;
        }
    }
    else if (gEvilImpulse101)
    {
        SetTouch(NULL);
        SetThink(&CBaseEntity::SUB_Remove);
        pev->nextthink = gpGlobals->time + 0.1f;
    }
}

int CBasePlayerWeapon::ExtractAmmo(CBasePlayerWeapon *pWeapon)
{
    int iReturn = 0;

    if (pszAmmo1() != NULL)
    {
        iReturn = pWeapon->AddPrimaryAmmo(m_iDefaultAmmo, (char *)pszAmmo1(), iMaxClip(), iMaxAmmo1());
        m_iDefaultAmmo = 0;
    }

    if (pszAmmo2() != NULL)
    {
        iReturn = pWeapon->AddSecondaryAmmo(0, (char *)pszAmmo2(), iMaxAmmo2());
    }

    return iReturn;
}

int CBasePlayerWeapon::ExtractClipAmmo(CBasePlayerWeapon *pWeapon)
{
    int iAmmo;

    if (m_iClip == WEAPON_NOCLIP)
    {
        iAmmo = 0;
    }
    else
    {
        iAmmo = m_iClip;
    }

    return pWeapon->m_pPlayer->GiveAmmo(iAmmo, (char *)pszAmmo1(), iMaxAmmo1());
}

void CBasePlayerWeapon::RetireWeapon(void)
{
    m_pPlayer->pev->viewmodel = iStringNull;
    m_pPlayer->pev->weaponmodel = iStringNull;

    g_pGameRules->GetNextBestWeapon(m_pPlayer, this);
}

LINK_ENTITY_TO_CLASS(weaponbox, CWeaponBox);

TYPEDESCRIPTION CWeaponBox::m_SaveData[] =
{
    DEFINE_ARRAY(CWeaponBox, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS),
    DEFINE_ARRAY(CWeaponBox, m_rgiszAmmo, FIELD_STRING, MAX_AMMO_SLOTS),
    DEFINE_ARRAY(CWeaponBox, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES),
    DEFINE_FIELD(CWeaponBox, m_cAmmoTypes, FIELD_INTEGER),
    DEFINE_FIELD(CWeaponBox, m_nPlayerIdx, FIELD_INTEGER),
    DEFINE_FIELD(CWeaponBox, m_iNumBatteries, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CWeaponBox, CBaseEntity);

void CWeaponBox::Precache(void)
{
    PRECACHE_MODEL("models/w_weaponbox.dol");
}

void CWeaponBox::KeyValue(KeyValueData *pkvd)
{
    if (FStrEq(pkvd->szKeyName, "player_index"))
    {
        m_nPlayerIdx = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (FStrEq(pkvd->szKeyName, "batteries"))
    {
        m_iNumBatteries = atoi(pkvd->szValue);
        pkvd->fHandled = TRUE;
    }
    else if (m_cAmmoTypes < MAX_AMMO_SLOTS)
    {
        PackAmmo(ALLOC_STRING(pkvd->szKeyName), atoi(pkvd->szValue));
        m_cAmmoTypes++;

        pkvd->fHandled = TRUE;
    }
    else
    {
        ALERT(at_console, "WeaponBox too full! only %d ammotypes allowed\n", MAX_AMMO_SLOTS);
    }
}

void CWeaponBox::Spawn(void)
{
    Precache();

    pev->movetype = MOVETYPE_TOSS;
    pev->solid = SOLID_TRIGGER;

    UTIL_SetSize(pev, g_vecZero, g_vecZero);

    SET_MODEL(ENT(pev), "models/w_weaponbox.dol");
}

void CWeaponBox::Kill(void)
{
    CBasePlayerItem *pWeapon;
    int i;

    for (i = 0; i < MAX_ITEM_TYPES; i++)
    {
        pWeapon = m_rgpPlayerItems[i];

        while (pWeapon)
        {
            pWeapon->SetThink(&CBaseEntity::SUB_Remove);
            pWeapon->pev->nextthink = gpGlobals->time + 0.1f;
            pWeapon = pWeapon->m_pNext;
        }
    }

    UTIL_Remove(this);
}

void CWeaponBox::Touch(CBaseEntity *pOther)
{
    if (!(pev->flags & FL_ONGROUND))
    {
        return;
    }

    if (!pOther->IsPlayer())
    {
        return;
    }

    if (!pOther->IsAlive())
    {
        return;
    }

    CBasePlayer *pPlayer = (CBasePlayer *)pOther;

    if (pPlayer->m_afPhysicsFlags & PFLAG_PS2_NOPICKUP)
        return;

    if (g_pGameRules->IsMultiplayer() && m_nPlayerIdx && g_pGameRules->Ps2Virt64(m_nPlayerIdx) != pOther)
        return;

    int i;

    for (i = 0; i < MAX_AMMO_SLOTS; i++)
    {
        if (!FStringNull(m_rgiszAmmo[i]))
        {
            pPlayer->GiveAmmo(m_rgAmmo[i], (char *)STRING(m_rgiszAmmo[i]), MaxAmmoCarry(m_rgiszAmmo[i]));

            m_rgiszAmmo[i] = iStringNull;
            m_rgAmmo[i] = 0;
        }
    }

    if (m_iNumBatteries > 0 && pPlayer)
    {
        for (i = 0; i < m_iNumBatteries; i++)
            pPlayer->GiveNamedItem("item_battery");
    }

    for (i = 0; i < MAX_ITEM_TYPES; i++)
    {
        if (m_rgpPlayerItems[i])
        {
            CBasePlayerItem *pItem;

            while (m_rgpPlayerItems[i])
            {
                pItem = m_rgpPlayerItems[i];
                m_rgpPlayerItems[i] = m_rgpPlayerItems[i]->m_pNext;

                if (pPlayer->AddPlayerItem(pItem))
                {
                    pItem->AttachToPlayer(pPlayer);
                }
            }
        }
    }

    EMIT_SOUND(pOther->edict(), CHAN_ITEM, "items/gunpickup2.wav", 1, ATTN_NORM);
    SetTouch(NULL);
    UTIL_Remove(this);
}

BOOL CWeaponBox::PackWeapon(CBasePlayerItem *pWeapon)
{
    if (HasWeapon(pWeapon))
    {
        return FALSE;
    }

    if (pWeapon->m_pPlayer)
    {
        if (!pWeapon->m_pPlayer->RemovePlayerItem(pWeapon))
        {
            return FALSE;
        }
    }

    int iWeaponSlot = pWeapon->iItemSlot();

    if (m_rgpPlayerItems[iWeaponSlot])
    {
        pWeapon->m_pNext = m_rgpPlayerItems[iWeaponSlot];
        m_rgpPlayerItems[iWeaponSlot] = pWeapon;
    }
    else
    {
        m_rgpPlayerItems[iWeaponSlot] = pWeapon;
        pWeapon->m_pNext = NULL;
    }

    pWeapon->pev->spawnflags |= SF_NORESPAWN;
    pWeapon->pev->movetype = MOVETYPE_NONE;
    pWeapon->pev->solid = SOLID_NOT;
    pWeapon->pev->effects |= EF_NODRAW;
    pWeapon->pev->modelindex = 0;
    pWeapon->pev->model = iStringNull;
    pWeapon->pev->owner = edict();
    pWeapon->SetThink(NULL);
    pWeapon->SetTouch(NULL);
    pWeapon->m_pPlayer = NULL;

    return TRUE;
}

BOOL CWeaponBox::PackAmmo(int iszName, int iCount)
{
    int iMaxCarry;

    if (FStringNull(iszName))
    {
        ALERT(at_console, "NULL String in PackAmmo!\n");
        return FALSE;
    }

    iMaxCarry = MaxAmmoCarry(iszName);

    if (iMaxCarry != -1 && iCount > 0)
    {
        GiveAmmo(iCount, (char *)STRING(iszName), iMaxCarry);
        return TRUE;
    }

    return FALSE;
}

int CWeaponBox::GiveAmmo(int iCount, char *szName, int iMax, int *pIndex)
{
    int i;

    for (i = 1; i < MAX_AMMO_SLOTS && !FStringNull(m_rgiszAmmo[i]); i++)
    {
        if (stricmp(szName, STRING(m_rgiszAmmo[i])) == 0)
        {
            if (pIndex)
                *pIndex = i;

            int iAdd = min(iCount, iMax - m_rgAmmo[i]);
            if (iCount == 0 || iAdd > 0)
            {
                m_rgAmmo[i] += iAdd;

                return i;
            }
            return -1;
        }
    }
    if (i < MAX_AMMO_SLOTS)
    {
        if (pIndex)
            *pIndex = i;

        m_rgiszAmmo[i] = MAKE_STRING(szName);
        m_rgAmmo[i] = iCount;

        return i;
    }
    ALERT(at_console, "out of named ammo slots\n");
    return i;
}

BOOL CWeaponBox::HasWeapon(CBasePlayerItem *pCheckItem)
{
    CBasePlayerItem *pItem = m_rgpPlayerItems[pCheckItem->iItemSlot()];

    while (pItem)
    {
        if (FClassnameIs(pItem->pev, STRING(pCheckItem->pev->classname)))
        {
            return TRUE;
        }
        pItem = pItem->m_pNext;
    }

    return FALSE;
}

BOOL CWeaponBox::IsEmpty(void)
{
    int i;

    for (i = 0; i < MAX_ITEM_TYPES; i++)
    {
        if (m_rgpPlayerItems[i])
        {
            return FALSE;
        }
    }

    for (i = 0; i < MAX_AMMO_SLOTS; i++)
    {
        if (!FStringNull(m_rgiszAmmo[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

void CWeaponBox::SetObjectCollisionBox(void)
{
    pev->absmin = pev->origin + Vector(-16, -16, 0);
    pev->absmax = pev->origin + Vector(16, 16, 16);
}

/* PS2: registra os ponteiros de método pelo id (MethodPointerTable) */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);

extern "C" void func_00212630(void)
{
    func_002397D0("337", static_cast<BASEPTR>(&CBasePlayerItem::FallThink));
    func_002397D0("338", static_cast<BASEPTR>(&CBasePlayerItem::Materialize));
    func_002397D0("339", static_cast<BASEPTR>(&CBaseEntity::SUB_Remove));
    func_002397D0("340", static_cast<BASEPTR>(&CBasePlayerItem::AttemptToMaterialize));
    func_002397D0("341", static_cast<BASEPTR>(&CBaseEntity::SUB_Remove));
    func_002397D0("342", static_cast<BASEPTR>(&CBasePlayerAmmo::Materialize));
    func_002397F0("343", static_cast<ENTITYFUNCPTR>(&CBasePlayerItem::DefaultTouch));
    func_002397F0("344", static_cast<ENTITYFUNCPTR>(&CBasePlayerAmmo::DefaultTouch));
    func_002397D0("351", static_cast<BASEPTR>(&CWeaponBox::Kill));
}

DLL_GLOBAL short g_sModelIndexLaser;
DLL_GLOBAL short g_sModelIndexLaserDot;
DLL_GLOBAL short g_sModelIndexFireball;
DLL_GLOBAL short g_sModelIndexSmoke;
DLL_GLOBAL short g_sModelIndexWExplosion;
DLL_GLOBAL short g_sModelIndexBubbles;
DLL_GLOBAL short g_sModelIndexBloodDrop;
DLL_GLOBAL short g_sModelIndexBloodSpray;

MULTIDAMAGE gMultiDamage;
