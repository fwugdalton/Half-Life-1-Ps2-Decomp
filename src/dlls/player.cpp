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
/* dlls/player.cpp: o jogador (CBasePlayer). Base do HLSDK antigo; o PS2 ganhou a
   mira com alvo (targeting_spot), o destaque de objetos usáveis (useable_graphic),
   o histórico de nós do enxame, a munição infinita e a gravidade padrão. */

typedef struct _FILE FILE;
#include "dlls/extdll.h"
#include "dlls/util.h"

#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/trains.h"
#include "dlls/nodes.h"
#include "dlls/weapons.h"
#include "dlls/soundent.h"
#include "dlls/monsters.h"
#include "dlls/shake.h"
#include "dlls/decals.h"
#include "dlls/gamerules.h"
#include "dlls/effects.h"
#include "dlls/skill.h"
#include "include_asm.h"

extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern DLL_GLOBAL	BOOL	g_fDrawLines;
extern DLL_GLOBAL int		g_iSkillLevel, gDisplayTitle;

BOOL gInitHUD = TRUE;

extern void CopyToBodyQue(entvars_t* pev);
extern void respawn(entvars_t *pev, BOOL fCopyCorpse);
extern Vector VecBModelOrigin(entvars_t *pevBModel );
extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );


#define IN_JUMP		(1 << 1)
#define IN_DUCK		(1 << 2)
#define IN_FORWARD	(1 << 3)
#define IN_BACK		(1 << 4)
#define IN_USE		(1 << 5)
#define IN_CANCEL	(1 << 6)
#define IN_LEFT		(1 << 7)
#define IN_RIGHT	(1 << 8)
#define IN_MOVELEFT	(1 << 9)
#define IN_MOVERIGHT (1 << 10)
#define IN_RUN		(1 << 12)
#define IN_ALT1		(1 << 14)
#define IN_SCORE	(1 << 15)

extern char TEXTURETYPE_Find( char *name );
extern int UTIL_PointContents( const Vector &vec );
extern float UTIL_SplineFraction( float value, float scale );
extern Vector UTIL_ClampVectorToBox( const Vector &input, const Vector &clampSize );

extern "C" double sqrt( double x );
int MapTextureTypeStepType( char chTextureType );

extern "C" int cl_numlocal;
extern "C" int D_003241F0;	/* jogadores congelados (trigger_playerfreeze, triggers.cpp) */
extern "C" void func_002071C8( CBasePlayer *pPlayer, unsigned char type, unsigned char intensity, unsigned short duration );	/* vibração */

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;
extern "C" ps2cvar_t D_00332668;	/* intf_playerdead (motor) */
#define intf_playerdead D_00332668
extern ps2cvar_t forcerespawn;

// the world node graph
extern CGraph	WorldGraph;

#define TRAIN_ACTIVE	0x80
#define TRAIN_NEW		0xc0
#define TRAIN_OFF		0x00
#define TRAIN_NEUTRAL	0x01
#define TRAIN_SLOW		0x02
#define TRAIN_MEDIUM	0x03
#define TRAIN_FAST		0x04
#define TRAIN_BACK		0x05

#define DMG_TIMEBASED		(~(0x3fff))	// mask for time-based damage

#define SUIT_GROUP			TRUE
#define	SUIT_SENTENCE		FALSE

#define	SUIT_REPEAT_OK		0
#define SUIT_NEXT_IN_30SEC	30
#define SUIT_NEXT_IN_1MIN	60
#define SUIT_NEXT_IN_5MIN	300
#define SUIT_NEXT_IN_10MIN	600
#define SUIT_NEXT_IN_30MIN	1800
#define SUIT_NEXT_IN_1HOUR	3600

#define CSUITNOREPEAT		32

#define	SOUND_FLASHLIGHT_ON		"items/flashlight1.wav"
#define	SOUND_FLASHLIGHT_OFF	"items/flashlight1.wav"

#define GIB_NORMAL			0
#define GIB_NEVER			1
#define GIB_ALWAYS			2

#define WEAPON_SUIT			31
#define EF_DIMLIGHT			8
#define WEAPON_ALLWEAPONS	(~(1<<WEAPON_SUIT))

#define	FLASH_DRAIN_TIME	 1.2f //100 units/3 minutes
#define	FLASH_CHARGE_TIME	 0.2f // 100 units/20 seconds  (seconds per unit)

//=========================================================
// PS2: marcador da mira (sprite em volta do alvo)
//=========================================================
class CTargetSpot : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void ) { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static CTargetSpot *Create( int iType );
	void Hide( void );
	void SetTarget( CBaseEntity *pTarget );
	void EXPORT ShrinkThink( void );
};
LINK_ENTITY_TO_CLASS( targeting_spot, CTargetSpot );

//=========================================================
// PS2: realce do objeto usável à frente do jogador
//=========================================================
class CUseableGraphic : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	int ObjectCaps( void ) { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static CUseableGraphic *Create( CBaseEntity *pTarget, int iType );
};

// Global Savedata for player
TYPEDESCRIPTION	CBasePlayer::m_playerSaveData[] =
{
	DEFINE_FIELD( CBasePlayer, m_flFlashLightTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_iFlashBattery, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_afButtonLast, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonPressed, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_afButtonReleased, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgItems, FIELD_INTEGER, MAX_ITEMS ),
	DEFINE_FIELD( CBasePlayer, m_afPhysicsFlags, FIELD_INTEGER ),

	DEFINE_FIELD( CBasePlayer, m_flTimeStepSound, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flTimeWeaponIdle, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flSwimTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flDuckTime, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_flWallJumpTime, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_flSuitUpdate, FIELD_TIME ),
	DEFINE_ARRAY( CBasePlayer, m_rgSuitPlayList, FIELD_INTEGER, CSUITPLAYLIST ),
	DEFINE_FIELD( CBasePlayer, m_iSuitPlayNext, FIELD_INTEGER ),
	DEFINE_ARRAY( CBasePlayer, m_rgiSuitNoRepeat, FIELD_INTEGER, CSUITNOREPEAT ),
	DEFINE_ARRAY( CBasePlayer, m_rgflSuitNoRepeatTime, FIELD_TIME, CSUITNOREPEAT ),
	DEFINE_FIELD( CBasePlayer, m_lastDamageAmount, FIELD_INTEGER ),

	DEFINE_ARRAY( CBasePlayer, m_rgpPlayerItems, FIELD_CLASSPTR, MAX_ITEM_TYPES ),
	DEFINE_FIELD( CBasePlayer, m_pActiveItem, FIELD_CLASSPTR ),
	DEFINE_FIELD( CBasePlayer, m_pLastItem, FIELD_CLASSPTR ),

	DEFINE_ARRAY( CBasePlayer, m_rgAmmo, FIELD_INTEGER, MAX_AMMO_SLOTS ),
	DEFINE_FIELD( CBasePlayer, m_idrowndmg, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_idrownrestored, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_tSneaking, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_iTrain, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_bitsHUDDamage, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_flFallVelocity, FIELD_FLOAT ),
	DEFINE_FIELD( CBasePlayer, m_iTargetVolume, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iWeaponVolume, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iExtraSoundTypes, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iWeaponFlash, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_fLongJump, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_fInitHUD, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_tbdPrev, FIELD_TIME ),

	DEFINE_FIELD( CBasePlayer, m_pTank, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_iHideHUD, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_iFOV, FIELD_INTEGER ),

	// PS2
	DEFINE_FIELD( CBasePlayer, m_iTeamNum, FIELD_INTEGER ),
	DEFINE_ARRAY( CBasePlayer, m_piSwarmNodeHistory, FIELD_INTEGER, 11 ),
	DEFINE_FIELD( CBasePlayer, m_iSwarmHistoryIdx, FIELD_INTEGER ),
	DEFINE_FIELD( CBasePlayer, m_hTargetedEnemy, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_hAutoAimTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_vecLastAngle, FIELD_VECTOR ),
	DEFINE_FIELD( CBasePlayer, m_vecLastTargetPos, FIELD_VECTOR ),
	DEFINE_FIELD( CBasePlayer, m_hUseableHighlight, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_hLastUseableItem, FIELD_EHANDLE ),
	DEFINE_FIELD( CBasePlayer, m_flNextUseableCheck, FIELD_TIME ),
	DEFINE_FIELD( CBasePlayer, m_fInfiniteAmmo, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBasePlayer, m_flDefaultGravity, FIELD_FLOAT ),
};

// PS2: aliados do co-op
char *g_szPS2AllyClasses[] =
{
	"monster_human_grunt_ally",
	"monster_human_medic_ally",
	"monster_human_torch_ally",
	"monster_generic_squad",
};

extern int gmsgRumble;	/* fica no .data de outro arquivo (0x325084) */

int giPrecacheGrunt = 0;
int gmsgShake = 0;
int gmsgFade = 0;
int gmsgSelAmmo = 0;
int gmsgFlashlight = 0;
int gmsgFlashBattery = 0;
int gmsgResetHUD = 0;
int gmsgInitHUD = 0;
int gmsgShowGameTitle = 0;
int gmsgCurWeapon = 0;
int gmsgHealth = 0;
int gmsgDamage = 0;
int gmsgBattery = 0;
int gmsgTrain = 0;
int gmsgLogo = 0;
int gmsgWeaponList = 0;
int gmsgAmmoX = 0;
int gmsgHudText = 0;
int gmsgDeathMsg = 0;
int gmsgScoreInfo = 0;
int gmsgTeamInfo = 0;
int gmsgTeamScore = 0;
int gmsgGameMode = 0;
int gmsgMOTD = 0;
int gmsgAmmoPickup = 0;
int gmsgWeapPickup = 0;
int gmsgItemPickup = 0;
int gmsgHideWeapon = 0;
int gmsgSetCurWeap = 0;
int gmsgSayText = 0;
int gmsgTextMsg = 0;
int gmsgSetFOV = 0;
int gmsgShowMenu = 0;
int gmsgTankUse = 0;
int gmsgTankAmmo = 0;
int gmsgSlaveHud = 0;
int gmsgButtonHud = 0;
int gmsgCrouchState = 0;
int gmsgHudColor = 0;
int gmsgToggleScore = 0;

LINK_ENTITY_TO_CLASS( player, CBasePlayer );

void CBasePlayer :: Pain( void )
{
	float	flRndSound;//sound randomizer

	flRndSound = RANDOM_FLOAT ( 0 , 1 );

	if ( flRndSound <= 0.33f )
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain5.wav", 1, ATTN_NORM);
	else if ( flRndSound <= 0.66f )
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain6.wav", 1, ATTN_NORM);
	else
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain7.wav", 1, ATTN_NORM);
}

Vector VecVelocityForDamage(float flDamage)
{
	Vector vec(RANDOM_FLOAT(-100,100), RANDOM_FLOAT(-100,100), RANDOM_FLOAT(200,300));

	if (flDamage > -50)
		vec = vec * 0.7f;
	else if (flDamage > -200)
		vec = vec * 2;
	else
		vec = vec * 10;

	return vec;
}

int TrainSpeed(int iSpeed, int iMax)
{
	float fSpeed, fMax;
	int iRet = 0;

	fMax = (float)iMax;
	fSpeed = iSpeed;

	fSpeed = fSpeed/fMax;

	if (iSpeed < 0)
		iRet = TRAIN_BACK;
	else if (iSpeed == 0)
		iRet = TRAIN_NEUTRAL;
	else if (fSpeed < 0.33f)
		iRet = TRAIN_SLOW;
	else if (fSpeed < 0.66f)
		iRet = TRAIN_MEDIUM;
	else
		iRet = TRAIN_FAST;

	return iRet;
}

void CBasePlayer :: DeathSound( void )
{
	// temporarily using pain sounds for death sounds
	switch (RANDOM_LONG(1,5))
	{
	case 1:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain5.wav", 1, ATTN_NORM);
		break;
	case 2:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain6.wav", 1, ATTN_NORM);
		break;
	case 3:
		EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_pain7.wav", 1, ATTN_NORM);
		break;
	}

	// play one of the suit death alarms
	EMIT_GROUPNAME_SUIT(ENT(pev), "HEV_DEAD");
}

int CBasePlayer :: TakeHealth( float flHealth, int bitsDamageType )
{
	return CBaseMonster :: TakeHealth (flHealth, bitsDamageType);
}

Vector CBasePlayer :: GetGunPosition( )
{
	return pev->origin + pev->view_ofs;
}

void CBasePlayer :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if ( pev->takedamage )
	{
		m_LastHitGroup = ptr->iHitgroup;

		switch ( ptr->iHitgroup )
		{
		case HITGROUP_GENERIC:
			break;
		case HITGROUP_HEAD:
			flDamage *= gSkillData.plrHead;
			break;
		case HITGROUP_CHEST:
			flDamage *= gSkillData.plrChest;
			break;
		case HITGROUP_STOMACH:
			flDamage *= gSkillData.plrStomach;
			break;
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			flDamage *= gSkillData.plrArm;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			flDamage *= gSkillData.plrLeg;
			break;
		default:
			break;
		}

		SpawnBlood(ptr->vecEndPos, BloodColor(), flDamage);// a little surface blood.
		TraceBleed( flDamage, vecDir, ptr, bitsDamageType );
		AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
	}
}

#define ARMOR_RATIO	 0.2	// Armor Takes 80% of the damage
#define ARMOR_BONUS  0.5	// Each Point of Armor is work 1/x points of health

#ifdef NON_MATCHING
/* não casa com nem sem -fmove-all-movables (alocação de registradores e ordem do cálculo de armadura) */
int CBasePlayer :: TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	// have suit diagnose the problem - ie: report damage type
	int bitsDamage = bitsDamageType;
	int ffound = TRUE;
	int fmajor;
	int fcritical;
	int fTookDamage;
	int ftrivial;
	float flRatio;
	float flBonus;
	float flHealthPrev = pev->health;
	int iRumble = (int)( flDamage * 2.55f ) + 30;

	flBonus = ARMOR_BONUS;
	flRatio = ARMOR_RATIO;

	// PS2: o parceiro da IA reage ao dano
	if ( pev->flags & FL_FAKECLIENT )
		Ps2AITakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );

	if ( ( bitsDamageType & DMG_BLAST ) && g_pGameRules->Ps2Virt07() )
	{
		// blasts damage armor more.
		flBonus *= 2;
	}

	// Already dead
	if ( !IsAlive() )
		return 0;
	// go take the damage first

	CBaseEntity *pAttacker = CBaseEntity::Instance(pevAttacker);

	// PS2: não se morre no mapa de treino
	if ( ( pev->health < 10 || pev->health <= flDamage ) && !strncmp( STRING(gpGlobals->mapname), "t0", 2 ) )
		return 0;

	if ( !g_pGameRules->FPlayerCanTakeDamage( this, pAttacker ) )
	{
		// Refuse the damage
		return 0;
	}

	// keep track of amount of damage last sustained
	m_lastDamageAmount = flDamage;

	// Armor.
	if (pev->armorvalue && !(bitsDamageType & (DMG_FALL | DMG_DROWN)) )// armor doesn't protect against fall or drown damage!
	{
		float flNew = flDamage * flRatio;

		float flArmor;
		int iArmorTaken;

		flArmor = (flDamage - flNew) * flBonus;

		// Does this use more armor than we have?
		if (flArmor > pev->armorvalue)
		{
			flArmor = pev->armorvalue;
			flArmor *= (1/flBonus);
			flNew = flDamage - flArmor;
			pev->armorvalue = 0;
			iArmorTaken = flNew;
		}
		else
		{
			pev->armorvalue -= flArmor;
			iArmorTaken = flArmor;
		}
		m_ps2Stat5F8 += iArmorTaken;

		flDamage = flNew;
	}

	// PS2: estatística do dano recebido
	m_ps2Stat5F8 += ( (int)flDamage < (int)pev->health ) ? (int)flDamage : (int)pev->health;

	if ( g_pGameRules->IsMultiplayer() || g_pGameRules->Ps2Virt08() )
	{
		if ( g_pGameRules->IsMultiplayer() && cl_numlocal == 1 )
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( m_iTeamNum );
				WRITE_SHORT( pev->frags );
				WRITE_SHORT( m_iDeaths );
				WRITE_SHORT( m_ps2Stat5F8 );
				WRITE_SHORT( m_ps2ShotsFired );
				WRITE_SHORT( m_ps2ShotsHit );
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( ENTINDEX(edict()) );
				WRITE_SHORT( pev->frags );
				WRITE_SHORT( m_iDeaths );
				WRITE_SHORT( m_ps2Stat5F8 );
				WRITE_SHORT( m_ps2ShotsFired );
				WRITE_SHORT( m_ps2ShotsHit );
			MESSAGE_END();
		}
	}

	// this cast to INT is critical!!! If a player ends up with 0.5 health, the engine will get that
	// as an int (zero) and think the player is dead! (this will incite a clientside screentilt, etc)
	fTookDamage = CBaseMonster::TakeDamage(pevInflictor, pevAttacker, (int)flDamage, bitsDamageType);

	// PS2: vibração do controle
	if ( fTookDamage )
		func_002071C8( this, 1, iRumble, 50 );

	// reset damage time countdown for each type of time based damage player just sustained

	{
		for (int i = 0; i < CDMG_TIMEBASED; i++)
			if (bitsDamageType & (DMG_PARALYZE << i))
				m_rgbTimeBasedDamage[i] = 0;
	}

	// how bad is it, doc?

	ftrivial = (pev->health > 75 || m_lastDamageAmount < 5);
	fmajor = (m_lastDamageAmount > 25);
	fcritical = (pev->health < 30);

	m_bitsDamageType |= bitsDamage; // Save this so we can report it to the client
	m_bitsHUDDamage = -1;  // make sure the damage bits get resent

	while (fTookDamage && (!ftrivial || (bitsDamage & DMG_TIMEBASED)) && ffound && bitsDamage)
	{
		ffound = FALSE;

		if (bitsDamage & DMG_CLUB)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG4", FALSE, SUIT_NEXT_IN_30SEC);	// minor fracture
			bitsDamage &= ~DMG_CLUB;
			ffound = TRUE;
		}
		if (bitsDamage & (DMG_FALL | DMG_CRUSH))
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG5", FALSE, SUIT_NEXT_IN_30SEC);	// major fracture
			else
				SetSuitUpdate("!HEV_DMG4", FALSE, SUIT_NEXT_IN_30SEC);	// minor fracture

			bitsDamage &= ~(DMG_FALL | DMG_CRUSH);
			ffound = TRUE;
		}

		if (bitsDamage & DMG_BULLET)
		{
			if (m_lastDamageAmount > 5)
				SetSuitUpdate("!HEV_DMG6", FALSE, SUIT_NEXT_IN_30SEC);	// blood loss detected

			bitsDamage &= ~DMG_BULLET;
			ffound = TRUE;
		}

		if (bitsDamage & DMG_SLASH)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG1", FALSE, SUIT_NEXT_IN_30SEC);	// major laceration
			else
				SetSuitUpdate("!HEV_DMG0", FALSE, SUIT_NEXT_IN_30SEC);	// minor laceration

			bitsDamage &= ~DMG_SLASH;
			ffound = TRUE;
		}

		if (bitsDamage & DMG_SONIC)
		{
			if (fmajor)
				SetSuitUpdate("!HEV_DMG2", FALSE, SUIT_NEXT_IN_1MIN);	// internal bleeding
			bitsDamage &= ~DMG_SONIC;
			ffound = TRUE;
		}

		if (bitsDamage & (DMG_POISON | DMG_PARALYZE))
		{
			SetSuitUpdate("!HEV_DMG3", FALSE, SUIT_NEXT_IN_1MIN);	// blood toxins detected
			bitsDamage &= ~(DMG_POISON | DMG_PARALYZE);
			ffound = TRUE;
		}

		if (bitsDamage & DMG_ACID)
		{
			SetSuitUpdate("!HEV_DET1", FALSE, SUIT_NEXT_IN_1MIN);	// hazardous chemicals detected
			bitsDamage &= ~DMG_ACID;
			ffound = TRUE;
		}

		if (bitsDamage & DMG_NERVEGAS)
		{
			SetSuitUpdate("!HEV_DET0", FALSE, SUIT_NEXT_IN_1MIN);	// biohazard detected
			bitsDamage &= ~DMG_NERVEGAS;
			ffound = TRUE;
		}

		if (bitsDamage & DMG_RADIATION)
		{
			SetSuitUpdate("!HEV_DET2", FALSE, SUIT_NEXT_IN_1MIN);	// radiation detected
			bitsDamage &= ~DMG_RADIATION;
			ffound = TRUE;
		}
		if (bitsDamage & DMG_SHOCK)
		{
			bitsDamage &= ~DMG_SHOCK;
			ffound = TRUE;
		}
	}

	pev->punchangle.x = -2;

	if (fTookDamage && !ftrivial && fmajor && flHealthPrev >= 75)
	{
		// first time we take major damage...
		// turn automedic on if not on
		SetSuitUpdate("!HEV_MED1", FALSE, SUIT_NEXT_IN_30MIN);	// automedic on

		// give morphine shot if not given recently
		SetSuitUpdate("!HEV_HEAL7", FALSE, SUIT_NEXT_IN_30MIN);	// morphine shot
	}

	if (fTookDamage && !ftrivial && fcritical && flHealthPrev < 75)
	{

		// already took major damage, now it's critical...
		if (pev->health < 6)
			SetSuitUpdate("!HEV_HLTH3", FALSE, SUIT_NEXT_IN_10MIN);	// near death
		else if (pev->health < 20)
			SetSuitUpdate("!HEV_HLTH2", FALSE, SUIT_NEXT_IN_10MIN);	// health critical

		// give critical health warnings
		if (!RANDOM_LONG(0,3) && flHealthPrev < 50)
			SetSuitUpdate("!HEV_DMG7", FALSE, SUIT_NEXT_IN_5MIN); //seek medical attention
	}

	// if we're taking time based damage, warn about its continuing effects
	if (fTookDamage && (bitsDamageType & DMG_TIMEBASED) && flHealthPrev < 75)
		{
			if (flHealthPrev < 50)
			{
				if (!RANDOM_LONG(0,3))
					SetSuitUpdate("!HEV_DMG7", FALSE, SUIT_NEXT_IN_5MIN); //seek medical attention
			}
			else
				SetSuitUpdate("!HEV_HLTH1", FALSE, SUIT_NEXT_IN_10MIN);	// health dropping
		}

	return fTookDamage;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/player", TakeDamage__11CBasePlayerP9entvars_sT1fi);
#endif

