/* cl_dll/scoreboard.cpp: CHudScoreboard do HLSDK, versão do PS2 (placar por jogador,
   colunas e posições vindas dos textos SCORE_* do titles.txt, coluna de precisão no
   lugar do ping, frags no canto da tela na tela dividida). */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

extern "C" {
char *strncpy( char *dst, const char *src, unsigned int n );
void *memset( void *dst, int c, unsigned int n );
int sprintf( char *buf, const char *fmt, ... );
int atoi( const char *s );
int strcasecmp( const char *a, const char *b );
}

#define stricmp strcasecmp

#define TRUE 1
#define FALSE 0

#define max(a,b)	(((a) > (b)) ? (a) : (b))


extern "C" int cl_numlocal;

/* static char buf[64] do DrawPlayers, no .bss (fora do objeto) */
extern "C" char D_003A3230[64];

DECLARE_COMMAND( m_Scoreboard, ShowScores );
DECLARE_COMMAND( m_Scoreboard, HideScores );

DECLARE_MESSAGE( m_Scoreboard, ScoreInfo );
DECLARE_MESSAGE( m_Scoreboard, TeamInfo );
DECLARE_MESSAGE( m_Scoreboard, TeamScore );

int CHudScoreboard :: Init( int clientId )
{
	gHUD[clientId].AddHudElem( this );

	if ( clientId == 0 )
	{
		HOOK_COMMAND( "+showscores", ShowScores );
		HOOK_COMMAND( "-showscores", HideScores );
	}

	HOOK_MESSAGE( ScoreInfo );
	HOOK_MESSAGE( TeamScore );
	HOOK_MESSAGE( TeamInfo );

	InitHUDData( clientId );

	return 1;
}

int CHudScoreboard :: VidInit( int clientId )
{
	// Load sprites here

	return 1;
}

void CHudScoreboard :: InitHUDData( int clientId )
{
	memset( m_PlayerExtraInfo, 0, sizeof m_PlayerExtraInfo );
	m_iLastKilledBy = 0;
	m_fLastKillTime = 0;
	m_iPlayerNum = 0;
	m_iNumTeams = 0;
	m_iShowscoresHeld = 0;
	memset( m_TeamInfo, 0, sizeof m_TeamInfo );

	m_iFlags &= ~HUD_ACTIVE;  // starts out inactive

	m_iFlags |= HUD_INTERMISSION; // is always drawn during an intermission
}

// Y positions
#define ROW_GAP  23
#define ROW_RANGE_MIN 130
#define ROW_RANGE_MAX ( ScreenHeight - 50 )

#ifndef NON_MATCHING
/* as strings do Draw e do DrawPlayers (que ficam em asm) saem aqui no .rodata */
__asm__(".section .rodata\n.align 3\nD_003393E8:\n.asciz \"SCORE_ACCURACY\"\n.align 3\nD_003393F8:\n.asciz \"SCORE_PLAYER\"\n.align 3\nD_00339408:\n.asciz \"SCORE_KILLS\"\n.align 3\nD_00339418:\n.asciz \"/\"\n.align 3\nD_00339420:\n.asciz \"SCORE_DEATHS\"\n.align 3\nD_00339430:\n.asciz \"SCORE_DAMAGE\"\n.align 3\nD_00339440:\n.asciz \"\"\n.previous\n");
#endif

