/* dlls/aiplayer.cpp (nome inventado): o parceiro controlado pela IA do co-op do
   Decay (PS2). Não há fonte; o código segue o HPB bot (botman), adaptado como métodos
   do CBasePlayer (BotSpawnInit, BotThink, BotChangeYaw...). */

typedef struct _FILE FILE;
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "dlls/soundent.h"
#include "dlls/monsters.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define WEAPON_CROWBAR		1
#define WEAPON_GLOCK		2
#define WEAPON_PYTHON		3
#define WEAPON_MP5			4
#define WEAPON_CHAINGUN		5
#define WEAPON_CROSSBOW		6
#define WEAPON_SHOTGUN		7
#define WEAPON_RPG			8
#define WEAPON_GAUSS		9
#define WEAPON_EGON			10
#define WEAPON_HORNETGUN	11
#define WEAPON_HANDGRENADE	12
#define WEAPON_TRIPMINE		13
#define WEAPON_SATCHEL		14
#define WEAPON_SNARK		15
#define WEAPON_SUIT			31

#ifndef IN_JUMP
#define IN_JUMP		(1 << 1)
#define IN_DUCK		(1 << 2)
#define IN_FORWARD	(1 << 3)
#define IN_BACK		(1 << 4)
#define IN_USE		(1 << 5)
#endif

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;
/* parâmetros de cada arma para a IA (índice = m_ps2AIType, que é o id da arma) */
typedef struct
{
	int iPrimaryMinDist;		/* 0x00 */
	int iPrimaryMaxDist;		/* 0x04 */
	float flPrimaryAim[3];		/* 0x08: desvio da mira por mp_aiskill */
	float flPrimaryDelay;		/* 0x14 */
	float flMinEnemyHealth;		/* 0x18: só usa a arma em inimigos com max_health maior */
	BOOL fSecondary;			/* 0x1C */
	int iSecondaryMinDist;		/* 0x20 */
	int iSecondaryMaxDist;		/* 0x24 */
	float flSecondaryAim[3];	/* 0x28 */
	int iSecondaryPercent;		/* 0x34 */
	float flSecondaryDelay;		/* 0x38 */
	float flSecondaryCharge;	/* 0x3C */
	int iPriority;				/* 0x40: prioridade na escolha da arma */
} ai_weapon_t;

extern ai_weapon_t g_AIWeapons[];

/* munição que a IA pega: classname, nome da munição e máximo */
typedef struct
{
	char *pszClassname;
	char *pszAmmoName;
	int iMaxCarry;
} ai_ammo_t;

extern ai_ammo_t g_AIAmmo[];
void UTIL_TraceLineNoEnts( const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr );
Vector VecBModelOrigin( entvars_t *pevBModel );
int UTIL_PointContents( const Vector &vec );
Vector VecCheckToss( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flGravityAdj );
Vector VecCheckThrow( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj );

extern ps2cvar_t coopslave;
extern ps2cvar_t aiskill;
int SENTENCEG_PlayRndSz(edict_t *entity, const char *szrootname, float volume, float attenuation, int flags, int pitch);

/* munição que a IA procura (0x30D8D0) */
ai_ammo_t g_AIAmmo[] =
{
	{ "ammo_glockclip", "9mm", 250 },
	{ "ammo_9mmclip", "9mm", 250 },
	{ "ammo_9mmAR", "9mm", 250 },
	{ "ammo_9mmbox", "9mm", 250 },
	{ "ammo_mp5clip", "9mm", 250 },
	{ "ammo_chainboxclip", "9mm", 250 },
	{ "ammo_357", "357", 36 },
	{ "ammo_buckshot", "buckshot", 125 },
	{ "ammo_mp5grenades", "ARgrenades", 10 },
	{ "ammo_ARgrenades", "ARgrenades", 10 },
	{ "ammo_crossbow", "bolts", 50 },
	{ "ammo_rpgclip", "rockets", 5 },
	{ "ammo_egonclip", "uranium", 100 },
	{ "ammo_gaussclip", "uranium", 100 },
	{ NULL, NULL, 0 },
};

/* parâmetros das armas para a IA, pelo id da arma (0x30D988) */
ai_weapon_t g_AIWeapons[] =
{
	{ 0, 64, { 0.0f, 0.0f, 0.0f }, 0.1f, 0.0f, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 0, 0.1f, 0.0f, -1 },
	{ 0, 64, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 0, 0, 0, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 2 },
	{ 65, 2048, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 1, 65, 512, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 4 },
	{ 256, 4096, { 8.0f, 12.0f, 18.0f }, 0.1f, 21.0f, 1, 65, 1024, { 8.0f, 12.0f, 18.0f }, 40, 0.1f, 21.0f, 6 },
	{ 65, 2048, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 1, 384, 1152, { 10.0f, 15.0f, 25.0f }, 0, 4.0f, 100.0f, 5 },
	{ 0, 0, { 0.0f, 0.0f, 0.0f }, 0.1f, 0.0f, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 0, 0.1f, 0.0f, -1 },
	{ 512, 4096, { 8.0f, 12.0f, 18.0f }, 0.1f, 0.0f, 1, 512, 4096, { 8.0f, 12.0f, 18.0f }, 20, 0.1f, 0.0f, 8 },
	{ 65, 384, { 10.0f, 15.0f, 25.0f }, 0.8f, 0.0f, 1, 65, 192, { 10.0f, 15.0f, 25.0f }, 0, 1.0f, 0.0f, 7 },
	{ 512, 4096, { 4.0f, 6.0f, 10.0f }, 5.0f, 100.0f, 1, 512, 4096, { 4.0f, 6.0f, 10.0f }, 0, 5.0f, 100.0f, 13 },
	{ 128, 2048, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 1, 256, 1024, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 11 },
	{ 65, 768, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 0, 0, 0, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 10 },
	{ 65, 4096, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 0, 65, 2048, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 9 },
	{ 0, 0, { 10.0f, 15.0f, 25.0f }, 5.0f, 0.0f, 0, 0, 0, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, -1 },
	{ 0, 0, { 0.0f, 0.0f, 0.0f }, 0.1f, 0.0f, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 0, 0.1f, 0.0f, -1 },
	{ 0, 0, { 0.0f, 0.0f, 0.0f }, 0.1f, 0.0f, 0, 0, 0, { 0.0f, 0.0f, 0.0f }, 0, 0.1f, 0.0f, -1 },
	{ 256, 4096, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 0, 0, 0, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 3 },
	{ 0, 64, { 10.0f, 15.0f, 25.0f }, 0.1f, 0.0f, 1, 192, 4096, { 10.0f, 15.0f, 25.0f }, 0, 0.1f, 0.0f, 0 },
};

/* BotSpawnInit */
void CBasePlayer :: Ps2AISpawnInit( void )
{
	pev->flags = FL_CLIENT | FL_FAKECLIENT;
	pev->ideal_yaw = pev->v_angle.y = pev->angles.y;
	pev->yaw_speed = 20;

	m_iAIUnk728 = 0;
	m_vecAIPrevOrigin = pev->origin;
	m_flAIUnk700 = 0;
	m_flAIMaxSpeed = CVAR_GET_FLOAT( "sv_maxspeed" );
	m_flAILadderDir = 0;

	if ( RANDOM_LONG( 1, 100 ) <= 50 )
		m_flAIWanderDir = 1;
	else
		m_flAIWanderDir = 2;

	m_iAIUnk72C = 0;
	m_iAIState = 0;
	m_flAIUseHealthTime = 0;
	m_flAIUseHEVTime = 0;
	m_flAIWallOnRight = 0;
	m_flAIWallOnLeft = 0;
	m_flAIDontAvoidWallTime = 0;
	m_flAIUnk740 = -1;
	m_hEnemy = NULL;
	m_hAIUser = NULL;
	m_hAIPickupItem = NULL;
	m_flAINavigateTime = gpGlobals->time;
	m_bAISkill = (int)aiskill.value;

	SetUse( &CBasePlayer::Ps2AIUse );
}