//=========================================================
// PackDeadPlayerItems - call this when a player dies to
// pack up the appropriate weapons and ammo items, and to
// destroy anything that shouldn't be packed.
//
// This is pretty brute force :(
//=========================================================
void CBasePlayer::PackDeadPlayerItems( void )
{
	int iWeaponRules;
	int iAmmoRules;
	int i;
	CBasePlayerWeapon *rgpPackWeapons[ 20 ];// 20 hardcoded for now. How to determine exactly how many weapons we have?
	int iPackAmmo[ MAX_AMMO_SLOTS + 1];
	int iPW = 0;// index into packweapons array
	int iPA = 0;// index into packammo array

	memset(rgpPackWeapons, NULL, sizeof(rgpPackWeapons) );
	memset(iPackAmmo, -1, sizeof(iPackAmmo) );

	// get the game rules
	iWeaponRules = g_pGameRules->DeadPlayerWeapons( this );
 	iAmmoRules = g_pGameRules->DeadPlayerAmmo( this );

	if ( iWeaponRules == GR_PLR_DROP_GUN_NO && iAmmoRules == GR_PLR_DROP_AMMO_NO )
	{
		// nothing to pack. Remove the weapons and return. Don't call create on the box!
		RemoveAllItems( TRUE );
		return;
	}

// go through all of the weapons and make a list of the ones to pack
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerItem *pPlayerItem = m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			{
				switch( iWeaponRules )
				{
				case GR_PLR_DROP_GUN_ACTIVE:
					if ( m_pActiveItem && pPlayerItem == m_pActiveItem )
					{
						// this is the active item. Pack it.
						rgpPackWeapons[ iPW++ ] = (CBasePlayerWeapon *)pPlayerItem;
					}
					break;

				case GR_PLR_DROP_GUN_ALL:
					rgpPackWeapons[ iPW++ ] = (CBasePlayerWeapon *)pPlayerItem;
					break;

				default:
					break;
				}

				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}

// now go through ammo and make a list of which types to pack.
	if ( iAmmoRules != GR_PLR_DROP_AMMO_NO )
	{
		for ( i = 0 ; i < MAX_AMMO_SLOTS ; i++ )
		{
			if ( m_rgAmmo[ i ] > 0 )
			{
				// player has some ammo of this type.
				switch ( iAmmoRules )
				{
				case GR_PLR_DROP_AMMO_ALL:
					iPackAmmo[ iPA++ ] = i;
					break;

				case GR_PLR_DROP_AMMO_ACTIVE:
					if ( m_pActiveItem && i == m_pActiveItem->PrimaryAmmoIndex() )
					{
						// this is the primary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					else if ( m_pActiveItem && i == m_pActiveItem->SecondaryAmmoIndex() )
					{
						// this is the secondary ammo type for the active weapon
						iPackAmmo[ iPA++ ] = i;
					}
					break;

				default:
					break;
				}
			}
		}
	}

// create a box to pack the stuff into.
	CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create( "weaponbox", pev->origin, pev->angles, edict() );

	pWeaponBox->pev->angles.x = 0;// don't let weaponbox tilt.
	pWeaponBox->pev->angles.z = 0;

	pWeaponBox->SetThink( &CWeaponBox::Kill );
	pWeaponBox->pev->nextthink = gpGlobals->time + 120;

// back these two lists up to their first elements
	iPA = 0;
	iPW = 0;

// pack the ammo
	while ( iPackAmmo[ iPA ] != -1 )
	{
		pWeaponBox->PackAmmo( MAKE_STRING( CBasePlayerItem::AmmoInfoArray[ iPackAmmo[ iPA ] ].pszName ), m_rgAmmo[ iPackAmmo[ iPA ] ] );
		iPA++;
	}

// now pack all of the items in the lists
	while ( rgpPackWeapons[ iPW ] )
	{
		// weapon unhooked from the player. Pack it into der box.
		pWeaponBox->PackWeapon( rgpPackWeapons[ iPW ] );

		iPW++;
	}

	pWeaponBox->pev->velocity = pev->velocity * 1.2f;// weaponbox has player's velocity, then some.

	RemoveAllItems( TRUE );// now strip off everything that wasn't handled by the code above.
}

void CBasePlayer::RemoveAllItems( BOOL removeSuit )
{
	// PS2: no modo vortigaunt as armas ficam
	if ( !removeSuit && ( m_iHideHUD & HIDEHUD_PS2_NOPICKUP ) )
		return;

	if (m_pActiveItem)
	{
		ResetAutoaim( );
		m_pActiveItem->Holster( );
		m_pActiveItem = NULL;
	}

	m_pLastItem = NULL;

	int i;
	CBasePlayerItem *pPendingItem;
	for (i = 0; i < MAX_ITEM_TYPES; i++)
	{
		m_pActiveItem = m_rgpPlayerItems[i];
		while (m_pActiveItem)
		{
			pPendingItem = m_pActiveItem->m_pNext;
			m_pActiveItem->Drop( );
			m_pActiveItem = pPendingItem;
		}
		m_rgpPlayerItems[i] = NULL;
	}
	m_pActiveItem = NULL;

	pev->viewmodel		= 0;
	pev->weaponmodel	= 0;

	if ( removeSuit )
		pev->weapons = 0;
	else
		pev->weapons &= ~WEAPON_ALLWEAPONS;

	for ( i = 0; i < MAX_AMMO_SLOTS;i++)
		m_rgAmmo[i] = 0;

	UpdateClientData();
	// send Selected Weapon Message to our client
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0);
		WRITE_BYTE(0);
	MESSAGE_END();
}

/*
 * GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
 *
 * ENTITY_METHOD(PlayerDie)
 */
/* sem inicializador: vai para o fim do .data (0x317D78) */
BOOL gEvilImpulse101;

entvars_t *g_pevLastInflictor;  // Set in combat.cpp.  Used to pass the damage inflictor for death messages.
								// Better solution:  Add as parameter to all Killed() functions.

void CBasePlayer::Killed( entvars_t *pevAttacker, int iGib )
{
	CSound *pSound;

	// PS2: o parceiro da IA
	if ( pev->flags & FL_FAKECLIENT )
		Ps2AIKilled();

	g_pGameRules->PlayerKilled( this, pevAttacker, g_pevLastInflictor );

	if ( m_pTank != NULL )
	{
		m_pTank->Use( this, this, USE_OFF, 0 );
		m_pTank = NULL;
	}

	// this client isn't going to be thinking for a while, so reset the sound until they respawn
	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( edict() ) );
	{
		if ( pSound )
		{
			pSound->Reset();
		}
	}

	SetAnimation( PLAYER_DIE );

	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes

	pev->view_ofs		= Vector(0,0,-8);
	pev->deadflag		= DEAD_DYING;
	pev->solid			= SOLID_NOT;
	pev->movetype		= MOVETYPE_TOSS;
	ClearBits( pev->flags, FL_ONGROUND );
	if (pev->velocity.z < 10)
		pev->velocity.z += RANDOM_FLOAT(0,300);

	// clear out the suit message cache so we don't keep chattering
	SetSuitUpdate(NULL, FALSE, 0);

	// send "health" update message to zero
	m_iClientHealth = 0;
	MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
		WRITE_BYTE( m_iClientHealth );
	MESSAGE_END();

	// Tell Ammo Hud that the player is dead
	MESSAGE_BEGIN( MSG_ONE, gmsgCurWeapon, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(0XFF);
		WRITE_BYTE(0xFF);
	MESSAGE_END();

	// reset FOV
	m_iFOV = m_iClientFOV = 0;

	MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, pev );
		WRITE_BYTE(0);
	MESSAGE_END();

	if ( ( pev->health < -40 && iGib != GIB_NEVER ) || iGib == GIB_ALWAYS )
	{
		GibMonster();	// This clears pev->model
		pev->effects |= EF_NODRAW;
		return;
	}

	DeathSound();

	pev->angles.x = 0;
	pev->angles.z = 0;

	SetThink(&CBasePlayer::PlayerDeathThink);
	pev->nextthink = gpGlobals->time + 0.1f;

	// PS2: avisa a interface que o jogador morreu
	intf_playerdead.value = 1;
}

void CBasePlayer::SetAnimation( PLAYER_ANIM playerAnim )
{
	int animDesired;
	float speed;
	char szAnim[64];

	speed = sqrtf( pev->velocity.x * pev->velocity.x + pev->velocity.y * pev->velocity.y );

	if (pev->flags & FL_FROZEN)
	{
		speed = 0;
		playerAnim = PLAYER_IDLE;
	}

	switch (playerAnim) 
	{
	case PLAYER_JUMP:
		m_IdealActivity = ACT_HOP;
		break;
	
	case PLAYER_SUPERJUMP:
		m_IdealActivity = ACT_LEAP;
		break;
	
	case PLAYER_DIE:
		m_IdealActivity = ACT_DIESIMPLE;
		m_IdealActivity = GetDeathActivity( );
		break;

	case PLAYER_ATTACK1:	
		switch( m_Activity )
		{
		case ACT_HOVER:
		case ACT_SWIM:
		case ACT_HOP:
		case ACT_LEAP:
		case ACT_DIESIMPLE:
			m_IdealActivity = m_Activity;
			break;
		default:
			m_IdealActivity = ACT_RANGE_ATTACK1;
			break;
		}
		break;

	case PLAYER_PS2_ATTACK2:	
		switch( m_Activity )
		{
		case ACT_HOVER:
		case ACT_SWIM:
		case ACT_HOP:
		case ACT_LEAP:
		case ACT_DIESIMPLE:
			m_IdealActivity = m_Activity;
			break;
		default:
			m_IdealActivity = ACT_RANGE_ATTACK2;
			break;
		}
		break;

	case PLAYER_IDLE:
	case PLAYER_WALK:
		if ( !FBitSet( pev->flags, FL_ONGROUND ) && (m_Activity == ACT_HOP || m_Activity == ACT_LEAP) )	// Still jumping
		{
			m_IdealActivity = m_Activity;
		}
		else if ( pev->waterlevel > 1 )
		{
			if ( speed == 0 )
				m_IdealActivity = ACT_HOVER;
			else
				m_IdealActivity = ACT_SWIM;
		}
		else
		{
			m_IdealActivity = ACT_WALK;
		}
		break;
	}

	switch (m_IdealActivity)
	{
	case ACT_HOVER:
	case ACT_LEAP:
	case ACT_SWIM:
	case ACT_HOP:
	case ACT_DIESIMPLE:
	default:
		if ( m_Activity == m_IdealActivity)
			return;
		m_Activity = m_IdealActivity;

		animDesired = LookupActivity( m_Activity );
		// Already using the desired animation?
		if (pev->sequence == animDesired)
			return;

		pev->gaitsequence = 0;
		pev->sequence		= animDesired;
		pev->frame			= 0;
		ResetSequenceInfo( );
		return;

	case ACT_RANGE_ATTACK1:
		if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
			strcpy( szAnim, "crouch_shoot_" );
		else
			strcpy( szAnim, "ref_shoot_" );
		strcat( szAnim, m_szAnimExtention );
		animDesired = LookupSequence( szAnim );
		if (animDesired == -1)
			animDesired = 0;

		if ( pev->sequence != animDesired || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		if (!m_fSequenceLoops)
		{
			pev->effects |= EF_NOINTERP;
		}

		m_Activity = m_IdealActivity;

		pev->sequence		= animDesired;
		ResetSequenceInfo( );
		break;

	case ACT_RANGE_ATTACK2:
		if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
			strcpy( szAnim, "crouch_shoot_mp5" );
		else
			strcpy( szAnim, "ref_shoot_mp5" );
		animDesired = LookupSequence( szAnim );
		if (animDesired == -1)
			animDesired = 0;

		if ( pev->sequence != animDesired || !m_fSequenceLoops )
		{
			pev->frame = 0;
		}

		if (!m_fSequenceLoops)
		{
			pev->effects |= EF_NOINTERP;
		}

		m_Activity = m_IdealActivity;

		pev->sequence		= animDesired;
		ResetSequenceInfo( );
		break;

	case ACT_WALK:
		if (m_Activity != ACT_RANGE_ATTACK1 || m_fSequenceFinished)
		{
			if ( FBitSet( pev->flags, FL_DUCKING ) )	// crouching
				strcpy( szAnim, "crouch_aim_" );
			else
				strcpy( szAnim, "ref_aim_" );
			strcat( szAnim, m_szAnimExtention );
			animDesired = LookupSequence( szAnim );
			if (animDesired == -1)
				animDesired = 0;
			m_Activity = ACT_WALK;
		}
		else
		{
			animDesired = pev->sequence;
		}
	}

	if ( FBitSet( pev->flags, FL_DUCKING ) )
	{
		if ( speed == 0)
			pev->gaitsequence	= LookupActivity( ACT_CROUCHIDLE );
		else
			pev->gaitsequence	= LookupActivity( ACT_CROUCH );
	}
	else if ( speed > 220 )
	{
		pev->gaitsequence	= LookupActivity( ACT_RUN );
		MESSAGE_BEGIN( MSG_ONE, gmsgCrouchState, NULL, ENT(pev) );
			WRITE_BYTE( 1 );
		MESSAGE_END();
	}
	else if (speed > 0)
	{
		pev->gaitsequence	= LookupActivity( ACT_WALK );
		MESSAGE_BEGIN( MSG_ONE, gmsgCrouchState, NULL, ENT(pev) );
			WRITE_BYTE( 0 );
		MESSAGE_END();
	}
	else
	{
		pev->gaitsequence	= LookupSequence( "deep_idle" );
		MESSAGE_BEGIN( MSG_ONE, gmsgCrouchState, NULL, ENT(pev) );
			WRITE_BYTE( 0 );
		MESSAGE_END();
	}

	// Already using the desired animation?
	if (pev->sequence == animDesired)
		return;

	pev->sequence		= animDesired;
	pev->frame			= 0;
	ResetSequenceInfo( );
}

/*
===========
WaterMove
============
*/
#define AIRTIME	12		// lung full of air lasts this many seconds

#define itbd_DrownRecover	4

void CBasePlayer::WaterMove()
{
	int air;

	if (pev->movetype == MOVETYPE_NOCLIP)
		return;

	if (pev->health < 0)
		return;

	if (pev->waterlevel != 3) 
	{
		// not underwater
		
		// play 'up for air' sound
		if (pev->air_finished < gpGlobals->time)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade1.wav", 1, ATTN_NORM);
		else if (pev->air_finished < gpGlobals->time + 9)
			EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_wade2.wav", 1, ATTN_NORM);

		pev->air_finished = gpGlobals->time + AIRTIME;
		pev->dmg = 2;

		// if we took drowning damage, give it back slowly
		if (m_idrowndmg > m_idrownrestored)
		{
			m_bitsDamageType |= DMG_DROWNRECOVER;
			m_bitsDamageType &= ~DMG_DROWN;
			m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;
		}

	}
	else
	{	// fully under water
		// stop restoring damage while underwater
		m_bitsDamageType &= ~DMG_DROWNRECOVER;
		m_rgbTimeBasedDamage[itbd_DrownRecover] = 0;

		if (pev->air_finished < gpGlobals->time)		// drown!
		{
			if (pev->pain_finished < gpGlobals->time)
			{
				// take drowning damage
				pev->dmg += 1;
				if (pev->dmg > 5)
					pev->dmg = 5;
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), pev->dmg, DMG_DROWN);
				pev->pain_finished = gpGlobals->time + 1;
				
				// track drowning damage, give it back when
				// player finally takes a breath

				m_idrowndmg += (int)pev->dmg;
			} 
		}
		else
		{
			m_bitsDamageType &= ~DMG_DROWN;
		}
	}

	if (!pev->waterlevel)
	{
		if (FBitSet(pev->flags, FL_INWATER))
		{       
			switch (RANDOM_LONG(0,3))
			{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade2.wav", 1, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade3.wav", 1, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade4.wav", 1, ATTN_NORM); break;
			}

			ClearBits(pev->flags, FL_INWATER);
		}
		return;
	}
	
	// make bubbles

	air = (int)(pev->air_finished - gpGlobals->time);
	if (!RANDOM_LONG(0,0x1f) && RANDOM_LONG(0,AIRTIME-1) >= air)
	{
		switch (RANDOM_LONG(0,3))
		{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim1.wav", 0.8f, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim2.wav", 0.8f, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim3.wav", 0.8f, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_swim4.wav", 0.8f, ATTN_NORM); break;
		}
	}

	if (pev->watertype == CONTENTS_LAVA)		// do damage
	{
		if (pev->dmgtime < gpGlobals->time)
			TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 10 * pev->waterlevel, DMG_BURN);
	}
	else if (pev->watertype == CONTENTS_SLIME)		// do damage
	{
		pev->dmgtime = gpGlobals->time + 1;
		TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), 4 * pev->waterlevel, DMG_ACID);
	}
	
	if (!FBitSet(pev->flags, FL_INWATER))
	{
		if (pev->watertype == CONTENTS_WATER)
		{
			switch (RANDOM_LONG(0,3))
			{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade2.wav", 1, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade3.wav", 1, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade4.wav", 1, ATTN_NORM); break;
			}
		}

		SetBits(pev->flags, FL_INWATER);
		pev->dmgtime = 0;
	}
}


// TRUE if the player is attached to a ladder
BOOL CBasePlayer::IsOnLadder( void )
{ 
	return ( pev->movetype == MOVETYPE_FLY );
}

void CBasePlayer::CheckWaterJump()
{
	if ( IsOnLadder() )
		return;

	Vector vecStart = pev->origin;
	vecStart.z += 8;

	UTIL_MakeVectors(pev->angles);
	gpGlobals->v_forward.z = 0;
	gpGlobals->v_forward = gpGlobals->v_forward.Normalize();

	Vector vecEnd = vecStart + gpGlobals->v_forward * 24;

	TraceResult tr;
	UTIL_TraceLine(vecStart, vecEnd, ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction < 1)
	{
		vecStart.z += pev->maxs.z - 8;
		vecEnd = vecStart + gpGlobals->v_forward * 24;
		pev->movedir = tr.vecPlaneNormal * -50;
		UTIL_TraceLine(vecStart, vecEnd, ignore_monsters, ENT(pev), &tr);
		if (tr.flFraction == 1)
		{
			SetBits(pev->flags, FL_WATERJUMP);
			pev->velocity.z = 225;
			pev->teleport_time = gpGlobals->time + 2;
		}
	}
}

void CBasePlayer::PlayerDeathThink(void)
{
	float flForward;

	if (FBitSet(pev->flags, FL_ONGROUND))
	{
		flForward = pev->velocity.Length() - 20;
		if (flForward <= 0)
			pev->velocity = g_vecZero;
		else    
			pev->velocity = flForward * pev->velocity.Normalize();
	}

	if ( HasWeapons() )
	{
		PackDeadPlayerItems();
	}

	if (pev->modelindex && (!m_fSequenceFinished) && (pev->deadflag == DEAD_DYING))
	{
		StudioFrameAdvance( );

		m_iRespawnFrames++;
		if ( m_iRespawnFrames < 60 )
			return;
	}

	if ( pev->deadflag == DEAD_DYING )
		pev->deadflag = DEAD_DEAD;
	
	StopAnimation();

	pev->effects |= EF_NOINTERP;
	pev->framerate = 0.0;

	BOOL fAnyButtonDown = pev->button;
	
	// wait for all buttons released
	if (pev->deadflag == DEAD_DEAD)
	{
		if (fAnyButtonDown && gpGlobals->time < m_fDeadTime + 2)
			return;

		if ( g_pGameRules->FPlayerCanRespawn( this ) )
		{
			m_fDeadTime = gpGlobals->time;
			pev->deadflag = DEAD_RESPAWNABLE;
		}
		
		return;
	}

	if ( g_pGameRules->Ps2Virt07() && ( gpGlobals->time > (m_fDeadTime + 6) ) &&  !(m_afPhysicsFlags & PFLAG_OBSERVER) )
	{
		// go to dead camera. 
		StartDeathCam();
	}

	if (!fAnyButtonDown 
		&& !( g_pGameRules->Ps2Virt07() && forcerespawn.value > 0 && (gpGlobals->time > (m_fDeadTime + 5))) )
		return;

	pev->button = 0;
	m_iRespawnFrames = 0;

	respawn(pev, !(m_afPhysicsFlags & PFLAG_OBSERVER) );// don't copy a corpse if we're in deathcam.
	pev->nextthink = -1;
}

//=========================================================
// StartDeathCam - find an intermission spot and send the
// player off into observer mode
//=========================================================
void CBasePlayer::StartDeathCam( void )
{
	edict_t *pSpot, *pNewSpot;
	int iRand;

	if ( pev->view_ofs == g_vecZero )
	{
		// don't accept subsequent attempts to StartDeathCam()
		return;
	}

	pSpot = FIND_ENTITY_BY_CLASSNAME( NULL, "info_intermission");	

	if ( !FNullEnt( pSpot ) )
	{
		// at least one intermission spot in the world.
		iRand = RANDOM_LONG( 0, 3 );

		while ( iRand > 0 )
		{
			pNewSpot = FIND_ENTITY_BY_CLASSNAME( pSpot, "info_intermission");
			
			if ( pNewSpot )
			{
				pSpot = pNewSpot;
			}

			iRand--;
		}

		CopyToBodyQue( pev );
		StartObserver( pSpot->v.origin, pSpot->v.v_angle );
	}
	else
	{
		// no intermission spot. Push them up in the air, looking down at their corpse
		TraceResult tr;
		CopyToBodyQue( pev );
		UTIL_TraceLine( pev->origin, pev->origin + Vector( 0, 0, 128 ), ignore_monsters, edict(), &tr );
		StartObserver( tr.vecEndPos, UTIL_VecToAngles( tr.vecEndPos - pev->origin  ) );
		return;
	}
}

void CBasePlayer::StartObserver( Vector vecPosition, Vector vecViewAngle )
{
	m_afPhysicsFlags |= PFLAG_OBSERVER;

	pev->view_ofs = g_vecZero;
	pev->angles = pev->v_angle = vecViewAngle;
	pev->fixangle = TRUE;
	pev->solid = SOLID_NOT;
	pev->takedamage = DAMAGE_NO;
	pev->movetype = MOVETYPE_NONE;
	pev->modelindex = 0;
	UTIL_SetOrigin( pev, vecPosition );
}

// 
// PlayerUse - handles USE keypress
//
#define	PLAYER_SEARCH_RADIUS	(float)64

