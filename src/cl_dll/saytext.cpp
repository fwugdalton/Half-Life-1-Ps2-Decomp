/* cl_dll/saytext.cpp: CHudSayText do HLSDK, versão do PS2 (sem cores dos nomes nem
   gViewPort, um buffer só para todos os jogadores, cvar hud_saytext_time lida só quando
   chega a 1ª linha). */

#include "cl_dll/hud.h"

extern "C" {
char *strncpy( char *dst, const char *src, unsigned int n );
char *strcat( char *dst, const char *src );
unsigned int strlen( const char *s );
void *memset( void *dst, int c, unsigned int n );
void *func_002E46A4( void *dst, const void *src, unsigned int n );
}

#define memmove func_002E46A4

#define min(a,b)	(((a) < (b)) ? (a) : (b))
#define max(a,b)	(((a) > (b)) ? (a) : (b))

#define MAX_LINES	5
#define MAX_CHARS_PER_LINE	256  /* it can be less than this, depending on char size */

// allow 20 pixels on either side of the text
#define MAX_LINE_WIDTH  ( ScreenWidth - 40 )
#define LINE_START  10

/* g_szLineBuffer fica no .bss (fora do objeto) */
extern "C" char D_003A2C30[ MAX_LINES + 1 ][ MAX_CHARS_PER_LINE ];
#define g_szLineBuffer D_003A2C30

static float flSayTextTime = 5;
static float flScrollTime = 0;  // the time at which the lines next scroll up

static int Y_START = 0;
static int line_height = 0;

DECLARE_MESSAGE( m_SayText, SayText );

int CHudSayText :: Init( int clientId )
{
	gHUD[clientId].AddHudElem( this );

	HOOK_MESSAGE( SayText );

	InitHUDData( clientId );

	if ( clientId == 0 )
		gEngfuncs.pfnRegisterVariable( "hud_saytext_time", "5", 0 );

	return 1;
}


void CHudSayText :: InitHUDData( int clientId )
{
	memset( g_szLineBuffer, 0, sizeof g_szLineBuffer );
}

int CHudSayText :: VidInit( int clientId )
{
	return 1;
}


int ScrollTextUp( void )
{
	ConsolePrint( g_szLineBuffer[0] ); // move the first line into the console buffer
	g_szLineBuffer[MAX_LINES][0] = 0;
	memmove( g_szLineBuffer[0], g_szLineBuffer[1], sizeof(g_szLineBuffer) - sizeof(g_szLineBuffer[0]) ); // overwrite the first line
	g_szLineBuffer[MAX_LINES-1][0] = 0;

	if ( g_szLineBuffer[0][0] == ' ' ) // also scroll up following lines
	{
		g_szLineBuffer[0][0] = 2;
		return 1 + ScrollTextUp();
	}

	return 1;
}

int CHudSayText :: Draw( float flTime, int clientId, int iDrawFlags )
{
	if ( !( iDrawFlags & 0x24 ) )
		return 1;

	int y = Y_START + 24;

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	flScrollTime = min( flScrollTime, flTime + flSayTextTime );

	// make sure the scrolltime is within reasonable bounds,  to guard against the clock being reset
	flScrollTime = min( flScrollTime, flTime + flSayTextTime );

	if ( flScrollTime <= flTime )
	{
		if ( *g_szLineBuffer[0] )
		{
			flScrollTime = flTime + flSayTextTime;
			// push the console up
			ScrollTextUp();
		}
		else
		{ // buffer is empty,  just disable drawing of this section
			m_iFlags &= ~HUD_ACTIVE;
		}
	}

	for ( int i = 0; i < MAX_LINES; i++ )
	{
		if ( *g_szLineBuffer[i] )
		{
			// normal draw
			DrawConsoleString( LINE_START, y, g_szLineBuffer[i] );
		}

		y += line_height;
	}

	return 1;
}