int CBasePlayer :: ISoundMask( void )
{
	return bits_SOUND_WORLD | bits_SOUND_COMBAT | bits_SOUND_DANGER;
}

/* BotThink */
void CBasePlayer :: Ps2AIThink( void )
{
	Vector vecMoved;

	pev->button = 0;

	if ( ( g_pGameRules->Ps2Virt07() && !g_pGameRules->IsMultiplayer() && pev->health < 1.0f ) || pev->deadflag != DEAD_NO )
	{
		if ( RANDOM_LONG( 1, 100 ) > 50 )
			pev->button = IN_ATTACK;

		(*g_engfuncs.pfnRunPlayerMove)( edict(), pev->v_angle, 0, 0, 0, pev->button, 0, (byte)( gpGlobals->frametime * 1000 ) );
		return;
	}

	vecMoved = m_vecAIPrevOrigin - pev->origin;
	m_vecAIPrevOrigin = pev->origin;
	m_flAIMoveSpeed = 0;

	float flYaw = Ps2AIChangeYaw( pev->yaw_speed );

	if ( (CBaseEntity *)m_hEnemy == NULL )
	{
		if ( m_iAIFlags & 0x200 )
			m_iAIFlags &= ~0x200;
	}
	else if ( !( m_iAIFlags & 0x200 ) )
	{
		m_iAIFlags |= 0x1200;
		m_flAIUnk754 = 0;

		if ( m_flAIUnk754 == coopslave.value && m_flAINextAttackSentence < gpGlobals->time )
		{
			if ( m_iTeamNum == 1 )
				PlaySentence( "GINA_ATTACK", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
			else
				PlaySentence( "COL_ATTACK", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );

			m_flAINextAttackSentence = gpGlobals->time + 10.0f;
		}
	}

	if ( !m_iAIUnk6FC && (CBaseEntity *)m_hEnemy != NULL && ((CBaseEntity *)m_hEnemy)->IsAlive() )
		Ps2AIShootAtEnemy( flYaw );

	if ( !pev->deadflag )
	{
		pev->angles.z = pev->v_angle.z = 0;
		pev->angles.x = 0;
	}

	pev->punchangle = g_vecZero;

	(*g_engfuncs.pfnRunPlayerMove)( edict(), pev->v_angle, m_flAIMoveSpeed, 0, 0, pev->button, 0, (byte)( gpGlobals->frametime * 1000 ) );
}

void CBasePlayer :: Ps2AIUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
}

/* BotChangeYaw */
float CBasePlayer :: Ps2AIChangeYaw( float speed )
{
	float ideal, current, diff;

	current = pev->v_angle.y;
	ideal = pev->ideal_yaw;

	diff = fabsf( current - ideal );
	if ( diff > 180 )
		diff = 360 - diff;

	if ( diff < 1 )
	{
		pev->v_angle.y = ideal;
		pev->angles.x = 0;
		pev->angles.y = ideal;
		float &flRoll = pev->angles.z;
		flRoll = 0;
		return flRoll;
	}

	if ( diff < speed )
		speed = diff;

	if ( current >= 0 && ideal >= 0 )
	{
		if ( current > ideal )
			current -= speed;
		else
			current += speed;
	}
	else if ( current >= 0 && ideal < 0 )
	{
		if ( current - 180 > ideal )
			current += speed;
		else
			current -= speed;
	}
	else if ( current < 0 && ideal >= 0 )
	{
		if ( current + 180 > ideal )
			current += speed;
		else
			current -= speed;
	}
	else
	{
		if ( current > ideal )
			current -= speed;
		else
			current += speed;
	}

	if ( current > 180 )
		current -= 360;
	if ( current < -180 )
		current += 360;

	pev->v_angle.y = current;
	pev->angles.x = 0;
	pev->angles.y = current;
	pev->angles.z = 0;
	return speed;
}

/* BotOnLadder (simplificado): vira para a escada mais próxima e sobe ou desce */
void CBasePlayer :: Ps2AIOnLadder( void )
{
	if ( m_flAILadderDir == 0 )
	{
		edict_t *pent = NULL;
		edict_t *pNearest = NULL;
		float flMinDist = 8192;

		while ( !FNullEnt( pent = FIND_ENTITY_BY_CLASSNAME( pent, "func_ladder" ) ) )
		{
			float flDist = ( pev->origin - pent->v.origin ).Length2D();

			if ( flDist < flMinDist )
			{
				flMinDist = flDist;
				pNearest = pent;
			}
		}

		if ( pNearest )
			m_flAILadderYaw = pNearest->v.angles.y;
		else
			m_flAILadderYaw = pev->v_angle.y;
	}

	pev->v_angle.y = pev->ideal_yaw = m_flAILadderYaw;

	if ( m_flAILadderDir == 1 )
		pev->v_angle.x = -60;
	else if ( m_flAILadderDir == 2 )
		pev->v_angle.x = 60;
	else
	{
		pev->v_angle.x = -60;
		m_flAILadderDir = 1;
	}

	pev->button |= IN_FORWARD;
	m_iAIState &= ~0x10;
}

/* BotShootAtEnemy: mira no inimigo e atira (devolve TRUE se mirou) */
BOOL CBasePlayer :: Ps2AIShootAtEnemy( float flMoved )
{
	Vector vecDir;
	Vector vecAngles;

	if ( !m_pActiveItem )
		return FALSE;

	if ( CBasePlayerItem::ItemInfoArray[ m_pActiveItem->m_iId ].iMaxClip != WEAPON_NOCLIP )
	{
		if ( ((CBasePlayerWeapon *)m_pActiveItem)->m_iClip <= 0 )
		{
			if ( m_rgAmmo[ m_pActiveItem->PrimaryAmmoIndex() ] > 0 )
			{
				if ( m_ps2AIType == 8 )
				{
					m_vecAITarget = Ps2AIBodyTarget( m_hEnemy );
					vecDir = m_vecAITarget - GetGunPosition();
					vecAngles = UTIL_VecToAngles( vecDir );

					pev->v_angle.x = 360 - vecAngles.x;
					pev->angles.x = 0;
					pev->angles.y = pev->v_angle.y;
					pev->angles.z = 0;
					pev->ideal_yaw = vecAngles.y;

					if ( pev->ideal_yaw > 180 )
						pev->ideal_yaw -= 360;
					if ( pev->ideal_yaw < -180 )
						pev->ideal_yaw += 360;

					return TRUE;
				}

				m_iAIState |= 0x20;
				return FALSE;
			}

			Ps2AIChangeWeapon();
		}
		else
		{
			if ( m_iAIState & 0x20 )
				m_iAIState &= ~0x10;
			m_iAIState &= ~0x20;
		}
	}
	else if ( m_rgAmmo[ m_pActiveItem->PrimaryAmmoIndex() ] <= 0 )
	{
		Ps2AIChangeWeapon();
	}

	if ( m_iAIState & 0x400 )
		return FALSE;

	vecDir = ((CBaseEntity *)m_hEnemy)->pev->origin + ((CBaseEntity *)m_hEnemy)->pev->view_ofs;
	if ( !FVisible( vecDir ) )
	{
		if ( m_flAIUnk754 == 0 )
			m_flAIUnk754 = gpGlobals->time;

		if ( m_flAIUnk754 + 2.0f < gpGlobals->time )
		{
			m_hEnemy = NULL;
			m_flAIUnk754 = 0;
		}
		else
		{
			m_flAIMoveSpeed = 0;
			m_iAIState &= ~0x10;
		}
		return FALSE;
	}

	m_flAIUnk754 = 0;

	if ( m_ps2AIType != 12 || m_flAIUnk700 <= gpGlobals->time )
		m_vecAITarget = Ps2AIBodyTarget( m_hEnemy );

	vecDir = m_vecAITarget - GetGunPosition();
	vecAngles = UTIL_VecToAngles( vecDir );

	pev->v_angle.x = 360 - vecAngles.x;
	pev->angles.x = 0;
	pev->angles.y = pev->v_angle.y;
	pev->angles.z = 0;
	pev->ideal_yaw = vecAngles.y;

	if ( pev->ideal_yaw > 180 )
		pev->ideal_yaw -= 360;
	if ( pev->ideal_yaw < -180 )
		pev->ideal_yaw += 360;

	if ( m_flAIUnk700 <= gpGlobals->time )
	{
		float flDiff = fabsf( pev->v_angle.y - pev->ideal_yaw );
		if ( flDiff > 180 )
			flDiff = 360 - flDiff;

		if ( !( m_iAIState & 0x1000 ) && ( flDiff < 1.0f || gpGlobals->time < m_flAIUnk740 ) )
		{
			Ps2AIFireWeapon( m_vecAITarget, vecDir, m_ps2AIType, TRUE );
			m_iAIState &= ~0x2000;
			return TRUE;
		}
	}

	m_iAIState &= ~0x1000;
	return TRUE;
}