#ifdef NON_MATCHING
/* casa só com -fmove-all-movables (o alvo iça constantes do laço); o arquivo compila sem a flag */
void CBasePlayer::PlayerUse ( void )
{
	// Was use pressed or released?
	if ( ! ((pev->button | m_afButtonPressed | m_afButtonReleased) & IN_USE) )
		return;

	// Hit Use on a train?
	if ( m_afButtonPressed & IN_USE )
	{
		if ( m_pTank != NULL )
		{
			// Stop controlling the tank
			// TODO: Send HUD Update
			m_pTank->Use( this, this, USE_OFF, 0 );
			m_pTank = NULL;
			return;
		}
		else
		{
			if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
			{
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
			else
			{	// Start controlling the train!
				CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );

				if ( pTrain && !(pev->button & IN_JUMP) && FBitSet(pev->flags, FL_ONGROUND) && (pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) && pTrain->OnControls(pev) )
				{
					m_afPhysicsFlags |= PFLAG_ONTRAIN;
					m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
					m_iTrain |= TRAIN_NEW;
					EMIT_SOUND( ENT(pev), CHAN_ITEM, "plats/train_use1.wav", 0.8f, ATTN_NORM);
					return;
				}
			}
		}
	}

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector		vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing
	
	while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
	{

		if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			vecLOS = (VecBModelOrigin( pObject->pev ) - (pev->origin + pev->view_ofs));
			
			vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5f );
			
			flDot = DotProduct (vecLOS , gpGlobals->v_forward);
			if (flDot > flMaxDot )
			{// only if the item is in front of the user
				pClosest = pObject;
				flMaxDot = flDot;
			}
		}
	}
	pObject = pClosest;

	// Found an object
	if (pObject )
	{
		//!!!UNDONE: traceline here to prevent USEing buttons through walls			
		int caps = pObject->ObjectCaps();

		if ( m_afButtonPressed & IN_USE )
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_select.wav", 0.4f, ATTN_NORM);

		if ( ( (pev->button & IN_USE) && (caps & FCAP_CONTINUOUS_USE) ) ||
			 ( (m_afButtonPressed & IN_USE) && (caps & (FCAP_IMPULSE_USE|FCAP_ONOFF_USE)) ) )
		{
			if ( caps & FCAP_CONTINUOUS_USE )
				m_afPhysicsFlags |= PFLAG_USING;

			pObject->Use( this, this, USE_SET, 1 );
		}
		// UNDONE: Send different USE codes for ON/OFF.  Cache last ONOFF_USE object to send 'off' if you turn away
		else if ( (m_afButtonReleased & IN_USE) && (pObject->ObjectCaps() & FCAP_ONOFF_USE) )	// BUGBUG This is an "off" use
		{
			pObject->Use( this, this, USE_SET, 0 );
		}
	}
	else
	{
		if ( m_afButtonPressed & IN_USE )
			EMIT_SOUND( ENT(pev), CHAN_ITEM, "common/wpn_denyselect.wav", 0.4f, ATTN_NORM);
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/player", PlayerUse__11CBasePlayer);
#endif

// PS2: procura o objeto usável à frente do jogador para realçá-lo
void CBasePlayer::Ps2FindUseable( void )
{
	if ( gpGlobals->time < m_flNextUseableCheck )
		return;

	m_flNextUseableCheck = gpGlobals->time + 0.2f;

	CBaseEntity *pObject = NULL;
	CBaseEntity *pClosest = NULL;
	Vector		vecLOS;
	float flMaxDot = VIEW_FIELD_NARROW;
	float flDot;

	UTIL_MakeVectors ( pev->v_angle );// so we know which way we are facing
	
	while ((pObject = UTIL_FindEntityInSphere( pObject, pev->origin, PLAYER_SEARCH_RADIUS )) != NULL)
	{
		if (pObject->ObjectCaps() & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			if ( pObject->IsPlayer() )
				continue;
			if ( pObject->MyMonsterPointer() )
				continue;
			if ( !strcmp( STRING(pObject->pev->classname), "func_pushable" ) )
				continue;
			if ( pObject->pev->spawnflags & 0x200 )
				continue;

			Vector vecOrigin = pObject->pev->origin;
			BOOL fBModel = FALSE;

			if ( pObject->IsBSPModel() )
				fBModel = TRUE;
			else
			{
				const char *pszModel = STRING(pObject->pev->model);
				if ( pszModel && *pszModel == '*' )
					fBModel = TRUE;
			}

			if ( fBModel == TRUE )
				vecOrigin = VecBModelOrigin( pObject->pev );

			vecLOS = (vecOrigin - (pev->origin + pev->view_ofs));
			
			vecLOS = UTIL_ClampVectorToBox( vecLOS, pObject->pev->size * 0.5f );
			
			flDot = DotProduct (vecLOS , gpGlobals->v_forward);
			if (flDot > flMaxDot )
			{
				pClosest = pObject;
				flMaxDot = flDot;
			}
		}
	}

	if ( pClosest )
	{
		Ps2HighlightUseable( pClosest );
	}
	else if ( m_hUseableHighlight.Get() )
	{
		UTIL_Remove( m_hUseableHighlight );
		m_hUseableHighlight = NULL;
		m_hLastUseableItem = NULL;
	}
}

#define PLAYER_LONGJUMP_SPEED 350 // how fast we longjump

void CBasePlayer::Jump()
{
	if (FBitSet(pev->flags, FL_WATERJUMP))
		return;
	
	if (pev->waterlevel >= 2)
	{
		// play swiming sound
		if ( m_flSwimTime < gpGlobals->time )
		{
			m_flSwimTime = gpGlobals->time + 1;
			switch (RANDOM_LONG(0,3))
			{
			case 0:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM); break;
			case 1:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade2.wav", 1, ATTN_NORM); break;
			case 2:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade3.wav", 1, ATTN_NORM); break;
			case 3:	EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade4.wav", 1, ATTN_NORM); break;
			}
		}

		return;
	}

	// If this isn't the first frame pressing the jump button, break out.
	if ( !FBitSet( m_afButtonPressed, IN_JUMP ) )
	{
		// PS2: opção de agachar com o botão de pulo
		if ( ( ENTINDEX( ENT(pev) ) == 1 && CVAR_GET_FLOAT( "sv_cjump_style_1" ) > 0 ) ||
			 ( ENTINDEX( ENT(pev) ) == 2 && CVAR_GET_FLOAT( "sv_cjump_style_2" ) > 0 ) )
		{
			if ( gpGlobals->time - m_flPs2JumpTime > 0.2f )
			{
				pev->button |= IN_DUCK;
				m_afButtonPressed |= IN_DUCK;
				Duck();
			}
		}
		return;         // don't pogo stick
	}

	if ( !(pev->flags & FL_ONGROUND) || !pev->groundentity )
	{
		// PS2: pulo duplo com o módulo de pulo longo
		if ( gpGlobals->time - m_flPs2JumpTime > 0.5f )
			return;
		if ( m_fPs2DoubleJump )
			return;
		if ( !m_fLongJump )
			return;
	}

	// many features in this function use v_forward, so makevectors now.
	UTIL_MakeVectors (pev->angles);

	ClearBits(pev->flags, FL_ONGROUND);		// don't stairwalk
	
	SetAnimation( PLAYER_JUMP );

	PlayStepSound( MapTextureTypeStepType( m_chTextureType ), 1.0 );

	if ( FBitSet(pev->flags, FL_DUCKING ) || FBitSet(m_afPhysicsFlags, PFLAG_DUCKING) )
	{
		if ( m_fLongJump && (pev->button & IN_DUCK) && gpGlobals->time - m_flDuckTime < 1 && pev->velocity.Length() > 50 )
		{// If jump pressed within a second of duck while moving, long jump!
			if ( RANDOM_LONG(0,1) )
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "player/pl_long_jump.wav", 1, ATTN_NORM);
			else
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "player/pl_long_jump.wav", 1, ATTN_NORM);

			pev->punchangle.x = -5;
			pev->velocity = gpGlobals->v_forward * (PLAYER_LONGJUMP_SPEED * 1.6);
			pev->velocity.z = sqrtf(2 * 800 * 56);	// Jump 56 units
			SetAnimation( PLAYER_SUPERJUMP );
		}
		else
		{
			pev->velocity.z = sqrtf(2 * 800 * 45);	// Jump 45 units
		}

		m_flPs2JumpTime = gpGlobals->time;
		m_fPs2DoubleJump = TRUE;
	}
	else if ( gpGlobals->time - m_flPs2JumpTime < 0.5f )
	{
		// PS2: segundo pulo
		if ( m_fLongJump && pev->velocity.Length2D() > 50 )
		{
			if ( RANDOM_LONG(0,1) )
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "player/pl_long_jump.wav", 1, ATTN_NORM);
			else
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "player/pl_long_jump.wav", 1, ATTN_NORM);

			pev->punchangle.x = -5;
			pev->velocity = gpGlobals->v_forward * (PLAYER_LONGJUMP_SPEED * 1.6);
			pev->velocity.z = sqrt(2 * 800 * 56);
			SetAnimation( PLAYER_SUPERJUMP );
		}

		m_flPs2JumpTime = gpGlobals->time;
		m_fPs2DoubleJump = TRUE;
	}
	else
	{
		pev->velocity.z = sqrtf(2 * 800 * 45);	// Jump 45 units
		m_flPs2JumpTime = gpGlobals->time;
		m_fPs2DoubleJump = FALSE;
	}

	// If you're standing on a conveyor, add it's velocity to yours (for momentum)
	entvars_t *pevGround = VARS(pev->groundentity);
	if ( pevGround && (pevGround->flags & FL_CONVEYOR) )
	{
		pev->velocity = pev->velocity + pev->basevelocity;
	}
}

// This is a glorious hack to find free space when you've crouched into some solid space
// Our crouching collisions do not work correctly for some reason and this is easier
// than fixing the problem :(
void FixPlayerCrouchStuck( edict_t *pPlayer )
{
	TraceResult trace;

	// Move up as many as 18 pixels if the player is stuck.
	for ( int i = 0; i < 18; i++ )
	{
		UTIL_TraceHull( pPlayer->v.origin, pPlayer->v.origin, dont_ignore_monsters, head_hull, pPlayer, &trace );
		if ( trace.fStartSolid )
			pPlayer->v.origin.z ++;
		else
			break;
	}
}

#define TIME_TO_DUCK	0.4f

void CBasePlayer::Duck( )
{
	float time, duckFraction;

	if (pev->button & IN_DUCK) 
	{
		if ( (m_afButtonPressed & IN_DUCK) && !(pev->flags & FL_DUCKING) )
		{
			m_flDuckTime = gpGlobals->time;
			SetBits( m_afPhysicsFlags, PFLAG_DUCKING );
		}
		time = gpGlobals->time - m_flDuckTime;

		// if we're not already done ducking
		if ( FBitSet( m_afPhysicsFlags, PFLAG_DUCKING ) )
		{
			// Finish ducking immediately if duck time is over or not on ground
			if ( time >= TIME_TO_DUCK || !(pev->flags & FL_ONGROUND) )
			{
				// HACKHACK - Fudge for collision bug - no time to fix this properly
				if ( pev->flags & FL_ONGROUND )
				{
					pev->origin = pev->origin - (VEC_DUCK_HULL_MIN - VEC_HULL_MIN);
					FixPlayerCrouchStuck( edict() );
				}

				UTIL_SetOrigin( pev, pev->origin );

				UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
				pev->view_ofs = VEC_DUCK_VIEW;
				SetBits( pev->flags, FL_DUCKING );		// Hull is duck hull
				ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );	// Done ducking (don't ease-in when the player is already ducked)
			}
			else
			{
				// Calc parametric time
				duckFraction = UTIL_SplineFraction( time, (1.0/TIME_TO_DUCK) );
				pev->view_ofs = ((VEC_DUCK_VIEW - (VEC_DUCK_HULL_MIN - VEC_HULL_MIN)) * duckFraction) + (VEC_VIEW * (1-duckFraction));
			}
		}

		MESSAGE_BEGIN( MSG_ONE, gmsgCrouchState, NULL, ENT(pev) );
			WRITE_BYTE( 2 );
		MESSAGE_END();

		SetAnimation( PLAYER_WALK );
	}
	else
	{
		// Try to unduck
		TraceResult trace;
		Vector newOrigin = pev->origin;

		if ( pev->flags & FL_ONGROUND )
			newOrigin = newOrigin + (VEC_DUCK_HULL_MIN - VEC_HULL_MIN);

		UTIL_TraceHull( newOrigin, newOrigin, dont_ignore_monsters, human_hull, ENT(pev), &trace );

		if ( !trace.fStartSolid )
		{
			ClearBits( pev->flags, FL_DUCKING );
			ClearBits( m_afPhysicsFlags, PFLAG_DUCKING );
			pev->view_ofs = VEC_VIEW;
			UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
			pev->origin = newOrigin;
		}

		MESSAGE_BEGIN( MSG_ONE, gmsgCrouchState, NULL, ENT(pev) );
			WRITE_BYTE( 0 );
		MESSAGE_END();
	}
}

//
// ID's player as such.
//
int  CBasePlayer::Classify ( void )
{
	return CLASS_PLAYER;
}


void CBasePlayer::AddPoints( int score, BOOL bAllowNegativeScore )
{
	// Positive score always adds
	if ( score < 0 )
	{
		if ( !bAllowNegativeScore )
		{
			if ( pev->frags < 0 )		// Can't go more negative
				return;
			
			if ( -score > (int)pev->frags )	// Will this go negative?
			{
				score = -pev->frags;		// Sum will be 0
			}
		}
	}

	pev->frags += score;

	if ( g_pGameRules->IsMultiplayer() && cl_numlocal == 1 )
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( m_iTeamNum );
			WRITE_SHORT( pev->frags );
			WRITE_SHORT( m_iDeaths );
			WRITE_SHORT( m_ps2Stat5F8 );
			WRITE_SHORT( m_ps2ShotsFired );
			WRITE_SHORT( m_ps2ShotsHit );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
			WRITE_BYTE( ENTINDEX(edict()) );
			WRITE_SHORT( pev->frags );
			WRITE_SHORT( m_iDeaths );
			WRITE_SHORT( m_ps2Stat5F8 );
			WRITE_SHORT( m_ps2ShotsFired );
			WRITE_SHORT( m_ps2ShotsHit );
		MESSAGE_END();
	}
}


void CBasePlayer::AddPointsToTeam( int score, BOOL bAllowNegativeScore )
{
	int index = entindex();

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *pPlayer = UTIL_PlayerByIndex( i );

		if ( pPlayer && i != index )
		{
			if ( g_pGameRules->PlayerRelationship( this, pPlayer ) == GR_TEAMMATE )
			{
				pPlayer->AddPoints( score, bAllowNegativeScore );
			}
		}
	}
}

#define STEP_CONCRETE	0		// default step sound
#define STEP_METAL		1		// metal floor
#define STEP_DIRT		2		// dirt, sand, rock
#define STEP_VENT		3		// ventillation duct
#define STEP_GRATE		4		// metal grating
#define STEP_TILE		5		// floor tiles
#define STEP_SLOSH		6		// shallow liquid puddle
#define STEP_WADE		7		// wading in liquid
#define STEP_LADDER		8		// climbing ladder

// play step sound to player
void CBasePlayer::PlayStepSound(int step, float fvol)
{
	static int iSkipStep = 0;

	if ( !g_pGameRules->PlayFootstepSounds( this, fvol ) )
		return;

	// irand - 0,1 for right foot, 2,3 for left foot
	// used to alternate left and right foot
	int irand = RANDOM_LONG(0,1) + (m_iStepLeft * 2);

	m_iStepLeft = !m_iStepLeft;

	switch (step)
	{
	default:
	case STEP_CONCRETE:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_step4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_METAL:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_metal4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_DIRT:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_dirt4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_VENT:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_duct4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_GRATE:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_grate4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_TILE:
		if ( !RANDOM_LONG(0,4) )
			irand = 4;
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile4.wav", fvol, ATTN_NORM);	break;
		case 4:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_tile5.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_SLOSH:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_slosh4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_WADE:
		if ( iSkipStep == 0 )
		{
			iSkipStep++;
			break;
		}

		if ( iSkipStep++ == 3 )
		{
			iSkipStep = 0;
		}

		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade2.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade3.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_wade4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	case STEP_LADDER:
		switch (irand)
		{
		case 0:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder1.wav", fvol, ATTN_NORM);	break;
		case 1:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder3.wav", fvol, ATTN_NORM);	break;
		case 2:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder2.wav", fvol, ATTN_NORM);	break;
		case 3:	EMIT_SOUND( ENT(pev), CHAN_BODY, "player/pl_ladder4.wav", fvol, ATTN_NORM);	break;
		}
		break;
	}
}

#define CHAR_TEX_CONCRETE	'C'			// texture types
#define CHAR_TEX_METAL		'M'
#define CHAR_TEX_DIRT		'D'
#define CHAR_TEX_VENT		'V'
#define CHAR_TEX_GRATE		'G'
#define CHAR_TEX_TILE		'T'
#define CHAR_TEX_SLOSH		'S'
#define CHAR_TEX_WOOD		'W'
#define CHAR_TEX_COMPUTER	'P'
#define CHAR_TEX_GLASS		'Y'
#define CHAR_TEX_FLESH		'F'

// Simple mapping from texture type character to step type
int MapTextureTypeStepType(char chTextureType)
{
	switch (chTextureType)
	{
		default:
		case CHAR_TEX_CONCRETE:	return STEP_CONCRETE;	
		case CHAR_TEX_METAL: return STEP_METAL;	
		case CHAR_TEX_DIRT: return STEP_DIRT;	
		case CHAR_TEX_VENT: return STEP_VENT;	
		case CHAR_TEX_GRATE: return STEP_GRATE;	
		case CHAR_TEX_TILE: return STEP_TILE;
		case CHAR_TEX_SLOSH: return STEP_SLOSH;
	}
}

// Try to find texture under player and play step sound for it
void CBasePlayer::UpdateStepSound( void )
{
	int	fWalking;
	float fvol;
	char szbuffer[64];
	const char *pTextureName;
	Vector start, end;
	float rgfl1[3];
	float rgfl2[3];
	Vector knee;
	Vector feet;
	Vector center;
	float height;
	float speed;
	float velrun;
	float velwalk;
	float flduck;
	int	fLadder;
	int step;

	if (gpGlobals->time <= m_flTimeStepSound)
		return;

	if (pev->flags & FL_FROZEN)
		return;

	speed = pev->velocity.Length();

	// determine if we are on a ladder
	fLadder = IsOnLadder();

	// UNDONE: need defined numbers for run, walk, crouch, crouch run velocities!!!!	
	if (FBitSet(pev->flags, FL_DUCKING) || fLadder)
	{
		velwalk = 60;		// These constants should be based on cl_movespeedkey * cl_forwardspeed somehow
		velrun = 80;		// UNDONE: Move walking to server
		flduck = 0.1f;
	}
	else
	{
		velwalk = 120;
		velrun = 210;
		flduck = 0.0;
	}

	// if we're on a ladder or on the ground, and we're moving fast enough,
	// play step sound.  Also, if m_flTimeStepSound is zero, get the new
	// sound right away - we just started moving in new level.

	if ((fLadder || FBitSet (pev->flags, FL_ONGROUND)) && pev->velocity != g_vecZero 
		&& (speed >= velwalk || !m_flTimeStepSound))
	{
		SetAnimation( PLAYER_WALK );
		
		fWalking = speed < velrun;		

		center = knee = feet = (pev->absmin + pev->absmax) * 0.5f;
		height = pev->absmax.z - pev->absmin.z;

		knee.z = pev->absmin.z + height * 0.2f;
		feet.z = pev->absmin.z;

		// find out what we're stepping in or on...
		if (fLadder)
		{
			step = STEP_LADDER;
			fvol = 0.35f;
			m_flTimeStepSound = gpGlobals->time + 0.35f;
		}
		else if ( UTIL_PointContents ( knee ) == CONTENTS_WATER )
		{
			step = STEP_WADE;
			fvol = 0.65f;
			m_flTimeStepSound = gpGlobals->time + 0.6f;
		}
		else if (UTIL_PointContents ( feet ) == CONTENTS_WATER )
		{
			step = STEP_SLOSH;
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
		}
		else
		{
			// find texture under player, if different from current texture, 
			// get material type

			start = end = center;							// center point of player BB
			start.z = end.z = pev->absmin.z;				// copy zmin
			start.z += 4.0f;									// extend start up
			end.z -= 24.0f;									// extend end down
			
			start.CopyToArray(rgfl1);
			end.CopyToArray(rgfl2);

			pTextureName = TRACE_TEXTURE( ENT( pev->groundentity), rgfl1, rgfl2 );
			if ( pTextureName )
			{
				// strip leading '-0' or '{' or '!'
				if (*pTextureName == '-')
					pTextureName += 2;
				if (*pTextureName == '{' || *pTextureName == '!')
					pTextureName++;
				
				if (strncasecmp(pTextureName, m_szTextureName, CBTEXTURENAMEMAX-1))
				{
					// current texture is different from texture player is on...
					// set current texture
					strcpy(szbuffer, pTextureName);
					szbuffer[CBTEXTURENAMEMAX - 1] = 0;
					strcpy(m_szTextureName, szbuffer);
					
					// get texture type
					m_chTextureType = TEXTURETYPE_Find(m_szTextureName);	
				}
			}
			
			step = MapTextureTypeStepType(m_chTextureType);

			switch (m_chTextureType)
			{
			default:
		case CHAR_TEX_CONCRETE:
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_METAL:
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_DIRT:
			fvol = fWalking ? 0.25f : 0.55f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_VENT:
			fvol = fWalking ? 0.4f : 0.7f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_GRATE:
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_TILE:
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

		case CHAR_TEX_SLOSH:
			fvol = fWalking ? 0.2f : 0.5f;
			m_flTimeStepSound = fWalking ? gpGlobals->time + 0.4f : gpGlobals->time + 0.3f;
			break;

			}
		}
		
		m_flTimeStepSound += flduck; // slower step time if ducking

		// play the sound
		// 35% volume if ducking
		if ( pev->flags & FL_DUCKING )
			fvol *= 0.35f;

		PlayStepSound(step, fvol);
	}
}

// PS2: solta o inimigo travado na mira
#define PS2_RELEASE_TARGET() \
	{ \
		CBaseEntity *pEnemy = m_hTargetedEnemy; \
		pEnemy->pev->effects &= ( ENTINDEX( edict() ) == 1 ) ? ~0x100 : ~0x200; \
		m_hTargetedEnemy = NULL; \
		UTIL_Remove( m_ps2hUnk79C ); \
		m_ps2hUnk79C = NULL; \
		(*g_engfuncs.pfnPs2Unk1E0)( ENTINDEX( edict() ), 0 ); \
	}

void CBasePlayer::PreThink(void)
{
	int buttonsChanged = (m_afButtonLast ^ pev->button);	// These buttons have changed this frame
	
	// Debounced button codes for pressed/released
	// UNDONE: Do we need auto-repeat?
	m_afButtonPressed =  buttonsChanged & pev->button;		// The changed ones still down are "pressed"
	m_afButtonReleased = buttonsChanged & (~pev->button);	// The ones not down are "released"

	g_pGameRules->PlayerThink( this );

	if ( g_fGameOver )
		return;         // intermission or finale

	// PS2: travar a mira num inimigo
	if ( !g_pGameRules->Ps2Virt07() && !(pev->flags & FL_FAKECLIENT) )
	{
		if ( (buttonsChanged & 0x4000) && (m_afButtonPressed & 0x4000) )
		{
			if ( !(CBaseEntity *)m_hTargetedEnemy )
			{
				if ( g_iSkillLevel == 1 )
					Ps2AcquireTarget( GetGunPosition(), 8192, 0.70710677f );
				else if ( g_iSkillLevel == 2 )
					Ps2AcquireTarget( GetGunPosition(), 8192, 0.37460659f );
				else
					Ps2AcquireTarget( GetGunPosition(), 8192, 0.13052619f );
			}
			else
				PS2_RELEASE_TARGET();
		}

		if ( (CBaseEntity *)m_hTargetedEnemy )
		{
			if ( ((CBaseEntity *)m_hTargetedEnemy)->pev->deadflag != DEAD_NO || ((CBaseEntity *)m_hTargetedEnemy)->pev->takedamage == DAMAGE_NO )
				PS2_RELEASE_TARGET();
		}
	}

	UTIL_MakeVectors(pev->v_angle);             // is this still used?
	
	ItemPreFrame( );
	WaterMove();

	if ( g_pGameRules && g_pGameRules->FAllowFlashlight() )
		m_iHideHUD &= ~HIDEHUD_FLASHLIGHT;
	else
		m_iHideHUD |= HIDEHUD_FLASHLIGHT;

	// JOHN: checks if new client data (for HUD and view control) needs to be sent to the client
	UpdateClientData();
	
	CheckTimeBasedDamage();

	CheckSuitUpdate();

	if (pev->waterlevel == 2) 
		CheckWaterJump();
	else if ( pev->waterlevel == 0 )
	{
		pev->flags &= ~FL_WATERJUMP;	// Clear this if out of water
	}

	if (pev->deadflag >= DEAD_DYING)
	{
		PlayerDeathThink();
		return;
	}

	// So the correct flags get sent to client asap.
	//
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
		pev->flags |= FL_ONTRAIN;
	else 
		pev->flags &= ~FL_ONTRAIN;

	// Train speed control
	if ( m_afPhysicsFlags & PFLAG_ONTRAIN )
	{
		CBaseEntity *pTrain = CBaseEntity::Instance( pev->groundentity );
		float vel;
		
		if ( !pTrain )
		{
			TraceResult trainTrace;
			// Maybe this is on the other side of a level transition
			UTIL_TraceLine( pev->origin, pev->origin + Vector(0,0,-38), ignore_monsters, ENT(pev), &trainTrace );

			// HACKHACK - Just look for the func_tracktrain classname
			if ( trainTrace.flFraction != 1.0f && trainTrace.pHit )
			pTrain = CBaseEntity::Instance( trainTrace.pHit );


			if ( !pTrain || !(pTrain->ObjectCaps() & FCAP_DIRECTIONAL_USE) || !pTrain->OnControls(pev) )
			{
				//ALERT( at_error, "In train mode with no train!\n" );
				m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
				m_iTrain = TRAIN_NEW|TRAIN_OFF;
				return;
			}
		}
		else if ( !FBitSet( pev->flags, FL_ONGROUND ) || FBitSet( pTrain->pev->spawnflags, SF_TRACKTRAIN_NOCONTROL ) || (pev->button & (IN_MOVELEFT|IN_MOVERIGHT) ) )
		{
			// Turn off the train if you jump, strafe, or the train controls go dead
			m_afPhysicsFlags &= ~PFLAG_ONTRAIN;
			m_iTrain = TRAIN_NEW|TRAIN_OFF;
			return;
		}

		pev->velocity = g_vecZero;
		vel = 0;
		if ( m_afButtonPressed & IN_FORWARD )
		{
			vel = 1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		}
		else if ( m_afButtonPressed & IN_BACK )
		{
			vel = -1;
			pTrain->Use( this, this, USE_SET, (float)vel );
		}

		if (vel)
		{
			m_iTrain = TrainSpeed(pTrain->pev->speed, pTrain->pev->impulse);
			m_iTrain |= TRAIN_ACTIVE|TRAIN_NEW;
		}

	} else if (m_iTrain & TRAIN_ACTIVE)
		m_iTrain = TRAIN_NEW; // turn off train

	if ( !D_003241F0 )
	{
		if (pev->button & IN_JUMP)
		{
			// If on a ladder, jump off the ladder
			// else Jump
			Jump();
		}
	}

	if ( !D_003241F0 )
	{
		// If trying to duck, already ducked, or in the process of ducking
		if ((pev->button & IN_DUCK) || FBitSet(pev->flags,FL_DUCKING) || (m_afPhysicsFlags & PFLAG_DUCKING) )
			Duck();
	}

	UpdateStepSound();

	if ( !FBitSet ( pev->flags, FL_ONGROUND ) )
	{
		m_flFallVelocity = -pev->velocity.z;
	}

	if ( m_afPhysicsFlags & PFLAG_ONBARNACLE )
	{
		pev->velocity = g_vecZero;
	}

	m_afConditions = 0;

	// PS2: o parceiro da IA procura inimigos
	if ( pev->flags & FL_FAKECLIENT )
	{
		if ( m_flPs2NextLook < gpGlobals->time )
		{
			Ps2Look( m_flDistLook );
			m_flPs2NextLook = gpGlobals->time + 0.5f;
		}
		Ps2GetEnemy();
	}

	if ( !(pev->flags & FL_FAKECLIENT) )
		Ps2FindUseable();
}

