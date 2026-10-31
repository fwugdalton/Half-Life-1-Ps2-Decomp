/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/* dlls/healthkit.cpp: o kit médico (item_healthkit) e o carregador de parede
   (func_healthcharger) do HLSDK. O PS2 acrescenta o carregador novo com braço e
   frasco (item_healthcharger e item_healthchargercan, os do Decay). */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/skill.h"
#include "dlls/gamerules.h"
#include "include_asm.h"
#include "inline_include_asm.h"

#define WEAPON_SUIT			31
#define TE_ELIGHT			28

enum { ignore_glass = 1, dont_ignore_glass = 0 };
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, int igmon, int ignoreGlass, edict_t *pentIgnore, TraceResult *ptr );

class CItem : public CBaseEntity
{
public:
	void	Spawn( void );
	CBaseEntity*	Respawn( void );
	void	EXPORT ItemTouch( CBaseEntity *pOther );
	void	EXPORT Materialize( void );
	virtual BOOL MyTouch( CBasePlayer *pPlayer ) { return FALSE; };
};

extern int gmsgItemPickup;

class CHealthKit : public CItem
{
	void Spawn( void );
	void Precache( void );
	BOOL MyTouch( CBasePlayer *pPlayer );

/*
	virtual int		Save( CSave &save ); 
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];
*/

};


LINK_ENTITY_TO_CLASS( item_healthkit, CHealthKit );

/*
TYPEDESCRIPTION	CHealthKit::m_SaveData[] = 
{

};


IMPLEMENT_SAVERESTORE( CHealthKit, CItem);
*/

void CHealthKit :: Spawn( void )
{
	Precache( );
	SET_MODEL(ENT(pev), "models/w_medkit.dol");

	CItem::Spawn();
}

void CHealthKit::Precache( void )
{
	PRECACHE_MODEL("models/w_medkit.dol");
	PRECACHE_SOUND("items/smallmedkit1.wav");
}

BOOL CHealthKit::MyTouch( CBasePlayer *pPlayer )
{
	// PS2: flag 0x40 do m_afPhysicsFlags impede pegar (exceto com impulse 101)
	if ( !gEvilImpulse101 && ( pPlayer->m_afPhysicsFlags & 0x40 ) )
	{
		return FALSE;
	}

	if ( pPlayer->TakeHealth( gSkillData.healthkitCapacity, DMG_GENERIC ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgItemPickup, NULL, pPlayer->pev );
			WRITE_STRING( STRING(pev->classname) );
		MESSAGE_END();

		EMIT_SOUND(ENT(pPlayer->pev), CHAN_ITEM, "items/smallmedkit1.wav", 1, ATTN_NORM);

		if ( g_pGameRules->ItemShouldRespawn( this ) )
		{
			Respawn();
		}
		else
		{
			UTIL_Remove(this);	
		}

		return TRUE;
	}

	return FALSE;
}



//-------------------------------------------------------------
// Wall mounted health kit
//-------------------------------------------------------------
class CWallHealth : public CBaseToggle
{
public:
	void Spawn( );
	void Precache( void );
	void EXPORT Off(void);
	void EXPORT Recharge(void);
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return (CBaseToggle :: ObjectCaps() | FCAP_CONTINUOUS_USE) & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	float m_flNextCharge; 
	int		m_iReactivate ; // DeathMatch Delay until reactvated
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float   m_flSoundTime;
};

