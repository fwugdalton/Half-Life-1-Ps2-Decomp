/* cl_dll/status_icons.cpp: CHudStatusIcons do HLSDK, versão do PS2 (HUD por jogador,
   sem IsSpectateOnly e sem o som do timer da granada). */

#include "cl_dll/hud.h"

extern "C" {
char *strcpy( char *dst, const char *src );
void *memset( void *dst, int c, unsigned int n );
int strcasecmp( const char *a, const char *b );
}

#define stricmp strcasecmp

DECLARE_MESSAGE( m_StatusIcons, StatusIcon );

int CHudStatusIcons::Init( int clientId )
{
	HOOK_MESSAGE( StatusIcon );

	gHUD[clientId].AddHudElem( this );

	Reset( clientId );

	return 1;
}

int CHudStatusIcons::VidInit( int clientId )
{

	return 1;
}

void CHudStatusIcons::Reset( int clientId )
{
	memset( m_IconList, 0, sizeof m_IconList );
	m_iFlags &= ~HUD_ACTIVE;
}

// Draw status icons along the left-hand side of the screen
int CHudStatusIcons::Draw( float flTime, int clientId, int iDrawFlags )
{
	if ( !(iDrawFlags & 0x24) )
		return 1;

	// find starting position to draw from, along right-hand side of screen
	int x = 13;
	int y = ScreenHeight / 2;

	// loop through icon list, and draw any valid icons drawing up from the middle of screen
	for ( int i = 0; i < MAX_ICONSPRITES; i++ )
	{
		if ( m_IconList[i].spr )
		{
			y -= ( m_IconList[i].rc.bottom - m_IconList[i].rc.top ) + 5;

			SPR_Set( m_IconList[i].spr, m_IconList[i].r, m_IconList[i].g, m_IconList[i].b );
			SPR_DrawAdditive( 0, x, y, &m_IconList[i].rc );
		}
	}

	return 1;
}

int CHudStatusIcons::MsgFunc_StatusIcon( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int ShouldEnable = READ_BYTE();
	char *pszIconName = READ_STRING();
	if ( ShouldEnable )
	{
		int r = READ_BYTE();
		int g = READ_BYTE();
		int b = READ_BYTE();
		EnableIcon( pszIconName, r, g, b, clientId );
		m_iFlags |= HUD_ACTIVE;
	}
	else
	{
		DisableIcon( pszIconName );
	}

	return 1;
}

// add the icon to the icon list, and set it's drawing color
void CHudStatusIcons::EnableIcon( char *pszIconName, unsigned char red, unsigned char green, unsigned char blue, int clientId )
{
	int i;
	// check to see if the sprite is in the current list
	for ( i = 0; i < MAX_ICONSPRITES; i++ )
	{
		if ( !stricmp( m_IconList[i].szSpriteName, pszIconName ) )
			break;
	}

	if ( i == MAX_ICONSPRITES )
	{
		// icon not in list, so find an empty slot to add to
		for ( i = 0; i < MAX_ICONSPRITES; i++ )
		{
			if ( !m_IconList[i].spr )
				break;
		}
	}

	// if we've run out of space in the list, overwrite the first icon
	if ( i == MAX_ICONSPRITES )
	{
		i = 0;
	}

	// Load the sprite and add it to the list
	// the sprite must be listed in hud.txt
	int spr_index = gHUD[clientId].GetSpriteIndex( pszIconName );
	m_IconList[i].spr = gHUD[clientId].GetSprite( spr_index );
	m_IconList[i].rc = gHUD[clientId].GetSpriteRect( spr_index );
	m_IconList[i].r = red;
	m_IconList[i].g = green;
	m_IconList[i].b = blue;
	strcpy( m_IconList[i].szSpriteName, pszIconName );
}

void CHudStatusIcons::DisableIcon( char *pszIconName )
{
	// find the sprite is in the current list
	for ( int i = 0; i < MAX_ICONSPRITES; i++ )
	{
		if ( !stricmp( m_IconList[i].szSpriteName, pszIconName ) )
		{
			// clear the item from the list
			memset( &m_IconList[i], 0, sizeof(icon_sprite_t) );
			return;
		}
	}
}