// PS2: cópia do CBaseMonster::Look para o parceiro da IA
void CBasePlayer::Ps2Look ( int iDistance )
{
	int	iSighted = 0;

	// DON'T let visibility information from last frame sit around!
	m_afConditions &= ~(bits_COND_SEE_HATE | bits_COND_SEE_DISLIKE | bits_COND_SEE_ENEMY | bits_COND_SEE_FEAR | bits_COND_SEE_NEMESIS | bits_COND_SEE_CLIENT);

	m_pLink = NULL;

	CBaseEntity	*pSightEnt = NULL;// the current visible entity that we're dealing with

	// See no evil if prisoner is set
	if ( !FBitSet( pev->spawnflags, SF_MONSTER_PRISONER ) )
	{
		CBaseEntity *pList[100];

		Vector delta = Vector( iDistance, iDistance, iDistance );

		// Find only monsters/clients in box, NOT limited to PVS
		int count = UTIL_EntitiesInBox( pList, 100, pev->origin - delta, pev->origin + delta, FL_CLIENT|FL_MONSTER );
		for ( int i = 0; i < count; i++ )
		{
			pSightEnt = pList[i];
			// !!!temporarily only considering other monsters and clients, don't see prisoners
			if ( pSightEnt != this												&& 
				 !FBitSet( pSightEnt->pev->spawnflags, SF_MONSTER_PRISONER )	&& 
				 pSightEnt->pev->health > 0 )
			{
				if ( pSightEnt->pev->effects == EF_NODRAW )
					continue;

				int iRelationship;
				if ( pSightEnt->MyMonsterPointer() )
					iRelationship = ((CBaseMonster *)pSightEnt)->IRelationship( this );
				else
					iRelationship = IRelationship( pSightEnt );

				// the looker will want to consider this entity
				// don't check anything else about an entity that can't be seen, or an entity that you don't care about.
				if ( iRelationship != R_NO && FInViewCone( pSightEnt ) && !FBitSet( pSightEnt->pev->flags, FL_NOTARGET ) && FVisible( pSightEnt ) && pSightEnt->pev->takedamage == DAMAGE_AIM )
				{
					if ( pSightEnt->pev->flags & FL_CLIENT )
					{
						if ( pev->spawnflags & SF_MONSTER_WAIT_TILL_SEEN )
						{
							CBaseMonster *pClient;

							pClient = pSightEnt->MyMonsterPointer();
							// don't link this client in the list if the monster is wait till seen and the player isn't facing the monster
							if ( pSightEnt && !pClient->FInViewCone( this ) )
							{
								// we're not in the player's view cone. 
								continue;
							}
							else
							{
								// player sees us, become normal now.
								pev->spawnflags &= ~SF_MONSTER_WAIT_TILL_SEEN;
							}
						}

						// if we see a client, remember that (mostly for scripted AI)
						iSighted |= bits_COND_SEE_CLIENT;
					}

					if ( m_ps2AIType == 8 && pSightEnt->pev->max_health <= 100 )
						continue;

					if ( !FVisible( pSightEnt->pev->origin + pSightEnt->pev->view_ofs ) )
						continue;

					pSightEnt->m_pLink = m_pLink;
					m_pLink = pSightEnt;

					if ( pSightEnt == m_hEnemy )
					{
						// we know this ent is visible, so if it also happens to be our enemy, store that now.
						iSighted |= bits_COND_SEE_ENEMY;
					}

					// don't add the Enemy's relationship to the conditions. We only want to worry about conditions when
					// we see monsters other than the Enemy.
					switch ( iRelationship )
					{
					case	R_NM:
						iSighted |= bits_COND_SEE_NEMESIS;		
						break;
					case	R_HT:		
						iSighted |= bits_COND_SEE_HATE;		
						break;
					case	R_DL:
						iSighted |= bits_COND_SEE_DISLIKE;
						break;
					case	R_FR:
						iSighted |= bits_COND_SEE_FEAR;
						break;
					case    R_AL:
						break;
					default:
						ALERT ( at_aiconsole, "%s can't assess %s\n", STRING(pev->classname), STRING(pSightEnt->pev->classname ) );
						break;
					}
				}
			}
		}
	}
	
	m_afConditions |= iSighted;
}

/* 0x1C94B8: BestVisibleEnemy do CBaseMonster para o jogador (PS2) */
CBaseEntity *CBasePlayer::Ps2BestVisibleEnemy ( void )
{
	CBaseEntity	*pReturn;
	CBaseEntity	*pNextEnt;
	float		iNearest;
	float		iDist;
	int			iBestRelationship;

	iNearest = 8192;// so first visible entity will become the closest.
	pNextEnt = m_pLink;
	pReturn = NULL;
	iBestRelationship = R_NO;

	while ( pNextEnt != NULL )
	{
		if ( pNextEnt->IsAlive() )
		{
			CBaseMonster *pMonster = pNextEnt->MyMonsterPointer();
			if ( pMonster )
			{
				if ( pMonster->IRelationship( this ) > iBestRelationship )
				{
					// this entity is disliked MORE than the entity that we 
					// currently think is the best visible enemy. No need to do 
					// a distance check, just get mad at this one for now.
					iBestRelationship = pMonster->IRelationship ( this );
					iNearest = ( pNextEnt->pev->origin - pev->origin ).Length();
					pReturn = pNextEnt;
				}
				else if ( pMonster->IRelationship( this ) == iBestRelationship )
				{
					// this entity is disliked just as much as the entity that
					// we currently think is the best visible enemy, so we only
					// get mad at it if it is closer.
					iDist = ( pNextEnt->pev->origin - pev->origin ).Length();
					
					if ( iDist <= iNearest )
					{
						iNearest = iDist;
						iBestRelationship = pMonster->IRelationship ( this );
						pReturn = pNextEnt;
					}
				}
			}
		}

		pNextEnt = pNextEnt->m_pLink;
	}

	return pReturn;
}

/* 0x1C96A0: GetEnemy simplificado do PS2 (sem PushEnemy e sem o dono) */
BOOL CBasePlayer::Ps2GetEnemy ( void )
{
	CBaseEntity *pNewEnemy;

	if ( m_afConditions & (bits_COND_SEE_HATE | bits_COND_SEE_DISLIKE | bits_COND_SEE_NEMESIS) )
	{
		pNewEnemy = Ps2BestVisibleEnemy();

		if ( pNewEnemy != m_hEnemy && pNewEnemy != NULL)
		{
			m_afConditions |= bits_COND_NEW_ENEMY;
			m_hEnemy = pNewEnemy;
		}
	}

	// remember old enemies
	if (m_hEnemy == NULL && PopEnemy( ))
	{
		if ( m_pSchedule )
		{
			if ( m_pSchedule->iInterruptMask & bits_COND_NEW_ENEMY )
			{
				m_afConditions |= bits_COND_NEW_ENEMY;
			}
		}
	}

	if ( (CBaseEntity *)m_hEnemy && ((CBaseEntity *)m_hEnemy)->pev->deadflag )
		m_hEnemy = NULL;

	return m_hEnemy != NULL;
}

#define PARALYZE_DURATION	2		// number of 2 second intervals to take damage
#define PARALYZE_DAMAGE		1.0		// damage to take each 2 second interval

#define NERVEGAS_DURATION	2
#define NERVEGAS_DAMAGE		5.0

#define POISON_DURATION		5
#define POISON_DAMAGE		2.0f

#define RADIATION_DURATION	2
#define RADIATION_DAMAGE	1.0

#define ACID_DURATION		2
#define ACID_DAMAGE			5.0

#define SLOWBURN_DURATION	2
#define SLOWBURN_DAMAGE		1.0

#define SLOWFREEZE_DURATION	2
#define SLOWFREEZE_DAMAGE	1.0

#define itbd_Paralyze		0		
#define itbd_NerveGas		1
#define itbd_Poison			2
#define itbd_Radiation		3
#define itbd_Acid			5
#define itbd_SlowBurn		6
#define itbd_SlowFreeze		7

void CBasePlayer::CheckTimeBasedDamage() 
{
	int i;
	BYTE bDuration = 0;

	if (!(m_bitsDamageType & DMG_TIMEBASED))
		return;

	// only check for time based damage approx. every 2 seconds
	if (fabsf(gpGlobals->time - m_tbdPrev) < 2.0f)
		return;
	
	m_tbdPrev = gpGlobals->time;

	for (i = 0; i < CDMG_TIMEBASED; i++)
	{
		// make sure bit is set for damage type
		if (m_bitsDamageType & (DMG_PARALYZE << i))
		{
			switch (i)
			{
			case itbd_Paralyze:
				// UNDONE - flag movement as half-speed
				bDuration = PARALYZE_DURATION;
				break;
			case itbd_NerveGas:
				bDuration = NERVEGAS_DURATION;
				break;
			case itbd_Poison:
				TakeDamage(pev, pev, POISON_DAMAGE, DMG_GENERIC);
				bDuration = POISON_DURATION;
				break;
			case itbd_Radiation:
				bDuration = RADIATION_DURATION;
				break;
			case itbd_DrownRecover:
				// NOTE: this hack is actually used to RESTORE health
				// after the player has been drowning and finished
				// taking drowning damage (DMG_DROWN).  
				if (m_idrowndmg > m_idrownrestored)
				{
					int idif = min(m_idrowndmg - m_idrownrestored, 10);

					TakeHealth(idif, DMG_GENERIC);
					m_idrownrestored += idif;
				}
				bDuration = 4;	// get up to 5*10 = 50 points back
				break;
			case itbd_Acid:
				bDuration = ACID_DURATION;
				break;
			case itbd_SlowBurn:
				bDuration = SLOWBURN_DURATION;
				break;
			case itbd_SlowFreeze:
				bDuration = SLOWFREEZE_DURATION;
				break;
			default:
				bDuration = 0;
			}

			if (m_rgbTimeBasedDamage[i])
			{
				// use up an antitoxin on poison or nervegas after a few seconds of damage					
				if (((i == itbd_NerveGas) && (m_rgbTimeBasedDamage[i] < NERVEGAS_DURATION)) ||
					((i == itbd_Poison)   && (m_rgbTimeBasedDamage[i] < POISON_DURATION)))
				{
					if (m_rgItems[ITEM_ANTIDOTE])
					{
						m_rgbTimeBasedDamage[i] = 0;
						m_rgItems[ITEM_ANTIDOTE]--;
						SetSuitUpdate("!HEV_HEAL4", FALSE, SUIT_REPEAT_OK);
					}
				}

				// decrement damage duration, detect when done.
				if (!m_rgbTimeBasedDamage[i] || --m_rgbTimeBasedDamage[i] == 0)
				{
					m_rgbTimeBasedDamage[i] = 0;
					// if we're done, clear damage bits
					m_bitsDamageType &= ~(DMG_PARALYZE << i);	
				}
			}
			else
				// first time taking this damage type - init damage duration
				m_rgbTimeBasedDamage[i] = bDuration;
		}
	}
}

/*
 *  GEIGER
 */

#define GEIGERDELAY 0.25f

#define CBSENTENCENAME_MAX 16
#define CVOXFILESENTENCEMAX		1536
extern char gszallsentencenames[CVOXFILESENTENCEMAX][CBSENTENCENAME_MAX];

/* sem inicializador: o gcc põe no fim do .data, depois do g_pevLastInflictor (0x317D80) */
int gmsgGeigerRange;

void CBasePlayer :: UpdateGeigerCounter( void )
{
	BYTE range;

	// delay per update ie: don't flood net with these msgs
	if (gpGlobals->time < m_flgeigerDelay)
		return;

	m_flgeigerDelay = gpGlobals->time + GEIGERDELAY;
		
	// send range to radition source to client

	range = (BYTE) (m_flgeigerRange / 4);

	if (range != m_igeigerRangePrev)
	{
		m_igeigerRangePrev = range;

		MESSAGE_BEGIN( MSG_ONE, gmsgGeigerRange, NULL, pev );
			WRITE_BYTE( range );
		MESSAGE_END();
	}

	// reset counter and semaphore
	if (!RANDOM_LONG(0,3))
		m_flgeigerRange = 1000;
}

/*
 * SUIT
 */

#define SUITUPDATETIME	3.5f
#define SUITFIRSTUPDATETIME 0.1f

void CBasePlayer::CheckSuitUpdate()
{
	int i;
	int isentence = 0;
	int isearch = m_iSuitPlayNext;
	
	// Ignore suit updates if no suit
	if ( !(pev->weapons & (1<<WEAPON_SUIT)) )
		return;

	// if in range of radiation source, ping geiger counter
	UpdateGeigerCounter();

	if ( g_pGameRules->Ps2Virt07() )
	{
		// don't bother updating HEV voice in multiplayer.
		return;
	}

	if ( gpGlobals->time >= m_flSuitUpdate && m_flSuitUpdate > 0)
	{
		// play a sentence off of the end of the queue
		for (i = 0; i < CSUITPLAYLIST; i++)
			{
			if (isentence = m_rgSuitPlayList[isearch])
				break;
			
			if (++isearch == CSUITPLAYLIST)
				isearch = 0;
			}

		if (isentence)
		{
			m_rgSuitPlayList[isearch] = 0;
			if (isentence > 0)
			{
				// play sentence number

				char sentence[CBSENTENCENAME_MAX+1];
				strcpy(sentence, "!");
				strcat(sentence, gszallsentencenames[isentence]);
				EMIT_SOUND_SUIT(ENT(pev), sentence);
			}
			else
			{
				// play sentence group
				EMIT_GROUPID_SUIT(ENT(pev), -isentence);
			}
		m_flSuitUpdate = gpGlobals->time + SUITUPDATETIME;
		}
		else
			// queue is empty, don't check 
			m_flSuitUpdate = 0;
	}
}

extern int SENTENCEG_GetIndex(const char *szrootname);

// add sentence to suit playlist queue. if fgroup is true, then
// name is a sentence group (HEV_AA), otherwise name is a specific
// sentence name ie: !HEV_AA0.  If iNoRepeat is specified in
// seconds, then we won't repeat playback of this word or sentence
// for at least that number of seconds.

void CBasePlayer::SetSuitUpdate(char *name, int fgroup, int iNoRepeatTime)
{
	int i;
	int isentence;
	int iempty = -1;
	
	/* PS2: modo vortigaunt não tem voz do traje */
	if (m_iHideHUD & HIDEHUD_PS2_NOPICKUP)
		return;

	// Ignore suit updates if no suit
	if ( !(pev->weapons & (1<<WEAPON_SUIT)) )
		return;

	if ( g_pGameRules->Ps2Virt07() )
	{
		// due to static channel design, etc. We don't play HEV sounds in multiplayer right now.
		return;
	}

	if ( !g_pGameRules->Ps2Virt66() )
		return;

	// if name == NULL, then clear out the queue

	if (!name)
	{
		for (i = 0; i < CSUITPLAYLIST; i++)
			m_rgSuitPlayList[i] = 0;
		return;
	}
	// get sentence or group number
	if (!fgroup)
	{
		isentence = SENTENCEG_Lookup(name, NULL);
		if (isentence < 0)
			return;
	}
	else
		// mark group number as negative
		isentence = -SENTENCEG_GetIndex(name);

	// check norepeat list - this list lets us cancel
	// the playback of words or sentences that have already
	// been played within a certain time.

	for (i = 0; i < CSUITNOREPEAT; i++)
	{
		if (isentence == m_rgiSuitNoRepeat[i])
			{
			// this sentence or group is already in 
			// the norepeat list

			if (m_rgflSuitNoRepeatTime[i] < gpGlobals->time)
				{
				// norepeat time has expired, clear it out
				m_rgiSuitNoRepeat[i] = 0;
				m_rgflSuitNoRepeatTime[i] = 0.0;
				iempty = i;
				break;
				}
			else
				{
				// don't play, still marked as norepeat
				return;
				}
			}
		// keep track of empty slot
		if (!m_rgiSuitNoRepeat[i])
			iempty = i;
	}

	// sentence is not in norepeat list, save if norepeat time was given

	if (iNoRepeatTime)
	{
		if (iempty < 0)
			iempty = RANDOM_LONG(0, CSUITNOREPEAT-1); // pick random slot to take over
		m_rgiSuitNoRepeat[iempty] = isentence;
		m_rgflSuitNoRepeatTime[iempty] = iNoRepeatTime + gpGlobals->time;
	}

	// find empty spot in queue, or overwrite last spot
	
	m_rgSuitPlayList[m_iSuitPlayNext++] = isentence;
	if (m_iSuitPlayNext == CSUITPLAYLIST)
		m_iSuitPlayNext = 0;

	if (m_flSuitUpdate <= gpGlobals->time)
	{
		if (m_flSuitUpdate == 0)
			// play queue is empty, don't delay too long before playback
			m_flSuitUpdate = gpGlobals->time + SUITFIRSTUPDATETIME;
		else 
			m_flSuitUpdate = gpGlobals->time + SUITUPDATETIME; 
	}
}

/*
================
CheckPowerups

Check for turning off powerups

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
================
*/
	static void
CheckPowerups(entvars_t *pev)
{
	if (pev->health <= 0)
		return;

	pev->modelindex = g_ulModelIndexPlayer;    // don't use eyes
}


//=========================================================
// UpdatePlayerSound - updates the position of the player's
// reserved sound slot in the sound list.
//=========================================================
void CBasePlayer :: UpdatePlayerSound ( void )
{
	int iBodyVolume;
	int iVolume;
	CSound *pSound;

	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt :: ClientSoundIndex( edict() ) );

	if ( !pSound )
	{
		ALERT ( at_console, "Client lost reserved sound!\n" );
		return;
	}

	pSound->m_iType = bits_SOUND_NONE;

	// now calculate the best target volume for the sound. If the player's weapon
	// is louder than his body/movement, use the weapon volume, else, use the body volume.
	
	if ( FBitSet ( pev->flags, FL_ONGROUND ) )
	{	
		iBodyVolume = pev->velocity.Length(); 

		// clamp the noise that can be made by the body, in case a push trigger,
		// weapon recoil, or anything shoves the player abnormally fast. 
		if ( iBodyVolume > 512 )
		{
			iBodyVolume = 512;
		}
	}
	else
	{
		iBodyVolume = 0;
	}

	if ( pev->button & IN_JUMP )
	{
		iBodyVolume += 100;
	}

// convert player move speed and actions into sound audible by monsters.
	if ( m_iWeaponVolume > iBodyVolume )
	{
		m_iTargetVolume = m_iWeaponVolume;

		// OR in the bits for COMBAT sound if the weapon is being louder than the player. 
		pSound->m_iType |= bits_SOUND_COMBAT;
	}
	else
	{
		m_iTargetVolume = iBodyVolume;
	}

	// decay weapon volume over time so bits_SOUND_COMBAT stays set for a while
	m_iWeaponVolume -= (int)(250 * gpGlobals->frametime);


	// if target volume is greater than the player sound's current volume, we paste the new volume in 
	// immediately. If target is less than the current volume, current volume is not set immediately to the
	// lower volume, rather works itself towards target volume over time. This gives monsters a much better chance
	// to hear a sound, especially if they don't listen every frame.
	iVolume = pSound->m_iVolume;

	if ( m_iTargetVolume > iVolume )
	{
		iVolume = m_iTargetVolume;
	}
	else if ( iVolume > m_iTargetVolume )
	{
		iVolume -= (int)(250 * gpGlobals->frametime);

		if ( iVolume < m_iTargetVolume )
		{
			iVolume = 0;
		}
	}

	if ( m_fNoPlayerSound )
	{
		// debugging flag, lets players move around and shoot without monsters hearing.
		iVolume = 0;
	}

	if ( gpGlobals->time > m_flStopExtraSoundTime )
	{
		// since the extra sound that a weapon emits only lasts for one client frame, we keep that sound around for a server frame or two 
		// after actual emission to make sure it gets heard.
		m_iExtraSoundTypes = 0;
	}

	if ( pSound )
	{
		pSound->m_vecOrigin = pev->origin;
		pSound->m_iType |= ( bits_SOUND_PLAYER | m_iExtraSoundTypes );
		pSound->m_iVolume = iVolume;
	}

	// keep track of virtual muzzle flash
	m_iWeaponFlash -= (int)(256 * gpGlobals->frametime);
	if (m_iWeaponFlash < 0)
		m_iWeaponFlash = 0;

	//UTIL_MakeVectors ( pev->angles );
	//gpGlobals->v_forward.z = 0;

	// Below are a couple of useful little bits that make it easier to determine just how much noise the 
	// player is making. 
	// UTIL_ParticleEffect ( pev->origin + gpGlobals->v_forward * iVolume, g_vecZero, 255, 25 );
	//ALERT ( at_console, "%d/%d\n", iVolume, m_iTargetVolume );
}



#define PLAYER_FALL_PUNCH_THRESHHOLD	(float)350
#define PLAYER_MAX_SAFE_FALL_SPEED		580
#define PLAYER_MIN_BOUNCE_SPEED			200