/* BotBodyTarget: ponto do inimigo com um desvio que depende da distância e da arma */
Vector CBasePlayer :: Ps2AIBodyTarget( CBaseEntity *pEnemy )
{
	Vector target;
	ItemInfo II;
	float f_distance;
	float f_scale;
	float flAim;
	float d_x, d_y, d_z;

	f_distance = ( pEnemy->pev->origin - pev->origin ).Length();

	if ( f_distance > 1000 )
		f_scale = 1.0f;
	else if ( f_distance > 100 )
		f_scale = f_distance / 1000.0f;
	else
		f_scale = 0.1f;

	if ( CBasePlayerItem::ItemInfoArray[ m_pActiveItem->m_iId ].iMaxClip != WEAPON_NOCLIP )
	{
		if ( ( ((CBasePlayerWeapon *)m_pActiveItem)->m_iClip <= 0 && m_rgAmmo[ m_pActiveItem->PrimaryAmmoIndex() ] <= 0 )
			|| f_distance < g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist
			|| g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist < f_distance
			|| pEnemy->pev->max_health < g_AIWeapons[ m_ps2AIType ].flMinEnemyHealth )
		{
			Ps2AIChangeWeapon();
		}
	}

	if ( m_ps2AIType == 0 && m_pActiveItem )
	{
		m_pActiveItem->GetItemInfo( &II );
		m_ps2AIType = II.iId;
	}

	flAim = g_AIWeapons[ m_ps2AIType ].flPrimaryAim[ m_bAISkill ];

	switch ( m_ps2AIType )
	{
	case WEAPON_GLOCK:
	case WEAPON_SHOTGUN:
	case WEAPON_HORNETGUN:
		if ( f_distance >= g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist && f_distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist )
			flAim = g_AIWeapons[ m_ps2AIType ].flSecondaryAim[ m_bAISkill ];
		break;

	case WEAPON_MP5:
		if ( RANDOM_LONG( 0, 100 ) <= 10 && m_rgAmmo[ m_pActiveItem->SecondaryAmmoIndex() ] > 0
			&& f_distance >= g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist && f_distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist )
			flAim = g_AIWeapons[ m_ps2AIType ].flSecondaryAim[ m_bAISkill ];
		break;

	case WEAPON_GAUSS:
		if ( m_flAIUnk740 != -1 || ( f_distance >= g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist && f_distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist ) )
		{
			if ( m_flAIUnk740 == -1 )
				m_flAIUnk740 = gpGlobals->time + RANDOM_FLOAT( 0.5f, 1.5f );
			else if ( m_flAIUnk740 < gpGlobals->time )
				flAim = g_AIWeapons[ m_ps2AIType ].flSecondaryAim[ m_bAISkill ];
		}
		break;
	}

	if ( RANDOM_LONG( 0, 4 ) == 0 )
		target = pEnemy->Center() + pEnemy->pev->view_ofs;
	else
		target = pEnemy->Center();

	d_x = RANDOM_FLOAT( -flAim, flAim ) * f_scale;
	d_y = RANDOM_FLOAT( -flAim, flAim ) * f_scale;
	d_z = RANDOM_FLOAT( -flAim, flAim ) * f_scale;

	target = target + Vector( d_x, d_y, d_z );

	return target;
}

/* FVisible do HPB bot: linha de visão dos olhos até vecOrigin, sem monstros */
BOOL CBasePlayer :: Ps2AIFVisible( const Vector &vecOrigin )
{
	TraceResult tr;
	Vector vecLookerOrigin;

	vecLookerOrigin = EyePosition();

	UTIL_TraceLine( vecLookerOrigin, vecOrigin, ignore_monsters, dont_ignore_glass, edict(), &tr );

	if ( tr.flFraction != 1.0f )
		return FALSE;
	else
		return TRUE;
}

/* BotFollowUser: segue o jogador que deu "use" no parceiro */
BOOL CBasePlayer :: Ps2AIFollowUser( float flMoved, float flUnk )
{
	Vector vecUser;
	BOOL fVisible;
	float f_distance;

	Vector vecEnd = ((CBaseEntity *)m_hAIUser)->pev->origin + ((CBaseEntity *)m_hAIUser)->pev->view_ofs;

	pev->v_angle.x = 0;
	pev->v_angle.z = 0;
	pev->angles.x = 0;
	pev->angles.y = pev->v_angle.y;
	pev->angles.z = 0;

	if ( !((CBaseEntity *)m_hAIUser)->IsAlive() )
	{
		m_hAIUser = NULL;
		return FALSE;
	}

	fVisible = Ps2AIFVisible( vecEnd );
	vecUser = ((CBaseEntity *)m_hAIUser)->pev->origin;

	if ( fVisible )
	{
		m_flAIUseTime = gpGlobals->time;

		if ( flUnk == 0 && flMoved < 1.0f )
		{
			if ( !( m_iAIState & 0x100 ) )
			{
				m_iAIState |= 0x100;
				m_flAIStuckTime = gpGlobals->time;
			}
			else if ( m_flAIStuckTime + 1.0f < gpGlobals->time )
			{
				vecUser = ((CBaseEntity *)m_hAIUser)->pev->origin;

				if ( Ps2AICanJumpUp() )
				{
					pev->button |= IN_JUMP;
					m_iAIState |= 0x40;
					m_flAIMoveSpeed = m_flAIMaxSpeed;
				}
				else if ( Ps2AICanDuckUnder() )
				{
					pev->button |= IN_DUCK;
				}
				else
				{
					Ps2AINavigate( flMoved, ((CBaseEntity *)m_hAIUser)->pev->origin );
					if ( !( m_iAIState & 0x10 ) )
						return FALSE;
				}
			}
		}
		else
		{
			m_iAIState &= ~0x100;
		}

		Vector v_user = vecUser - pev->origin;
		Vector bot_angles = UTIL_VecToAngles( v_user );

		pev->ideal_yaw = bot_angles.y;

		if ( pev->ideal_yaw > 180 )
			pev->ideal_yaw -= 360;
		if ( pev->ideal_yaw < -180 )
			pev->ideal_yaw += 360;

		f_distance = v_user.Length();

		if ( f_distance > 200 )
			m_flAIMoveSpeed = m_flAIMaxSpeed;
		else if ( f_distance > 50 )
			m_flAIMoveSpeed = m_flAIMaxSpeed * 0.5f;
		else
		{
			m_flAIMoveSpeed = 0;
			m_iAIState &= ~0x10;
		}

		return TRUE;
	}

	Ps2AINavigate( flMoved, ((CBaseEntity *)m_hAIUser)->pev->origin );
	if ( m_iAIState & 0x10 )
		return TRUE;
	return FALSE;
}

BOOL CBasePlayer :: Ps2AICheckWallOnLeft( void )
{
	Vector v_src, v_left;
	TraceResult tr;

	UTIL_MakeVectors( pev->angles );

	v_src = pev->origin;
	v_left = v_src + gpGlobals->v_right * -40;

	UTIL_TraceLineNoEnts( v_src, v_left, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction < 1.0f )
	{
		if ( m_flAIWallOnLeft == 0 )
			m_flAIWallOnLeft = gpGlobals->time;

		return FALSE;
	}

	return TRUE;
}