int CHudScoreboard :: Draw( float flTime, int clientId, int iDrawFlags )
{
#ifndef NON_MATCHING
	/* o gcc não deixa o 154.0f do 1º ypos sem converter e aloca os registradores de
	   outro jeito; o asm traz as strings SCORE_*, "/" e "" */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/scoreboard", Draw__14CHudScoreboardfii );
#else
	client_textmessage_t *pPlayer, *pKills, *pDeaths, *pDamage, *pAccuracy;
	int i, j;
	float list_slot = 0;

	if ( ( iDrawFlags & 0xA ) && !gHUD[clientId].m_Teamplay && cl_numlocal >= 2 )
	{
		int iWidth = gHUD[clientId].GetSpriteRect( gHUD[clientId].m_HUD_number_0 ).right - gHUD[clientId].GetSpriteRect( gHUD[clientId].m_HUD_number_0 ).left;
		gHUD[clientId].DrawHudNumber( ScreenWidth - iWidth * 3 - 24, gHUD[clientId].m_iFontHeight / 2 + 24, DHN_DRAWZERO,
			m_PlayerExtraInfo[clientId == 0 ? 1 : 2].frags, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB );
	}

	if ( !( iDrawFlags & 0xA4 ) )
		return 1;

	if ( !m_iShowscoresHeld && gHUD[clientId].m_Health.m_iHealth > 0 && !gHUD[clientId].m_iIntermission )
		return 1;

	if ( !m_iShowscoresHeld && gHUD[clientId].m_Teamplay == 1 )
		return 1;

	GetAllPlayersInfo( clientId );

	// just sort the list on the fly
	// list is sorted first by frags, then by deaths
	int xpos_rel = ( ScreenWidth - 40 - (int)TextMessageGet( "SCORE_ACCURACY" )->y ) / 2;

	// print the heading line
	pPlayer = TextMessageGet( "SCORE_PLAYER" );
	int ypos = (int)( ROW_RANGE_MIN + ( list_slot * ROW_GAP ) + 24 );
	int xpos = (int)pPlayer->x + xpos_rel;

	if ( !gHUD[clientId].m_Teamplay )
		gHUD[clientId].DrawHudString( xpos, ypos, (int)pPlayer->y + xpos_rel, (char *)pPlayer->pMessage, 255, 140, 0, clientId );
	else
		gHUD[clientId].DrawHudString( xpos, ypos, (int)pPlayer->y + xpos_rel, (char *)pPlayer->pMessage, 255, 140, 0, clientId );

	pKills = TextMessageGet( "SCORE_KILLS" );
	gHUD[clientId].DrawHudStringReverse( (int)pKills->y + xpos_rel, ypos, 0, (char *)pKills->pMessage, 255, 140, 0, clientId );
	gHUD[clientId].DrawHudString( (int)pKills->y + ( xpos_rel + 10 ), ypos, ScreenWidth, "/", 255, 140, 0, clientId );

	pDeaths = TextMessageGet( "SCORE_DEATHS" );
	gHUD[clientId].DrawHudString( (int)pDeaths->x + xpos_rel + 5, ypos, ScreenWidth, (char *)pDeaths->pMessage, 255, 140, 0, clientId );

	pDamage = TextMessageGet( "SCORE_DAMAGE" );
	gHUD[clientId].DrawHudString( (int)pDamage->y + xpos_rel - 35, ypos, ScreenWidth, (char *)pDamage->pMessage, 255, 140, 0, clientId );

	pAccuracy = TextMessageGet( "SCORE_ACCURACY" );
	gHUD[clientId].DrawHudString( (int)pAccuracy->y + xpos_rel - 35, ypos, ScreenWidth, (char *)pAccuracy->pMessage, 255, 140, 0, clientId );

	list_slot += 1.2f;
	ypos = (int)( ROW_RANGE_MIN + ( list_slot * ROW_GAP ) + 24 );
	xpos = (int)pPlayer->x + xpos_rel;
	FillRGBA( xpos - 5, ypos, (int)pAccuracy->y + 35, 2, 255, 140, 0, 255 );  // draw the seperator line

	list_slot += 0.8f;

	if ( !gHUD[clientId].m_Teamplay )
	{
		// it's not teamplay,  so just draw a simple player list
		DrawPlayers( clientId, iDrawFlags, xpos_rel, list_slot );
		return 1;
	}

	// clear out team scores
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		if ( !m_TeamInfo[i].scores_overriden )
			m_TeamInfo[i].frags = m_TeamInfo[i].deaths = 0;
		m_TeamInfo[i].ping = m_TeamInfo[i].packetloss = 0;
	}

	// recalc the team scores, then draw them
	for ( i = 1; i < MAX_PLAYERS; i++ )
	{
		if ( m_PlayerInfoList[i].name == NULL )
			continue; // empty player slot, skip

		if ( m_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// find what team this player is in
		for ( j = 1; j <= m_iNumTeams; j++ )
		{
			if ( !stricmp( m_PlayerExtraInfo[i].teamname, m_TeamInfo[j].name ) )
				break;
		}
		if ( j > m_iNumTeams )  // player is not in a team, skip to the next guy
			continue;

		if ( !m_TeamInfo[j].scores_overriden )
		{
			m_TeamInfo[j].frags += m_PlayerExtraInfo[i].frags;
			m_TeamInfo[j].deaths += m_PlayerExtraInfo[i].deaths;
		}

		m_TeamInfo[j].ping += m_PlayerInfoList[i].ping;
		m_TeamInfo[j].packetloss += m_PlayerInfoList[i].packetloss;

		if ( m_PlayerInfoList[i].thisplayer )
			m_TeamInfo[j].ownteam = TRUE;
		else
			m_TeamInfo[j].ownteam = FALSE;
	}

	// find team ping/packetloss averages
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		m_TeamInfo[i].already_drawn = FALSE;

		if ( m_TeamInfo[i].players > 0 )
		{
			m_TeamInfo[i].ping /= m_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			m_TeamInfo[i].packetloss /= m_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
		}
	}

	// Draw the teams
	while ( 1 )
	{
		int highest_frags = -99999; int lowest_deaths = 99999;
		int best_team = 0;

		for ( i = 1; i <= m_iNumTeams; i++ )
		{
			if ( m_TeamInfo[i].players < 0 )
				continue;

			if ( !m_TeamInfo[i].already_drawn && m_TeamInfo[i].frags >= highest_frags )
			{
				if ( m_TeamInfo[i].frags > highest_frags || m_TeamInfo[i].deaths < lowest_deaths )
				{
					best_team = i;
					lowest_deaths = m_TeamInfo[i].deaths;
					highest_frags = m_TeamInfo[i].frags;
				}
			}
		}

		// draw the best team on the scoreboard
		if ( !best_team )
			break;

		// draw out the best team
		team_info_t *team_info = &m_TeamInfo[best_team];

		ypos = (int)( ROW_RANGE_MIN + ( list_slot * ROW_GAP ) + 24 );

		// check we haven't drawn too far down
		if ( ypos > ROW_RANGE_MAX )  // don't draw to close to the lower border
			break;

		xpos = (int)pPlayer->x + xpos_rel;

		if ( team_info->ownteam ) // if it is their team, draw the background different color
		{
			// overlay the background in blue,  then draw the score text over it
			FillRGBA( (int)pPlayer->x + xpos_rel - 5, ypos, (int)pAccuracy->y - 5, ROW_GAP, 0, 0, 255, 70 );
		}

		// draw their name (left to right)
		gHUD[clientId].DrawHudString( xpos, ypos, (int)pPlayer->y + xpos_rel, team_info->name, 255, 225, 55, clientId );

		// draw kills (right to left)
		xpos = (int)pKills->x + xpos_rel;
		gHUD[clientId].DrawHudNumberString( xpos, ypos, (int)pKills->x + xpos_rel, team_info->frags, 255, 225, 55, clientId );

		// draw divider
		xpos = (int)pKills->y + ( xpos_rel + 10 );
		gHUD[clientId].DrawHudString( xpos, ypos, xpos + 20, "/", 255, 225, 55, clientId );

		// draw deaths
		xpos = (int)pDeaths->x + xpos_rel;
		gHUD[clientId].DrawHudNumberString( xpos, ypos, (int)pDeaths->x + xpos_rel, team_info->deaths, 255, 225, 55, clientId );

		team_info->already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get drawn again

		// draw all the players that belong to this team, indented slightly
		list_slot = DrawPlayers( clientId, iDrawFlags, xpos_rel, list_slot + 1, 10, team_info->name );
	}

	// draw all the players who are not in a team
	list_slot += 0.5f;
	DrawPlayers( clientId, iDrawFlags, xpos_rel, list_slot, 0, "" );

	return 1;