/* PostThink do SDK antigo: sons de queda e passos no pouso */
void CBasePlayer::PostThink()
{
	if ( g_fGameOver )
	{
		// intermission or finale
		StudioFrameAdvance( );
		if ( IsAlive() )
			SetAnimation( PLAYER_IDLE );
		return;
	}

	if (!IsAlive())
		return;

	// Handle Tank controlling
	if ( m_pTank != NULL )
	{ // if they've moved too far from the gun,  or selected a weapon, unuse the gun
		if ( m_pTank->OnControls( pev ) && !pev->weaponmodel )
		{  
			m_pTank->Use( this, this, USE_SET, 2 );	// try fire the gun
		}
		else
		{  // they've moved off the platform
			m_pTank->Use( this, this, USE_OFF, 0 );
			m_pTank = NULL;
		}
	}

// do weapon stuff
	ItemPostFrame( );

// check to see if player landed hard enough to make a sound
// falling farther than half of the maximum safe distance, but not as far a max safe distance will
// play a bootscrape sound, and no damage will be inflicted. Fallling a distance shorter than half
// of maximum safe distance will make no sound. Falling farther than max safe distance will play a 
// fallpain sound, and damage will be inflicted based on how far the player fell

	if ( (FBitSet(pev->flags, FL_ONGROUND)) && (pev->health > 0) && m_flFallVelocity >= PLAYER_FALL_PUNCH_THRESHHOLD )
	{
		float fvol = 0.5;

		// ALERT ( at_console, "%f\n", m_flFallVelocity );

		if (pev->watertype == CONTENTS_WATER)
		{
			// Did he hit the world or a non-moving entity?
			// BUG - this happens all the time in water, especially when 
			// BUG - water has current force
			// if ( !pev->groundentity || VARS(pev->groundentity)->velocity.z == 0 )
				// EMIT_SOUND(ENT(pev), CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM);
		}
		else if ( m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED )
		{// after this point, we start doing damage
			
			switch ( RANDOM_LONG(0,1) )
			{
			case 0:
				EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_fallpain2.wav", 1, ATTN_NORM);
			case 1:
				EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pl_fallpain3.wav", 1, ATTN_NORM);
				break;
			}

			float flFallDamage = g_pGameRules->FlPlayerFallDamage( this );

			if ( flFallDamage > pev->health )
			{//splat
				// note: play on item channel because we play footstep landing on body channel
				EMIT_SOUND(ENT(pev), CHAN_ITEM, "common/bodysplat.wav", 1, ATTN_NORM);
			}

			if ( flFallDamage > 0 )
			{
				TakeDamage(VARS(eoNullEntity), VARS(eoNullEntity), flFallDamage, DMG_FALL ); 
			}

			fvol = 1.0;
		}
		else if ( m_flFallVelocity > PLAYER_MAX_SAFE_FALL_SPEED / 2 )
		{
			fvol = 0.85;
		}
		else if ( m_flFallVelocity < PLAYER_MIN_BOUNCE_SPEED )
		{
			fvol = 0;
		}

		if ( fvol > 0.0f )
		{
			// get current texture under player right away
			m_flTimeStepSound = 0;
			UpdateStepSound();

			PlayStepSound( MapTextureTypeStepType( m_chTextureType ), fvol );

			// play step sound for current texture
			pev->punchangle.x = m_flFallVelocity * 0.018f;	// punch view

			if ( pev->punchangle.x > 8 )
			{
				pev->punchangle.x = 8;
			}

			if ( RANDOM_LONG( 0, 1 ) )
			{
				pev->punchangle.z = m_flFallVelocity * 0.009f;
			}
		}

		if ( IsAlive() )
		{
			SetAnimation( PLAYER_WALK );
		}
    }

	if (FBitSet(pev->flags, FL_ONGROUND))
	{		
		if (m_flFallVelocity > 64 && !g_pGameRules->Ps2Virt07())
		{
			CSoundEnt::InsertSound ( bits_SOUND_PLAYER, pev->origin, m_flFallVelocity, 0.2 );
			// ALERT( at_console, "fall %f\n", m_flFallVelocity );
		}
		m_flFallVelocity = 0;
	}

	// select the proper animation for the player character	
	if ( IsAlive() )
	{
		if (!pev->velocity.x && !pev->velocity.y)
			SetAnimation( PLAYER_IDLE );
		else if ((pev->velocity.x || pev->velocity.y) && (FBitSet(pev->flags, FL_ONGROUND)))
			SetAnimation( PLAYER_WALK );
		else if (pev->waterlevel > 1)
			SetAnimation( PLAYER_WALK );
	}

	StudioFrameAdvance( );
	CheckPowerups(pev);

	UpdatePlayerSound();

	// Track button info so we can detect 'pressed' and 'released' buttons next frame
	m_afButtonLast = pev->button;
}

extern void UTIL_PlayerDecalTrace( TraceResult *pTrace, int playernum, int decalNumber, BOOL bIsCustom );
extern void UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor );
extern int DispatchSpawn( edict_t *pent );
extern void DispatchTouch( edict_t *pentTouched, edict_t *pentOther );
extern CBaseEntity *UTIL_FindEntityByTargetname( CBaseEntity *pStartEntity, const char *szName );

// checks if the spot is clear of players
BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot )
{
	CBaseEntity *ent = NULL;

	if ( !pSpot->IsTriggered( pPlayer ) )
	{
		return FALSE;
	}

	while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 128 )) != NULL )
	{
		// if ent is a client, don't spawn on 'em
		if ( ent->IsPlayer() && ent != pPlayer )
			return FALSE;
	}

	return TRUE;
}


/* sem inicializador: fica no fim do .data (0x317D84) */
DLL_GLOBAL CBaseEntity	*g_pLastSpawn;
inline int FNullEnt( CBaseEntity *ent ) { return (ent == NULL) || FNullEnt( ent->edict() ); }

/*
============
EntSelectSpawnPoint

Returns the entity to spawn at

USES AND SETS GLOBAL g_pLastSpawn
============
*/
edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer )
{
	CBaseEntity *pSpot;
	edict_t		*player;

	player = pPlayer->edict();

// choose a info_player_deathmatch point
	if (g_pGameRules->IsMultiplayer())
	{
		/* PS2: o info_player_coop com o team igual ao índice do jogador */
		pSpot = UTIL_FindEntityByClassname( NULL, "info_player_coop");
		if ( !FNullEnt(pSpot) )
		{
			do
			{
				if ( pSpot->pev->team == ENTINDEX( pPlayer->edict() ) )
					break;
				pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_coop");
			} while ( !FNullEnt(pSpot) );
			if ( !FNullEnt(pSpot) )
				goto ReturnSpot;
		}
		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_deathmatch");
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
		pSpot = UTIL_FindEntityByClassname( g_pLastSpawn, "info_player_start");
		if ( !FNullEnt(pSpot) ) 
			goto ReturnSpot;
	}
	else if ( g_pGameRules->Ps2Virt08() )
	{
		pSpot = g_pLastSpawn;
		// Randomize the start spot
		for ( int i = RANDOM_LONG(1,5); i > 0; i-- )
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
		if ( FNullEnt( pSpot ) )  // skip over the null point
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );

		CBaseEntity *pFirstSpot = pSpot;

		do 
		{
			if ( pSpot )
			{
				// check if pSpot is valid
				if ( IsSpawnPointValid( pPlayer, pSpot ) )
				{
					if ( pSpot->pev->origin == Vector( 0, 0, 0 ) )
					{
						pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
						continue;
					}

					// if so, go to pSpot
					goto ReturnSpot;
				}
			}
			// increment pSpot
			pSpot = UTIL_FindEntityByClassname( pSpot, "info_player_deathmatch" );
		} while ( pSpot != pFirstSpot ); // loop if we're not back to the start

		// we haven't found a place to spawn yet,  so kill any guy at the first spawn point and spawn there
		if ( !FNullEnt( pSpot ) )
		{
			CBaseEntity *ent = NULL;
			while ( (ent = UTIL_FindEntityInSphere( ent, pSpot->pev->origin, 128 )) != NULL )
			{
				// if ent is a client, kill em (unless they are ourselves)
				if ( ent->IsPlayer() && !(ent->edict() == player) )
					ent->TakeDamage( VARS(INDEXENT(0)), VARS(INDEXENT(0)), 300, DMG_GENERIC );
			}
			goto ReturnSpot;
		}
	}

	// If startspot is set, (re)spawn there.
	if ( FStringNull( gpGlobals->startspot ) || !strlen(STRING(gpGlobals->startspot)))
	{
		pSpot = UTIL_FindEntityByClassname(NULL, "info_player_start");
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
	}
	else
	{
		pSpot = UTIL_FindEntityByTargetname( NULL, STRING(gpGlobals->startspot) );
		if ( !FNullEnt(pSpot) )
			goto ReturnSpot;
	}

ReturnSpot:
	if ( FNullEnt( pSpot ) )
	{
		ALERT(at_error, "PutClientInServer: no info_player_start on level");
		return INDEXENT(0);
	}

	g_pLastSpawn = pSpot;
	return pSpot->edict();
}

extern "C" ps2cvar_t D_00310060;	/* mp_coopslave: o parceiro é o vortigaunt */
extern "C" ps2cvar_t D_00332608;	/* notarget do motor */
extern "C" ps2cvar_t D_00332620;	/* god do motor */
extern "C" ps2cvar_t D_00332638;	/* gravidade do jogador (< 0 usa a padrão) */

void CBasePlayer::Spawn( void )
{
	int i;

	pev->classname		= MAKE_STRING("player");
	pev->health			= 100;
	pev->armorvalue		= 0;
	pev->takedamage		= DAMAGE_YES;
	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_WALK;
	pev->max_health		= pev->health;
	pev->flags			= FL_CLIENT;
	pev->air_finished	= gpGlobals->time + 12;
	pev->dmg			= 2;				// initial water damage
	pev->effects		= 0;
	pev->deadflag		= DEAD_NO;
	pev->dmg_take		= 0;
	pev->dmg_save		= 0;
	m_bitsHUDDamage		= -1;
	m_bitsDamageType	= 0;
	m_iClientFOV		= -1; // make sure fov reset is sent
	m_afPhysicsFlags	= 0;
	m_fLongJump			= FALSE;// no longjump module. 
	m_iFOV				= 0;// init field of view.

	m_flNextDecalTime	= 0;// let this player decal as soon as he spawns.

	m_flgeigerDelay = gpGlobals->time + 2.0f;	// wait a few seconds until user-defined message registrations
												// are recieved by all clients
	
	m_flTimeStepSound	= 0;
	m_iStepLeft = 0;
	m_flFieldOfView		= 0.5;// some monsters use this to determine whether or not the player is looking at them.

	m_bloodColor	= BLOOD_COLOR_RED;
	m_flNextAttack	= gpGlobals->time;
	StartSneaking();

	m_iFlashBattery = 99;
	m_flFlashLightTime = 1; // force first message

// dont let uninitialized value here hurt the player
	m_flFallVelocity = 0;

	g_pGameRules->SetDefaultPlayerTeam( this );
	g_pGameRules->GetPlayerSpawnSpot( this );

	if ( g_pGameRules->IsMultiplayer() )
	{
		if ( D_00310060.value > 0 )
			SET_MODEL(ENT(pev), "models/player/dm_slave/dm_slave.dol");
		else
			SET_MODEL(ENT(pev), "models/player/ginacol/ginacol.dol");
	}
	else if ( g_pGameRules->Ps2Virt07() )
		SET_MODEL(ENT(pev), "models/player.dol");
	else
		SET_MODEL(ENT(pev), "models/player.dol");

	g_ulModelIndexPlayer = pev->modelindex;

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);

	pev->view_ofs = VEC_VIEW;
	Precache();
	m_HackedGunPos		= Vector( 0, 32, 0 );

	if ( m_iPlayerSound == SOUNDLIST_EMPTY )
	{
		ALERT ( at_console, "Couldn't alloc player sound slot!\n" );
	}

	m_fNoPlayerSound = FALSE;// normal sound behavior.

	m_pLastItem = NULL;
	m_fInitHUD = TRUE;
	m_iClientHideHUD = -1;  // force this to be recalculated
	m_fWeapon = FALSE;
	m_pClientActiveItem = NULL;
	m_iClientBattery = -1;

	// reset all ammo values to 0
	for ( i = 0; i < MAX_AMMO_SLOTS; i++ )
	{
		m_rgAmmo[i] = 0;
		m_rgAmmoLast[i] = 0;  // client ammo values also have to be reset  (the death hud clear messages does on the client side)
	}

	m_lastx = m_lasty = 0;

	for ( i = 0; i < 11; i++ )
		m_piSwarmNodeHistory[i] = -1;
	m_iSwarmHistoryIdx = 0;

	m_flFieldOfView = -0.7f;
	m_flDistLook = 2048;
	m_hTargetedEnemy = NULL;
	m_afButtonPressed &= ~IN_ALT1;
	m_afButtonReleased &= ~IN_ALT1;
	m_hUseableHighlight = NULL;
	m_hLastUseableItem = NULL;
	m_flNextUseableCheck = 0;

	g_pGameRules->PlayerSpawn( this );

	if ( D_00332608.value > 0 )
		pev->flags |= FL_NOTARGET;
	else
		pev->flags &= ~FL_NOTARGET;

	if ( D_00332620.value > 0 )
		pev->flags |= FL_GODMODE;
	else
		pev->flags &= ~FL_GODMODE;

	if ( D_00332638.value >= 0 )
		pev->gravity = D_00332638.value;
	else
		pev->gravity = m_flDefaultGravity;

	intf_playerdead.value = 0;
}

/* PS2: sem o teste de "já registradas" e com as mensagens novas do PS2 */
/* no PS2 as strings das mensagens saem depois das do Precache: o corpo entra direto
   no Precache (como macro), não como função inline definida antes */
#define LinkUserMessages() \
	gmsgSelAmmo = REG_USER_MSG("SelAmmo", 4); \
	gmsgCurWeapon = REG_USER_MSG("CurWeapon", 3); \
	gmsgGeigerRange = REG_USER_MSG("Geiger", 1); \
	gmsgFlashlight = REG_USER_MSG("Flashlight", 2); \
	gmsgFlashBattery = REG_USER_MSG("FlashBat", 1); \
	gmsgHealth = REG_USER_MSG( "Health", 1 ); \
	gmsgDamage = REG_USER_MSG( "Damage", 12 ); \
	gmsgBattery = REG_USER_MSG( "Battery", 2); \
	gmsgTrain = REG_USER_MSG( "Train", 1); \
	gmsgHudText = REG_USER_MSG( "HudText", -1 ); \
	gmsgSayText = REG_USER_MSG( "SayText", -1 ); \
	gmsgTextMsg = REG_USER_MSG( "TextMsg", -1 ); \
	gmsgWeaponList = REG_USER_MSG("WeaponList", -1); \
	gmsgResetHUD = REG_USER_MSG("ResetHUD", 1); \
	gmsgInitHUD = REG_USER_MSG("InitHUD", 0 ); \
	gmsgShowGameTitle = REG_USER_MSG("GameTitle", 1); \
	gmsgDeathMsg = REG_USER_MSG( "DeathMsg", -1 ); \
	gmsgScoreInfo = REG_USER_MSG( "ScoreInfo", 11 ); \
	gmsgTeamInfo = REG_USER_MSG( "TeamInfo", -1 );   \
	gmsgTeamScore = REG_USER_MSG( "TeamScore", -1 );   \
	gmsgGameMode = REG_USER_MSG( "GameMode", 1 ); \
	gmsgMOTD = REG_USER_MSG( "MOTD", -1 ); \
	gmsgAmmoPickup = REG_USER_MSG( "AmmoPickup", 2 ); \
	gmsgWeapPickup = REG_USER_MSG( "WeapPickup", 1 ); \
	gmsgItemPickup = REG_USER_MSG( "ItemPickup", -1 ); \
	gmsgHideWeapon = REG_USER_MSG( "HideWeapon", 1 ); \
	gmsgSetFOV = REG_USER_MSG( "SetFOV", 1 ); \
	gmsgShowMenu = REG_USER_MSG( "ShowMenu", -1 ); \
	gmsgRumble = REG_USER_MSG( "SetActuator", 4 ); \
	gmsgShake = REG_USER_MSG("ScreenShake", 6); \
	gmsgFade = REG_USER_MSG("ScreenFade", 10); \
	gmsgAmmoX = REG_USER_MSG("AmmoX", 2); \
	gmsgTankUse = REG_USER_MSG("TankUse", 5); \
	gmsgTankAmmo = REG_USER_MSG("TankAmmo", 2); \
	gmsgSlaveHud = REG_USER_MSG("ISlaveHud", 1); \
	gmsgButtonHud = REG_USER_MSG("ButtonHud", 4); \
	gmsgCrouchState = REG_USER_MSG("CrouchState", 1); \
	gmsgHudColor = REG_USER_MSG("HudColor", 3); \
	gmsgToggleScore = REG_USER_MSG("ToggleScore", 0)

void CBasePlayer :: Precache( void )
{
	// in the event that the player JUST spawned, and the level node graph
	// was loaded, fix all of the node graph pointers before the game starts.
	
	// !!!BUGBUG - now that we have multiplayer, this needs to be moved!
	if ( WorldGraph.m_fGraphPresent && !WorldGraph.m_fGraphPointersSet )
	{
		if ( !WorldGraph.FSetGraphPointers() )
		{
			ALERT ( at_console, "**Graph pointers were not set!\n");
		}
		else
		{
			ALERT ( at_console, "**Graph Pointers Set!\n" );
		} 
	}

	// SOUNDS / MODELS ARE PRECACHED in ClientPrecache() (game specific)
	// because they need to precache before any clients have connected

	// init geiger counter vars during spawn and each time
	// we cross a level transition

	m_flgeigerRange = 1000;
	m_igeigerRangePrev = 1000;

	m_bitsDamageType = 0;
	m_bitsHUDDamage = -1;

	m_iClientBattery = -1;

	m_iTrain = TRAIN_NEW;

	// Make sure any necessary user messages have been registered
	LinkUserMessages();

	m_iUpdateTime = 5;  // won't update for 1/2 a second

	if ( gInitHUD )
		m_fInitHUD = TRUE;
}

int CBasePlayer::Save( CSave &save )
{
	if ( !CBaseMonster::Save(save) )
		return 0;

	return save.WriteFields( "PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData) );
}


//
// Marks everything as new so the player will resend this to the hud.
//
void CBasePlayer::RenewItems(void)
{

}

extern "C" ps2cvar_t D_003325F0;	/* munição infinita (cheat) */

/* PS2: enche um tipo de munição até o máximo */
#define PS2_FILL_AMMO( name, max ) \
	{ \
		int iAmmo = GetAmmoIndex( name ); \
		if ( iAmmo >= 0 && iAmmo < MAX_AMMO_SLOTS && m_rgAmmo[iAmmo] < (max) ) \
			m_rgAmmo[iAmmo] = (max); \
	}

int CBasePlayer::Restore( CRestore &restore )
{
	if ( !CBaseMonster::Restore(restore) )
		return 0;

	int status = restore.ReadFields( "PLAYER", this, m_playerSaveData, ARRAYSIZE(m_playerSaveData) );

	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;
	// landmark isn't present.
	if ( !pSaveData->fUseLandmark )
	{
		ALERT( at_console, "No Landmark:%s\n", pSaveData->szLandmarkName );

		// default to normal spawn
		edict_t* pentSpawnSpot = EntSelectSpawnPoint( this );
		pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
		pev->angles = VARS(pentSpawnSpot)->angles;
	}
	pev->v_angle.z = 0;	// Clear out roll
	pev->angles = pev->v_angle;

	pev->fixangle = TRUE;           // turn this way immediately

// Copied from spawn() for now
	m_bloodColor	= BLOOD_COLOR_RED;

	g_ulModelIndexPlayer = pev->modelindex;

	if ( FBitSet(pev->flags, FL_DUCKING) ) 
	{
		// Use the crouch HACK
		FixPlayerCrouchStuck( edict() );
		UTIL_SetSize(pev, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX);
	}
	else
	{
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
	}

	RenewItems();

	if ( (CBaseEntity *)m_hTargetedEnemy )
	{
		CBaseEntity *pEnemy = m_hTargetedEnemy;
		pEnemy->pev->effects &= ( ENTINDEX( edict() ) == 1 ) ? ~0x100 : ~0x200;
		m_hTargetedEnemy = NULL;
		m_ps2hUnk79C = NULL;
		(*g_engfuncs.pfnPs2Unk1E0)( ENTINDEX( edict() ), 0 );
	}

	if ( m_fInfiniteAmmo = ( D_003325F0.value > 0 ) )
	{
		ALERT( at_console, "Infinite Ammo.\n" );
		PS2_FILL_AMMO( "9mm", 250 );
		PS2_FILL_AMMO( "ARgrenades", 10 );
		PS2_FILL_AMMO( "buckshot", 125 );
		PS2_FILL_AMMO( "357", 36 );
		PS2_FILL_AMMO( "rockets", 5 );
		PS2_FILL_AMMO( "bolts", 50 );
		PS2_FILL_AMMO( "uranium", 100 );
	}

	if ( D_00332608.value > 0 )
		pev->flags |= FL_NOTARGET;
	else
		pev->flags &= ~FL_NOTARGET;

	if ( D_00332620.value > 0 )
		pev->flags |= FL_GODMODE;
	else
		pev->flags &= ~FL_GODMODE;

	if ( D_00332638.value >= 0 )
		pev->gravity = D_00332638.value;
	else
		pev->gravity = m_flDefaultGravity;

	m_ps2Unk5FC = 1;
	m_iClientFOV = -1;
	intf_playerdead.value = 0;

	return status;
}


void CBasePlayer::SelectNextItem( int iItem )
{
	CBasePlayerItem *pItem;

	pItem = m_rgpPlayerItems[ iItem ];
	
	if (!pItem)
		return;

	if (pItem == m_pActiveItem)
	{
		// select the next one in the chain
		pItem = m_pActiveItem->m_pNext; 
		if (! pItem)
		{
			return;
		}

		CBasePlayerItem *pLast;
		pLast = pItem;
		while (pLast->m_pNext)
			pLast = pLast->m_pNext;

		// relink chain
		pLast->m_pNext = m_pActiveItem;
		m_pActiveItem->m_pNext = NULL;
		m_rgpPlayerItems[ iItem ] = pItem;
	}

	ResetAutoaim( );

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
	{
		m_pActiveItem->Holster( );
	}
	
	m_pActiveItem = pItem;

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
}

void CBasePlayer::SelectItem(const char *pstr)
{
	if (!pstr)
		return;

	CBasePlayerItem *pItem = NULL;

	for (int i = 0; i < MAX_ITEM_TYPES; i++)
	{
		if (m_rgpPlayerItems[i])
		{
			pItem = m_rgpPlayerItems[i];
	
			while (pItem)
			{
				if (FClassnameIs(pItem->pev, pstr))
					break;
				pItem = pItem->m_pNext;
			}
		}

		if (pItem)
			break;
	}

	if (!pItem)
		return;

	
	if (pItem == m_pActiveItem)
		return;

	ResetAutoaim( );

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	m_pLastItem = m_pActiveItem;
	m_pActiveItem = pItem;

	if (m_pActiveItem)
	{
		m_pActiveItem->Deploy( );
		m_pActiveItem->UpdateItemInfo( );
	}
}


void CBasePlayer::SelectLastItem(void)
{
	if (!m_pLastItem)
	{
		return;
	}

	if ( m_pActiveItem && !m_pActiveItem->CanHolster() )
	{
		return;
	}

	ResetAutoaim( );

	// FIX, this needs to queue them up and delay
	if (m_pActiveItem)
		m_pActiveItem->Holster( );
	
	CBasePlayerItem *pTemp = m_pActiveItem;
	m_pActiveItem = m_pLastItem;
	m_pLastItem = pTemp;
	m_pActiveItem->Deploy( );
	m_pActiveItem->UpdateItemInfo( );
}


//==============================================
// HasWeapons - do I have any weapons at all?
//==============================================
BOOL CBasePlayer::HasWeapons( void )
{
	int i;

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( m_rgpPlayerItems[ i ] )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CBasePlayer::SelectPrevItem( int iItem )
{
}


const char *CBasePlayer::TeamID( void )
{
	if ( pev == NULL )		// Not fully connected yet
		return "";

	// return their team name
	return m_szTeamName;
}


//==============================================
// !!!UNDONE:ultra temporary SprayCan entity to apply
// decal frame at a time. For PreAlpha CD
//==============================================
class CSprayCan : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Think( void );

	virtual int	ObjectCaps( void ) { return FCAP_DONT_SAVE; }
};

void CSprayCan::Spawn ( entvars_t *pevOwner )
{
	/* PS2: sem spray no multijogador */
	if ( g_pGameRules->IsMultiplayer() )
	{
		UTIL_Remove( this );
		return;
	}

	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);
	pev->frame = 0;

	pev->nextthink = gpGlobals->time + 0.1f;
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/sprayer.wav", 1, ATTN_NORM);
}

