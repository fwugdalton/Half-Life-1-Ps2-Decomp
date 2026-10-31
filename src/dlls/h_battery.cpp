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
/* dlls/h_battery.cpp: o carregador de HEV de parede (func_recharge) do HLSDK.
   O PS2 acrescenta o carregador novo com braço, cilindro giratório e raio
   (item_recharge, o do Decay) e o campo de força que o acompanha
   (item_rechargefield). */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/skill.h"
#include "dlls/gamerules.h"
#include "dlls/effects.h"
#include "include_asm.h"

#define WEAPON_SUIT			31
#define TE_ELIGHT			28

enum { ignore_glass = 1, dont_ignore_glass = 0 };
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, int igmon, int ignoreGlass, edict_t *pentIgnore, TraceResult *ptr );

class CRecharge : public CBaseToggle
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

TYPEDESCRIPTION CRecharge::m_SaveData[] =
{
	DEFINE_FIELD( CRecharge, m_flNextCharge, FIELD_TIME ),
	DEFINE_FIELD( CRecharge, m_iReactivate, FIELD_INTEGER),
	DEFINE_FIELD( CRecharge, m_iJuice, FIELD_INTEGER),
	DEFINE_FIELD( CRecharge, m_iOn, FIELD_INTEGER),
	DEFINE_FIELD( CRecharge, m_flSoundTime, FIELD_TIME ),
};

IMPLEMENT_SAVERESTORE( CRecharge, CBaseEntity );

LINK_ENTITY_TO_CLASS(func_recharge, CRecharge);


void CRecharge::KeyValue( KeyValueData *pkvd )
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

void CRecharge::Spawn()
{
	Precache( );

	pev->solid		= SOLID_BSP;
	pev->movetype	= MOVETYPE_PUSH;

	UTIL_SetOrigin(pev, pev->origin);		// set size and link into world
	UTIL_SetSize(pev, pev->mins, pev->maxs);
	SET_MODEL(ENT(pev), STRING(pev->model) );
	m_iJuice = gSkillData.suitchargerCapacity;
	pev->frame = 0;
}

void CRecharge::Precache()
{
	PRECACHE_SOUND("items/suitcharge1.wav");
	PRECACHE_SOUND("items/suitchargeno1.wav");
	PRECACHE_SOUND("items/suitchargeok1.wav");
}


void CRecharge::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// if it's not a player, ignore
	if (!FClassnameIs(pActivator->pev, "player"))
		return;

	// PS2: com a flag 0x40 do m_afPhysicsFlags o carregador não funciona
	if ( pActivator && pActivator->IsPlayer() && ( ((CBasePlayer *)pActivator)->m_afPhysicsFlags & 0x40 ) )
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
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeno1.wav", 0.85f, ATTN_NORM );
		}
		return;
	}

	pev->nextthink = pev->ltime + 0.25f;
	SetThink(&CRecharge::Off);

	// Time to recharge yet?

	if (m_flNextCharge >= gpGlobals->time)
		return;

	// Make sure that we have a caller
	if (!pActivator)
		return;

	m_hActivator = pActivator;

	//only recharge the player

	if (!m_hActivator->IsPlayer() )
		return;

	// Play the on sound or the looping charging sound
	if (!m_iOn)
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85f, ATTN_NORM );
		m_flSoundTime = 0.56f + gpGlobals->time;
	}
	if ((m_iOn == 1) && (m_flSoundTime <= gpGlobals->time))
	{
		m_iOn++;
		EMIT_SOUND(ENT(pev), CHAN_STATIC, "items/suitcharge1.wav", 0.85f, ATTN_NORM );
	}

	// charge the player
	if (m_hActivator->pev->armorvalue < 100)
	{
		m_iJuice--;
		m_hActivator->pev->armorvalue += 1;

		if (m_hActivator->pev->armorvalue > 100)
			m_hActivator->pev->armorvalue = 100;
	}

	// govern the rate of charge
	m_flNextCharge = gpGlobals->time + 0.1f;
}

void CRecharge::Recharge(void)
{
	m_iJuice = gSkillData.suitchargerCapacity;
	pev->frame = 0;
	SetThink( &CRecharge::SUB_DoNothing );
}