TYPEDESCRIPTION CWallHealth::m_SaveData[] =
{
	DEFINE_FIELD( CWallHealth, m_flNextCharge, FIELD_TIME),
	DEFINE_FIELD( CWallHealth, m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD( CWallHealth, m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD( CWallHealth, m_iOn, FIELD_INTEGER),
	DEFINE_FIELD( CWallHealth, m_flSoundTime, FIELD_TIME),
};

IMPLEMENT_SAVERESTORE( CWallHealth, CBaseEntity );

LINK_ENTITY_TO_CLASS(func_healthcharger, CWallHealth);


void CWallHealth::KeyValue( KeyValueData *pkvd )
{
	if (	FStrEq(pkvd->szKeyName, "style") ||
				FStrEq(pkvd->szKeyName, "height") ||
				FStrEq(pkvd->szKeyName, "value1") ||
				FStrEq(pkvd->szKeyName, "value2") ||
				FStrEq(pkvd->szKeyName, "value3"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "dmdelay"))
	{
		m_iReactivate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CWallHealth::Spawn()
{
	Precache( );

	pev->solid		= SOLID_BSP;
	pev->movetype	= MOVETYPE_PUSH;

	UTIL_SetOrigin(pev, pev->origin);		// set size and link into world
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SET_MODEL(ENT(pev), STRING(pev->model) );
	m_iJuice = gSkillData.healthchargerCapacity;
	pev->frame = 0;			

}

void CWallHealth::Precache()
{
	PRECACHE_SOUND("items/medshot4.wav");
	PRECACHE_SOUND("items/medshotno1.wav");
	PRECACHE_SOUND("items/medcharge4.wav");
}


void CWallHealth::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
	// Make sure that we have a caller
	if (!pActivator)
		return;
	// if it's not a player, ignore
	if ( !pActivator->IsPlayer() )
		return;

	// if there is no juice left, turn it off
	if (m_iJuice <= 0)
	{
		pev->frame = 1;			
		Off();
	}

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ((m_iJuice <= 0) || (!(pActivator->pev->weapons & (1<<WEAPON_SUIT))))
	{
		if (m_flSoundTime <= gpGlobals->time)
		{
			m_flSoundTime = gpGlobals->time + 0.62f;
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshotno1.wav", 1.0f, ATTN_NORM );
		}
		return;
	}

	pev->nextthink = pev->ltime + 0.25f;
	SetThink(&CWallHealth::Off);

	// Time to recharge yet?

	if (m_flNextCharge >= gpGlobals->time)
		return;

	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0f, ATTN_NORM );
		m_flSoundTime = 0.56f + gpGlobals->time;
	}
	if ((m_iOn == 1) && (m_flSoundTime <= gpGlobals->time))
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/medcharge4.wav", 1.0f, ATTN_NORM );
	}


	// charge the player
	// PS2: com a flag 0x40 o carregador tira vida em vez de dar
	if ( pActivator && pActivator->IsPlayer() && ( ((CBasePlayer *)pActivator)->m_afPhysicsFlags & 0x40 ) )
	{
		pActivator->TakeDamage( pActivator->pev, pActivator->pev, 1, DMG_GENERIC );
		m_iJuice--;
	}
	else if ( pActivator->TakeHealth( 1, DMG_GENERIC ) )
	{
		m_iJuice--;
	}

	// govern the rate of charge
	m_flNextCharge = gpGlobals->time + 0.1f;
}

void CWallHealth::Recharge(void)
{
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0f, ATTN_NORM );
	m_iJuice = gSkillData.healthchargerCapacity;
	pev->frame = 0;			
	SetThink( &CWallHealth::SUB_DoNothing );
}

void CWallHealth::Off(void)
{
	// Stop looping sound.
	if (m_iOn > 1)
		STOP_SOUND( ENT(pev), CHAN_STATIC, "items/medcharge4.wav" );

	m_iOn = 0;

	if ((!m_iJuice) &&  ( ( m_iReactivate = g_pGameRules->FlHealthChargerRechargeTime() ) > 0.0f) )
	{
		pev->nextthink = pev->ltime + m_iReactivate;
		SetThink(&CWallHealth::Recharge);
	}
	else
		SetThink( &CWallHealth::SUB_DoNothing );
}