void CSprayCan::Think( void )
{
	TraceResult	tr;	
	int playernum;
	int nFrames;
	CBasePlayer *pPlayer;
	
	pPlayer = (CBasePlayer *)GET_PRIVATE(pev->owner);

	if (pPlayer)
		nFrames = pPlayer->GetCustomDecalFrames();
	else
		nFrames = -1;

	playernum = ENTINDEX(pev->owner);
	
	// ALERT(at_console, "Spray by player %i, %i of %i\n", playernum, (int)(pev->frame + 1), nFrames);

	UTIL_MakeVectors(pev->angles);
	UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);

	// No customization present.
	if (nFrames == -1)
	{
		UTIL_DecalTrace( &tr, DECAL_LAMBDA6 );
		UTIL_Remove( this );
	}
	else
	{
		UTIL_PlayerDecalTrace( &tr, playernum, pev->frame, TRUE );
		// Just painted last custom frame.
		if ( pev->frame++ >= (nFrames - 1))
			UTIL_Remove( this );
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

class	CBloodSplat : public CBaseEntity
{
public:
	void	Spawn ( entvars_t *pevOwner );
	void	Spray ( void );
};

void CBloodSplat::Spawn ( entvars_t *pevOwner )
{
	pev->origin = pevOwner->origin + Vector ( 0 , 0 , 32 );
	pev->angles = pevOwner->v_angle;
	pev->owner = ENT(pevOwner);

	SetThink ( &CBloodSplat::Spray );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CBloodSplat::Spray ( void )
{
	TraceResult	tr;	
	
	if ( g_Language != LANGUAGE_GERMAN )
	{
		UTIL_MakeVectors(pev->angles);
		UTIL_TraceLine ( pev->origin, pev->origin + gpGlobals->v_forward * 128, ignore_monsters, pev->owner, & tr);

		UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
	}
	SetThink ( &CBloodSplat::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

//==============================================



void CBasePlayer::GiveNamedItem( const char *pszName )
{
	edict_t	*pent;

	int istr = MAKE_STRING(pszName);

	pent = CREATE_NAMED_ENTITY(istr);
	if ( FNullEnt( pent ) )
	{
		ALERT ( at_console, "NULL Ent in GiveNamedItem!\n" );
		return;
	}
	VARS( pent )->origin = pev->origin;
	pent->v.spawnflags |= SF_NORESPAWN;

	DispatchSpawn( pent );
	DispatchTouch( pent, ENT( pev ) );
}



CBaseEntity *FindEntityForward( CBaseEntity *pMe )
{
	TraceResult tr;

	UTIL_MakeVectors(pMe->pev->v_angle);
	UTIL_TraceLine(pMe->pev->origin + pMe->pev->view_ofs,pMe->pev->origin + pMe->pev->view_ofs + gpGlobals->v_forward * 8192,dont_ignore_monsters, pMe->edict(), &tr );
	if ( tr.flFraction != 1.0f && !FNullEnt( tr.pHit) )
	{
		CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
		return pHit;
	}
	return NULL;
}


BOOL CBasePlayer :: FlashlightIsOn( void )
{
	return FBitSet(pev->effects, EF_DIMLIGHT);
}


void CBasePlayer :: FlashlightTurnOn( void )
{
	if ( !g_pGameRules->FAllowFlashlight() )
	{
		return;
	}

	if ( (pev->weapons & (1<<WEAPON_SUIT)) )
	{
		EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, SOUND_FLASHLIGHT_ON, 1.0, ATTN_NORM, 0, PITCH_NORM );
		SetBits(pev->effects, EF_DIMLIGHT);
		if ( !(pev->flags & FL_FAKECLIENT) )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgFlashlight, NULL, pev );
			WRITE_BYTE(1);
			WRITE_BYTE(m_iFlashBattery);
			MESSAGE_END();
		}

		m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;

	}
}


void CBasePlayer :: FlashlightTurnOff( void )
{
	EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, SOUND_FLASHLIGHT_OFF, 1.0, ATTN_NORM, 0, PITCH_NORM );
    ClearBits(pev->effects, EF_DIMLIGHT);
	if ( !(pev->flags & FL_FAKECLIENT) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgFlashlight, NULL, pev );
		WRITE_BYTE(0);
		WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();
	}

	m_flFlashLightTime = FLASH_CHARGE_TIME + gpGlobals->time;

}

/*
===============
ForceClientDllUpdate

When recording a demo, we need to have the server tell us the entire client state
so that the client side .dll can behave correctly.
Reset stuff so that the state is transmitted.
===============
*/
void CBasePlayer :: ForceClientDllUpdate( void )
{
	m_iClientHealth  = -1;
	m_iClientBattery = -1;
	m_iTrain |= TRAIN_NEW;  // Force new train message.
	m_fWeapon = FALSE;          // Force weapon send
	m_fKnownItem = FALSE;    // Force weaponinit messages.

	// Now force all the necessary messages
	//  to be sent.
	UpdateClientData();
}


extern ps2cvar_t decalfrequency;

/*
============
ImpulseCommands
============
*/
void CBasePlayer::ImpulseCommands( )
{
	TraceResult	tr;// UNDONE: kill me! This is temporary for PreAlpha CDs

	// Handle use events
	PlayerUse();
		
	int iImpulse = pev->impulse;
	switch (iImpulse)
	{
	case 99:
		{

		int iOn;

		if (!gmsgLogo)
		{
			iOn = 1;
			gmsgLogo = REG_USER_MSG("Logo", 1);
		} 
		else 
		{
			iOn = 0;
		}
		
		ASSERT( gmsgLogo > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgLogo, NULL, pev );
			WRITE_BYTE(iOn);
		MESSAGE_END();

		if(!iOn)
			gmsgLogo = 0;
		break;
		}
	case 100:
		/* PS2: no co-op de um aparelho também mostra/esconde o placar */
		if ( g_pGameRules->IsMultiplayer() && cl_numlocal == 1 )
		{
			const char *pszMap = STRING( gpGlobals->mapname );
			if ( strncmp( "outro", &pszMap[ strlen( pszMap ) - 5 ], 5 ) )
			{
				char szCmd[64];

				if ( m_ps2ShowScores = !m_ps2ShowScores )
					sprintf( szCmd, "+showscores %d\n", ENTINDEX( edict() ) - 1 );
				else
					sprintf( szCmd, "-showscores %d\n", ENTINDEX( edict() ) - 1 );
				CLIENT_COMMAND( edict(), szCmd );
			}
		}

        // temporary flashlight for level designers
        if ( FlashlightIsOn() )
		{
			FlashlightTurnOff();
		}
        else 
		{
			FlashlightTurnOn();
		}
		break;

	case	201:// paint decal
		
		if ( gpGlobals->time < m_flNextDecalTime )
		{
			// too early!
			break;
		}

		UTIL_MakeVectors(pev->v_angle);
		UTIL_TraceLine ( pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, ENT(pev), & tr);

		if ( tr.flFraction != 1.0f )
		{// line hit something, so paint a decal
			m_flNextDecalTime = gpGlobals->time + decalfrequency.value;
			CSprayCan *pCan = GetClassPtr((CSprayCan *)NULL);
			pCan->Spawn( pev );
		}

		break;

	case 204:  //  Demo recording, update client dll specific data again.
		ForceClientDllUpdate(); 
		break;

	default:
		// check all of the cheat impulse commands now
		CheatImpulseCommands( iImpulse );
		break;
	}
	
	pev->impulse = 0;
}

class CGib
{
public:
	static	void SpawnRandomGibs( entvars_t *pevVictim, int cGibs, int human, int fBig );
};

class CGlobalState
{
public:
	void	DumpGlobals( void );
};
extern CGlobalState gGlobalState;

extern float g_flWeaponCheat;

//=========================================================
//=========================================================
void CBasePlayer::CheatImpulseCommands( int iImpulse )
{
	if ( g_flWeaponCheat == 0.0f )
	{
		return;
	}

	CBaseEntity *pEntity;
	TraceResult tr;

	switch ( iImpulse )
	{
	case 76:
		{
			if (!giPrecacheGrunt)
			{
				giPrecacheGrunt = 1;
				ALERT(at_console, "You must now restart to use Grunt-o-matic.\n");
			}
			else
			{
				UTIL_MakeVectors( Vector( 0, pev->v_angle.y, 0 ) );
				Create("monster_human_grunt", pev->origin + gpGlobals->v_forward * 128, pev->angles);
			}
			break;
		}


	case 101:
		gEvilImpulse101 = TRUE;
		/* PS2: sem traje no co-op nem no modo do Ps2Virt07 */
		if ( !g_pGameRules->IsMultiplayer() && !g_pGameRules->Ps2Virt07() )
			GiveNamedItem( "item_suit" );
		GiveNamedItem( "item_battery" );
		GiveNamedItem( "weapon_crowbar" );
		GiveNamedItem( "weapon_9mmhandgun" );
		GiveNamedItem( "ammo_9mmclip" );
		GiveNamedItem( "weapon_shotgun" );
		GiveNamedItem( "ammo_buckshot" );
		GiveNamedItem( "weapon_9mmAR" );
		GiveNamedItem( "ammo_9mmAR" );
		GiveNamedItem( "ammo_ARgrenades" );
		GiveNamedItem( "weapon_handgrenade" );
		GiveNamedItem( "weapon_tripmine" );
		GiveNamedItem( "weapon_islave" );
		GiveNamedItem( "weapon_357" );
		GiveNamedItem( "ammo_357" );
		GiveNamedItem( "weapon_crossbow" );
		GiveNamedItem( "ammo_crossbow" );
		if ( !g_pGameRules->IsMultiplayer() )
			GiveNamedItem( "weapon_egon" );
		if ( !g_pGameRules->IsMultiplayer() )
			GiveNamedItem( "weapon_gauss" );
		if ( !g_pGameRules->IsMultiplayer() )
			GiveNamedItem( "ammo_gaussclip" );
		GiveNamedItem( "weapon_rpg" );
		GiveNamedItem( "ammo_rpgclip" );
		GiveNamedItem( "weapon_satchel" );
		GiveNamedItem( "weapon_snark" );
		if ( !g_pGameRules->IsMultiplayer() )
			GiveNamedItem( "weapon_hornetgun" );
		gEvilImpulse101 = FALSE;
		break;

	case 102:
		// Gibbage!!!
		CGib::SpawnRandomGibs( pev, 1, 1, 0 );
		break;

	case 103:
		// What the hell are you doing?
		pEntity = FindEntityForward( this );
		if ( pEntity )
		{
			CBaseMonster *pMonster = pEntity->MyMonsterPointer();
			if ( pMonster )
				pMonster->ReportAIState();
		}
		break;

	case 104:
		// Dump all of the global state varaibles (and global entity names)
		gGlobalState.DumpGlobals();
		break;

	case	105:// player makes no sound for monsters to hear.
		{
			if ( m_fNoPlayerSound )
			{
				ALERT ( at_console, "Player is audible\n" );
				m_fNoPlayerSound = FALSE;
			}
			else
			{
				ALERT ( at_console, "Player is silent\n" );
				m_fNoPlayerSound = TRUE;
			}
			break;
		}

	case 106:
		// Give me the classname and targetname of this entity.
		pEntity = FindEntityForward( this );
		if ( pEntity )
		{
			ALERT ( at_console, "Classname: %s", STRING( pEntity->pev->classname ) );
			
			if ( !FStringNull ( pEntity->pev->targetname ) )
			{
				ALERT ( at_console, " - Targetname: %s\n", STRING( pEntity->pev->targetname ) );
			}
			else
			{
				ALERT ( at_console, " - TargetName: No Targetname\n" );
			}

			ALERT ( at_console, "Model: %s\n", STRING( pEntity->pev->model ) );
			if ( pEntity->pev->globalname )
				ALERT ( at_console, "Globalname: %s\n", STRING( pEntity->pev->globalname ) );
		}
		break;

	case 107:
		{
			TraceResult tr;

			edict_t		*pWorld = g_engfuncs.pfnPEntityOfEntIndex( 0 );

			Vector start = pev->origin + pev->view_ofs;
			Vector end = start + gpGlobals->v_forward * 1024;
			UTIL_TraceLine( start, end, ignore_monsters, edict(), &tr );
			if ( tr.pHit )
				pWorld = tr.pHit;
			const char *pTextureName = TRACE_TEXTURE( pWorld, start, end );
			if ( pTextureName )
				ALERT( at_console, "Texture: %s\n", pTextureName );
		}
		break;
	case	195:// show shortest paths for entire level to nearest node
		{
			Create("node_viewer_fly", pev->origin, pev->angles);
		}
		break;
	case	196:// show shortest paths for entire level to nearest node
		{
			Create("node_viewer_large", pev->origin, pev->angles);
		}
		break;
	case	197:// show shortest paths for entire level to nearest node
		{
			Create("node_viewer_human", pev->origin, pev->angles);
		}
		break;
	case	199:// show nearest node and all connections
		{
			ALERT ( at_console, "%d\n", WorldGraph.FindNearestNode ( pev->origin, bits_NODE_GROUP_REALM ) );
			WorldGraph.ShowNodeConnections ( WorldGraph.FindNearestNode ( pev->origin, bits_NODE_GROUP_REALM ) );
		}
		break;
	case	202:// Random blood splatter
		UTIL_MakeVectors(pev->v_angle);
		UTIL_TraceLine ( pev->origin + pev->view_ofs, pev->origin + pev->view_ofs + gpGlobals->v_forward * 128, ignore_monsters, ENT(pev), & tr);

		if ( tr.flFraction != 1.0f )
		{// line hit something, so paint a decal
			CBloodSplat *pBlood = GetClassPtr((CBloodSplat *)NULL);
			pBlood->Spawn( pev );
		}
		break;
	case	203:// remove creature.
		pEntity = FindEntityForward( this );
		if ( pEntity )
		{
			if ( pEntity->pev->takedamage )
				pEntity->SetThink(&CBaseEntity::SUB_Remove);
		}
		break;
	case	222:	/* PS2: munição infinita */
		m_fInfiniteAmmo = !m_fInfiniteAmmo;
		ALERT( at_console, "Infinite Ammo.\n" );
		PS2_FILL_AMMO( "9mm", 250 );
		PS2_FILL_AMMO( "ARgrenades", 10 );
		PS2_FILL_AMMO( "buckshot", 125 );
		PS2_FILL_AMMO( "357", 36 );
		PS2_FILL_AMMO( "rockets", 5 );
		PS2_FILL_AMMO( "bolts", 50 );
		PS2_FILL_AMMO( "uranium", 100 );
		break;
	}
}

#ifdef NON_MATCHING
/* casa só com -fmove-all-movables; o arquivo compila sem a flag */
int CBasePlayer::AddPlayerItem( CBasePlayerItem *pItem )
{
	CBasePlayerItem *pInsert;
	
	pInsert = m_rgpPlayerItems[pItem->iItemSlot()];

	while (pInsert)
	{
		if (FClassnameIs( pInsert->pev, STRING( pItem->pev->classname) ))
		{
			/* PS2: a mesma arma já está na lista */
			if ( pItem == pInsert )
				return FALSE;

			if (pItem->AddDuplicate( pInsert ))
			{
				g_pGameRules->PlayerGotWeapon ( this, pItem );
				pItem->CheckRespawn();

				// ugly hack to update clip w/o an update clip message
				pInsert->UpdateItemInfo( );
				if (m_pActiveItem)
					m_pActiveItem->UpdateItemInfo( );

				pItem->Kill( );
			}
			else if (gEvilImpulse101)
			{
				// FIXME: remove anyway for deathmatch testing
				pItem->Kill( );
			}
			return FALSE;
		}
		pInsert = pInsert->m_pNext;
	}


	if (pItem->AddToPlayer( this ))
	{
		g_pGameRules->PlayerGotWeapon ( this, pItem );
		pItem->CheckRespawn();

		pItem->m_pNext = m_rgpPlayerItems[pItem->iItemSlot()];
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem;

		// should we switch to this item?
		if ( g_pGameRules->FShouldSwitchWeapon( this, pItem ) )
		{
			SwitchWeapon( pItem );
		}

		return TRUE;
	}
	else if (gEvilImpulse101)
	{
		// FIXME: remove anyway for deathmatch testing
		pItem->Kill( );
	}
	return FALSE;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/player", AddPlayerItem__11CBasePlayerP15CBasePlayerItem);
#endif



int CBasePlayer::RemovePlayerItem( CBasePlayerItem *pItem )
{
	if (m_pActiveItem == pItem)
	{
		ResetAutoaim( );
		pItem->Holster( );
		pItem->pev->nextthink = 0;// crowbar may be trying to swing again, etc.
		pItem->SetThink( NULL );
		m_pActiveItem = NULL;
		pev->viewmodel = 0;
		pev->weaponmodel = 0;
	}
	else if ( m_pLastItem == pItem )
		m_pLastItem = NULL;

	CBasePlayerItem *pPrev = m_rgpPlayerItems[pItem->iItemSlot()];

	if (pPrev == pItem)
	{
		m_rgpPlayerItems[pItem->iItemSlot()] = pItem->m_pNext;
		return TRUE;
	}
	else
	{
		while (pPrev && pPrev->m_pNext != pItem)
		{
			pPrev = pPrev->m_pNext;
		}
		if (pPrev)
		{
			pPrev->m_pNext = pItem->m_pNext;
			return TRUE;
		}
	}
	return FALSE;
}


//
// Returns the unique ID for the ammo, or -1 if error
//
int CBasePlayer :: GiveAmmo( int iCount, char *szName, int iMax )
{
	if ( !szName )
	{
		// no ammo.
		return -1;
	}

	if ( !g_pGameRules->CanHaveAmmo( this, szName, iMax ) )
	{
		// game rules say I can't have any more of this ammo type.
		return -1;
	}

	int i = 0;

	i = GetAmmoIndex( szName );

	if ( i < 0 || i >= MAX_AMMO_SLOTS )
		return -1;

	int iAdd = min( iCount, iMax - m_rgAmmo[i] );
	if ( iAdd < 1 )
		return i;

	m_rgAmmo[ i ] += iAdd;


	if ( gmsgAmmoPickup )  // make sure the ammo messages have been linked first
	{
		// Send the message that ammo has been picked up
		MESSAGE_BEGIN( MSG_ONE, gmsgAmmoPickup, NULL, pev );
			WRITE_BYTE( GetAmmoIndex(szName) );		// ammo ID
			WRITE_BYTE( iAdd );		// amount
		MESSAGE_END();
	}


	return i;
}


/*
============
ItemPreFrame

Called every frame by the player PreThink
============
*/
void CBasePlayer::ItemPreFrame()
{
    if ( gpGlobals->time < m_flNextAttack )
	{
		return;
	}

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPreFrame( );
}


/*
============
ItemPostFrame

Called every frame by the player PostThink
============
*/
void CBasePlayer::ItemPostFrame()
{
	// check if the player is using a tank
	if ( m_pTank != NULL )
		return;

    if ( gpGlobals->time < m_flNextAttack )
	{
		return;
	}

	ImpulseCommands();

	if (!m_pActiveItem)
		return;

	m_pActiveItem->ItemPostFrame( );
}

int CBasePlayer::AmmoInventory( int iAmmoIndex )
{
	if (iAmmoIndex == -1)
	{
		return -1;
	}

	return m_rgAmmo[ iAmmoIndex ];
}

int CBasePlayer::GetAmmoIndex(const char *psz)
{
	int i;

	if (!psz)
		return -1;

	for (i = 1; i < MAX_AMMO_SLOTS; i++)
	{
		if ( !CBasePlayerItem::AmmoInfoArray[i].pszName )
			continue;

		if (stricmp( psz, CBasePlayerItem::AmmoInfoArray[i].pszName ) == 0)
			return i;
	}

	return -1;
}

// Called from UpdateClientData
// makes sure the client has all the necessary ammo info,  if values have changed
void CBasePlayer::SendAmmoUpdate(void)
{
	for (int i=0; i < MAX_AMMO_SLOTS;i++)
	{
		if (m_rgAmmo[i] != m_rgAmmoLast[i])
		{
			m_rgAmmoLast[i] = m_rgAmmo[i];

			ASSERT( m_rgAmmo[i] >= 0 );
			ASSERT( m_rgAmmo[i] < 255 );

			// send "Ammo" update message
			MESSAGE_BEGIN( MSG_ONE, gmsgAmmoX, NULL, pev );
				WRITE_BYTE( i );
				WRITE_BYTE( max( min( m_rgAmmo[i], 254 ), 0 ) );  // clamp the value to one byte
			MESSAGE_END();
		}
	}
}


extern void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

#define DMG_SHOWNHUD	0x774118	/* tipos de dano com ícone no HUD do PS2 */

/*
=========================================================
	UpdateClientData

resends any changed player HUD info to the client.
Called every frame by PlayerPreThink
Also called at start of demo recording and playback by
ForceClientDllUpdate to ensure the demo gets messages
reflecting all of the HUD state info.
=========================================================
*/
#ifdef NON_MATCHING
/* casa só com -fmove-all-movables; o arquivo compila sem a flag */
void CBasePlayer :: UpdateClientData( void )
{
	if (m_fInitHUD)
	{
		m_fInitHUD = FALSE;
		gInitHUD = FALSE;

		MESSAGE_BEGIN( MSG_ONE, gmsgResetHUD, NULL, pev );
			WRITE_BYTE( 0 );
		MESSAGE_END();

		if ( !m_fGameHUDInitialized )
		{
			MESSAGE_BEGIN( MSG_ONE, gmsgInitHUD, NULL, pev );
			MESSAGE_END();

			g_pGameRules->InitHUD( this );
			m_fGameHUDInitialized = TRUE;
			if ( g_pGameRules->Ps2Virt07() )
			{
				FireTargets( "game_playerjoin", this, this, USE_TOGGLE, 0 );
			}
		}
		FireTargets( "game_playerspawn", this, this, USE_TOGGLE, 0 );
	}

	if ( m_iHideHUD != m_iClientHideHUD )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgHideWeapon, NULL, pev );
			WRITE_BYTE( m_iHideHUD );
		MESSAGE_END();

		m_iClientHideHUD = m_iHideHUD;
	}

	if ( m_iFOV != m_iClientFOV )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgSetFOV, NULL, pev );
			WRITE_BYTE( m_iFOV );
		MESSAGE_END();

		// cache FOV change at end of function, so weapon updates can see that FOV has changed
	}

	// HACKHACK -- send the message to display the game title
	if (gDisplayTitle)
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgShowGameTitle, NULL, pev );
		WRITE_BYTE( 0 );
		MESSAGE_END();
		gDisplayTitle = 0;
	}

	if (pev->health != m_iClientHealth)
	{
		int iHealth = pev->health;
		if ( iHealth < 0 )
			iHealth = 0;  // make sure that no negative health values are sent

		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgHealth, NULL, pev );
			WRITE_BYTE( iHealth );
		MESSAGE_END();

		m_iClientHealth = pev->health;
	}


	if (pev->armorvalue != m_iClientBattery)
	{
		m_iClientBattery = pev->armorvalue;

		ASSERT( gmsgBattery > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgBattery, NULL, pev );
			WRITE_SHORT( (int)pev->armorvalue);
		MESSAGE_END();
	}

	if (pev->dmg_take || pev->dmg_save || m_bitsHUDDamage != m_bitsDamageType)
	{
		// Comes from inside me if not set
		Vector damageOrigin = pev->origin;
		// send "damage" message
		// causes screen to flash, and pain compass to show direction of damage
		edict_t *other = pev->dmg_inflictor;
		if ( other )
		{
			CBaseEntity *pEntity = CBaseEntity::Instance(other);
			if ( pEntity )
				damageOrigin = pEntity->Center();
		}

		// only send down damage type that have hud art
		int visibleDamageBits = m_bitsDamageType & DMG_SHOWNHUD;

		MESSAGE_BEGIN( MSG_ONE, gmsgDamage, NULL, pev );
			WRITE_BYTE( (BYTE)pev->dmg_save );
			WRITE_BYTE( (BYTE)pev->dmg_take );
			WRITE_LONG( visibleDamageBits );
			WRITE_COORD( damageOrigin.x );
			WRITE_COORD( damageOrigin.y );
			WRITE_COORD( damageOrigin.z );
		MESSAGE_END();
	
		pev->dmg_take = 0;
		pev->dmg_save = 0;
		m_bitsHUDDamage = m_bitsDamageType;
		
		// Clear off non-time-based damage indicators
		m_bitsDamageType &= DMG_TIMEBASED;
	}

	// Update Flashlight
	if (((m_flFlashLightTime) && (m_flFlashLightTime <= gpGlobals->time)) || m_ps2Unk5FC)
	{
		if (FlashlightIsOn())
		{
			if (m_iFlashBattery)
			{
				m_flFlashLightTime = FLASH_DRAIN_TIME + gpGlobals->time;
				m_iFlashBattery--;
				
				if (!m_iFlashBattery)
					FlashlightTurnOff();
			}
		}
		else
		{
			if (m_iFlashBattery < 100)
			{
				m_flFlashLightTime = FLASH_CHARGE_TIME + gpGlobals->time;
				m_iFlashBattery++;
			}
			else
				m_flFlashLightTime = 0;
		}

		MESSAGE_BEGIN( MSG_ONE, gmsgFlashBattery, NULL, pev );
		WRITE_BYTE(m_iFlashBattery);
		MESSAGE_END();

		m_ps2Unk5FC = 0;
	}


	if (m_iTrain & TRAIN_NEW)
	{
		ASSERT( gmsgTrain > 0 );
		// send "health" update message
		MESSAGE_BEGIN( MSG_ONE, gmsgTrain, NULL, pev );
			WRITE_BYTE(m_iTrain & 0xF);
		MESSAGE_END();

		m_iTrain &= ~TRAIN_NEW;
	}

	//
	// New Weapon?
	//
	if (!m_fKnownItem)
	{
		m_fKnownItem = TRUE;

		// Send ALL the weapon info now
		int i;

		for (i = 0; i < MAX_WEAPONS; i++)
		{
			ItemInfo& II = CBasePlayerItem::ItemInfoArray[i];

			if ( !II.iId )
				continue;

			const char *pszName;
			if (!II.pszName)
				pszName = "Empty";
			else
				pszName = II.pszName;

			MESSAGE_BEGIN( MSG_ONE, gmsgWeaponList, NULL, pev );  
				WRITE_STRING(pszName);			// string	weapon name
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo1));	// byte		Ammo Type
				WRITE_BYTE(II.iMaxAmmo1);				// byte     Max Ammo 1
				WRITE_BYTE(GetAmmoIndex(II.pszAmmo2));	// byte		Ammo2 Type
				WRITE_BYTE(II.iMaxAmmo2);				// byte     Max Ammo 2
				WRITE_BYTE(II.iSlot);					// byte		bucket
				WRITE_BYTE(II.iPosition);				// byte		bucket pos
				WRITE_BYTE(II.iId);						// byte		id (bit index into pev->weapons)
				WRITE_BYTE(II.iFlags);					// byte		Flags
			MESSAGE_END();
		}
	}


	SendAmmoUpdate();

	// Update all the items
	for ( int i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		if ( m_rgpPlayerItems[i] )  // each item updates it's successors
			m_rgpPlayerItems[i]->UpdateClientData( this );
	}

	// Cache and client weapon change
	m_pClientActiveItem = m_pActiveItem;
	m_iClientFOV = m_iFOV;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/player", UpdateClientData__11CBasePlayer);