void CRecharge::Off(void)
{
	// Stop looping sound.
	if (m_iOn > 1)
		STOP_SOUND( ENT(pev), CHAN_STATIC, "items/suitcharge1.wav" );

	m_iOn = 0;

	if ((!m_iJuice) &&  ( ( m_iReactivate = g_pGameRules->FlHEVChargerRechargeTime() ) > 0) )
	{
		pev->nextthink = pev->ltime + m_iReactivate;
		SetThink(&CRecharge::Recharge);
	}
	else
		SetThink( &CRecharge::SUB_DoNothing );
}

//-------------------------------------------------------------
// PS2: campo de força do carregador novo
//-------------------------------------------------------------
class CRechargeField : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	static CRechargeField *Create( Vector vecOrigin, Vector vecAngles );
};

LINK_ENTITY_TO_CLASS( item_rechargefield, CRechargeField );

void CRechargeField::Spawn( void )
{
	Precache();

	pev->solid = SOLID_NOT;
	SET_MODEL( ENT(pev), "models/field.dol" );
	pev->movetype = MOVETYPE_NONE;
	UTIL_SetOrigin( pev, pev->origin );

	pev->rendermode = kRenderTransAlpha;
	pev->renderamt = 150;
}

void CRechargeField::Precache( void )
{
	PRECACHE_MODEL( "models/field.dol" );
}

CRechargeField *CRechargeField::Create( Vector vecOrigin, Vector vecAngles )
{
	CRechargeField *pField = GetClassPtr( (CRechargeField *)NULL );

	pField->pev->classname = MAKE_STRING( "item_rechargefield" );
	pField->pev->origin = vecOrigin;
	pField->pev->angles = vecAngles;
	pField->Spawn();

	return pField;
}

//-------------------------------------------------------------
// PS2: carregador novo (o do Decay), com braço que segue o jogador,
// cilindro que gira enquanto carrega e um raio até o braço.
//-------------------------------------------------------------
class CRechargeNew : public CBaseToggle
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
	void SetChargeLight( int iRadius );
	void EXPORT InitThink( void );
	void EXPORT IdleThink( void );

	static	TYPEDESCRIPTION m_SaveData[];

	float	m_flNextCharge;
	float	m_flReactivate;
	int		m_iJuice;
	int		m_iOn;			// 0 = off, 1 = startup, 2 = going
	float	m_flSoundTime;
	float	m_flNextReactivate;
	EHANDLE	m_hField;
	EHANDLE	m_hPlayer;
	CBeam	*m_pBeam;
	BOOL	m_fPlayerIsNear;
	int		m_iIdleState;
	float	m_flIdealArmYaw;
	float	m_flIdealEyeYaw;
	float	m_flCurrentArmYaw;
	float	m_flCurrentEyeYaw;
	float	m_flOffTime;
	BOOL	m_fLightsOn;
	int		m_iUnused;
	float	m_flCylinderRotation;
	BOOL	m_fRestored;	// não vai para o save
};

LINK_ENTITY_TO_CLASS( item_recharge, CRechargeNew );

TYPEDESCRIPTION CRechargeNew::m_SaveData[] =
{
	DEFINE_FIELD( CRechargeNew, m_flNextCharge, FIELD_TIME ),
	DEFINE_FIELD( CRechargeNew, m_flReactivate, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_iJuice, FIELD_INTEGER ),
	DEFINE_FIELD( CRechargeNew, m_iOn, FIELD_INTEGER ),
	DEFINE_FIELD( CRechargeNew, m_flSoundTime, FIELD_TIME ),
	DEFINE_FIELD( CRechargeNew, m_flNextReactivate, FIELD_TIME ),
	DEFINE_FIELD( CRechargeNew, m_hField, FIELD_EHANDLE ),
	DEFINE_FIELD( CRechargeNew, m_fPlayerIsNear, FIELD_BOOLEAN ),
	DEFINE_FIELD( CRechargeNew, m_iIdleState, FIELD_INTEGER ),
	DEFINE_FIELD( CRechargeNew, m_flIdealArmYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_flIdealEyeYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_flCurrentArmYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_flCurrentEyeYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_flOffTime, FIELD_TIME ),
	DEFINE_FIELD( CRechargeNew, m_fLightsOn, FIELD_BOOLEAN ),
	DEFINE_FIELD( CRechargeNew, m_flCylinderRotation, FIELD_FLOAT ),
	DEFINE_FIELD( CRechargeNew, m_pBeam, FIELD_CLASSPTR ),
};

