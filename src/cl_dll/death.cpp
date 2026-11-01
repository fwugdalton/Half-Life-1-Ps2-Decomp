/* cl_dll/death.cpp: CHudDeathNotice do HLSDK, versão do PS2. Uma lista de avisos por
   jogador local (rgDeathNoticeList[2][5], com o clientId dono em cada item), sem cores
   de time, sem gViewPort/espectador e sem o caso "killed a" (morte de objeto). */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

extern "C" {
char *strcpy( char *dst, const char *src );
char *strncpy( char *dst, const char *src, unsigned int n );
char *func_002E61B8( char *dst, const char *src, unsigned int n );	/* strncat */
void *func_002E46A4( void *dst, const void *src, unsigned int n );	/* memmove */
void *memset( void *dst, int c, unsigned int n );
int strcmp( const char *a, const char *b );
}
#define strncat func_002E61B8
#define memmove func_002E46A4

#undef CVAR_GET_FLOAT
inline float CVAR_GET_FLOAT( const char *x ) { return gEngfuncs.pfnGetCvarFloat( (char*)x ); }
#define CVAR_CREATE( x, y, z ) gEngfuncs.pfnRegisterVariable( (char*)x, (char*)y, z )

#define min(a,b)	(((a) < (b)) ? (a) : (b))

inline int ConsoleStringLen( const char *string )
{
	int _width, _height;
	GetConsoleStringSize( string, &_width, &_height );
	return _width;
}

/* .rodata do arquivo em asm: as strings são usadas pelas funções NON_MATCHING */
__asm__(
".section .rodata\n"
".align 3\n"
".globl D_00338F00\n"
"D_00338F00:\n"
".word 0x74616544\n"
".word 0x67734D68\n"
".word 0x00000000\n"
".word 0x00000000\n"
".globl D_00338F10\n"
"D_00338F10:\n"
".word 0x5F647568\n"
".word 0x74616564\n"
".word 0x746F6E68\n"
".word 0x5F656369\n"
".word 0x656D6974\n"
".word 0x00000000\n"
".globl D_00338F28\n"
"D_00338F28:\n"
".word 0x00000036\n"
".word 0x00000000\n"
".globl D_00338F30\n"
"D_00338F30:\n"
".word 0x6B735F64\n"
".word 0x006C6C75\n"
".globl D_00338F38\n"
"D_00338F38:\n"
".word 0x00005F64\n"
".word 0x00000000\n"
".globl D_00338F40\n"
"D_00338F40:\n"
".word 0x00000000\n"
".word 0x00000000\n"
".globl D_00338F48\n"
"D_00338F48:\n"
".word 0x65745F64\n"
".word 0x616D6D61\n"
".word 0x00006574\n"
".word 0x00000000\n"
".globl D_00338F58\n"
"D_00338F58:\n"
".word 0x6F775F64\n"
".word 0x00646C72\n"
".globl D_00338F60\n"
"D_00338F60:\n"
".word 0x65696420\n"
".word 0x00000064\n"
".globl D_00338F68\n"
"D_00338F68:\n"
".word 0x6C696B20\n"
".word 0x2064656C\n"
".word 0x666C6573\n"
".word 0x00000000\n"
".globl D_00338F78\n"
"D_00338F78:\n"
".word 0x6C696B20\n"
".word 0x2064656C\n"
".word 0x20736968\n"
".word 0x6D616574\n"
".word 0x6574616D\n"
".word 0x00000020\n"
".globl D_00338F90\n"
"D_00338F90:\n"
".word 0x6C696B20\n"
".word 0x2064656C\n"
".word 0x00000000\n"
".word 0x00000000\n"
".globl D_00338FA0\n"
"D_00338FA0:\n"
".word 0x74697720\n"
".word 0x00002068\n"
".globl D_00338FA8\n"
"D_00338FA8:\n"
".word 0x6E6F6765\n"
".word 0x00000000\n"
".globl D_00338FB0\n"
"D_00338FB0:\n"
".word 0x6C675F64\n"
".word 0x206E6F75\n"
".word 0x006E7567\n"
".word 0x00000000\n"
".globl D_00338FC0\n"
"D_00338FC0:\n"
".word 0x73756167\n"
".word 0x00000073\n"
".globl D_00338FC8\n"
"D_00338FC8:\n"
".word 0x61745F64\n"
".word 0x61632075\n"
".word 0x6E6F6E6E\n"
".word 0x00000000\n"
".globl D_00338FD8\n"
"D_00338FD8:\n"
".word 0x0000000A\n"
".word 0x00000000\n"
".previous\n"
);
extern "C" const char D_00338F00[], D_00338F10[], D_00338F28[], D_00338F30[];