#endif

//=========================================================
BOOL CBasePlayer :: FBecomeProne ( void )
{
	m_afPhysicsFlags |= PFLAG_ONBARNACLE;
	return TRUE;
}

//=========================================================
// BarnacleVictimBitten - bad name for a function that is called
// by Barnacle victims when the barnacle pulls their head
// into its mouth. For the player, just die.
//=========================================================
void CBasePlayer :: BarnacleVictimBitten ( entvars_t *pevBarnacle )
{
	TakeDamage ( pevBarnacle, pevBarnacle, pev->health + pev->armorvalue, DMG_SLASH | DMG_ALWAYSGIB );
}

//=========================================================
// BarnacleVictimReleased - overridden for player who has
// physics flags concerns. 
//=========================================================
void CBasePlayer :: BarnacleVictimReleased ( void )
{
	m_afPhysicsFlags &= ~PFLAG_ONBARNACLE;
}


//=========================================================
// Illumination 
// return player light level plus virtual muzzle flash
//=========================================================
int CBasePlayer :: Illumination( void )
{
	int iIllum = CBaseEntity::Illumination( );

	iIllum += m_iWeaponFlash;
	if (iIllum > 255)
		return 255;
	return iIllum;
}


void CBasePlayer :: EnableControl(BOOL fControl)
{
	if (!fControl)
		pev->flags |= FL_FROZEN;
	else
		pev->flags &= ~FL_FROZEN;

}



extern ps2cvar_t *g_psv_aim;

/* PS2: sem auto-mira no modo co-op (Ps2Virt07) e para bots; a mira some
   devagar quando não há alvo travado (m_hTargetedEnemy). */
Vector CBasePlayer :: GetAutoaimVector( float flDelta )
{
	if ( g_pGameRules->Ps2Virt07() || (pev->flags & FL_FAKECLIENT) )
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		if ( m_vecAutoAim.x != m_lastx ||
			 m_vecAutoAim.y != m_lasty )
		{
			SET_CROSSHAIRANGLE( edict(), -m_vecAutoAim.x, m_vecAutoAim.y );

			m_lastx = m_vecAutoAim.x;
			m_lasty = m_vecAutoAim.y;
		}
		UTIL_MakeVectors( pev->v_angle + pev->punchangle );
		return gpGlobals->v_forward;
	}

	if ( g_iSkillLevel == 3 )
		flDelta *= 0.5f;
	else if ( g_iSkillLevel != 2 )
		flDelta += flDelta;

	Vector vecSrc = GetGunPosition( );
	float flDist = 2048;

	m_vecAutoAim = Vector( 0, 0, 0 );

	BOOL m_fOldTargeting = m_fOnTarget;
	Vector angles = AutoaimDeflection(vecSrc, flDist, flDelta );

	// update ontarget if changed
	if (m_fOldTargeting != m_fOnTarget)
	{
		m_pActiveItem->UpdateItemInfo( );
	}

	if (angles.x > 180)
		angles.x -= 360;
	if (angles.x < -180)
		angles.x += 360;
	if (angles.y > 180)
		angles.y -= 360;
	if (angles.y < -180)
		angles.y += 360;

	if (angles.x > 25 || angles.x < -25)
	{
		angles = Vector( 0, 0, 0 );
		m_hAutoAimTarget = NULL;
		if ( !(CBaseEntity *)m_hTargetedEnemy )
			m_fOnTarget = 1;
	}

	if ( !(CBaseEntity *)m_hTargetedEnemy )
	{
		float t = gpGlobals->frametime * 8;
		if ( t > 1 )
			t = 1;
		m_vecAutoAim = m_vecAutoAimPrev * (1 - t) + angles * t;
	}
	else
	{
		m_vecAutoAim = angles;
	}
	m_vecAutoAimPrev = m_vecAutoAim;

	// Don't send across network if sv_aim is 0
	if ( g_psv_aim->value != 0 )
	{
		if ( m_vecAutoAim.x != m_lastx ||
			 m_vecAutoAim.y != m_lasty )
		{
			SET_CROSSHAIRANGLE( edict(), -m_vecAutoAim.x, m_vecAutoAim.y );

			m_lastx = m_vecAutoAim.x;
			m_lasty = m_vecAutoAim.y;
		}
	}

	UTIL_MakeVectors( pev->v_angle + pev->punchangle );
	return gpGlobals->v_forward;
}

/* PS2: troca o realce (useable_graphic) para o objeto usável pObject */
void CBasePlayer :: Ps2HighlightUseable( CBaseEntity *pObject )
{
	if ( !pObject || (CBaseEntity *)m_hLastUseableItem == pObject )
		return;

	if ( m_hUseableHighlight.Get() )
	{
		UTIL_Remove( m_hUseableHighlight );
		m_hUseableHighlight = NULL;
		m_hLastUseableItem = NULL;
	}

	if ( Ps2IsSlave() )
		m_hUseableHighlight = CUseableGraphic::Create( pObject, 3 );
	else if ( cl_numlocal == 1 )
		m_hUseableHighlight = CUseableGraphic::Create( pObject, m_iTeamNum );
	else
		m_hUseableHighlight = CUseableGraphic::Create( pObject, ENTINDEX( edict() ) );

	if ( m_hUseableHighlight.Get() )
	{
		if ( ENTINDEX( edict() ) == 1 )
			m_hUseableHighlight->pev->renderfx = 21;
		else
			m_hUseableHighlight->pev->renderfx = 22;
	}

	m_hLastUseableItem = pObject;
}

void UTIL_TraceLineNoEnts(const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr);

/* o arquivo compila sem -fmove-all-movables por causa desta (o ROM não iça
   2.0/0.5/0.2 nem &gpGlobals no laço).
   PS2: m_fOnTarget vale 1 (sem alvo), 2 (monstro) ou 3 (monstro amigo); o
   inimigo travado (m_hTargetedEnemy) tem prioridade e é mirado no último attachment. */
Vector CBasePlayer :: AutoaimDeflection( Vector &vecSrc, float flDist, float flDelta  )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBaseEntity	*pEntity;
	float		bestdot;
	Vector		bestdir;
	Vector		vecTarget;
	edict_t		*bestent;
	TraceResult tr;

	if ( g_psv_aim->value == 0 )
	{
		if ( !(CBaseEntity *)m_hTargetedEnemy )
			m_fOnTarget = TRUE;
		return g_vecZero;
	}

	if ( (CBaseEntity *)m_hTargetedEnemy && !((CBaseEntity *)m_hTargetedEnemy)->pev->deadflag
		&& ((CBaseEntity *)m_hTargetedEnemy)->pev->takedamage != DAMAGE_NO
		&& !(((CBaseEntity *)m_hTargetedEnemy)->pev->effects & EF_NODRAW) )
	{
		if ((pev->waterlevel != 3 && ((CBaseEntity *)m_hTargetedEnemy)->pev->waterlevel == 3) 
			|| (pev->waterlevel == 3 && ((CBaseEntity *)m_hTargetedEnemy)->pev->waterlevel != 3))
		{
			PS2_RELEASE_TARGET();
		}
		else
		{
			((CBaseAnimating *)(CBaseEntity *)m_hTargetedEnemy)->GetLastAttachment( vecTarget );
			bestdir = (vecTarget - vecSrc).Normalize( );
			bestdir = UTIL_VecToAngles (bestdir);
			bestdir.x = -bestdir.x;
			bestdir = bestdir - pev->v_angle;

			if ( ((CBaseEntity *)m_hTargetedEnemy)->MyMonsterPointer() )
			{
				m_fOnTarget = 2;
				if ( ((CBaseMonster *)(CBaseEntity *)m_hTargetedEnemy)->IRelationship( this ) < 0 )
					m_fOnTarget = 3;
			}
			return bestdir;
		}
	}

	UTIL_MakeVectors( pev->v_angle + m_vecAutoAim );

	// try all possible entities
	bestdir = gpGlobals->v_forward;
	bestdot = flDelta; // +- 10 degrees
	bestent = NULL;

	m_fOnTarget = TRUE;

	UTIL_TraceLineNoEnts( vecSrc, vecSrc + bestdir * flDist, dont_ignore_monsters, edict(), &tr );

	if ( tr.pHit && tr.pHit->v.takedamage != DAMAGE_NO && !(tr.pHit->v.effects & EF_NODRAW) && !(tr.pHit->v.spawnflags & 0x80000) )
	{
		// don't look through water
		if (!((pev->waterlevel != 3 && tr.pHit->v.waterlevel == 3) 
			|| (pev->waterlevel == 3 && tr.pHit->v.waterlevel == 0)))
		{
			if (tr.pHit->v.takedamage == DAMAGE_AIM)
			{
				m_hAutoAimTarget = Instance( tr.pHit );
				if ( ((CBaseEntity *)m_hAutoAimTarget)->MyMonsterPointer() )
				{
					m_fOnTarget = 2;
					if ( ((CBaseMonster *)(CBaseEntity *)m_hAutoAimTarget)->IRelationship( this ) < 0 )
						m_fOnTarget = 3;
				}

				((CBaseAnimating *)(CBaseEntity *)m_hAutoAimTarget)->GetLastAttachment( vecTarget );
				bestdir = (vecTarget - vecSrc).Normalize( );
				bestdir = UTIL_VecToAngles (bestdir);
				bestdir.x = -bestdir.x;
				bestdir = bestdir - pev->v_angle;
				return bestdir;
			}
		}
	}

	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		Vector center;
		Vector dir;
		float dot;

		if ( pEdict->free )	// Not in use
			continue;
		
		if (pEdict->v.takedamage != DAMAGE_AIM)
			continue;
		if (pEdict == edict())
			continue;
		if ( !g_pGameRules->ShouldAutoAim( this, pEdict ) )
			continue;
		if ( pEdict->v.effects & EF_NODRAW )
			continue;

		pEntity = Instance( pEdict );
		if (pEntity == NULL)
			continue;

		if (!pEntity->IsAlive())
			continue;

		// don't look through water
		if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) 
			|| (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
			continue;

		if ( !pEntity->MyMonsterPointer() )
			continue;
		if ( pEntity->pev->spawnflags & 0x80000 )
			continue;

		center = pEntity->Center( );

		dir = (center - vecSrc).Normalize( );

		// make sure it's in front of the player
		if (DotProduct (dir, gpGlobals->v_forward ) < 0)
			continue;

		dot = fabsf( DotProduct (dir, gpGlobals->v_right ) ) 
			+ fabsf( DotProduct (dir, gpGlobals->v_up ) ) * 0.5f;

		// tweek for distance
		dot *= 1.0f + 0.2f * ((center - vecSrc).Length() / flDist);

		if (dot > bestdot)
			continue;	// to far to turn

		UTIL_TraceLineNoEnts( vecSrc, center, dont_ignore_monsters, edict(), &tr );
		if (tr.flFraction != 1.0f && tr.pHit != pEdict)
			continue;

		// can shoot at this one
		bestdot = dot;
		bestent = pEdict;
		bestdir = dir;
	}

	if (bestent)
	{
		bestdir = UTIL_VecToAngles (bestdir);
		bestdir.x = -bestdir.x;
		bestdir = bestdir - pev->v_angle;

		m_hAutoAimTarget = (CBaseEntity *)GET_PRIVATE( bestent );
		if ( ((CBaseEntity *)m_hAutoAimTarget)->MyMonsterPointer() )
		{
			m_fOnTarget = 2;
			if ( ((CBaseMonster *)(CBaseEntity *)m_hAutoAimTarget)->IRelationship( this ) < 0 )
				m_fOnTarget = 3;
		}
		return bestdir;
	}

	m_hAutoAimTarget = NULL;
	return Vector( 0, 0, 0 );
}

void CBasePlayer :: ResetAutoaim( )
{
	if ( cl_numlocal >= 2 )
	{
		Ps2ResetAutoaimOld( );
		return;
	}

	if (m_vecAutoAim.x != 0 || m_vecAutoAim.y != 0)
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		SET_CROSSHAIRANGLE( edict(), 0, 0 );
	}
	if ( !(CBaseEntity *)m_hTargetedEnemy )
		m_fOnTarget = TRUE;
}

/* PS2: o GetAutoaimVector do HLSDK, mantido para a tela dividida (alcance 2048) */
Vector CBasePlayer :: Ps2GetAutoaimVectorOld( float flDelta )
{
	if (g_iSkillLevel == 3)
	{
		UTIL_MakeVectors( pev->v_angle + pev->punchangle );
		return gpGlobals->v_forward;
	}

	Vector vecSrc = GetGunPosition( );
	float flDist = 2048;

	// always use non-sticky autoaim
	// UNDONE: use sever variable to chose!
	if (1 || g_iSkillLevel == 2)
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		// flDelta *= 0.5;
	}

	BOOL m_fOldTargeting = m_fOnTarget;
	Vector angles = Ps2AutoaimDeflectionOld(vecSrc, flDist, flDelta );

	// update ontarget if changed
	if ( !g_pGameRules->AllowAutoTargetCrosshair() )
		m_fOnTarget = 0;
	else if (m_fOldTargeting != m_fOnTarget)
	{
		m_pActiveItem->UpdateItemInfo( );
	}

	if (angles.x > 180)
		angles.x -= 360;
	if (angles.x < -180)
		angles.x += 360;
	if (angles.y > 180)
		angles.y -= 360;
	if (angles.y < -180)
		angles.y += 360;

	if (angles.x > 25)
		angles.x = 25;
	if (angles.x < -25)
		angles.x = -25;
	if (angles.y > 12)
		angles.y = 12;
	if (angles.y < -12)
		angles.y = -12;


	// always use non-sticky autoaim
	// UNDONE: use sever variable to chose!
	if (0 || g_iSkillLevel == 1)
	{
		m_vecAutoAim = m_vecAutoAim * 0.67f + angles * 0.33f;
	}
	else
	{
		m_vecAutoAim = angles * 0.9f;
	}

	// m_vecAutoAim = m_vecAutoAim * 0.99;

	// Don't send across network if sv_aim is 0
	if ( g_psv_aim->value != 0 )
	{
		if ( m_vecAutoAim.x != m_lastx ||
			 m_vecAutoAim.y != m_lasty )
		{
			SET_CROSSHAIRANGLE( edict(), -m_vecAutoAim.x, m_vecAutoAim.y );
			
			m_lastx = m_vecAutoAim.x;
			m_lasty = m_vecAutoAim.y;
		}
	}

	// ALERT( at_console, "%f %f\n", angles.x, angles.y );

	UTIL_MakeVectors( pev->v_angle + pev->punchangle + m_vecAutoAim );
	return gpGlobals->v_forward;
}

/* PS2: o AutoaimDeflection do HLSDK (tela dividida), com UTIL_TraceLineNoEnts */
Vector CBasePlayer :: Ps2AutoaimDeflectionOld( Vector &vecSrc, float flDist, float flDelta  )
{
	edict_t		*pEdict = g_engfuncs.pfnPEntityOfEntIndex( 1 );
	CBaseEntity	*pEntity;
	float		bestdot;
	Vector		bestdir;
	edict_t		*bestent;
	TraceResult tr;
	Vector		center;
	Vector		dir;
	float		dot;

	if ( g_psv_aim->value == 0 )
	{
		m_fOnTarget = FALSE;
		return g_vecZero;
	}

	UTIL_MakeVectors( pev->v_angle + pev->punchangle + m_vecAutoAim );

	// try all possible entities
	bestdir = gpGlobals->v_forward;
	bestdot = flDelta; // +- 10 degrees
	bestent = NULL;

	m_fOnTarget = FALSE;

	UTIL_TraceLineNoEnts( vecSrc, vecSrc + bestdir * flDist, dont_ignore_monsters, edict(), &tr );


	if ( tr.pHit && tr.pHit->v.takedamage != DAMAGE_NO)
	{
		// don't look through water
		if (!((pev->waterlevel != 3 && tr.pHit->v.waterlevel == 3) 
			|| (pev->waterlevel == 3 && tr.pHit->v.waterlevel == 0)))
		{
			if (tr.pHit->v.takedamage == DAMAGE_AIM)
				m_fOnTarget = TRUE;

			return m_vecAutoAim;
		}
	}

	for ( int i = 1; i < gpGlobals->maxEntities; i++, pEdict++ )
	{
		if ( pEdict->free )	// Not in use
			continue;
		
		if (pEdict->v.takedamage != DAMAGE_AIM)
			continue;
		if (pEdict == edict())
			continue;
//		if (pev->team > 0 && pEdict->v.team == pev->team)
//			continue;	// don't aim at teammate
		if ( !g_pGameRules->ShouldAutoAim( this, pEdict ) )
			continue;

		pEntity = Instance( pEdict );
		if (pEntity == NULL)
			continue;

		if (!pEntity->MyMonsterPointer())
			continue;

		if (!pEntity->IsAlive())
			continue;

		// don't look through water
		if ((pev->waterlevel != 3 && pEntity->pev->waterlevel == 3) 
			|| (pev->waterlevel == 3 && pEntity->pev->waterlevel == 0))
			continue;

		center = pEntity->BodyTarget( vecSrc );

		dir = (center - vecSrc).Normalize( );

		// make sure it's in front of the player
		if (DotProduct (dir, gpGlobals->v_forward ) < 0)
			continue;

		dot = fabsf( DotProduct (dir, gpGlobals->v_right ) ) 
			+ fabsf( DotProduct (dir, gpGlobals->v_up ) ) * 0.5f;

		// tweek for distance
		dot *= 1.0f + 0.2f * ((center - vecSrc).Length() / flDist);

		if (dot > bestdot)
			continue;	// to far to turn

		UTIL_TraceLineNoEnts( vecSrc, center, dont_ignore_monsters, edict(), &tr );
		if (tr.flFraction != 1.0f && tr.pHit != pEdict)
		{
			// ALERT( at_console, "hit %s, can't see %s\n", STRING( tr.pHit->v.classname ), STRING( pEdict->v.classname ) );
			continue;
		}

		// don't shoot at friends
		if (IRelationship( pEntity ) < 0)
		{
			if ( !pEntity->IsPlayer() && !g_pGameRules->Ps2Virt08())
				// ALERT( at_console, "friend\n");
				continue;
		}

		// can shoot at this one
		bestdot = dot;
		bestent = pEdict;
		bestdir = dir;
	}

	if (bestent)
	{
		bestdir = UTIL_VecToAngles (bestdir);
		bestdir.x = -bestdir.x;
		bestdir = bestdir - pev->v_angle - pev->punchangle;

		if (bestent->v.takedamage == DAMAGE_AIM)
			m_fOnTarget = TRUE;

		return bestdir;
	}

	return Vector( 0, 0, 0 );
}

void CBasePlayer :: Ps2ResetAutoaimOld( )
{
	if (m_vecAutoAim.x != 0 || m_vecAutoAim.y != 0)
	{
		m_vecAutoAim = Vector( 0, 0, 0 );
		SET_CROSSHAIRANGLE( edict(), 0, 0 );
	}
	m_fOnTarget = FALSE;
}

/*
=============
SetCustomDecalFrames

  UNDONE:  Determine real frame limit, 8 is a placeholder.
  Note:  -1 means no custom frames present.
=============
*/
void CBasePlayer :: SetCustomDecalFrames( int nFrames )
{
	if (nFrames > 0 &&
		nFrames < 8)
		m_nCustomSprayFrames = nFrames;
	else
		m_nCustomSprayFrames = -1;
}

/*
=============
GetCustomDecalFrames

  Returns the # of custom frames this player's custom clan logo contains.
=============
*/
int CBasePlayer :: GetCustomDecalFrames( void )
{
	return m_nCustomSprayFrames;
}

extern "C" ps2cvar_t D_0030FF28;	/* mp_weaponstay */

