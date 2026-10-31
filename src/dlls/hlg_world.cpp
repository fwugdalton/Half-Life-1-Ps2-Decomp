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
/* dlls/hlg_world.cpp: o world.cpp do HLSDK com o nome que a Gearbox deu no PS2.
   Mudanças do PS2: o CopyToBodyQue foi esvaziado, o corpo ganhou um fade-out,
   o CWorld guarda listas de armas e munições vindas das keyvalues "weapon_*" e
   "ammo_*" (passadas ao W_Precache no multiplayer) e o coop ganhou a keyvalue
   "slavecoop", que escolhe o modelo do jogador. */
/*

===== world.cpp ========================================================

  precaches and defs for entities and other data that must always be available.

*/

typedef struct _FILE FILE;

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/nodes.h"
#include "dlls/soundent.h"
#include "dlls/decals.h"
#include "dlls/skill.h"
#include "dlls/effects.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "dlls/gamerules.h"
#include "dlls/teamplay_gamerules.h"
#include "include_asm.h"

extern "C" void *Mem_DbgCalloc( int size, int count, const char *file, int line );
extern "C" void Mem_Free( void *ptr );
#define calloc(a, b)	Mem_DbgCalloc( a, b, "dlls/hlg_world.cpp", 363 )
#define free(p)			Mem_Free( p )

extern "C" char *strcpy( char *dest, const char *src );
extern "C" int strncmp( const char *s1, const char *s2, unsigned int n );
extern "C" int atoi( const char *s );
extern "C" double atof( const char *s );

extern CGraph WorldGraph;
extern CSoundEnt *pSoundEnt;

/* globais do PS2 ainda sem nome */
extern "C" CBaseEntity *D_00317D84;
#define g_pLastSpawn D_00317D84
extern "C" int D_003241F0;
extern "C" int D_00312C48;
#define gDisplayTitle D_00312C48
extern "C" int D_00317958;
#define gInitHUD D_00317958

extern void W_Precache( int *piszWeapons, int *piszAmmo, int iCount );
extern void ClientPrecache( void );
extern CGameRules *InstallGameRules( void );
extern void SENTENCEG_Init( void );
extern void TEXTURETYPE_Init( void );

typedef enum { GLOBAL_OFF = 0, GLOBAL_ON = 1, GLOBAL_DEAD = 2 } GLOBALESTATE;

typedef struct globalentity_s globalentity_t;

struct globalentity_s
{
	char			name[64];
	char			levelName[32];
	GLOBALESTATE	state;
	globalentity_t	*pNext;
};

class CGlobalState
{
public:
					CGlobalState();
	void			Reset( void );
	void			ClearStates( void );
	void			EntityAdd( string_t globalname, string_t mapName, GLOBALESTATE state );
	void			EntitySetState( string_t globalname, GLOBALESTATE state );
	void			EntityUpdate( string_t globalname, string_t mapname );
	const globalentity_t	*EntityFromTable( string_t globalname );
	GLOBALESTATE	EntityGetState( string_t globalname );
	int				EntityInTable( string_t globalname ) { return (Find( globalname ) != NULL) ? 1 : 0; }
	int				Save( CSave &save );
	int				Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

//#ifdef _DEBUG
	void			DumpGlobals( void );
//#endif

private:
	globalentity_t	*Find( string_t globalname );
	globalentity_t	*m_pList;
	int				m_listCount;
};

class CWorld : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	int m_rgiszWeapons[32];
	int m_rgiszAmmo[32];
};

DLL_GLOBAL edict_t				*g_pBodyQueueHead;
CGlobalState					gGlobalState;
float							g_flWeaponCheat;