#endif
}

// returns the ypos where it finishes drawing
int CHudScoreboard :: DrawPlayers( int clientId, int iDrawFlags, int xpos_rel, float list_slot, int nameoffset, char *team )
{
#ifndef NON_MATCHING
	/* a ROM repete as conversões float->int que o nosso gcc junta e junta os testes do
	   destaque de outro jeito; o asm traz o "%d%%" */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/scoreboard", DrawPlayers__14CHudScoreboardiiifiPc );
#else
	client_textmessage_t *pPlayer, *pAccuracy, *pText;
	int bHighlight = iDrawFlags & 0x24;

	// draw the players, in order,  and restricted to team if set
	while ( 1 )
	{
		// Find the top ranking player
		int highest_frags = -99999;	int lowest_deaths = 99999;
		int best_player = 0;

		for ( int i = 1; i < MAX_PLAYERS; i++ )
		{
			if ( m_PlayerInfoList[i].name && m_PlayerExtraInfo[i].frags >= highest_frags )
			{
				if ( !(team && stricmp(m_PlayerExtraInfo[i].teamname, team)) )  // make sure it is the specified team
				{
					extra_player_info_t *pl_info = &m_PlayerExtraInfo[i];
					if ( pl_info->frags > highest_frags || pl_info->deaths < lowest_deaths )
					{
						best_player = i;
						lowest_deaths = pl_info->deaths;
						highest_frags = pl_info->frags;
					}
				}
			}
		}

		if ( !best_player )
			break;

		// draw out the best player
		hud_player_info_t *pl_info = &m_PlayerInfoList[best_player];

		int ypos = (int)( ROW_RANGE_MIN + ( list_slot * ROW_GAP ) + 24 );

		// check we haven't drawn too far down
		if ( ypos > ROW_RANGE_MAX )  // don't draw to close to the lower border
			break;

		pPlayer = TextMessageGet( "SCORE_PLAYER" );
		pAccuracy = TextMessageGet( "SCORE_ACCURACY" );

		int xpos = (int)pPlayer->x + xpos_rel;
		if ( best_player == m_iLastKilledBy && m_fLastKillTime && m_fLastKillTime > gHUD[clientId].m_flTime )
		{
			if ( pl_info->thisplayer && pl_info->iClient == clientId )
			{  // green is the suicide color? i wish this could do grey...
				if ( bHighlight )
					FillRGBA( (int)pPlayer->x + xpos_rel - 5, ypos, (int)pAccuracy->y - 5, ROW_GAP, 80, 155, 0, 70 );
			}
			else
			{  // Highlight the killers name - overlay the background in red,  then draw the score text over it
				if ( bHighlight )
					FillRGBA( (int)pPlayer->x + xpos_rel - 5, ypos, (int)pAccuracy->y - 5, ROW_GAP, 255, 0, 0, (int)( ( (float)m_fLastKillTime - gHUD[clientId].m_flTime ) * 15.0f ) );
			}
		}
		else if ( pl_info->thisplayer && pl_info->iClient == clientId && bHighlight && cl_numlocal >= 2 ) // if it is their name, draw it a different color
		{
			// overlay the background in blue,  then draw the score text over it
			FillRGBA( (int)pPlayer->x + xpos_rel - 5, ypos, (int)pAccuracy->y - 5, ROW_GAP, 0, 0, 255, 70 );
		}

		// draw their name (left to right)
		gHUD[clientId].DrawHudString( xpos + nameoffset, ypos, (int)pPlayer->y + xpos_rel, pl_info->name, 255, 255, 255, clientId );

		// draw kills (right to left)
		pText = TextMessageGet( "SCORE_KILLS" );
		xpos = (int)pText->y + xpos_rel;
		gHUD[clientId].DrawHudNumberString( xpos, ypos, (int)pText->x + xpos_rel, m_PlayerExtraInfo[best_player].frags, 255, 255, 255, clientId );

		// draw divider
		xpos = (int)pText->y + ( xpos_rel + 10 );
		gHUD[clientId].DrawHudString( xpos, ypos, xpos + 20, "/", 255, 255, 255, clientId );

		// draw deaths
		pText = TextMessageGet( "SCORE_DEATHS" );
		xpos = (int)pText->y + xpos_rel;
		gHUD[clientId].DrawHudNumberString( xpos, ypos, (int)pText->x + xpos_rel, m_PlayerExtraInfo[best_player].deaths, 255, 255, 255, clientId );

		// draw damage
		pText = TextMessageGet( "SCORE_DAMAGE" );
		xpos = (int)pText->y + xpos_rel;
		gHUD[clientId].DrawHudNumberString( xpos + 30, ypos, (int)pText->x + xpos_rel, m_PlayerExtraInfo[best_player].damage, 255, 255, 255, clientId );

		// draw accuracy
		pText = TextMessageGet( "SCORE_ACCURACY" );
		sprintf( D_003A3230, "%d%%", (int)( 1.0f / m_PlayerExtraInfo[best_player].shots * m_PlayerExtraInfo[best_player].hits * 100.0f ) );
		xpos = ( (int)pText->y - (int)pText->x ) / 2 + (int)pText->x + xpos_rel;
		gHUD[clientId].DrawHudStringReverse( xpos + 55, ypos, xpos - 20, D_003A3230, 255, 255, 255, clientId );

		pl_info->name = NULL;  // set the name to be NULL, so this client won't get drawn again
		list_slot++;
	}

	return (int)list_slot;
#endif
}