DECLARE_MESSAGE( m_DeathNotice, DeathMsg );

#define MAX_PLAYER_NAME_LENGTH	32

struct DeathNoticeItem {
	char szKiller[MAX_PLAYER_NAME_LENGTH];	/* 0x0 */
	char szVictim[MAX_PLAYER_NAME_LENGTH];	/* 0x20 */
	int iId;	// the index number of the associated sprite	/* 0x40 */
	int iSuicide;		/* 0x44 */
	int iTeamKill;		/* 0x48 */
	float flDisplayTime;	/* 0x4C */
	int iClient;		/* 0x50: PS2, jogador local dono do aviso */
};

#define MAX_DEATHNOTICES	4
static int DEATHNOTICE_DISPLAY_TIME = 6;

#define DEATHNOTICE_TOP		42

DeathNoticeItem rgDeathNoticeList[2][ MAX_DEATHNOTICES + 1 ] = { 0 };

int CHudDeathNotice :: Init( int clientId )
{
	gHUD[clientId].AddHudElem( this );

	gEngfuncs.pfnHookUserMsg( (char*)D_00338F00, __MsgFunc_DeathMsg, clientId );

	if ( !clientId )
		CVAR_CREATE( D_00338F10, D_00338F28, 0 );	/* "hud_deathnotice_time", "6" */

	return 1;
}

void CHudDeathNotice :: InitHUDData( void )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}

int CHudDeathNotice :: VidInit( int clientId )
{
	m_HUD_d_skull = gHUD[clientId].GetSpriteIndex( D_00338F30 );	/* "d_skull" */

	return 1;
}

void CHudDeathNotice :: Reset( int clientId )
{
	memset( rgDeathNoticeList, 0, sizeof(rgDeathNoticeList) );
}

int CHudDeathNotice :: Draw( float flTime, int clientId, int iDrawFlags )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/death", Draw__15CHudDeathNoticefii);
#else

	int x, y, r, g, b;

	if ( !( iDrawFlags & 2 ) )
		return 1;

	for ( int i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[clientId][i].iId == 0 )
			break;  // we've gone through them all

		if ( rgDeathNoticeList[clientId][i].iClient != clientId )
			continue;

		if ( rgDeathNoticeList[clientId][i].flDisplayTime < flTime )
		{ // display time has expired
			// remove the current item from the list
			memmove( &rgDeathNoticeList[clientId][i], &rgDeathNoticeList[clientId][i+1], sizeof(DeathNoticeItem) * (MAX_DEATHNOTICES - i) );
			i--;  // continue on the next item;  stop the counter getting incremented
			continue;
		}

		rgDeathNoticeList[clientId][i].flDisplayTime = min( rgDeathNoticeList[clientId][i].flDisplayTime, gHUD[clientId].m_flTime + DEATHNOTICE_DISPLAY_TIME );

		// Draw the death notice
		y = DEATHNOTICE_TOP + 2 + (20 * i);

		int id = (rgDeathNoticeList[clientId][i].iId == -1) ? m_HUD_d_skull : rgDeathNoticeList[clientId][i].iId;
		x = ScreenWidth - ConsoleStringLen(rgDeathNoticeList[clientId][i].szVictim) - (gHUD[clientId].GetSpriteRect(id).right - gHUD[clientId].GetSpriteRect(id).left) - 8;

		if ( !rgDeathNoticeList[clientId][i].iSuicide )
		{
			x -= (5 + ConsoleStringLen( rgDeathNoticeList[clientId][i].szKiller ) );

			// Draw killers name
			x = 5 + DrawConsoleString( x, y, rgDeathNoticeList[clientId][i].szKiller );
		}

		r = 255;  g = 80;	b = 0;
		if ( rgDeathNoticeList[clientId][i].iTeamKill )
		{
			r = 10;	g = 240; b = 10;  // display it in sickly green
		}

		// Draw death weapon
		SPR_Set( gHUD[clientId].GetSprite(id), r, g, b );
		SPR_DrawAdditive( 0, x, y, &gHUD[clientId].GetSpriteRect(id) );

		x += (gHUD[clientId].GetSpriteRect(id).right - gHUD[clientId].GetSpriteRect(id).left);

		// Draw victims name
		x = DrawConsoleString( x, y, rgDeathNoticeList[clientId][i].szVictim );
	}

	return 1;
