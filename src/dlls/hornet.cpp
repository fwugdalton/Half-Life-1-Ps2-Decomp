/* dlls/hornet.cpp: o marimbondo (hornet) do HLSDK, projétil do hivehand e do
   alien grunt. Versão antiga do SDK: o som do disparo sai no Spawn. */
//=========================================================
// Hornets
//=========================================================

typedef struct _FILE FILE;
#define DLLS_MONSTERS_CPP
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/soundent.h"
#include "dlls/hornet.h"
#include "dlls/gamerules.h"
#include "dlls/skill.h"
#include "include_asm.h"

extern "C" int cl_numlocal;

#define TE_SPRITE		17
#define TE_BEAMFOLLOW		22


LINK_ENTITY_TO_CLASS( hornet, CHornet );

//=========================================================
// Save/Restore
//=========================================================
TYPEDESCRIPTION	CHornet::m_SaveData[] = 
{
	DEFINE_FIELD( CHornet, m_flStopAttack, FIELD_TIME ),
	DEFINE_FIELD( CHornet, m_iHornetType, FIELD_INTEGER ),
	DEFINE_FIELD( CHornet, m_flFlySpeed, FIELD_FLOAT ),
};

int iHornetPuff = 0;
int iHornetTrail = 0;

IMPLEMENT_SAVERESTORE( CHornet, CBaseMonster );

