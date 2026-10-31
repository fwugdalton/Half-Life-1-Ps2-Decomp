/* dlls/egon.cpp: o canhão de partículas (weapon_egon) e a munição
   (ammo_egonclip) do HLSDK, na versão antiga do servidor (feixes CBeam e
   CSprite criados no servidor, sem eventos). O PS2 acrescenta o rumble do
   controle enquanto o feixe está ligado e a munição infinita do cheat no
   UseAmmo; o botão secundário só chama o WeaponIdle. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/effects.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define WEAPON_EGON             10
#define URANIUM_MAX_CARRY       100
#define EGON_DEFAULT_GIVE       20
#define EGON_WEIGHT             20
#define AMMO_URANIUMBOX_GIVE    20
#define WEAPON_NOCLIP           -1

#define	EGON_PRIMARY_VOLUME		450
#define EGON_BEAM_SPRITE		"sprites/xbeam1.spz"
#define EGON_FLARE_SPRITE		"sprites/XSpark1.spz"
#define EGON_SOUND_OFF			"weapons/egon_off1.wav"
#define EGON_SOUND_RUN			"weapons/egon_run3.wav"
#define EGON_SOUND_STARTUP		"weapons/egon_windup2.wav"

#define SF_SPRITE_TEMPORARY		0x8000

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);
extern "C" float sinf(float);
void UTIL_ScreenShake( const Vector &center, float amplitude, float frequency, float duration, float radius );
void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
extern "C" float fabsf(float);

enum egon_e {
	EGON_IDLE1 = 0,
	EGON_FIDGET1,
	EGON_ALTFIREON,
	EGON_ALTFIRECYCLE,
	EGON_ALTFIREOFF,
	EGON_FIRE1,
	EGON_FIRE2,
	EGON_FIRE3,
	EGON_FIRE4,
	EGON_DRAW,
	EGON_HOLSTER
};

class CEgon : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	BOOL Deploy( void );
	void Holster( void );

	void CreateEffect( void );
	void UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend );
	void DestroyEffect( void );

	void EndAttack( void );
	void Attack( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	float GetPulseInterval( void );
	float GetDischargeInterval( void );

	void Fire( const Vector &vecOrigSrc, const Vector &vecDir );

	BOOL HasAmmo( void )
	{
		if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
			return FALSE;
		return TRUE;
	}

	void UseAmmo( int count )
	{
		if ( !m_pPlayer->m_ps2InfiniteAmmo )
		{
			if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= count )
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= count;
			else
				m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] = 0;
		}
	}

	enum EGON_FIRESTATE { FIRE_OFF, FIRE_CHARGE };
	enum EGON_FIREMODE { FIRE_NARROW, FIRE_WIDE};

	float m_flAmmoUseTime;// since we use < 1 point of ammo per update, we subtract ammo on a timer.

private:
	float				m_shootTime;
	CBeam				*m_pBeam;
	CBeam				*m_pNoise;
	CSprite				*m_pSprite;
	EGON_FIRESTATE		m_fireState;
	EGON_FIREMODE		m_fireMode;
	float				m_shakeTime;
	BOOL				m_deployed;
};

int g_fireAnims1[] = { EGON_FIRE1, EGON_FIRE2, EGON_FIRE3, EGON_FIRE4 };
int g_fireAnims2[] = { EGON_ALTFIRECYCLE };

LINK_ENTITY_TO_CLASS( weapon_egon, CEgon );

TYPEDESCRIPTION	CEgon::m_SaveData[] = 
{
	DEFINE_FIELD( CEgon, m_pBeam, FIELD_CLASSPTR ),
	DEFINE_FIELD( CEgon, m_pNoise, FIELD_CLASSPTR ),
	DEFINE_FIELD( CEgon, m_pSprite, FIELD_CLASSPTR ),
	DEFINE_FIELD( CEgon, m_shootTime, FIELD_TIME ),
	DEFINE_FIELD( CEgon, m_fireState, FIELD_INTEGER ),
	DEFINE_FIELD( CEgon, m_fireMode, FIELD_INTEGER ),
	DEFINE_FIELD( CEgon, m_shakeTime, FIELD_TIME ),
	DEFINE_FIELD( CEgon, m_flAmmoUseTime, FIELD_TIME ),
};
IMPLEMENT_SAVERESTORE( CEgon, CBasePlayerWeapon );

void CEgon::Spawn( )
{
	Precache( );
	m_iId = WEAPON_EGON;
	SET_MODEL(ENT(pev), "models/w_egon.dol");

	m_iDefaultAmmo = EGON_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

void CEgon::Precache( void )
{
	PRECACHE_MODEL("models/w_egon.dol");
	PRECACHE_MODEL("models/v_egon.dol");
	PRECACHE_MODEL("models/p_egon.dol");

	PRECACHE_MODEL("models/w_9mmclip.dol");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND( EGON_SOUND_OFF );
	PRECACHE_SOUND( EGON_SOUND_RUN );
	PRECACHE_SOUND( EGON_SOUND_STARTUP );

	PRECACHE_MODEL( EGON_BEAM_SPRITE );
	PRECACHE_MODEL( EGON_FLARE_SPRITE );

	PRECACHE_SOUND ("weapons/357_cock1.wav");
}

BOOL CEgon::Deploy( void )
{
	m_deployed = FALSE;
	return DefaultDeploy( "models/v_egon.dol", "models/p_egon.dol", EGON_DRAW, "egon" );
}

int CEgon::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

void CEgon::Holster( void )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;
	// m_flTimeWeaponIdle = gpGlobals->time + UTIL_RandomFloat ( 10, 15 );
	SendWeaponAnim( EGON_HOLSTER );

	if ( m_fireState != FIRE_OFF )
		EndAttack();
}

int CEgon::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "uranium";
	p->iMaxAmmo1 = URANIUM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 2;
	p->iId = m_iId = WEAPON_EGON;
	p->iFlags = 0;
	p->iWeight = EGON_WEIGHT;

	return 1;
}

#define EGON_PULSE_INTERVAL			0.1f
#define EGON_DISCHARGE_INTERVAL		0.1f

float CEgon::GetPulseInterval( void )
{
	if ( g_pGameRules->Ps2Virt07() )
	{
		return 0.1f;
	}

	return EGON_PULSE_INTERVAL;
}

float CEgon::GetDischargeInterval( void )
{
	if ( g_pGameRules->Ps2Virt07() )
	{
		return 0.1f;
	}

	return EGON_DISCHARGE_INTERVAL;
}

void CEgon::Attack( void )
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		if ( m_pBeam )
		{
			EndAttack();
		}
		else
		{
			PlayEmptySound( );
		}
		return;
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );

	switch( m_fireState )
	{
		case FIRE_OFF:
		{
			if ( !HasAmmo() )
			{
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.25f;
				PlayEmptySound( );
				return;
			}

			m_flAmmoUseTime = gpGlobals->time;// start using ammo ASAP.

			SendWeaponAnim( g_fireAnims1[ RANDOM_LONG(0,ARRAYSIZE(g_fireAnims1)-1) ] );
			m_shakeTime = 0;

			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;
			m_flTimeWeaponIdle = gpGlobals->time + 0.1f;
			m_shootTime = gpGlobals->time + 2;

			if ( m_fireMode == FIRE_WIDE )
			{
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, EGON_SOUND_STARTUP, 0.98f, ATTN_NORM, 0, 125 );
			}
			else
			{
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, EGON_SOUND_STARTUP, 0.9f, ATTN_NORM, 0, 100 );
			}

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
			m_fireState = FIRE_CHARGE;
		}
		break;

		case FIRE_CHARGE:
		{
			Fire( vecSrc, vecAiming );
			m_pPlayer->m_iWeaponVolume = EGON_PRIMARY_VOLUME;

			func_002071C8( m_pPlayer, 1, RANDOM_LONG( 40, 175 ), RANDOM_LONG( 5, 15 ) );

			if ( m_shootTime != 0 && gpGlobals->time > m_shootTime )
			{
				if ( m_fireMode == FIRE_WIDE )
				{
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, EGON_SOUND_RUN, 0.98f, ATTN_NORM, 0, 125 );
				}
				else
				{
					EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_STATIC, EGON_SOUND_RUN, 0.9f, ATTN_NORM, 0, 100 );
				}

				m_shootTime = 0;
			}
			if ( !HasAmmo() )
			{
				EndAttack();
				m_fireState = FIRE_OFF;
				m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 1.0f;
			}

		}
		break;
	}
}

void CEgon::PrimaryAttack( void )
{
	m_fireMode = FIRE_WIDE;
	Attack();
}

void CEgon::SecondaryAttack( void )
{
	WeaponIdle();
}

void CEgon::Fire( const Vector &vecOrigSrc, const Vector &vecDir )
{
	Vector vecDest = vecOrigSrc + vecDir * 2048;
	edict_t		*pentIgnore;
	TraceResult tr;

	pentIgnore = m_pPlayer->edict();
	Vector tmpSrc = vecOrigSrc + gpGlobals->v_up * -8 + gpGlobals->v_right * 3;

	// ALERT( at_console, "." );
	
	UTIL_TraceLine( vecOrigSrc, vecDest, dont_ignore_monsters, pentIgnore, &tr );

	if (tr.fAllSolid)
		return;

	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);

	if (pEntity == NULL)
		return;

	if ( g_pGameRules->Ps2Virt07() )
	{
		if ( m_pSprite && pEntity->pev->takedamage )
		{
			m_pSprite->pev->effects &= ~EF_NODRAW;
		}
		else if ( m_pSprite )
		{
			m_pSprite->pev->effects |= EF_NODRAW;
		}
	}

	float timedist = 0;

	switch ( m_fireMode )
	{
	case FIRE_NARROW:
		if ( pev->dmgtime < gpGlobals->time )
		{
			// Narrow mode only does damage to the entity it hits
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonNarrow, vecDir, &tr, DMG_ENERGYBEAM );
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->Ps2Virt07() )
			{
				// multiplayer uses 1 ammo every 1/10th second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1f;
				}
			}
			else
			{
				// single player, use 3 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.166f;
				}
			}

			pev->dmgtime = gpGlobals->time + GetPulseInterval();
		}
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetPulseInterval();
		break;
	
	case FIRE_WIDE:
		if ( pev->dmgtime < gpGlobals->time )
		{
			// wide mode does damage to the ent, and radius damage
			ClearMultiDamage();
			if (pEntity->pev->takedamage)
			{
				pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgEgonWide, vecDir, &tr, DMG_ENERGYBEAM | DMG_ALWAYSGIB);
			}
			ApplyMultiDamage(m_pPlayer->pev, m_pPlayer->pev);

			if ( g_pGameRules->Ps2Virt07() )
			{
				// radius damage a little more potent in multiplayer.
				::RadiusDamage( tr.vecEndPos, pev, m_pPlayer->pev, gSkillData.plrDmgEgonWide/4, 128, CLASS_NONE, DMG_ENERGYBEAM | DMG_BLAST | DMG_ALWAYSGIB );
			}

			if ( !m_pPlayer->IsAlive() )
				return;

			if ( g_pGameRules->Ps2Virt07() )
			{
				//multiplayer uses 5 ammo/second
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.2f;
				}
			}
			else
			{
				// Wide mode uses 10 charges per second in single player
				if ( gpGlobals->time >= m_flAmmoUseTime )
				{
					UseAmmo( 1 );
					m_flAmmoUseTime = gpGlobals->time + 0.1f;
				}
			}

			pev->dmgtime = gpGlobals->time + GetDischargeInterval();
			if ( m_shakeTime < gpGlobals->time )
			{
				UTIL_ScreenShake( tr.vecEndPos, 5.0f, 150.0f, 0.75f, 250.0f );
				m_shakeTime = gpGlobals->time + 1.5f;
			}
		}
		timedist = ( pev->dmgtime - gpGlobals->time ) / GetDischargeInterval();
		break;
	}

	if ( timedist < 0 )
		timedist = 0;
	else if ( timedist > 1 )
		timedist = 1;
	timedist = 1-timedist;

	UpdateEffect( tmpSrc, tr.vecEndPos, timedist );
}

void CEgon::UpdateEffect( const Vector &startPoint, const Vector &endPoint, float timeBlend )
{
	if ( !m_pBeam )
	{
		CreateEffect();
	}

	m_pBeam->RelinkBeam();
	m_pBeam->SetStartPos( endPoint );
	m_pBeam->SetBrightness( 255 - (timeBlend*180) );
	m_pBeam->SetWidth( 40 - (timeBlend*20) );

	if ( m_fireMode == FIRE_WIDE )
		m_pBeam->SetColor( 30 + (25*timeBlend), 30 + (30*timeBlend), 64 + 80*fabsf(sinf(gpGlobals->time*10)) );
	else
		m_pBeam->SetColor( 60 + (25*timeBlend), 120 + (30*timeBlend), 64 + 80*fabsf(sinf(gpGlobals->time*10)) );

	UTIL_SetOrigin( m_pSprite->pev, endPoint );
	m_pSprite->pev->frame += 8 * gpGlobals->frametime;
	if ( m_pSprite->pev->frame > m_pSprite->Frames() )
		m_pSprite->pev->frame = 0;

	m_pNoise->SetStartPos( endPoint );
	m_pNoise->RelinkBeam();
}

void CEgon::CreateEffect( void )
{
	DestroyEffect();

	m_pBeam = CBeam::BeamCreate( EGON_BEAM_SPRITE, 40 );
	m_pBeam->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pBeam->SetFlags( BEAM_FSINE );
	m_pBeam->SetEndAttachment( 1 );
	m_pBeam->pev->spawnflags |= SF_BEAM_TEMPORARY;	// Flag these to be destroyed on save/restore or level transition

	m_pNoise = CBeam::BeamCreate( EGON_BEAM_SPRITE, 55 );
	m_pNoise->PointEntInit( pev->origin, m_pPlayer->entindex() );
	m_pNoise->SetScrollRate( 25 );
	m_pNoise->SetBrightness( 100 );
	m_pNoise->SetEndAttachment( 1 );
	m_pNoise->pev->spawnflags |= SF_BEAM_TEMPORARY;

	m_pSprite = CSprite::SpriteCreate( EGON_FLARE_SPRITE, pev->origin, FALSE );
	m_pSprite->pev->scale = 1.0f;
	m_pSprite->SetTransparency( kRenderGlow, 255, 255, 255, 255, kRenderFxNoDissipation );
	m_pSprite->pev->spawnflags |= SF_SPRITE_TEMPORARY;

	if ( m_fireMode == FIRE_WIDE )
	{
		m_pBeam->SetScrollRate( 50 );
		m_pBeam->SetNoise( 20 );
		m_pNoise->SetColor( 50, 50, 255 );
		m_pNoise->SetNoise( 8 );
	}
	else
	{
		m_pBeam->SetScrollRate( 110 );
		m_pBeam->SetNoise( 5 );
		m_pNoise->SetColor( 80, 120, 255 );
		m_pNoise->SetNoise( 2 );
	}
}

void CEgon::DestroyEffect( void )
{
	if ( m_pBeam )
	{
		UTIL_Remove( m_pBeam );
		m_pBeam = NULL;
	}
	if ( m_pNoise )
	{
		UTIL_Remove( m_pNoise );
		m_pNoise = NULL;
	}
	if ( m_pSprite )
	{
		if ( m_fireMode == FIRE_WIDE )
			m_pSprite->Expand( 10, 500 );
		else
			UTIL_Remove( m_pSprite );
		m_pSprite = NULL;
	}
}

void CEgon::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if ( m_fireState != FIRE_OFF )
		 EndAttack();
	
	int iAnim;

	float flRand = RANDOM_FLOAT(0,1);

	if ( flRand <= 0.5f )
	{
		iAnim = EGON_IDLE1;
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT(10,15);
	}
	else 
	{
		iAnim = EGON_FIDGET1;
		m_flTimeWeaponIdle = gpGlobals->time + 3;
	}

	SendWeaponAnim( iAnim );
	m_deployed = TRUE;
}

void CEgon::EndAttack( void )
{
	STOP_SOUND( ENT(m_pPlayer->pev), CHAN_STATIC, EGON_SOUND_RUN );
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, EGON_SOUND_OFF, 0.98f, ATTN_NORM, 0, 100); 

	m_fireState = FIRE_OFF;
	m_flTimeWeaponIdle = gpGlobals->time + 2.0f;
	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.5f;

	DestroyEffect();
}

class CEgonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_chainammo.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_chainammo.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_URANIUMBOX_GIVE, "uranium", URANIUM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_egonclip, CEgonAmmo );