DLL_DECALLIST gDecals[] = {
	{ "{shot1",	0 },		// DECAL_GUNSHOT1 
	{ "{shot2",	0 },		// DECAL_GUNSHOT2
	{ "{shot3",0 },			// DECAL_GUNSHOT3
	{ "{shot4",	0 },		// DECAL_GUNSHOT4
	{ "{shot5",	0 },		// DECAL_GUNSHOT5
	{ "{lambda01", 0 },		// DECAL_LAMBDA1
	{ "{lambda02", 0 },		// DECAL_LAMBDA2
	{ "{lambda03", 0 },		// DECAL_LAMBDA3
	{ "{lambda04", 0 },		// DECAL_LAMBDA4
	{ "{lambda05", 0 },		// DECAL_LAMBDA5
	{ "{lambda06", 0 },		// DECAL_LAMBDA6
	{ "{scorch1", 0 },		// DECAL_SCORCH1
	{ "{scorch2", 0 },		// DECAL_SCORCH2
	{ "{blood1", 0 },		// DECAL_BLOOD1
	{ "{blood2", 0 },		// DECAL_BLOOD2
	{ "{blood3", 0 },		// DECAL_BLOOD3
	{ "{blood4", 0 },		// DECAL_BLOOD4
	{ "{blood5", 0 },		// DECAL_BLOOD5
	{ "{blood6", 0 },		// DECAL_BLOOD6
	{ "{yblood1", 0 },		// DECAL_YBLOOD1
	{ "{yblood2", 0 },		// DECAL_YBLOOD2
	{ "{yblood3", 0 },		// DECAL_YBLOOD3
	{ "{yblood4", 0 },		// DECAL_YBLOOD4
	{ "{yblood5", 0 },		// DECAL_YBLOOD5
	{ "{yblood6", 0 },		// DECAL_YBLOOD6
	{ "{break1", 0 },		// DECAL_GLASSBREAK1
	{ "{break2", 0 },		// DECAL_GLASSBREAK2
	{ "{break3", 0 },		// DECAL_GLASSBREAK3
	{ "{bigshot1", 0 },		// DECAL_BIGSHOT1
	{ "{bigshot2", 0 },		// DECAL_BIGSHOT2
	{ "{bigshot3", 0 },		// DECAL_BIGSHOT3
	{ "{bigshot4", 0 },		// DECAL_BIGSHOT4
	{ "{bigshot5", 0 },		// DECAL_BIGSHOT5
	{ "{spit1", 0 },		// DECAL_SPIT1
	{ "{spit2", 0 },		// DECAL_SPIT2
	{ "{bproof1", 0 },		// DECAL_BPROOF1
	{ "{gargstomp", 0 },	// DECAL_GARGSTOMP1,	// Gargantua stomp crack
	{ "{smscorch1", 0 },	// DECAL_SMALLSCORCH1,	// Small scorch mark
	{ "{smscorch2", 0 },	// DECAL_SMALLSCORCH2,	// Small scorch mark
	{ "{smscorch3", 0 },	// DECAL_SMALLSCORCH3,	// Small scorch mark
	{ "{mommablob", 0 },	// DECAL_MOMMABIRTH		// BM Birth spray
	{ "{mommablob", 0 },	// DECAL_MOMMASPLAT		// BM Mortar spray?? need decal
};

/*
==============================================================================

BODY QUE

==============================================================================
*/

#define SF_DECAL_NOTINDEATHMATCH		2048

class CDecal : public CBaseEntity
{
public:
	void	Spawn( void );
	void	KeyValue( KeyValueData *pkvd );
	void	EXPORT StaticDecal( void );
	void	EXPORT TriggerDecal( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( infodecal, CDecal );

// UNDONE:  These won't get sent to joining players in multi-player
void CDecal :: Spawn( void )
{
	if ( pev->skin < 0 || (gpGlobals->deathmatch && FBitSet( pev->spawnflags, SF_DECAL_NOTINDEATHMATCH )) )
	{
		REMOVE_ENTITY(ENT(pev));
		return;
	}

	if ( FStringNull ( pev->targetname ) )
	{
		SetThink( &CDecal::StaticDecal );
		// if there's no targetname, the decal will spray itself on as soon as the world is done spawning.
		pev->nextthink = gpGlobals->time;
	}
	else
	{
		// if there IS a targetname, the decal sprays itself on when it is triggered.
		SetThink ( &CDecal::SUB_DoNothing );
		SetUse( &CDecal::TriggerDecal);
	}
}

void CDecal :: TriggerDecal ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// this is set up as a USE function for infodecals that have targetnames, so that the
	// decal doesn't get applied until it is fired. (usually by a scripted sequence)
	TraceResult trace;
	int			entityIndex;

	UTIL_TraceLine( pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace );

	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY);
		WRITE_BYTE( TE_BSPDECAL );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_SHORT( (int)pev->skin );
		entityIndex = (short)ENTINDEX(trace.pHit);
		WRITE_SHORT( entityIndex );
		if ( entityIndex )
			WRITE_SHORT( (int)VARS(trace.pHit)->modelindex );
	MESSAGE_END();