int CRechargeNew::Save( CSave &save )
{
	if ( !CBaseToggle::Save(save) )
		return 0;
	return save.WriteFields( "CRechargeNew", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CRechargeNew::Restore( CRestore &restore )
{
	if ( !CBaseToggle::Restore(restore) )
		return 0;
	m_fRestored = TRUE;
	return restore.ReadFields( "CRechargeNew", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

void CRechargeNew::KeyValue( KeyValueData *pkvd )
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
		m_flReactivate = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CRechargeNew::Spawn( void )
{
	Precache( );

	pev->solid = SOLID_BBOX;
	SET_MODEL( ENT(pev), "models/hev.dol" );

	if ( pev->angles.y < 0 )
		pev->angles.y += 360;

	if ( pev->angles.y == 0 || pev->angles.y == 180 )
		UTIL_SetSize( pev, Vector(-16, -18, 0), Vector(16, 18, 32) );
	else
		UTIL_SetSize( pev, Vector(-18, -16, 0), Vector(18, 16, 32) );

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		pev->view_ofs = Vector( 4, -8, 16 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		pev->view_ofs = Vector( 8, -4, 16 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		pev->view_ofs = Vector( -4, 8, 16 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		pev->view_ofs = Vector( -8, 4, 16 );

	m_hField = CRechargeField::Create( pev->origin, pev->angles );

	m_flNextReactivate = 0;
	m_fPlayerIsNear = FALSE;
	m_iIdleState = 0;
	m_fLightsOn = FALSE;

	pev->movetype = MOVETYPE_PUSH;
	pev->skin = 0;

	pev->sequence = LookupSequence( "rest" );
	pev->frame = 0;
	ResetSequenceInfo();
	SetBoneController( 3, 0 );
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	SetBoneController( 2, 0 );

	m_flOffTime = m_flIdealEyeYaw = m_flCurrentEyeYaw = m_flIdealArmYaw = m_flCurrentArmYaw = 0;

	UTIL_SetOrigin( pev, pev->origin );
	m_iJuice = gSkillData.suitchargerCapacity;

	SetThink( &CRechargeNew::InitThink );
	pev->nextthink = pev->ltime + 0.5f;

	m_flCylinderRotation = 0;

	m_pBeam = CBeam::BeamCreate( "sprites/lgtning.spz", 6 );
	if ( m_pBeam )
	{
		m_pBeam->EntsInit( entindex(), entindex() );
		m_pBeam->SetEndAttachment( 3 );
		m_pBeam->SetStartAttachment( 4 );
		m_pBeam->SetColor( 128, 192, 0 );
		m_pBeam->SetBrightness( 0 );
		m_pBeam->SetNoise( 15 );
	}
}

void CRechargeNew::Precache( void )
{
	PRECACHE_MODEL( "models/hev.dol" );
	UTIL_PrecacheOther( "item_rechargefield" );
	PRECACHE_SOUND( "items/suitcharge1.wav" );
	PRECACHE_SOUND( "items/suitchargeno1.wav" );
	PRECACHE_SOUND( "items/suitchargeok1.wav" );
}

void CRechargeNew::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Make sure that we have a caller
	if ( !pActivator )
		return;
	// if it's not a player, ignore
	if ( !pActivator->IsPlayer() )
		return;

	if ( (pActivator->pev->origin - pev->origin).Length2D() > 48 )
		return;

	if ( ((CBasePlayer *)pActivator)->m_afPhysicsFlags & 0x40 )
		return;

	// if the player doesn't have the suit, or there is no juice left, make the deny noise
	if ( (m_iJuice <= 0) || (!(pActivator->pev->weapons & (1<<WEAPON_SUIT))) )
	{
		if ( m_flSoundTime <= gpGlobals->time )
		{
			m_flSoundTime = gpGlobals->time + 0.62f;
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/suitchargeno1.wav", 0.85f, ATTN_NORM );
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
		EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85f, ATTN_NORM );
		m_flSoundTime = gpGlobals->time + 0.56f;
		pev->sequence = LookupSequence( "give_charge" );
		pev->frame = 0;
		ResetSequenceInfo();
		m_pBeam->SetBrightness( 196 );
	}
	if ( (m_iOn == 1) && (m_flSoundTime <= gpGlobals->time) )
	{
		m_iOn++;
		EMIT_SOUND( ENT(pev), CHAN_STATIC, "items/suitcharge1.wav", 0.85f, ATTN_NORM );
	}

	// charge the player
	if ( pActivator->pev->armorvalue < 100 )
	{
		m_iJuice--;
		pActivator->pev->armorvalue += 1;

		if ( pActivator->pev->armorvalue > 100 )
			pActivator->pev->armorvalue = 100;

		// if there is no juice left, turn it off
		if ( m_iJuice <= 0 )
		{
			pev->skin = 1;
			Off();
		}
	}

	// govern the rate of charge
	m_flNextCharge = gpGlobals->time + 0.1f;
}

void CRechargeNew::Recharge( void )
{
	m_iJuice = gSkillData.suitchargerCapacity;
	m_flNextReactivate = 0;
}

void CRechargeNew::Off( void )
{
	// Stop looping sound.
	if ( m_iOn > 1 )
		STOP_SOUND( ENT(pev), CHAN_STATIC, "items/suitcharge1.wav" );

	m_iOn = 0;

	pev->sequence = LookupSequence( "retract_charge" );
	pev->frame = 0;
	ResetSequenceInfo();

	if ( (!m_iJuice) && ( ( m_flReactivate = g_pGameRules->FlHEVChargerRechargeTime() ) > 0 ) )
		m_flNextReactivate = gpGlobals->time + m_flReactivate;

	if ( !m_iJuice )
		m_pBeam->SetBrightness( 0 );
	else
		m_pBeam->SetBrightness( 96 );

	m_flOffTime = 0;
}

BOOL CRechargeNew::FindPlayer( void )
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

float CRechargeNew::VecToYaw( Vector vecDir )
{
	if ( vecDir.x == 0 && vecDir.y == 0 && vecDir.z == 0 )
		return pev->angles.y;

	return UTIL_VecToYaw( vecDir );
}

void CRechargeNew::SetIdealYaws( void )
{
	float flYaw;
	Vector vecSrc;

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		vecSrc = pev->origin + Vector( 4, 0, 16 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		vecSrc = pev->origin + Vector( 0, -4, 16 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		vecSrc = pev->origin + Vector( -4, 0, 16 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		vecSrc = pev->origin + Vector( 0, 4, 16 );

	flYaw = VecToYaw( m_hPlayer->pev->origin - vecSrc ) - pev->angles.y;

	if ( flYaw > 180 )
		flYaw -= 360;
	if ( flYaw < -180 )
		flYaw += 360;

	m_flIdealArmYaw = flYaw;

	if ( ( pev->angles.y <= 45 && pev->angles.y >= 0 ) || ( pev->angles.y > 315 && pev->angles.y <= 360 ) )
		vecSrc = pev->origin + Vector( 4, -8, 16 );
	else if ( pev->angles.y <= 135 && pev->angles.y > 45 )
		vecSrc = pev->origin + Vector( 8, -4, 16 );
	else if ( pev->angles.y <= 225 && pev->angles.y > 135 )
		vecSrc = pev->origin + Vector( -4, 8, 16 );
	else if ( pev->angles.y <= 315 && pev->angles.y > 225 )
		vecSrc = pev->origin + Vector( -8, 4, 16 );

	flYaw = VecToYaw( m_hPlayer->pev->origin - vecSrc ) - pev->angles.y;

	if ( flYaw > 180 )
		flYaw -= 360;
	if ( flYaw < -180 )
		flYaw += 360;

	m_flIdealEyeYaw = flYaw;
}

void CRechargeNew::SetLights( BOOL fOn )
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

	m_fLightsOn = fOn;
}

/* PS2: luz amarela do cilindro; não é chamada em lugar nenhum */
void CRechargeNew::SetChargeLight( int iRadius )
{
	BOOL fOff = !iRadius;

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( entindex() + 0x3000 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( iRadius );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 192 );
		WRITE_BYTE( 0 );
		WRITE_BYTE( fOff );
		WRITE_COORD( 0 );
	MESSAGE_END();
}

void CRechargeNew::InitThink( void )
{
	SetThink( &CRechargeNew::IdleThink );
	pev->nextthink = pev->ltime + 0.1f;
}

void CRechargeNew::IdleThink( void )
{
	if ( m_fRestored )
	{
		m_pBeam->SetEndAttachment( 3 );
		m_pBeam->SetStartAttachment( 4 );
		if ( m_fLightsOn )
			m_fLightsOn = FALSE;
		m_fRestored = FALSE;
	}

	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished )
	{
		if ( m_fPlayerIsNear && m_iIdleState == 2 && m_iOn )
		{
			pev->sequence = LookupSequence( "charge_idle" );
			pev->frame = 0;
			ResetSequenceInfo();
		}
		else if ( m_fPlayerIsNear && m_iIdleState != 2 && !m_iOn )
		{
			pev->sequence = LookupSequence( "prep_charge" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iIdleState = 2;
		}
		else if ( !m_fPlayerIsNear && m_iIdleState != 1 && !m_iOn )
		{
			pev->sequence = LookupSequence( "rest" );
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
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "items/suitchargeok1.wav", 0.85f, ATTN_NORM );
			m_flSoundTime = gpGlobals->time + 0.56f;
			m_pBeam->SetBrightness( 96 );
		}

		SetIdealYaws();

		if ( m_flIdealArmYaw != m_flCurrentArmYaw )
		{
			if ( m_flIdealArmYaw < m_flCurrentArmYaw )
				m_flCurrentArmYaw = m_flCurrentArmYaw - min( m_flCurrentArmYaw - m_flIdealArmYaw, 5 );
			else
				m_flCurrentArmYaw = m_flCurrentArmYaw + min( m_flIdealArmYaw - m_flCurrentArmYaw, 5 );
			m_flCurrentArmYaw = m_flIdealArmYaw;
			SetBoneController( 3, m_flCurrentArmYaw );
		}

		if ( m_flIdealEyeYaw != m_flCurrentEyeYaw )
		{
			if ( m_flIdealEyeYaw < m_flCurrentEyeYaw )
				m_flCurrentEyeYaw = m_flCurrentEyeYaw - min( m_flCurrentEyeYaw - m_flIdealEyeYaw, 5 );
			else
				m_flCurrentEyeYaw = m_flCurrentEyeYaw + min( m_flIdealEyeYaw - m_flCurrentEyeYaw, 5 );
			m_flCurrentEyeYaw = m_flIdealEyeYaw;
			SetBoneController( 0, m_flCurrentEyeYaw );
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
		m_pBeam->pev->renderamt = flYaw;
	}

	if ( m_iOn )
	{
		if ( m_flCylinderRotation >= 360 )
			m_flCylinderRotation = 0;
		else
			m_flCylinderRotation += 10;
		SetBoneController( 1, m_flCylinderRotation );
		SetBoneController( 2, 360 - m_flCylinderRotation );
	}

	pev->nextthink = pev->ltime + 0.1f;
}

/* registro dos ponteiros de Think para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_0017E008(void)
{
	func_002397D0("131", static_cast<BASEPTR>(&CRecharge::SUB_DoNothing));
	func_002397D0("132", static_cast<BASEPTR>(&CRecharge::Recharge));
	func_002397D0("133", static_cast<BASEPTR>(&CRechargeNew::IdleThink));
	func_002397D0("134", static_cast<BASEPTR>(&CRechargeNew::InitThink));
	func_002397D0("135", static_cast<BASEPTR>(&CRecharge::Off));
}