BOOL CBasePlayer :: Ps2AICheckWallOnRight( void )
{
	Vector v_src, v_right;
	TraceResult tr;

	UTIL_MakeVectors( pev->angles );

	v_src = pev->origin;
	v_right = v_src + gpGlobals->v_right * 40;

	UTIL_TraceLineNoEnts( v_src, v_right, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction < 1.0f )
	{
		if ( m_flAIWallOnRight == 0 )
			m_flAIWallOnRight = gpGlobals->time;

		return FALSE;
	}

	return TRUE;
}

/* BotFindItem */
/* NON_MATCHING: ordem de carga de duas constantes içadas do laço (40 e 5) */
#ifdef NON_MATCHING
void CBasePlayer :: Ps2AIFindItem( void )
{
	CBaseEntity *pEnt = NULL;
	CBaseEntity *pPickupEntity = NULL;
	Vector pickup_origin;
	Vector entity_origin;
	float radius = 500;
	BOOL can_pickup;
	float min_distance;
	char item_name[40];
	TraceResult tr;
	Vector vecStart;
	Vector vecEnd;
	int angle_to_entity;
	float distance;

	m_hAIPickupItem = NULL;

	min_distance = radius + 1.0f;

	while ( ( pEnt = UTIL_FindEntityInSphere( pEnt, pev->origin, radius ) ) != NULL )
	{
		can_pickup = FALSE;

		strcpy( item_name, STRING( pEnt->pev->classname ) );

		if ( strncmp( "func_", item_name, 5 ) == 0 )
		{
			entity_origin = VecBModelOrigin( pEnt->pev );

			vecStart = pev->origin + pev->view_ofs;
			vecEnd = entity_origin;

			angle_to_entity = Ps2AIInFieldOfView( vecEnd - vecStart );

			if ( angle_to_entity > 45 )
				continue;

			if ( strcmp( "func_ladder", item_name ) == 0 )
			{
				entity_origin.z = pev->origin.z;
				vecEnd = entity_origin;

				UTIL_TraceLineNoEnts( vecStart, vecEnd, dont_ignore_monsters, pev->pContainingEntity, &tr );

				if ( tr.flFraction >= 1.0f )
				{
					distance = ( vecEnd - vecStart ).Length();

					if ( RANDOM_LONG( 1, 100 ) <= 100 && m_flAIEndUseLadderTime + 5.0f < gpGlobals->time )
					{
						if ( distance < 100 )
							m_flAIDontAvoidWallTime = gpGlobals->time + 5.0f;

						can_pickup = TRUE;
					}
				}
			}
			else
			{
				UTIL_TraceLineNoEnts( vecStart, vecEnd, dont_ignore_monsters, pev->pContainingEntity, &tr );

				if ( strcmp( item_name, STRING( tr.pHit->v.classname ) ) == 0 )
				{
					distance = ( vecEnd - vecStart ).Length();

					if ( ( m_iAIFlags & 2 ) && strcmp( "func_healthcharger", item_name ) == 0 )
					{
						if ( pev->health < 100 && pEnt->pev->frame == 0 )
						{
							if ( !( m_iAIState & 2 ) )
							{
								if ( distance < 40 && angle_to_entity <= 10 )
								{
									m_iAIState |= 2;
									m_flAIUseHealthTime = gpGlobals->time;
								}

								if ( distance < 100 )
									m_flAIDontAvoidWallTime = gpGlobals->time + 5.0f;

								can_pickup = TRUE;
							}
						}
						else
						{
							m_iAIState &= ~2;
						}
					}
					else if ( ( m_iAIFlags & 4 ) && strcmp( "func_recharge", item_name ) == 0 )
					{
						if ( pev->armorvalue < 100 && ( pev->weapons & ( 1 << WEAPON_SUIT ) ) && pEnt->pev->frame == 0 )
						{
							if ( !( m_iAIState & 4 ) )
							{
								if ( distance < 40 && angle_to_entity <= 10 )
								{
									m_iAIState |= 4;
									m_flAIUseHEVTime = gpGlobals->time;
								}

								if ( distance < 100 )
									m_flAIDontAvoidWallTime = gpGlobals->time + 5.0f;

								can_pickup = TRUE;
							}
						}
						else
						{
							m_iAIState &= ~4;
						}
					}
					else if ( ( m_iAIFlags & 8 ) && strcmp( "func_button", item_name ) == 0 )
					{
						if ( RANDOM_LONG( 1, 100 ) <= 100 && m_flAIUseTime + 5.0f < gpGlobals->time )
						{
							if ( !( m_iAIState & 8 ) )
							{
								if ( distance < 40 && angle_to_entity <= 10 )
								{
									m_iAIState |= 8;
									m_flAIUseTime = gpGlobals->time;
								}

								if ( distance < 100 )
									m_flAIDontAvoidWallTime = gpGlobals->time + 5.0f;

								can_pickup = TRUE;
							}
						}
						else
						{
							m_iAIState &= ~8;
						}
					}
				}
			}
		}
		else
		{
			entity_origin = pEnt->pev->origin;

			vecStart = pev->origin + pev->view_ofs;
			vecEnd = entity_origin;

			angle_to_entity = Ps2AIInFieldOfView( vecEnd - vecStart );

			if ( angle_to_entity > 45 )
				continue;

			if ( Ps2AIEntityIsVisible( vecEnd ) )
			{
				if ( strncmp( "weapon_", item_name, 7 ) == 0 )
				{
					if ( ( (CBasePlayerItem *)pEnt )->m_pPlayer )
						continue;

					if ( pEnt->pev->effects & EF_NODRAW )
						continue;

					can_pickup = g_pGameRules->CanHavePlayerItem( this, (CBasePlayerItem *)pEnt ) != 0;
				}
				else if ( strncmp( "ammo_", item_name, 5 ) == 0 )
				{
					int i;

					if ( pEnt->pev->effects & EF_NODRAW )
						continue;

					for ( i = 0; g_AIAmmo[i].pszClassname[0]; i++ )
					{
						if ( strcmp( g_AIAmmo[i].pszClassname, item_name ) == 0 && g_pGameRules->CanHaveAmmo( this, g_AIAmmo[i].pszAmmoName, g_AIAmmo[i].iMaxCarry ) )
						{
							can_pickup = TRUE;
							break;
						}
					}
				}
				else if ( strcmp( "item_battery", item_name ) == 0 )
				{
					if ( pEnt->pev->effects & EF_NODRAW )
						continue;

					if ( pev->armorvalue < 100 && ( pev->weapons & ( 1 << WEAPON_SUIT ) ) )
						can_pickup = TRUE;
				}
				else if ( strcmp( "item_healthkit", item_name ) == 0 )
				{
					if ( pEnt->pev->effects & EF_NODRAW )
						continue;

					if ( pev->health < 100 )
						can_pickup = TRUE;
				}
				else if ( strcmp( "weaponbox", item_name ) == 0 )
				{
					can_pickup = TRUE;
				}
				else if ( strcmp( "laser_spot", item_name ) == 0 )
				{
				}
				else if ( strcmp( "monster_tripmine", item_name ) == 0 )
				{
				}
				else if ( strcmp( "monster_satchel", item_name ) == 0 )
				{
				}
				else if ( strcmp( "monster_snark", item_name ) == 0 )
				{
				}
			}
		}

		if ( can_pickup )
		{
			distance = ( entity_origin - pev->origin ).Length();

			if ( distance < min_distance )
			{
				min_distance = distance;
				pPickupEntity = pEnt;
				pickup_origin = entity_origin;
			}
		}
	}

	if ( pPickupEntity != NULL )
	{
		Vector v_item = pickup_origin - pev->origin;
		Vector bot_angles = UTIL_VecToAngles( v_item );

		pev->ideal_yaw = bot_angles.y;

		if ( pev->ideal_yaw > 180 )
			pev->ideal_yaw -= 360;
		if ( pev->ideal_yaw < -180 )
			pev->ideal_yaw += 360;

		m_hAIPickupItem = pPickupEntity;
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/aiplayer", Ps2AIFindItem__11CBasePlayer);
#endif

/* começo do BotUseLift do HPB bot (ninguém chama) */
void CBasePlayer :: Ps2AIUseLift( void )
{
	if ( m_flAIUseTime == gpGlobals->time )
	{
		pev->button = IN_USE;

		pev->ideal_yaw = -pev->ideal_yaw;

		if ( pev->ideal_yaw > 180 )
			pev->ideal_yaw -= 360;
		if ( pev->ideal_yaw < -180 )
			pev->ideal_yaw += 360;
	}
}

/* olha para cima e pula se houver chão à frente (ninguém chama) */
void CBasePlayer :: Ps2AIJumpOutOfWater( void )
{
	Vector v_src, v_forward;
	TraceResult tr;

	pev->v_angle.x = -60;
	pev->button |= IN_FORWARD;

	UTIL_MakeVectors( pev->v_angle );

	v_src = pev->origin + pev->view_ofs;
	v_forward = v_src + gpGlobals->v_forward * 90;

	UTIL_TraceLineNoEnts( v_src, v_forward, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction >= 1.0f )
	{
		if ( UTIL_PointContents( tr.vecEndPos ) == CONTENTS_EMPTY )
		{
			v_src = tr.vecEndPos;
			v_forward = v_src;
			v_forward.z -= 90;

			UTIL_TraceLineNoEnts( v_src, v_forward, dont_ignore_monsters, pev->pContainingEntity, &tr );

			if ( tr.flFraction < 1.0f )
			{
				if ( UTIL_PointContents( tr.vecEndPos ) != CONTENTS_WATER )
					pev->button |= IN_JUMP;
			}
		}
	}
}

/* BotCantMoveForward (ninguém chama) */
BOOL CBasePlayer :: Ps2AICantMoveForward( TraceResult *tr )
{
	Vector v_src, v_forward;

	UTIL_MakeVectors( pev->angles );

	v_src = pev->origin + pev->view_ofs;
	v_forward = v_src + gpGlobals->v_forward * 40;

	UTIL_TraceLineNoEnts( v_src, v_forward, dont_ignore_monsters, pev->pContainingEntity, tr );

	if ( tr->flFraction < 1.0f )
		return TRUE;

	v_src = pev->origin;
	v_forward = v_src + gpGlobals->v_forward * 40;

	UTIL_TraceLineNoEnts( v_src, v_forward, dont_ignore_monsters, pev->pContainingEntity, tr );

	if ( tr->flFraction < 1.0f )
		return TRUE;

	return FALSE;
}

/* BotCanJumpUp (com as medidas do PS2) */
BOOL CBasePlayer :: Ps2AICanJumpUp( void )
{
	TraceResult tr;
	Vector v_jump, v_source, v_dest, v_floor;
	BOOL check_duck = FALSE;
	BOOL can_jump = FALSE;

	if ( !( pev->button & IN_JUMP ) && ( pev->flags & FL_ONGROUND ) )
	{

	v_jump = pev->angles;
	v_jump.x = 0;
	v_jump.z = 0;

	UTIL_MakeVectors( v_jump );

	v_source = pev->origin + Vector( 0, 0, -21 );
	v_dest = v_source + gpGlobals->v_forward * 24;

	UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction < 1.0f )
		check_duck = TRUE;

	if ( check_duck )
	{
		v_source = pev->origin + Vector( 0, 0, 28 );
		v_dest = v_source + gpGlobals->v_forward * 24;

		UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

		if ( tr.flFraction == 1.0f )
			can_jump = TRUE;

		if ( !can_jump )
			return FALSE;

		v_source = pev->origin + Vector( 0, 0, -36 ) + gpGlobals->v_forward * 24;
		v_dest = v_source + Vector( 0, 0, -64.1f );

		UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

		if ( tr.flFraction == 1.0f && !tr.fStartSolid )
			return FALSE;
		return can_jump;
	}
	else
	{
		v_source = pev->origin;
		v_dest = v_source + Vector( 0, 0, -108 );

		UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

		v_floor = tr.vecEndPos + Vector( 0, 0, 1 );

		v_source = v_floor + gpGlobals->v_forward * 24;
		v_dest = v_source + Vector( 0, 0, -17 );

		UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

		if ( tr.flFraction == 1.0f )
			can_jump = TRUE;

		v_dest = v_source + Vector( 0, 0, -64.1f );

		UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

		if ( tr.flFraction == 1.0f )
			return FALSE;
	}

	return can_jump;
	}

	return FALSE;
}

/* BotCanDuckUnder (o PS2 só olha se a cabeça bate em algo) */
BOOL CBasePlayer :: Ps2AICanDuckUnder( void )
{
	TraceResult tr;
	Vector v_duck, v_source, v_dest;

	v_duck = pev->angles;
	v_duck.x = 0;
	v_duck.z = 0;

	UTIL_MakeVectors( v_duck );

	v_source = pev->origin + Vector( 0, 0, 35 );
	v_dest = v_source + gpGlobals->v_forward * 24;

	UTIL_TraceLineNoEnts( v_source, v_dest, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction < 1.0f )
		return TRUE;

	return FALSE;
}

/* BotTurnAtWall (ninguém chama) */
void CBasePlayer :: Ps2AITurnAtWall( TraceResult *tr )
{
	Vector Normal;
	float Y, Y1, Y2, D1, D2, Z;

	Normal = UTIL_VecToAngles( tr->vecPlaneNormal );

	Y = pev->angles.y;
	Y = Y + 180;
	if ( Y > 359 )
		Y -= 360;

	Normal.y = Normal.y - 180;
	if ( Normal.y < 0 )
		Normal.y += 360;

	Y1 = Normal.y - 90;
	if ( RANDOM_LONG( 1, 100 ) <= 50 )
		Y1 = Y1 - RANDOM_FLOAT( 5.0f, 20.0f );
	if ( Y1 < 0 )
		Y1 += 360;

	Y2 = Normal.y + 90;
	if ( RANDOM_LONG( 1, 100 ) <= 50 )
		Y2 = Y2 + RANDOM_FLOAT( 5.0f, 20.0f );
	if ( Y2 > 359 )
		Y2 -= 360;

	D1 = fabsf( Y - Y1 );
	if ( D1 > 179 )
		D1 = fabsf( D1 - 360 );
	D2 = fabsf( Y - Y2 );
	if ( D2 > 179 )
		D2 = fabsf( D2 - 360 );

	if ( D1 > D2 )
		Z = Y1;
	else
		Z = Y2;

	if ( Z > 180 )
		Z -= 360;

	pev->ideal_yaw = Z;

	if ( pev->ideal_yaw > 180 )
		pev->ideal_yaw -= 360;
	if ( pev->ideal_yaw < -180 )
		pev->ideal_yaw += 360;

	m_flAIMoveSpeed = 0;
	m_iAIState &= ~0x10;
}

/* BotFireWeapon (do jeito do PS2: uma regra por arma, parâmetros em g_AIWeapons) */
/* NON_MATCHING: estrutura do switch e alocação de registradores diferentes em alguns cases */
#ifdef NON_MATCHING
BOOL CBasePlayer :: Ps2AIFireWeapon( Vector vecTarget, Vector vecDir, int iType, BOOL fUnk )
{
	TraceResult tr;
	Vector v_enemy;
	float distance;

	if ( m_pActiveItem == NULL )
		return FALSE;

	if ( CBasePlayerItem::ItemInfoArray[ m_pActiveItem->m_iId ].iMaxClip != -1 && ( (CBasePlayerWeapon *)m_pActiveItem )->m_iClip <= 0 )
	{
		if ( m_rgAmmo[ m_pActiveItem->PrimaryAmmoIndex() ] > 0 && !( m_iAIState & 0x10 ) )
			Ps2AITakeCover();
		return FALSE;
	}

	distance = ( vecTarget - GetGunPosition() ).Length();

	m_flAIUnk700 = gpGlobals->time + g_AIWeapons[ m_ps2AIType ].flPrimaryDelay;

	switch ( m_ps2AIType )
	{
	case WEAPON_GLOCK:
	case WEAPON_SHOTGUN:
	case WEAPON_HORNETGUN:
		if ( g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist <= distance && distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist )
		{
			pev->button |= IN_ATTACK2;
			m_flAIUnk700 = gpGlobals->time + g_AIWeapons[ m_ps2AIType ].flSecondaryDelay;
			break;
		}
		pev->button |= IN_ATTACK;
		break;

	case WEAPON_PYTHON:
	case WEAPON_CROSSBOW:
		if ( m_hEnemy->pev->max_health <= g_AIWeapons[ m_ps2AIType ].flMinEnemyHealth )
		{
			Ps2AIChangeWeapon();
			return FALSE;
		}
		if ( g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist <= distance && distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist )
			pev->button |= IN_ATTACK;
		else
			pev->button |= IN_ATTACK;
		break;

	case WEAPON_MP5:
		if ( RANDOM_LONG( 0, 100 ) <= 10 && m_rgAmmo[ m_pActiveItem->SecondaryAmmoIndex() ] > 0 )
		{
			float flDist = ( vecTarget - GetGunPosition() ).Length();

			if ( g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist <= flDist && flDist <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist
				&& m_hEnemy->pev->max_health >= g_AIWeapons[ m_ps2AIType ].flMinEnemyHealth
				&& ( m_hEnemy->Classify() == CLASS_HUMAN_MILITARY || m_hEnemy->Classify() == CLASS_ALIEN_MILITARY ) )
			{
				v_enemy = VecCheckThrow( pev, GetGunPosition(), vecTarget, 800, 0.5f );

				pev->v_angle = UTIL_VecToAngles( v_enemy );
				pev->v_angle.x = 360 - pev->v_angle.x;
				pev->punchangle = g_vecZero;

				pev->button |= IN_ATTACK2;
				m_flAIUnk700 = gpGlobals->time + g_AIWeapons[ m_ps2AIType ].flSecondaryDelay;
				break;
			}
		}
		pev->button |= IN_ATTACK;
		break;

	case WEAPON_HANDGRENADE:
		{
			v_enemy = VecCheckToss( pev, GetGunPosition(), vecTarget, 0.5f );

			pev->v_angle = UTIL_VecToAngles( v_enemy );
			pev->v_angle.x = 360 - pev->v_angle.x;
			pev->punchangle = g_vecZero;
		}
		pev->button |= IN_ATTACK;
		break;

	case WEAPON_GAUSS:
		if ( m_flAIUnk740 != -1 || ( g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist <= distance && distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist ) )
		{
			if ( m_flAIUnk740 == -1 )
				m_flAIUnk740 = gpGlobals->time + RANDOM_FLOAT( 0.5f, 1.5f );
			else if ( gpGlobals->time < m_flAIUnk740 )
			{
				pev->button |= IN_ATTACK2;
				m_flAIUnk700 = gpGlobals->time;
			}
			else
				m_flAIUnk740 = -1;
			break;
		}
		pev->button |= IN_ATTACK;
		break;

	case WEAPON_RPG:
		if ( m_hEnemy->pev->max_health <= g_AIWeapons[ m_ps2AIType ].flMinEnemyHealth )
		{
			Ps2AIChangeWeapon();
			return FALSE;
		}
		else
		{
			float flSide = 8;
			float flClear = 1.0f;

			UTIL_MakeVectors( pev->angles );

			Vector v_src = GetGunPosition() + gpGlobals->v_right * flSide;
			UTIL_TraceLineNoEnts( v_src, vecTarget + gpGlobals->v_right * flSide, dont_ignore_monsters, pev->pContainingEntity, &tr );

			if ( tr.flFraction != flClear && ( !tr.pHit || CBaseEntity::Instance( tr.pHit ) != m_hEnemy ) )
				return FALSE;

			v_src = GetGunPosition() - gpGlobals->v_right * flSide;
			UTIL_TraceLineNoEnts( v_src, vecTarget - gpGlobals->v_right * flSide, dont_ignore_monsters, pev->pContainingEntity, &tr );

			if ( tr.flFraction == flClear || ( tr.pHit && CBaseEntity::Instance( tr.pHit ) == m_hEnemy ) )
			{
				pev->button |= IN_ATTACK;
				break;
			}
			return FALSE;
		}

	case 16:
		if ( m_flAIUnk740 != -1 || ( g_AIWeapons[ m_ps2AIType ].iSecondaryMinDist <= distance && distance <= g_AIWeapons[ m_ps2AIType ].iSecondaryMaxDist ) )
		{
			if ( m_flAIUnk740 == -1 )
				m_flAIUnk740 = gpGlobals->time + RANDOM_FLOAT( 0.5f, 2.5f );
			else if ( gpGlobals->time < m_flAIUnk740 )
			{
				pev->button |= IN_ATTACK2;
				m_flAIUnk700 = gpGlobals->time;
			}
			else
				m_flAIUnk740 = -1;
			break;
		}
		pev->button |= IN_ATTACK;
		break;

	case WEAPON_CROWBAR:
		if ( g_AIWeapons[ m_ps2AIType ].iPrimaryMinDist <= distance && distance <= g_AIWeapons[ m_ps2AIType ].iPrimaryMaxDist )
			pev->button |= IN_ATTACK;
		break;

	default:
		if ( m_pActiveItem )
			pev->button |= IN_ATTACK;
		break;
	}

	return ( pev->button & ( IN_ATTACK | IN_ATTACK2 ) ) != 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/aiplayer", Ps2AIFireWeapon__11CBasePlayerG6VectorT1ii);
#endif

/* escolhe a melhor arma para a distância e a vida do inimigo */
/* NON_MATCHING: registradores e spills diferentes */
#ifdef NON_MATCHING
BOOL CBasePlayer :: Ps2AIChangeWeapon( void )
{
	CBasePlayerItem *pBest = NULL;
	int iBestPriority = -1;
	int iFallbackPriority = -1;
	CBasePlayerItem *pFallback = NULL;
	float distance = 0;
	int i;

	if ( !IsAlive() )
		return FALSE;

	if ( m_hEnemy )
		distance = ( m_hEnemy->pev->origin - GetGunPosition() ).Length();

	for ( i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		CBasePlayerItem *pItem = m_rgpPlayerItems[ i ];

		while ( pItem )
		{
			if ( ( (CBasePlayerWeapon *)pItem )->m_iClip > 0 || m_rgAmmo[ pItem->PrimaryAmmoIndex() ] > 0 )
			{
				int iPriority = g_AIWeapons[ pItem->m_iId ].iPriority;

				if ( iPriority > 0 && iBestPriority < iPriority )
				{
					if ( m_hEnemy != NULL
						&& ( distance < g_AIWeapons[ pItem->m_iId ].iPrimaryMinDist
							|| g_AIWeapons[ pItem->m_iId ].iPrimaryMaxDist < distance
							|| m_hEnemy->pev->max_health < g_AIWeapons[ pItem->m_iId ].flMinEnemyHealth ) )
					{
						if ( iFallbackPriority < g_AIWeapons[ pItem->m_iId ].iPriority )
						{
							iFallbackPriority = g_AIWeapons[ pItem->m_iId ].iPriority;
							pFallback = pItem;
						}
					}
					else
					{
						pBest = pItem;
						iBestPriority = g_AIWeapons[ pItem->m_iId ].iPriority;
					}
				}
			}

			pItem = pItem->m_pNext;
		}
	}

	if ( !pBest )
		pBest = pFallback;

	if ( !pBest )
		return FALSE;

	ItemInfo II;
	pBest->GetItemInfo( &II );

	if ( m_pActiveItem )
	{
		ItemInfo IICur;
		m_pActiveItem->GetItemInfo( &IICur );

		if ( IICur.iId != II.iId )
		{
			SwitchWeapon( pBest );
			return TRUE;
		}

		if ( !m_ps2AIType )
			m_ps2AIType = II.iId;
	}
	else
	{
		SwitchWeapon( pBest );
	}

	return TRUE;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/aiplayer", Ps2AIChangeWeapon__11CBasePlayer);
#endif

/* BotInFieldOfView (sem a correção do ângulo > 180) */
int CBasePlayer :: Ps2AIInFieldOfView( Vector dest )
{
	Vector entity_angles = UTIL_VecToAngles( dest );

	if ( entity_angles.y < 0 )
		entity_angles.y += 360;

	float view_angle = pev->v_angle.y;

	if ( view_angle < 0 )
		view_angle += 360;

	return abs( (int)view_angle - (int)entity_angles.y );
}

/* BotEntityIsVisible */
BOOL CBasePlayer :: Ps2AIEntityIsVisible( Vector dest )
{
	TraceResult tr;

	UTIL_TraceLine( pev->origin + pev->view_ofs, dest, ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction >= 1.0f )
		return TRUE;
	else
		return FALSE;
}

/* procura uma arma do jogador pelo id (ninguém chama) */
CBasePlayerItem *CBasePlayer :: Ps2AIFindWeapon( int iId )
{
	int i;

	for ( i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		CBasePlayerItem *pItem = m_rgpPlayerItems[ i ];

		while ( pItem )
		{
			if ( pItem->m_iId == iId )
				return pItem;

			pItem = pItem->m_pNext;
		}
	}

	return NULL;
}

/* monta uma rota pelos nós até o destino (no máximo a cada 2 segundos) */
void CBasePlayer :: Ps2AINavigate( float flMoved, Vector vecDest )
{
	if ( gpGlobals->time < m_flAINavigateTime + 2.0f )
		return;

	if ( (CBaseEntity *)m_hAIUser != NULL )
	{
		if ( ( ((CBaseEntity *)m_hAIUser)->pev->flags & FL_ONGROUND ) && BuildRoute( vecDest, 0x40, m_hAIUser ) )
			m_iAIState |= 0x10;
		else
			m_iAIState &= ~0x10;
	}
	else
	{
		if ( BuildRoute( vecDest, 0x40, m_hAIUser ) )
			m_iAIState |= 0x10;
		else
			m_iAIState &= ~0x10;
	}

	m_iAIState &= ~0x100;
	m_flAINavigateTime = gpGlobals->time;
}

/* segue a rota de nós (m_Route) até o fim; se ficar preso tenta pular ou agachar (ninguém chama) */
void CBasePlayer :: Ps2AIFollowRoute( float flExpected, float flMoved, float flTurn )
{
	Vector v_dest = m_Route[ m_iRouteIndex ].vecLocation - pev->origin;

	if ( v_dest.Length2D() < 64 )
	{
		if ( m_Route[ m_iRouteIndex ].iType & 0x80 )
		{
			m_iAIState &= ~0x10;
			return;
		}

		m_iRouteIndex++;

		if ( m_hAIUser )
			RouteSimplify( m_hAIUser );

		v_dest = m_Route[ m_iRouteIndex ].vecLocation - pev->origin;
	}

	Vector bot_angles = UTIL_VecToAngles( v_dest );

	pev->ideal_yaw = bot_angles.y;

	if ( pev->ideal_yaw > 180 )
		pev->ideal_yaw -= 360;
	if ( pev->ideal_yaw < -180 )
		pev->ideal_yaw += 360;

	if ( pev->yaw_speed <= flTurn )
		m_flAIMoveSpeed = 0;
	else
		m_flAIMoveSpeed = m_flAIMaxSpeed;

	float flDist = ( flMoved / flExpected > 2.0f ) ? flExpected : flMoved;

	if ( flTurn == 0 && flDist < 1.0f )
	{
		if ( !( m_iAIState & 0x100 ) )
		{
			m_iAIState |= 0x100;
			m_flAIStuckTime = gpGlobals->time;
		}
		else if ( m_flAIStuckTime + 1.0f < gpGlobals->time )
		{
			if ( m_iAIState & 0x800 )
			{
				m_flAIMoveSpeed = 0;
				m_iAIState &= ~0x100;
				m_iAIState &= ~0x800;
				m_iAIState &= ~0x10;
				return;
			}

			if ( Ps2AICanJumpUp() )
			{
				pev->button |= IN_JUMP;
				m_flAIMoveSpeed = m_flAIMaxSpeed;
				m_iAIState |= 0x840;
			}
			else if ( Ps2AICanDuckUnder() )
			{
				pev->button |= IN_DUCK;
				m_iAIState |= 0x800;
			}
			else
			{
				m_flAIMoveSpeed = 0;
				m_iAIState &= ~0x10;

				if ( m_flAINextAttackSentence < gpGlobals->time )
				{
					if ( coopslave.value == 0 )
					{
						if ( m_iTeamNum == 1 )
							PlaySentence( "GINA_STOP", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
						else
							PlaySentence( "COL_STOP", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
					}

					m_flAINextAttackSentence = gpGlobals->time + 10;
				}

				if ( (CBaseEntity *)m_hAIUser )
					m_hAIUser = NULL;
			}

			m_iAIState &= ~0x100;
		}
	}
	else
	{
		m_iAIState &= ~0x100;
		m_iAIState &= ~0x800;
	}
}

/* segue o rastro de posições deixado pelo dono (ninguém chama) */
void CBasePlayer :: Ps2AIFollowTrail( float flMoved, float flTurn )
{
	Vector v_dest = m_vecAITrailPos - pev->origin;

	if ( v_dest.Length2D() < 8 )
	{
		if ( !Ps2AIGetTrailPoint( m_vecAITrailPos ) )
		{
			m_iAIState &= ~0x80;
			return;
		}

		v_dest = m_vecAITrailPos - pev->origin;
	}

	Vector bot_angles = UTIL_VecToAngles( v_dest );

	pev->ideal_yaw = bot_angles.y;

	if ( pev->ideal_yaw > 180 )
		pev->ideal_yaw -= 360;
	if ( pev->ideal_yaw < -180 )
		pev->ideal_yaw += 360;

	if ( flTurn > 0 )
		m_flAIMoveSpeed = m_flAIMaxSpeed * 0.25f;
	else
		m_flAIMoveSpeed = m_flAIMaxSpeed;

	if ( m_flAIMoveSpeed > 0 && flMoved < 1.0f )
	{
		if ( !( m_iAIState & 0x100 ) )
		{
			m_iAIState |= 0x100;
			m_flAIStuckTime = gpGlobals->time;
			return;
		}

		if ( Ps2AICanJumpUp() )
		{
			pev->button |= IN_JUMP;
			m_iAIState |= 0x40;
			m_flAIMoveSpeed = m_flAIMaxSpeed;
		}
		else if ( Ps2AICanDuckUnder() )
		{
			pev->button |= IN_DUCK;
		}
		else
		{
			m_flAIMoveSpeed = 0;
			m_iAIState &= ~0x80;
		}
	}

	m_iAIState &= ~0x100;
}

/* recua 100 unidades na direção contrária a ideal_yaw (ninguém chama) */
void CBasePlayer :: Ps2AIBackOff( float flMoved )
{
	Vector vecDest = pev->angles;
	Vector vecForward;

	vecDest.y = pev->ideal_yaw + 180;

	g_engfuncs.pfnAngleVectors( vecDest, vecForward, NULL, NULL );

	vecDest = pev->origin + vecForward * 100;

	Ps2AINavigate( flMoved, vecDest );
}

/* procura cobertura contra o inimigo (no máximo a cada 2 segundos) */
void CBasePlayer :: Ps2AITakeCover( void )
{
	entvars_t *pevEnemy;

	if ( gpGlobals->time < m_flAINavigateTime + 2.0f )
		return;

	if ( !m_hEnemy )
		return;

	pevEnemy = m_hEnemy->pev;

	if ( FindLateralCover( pevEnemy->origin, pevEnemy->view_ofs ) )
		m_iAIState |= 0x10;
	else if ( FindCover( pevEnemy->origin, pevEnemy->view_ofs, 0, CoverRadius() ) )
		m_iAIState |= 0x10;

	m_flAINavigateTime = gpGlobals->time;
}

/* touch do parceiro: se um jogador esbarra correndo, vira para ele */
void CBasePlayer :: Ps2AITouch( CBaseEntity *pOther )
{
	if ( pOther->IsPlayer() )
	{
		float flSpeed = fabsf( pOther->pev->velocity.x ) + fabs( pOther->pev->velocity.y );

		if ( flSpeed > 50 )
		{
			m_afConditions |= 0x10000000;
			MakeIdealYaw( pOther->pev->origin );
		}
	}
}

/* guarda a posição atual (no chão) no rastro circular de 6 pontos */
void CBasePlayer :: Ps2AIRecordTrail( void )
{
	TraceResult tr;

	UTIL_TraceLine( pev->origin, pev->origin - Vector( 0, 0, 384 ), ignore_monsters, pev->pContainingEntity, &tr );

	m_vecAITrail[ m_iAITrailHead ] = pev->origin;
	m_vecAITrail[ m_iAITrailHead ].z = tr.vecEndPos.z + 8;

	m_iAITrailHead++;
	if ( m_iAITrailHead >= 6 )
		m_iAITrailHead = 0;

	if ( m_iAITrailTail == m_iAITrailHead )
	{
		m_iAITrailTail++;
		if ( m_iAITrailTail >= 6 )
			m_iAITrailTail = 0;
	}
}

/* acha o ponto mais antigo do rastro do dono que está visível */
BOOL CBasePlayer :: Ps2AIFindTrailPoint( Vector &vecPos )
{
	int iFound = -1;
	int i;
	CBasePlayer *pUser;

	if ( m_iAITrailTail == m_iAITrailHead )
		return FALSE;

	pUser = (CBasePlayer *)(CBaseEntity *)m_hAIUser;

	if ( !pUser )
		return FALSE;

	i = pUser->m_iAITrailTail;

	while ( i != pUser->m_iAITrailHead )
	{
		if ( Ps2AIFVisible( pUser->m_vecAITrail[ i ] ) )
		{
			iFound = i;
			break;
		}

		i++;
		if ( i >= 6 )
			i = 0;
	}

	if ( iFound == -1 )
		return FALSE;

	vecPos = pUser->m_vecAITrail[ iFound ];
	m_iAITrailIndex = iFound;

	return TRUE;
}

/* próximo ponto do rastro do dono */
BOOL CBasePlayer :: Ps2AIGetTrailPoint( Vector &vecPos )
{
	CBasePlayer *pUser = (CBasePlayer *)(CBaseEntity *)m_hAIUser;

	if ( !pUser )
		return FALSE;

	m_iAITrailIndex++;
	if ( m_iAITrailIndex >= 6 )
		m_iAITrailIndex = 0;

	if ( m_iAITrailIndex == pUser->m_iAITrailHead )
		return FALSE;

	vecPos = pUser->m_vecAITrail[ m_iAITrailIndex ];

	return TRUE;
}

/* foge da origem do dano: para trás, para a direita ou para a esquerda */
/* NON_MATCHING: agendamento dos argumentos das chamadas */
#ifdef NON_MATCHING
BOOL CBasePlayer :: Ps2AIDodge( Vector vecFrom, float flMoved, float flDist )
{
	Vector vecDir, vecSrc, vecEnd, vecAngles;
	TraceResult tr;

	if ( m_flAINavigateTime < 2.0f )
		m_flAINavigateTime = 0;
	else
		m_flAINavigateTime = gpGlobals->time - 2.0f;

	m_iAIState &= ~0x100;
	m_iAIState &= ~0x10;

	vecDir = ( pev->origin - vecFrom ).Normalize();
	vecSrc = pev->origin + Vector( 0, 0, 18 );
	vecEnd = vecSrc + vecDir * flDist;

	UTIL_TraceLineNoEnts( vecSrc, vecEnd, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction == 1.0f )
	{
		Ps2AINavigate( flMoved, vecEnd );
		if ( m_iAIState & 0x10 )
			return TRUE;
	}

	vecAngles = UTIL_VecToAngles( vecDir );
	UTIL_MakeVectors( vecAngles );

	vecEnd = vecSrc + gpGlobals->v_right * flDist;

	UTIL_TraceLineNoEnts( vecSrc, vecEnd, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction == 1.0f )
	{
		Ps2AINavigate( flMoved, pev->origin + gpGlobals->v_right * flDist );
		if ( m_iAIState & 0x10 )
			return TRUE;
	}

	flDist = -flDist;
	vecEnd = vecSrc + gpGlobals->v_right * flDist;

	UTIL_TraceLineNoEnts( vecSrc, vecEnd, dont_ignore_monsters, pev->pContainingEntity, &tr );

	if ( tr.flFraction == 1.0f )
	{
		Ps2AINavigate( flMoved, pev->origin + gpGlobals->v_right * flDist );
		if ( m_iAIState & 0x10 )
			return TRUE;
	}

	return FALSE;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/aiplayer", Ps2AIDodge__11CBasePlayerG6Vectorff);
#endif

/* NON_MATCHING: o alvo reaproveita o registrador de (m_iAIState & 0x400) como zero no FNullEnt e guarda &g_engfuncs em registrador */
#ifdef NON_MATCHING
int CBasePlayer::Ps2AITakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( !( m_iAIState & 0x400 ) && !FNullEnt( pevAttacker ) && CBaseEntity::Instance( pevAttacker ) )
	{
		if ( !CBaseEntity::Instance( pevAttacker )->MyMonsterPointer() )
		{
			Ps2AIDodge( pev->origin, 0, 96 );

			if ( m_iAIState & 0x10 )
				m_iAIState |= 0x400;

			if ( m_hAIUser != NULL && m_flAINextAttackSentence < gpGlobals->time )
			{
				if ( coopslave.value == 0 )
				{
					if ( m_iTeamNum == 1 )
						PlaySentence( "GINA_STOP", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
					else
						PlaySentence( "COL_STOP", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
				}

				m_flAINextAttackSentence = gpGlobals->time + 10;
				m_hAIUser = NULL;
			}

			return TRUE;
		}
		else if ( m_hEnemy == NULL && !CBaseEntity::Instance( pevAttacker )->IsPlayer() )
		{
			m_hEnemy = CBaseEntity::Instance( pevAttacker );
		}
	}

	if ( flDamage > 0 && m_flAINextAttackSentence < gpGlobals->time )
	{
		if ( coopslave.value == 0 )
		{
			if ( m_iTeamNum == 1 )
				PlaySentence( "GINA_PAIN", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
			else
				PlaySentence( "COL_PAIN", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
		}

		m_flAINextAttackSentence = gpGlobals->time + 10;
	}

	return FALSE;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/aiplayer", Ps2AITakeDamage__11CBasePlayerP9entvars_sT1fi);
#endif

void CBasePlayer::Ps2AIKilled( void )
{
	if ( coopslave.value == 0 )
	{
		if ( m_iTeamNum == 1 )
			PlaySentence( "GINA_DIE", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
		else
			PlaySentence( "COL_DIE", RANDOM_FLOAT( 2.8f, 3.2f ), 1.0f, 2.0f );
	}
}

void CBasePlayer::Ps2AISwitchWeapon( CBasePlayerItem *pWeapon )
{
	if ( pWeapon )
	{
		ItemInfo II;
		pWeapon->GetItemInfo( &II );
		m_ps2AIType = II.iId;
	}
	else
	{
		m_ps2AIType = 0;
	}
}

void CBasePlayer::PlaySentence( const char *pszSentence, float duration, float volume, float attenuation )
{
	if ( !pszSentence )
		return;

	if ( !IsAlive() )
		return;

	if ( pszSentence[0] == '!' )
		EMIT_SOUND_DYN( edict(), CHAN_VOICE, pszSentence, volume, attenuation, 0, PITCH_NORM );
	else
		SENTENCEG_PlayRndSz( edict(), pszSentence, volume, attenuation, 0, PITCH_NORM );
}

extern "C" void func_00239810( const char *name, USEPTR pfn );

extern "C" void func_0012CEE8( void )
{
	func_00239810( "007", static_cast<USEPTR>( &CBasePlayer::Ps2AIUse ) );
}