//=========================================================
// don't let hornets gib, ever.
//=========================================================
int CHornet :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// filter these bits a little.
	bitsDamageType &= ~ ( DMG_ALWAYSGIB );
	bitsDamageType |= DMG_NEVERGIB;

	return CBaseMonster :: TakeDamage ( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

//=========================================================
//=========================================================
void CHornet :: Spawn( void )
{
	Precache();

	pev->movetype	= MOVETYPE_FLY;
	pev->solid		= SOLID_BBOX;
	pev->takedamage = DAMAGE_YES;
	pev->flags		|= FL_MONSTER;
	pev->health		= 1;// weak!
	
	if ( g_pGameRules->Ps2Virt07() )
	{
		// hornets don't live as long in multiplayer
		m_flStopAttack = gpGlobals->time + 3.5f;
	}
	else
	{
		m_flStopAttack	= gpGlobals->time + 5.0f;
	}

	m_flFieldOfView = 0.9f; // +- 25 degrees

	if ( RANDOM_LONG ( 1, 5 ) <= 2 )
	{
		m_iHornetType = HORNET_TYPE_RED;
		m_flFlySpeed = HORNET_RED_SPEED;
	}
	else
	{
		m_iHornetType = HORNET_TYPE_ORANGE;
		m_flFlySpeed = HORNET_ORANGE_SPEED;
	}

	SET_MODEL(ENT( pev ), "models/hornet.dol");
	UTIL_SetSize( pev, Vector( -4, -4, -4 ), Vector( 4, 4, 4 ) );

	SetTouch( &CHornet::DieTouch );
	SetThink( &CHornet::StartTrack );

	edict_t *pSoundEnt = pev->owner;
	if ( !pSoundEnt )
		pSoundEnt = edict();

	switch (RANDOM_LONG(0,2))
	{
		case 0:	EMIT_SOUND( pSoundEnt, CHAN_WEAPON, "agrunt/ag_fire1.wav", 1, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( pSoundEnt, CHAN_WEAPON, "agrunt/ag_fire2.wav", 1, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( pSoundEnt, CHAN_WEAPON, "agrunt/ag_fire3.wav", 1, ATTN_NORM);	break;
	}

	if ( !FNullEnt(pev->owner) && (pev->owner->v.flags & FL_CLIENT) )
	{
		pev->dmg = gSkillData.plrDmgHornet;
	}
	else
	{
		// no real owner, or owner isn't a client. 
		pev->dmg = gSkillData.monDmgHornet;
	}
	
	pev->nextthink = gpGlobals->time + 0.1f;
	ResetSequenceInfo( );
}


void CHornet :: Precache()
{
	PRECACHE_MODEL("models/hornet.dol");

	PRECACHE_SOUND( "agrunt/ag_fire1.wav" );
	PRECACHE_SOUND( "agrunt/ag_fire2.wav" );
	PRECACHE_SOUND( "agrunt/ag_fire3.wav" );

	PRECACHE_SOUND( "hornet/ag_buzz1.wav" );
	PRECACHE_SOUND( "hornet/ag_buzz2.wav" );
	PRECACHE_SOUND( "hornet/ag_buzz3.wav" );

	PRECACHE_SOUND( "hornet/ag_hornethit1.wav" );
	PRECACHE_SOUND( "hornet/ag_hornethit2.wav" );
	PRECACHE_SOUND( "hornet/ag_hornethit3.wav" );

	iHornetPuff = PRECACHE_MODEL( "sprites/muz1.spz" );
	iHornetTrail = PRECACHE_MODEL("sprites/laserbeam.spz");
}	

//=========================================================
// hornets will never get mad at each other, no matter who the owner is.
//=========================================================
int CHornet::IRelationship ( CBaseEntity *pTarget )
{
	if ( pTarget->pev->modelindex == pev->modelindex )
	{
		return R_NO;
	}

	return CBaseMonster :: IRelationship( pTarget );
}

//=========================================================
// ID's Hornet as their owner
//=========================================================
int CHornet::Classify ( void )
{

	if ( pev->owner && pev->owner->v.flags & FL_CLIENT)
	{
		return CLASS_PLAYER_BIOWEAPON;
	}

	return	CLASS_ALIEN_BIOWEAPON;
}

//=========================================================
// StartTrack - starts a hornet out tracking its target
//=========================================================
void CHornet :: StartTrack ( void )
{
	IgniteTrail();

	SetTouch( &CHornet::TrackTouch );
	SetThink( &CHornet::TrackTarget );

	pev->nextthink = gpGlobals->time + 0.1f;
}

//=========================================================
// StartDart - starts a hornet out just flying straight.
//=========================================================
void CHornet :: StartDart ( void )
{
	IgniteTrail();

	SetTouch( &CHornet::DartTouch );

	SetThink( &CHornet::SUB_Remove );
	pev->nextthink = gpGlobals->time + 4;
}

void CHornet::IgniteTrail( void ) 
{
/*

  ted's suggested trail colors:

r161
g25
b97

r173
g39
b14

old colors
		case HORNET_TYPE_RED:
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			break;
		case HORNET_TYPE_ORANGE:
			WRITE_BYTE( 0   );   // r, g, b
			WRITE_BYTE( 100 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			break;
	
*/

	// trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE(  TE_BEAMFOLLOW );
		WRITE_SHORT( entindex() );	// entity
		WRITE_SHORT( iHornetTrail );	// model
		WRITE_BYTE( 3 ); // life
		WRITE_BYTE( 2 );  // width
		
		switch ( m_iHornetType )
		{
		case HORNET_TYPE_RED:
			WRITE_BYTE( 179 );   // r, g, b
			WRITE_BYTE( 39 );   // r, g, b
			WRITE_BYTE( 14 );   // r, g, b
			break;
		case HORNET_TYPE_ORANGE:
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 128 );   // r, g, b
			WRITE_BYTE( 0 );   // r, g, b
			break;
		}

		WRITE_BYTE( 128 );	// brightness

	MESSAGE_END();
}

//=========================================================
// Hornet is flying, gently tracking target
//=========================================================
void CHornet :: TrackTarget ( void )
{
	Vector	vecFlightDir;
	Vector	vecDirToEnemy;
	float	flDelta;

	StudioFrameAdvance( );

	if (gpGlobals->time > m_flStopAttack)
	{
		SetTouch( NULL );
		SetThink( &CHornet::SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1f;
		return;
	}

	// UNDONE: The player pointer should come back after returning from another level
	if ( m_hEnemy == NULL )
	{// enemy is dead.
		Look( 512 );
		m_hEnemy = BestVisibleEnemy( );
	}
	
	if ( m_hEnemy != NULL && FVisible( m_hEnemy ))
	{
		m_vecEnemyLKP = m_hEnemy->BodyTarget( pev->origin );
	}
	else
	{
		m_vecEnemyLKP = m_vecEnemyLKP + pev->velocity * m_flFlySpeed * 0.1f;
	}

	vecDirToEnemy = ( m_vecEnemyLKP - pev->origin ).Normalize();

	if (pev->velocity.Length() < 0.1f)
		vecFlightDir = vecDirToEnemy;
	else 
		vecFlightDir = pev->velocity.Normalize();

	// measure how far the turn is, the wider the turn, the slow we'll go this time.
	flDelta = DotProduct ( vecFlightDir, vecDirToEnemy );
	
	if ( flDelta < 0.5f )
	{// hafta turn wide again. play sound
		switch (RANDOM_LONG(0,2))
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz1.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz2.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz3.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
		}
	}

	if ( flDelta <= 0 && m_iHornetType == HORNET_TYPE_RED )
	{// no flying backwards, but we don't want to invert this, cause we'd go fast when we have to turn REAL far.
		flDelta = 0.25f;
	}

	pev->velocity = ( vecFlightDir + vecDirToEnemy).Normalize();

	if ( pev->owner && (pev->owner->v.flags & FL_MONSTER) )
	{
		// random pattern only applies to hornets fired by monsters, not players. 

		pev->velocity.x += RANDOM_FLOAT ( -0.10f, 0.10f );// scramble the flight dir a bit.
		pev->velocity.y += RANDOM_FLOAT ( -0.10f, 0.10f );
		pev->velocity.z += RANDOM_FLOAT ( -0.10f, 0.10f );
	}
	
	switch ( m_iHornetType )
	{
		case HORNET_TYPE_RED:
			pev->velocity = pev->velocity * ( m_flFlySpeed * flDelta );// scale the dir by the ( speed * width of turn )
			pev->nextthink = gpGlobals->time + RANDOM_FLOAT( 0.1f, 0.3f );
			break;
		case HORNET_TYPE_ORANGE:
			pev->velocity = pev->velocity * m_flFlySpeed;// do not have to slow down to turn.
			pev->nextthink = gpGlobals->time + 0.1f;// fixed think time
			break;
	}

	pev->angles = UTIL_VecToAngles (pev->velocity);

	pev->solid = SOLID_BBOX;

	// if hornet is close to the enemy, jet in a straight line for a half second.
	// (only in the single player game)
	if ( m_hEnemy != NULL && !g_pGameRules->Ps2Virt07() )
	{
		if ( flDelta >= 0.4f && ( pev->origin - m_vecEnemyLKP ).Length() <= 300 )
		{
			if ( cl_numlocal == 1 )
			{
				MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
					WRITE_BYTE( TE_SPRITE );
					WRITE_COORD( pev->origin.x);	// pos
					WRITE_COORD( pev->origin.y);
					WRITE_COORD( pev->origin.z);
					WRITE_SHORT( iHornetPuff );		// model
					// WRITE_BYTE( 0 );				// life * 10
					WRITE_BYTE( 2 );				// size * 10
					WRITE_BYTE( 128 );			// brightness
				MESSAGE_END();
			}

			switch (RANDOM_LONG(0,2))
			{
			case 0:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz1.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
			case 1:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz2.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
			case 2:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_buzz3.wav", HORNET_BUZZ_VOLUME, ATTN_NORM);	break;
			}
			pev->velocity = pev->velocity * 2;
			pev->nextthink = gpGlobals->time + 1.0f;
			// don't attack again
			m_flStopAttack = gpGlobals->time;
		}
	}
}

//=========================================================
// Tracking Hornet hit something
//=========================================================
void CHornet :: TrackTouch ( CBaseEntity *pOther )
{
	if ( pOther->edict() == pev->owner || pOther->pev->modelindex == pev->modelindex )
	{// bumped into the guy that shot it.
		pev->solid = SOLID_NOT;
		return;
	}

	if ( IRelationship( pOther ) <= R_NO )
	{
		// hit something we don't want to hurt, so turn around.

		pev->velocity = pev->velocity.Normalize();

		pev->velocity.x *= -1;
		pev->velocity.y *= -1;

		pev->origin = pev->origin + pev->velocity * 4; // bounce the hornet off a bit.
		pev->velocity = pev->velocity * m_flFlySpeed;

		return;
	}

	DieTouch( pOther );
}

void CHornet::DartTouch( CBaseEntity *pOther )
{
	DieTouch( pOther );
}

void CHornet::DieTouch ( CBaseEntity *pOther )
{
	if ( pOther && pOther->pev->takedamage )
	{// do the damage

		switch (RANDOM_LONG(0,2))
		{// buzz when you plug someone
			case 0:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit1.wav", 1, ATTN_NORM);	break;
			case 1:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit2.wav", 1, ATTN_NORM);	break;
			case 2:	EMIT_SOUND( ENT(pev), CHAN_VOICE, "hornet/ag_hornethit3.wav", 1, ATTN_NORM);	break;
		}
			
		pOther->TakeDamage( pev, VARS( pev->owner ), pev->dmg, DMG_BULLET );
	}

	pev->modelindex = 0;// so will disappear for the 0.1 secs we wait until NEXTTHINK gets rid
	pev->solid = SOLID_NOT;
	pev->velocity = g_vecZero;
	SetTouch( NULL );

	SetThink ( &CHornet::SUB_Remove );
	pev->nextthink = gpGlobals->time + 1;// stick around long enough for the sound to finish!
}


/* registro dos ponteiros de Think e Touch para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);

extern "C" void func_0018EBC8(void)
{
	func_002397D0("157", static_cast<BASEPTR>(&CHornet::SUB_Remove));
	func_002397D0("158", static_cast<BASEPTR>(&CHornet::StartTrack));
	func_002397D0("159", static_cast<BASEPTR>(&CHornet::TrackTarget));
	func_002397F0("160", static_cast<ENTITYFUNCPTR>(&CHornet::DartTouch));
	func_002397F0("161", static_cast<ENTITYFUNCPTR>(&CHornet::DieTouch));
	func_002397F0("162", static_cast<ENTITYFUNCPTR>(&CHornet::TrackTouch));
}