#endif
}

// This message handler may be better off elsewhere
int CHudDeathNotice :: MsgFunc_DeathMsg( int clientId, const char *pszName, int iSize, void *pbuf )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/death", MsgFunc_DeathMsg__15CHudDeathNoticeiPCciPv);
#else

	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );

	int killer = READ_BYTE();
	int victim = READ_BYTE();

	char killedwith[32];
	strcpy( killedwith, "d_" );
	strncat( killedwith, READ_STRING(), 32 );

	gHUD[clientId].m_Scoreboard.GetAllPlayersInfo( clientId );
	gHUD[clientId].m_Scoreboard.DeathMsg( killer, victim, clientId );

	int i;
	for ( i = 0; i < MAX_DEATHNOTICES; i++ )
	{
		if ( rgDeathNoticeList[clientId][i].iId == 0 )
			break;
	}
	if ( i == MAX_DEATHNOTICES )
	{ // move the rest of the list forward to make room for this item
		memmove( rgDeathNoticeList[clientId], rgDeathNoticeList[clientId]+1, sizeof(DeathNoticeItem) * MAX_DEATHNOTICES );
		i = MAX_DEATHNOTICES - 1;
	}

	// Get the Killer's name
	char *killer_name = gHUD[clientId].m_Scoreboard.m_PlayerInfoList[ killer ].name;
	char *victim_name = gHUD[clientId].m_Scoreboard.m_PlayerInfoList[ victim ].name;
	if ( !killer_name )
		killer_name = "";
	if ( !victim_name )
		victim_name = "";

	strncpy( rgDeathNoticeList[clientId][i].szKiller, killer_name, MAX_PLAYER_NAME_LENGTH );
	strncpy( rgDeathNoticeList[clientId][i].szVictim, victim_name, MAX_PLAYER_NAME_LENGTH );

	if ( killer == victim || killer == 0 )
		rgDeathNoticeList[clientId][i].iSuicide = 1;

	if ( !strcmp( killedwith, "d_teammate" ) )
		rgDeathNoticeList[clientId][i].iTeamKill = 1;

	// Find the sprite in the list
	int spr = gHUD[clientId].GetSpriteIndex( killedwith );

	rgDeathNoticeList[clientId][i].iId = spr;

	DEATHNOTICE_DISPLAY_TIME = CVAR_GET_FLOAT( "hud_deathnotice_time" );
	rgDeathNoticeList[clientId][i].flDisplayTime = gHUD[clientId].m_flTime + DEATHNOTICE_DISPLAY_TIME;
	rgDeathNoticeList[clientId][i].iClient = clientId;

	// record the death notice in the console
	if ( rgDeathNoticeList[clientId][i].iSuicide )
	{
		ConsolePrint( rgDeathNoticeList[clientId][i].szVictim );

		if ( !strcmp( killedwith, "d_world" ) )
		{
			ConsolePrint( " died" );
		}
		else
		{
			ConsolePrint( " killed self" );
		}
	}
	else if ( rgDeathNoticeList[clientId][i].iTeamKill )
	{
		ConsolePrint( rgDeathNoticeList[clientId][i].szKiller );
		ConsolePrint( " killed his teammate " );
		ConsolePrint( rgDeathNoticeList[clientId][i].szVictim );
	}
	else
	{
		ConsolePrint( rgDeathNoticeList[clientId][i].szKiller );
		ConsolePrint( " killed " );
		ConsolePrint( rgDeathNoticeList[clientId][i].szVictim );
	}

	if ( killedwith && *killedwith && (*killedwith > 13 ) && strcmp( killedwith, "d_world" ) && !rgDeathNoticeList[clientId][i].iTeamKill )
	{
		ConsolePrint( " with " );

		// replace the code names with the 'real' names
		if ( !strcmp( killedwith+2, "egon" ) )
			strcpy( killedwith, "d_gluon gun" );
		if ( !strcmp( killedwith+2, "gauss" ) )
			strcpy( killedwith, "d_tau cannon" );

		ConsolePrint( killedwith+2 ); // skip over the "d_" part
	}

	ConsolePrint( "\n" );

	return 1;
#endif
}