	SetThink( &CDecal::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}


void CDecal :: StaticDecal( void )
{
	TraceResult trace;
	int			entityIndex, modelIndex;

	UTIL_TraceLine( pev->origin - Vector(5,5,5), pev->origin + Vector(5,5,5),  ignore_monsters, ENT(pev), &trace );

	entityIndex = (short)ENTINDEX(trace.pHit);
	if ( entityIndex )
		modelIndex = (int)VARS(trace.pHit)->modelindex;
	else
		modelIndex = 0;

	g_engfuncs.pfnStaticDecal( pev->origin, (int)pev->skin, entityIndex, modelIndex );

	SUB_Remove();
}


void CDecal :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "texture"))
	{
		pev->skin = DECAL_INDEX( pkvd->szValue );
		
		// Found
		if ( pev->skin >= 0 )
			return;
		ALERT( at_console, "Can't find decal %s\n", pkvd->szValue );
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


// Body queue class here.... It's really just CBaseEntity
class CCorpse : public CBaseEntity
{
	virtual int ObjectCaps( void ) { return FCAP_DONT_SAVE; }	
public:
	void StartFadeOut( void );
	void EXPORT FadeOut( void );
};

LINK_ENTITY_TO_CLASS( bodyque, CCorpse );

static void InitBodyQue(void)
{
	string_t	istrClassname = MAKE_STRING("bodyque");

	g_pBodyQueueHead = CREATE_NAMED_ENTITY( istrClassname );
	entvars_t *pev = VARS(g_pBodyQueueHead);
	
	// Reserve 3 more slots for dead bodies
	for ( int i = 0; i < 3; i++ )
	{
		pev->owner = CREATE_NAMED_ENTITY( istrClassname );
		pev = VARS(pev->owner);
	}
	
	pev->owner = g_pBodyQueueHead;
}

/* fade-out do corpo, do PS2 (ninguém chama) */
void CCorpse::StartFadeOut( void )
{
	pev->renderamt = 255;
	pev->rendermode = kRenderTransTexture;
	pev->solid = SOLID_NOT;
	pev->avelocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.1f;
	SetThink( &CCorpse::FadeOut );
}

void CCorpse::FadeOut( void )
{
	if ( pev->renderamt > 7 )
	{
		pev->renderamt -= 7;
		pev->nextthink = gpGlobals->time + 0.1f;
	}
	else
	{
		pev->renderamt = 0;
	}
}

//
// make a body que entry for the given ent so the ent can be respawned elsewhere
//
// GLOBALS ASSUMED SET:  g_eoldenemy
//
void CopyToBodyQue(entvars_t *pev) 
{
	/* esvaziado no PS2 */
}


CGlobalState::CGlobalState( void )
{
	Reset();
}

void CGlobalState::Reset( void )
{
	m_pList = NULL; 
	m_listCount = 0;
}

globalentity_t *CGlobalState :: Find( string_t globalname )
{
	if ( !globalname )
		return NULL;

	globalentity_t *pTest;
	const char *pEntityName = STRING(globalname);

	
	pTest = m_pList;
	while ( pTest )
	{
		if ( FStrEq( pEntityName, pTest->name ) )
			break;
	
		pTest = pTest->pNext;
	}

	return pTest;
}


// This is available all the time now on impulse 104, remove later
//#ifdef _DEBUG
void CGlobalState :: DumpGlobals( void )
{
	static char *estates[] = { "Off", "On", "Dead" };
	globalentity_t *pTest;

	ALERT( at_console, "-- Globals --\n" );
	pTest = m_pList;
	while ( pTest )
	{
		ALERT( at_console, "%s: %s (%s)\n", pTest->name, pTest->levelName, estates[pTest->state] );
		pTest = pTest->pNext;
	}
}
//#endif


void CGlobalState :: EntityAdd( string_t globalname, string_t mapName, GLOBALESTATE state )
{
	globalentity_t *pNewEntity = (globalentity_t *)calloc( sizeof( globalentity_t ), 1 );
	pNewEntity->pNext = m_pList;
	m_pList = pNewEntity;
	strcpy( pNewEntity->name, STRING( globalname ) );
	strcpy( pNewEntity->levelName, STRING(mapName) );
	pNewEntity->state = state;
	m_listCount++;
}


void CGlobalState :: EntitySetState( string_t globalname, GLOBALESTATE state )
{
	globalentity_t *pEnt = Find( globalname );

	if ( pEnt )
		pEnt->state = state;
}


const globalentity_t *CGlobalState :: EntityFromTable( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );

	return pEnt;
}