int CHudSayText :: MsgFunc_SayText( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int client_index = READ_BYTE();		// the client who spoke the message
	SayTextPrint( READ_STRING(), iSize - 1, clientId );

	return 1;
}

void CHudSayText :: SayTextPrint( const char *pszBuf, int iBufSize, int clientId )
{
	int i;
	// find an empty string slot
	for ( i = 0; i < MAX_LINES; i++ )
	{
		if ( ! *g_szLineBuffer[i] )
			break;
	}
	if ( i == MAX_LINES )
	{
		// force scroll buffer up
		ScrollTextUp();
		i = MAX_LINES - 1;
	}

	strncpy( g_szLineBuffer[i], pszBuf, max( iBufSize - 1, MAX_CHARS_PER_LINE - 1 ) );

	// make sure the text fits in one line
	EnsureTextFitsInOneLineAndWrapIfHaveTo( i, clientId );

	// Set scroll time
	if ( i == 0 )
	{
		flSayTextTime = CVAR_GET_FLOAT( "hud_saytext_time" );
		flScrollTime = gHUD[clientId].m_flTime + flSayTextTime;
	}

	m_iFlags |= HUD_ACTIVE;
	PlaySound( "misc/talk.wav", 1, clientId );

	if ( ScreenHeight < 480 )
		Y_START = ScreenHeight - 35;
	else
		Y_START = ScreenHeight - 45;
	Y_START = Y_START - ( line_height * ( MAX_LINES + 1 ) );
}

void CHudSayText :: EnsureTextFitsInOneLineAndWrapIfHaveTo( int line, int clientId )
{
	int line_width = 0;
	GetConsoleStringSize( g_szLineBuffer[line], &line_width, &line_height );

	if ( (line_width + LINE_START) > MAX_LINE_WIDTH )
	{ // string is too long to fit on line
		// scan the string until we find what word is too long,  and wrap the end of the sentence after the word
		int length = LINE_START;
		int tmp_len = 0;
		char *last_break = NULL;
		for ( char *x = g_szLineBuffer[line]; *x != 0; x++ )
		{
			char buf[2];
			buf[1] = 0;

			if ( *x == ' ' && x != g_szLineBuffer[line] )  // store each line break,  except for the very first character
				last_break = x;

			buf[0] = *x;  // get the length of the current character
			GetConsoleStringSize( buf, &tmp_len, &line_height );
			length += tmp_len;

			if ( length > MAX_LINE_WIDTH )
			{  // needs to be broken up
				if ( !last_break )
					last_break = x-1;

				x = last_break;

				// find an empty string slot
				int j;
				do
				{
					for ( j = 0; j < MAX_LINES; j++ )
					{
						if ( ! *g_szLineBuffer[j] )
							break;
					}
					if ( j == MAX_LINES )
					{
						// need to make more room to display text, scroll stuff up then fix the pointers
						int linesmoved = ScrollTextUp();
						line -= linesmoved;
						last_break = last_break - (sizeof(g_szLineBuffer[0]) * linesmoved);
					}
				}
				while ( j == MAX_LINES );

				// copy remaining string into next buffer,  making sure it starts with a space character
				if ( (char)*last_break == (char)' ' )
				{
					int linelen = strlen(g_szLineBuffer[j]);
					int remaininglen = strlen(last_break);

					if ( (linelen - remaininglen) <= MAX_CHARS_PER_LINE )
						strcat( g_szLineBuffer[j], last_break );
				}
				else
				{
					if ( (strlen(g_szLineBuffer[j]) - strlen(last_break) - 2) < MAX_CHARS_PER_LINE )
					{
						strcat( g_szLineBuffer[j], " " );
						strcat( g_szLineBuffer[j], last_break );
					}
				}

				*last_break = 0; // cut off the last string

				EnsureTextFitsInOneLineAndWrapIfHaveTo( j, clientId );
				break;
			}
		}
	}
}