void CHudScoreboard :: GetAllPlayersInfo( int clientId )
{
	for ( int i = 1; i < MAX_PLAYERS; i++ )
	{
		gEngfuncs.pfnGetPlayerInfo( i, &m_PlayerInfoList[i], clientId );

		if ( m_PlayerInfoList[i].thisplayer && m_PlayerInfoList[i].iClient == clientId )
			m_iPlayerNum = i;  // !!!HACK: this should be initialized elsewhere... maybe gotten from the engine
	}
}

int CHudScoreboard :: MsgFunc_ScoreInfo( int clientId, const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();
	short frags = READ_SHORT();
	short deaths = READ_SHORT();
	short damage = READ_SHORT();
	short shots = READ_SHORT();
	short hits = READ_SHORT();

	if ( cl > 0 && cl <= MAX_PLAYERS )
	{
		m_PlayerExtraInfo[cl].frags = frags;
		m_PlayerExtraInfo[cl].deaths = deaths;
		m_PlayerExtraInfo[cl].damage = damage;
		m_PlayerExtraInfo[cl].shots = shots;
		m_PlayerExtraInfo[cl].hits = hits;
	}

	return 1;
}

// Message handler for TeamInfo message
// accepts two values:
//		byte: client number
//		string: client team name
int CHudScoreboard :: MsgFunc_TeamInfo( int clientId, const char *pszName, int iSize, void *pbuf )
{
	int i, j, k;

	BEGIN_READ( pbuf, iSize );
	short cl = READ_BYTE();

	if ( cl > 0 && cl <= MAX_PLAYERS )
	{  // set the players team
		strncpy( m_PlayerExtraInfo[cl].teamname, READ_STRING(), MAX_TEAM_NAME );
	}

	// rebuild the list of teams

	// clear out player counts from teams
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		m_TeamInfo[i].players = 0;
	}

	// rebuild the team list
	GetAllPlayersInfo( clientId );
	m_iNumTeams = 0;
	for ( i = 1; i < MAX_PLAYERS; i++ )
	{
		if ( m_PlayerInfoList[i].name == NULL )
			continue;

		if ( m_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// is this player in an existing team?
		for ( j = 1; j <= m_iNumTeams; j++ )
		{
			if ( m_TeamInfo[j].name[0] == '\0' )
				break;

			if ( !stricmp( m_PlayerExtraInfo[i].teamname, m_TeamInfo[j].name ) )
				break;
		}

		if ( j > m_iNumTeams )
		{ // they aren't in a listed team, so make a new one
			// search through for an empty team slot
			for ( k = 1; k <= m_iNumTeams; k++ )
			{
				if ( m_TeamInfo[k].name[0] == '\0' )
					break;
			}
			m_iNumTeams = max( k, m_iNumTeams );

			strncpy( m_TeamInfo[k].name, m_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
			m_TeamInfo[k].players = 0;
		}

		m_TeamInfo[j].players++;
	}

	// clear out any empty teams
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		if ( m_TeamInfo[i].players < 1 )
			memset( &m_TeamInfo[i], 0, sizeof(team_info_t) );
	}

	return 1;
}