//=========================================================
// DropPlayerItem - drop the named item, or if no name,
// the active item. 
//=========================================================
void CBasePlayer::DropPlayerItem ( char *pszItemName )
{
	if ( !g_pGameRules->Ps2Virt07() || (D_0030FF28.value > 0) )
	{
		// no dropping in single player.
		return;
	}

	if ( !strlen( pszItemName ) )
	{
		// if this string has no length, the client didn't type a name!
		// assume player wants to drop the active item.
		// make the string null to make future operations in this function easier
		pszItemName = NULL;
	} 

	CBasePlayerItem *pWeapon;
	int i;

	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pWeapon = m_rgpPlayerItems[ i ];

		while ( pWeapon )
		{
			if ( pszItemName )
			{
				// try to match by name. 
				if ( !strcmp( pszItemName, STRING( pWeapon->pev->classname ) ) )
				{
					// match! 
					break;
				}
			}
			else
			{
				// trying to drop active item
				if ( pWeapon == m_pActiveItem )
				{
					// active item!
					break;
				}
			}

			pWeapon = pWeapon->m_pNext; 
		}

		
		// if we land here with a valid pWeapon pointer, that's because we found the 
		// item we want to drop and hit a BREAK;  pWeapon is the item.
		if ( pWeapon )
		{
			g_pGameRules->GetNextBestWeapon( this, pWeapon );

			UTIL_MakeVectors ( pev->angles ); 

			pev->weapons &= ~(1<<pWeapon->m_iId);// take item off hud

			CWeaponBox *pWeaponBox = (CWeaponBox *)CBaseEntity::Create( "weaponbox", pev->origin + gpGlobals->v_forward * 10, pev->angles, edict() );
			pWeaponBox->pev->angles.x = 0;
			pWeaponBox->pev->angles.z = 0;
			pWeaponBox->PackWeapon( pWeapon );
			pWeaponBox->pev->velocity = gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;
			
			// drop half of the ammo for this weapon.
			int	iAmmoIndex;

			iAmmoIndex = GetAmmoIndex ( pWeapon->pszAmmo1() ); // ???
			
			if ( iAmmoIndex != -1 )
			{
				// this weapon weapon uses ammo, so pack an appropriate amount.
				if ( pWeapon->iFlags() & ITEM_FLAG_EXHAUSTIBLE )
				{
					// pack up all the ammo, this weapon is its own ammo type
					pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo1()), m_rgAmmo[ iAmmoIndex ] );
					m_rgAmmo[ iAmmoIndex ] = 0; 

				}
				else
				{
					// pack half of the ammo
					pWeaponBox->PackAmmo( MAKE_STRING(pWeapon->pszAmmo1()), m_rgAmmo[ iAmmoIndex ] / 2 );
					m_rgAmmo[ iAmmoIndex ] /= 2; 
				}

			}

			return;// we're done, so stop searching with the FOR loop.
		}
	}
}

//=========================================================
// HasPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasPlayerItem( CBasePlayerItem *pCheckItem )
{
	CBasePlayerItem *pItem = m_rgpPlayerItems[pCheckItem->iItemSlot()];

	while (pItem)
	{
		if (FClassnameIs( pItem->pev, STRING( pCheckItem->pev->classname) ))
		{
			return TRUE;
		}
		pItem = pItem->m_pNext;
	}

	return FALSE;
}

//=========================================================
// HasNamedPlayerItem Does the player already have this item?
//=========================================================
BOOL CBasePlayer::HasNamedPlayerItem( const char *pszItemName )
{
	CBasePlayerItem *pItem;
	int i;
 
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		pItem = m_rgpPlayerItems[ i ];
		
		while (pItem)
		{
			if ( !strcmp( pszItemName, STRING( pItem->pev->classname ) ) )
			{
				return TRUE;
			}
			pItem = pItem->m_pNext;
		}
	}

	return FALSE;
}

//=========================================================
// 
//=========================================================
BOOL CBasePlayer :: SwitchWeapon( CBasePlayerItem *pWeapon ) 
{
	if ( pWeapon && !pWeapon->CanDeploy() )
	{
		return FALSE;
	}
	
	ResetAutoaim( );
	
	if (m_pActiveItem)
	{
		m_pActiveItem->Holster( );
	}

	m_pActiveItem = pWeapon;

	if ( pWeapon )
	{
		pWeapon->Deploy( );
	}
	else
	{
		pev->viewmodel = 0;
		pev->weaponmodel = 0;
		m_szAnimExtention[0] = 0;
	}

	/* PS2: o parceiro da IA também troca de arma */
	if ( pev->flags & FL_FAKECLIENT )
		Ps2AISwitchWeapon( pWeapon );

	return TRUE;
}

/* PS2: os dois jogadores do co-op estão no mesmo time? */
BOOL CBasePlayer :: Ps2SameTeam( CBaseEntity *pOther )
{
	if ( !pOther || !pOther->IsPlayer() )
		return FALSE;

	return ((CBasePlayer *)pOther)->m_iTeamNum == m_iTeamNum;
}

BOOL CBasePlayer :: Ps2SameTeam( entvars_t *pevOther )
{
	if ( !pevOther )
		return FALSE;

	CBaseEntity *pOther = CBaseEntity::Instance( pevOther );
	if ( !pOther || !pOther->IsPlayer() )
		return FALSE;

	return ((CBasePlayer *)pOther)->m_iTeamNum == m_iTeamNum;
}

int CBasePlayer :: ObjectCaps( void )
{
	/* PS2: o parceiro da IA pode ser usado (+use) pelo jogador */
	if ( pev->flags & FL_FAKECLIENT )
		return FCAP_IMPULSE_USE;

	return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION;
}

extern float UTIL_AngleDiff( float destAngle, float srcAngle );

/* PS2: com um inimigo travado, vira a mira (vecAngles) para ele */
void CBasePlayer :: Ps2TrackTarget( int iPlayerIndex, Vector &vecAngles, Vector &vecTargetPos )
{
	if ( (CBaseEntity *)m_hTargetedEnemy )
	{
		if ( !IsAlive() || ((CBaseEntity *)m_hTargetedEnemy)->pev->deadflag != DEAD_NO
			|| ((CBaseEntity *)m_hTargetedEnemy)->pev->takedamage == DAMAGE_NO
			|| (((CBaseEntity *)m_hTargetedEnemy)->pev->effects & EF_NODRAW) )
		{
			PS2_RELEASE_TARGET();
			return;
		}

		Vector vecNew;
		Vector dir;
		Vector vecTarget;
		Vector angles;
		float flOfs;

		((CBaseEntity *)m_hTargetedEnemy)->pev->effects |= ( ENTINDEX( edict() ) == 1 ) ? 0x100 : 0x200;

		if ( STRING( ((CBaseEntity *)m_hTargetedEnemy)->pev->model )&& ( STRING( ((CBaseEntity *)m_hTargetedEnemy)->pev->model ) )[0] == '*' )
			m_vecLastTargetPos = VecBModelOrigin( ((CBaseEntity *)m_hTargetedEnemy)->pev );
		else
			m_vecLastTargetPos = vecTargetPos;

		dir = (m_vecLastTargetPos - EyePosition()).Normalize();
		vecNew = UTIL_VecToAngles( dir );
		vecNew.x = vecAngles.x;
		vecNew.z = vecAngles.z;

		vecTarget = m_vecLastTargetPos;
		if ( m_ps2Unk7A4 == 1 || ( !g_pGameRules->Ps2Virt07() && !g_pGameRules->IsMultiplayer()
			&& g_iSkillLevel == 1 && ((CBaseEntity *)m_hTargetedEnemy)->pev->deadflag == DEAD_NO ) )
		{
			flOfs = 0;
			m_ps2Unk7A4 = 0;
		}
		else
		{
			flOfs = (((CBaseEntity *)m_hTargetedEnemy)->pev->absmax.z - ((CBaseEntity *)m_hTargetedEnemy)->pev->absmin.z) * 0.5f;
		}

		vecTarget.z += flOfs;
		dir = (vecTarget - EyePosition()).Normalize();
		angles = UTIL_VecToAngles( dir );
		angles.x = -angles.x;
		if ( angles.x > 180 )
			angles.x -= 360;
		else if ( angles.x < -180 )
			angles.x += 360;

		if ( UTIL_AngleDiff( vecAngles.x, angles.x ) <= 0 )
		{
			vecNew.x = angles.x;
		}
		else
		{
			vecTarget = m_vecLastTargetPos;
			vecTarget.z -= flOfs;
			dir = (vecTarget - EyePosition()).Normalize();
			angles = UTIL_VecToAngles( dir );
			angles.x = -angles.x;
			if ( angles.x > 180 )
				angles.x -= 360;
			else if ( angles.x < -180 )
				angles.x += 360;

			if ( UTIL_AngleDiff( vecAngles.x, angles.x ) >= 0 )
				vecNew.x = angles.x;
		}

		vecAngles = vecNew;
	}
	else if ( (CBaseEntity *)m_ps2hUnk79C )
	{
		UTIL_Remove( m_ps2hUnk79C );
		m_ps2hUnk79C = NULL;
		(*g_engfuncs.pfnPs2Unk1E0)( ENTINDEX( edict() ), 0 );
	}
}

/* PS2: trava a mira no alvo atual da auto-mira */
void CBasePlayer :: Ps2AcquireTarget( Vector vecSrc, float flDist, float flDot )
{
	if ( g_psv_aim->value == 0 )
	{
		m_fOnTarget = TRUE;
		return;
	}

	if ( !(CBaseEntity *)m_hAutoAimTarget )
		return;

	m_hTargetedEnemy = m_hAutoAimTarget;
	m_vecLastAngle = pev->v_angle;

	if ( ((CBaseEntity *)m_hAutoAimTarget)->MyMonsterPointer() )
	{
		m_fOnTarget = 2;
		if ( ((CBaseMonster *)(CBaseEntity *)m_hAutoAimTarget)->IRelationship( this ) <= R_NO )
			m_fOnTarget = 3;
	}

	if ( !(CBaseEntity *)m_ps2hUnk79C )
	{
		if ( Ps2IsSlave() )
			m_ps2hUnk79C = CTargetSpot::Create( 3 );
		else if ( cl_numlocal == 1 )
			m_ps2hUnk79C = CTargetSpot::Create( m_iTeamNum );
		else
			m_ps2hUnk79C = CTargetSpot::Create( ENTINDEX( edict() ) );

		((CTargetSpot *)(CBaseEntity *)m_ps2hUnk79C)->SetTarget( m_hTargetedEnemy );
	}

	CBaseEntity *pEnemy = m_hTargetedEnemy;
	pEnemy->pev->effects |= ( ENTINDEX( edict() ) == 1 ) ? 0x100 : 0x200;
	(*g_engfuncs.pfnPs2Unk1E0)( ENTINDEX( edict() ), ENTINDEX( ((CBaseEntity *)m_hTargetedEnemy)->edict() ) );
	m_ps2Unk7A4 = 1;
}

void CBasePlayer :: Ps2Unk58( void )
{
	if ( (CBaseEntity *)m_hTargetedEnemy )
		m_hTargetedEnemy = NULL;
}

/* PS2: o jogador está com a arma do vortigaunt (WEAPON_ISLAVE)? */
BOOL CBasePlayer :: Ps2IsSlave( void )
{
	if ( m_pActiveItem )
	{
		ItemInfo II;
		m_pActiveItem->GetItemInfo( &II );
		if ( II.iId == 16 )
			return TRUE;
	}
	return FALSE;
}

//=========================================================
// Dead HEV suit prop
//=========================================================
class CDeadHEV : public CBaseMonster
{
public:
	void Spawn( void );
	int	Classify ( void ) { return	CLASS_HUMAN_MILITARY; }

	void KeyValue( KeyValueData *pkvd );

	int	m_iPose;// which sequence to display	-- temporary, don't need to save
	static char *m_szPoses[4];
};

char *CDeadHEV::m_szPoses[] = { "deadback", "deadsitting", "deadstomach", "deadtable" };

void CDeadHEV::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "pose"))
	{
		m_iPose = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else 
		CBaseMonster::KeyValue( pkvd );
}

LINK_ENTITY_TO_CLASS( monster_hevsuit_dead, CDeadHEV );

//=========================================================
// ********** DeadHEV SPAWN **********
//=========================================================
void CDeadHEV :: Spawn( void )
{
	PRECACHE_MODEL("models/player.dol");
	SET_MODEL(ENT(pev), "models/player.dol");

	pev->effects		= 0;
	pev->yaw_speed		= 8;
	pev->sequence		= 0;
	pev->body			= 1;
	SetBodygroup( 1, 1 );
	m_bloodColor		= BLOOD_COLOR_RED;

	pev->sequence = LookupSequence( m_szPoses[m_iPose] );

	if (pev->sequence == -1)
	{
		ALERT ( at_console, "Dead hevsuit with bad pose\n" );
		pev->sequence = 0;
	}

	// Corpses have less health
	pev->health			= 8;

	MonsterInitDead();
}

//=========================================================
// Multiplayer intermission spots.
//=========================================================
class CStripWeapons : public CPointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_fStripSuit;		/* PS2: o vortigaunt perde tudo, inclusive o "traje" */
};

LINK_ENTITY_TO_CLASS( player_weaponstrip, CStripWeapons );

TYPEDESCRIPTION	CStripWeapons::m_SaveData[] = 
{
	DEFINE_FIELD( CStripWeapons, m_fStripSuit, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CStripWeapons, CPointEntity );

void CStripWeapons :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "stripsuit"))
	{
		m_fStripSuit = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else 
		CPointEntity::KeyValue( pkvd );
}

void CStripWeapons :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = NULL;

	if ( pActivator && pActivator->IsPlayer() )
	{
		pPlayer = (CBasePlayer *)pActivator;
	}
	else if ( !g_pGameRules->Ps2Virt08() )
	{
		pPlayer = (CBasePlayer *)CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );
	}

	if ( pPlayer && (pPlayer->m_iHideHUD & HIDEHUD_PS2_NOPICKUP) && m_fStripSuit )
		pPlayer->RemoveAllItems( TRUE );
	else if ( pPlayer && !(pPlayer->m_iHideHUD & HIDEHUD_PS2_NOPICKUP) )
		pPlayer->RemoveAllItems( FALSE );
}


class CRevertSaved : public CPointEntity
{
public:
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT MessageThink( void );
	void	EXPORT LoadThink( void );
	void	KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	inline	float	Duration( void ) { return pev->dmg_take; }
	inline	float	HoldTime( void ) { return pev->dmg_save; }
	inline	float	MessageTime( void ) { return m_messageTime; }
	inline	float	LoadTime( void ) { return m_loadTime; }

	inline	void	SetDuration( float duration ) { pev->dmg_take = duration; }
	inline	void	SetHoldTime( float hold ) { pev->dmg_save = hold; }
	inline	void	SetMessageTime( float time ) { m_messageTime = time; }
	inline	void	SetLoadTime( float time ) { m_loadTime = time; }

private:
	float	m_messageTime;
	float	m_loadTime;
};

LINK_ENTITY_TO_CLASS( player_loadsaved, CRevertSaved );

TYPEDESCRIPTION	CRevertSaved::m_SaveData[] = 
{
	DEFINE_FIELD( CRevertSaved, m_messageTime, FIELD_FLOAT ),	// These are not actual times, but durations, so save as floats
	DEFINE_FIELD( CRevertSaved, m_loadTime, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CRevertSaved, CPointEntity );

void CRevertSaved :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "duration"))
	{
		SetDuration( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		SetHoldTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "messagetime"))
	{
		SetMessageTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "loadtime"))
	{
		SetLoadTime( atof(pkvd->szValue) );
		pkvd->fHandled = TRUE;
	}
	else 
		CPointEntity::KeyValue( pkvd );
}

void CRevertSaved :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	UTIL_ScreenFadeAll( pev->rendercolor, Duration(), HoldTime(), pev->renderamt, FFADE_OUT );
	pev->nextthink = gpGlobals->time + MessageTime();
	SetThink( &CRevertSaved::MessageThink );
}


void CRevertSaved :: MessageThink( void )
{
	UTIL_ShowMessageAll( STRING(pev->message) );
	float nextThink = LoadTime() - MessageTime();
	if ( nextThink > 0 ) 
	{
		pev->nextthink = gpGlobals->time + nextThink;
		SetThink( &CRevertSaved::LoadThink );
	}
	else
		LoadThink();
}


void CRevertSaved :: LoadThink( void )
{
	if ( !gpGlobals->deathmatch )
	{
		SERVER_COMMAND("reload\n");
	}
}

//=========================================================
// Multiplayer intermission spots.
//=========================================================
class CInfoIntermission:public CPointEntity
{
	void Spawn( void );
	void Think( void );
};

void CInfoIntermission::Spawn( void )
{
	UTIL_SetOrigin( pev, pev->origin );
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;
	pev->v_angle = g_vecZero;

	pev->nextthink = gpGlobals->time + 2;// let targets spawn!

}

void CInfoIntermission::Think ( void )
{
	edict_t *pTarget;

	// find my target
	pTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(pev->target) );

	if ( !FNullEnt(pTarget) )
	{
		pev->v_angle = UTIL_VecToAngles( (pTarget->v.origin - pev->origin).Normalize() );
		pev->v_angle.x = -pev->v_angle.x;
	}
}

LINK_ENTITY_TO_CLASS( info_intermission, CInfoIntermission );

//=========================================================
// PS2: marcador da mira (targeting_spot)
//=========================================================
CTargetSpot *CTargetSpot::Create( int iType )
{
	CTargetSpot *pSpot = GetClassPtr( (CTargetSpot *)NULL );

	pSpot->Spawn();
	pSpot->pev->classname = MAKE_STRING( "targeting_spot" );
	pSpot->pev->movetype = MOVETYPE_NONE;
	pSpot->pev->solid = SOLID_NOT;
	pSpot->pev->rendermode = kRenderTransAdd;

	if ( iType == 3 )
	{
		pSpot->pev->rendercolor.x = 0;
		pSpot->pev->rendercolor.y = 160;
		pSpot->pev->rendercolor.z = 0;
	}
	else if ( !g_pGameRules->IsMultiplayer() && !g_pGameRules->Ps2Virt08() )
	{
		pSpot->pev->rendercolor.x = 255;
		pSpot->pev->rendercolor.y = 160;
		pSpot->pev->rendercolor.z = 0;
	}
	else if ( iType == 1 )
	{
		pSpot->pev->rendercolor.x = 160;
		pSpot->pev->rendercolor.y = 160;
		pSpot->pev->rendercolor.z = 192;
	}
	else
	{
		pSpot->pev->rendercolor.x = 255;
		pSpot->pev->rendercolor.y = 128;
		pSpot->pev->rendercolor.z = 64;
	}

	pSpot->pev->renderamt = 255;
	SET_MODEL( ENT( pSpot->pev ), "sprites/bracket.spz" );
	UTIL_SetOrigin( pSpot->pev, pSpot->pev->origin );

	return pSpot;
}

void CTargetSpot::Spawn( void )
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->rendermode = kRenderTransAdd;
	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->renderamt = 255;
	SET_MODEL( ENT( pev ), "sprites/bracket.spz" );
	UTIL_SetOrigin( pev, pev->origin );
}

void CTargetSpot::Hide( void )
{
	pev->effects |= EF_NODRAW;
}

void CTargetSpot::SetTarget( CBaseEntity *pTarget )
{
	if ( !pTarget )
		return;

	UTIL_SetOrigin( pev, pTarget->pev->origin );
	pev->skin = ENTINDEX( pTarget->edict() );

	if ( pTarget->MyMonsterPointer() )
		pev->body = ((CBaseAnimating *)pTarget)->GetLastAttachmentIndex() + 1;
	else
		pev->body = 0;

	pev->aiment = pTarget->edict();
	pev->movetype = MOVETYPE_FOLLOW;
	pev->scale = 3.0f;
	SetThink( &CTargetSpot::ShrinkThink );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CTargetSpot::ShrinkThink( void )
{
	if ( pev->scale > 1.0f )
	{
		pev->scale -= 0.5f;
		pev->nextthink = gpGlobals->time + 0.1f;

		if ( pev->scale == 1.0f && pev->rendercolor.z == 64.0f )
			pev->scale = 0.9f;
	}
	else
	{
		SetThink( NULL );
	}
}

void CTargetSpot::Precache( void )
{
	PRECACHE_MODEL( "sprites/bracket.spz" );
}

//=========================================================
// PS2: realce do objeto usável (useable_graphic)
//=========================================================
LINK_ENTITY_TO_CLASS( useable_graphic, CUseableGraphic );

void CUseableGraphic::Spawn( void )
{
	Precache();
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;
	pev->rendermode = kRenderGlow;
	pev->rendercolor.x = 255;
	pev->rendercolor.y = 255;
	pev->rendercolor.z = 255;
	pev->renderamt = 255;
	pev->effects = 0;
	SET_MODEL( ENT( pev ), "sprites/bracket_02.spz" );
	UTIL_SetOrigin( pev, pev->origin );
}

void CUseableGraphic::Precache( void )
{
	PRECACHE_MODEL( "sprites/bracket_02.spz" );
}

CUseableGraphic *CUseableGraphic::Create( CBaseEntity *pTarget, int iType )
{
	CUseableGraphic *pGraphic;

	if ( !pTarget )
		return NULL;

	pGraphic = GetClassPtr( (CUseableGraphic *)NULL );
	if ( !pGraphic )
		return NULL;

	pGraphic->Spawn();
	pGraphic->pev->classname = MAKE_STRING( "useable_graphic" );

	Vector vecOrigin = pTarget->pev->origin;
	BOOL fBModel = FALSE;

	if ( pTarget->IsBSPModel() )
		fBModel = TRUE;
	else if ( STRING( pTarget->pev->model ) )
		fBModel = ( STRING( pTarget->pev->model ) )[0] == '*';

	if ( fBModel == TRUE )
		vecOrigin = VecBModelOrigin( pTarget->pev );

	float flZ = fabsf( ( pTarget->pev->absmax.z - pTarget->pev->absmin.z ) * 0.5f );
	float flX = fabsf( ( pTarget->pev->absmax.x - pTarget->pev->absmin.x ) * 0.5f );
	float flY = fabsf( ( pTarget->pev->absmax.y - pTarget->pev->absmin.y ) * 0.5f );

	pGraphic->pev->scale = flZ;
	if ( pGraphic->pev->scale < flX )
		pGraphic->pev->scale = flX;
	if ( pGraphic->pev->scale < flY )
		pGraphic->pev->scale = flY;
	if ( pGraphic->pev->scale > 32.0f )
		pGraphic->pev->scale = 32.0f;

	if ( !strcmp( "item_eyescanner", STRING( pTarget->pev->classname ) ) )
	{
		UTIL_MakeVectors( pTarget->pev->angles );
		vecOrigin = vecOrigin + gpGlobals->v_right * -2.3f;
		vecOrigin.z += 45.0f;
		pGraphic->pev->scale *= 0.5f;
	}
	else if ( !strcmp( "item_healthcharger", STRING( pTarget->pev->classname ) )
		|| !strcmp( "item_recharge", STRING( pTarget->pev->classname ) ) )
	{
		vecOrigin.z += 15.0f;
	}
	else if ( !strcmp( "func_tankcontrols", STRING( pTarget->pev->classname ) ) )
	{
		pGraphic->pev->scale *= 0.25f;
	}

	pGraphic->pev->origin = vecOrigin;
	UTIL_SetOrigin( pGraphic->pev, vecOrigin );

	if ( iType == 3 )
	{
		pGraphic->pev->rendercolor.x = 0;
		pGraphic->pev->rendercolor.y = 160;
		pGraphic->pev->rendercolor.z = 0;
	}
	else if ( !g_pGameRules->IsMultiplayer() && !g_pGameRules->Ps2Virt08() )
	{
		pGraphic->pev->rendercolor.x = 255;
		pGraphic->pev->rendercolor.y = 160;
		pGraphic->pev->rendercolor.z = 0;
	}
	else if ( iType == 1 )
	{
		pGraphic->pev->rendercolor.x = 160;
		pGraphic->pev->rendercolor.y = 160;
		pGraphic->pev->rendercolor.z = 192;
	}
	else
	{
		pGraphic->pev->rendercolor.x = 255;
		pGraphic->pev->rendercolor.y = 128;
		pGraphic->pev->rendercolor.z = 64;
	}

	return pGraphic;
}

/* registro dos ponteiros de Think para o save do PS2 */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );

extern "C" void func_001D2BF0( void )
{
	func_002397D0("236", static_cast<BASEPTR>(&CTargetSpot::ShrinkThink));
	func_002397D0("237", static_cast<BASEPTR>(&CBloodSplat::SUB_Remove));
	func_002397D0("238", static_cast<BASEPTR>(&CRevertSaved::LoadThink));
	func_002397D0("239", static_cast<BASEPTR>(&CBloodSplat::Spray));
	func_002397D0("240", static_cast<BASEPTR>(&CBaseEntity::SUB_Remove));
	func_002397D0("241", static_cast<BASEPTR>(&CRevertSaved::MessageThink));
	func_002397D0("242", static_cast<BASEPTR>(&CBasePlayer::PlayerDeathThink));
}
