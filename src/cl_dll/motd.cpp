/* cl_dll/motd.cpp: CHudMOTD do HLSDK antigo, versão do PS2 (tempo mínimo de 10 s, o
   tempo restante desce com o tempo de jogo de cada quadro). */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

extern "C" {
char *strcat( char *dst, const char *src );
}

#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))

#define CVAR_CREATE( x, y, z ) gEngfuncs.pfnRegisterVariable( (char*)x, (char*)y, z )

/* no .bss (fora do objeto) */
extern "C" int D_003990B4;
#define MOTD_DISPLAY_TIME D_003990B4
extern "C" float D_003A2418;	/* hora do quadro anterior */

DECLARE_MESSAGE( m_MOTD, MOTD );

int CHudMOTD :: Init( int clientId )
{
	gHUD[clientId].AddHudElem( this );

	HOOK_MESSAGE( MOTD );

	if ( clientId == 0 )
		CVAR_CREATE( "motd_display_time", "15", 0 );

	m_iFlags &= ~HUD_ACTIVE;  // start out inactive
	m_szMOTD[0] = 0;

	return 1;
}

int CHudMOTD :: VidInit( int clientId )
{
	// Load sprites here

	return 1;
}

void CHudMOTD :: Reset( int clientId )
{
	m_iFlags &= ~HUD_ACTIVE;  // start out inactive
	m_szMOTD[0] = 0;
	m_iLines = 0;
	m_flActiveTill = 0;
}

#define LINE_HEIGHT  13

int CHudMOTD :: Draw( float flTime, int clientId, int iDrawFlags )
{
#ifndef NON_MATCHING
	/* no alvo o endereço de gHUD[clientId].m_scrinfo.iWidth é refeito com lui dentro do
	   laço; o nosso gcc tira gHUD+0x1ED4 do laço (o resto casa) */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/motd", Draw__8CHudMOTDfii );
#else
	if ( !( iDrawFlags & 4 ) && !iDrawFlags )
		return 1;

	// Draw MOTD line-by-line

	if ( m_flActiveTill <= 0 )
	{ // finished with MOTD, disable it
		m_flActiveTill = 0;
		m_iFlags &= ~HUD_ACTIVE;
		m_iLines = 0;
		m_szMOTD[0] = 0;
		return 1;
	}

	float flDelta = gHUD[clientId].m_flTime - D_003A2418;
	D_003A2418 = gHUD[clientId].m_flTime;
	m_flActiveTill -= min( max( flDelta, 0 ), 1 );

	// find the top of where the MOTD should be drawn,  so the whole thing is centered in the screen
	int ypos = max(((ScreenHeight - (m_iLines * LINE_HEIGHT)) / 2) - 40, 30 ); // shift it up slightly
	char *ch = m_szMOTD;
	while ( *ch )
	{
		int line_length = 0;  // count the length of the current line
		char *next_line;
		for ( next_line = ch; *next_line != '\n' && *next_line != 0; next_line++ )
			line_length += gHUD[clientId].m_scrinfo.charWidths[ *next_line ];
		char *top = next_line;
		if ( *top == '\n' )
			*top = 0;
		else
			top = NULL;

		// find where to start drawing the line
		int xpos = (ScreenWidth - line_length) / 2;

		gHUD[clientId].DrawHudString( xpos, ypos, ScreenWidth, ch, 255, 180, 0, clientId );

		ypos += LINE_HEIGHT;

		if ( top )  // restore
			*top = '\n';
		ch = next_line;
		if ( *ch == '\n' )
			ch++;

		if ( ypos > (ScreenHeight - 20) )
			break;  // don't let it draw too low
	}

	return 1;
#endif
}

int CHudMOTD :: MsgFunc_MOTD( int clientId, const char *pszName, int iSize, void *pbuf )
{
	if ( m_iFlags & HUD_ACTIVE )
	{
		Reset( clientId ); // clear the current MOTD in prep for this one
	}

	BEGIN_READ( pbuf, iSize );

	int is_finished = READ_BYTE();
	strcat( m_szMOTD, READ_STRING() );

	if ( is_finished )
	{
		m_iFlags |= HUD_ACTIVE;

		MOTD_DISPLAY_TIME = ( CVAR_GET_FLOAT( "motd_display_time" ) >= 10 ) ? (int)CVAR_GET_FLOAT( "motd_display_time" ) : 10;

		m_flActiveTill = MOTD_DISPLAY_TIME;

		for ( char *sz = m_szMOTD; *sz != 0; sz++ )  // count the number of lines in the MOTD
		{
			if ( *sz == '\n' )
				m_iLines++;
		}
	}

	return 1;
}