// Message handler for TeamScore message
// accepts three values:
//		string: team name
//		short: teams kills
//		short: teams deaths
// if this message is never received, then scores will simply be the combined totals of the players.
int CHudScoreboard :: MsgFunc_TeamScore( int clientId, const char *pszName, int iSize, void *pbuf )
{
	int i;

	BEGIN_READ( pbuf, iSize );
	char *TeamName = READ_STRING();

	// find the team matching the name
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		if ( !stricmp( TeamName, m_TeamInfo[i].name ) )
			break;
	}
	if ( i > m_iNumTeams )
		return 1;

	// use this new score data instead of combined player scores
	m_TeamInfo[i].scores_overriden = TRUE;
	m_TeamInfo[i].frags = READ_SHORT();
	m_TeamInfo[i].deaths = READ_SHORT();

	return 1;
}

void CHudScoreboard :: DeathMsg( int killer, int victim, int clientId )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		m_fLastKillTime = (int)( gHUD[clientId].m_flTime + 10 );	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			m_iLastKilledBy = m_iPlayerNum;
	}
}

void CHudScoreboard :: UserCmd_ShowScores( int clientId )
{
	m_iShowscoresHeld = TRUE;
}

void CHudScoreboard :: UserCmd_HideScores( int clientId )
{
	m_iShowscoresHeld = FALSE;
}