GLOBALESTATE CGlobalState :: EntityGetState( string_t globalname )
{
	globalentity_t *pEnt = Find( globalname );
	if ( pEnt )
		return pEnt->state;

	return GLOBAL_OFF;
}


// Global Savedata for Delay
TYPEDESCRIPTION	CGlobalState::m_SaveData[] = 
{
	DEFINE_FIELD( CGlobalState, m_listCount, FIELD_INTEGER ),
};

// Global Savedata for Delay
TYPEDESCRIPTION	gGlobalEntitySaveData[] = 
{
	DEFINE_ARRAY( globalentity_t, name, FIELD_CHARACTER, 64 ),
	DEFINE_ARRAY( globalentity_t, levelName, FIELD_CHARACTER, 32 ),
	DEFINE_FIELD( globalentity_t, state, FIELD_INTEGER ),
};


int CGlobalState::Save( CSave &save )
{
	int i;
	globalentity_t *pEntity;

	if ( !save.WriteFields( "GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData) ) )
		return 0;
	
	pEntity = m_pList;
	for ( i = 0; i < m_listCount && pEntity; i++ )
	{
		if ( !save.WriteFields( "GENT", pEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData) ) )
			return 0;

		pEntity = pEntity->pNext;
	}
	
	return 1;
}

int CGlobalState::Restore( CRestore &restore )
{
	int i, listCount;
	globalentity_t tmpEntity;


	ClearStates();
	if ( !restore.ReadFields( "GLOBAL", this, m_SaveData, ARRAYSIZE(m_SaveData) ) )
		return 0;
	
	listCount = m_listCount;	// Get new list count
	m_listCount = 0;				// Clear loaded data

	for ( i = 0; i < listCount; i++ )
	{
		if ( !restore.ReadFields( "GENT", &tmpEntity, gGlobalEntitySaveData, ARRAYSIZE(gGlobalEntitySaveData) ) )
			return 0;
		EntityAdd( MAKE_STRING(tmpEntity.name), MAKE_STRING(tmpEntity.levelName), tmpEntity.state );
	}
	return 1;
}

void CGlobalState::EntityUpdate( string_t globalname, string_t mapname )
{
	globalentity_t *pEnt = Find( globalname );

	if ( pEnt )
		strcpy( pEnt->levelName, STRING(mapname) );
}


void CGlobalState::ClearStates( void )
{
	globalentity_t *pFree = m_pList;
	while ( pFree )
	{
		globalentity_t *pNext = pFree->pNext;
		free( pFree );
		pFree = pNext;
	}
	Reset();
}


void SaveGlobalState( SAVERESTOREDATA *pSaveData )
{
	CSave saveHelper( pSaveData );
	gGlobalState.Save( saveHelper );
}


void RestoreGlobalState( SAVERESTOREDATA *pSaveData )
{
	CRestore restoreHelper( pSaveData );
	gGlobalState.Restore( restoreHelper );
}


void ResetGlobalState( void )
{
	gGlobalState.ClearStates();
	gInitHUD = TRUE;	// Init the HUD on a new game / load game
}

// moved CWorld class definition to cbase.h
//=======================
// CWorld
//
// This spawns first when each level begins.
//=======================

LINK_ENTITY_TO_CLASS( worldspawn, CWorld );

#define SF_WORLD_DARK		0x0001		// Fade from black at startup
#define SF_WORLD_TITLE		0x0002		// Display game title at startup
#define SF_WORLD_FORCETEAM	0x0004		// Force teams
#define SF_MESSAGE_ONCE		0x0001
#define SF_WORLD_SLAVECOOP	0x0020		// PS2: coop com o modelo do escravo

extern DLL_GLOBAL BOOL		g_fGameOver;

void CWorld :: Spawn( void )
{
	g_fGameOver = FALSE;
	Precache( );
	g_flWeaponCheat = CVAR_GET_FLOAT( "sv_cheats" );

	for ( int i = 0; i < 32; i++ )
	{
		m_rgiszWeapons[i] = 0;
		m_rgiszAmmo[i] = 0;
	}

	D_003241F0 = 0;
}

void CWorld :: Precache( void )
{
	D_003241F0 = 0;
	g_pLastSpawn = NULL;
	
#if 1
	CVAR_SET_STRING("sv_gravity", "800"); // 67ft/sec
	CVAR_SET_STRING("sv_stepsize", "18");
#else
	CVAR_SET_STRING("sv_gravity", "384"); // 32ft/sec
	CVAR_SET_STRING("sv_stepsize", "24");
#endif

	CVAR_SET_STRING("room_type", "0");// clear DSP

	// Set up game rules
	if (g_pGameRules)
	{
		delete g_pGameRules;
	}

	g_pGameRules = InstallGameRules( );

	//!!!UNDONE why is there so much Spawn code in the Precache function? I'll just keep it here 

	///!!!LATER - do we want a sound ent in deathmatch? (sjb)
	//pSoundEnt = CBaseEntity::Create( "soundent", g_vecZero, g_vecZero, edict() );
	pSoundEnt = GetClassPtr( ( CSoundEnt *)NULL );
	pSoundEnt->Spawn();

	if ( !pSoundEnt )
	{
		ALERT ( at_console, "**COULD NOT CREATE SOUNDENT**\n" );
	}

	InitBodyQue();
	
// init sentence group playback stuff from sentences.txt.
// ok to call this multiple times, calls after first are ignored.

	SENTENCEG_Init();

// init texture type array from materials.txt

	TEXTURETYPE_Init();


// the area based ambient sounds MUST be the first precache_sounds

// player precaches     
	if ( g_pGameRules->IsMultiplayer() )
	{
		if ( m_rgiszWeapons[0] )
			W_Precache( m_rgiszWeapons, m_rgiszAmmo, 32 );	// get weapon precaches
		else
			W_Precache( NULL, NULL, 0 );
	}
	else
	{
		W_Precache( NULL, NULL, 0 );
	}

	ClientPrecache();

// sounds used from C physics code
	PRECACHE_SOUND("common/null.wav");				// clears sound channels

	PRECACHE_SOUND( "items/suitchargeok1.wav" );//!!! temporary sound for respawning weapons.
	PRECACHE_SOUND( "items/gunpickup2.wav" );// player picks up a gun.

	PRECACHE_SOUND( "common/bodydrop3.wav" );// dead bodies hitting the ground (animation events)
	PRECACHE_SOUND( "common/bodydrop4.wav" );
	
	g_Language = (int)CVAR_GET_FLOAT( "sv_language" );
	if ( g_Language == LANGUAGE_GERMAN )
	{
		PRECACHE_MODEL( "models/germangibs.dol" );
	}
	else
	{
		PRECACHE_MODEL( "models/hgibs.dol" );
		PRECACHE_MODEL( "models/agibs.dol" );
	}

//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	LIGHT_STYLE(0, "m");
	
	// 1 FLICKER (first variety)
	LIGHT_STYLE(1, "mmnmmommommnonmmonqnmmo");
	
	// 2 SLOW STRONG PULSE
	LIGHT_STYLE(2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");
	
	// 3 CANDLE (first variety)
	LIGHT_STYLE(3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");
	
	// 4 FAST STROBE
	LIGHT_STYLE(4, "mamamamamama");
	
	// 5 GENTLE PULSE 1
	LIGHT_STYLE(5,"jklmnopqrstuvwxyzyxwvutsrqponmlkj");
	
	// 6 FLICKER (second variety)
	LIGHT_STYLE(6, "nmonqnmomnmomomno");
	
	// 7 CANDLE (second variety)
	LIGHT_STYLE(7, "mmmaaaabcdefgmmmmaaaammmaamm");
	
	// 8 CANDLE (third variety)
	LIGHT_STYLE(8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");
	
	// 9 SLOW STROBE (fourth variety)
	LIGHT_STYLE(9, "aaaaaaaazzzzzzzz");
	
	// 10 FLUORESCENT FLICKER
	LIGHT_STYLE(10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	LIGHT_STYLE(11, "abcdefghijklmnopqrrqponmlkjihgfedcba");
	
	// 12 UNDERWATER LIGHT MUTATION
	// this light only distorts the lightmap - no contribution
	// is made to the brightness of affected surfaces
	LIGHT_STYLE(12, "mmnnmmnnnmmnn");
	
	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	LIGHT_STYLE(63, "a");

	for ( int i = 0; i < ARRAYSIZE(gDecals); i++ )
		gDecals[i].index = DECAL_INDEX( gDecals[i].name );

// init the WorldGraph.
	WorldGraph.InitGraph();

// make sure the .NOD file is newer than the .BSP file.
	if ( !WorldGraph.CheckNODFile ( ( char * )STRING( gpGlobals->mapname ) ) )
	{// NOD file is not present, or is older than the BSP file.
		WorldGraph.AllocNodes ();
	}
	else
	{// Load the node graph for this level
		if ( !WorldGraph.FLoadGraph ( (char *)STRING( gpGlobals->mapname ) ) )
		{// couldn't load, so alloc and prepare to build a graph.
			WorldGraph.AllocNodes ();
		}
		else
		{
			ALERT ( at_console, "\n*Graph Loaded!\n" );
		}
	}

	if ( pev->speed > 0 )
		CVAR_SET_FLOAT( "sv_zmax", pev->speed );
	else
		CVAR_SET_FLOAT( "sv_zmax", 4096 );

	if ( pev->netname )
	{
		ALERT( at_aiconsole, "Chapter title: %s\n", STRING(pev->netname) );
		CBaseEntity *pEntity = CBaseEntity::Create( "env_message", g_vecZero, g_vecZero, NULL );
		if ( pEntity )
		{
			pEntity->SetThink( &CBaseEntity::SUB_CallUseToggle );
			pEntity->pev->message = pev->netname;
			pev->netname = 0;
			pEntity->pev->nextthink = gpGlobals->time + 0.3f;
			pEntity->pev->spawnflags = SF_MESSAGE_ONCE;
		}
	}

	if ( pev->spawnflags & SF_WORLD_DARK )
		CVAR_SET_FLOAT( "v_dark", 1.0 );
	else
		CVAR_SET_FLOAT( "v_dark", 0.0 );

	if ( pev->spawnflags & SF_WORLD_TITLE )
		gDisplayTitle = TRUE;		// display the game title if this key is set
	else
		gDisplayTitle = FALSE;

	if ( pev->spawnflags & SF_WORLD_FORCETEAM )
	{
		CVAR_SET_FLOAT( "mp_defaultteam", 1 );
	}
	else
	{
		CVAR_SET_FLOAT( "mp_defaultteam", 0 );
	}

	if ( gpGlobals->coop )
	{
		if ( pev->spawnflags & SF_WORLD_SLAVECOOP )
		{
			CVAR_SET_FLOAT( "mp_coopslave", 1 );
			PRECACHE_MODEL( "models/player/dm_slave/dm_slave.dol" );
		}
		else
		{
			CVAR_SET_FLOAT( "mp_coopslave", 0 );
			PRECACHE_MODEL( "models/player/ginacol/ginacol.dol" );
		}
	}

	SERVER_COMMAND( "host_framerate 0\n" );
}


//
// Just to ignore the "wad" field.
//
void CWorld :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq(pkvd->szKeyName, "skyname") )
	{
		// Sent over net now.
		CVAR_SET_STRING( "sv_skyname", pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "sounds") )
	{
		gpGlobals->cdAudioTrack = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "WaveHeight") )
	{
		// Sent over net now.
		pev->scale = atof(pkvd->szValue) * (1.0/8.0);
		pkvd->fHandled = TRUE;
		CVAR_SET_FLOAT( "sv_wateramp", pev->scale );
	}
	else if ( FStrEq(pkvd->szKeyName, "MaxRange") )
	{
		pev->speed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "chaptertitle") )
	{
		pev->netname = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "startdark") )
	{
		// UNDONE: This is a gross hack!!! The CVAR is NOT sent over the client/sever link
		// but it will work for single player
		int flag = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
		if ( flag )
			pev->spawnflags |= SF_WORLD_DARK;
	}
	else if ( FStrEq(pkvd->szKeyName, "newunit") )
	{
		// Single player only.  Clear save directory if set
		if ( atoi(pkvd->szValue) )
			CVAR_SET_FLOAT( "sv_newunit", 1 );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "gametitle") )
	{
		if ( atoi(pkvd->szValue) )
			pev->spawnflags |= SF_WORLD_TITLE;

		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "mapteams") )
	{
		pev->team = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "defaultteam") )
	{
		if ( atoi(pkvd->szValue) )
		{
			pev->spawnflags |= SF_WORLD_FORCETEAM;
		}
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq(pkvd->szKeyName, "slavecoop") )
	{
		if ( atoi(pkvd->szValue) )
		{
			pev->spawnflags |= SF_WORLD_SLAVECOOP;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );

	/* PS2: "weapon_xxx"/"ammo_xxx" da worldspawn entram na lista do W_Precache */
	if ( !pkvd->fHandled )
	{
		if ( !strncmp( "weapon_", pkvd->szKeyName, 7 ) || !strncmp( "ammo_", pkvd->szKeyName, 5 ) )
		{
			for ( int i = 0; i < 32; i++ )
			{
				if ( !m_rgiszWeapons[i] )
				{
					m_rgiszWeapons[i] = ALLOC_STRING( pkvd->szKeyName );
					if ( !strncmp( "weapon_", pkvd->szValue, 7 ) || !strncmp( "ammo_", pkvd->szValue, 5 ) )
						m_rgiszAmmo[i] = ALLOC_STRING( pkvd->szValue );
					else
						m_rgiszAmmo[i] = 0;
					pkvd->fHandled = TRUE;
					break;
				}
			}
		}
	}
}

/* registro dos ponteiros de Think e Use para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0018D298(void)
{
	func_002397D0("151", static_cast<BASEPTR>(&CBaseEntity::SUB_CallUseToggle));
	func_002397D0("152", static_cast<BASEPTR>(&CDecal::SUB_DoNothing));
	func_002397D0("153", static_cast<BASEPTR>(&CDecal::StaticDecal));
	func_00239810("154", static_cast<USEPTR>(&CDecal::TriggerDecal));
	func_002397D0("155", static_cast<BASEPTR>(&CCorpse::FadeOut));
	func_002397D0("156", static_cast<BASEPTR>(&CDecal::SUB_Remove));
}