//-------------------------------------------------------------
// PS2: carregador novo (o do Decay), com braço que segue o jogador
// e um frasco separado (item_healthchargercan) que mostra a carga.
//-------------------------------------------------------------
class CWallHealthNewCanister : public CBaseAnimating
{
public:
	void Spawn( void );
	void Precache( void );
	void EXPORT CanisterThink( void );
	void SetJuice( int iJuice );
	void SetDraining( BOOL fDraining );
	static CWallHealthNewCanister *Create( Vector vecOrigin, Vector vecAngles );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_fDraining;
	BOOL	m_fSettling;
};

LINK_ENTITY_TO_CLASS( item_healthchargercan, CWallHealthNewCanister );

TYPEDESCRIPTION CWallHealthNewCanister::m_SaveData[] =
{
	DEFINE_FIELD( CWallHealthNewCanister, m_fDraining, FIELD_BOOLEAN ),
	DEFINE_FIELD( CWallHealthNewCanister, m_fSettling, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CWallHealthNewCanister, CBaseAnimating );

void CWallHealthNewCanister::Spawn( void )
{
	Precache();

	pev->solid = SOLID_NOT;
	SET_MODEL( ENT(pev), "models/health_charger_both.dol" );
	pev->movetype = MOVETYPE_NONE;
	UTIL_SetOrigin( pev, pev->origin );

	pev->rendermode = kRenderTransAlpha;
	pev->renderamt = 150;

	pev->sequence = LookupSequence( "still" );
	pev->frame = 0;
	ResetSequenceInfo();
	SetBoneController( 0, 0 );

	SetThink( &CWallHealthNewCanister::CanisterThink );
	pev->nextthink = gpGlobals->time + 0.1f;

	m_fDraining = FALSE;
	m_fSettling = FALSE;
}

void CWallHealthNewCanister::Precache( void )
{
	PRECACHE_MODEL( "models/health_charger_both.dol" );
}

void CWallHealthNewCanister::CanisterThink( void )
{
	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished && !m_fSequenceLoops )
	{
		if ( m_fSettling )
		{
			pev->sequence = LookupSequence( "still" );
			m_fSettling = FALSE;
		}
		pev->frame = 0;
		ResetSequenceInfo();
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

void CWallHealthNewCanister::SetJuice( int iJuice )
{
	SetBoneController( 0, iJuice * 0.1f + -11 );
}

void CWallHealthNewCanister::SetDraining( BOOL fDraining )
{
	if ( fDraining )
	{
		if ( !m_fDraining )
		{
			m_fSettling = FALSE;
			m_fDraining = TRUE;
			pev->sequence = LookupSequence( "slosh" );
			pev->frame = 0;
			ResetSequenceInfo();
		}
	}
	else
	{
		if ( m_fDraining )
		{
			m_fDraining = FALSE;
			m_fSettling = TRUE;
			pev->sequence = LookupSequence( "to_rest" );
			pev->frame = 0;
			ResetSequenceInfo();
		}
	}
}

CWallHealthNewCanister *CWallHealthNewCanister::Create( Vector vecOrigin, Vector vecAngles )
{
	CWallHealthNewCanister *pCanister = GetClassPtr( (CWallHealthNewCanister *)NULL );

	pCanister->pev->classname = MAKE_STRING( "item_healthchargercan" );
	pCanister->pev->origin = vecOrigin;
	pCanister->pev->angles = vecAngles;
	pCanister->Spawn();

	return pCanister;
}

class CWallHealthNew : public CBaseToggle
{
public:
	void Spawn( );
	void Precache( void );
	void EXPORT Off( void );
	void EXPORT Recharge( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual int	ObjectCaps( void ) { return CBaseToggle :: ObjectCaps() | FCAP_CONTINUOUS_USE; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	BOOL FindPlayer( void );
	float VecToYaw( Vector vecDir );
	void SetIdealYaws( void );
	void SetLights( BOOL fOn );
	void EXPORT InitThink( void );
	void EXPORT IdleThink( void );

	static	TYPEDESCRIPTION m_SaveData[];

	float	m_flNextCharge;
	float	m_flReactivate;
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float	m_flSoundTime;
	float	m_flNextReactivate;
	EHANDLE	m_hCanister;
	EHANDLE	m_hPlayer;
	BOOL	m_fPlayerIsNear;
	int		m_iIdleState;
	float	m_flIdealArmYaw;
	float	m_flIdealEyeYaw;
	float	m_flCurrentArmYaw;
	float	m_flCurrentEyeYaw;
	float	m_flOffTime;
	BOOL	m_fLightsOn;
	BOOL	m_fRestored;	// não vai para o save
};

LINK_ENTITY_TO_CLASS( item_healthcharger, CWallHealthNew );

TYPEDESCRIPTION CWallHealthNew::m_SaveData[] =
{
	DEFINE_FIELD( CWallHealthNew, m_flNextCharge, FIELD_TIME ),
	DEFINE_FIELD( CWallHealthNew, m_flReactivate, FIELD_FLOAT ),
	DEFINE_FIELD( CWallHealthNew, m_iJuice, FIELD_INTEGER ),
	DEFINE_FIELD( CWallHealthNew, m_iOn, FIELD_INTEGER ),
	DEFINE_FIELD( CWallHealthNew, m_flSoundTime, FIELD_TIME ),
	DEFINE_FIELD( CWallHealthNew, m_flNextReactivate, FIELD_TIME ),
	DEFINE_FIELD( CWallHealthNew, m_hCanister, FIELD_EHANDLE ),
	DEFINE_FIELD( CWallHealthNew, m_hPlayer, FIELD_EHANDLE ),
	DEFINE_FIELD( CWallHealthNew, m_fPlayerIsNear, FIELD_BOOLEAN ),
	DEFINE_FIELD( CWallHealthNew, m_iIdleState, FIELD_INTEGER ),
	DEFINE_FIELD( CWallHealthNew, m_flIdealArmYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CWallHealthNew, m_flIdealEyeYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CWallHealthNew, m_flCurrentArmYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CWallHealthNew, m_flCurrentEyeYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CWallHealthNew, m_flOffTime, FIELD_TIME ),
	DEFINE_FIELD( CWallHealthNew, m_fLightsOn, FIELD_BOOLEAN ),
};

int CWallHealthNew::Save( CSave &save )
{
	if ( !CBaseToggle::Save(save) )
		return 0;
	return save.WriteFields( "CWallHealthNew", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CWallHealthNew::Restore( CRestore &restore )
{
	if ( !CBaseToggle::Restore(restore) )
		return 0;
	m_fRestored = TRUE;
	return restore.ReadFields( "CWallHealthNew", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

void CWallHealthNew::KeyValue( KeyValueData *pkvd )
{
	if (	FStrEq(pkvd->szKeyName, "style") ||
				FStrEq(pkvd->szKeyName, "height") ||
				FStrEq(pkvd->szKeyName, "value1") ||
				FStrEq(pkvd->szKeyName, "value2") ||
				FStrEq(pkvd->szKeyName, "value3"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "dmdelay") )
	{
		m_flReactivate = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CWallHealthNew::Spawn( void )
{
#ifndef NON_MATCHING
	/* não casa (a conversão float->int do m_iJuice sai trunc+mfc1+sw no alvo e swc1 no
	   nosso); o corpo vem do asm, mas a função continua em C para o gcc emitir a vtable */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/dlls/healthkit", Spawn__14CWallHealthNew );
#endif
	Precache( );

	pev->solid = SOLID_BBOX;
	SET_MODEL( ENT(pev), "models/health_charger_body.dol" );

	if ( pev->angles.y < 0 )
		pev->angles.y += 360;

	if ( pev->angles.y == 0 || pev->angles.y == 180 )
		UTIL_SetSize( pev, Vector(-16, -18, 0), Vector(16, 18, 32) );
	else
		UTIL_SetSize( pev, Vector(-18, -16, 0), Vector(18, 16, 32) );

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		pev->view_ofs = Vector( 4, -8, 32 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		pev->view_ofs = Vector( 8, -4, 32 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		pev->view_ofs = Vector( -4, 8, 32 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		pev->view_ofs = Vector( -8, 4, 32 );

	m_hCanister = CWallHealthNewCanister::Create( pev->origin, pev->angles );

	m_flNextReactivate = 0;
	m_fPlayerIsNear = FALSE;
	m_iIdleState = 0;
	m_fLightsOn = FALSE;

	pev->movetype = MOVETYPE_PUSH;
	pev->skin = 0;

	pev->sequence = LookupSequence( "still" );
	pev->frame = 0;
	ResetSequenceInfo();
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	m_flOffTime = m_flIdealEyeYaw = m_flCurrentEyeYaw = m_flIdealArmYaw = m_flCurrentArmYaw = 0;

	UTIL_SetOrigin( pev, pev->origin );
	m_iJuice = gSkillData.healthchargerCapacity;

	SetThink( &CWallHealthNew::InitThink );
	pev->nextthink = pev->ltime + 0.5f;
}

void CWallHealthNew::Precache( void )
{
	PRECACHE_MODEL( "models/health_charger_body.dol" );
	UTIL_PrecacheOther( "item_healthchargercan" );
	PRECACHE_SOUND( "items/medshot4.wav" );
	PRECACHE_SOUND( "items/medshotno1.wav" );
	PRECACHE_SOUND( "items/medcharge4.wav" );
}

void CWallHealthNew::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CWallHealthNewCanister *pCanister = (CWallHealthNewCanister *)(CBaseEntity *)m_hCanister;

	// Make sure that we have a caller
	if ( !pActivator )
		return;
	// if it's not a player, ignore
	if ( !pActivator->IsPlayer() )
		return;

	if ( (pActivator->pev->origin - pev->origin).Length2D() > 48 )
		return;

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ( (m_iJuice <= 0) || (!(pActivator->pev->weapons & (1<<WEAPON_SUIT))) )
	{
		if ( m_flSoundTime <= gpGlobals->time )
		{
			m_flSoundTime = gpGlobals->time + 0.62f;
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/medshotno1.wav", 1.0f, ATTN_NORM );
		}
		return;
	}

	m_flOffTime = gpGlobals->time + 0.25f;

	// Time to recharge yet?
	if ( m_flNextCharge >= gpGlobals->time )
		return;

	// Play the on sound or the looping charging sound
	if ( !m_iOn )
	{
		m_iOn++;
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0f, ATTN_NORM );
		m_flSoundTime = gpGlobals->time + 0.56f;
		pCanister->SetDraining( TRUE );
		pev->sequence = LookupSequence( "give_shot" );
		pev->frame = 0;
		ResetSequenceInfo();
	}
	if ( (m_iOn == 1) && (m_flSoundTime <= gpGlobals->time) )
	{
		m_iOn++;
		EMIT_SOUND( ENT(pev), CHAN_STATIC, "items/medcharge4.wav", 1.0f, ATTN_NORM );
	}

	// charge the player
	if ( pActivator && pActivator->IsPlayer() && ( ((CBasePlayer *)pActivator)->m_afPhysicsFlags & 0x40 ) )
	{
		pActivator->TakeDamage( pActivator->pev, pActivator->pev, 1, DMG_GENERIC );
		m_iJuice--;
	}
	else if ( pActivator->TakeHealth( 1, DMG_GENERIC ) )
	{
		m_iJuice--;
		pCanister->SetJuice( (int)( m_iJuice / gSkillData.healthchargerCapacity * 100 ) );
	}

	// if there is no juice left, turn it off
	if ( m_iJuice <= 0 )
	{
		pev->skin = 1;
		Off();
	}

	// govern the rate of charge
	m_flNextCharge = gpGlobals->time + 0.1f;
}

void CWallHealthNew::Recharge( void )
{
#ifndef NON_MATCHING
	/* não casa pelo mesmo motivo do Spawn (trunc+mfc1+sw do m_iJuice) */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/dlls/healthkit", Recharge__14CWallHealthNew );
#endif
	CWallHealthNewCanister *pCanister = (CWallHealthNewCanister *)(CBaseEntity *)m_hCanister;

	EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0f, ATTN_NORM );
	m_iJuice = gSkillData.healthchargerCapacity;
	m_flNextReactivate = 0;
	pCanister->SetJuice( 100 );
}

void CWallHealthNew::Off( void )
{
	CWallHealthNewCanister *pCanister = (CWallHealthNewCanister *)(CBaseEntity *)m_hCanister;

	// Stop looping sound.
	if ( m_iOn > 1 )
		STOP_SOUND( ENT(pev), CHAN_STATIC, "items/medcharge4.wav" );

	m_iOn = 0;

	pev->sequence = LookupSequence( "retract_shot" );
	pev->frame = 0;
	ResetSequenceInfo();
	pCanister->SetDraining( FALSE );

	if ( (!m_iJuice) && ( ( m_flReactivate = g_pGameRules->FlHealthChargerRechargeTime() ) > 0.0f ) )
		m_flNextReactivate = gpGlobals->time + m_flReactivate;

	m_flOffTime = 0;
}

BOOL CWallHealthNew::FindPlayer( void )
{
	TraceResult tr;
	Vector vecSrc, vecEnd;
	BOOL fVisible = TRUE;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

		if ( pPlayer && (pPlayer->pev->origin - pev->origin).Length() < 96 )
		{
			if ( !(CBaseEntity *)m_hPlayer )
			{
				vecSrc = pev->origin + pev->view_ofs;
				vecEnd = pPlayer->EyePosition();
				UTIL_TraceLine( vecSrc, vecEnd, ignore_monsters, ignore_glass, ENT(pev), &tr );
				if ( tr.flFraction != 1.0f )
					fVisible = FALSE;
			}

			if ( fVisible )
			{
				m_hPlayer = pPlayer;
				return TRUE;
			}
		}
	}

	m_hPlayer = NULL;
	return FALSE;
}

float CWallHealthNew::VecToYaw( Vector vecDir )
{
	if ( vecDir.x == 0 && vecDir.y == 0 && vecDir.z == 0 )
		return pev->angles.y;

	return UTIL_VecToYaw( vecDir );
}

void CWallHealthNew::SetIdealYaws( void )
{
	float flYaw;
	Vector vecSrc;

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		vecSrc = pev->origin + Vector( 4, 8, 16 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		vecSrc = pev->origin + Vector( -8, -4, 16 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		vecSrc = pev->origin + Vector( -4, -8, 16 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		vecSrc = pev->origin + Vector( 8, 4, 16 );

	flYaw = VecToYaw( m_hPlayer->pev->origin - vecSrc ) - pev->angles.y;

	if ( flYaw > 180 )
		flYaw -= 360;
	if ( flYaw < -180 )
		flYaw += 360;

	m_flIdealArmYaw = flYaw;

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		vecSrc = pev->origin + Vector( 4, -8, 32 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		vecSrc = pev->origin + Vector( 8, -4, 32 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		vecSrc = pev->origin + Vector( -4, 8, 32 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		vecSrc = pev->origin + Vector( -8, 4, 32 );

	flYaw = VecToYaw( m_hPlayer->pev->origin - vecSrc ) - pev->angles.y;

	if ( flYaw > 180 )
		flYaw -= 360;
	if ( flYaw < -180 )
		flYaw += 360;

	m_flIdealEyeYaw = flYaw;
}

void CWallHealthNew::SetLights( BOOL fOn )
{
	if ( fOn == m_fLightsOn )
		return;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( entindex() + 0x1000 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( 16 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( !fOn );
		WRITE_COORD( 0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( entindex() + 0x2000 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( 16 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( !fOn );
		WRITE_COORD( 0 );
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( entindex() + 0x3000 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( 8 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( !fOn );
		WRITE_COORD( 0 );
	MESSAGE_END();

	m_fLightsOn = fOn;
}

void CWallHealthNew::InitThink( void )
{
	SetThink( &CWallHealthNew::IdleThink );
	pev->nextthink = pev->ltime + 0.1f;
}

void CWallHealthNew::IdleThink( void )
{
	if ( m_fRestored && m_fLightsOn )
	{
		m_fLightsOn = FALSE;
		m_fRestored = FALSE;
	}

	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished )
	{
		if ( m_fPlayerIsNear && m_iIdleState == 2 && m_iOn )
		{
			pev->sequence = LookupSequence( "shot_idle" );
			pev->frame = 0;
			ResetSequenceInfo();
		}
		else if ( m_fPlayerIsNear && m_iIdleState != 2 && !m_iOn )
		{
			pev->sequence = LookupSequence( "prep_shot" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iIdleState = 2;
		}
		else if ( !m_fPlayerIsNear && m_iIdleState != 1 && !m_iOn )
		{
			pev->sequence = LookupSequence( "still" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iIdleState = 1;
		}
	}

	if ( m_flNextReactivate > 0 && m_flNextReactivate < gpGlobals->time )
		Recharge();

	if ( m_flOffTime > 0 && m_flOffTime < gpGlobals->time )
		Off();

	if ( m_iJuice > 0 && FindPlayer() )
	{
		if ( !m_fPlayerIsNear )
		{
			pev->sequence = LookupSequence( "deploy" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_fPlayerIsNear = TRUE;
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/medshot4.wav", 1.0f, ATTN_NORM );
			m_flSoundTime = gpGlobals->time + 0.56f;
		}

		SetIdealYaws();

		if ( m_flIdealArmYaw != m_flCurrentArmYaw )
		{
			if ( m_flIdealArmYaw < m_flCurrentArmYaw )
				m_flCurrentArmYaw = m_flCurrentArmYaw - min( m_flCurrentArmYaw - m_flIdealArmYaw, 5 );
			else
				m_flCurrentArmYaw = m_flCurrentArmYaw + min( m_flIdealArmYaw - m_flCurrentArmYaw, 5 );
			m_flCurrentArmYaw = m_flIdealArmYaw;
			SetBoneController( 0, m_flCurrentArmYaw );
		}

		if ( m_flIdealEyeYaw != m_flCurrentEyeYaw )
		{
			if ( m_flIdealEyeYaw < m_flCurrentEyeYaw )
				m_flCurrentEyeYaw = m_flCurrentEyeYaw - min( m_flCurrentEyeYaw - m_flIdealEyeYaw, 5 );
			else
				m_flCurrentEyeYaw = m_flCurrentEyeYaw + min( m_flIdealEyeYaw - m_flCurrentEyeYaw, 5 );
			m_flCurrentEyeYaw = m_flIdealEyeYaw;
			SetBoneController( 1, m_flCurrentEyeYaw );
		}
	}
	else if ( m_fPlayerIsNear )
	{
		m_flIdealArmYaw = 0;
		float flYaw = m_flIdealArmYaw;	// o alvo relê o campo e usa o valor também no frame
		SetBoneController( 0, flYaw );
		pev->sequence = LookupSequence( "retract_arm" );
		pev->frame = flYaw;
		ResetSequenceInfo();
		m_fPlayerIsNear = FALSE;
	}

	pev->nextthink = pev->ltime + 0.1f;
}

/* registro dos ponteiros de Think para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_00186AA0(void)
{
	func_002397D0("143", static_cast<BASEPTR>(&CWallHealth::SUB_DoNothing));
	func_002397D0("144", static_cast<BASEPTR>(&CWallHealth::Recharge));
	func_002397D0("145", static_cast<BASEPTR>(&CWallHealthNew::IdleThink));
	func_002397D0("146", static_cast<BASEPTR>(&CWallHealthNew::InitThink));
	func_002397D0("147", static_cast<BASEPTR>(&CWallHealthNewCanister::CanisterThink));
	func_002397D0("148", static_cast<BASEPTR>(&CWallHealth::Off));
}
