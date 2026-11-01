/* cl_dll/menu.cpp: CHudMenu do HLSDK, versão do PS2. O Draw não desenha nada (o menu
   do PS2 é feito em outro lugar) e não há KB_ConvertString. */

#include "cl_dll/hud.h"

extern "C" {
char *strncpy( char *dst, const char *src, unsigned int n );
char *func_002E61B8( char *dst, const char *src, unsigned int n );	/* strncat */
char *strcpy( char *dst, const char *src );
unsigned int strlen( const char *s );
int sprintf( char *buf, const char *fmt, ... );
}
#define strncat func_002E61B8

char READ_CHAR( void );

#define MAX_MENU_STRING	512
char g_szMenuString[MAX_MENU_STRING] = "";
char g_szPrelocalisedMenuString[MAX_MENU_STRING] = "";

DECLARE_MESSAGE( m_Menu, ShowMenu );

int CHudMenu :: Init( int clientId )
{
	gHUD[clientId].AddHudElem( this );

	HOOK_MESSAGE( ShowMenu );

	InitHUDData( clientId );

	return 1;
}

void CHudMenu :: InitHUDData( int clientId )
{
	m_fMenuDisplayed = 0;
	m_bitsValidSlots = 0;
	Reset( clientId );
}

void CHudMenu :: Reset( int clientId )
{
	g_szPrelocalisedMenuString[0] = 0;
	m_fWaitingForMore = 0;
}

int CHudMenu :: VidInit( int clientId )
{
	return 1;
}

int CHudMenu :: Draw( float flTime, int clientId, int iDrawFlags )
{
	char szBuf[16];

	return 1;
}

// selects an item from the menu
void CHudMenu :: SelectMenuItem( int menu_item )
{
	// if menu_item is in a valid slot,  send a menuselect command to the server
	if ( (menu_item > 0) && (m_bitsValidSlots & (1 << (menu_item-1))) )
	{
		char szbuf[32];
		sprintf( szbuf, "menuselect %d\n", menu_item );
		ClientCmd( szbuf );

		// remove the menu
		m_fMenuDisplayed = 0;
		m_iFlags &= ~HUD_ACTIVE;
	}
}

// Message handler for ShowMenu message
// takes four values:
//		short: a bitfield of keys that are valid input
//		char : the duration, in seconds, the menu should stay up. -1 means is stays until something is chosen.
//		byte : a boolean, TRUE if there is more string yet to be received before displaying the menu, FALSE if it's the last string
//		string: menu string to display
// if this message is never received, then scores will simply be the combined totals of the players.
int CHudMenu :: MsgFunc_ShowMenu( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_bitsValidSlots = READ_SHORT();
	int DisplayTime = READ_CHAR();
	int NeedMore = READ_BYTE();

	if ( DisplayTime > 0 )
		m_flShutoffTime = DisplayTime + gHUD[clientId].m_flTime;
	else
		m_flShutoffTime = -1;

	if ( m_bitsValidSlots )
	{
		if ( !m_fWaitingForMore ) // this is the start of a new menu
		{
			strncpy( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING );
		}
		else
		{  // append to the current menu string
			strncat( g_szPrelocalisedMenuString, READ_STRING(), MAX_MENU_STRING - strlen(g_szPrelocalisedMenuString) );
		}
		g_szPrelocalisedMenuString[MAX_MENU_STRING-1] = 0;  // ensure null termination (strncat/strncpy does not)

		if ( !NeedMore )
		{  // we have the whole string, so we can localise it now
			strcpy( g_szMenuString, gHUD[clientId].m_TextMessage.BufferedLocaliseTextString( g_szPrelocalisedMenuString ) );
		}

		m_fMenuDisplayed = 1;
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		m_fMenuDisplayed = 0; // no valid slots means that the menu should be turned off
		m_iFlags &= ~HUD_ACTIVE;
	}

	m_fWaitingForMore = NeedMore;

	return 1;
}
