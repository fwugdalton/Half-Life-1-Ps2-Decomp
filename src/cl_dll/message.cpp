/* cl_dll/message.cpp: CHudMessage do HLSDK, versão do PS2 (margens de 8/24 pixels na
   tela, letra desenhada por gEngfuncs+0x74, sem mensagens "#" nem m_bEndAfterMessage). */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

extern "C" {
void *memset( void *dst, int c, unsigned int n );
unsigned int strlen( const char *s );
int func_002E5340( void );	/* rand */
}
#define rand func_002E5340

extern "C" int cl_numlocal;


DECLARE_MESSAGE( m_Message, HudText )
DECLARE_MESSAGE( m_Message, GameTitle )

int CHudMessage::Init( int clientId )
{
	HOOK_MESSAGE( HudText );
	HOOK_MESSAGE( GameTitle );

	gHUD[clientId].AddHudElem( this );

	Reset( clientId );

	return 1;
}

int CHudMessage::VidInit( int clientId )
{
	m_HUD_title_half = gHUD[clientId].GetSpriteIndex( "title_half" );
	m_HUD_title_life = gHUD[clientId].GetSpriteIndex( "title_life" );

	return 1;
}

void CHudMessage::Reset( int clientId )
{
	memset( m_pMessages, 0, sizeof( m_pMessages[0] ) * maxHUDMessages );
	memset( m_startTime, 0, sizeof( m_startTime[0] ) * maxHUDMessages );

	m_gameTitleTime = 0;
	m_pGameTitle = NULL;
}

float CHudMessage::FadeBlend( float fadein, float fadeout, float hold, float localTime )
{
	float fadeTime = fadein + hold;
	float fadeBlend;

	if ( localTime < 0 )
		return 0;

	if ( localTime < fadein )
	{
		fadeBlend = 1 - ((fadein - localTime) / fadein);
	}
	else if ( localTime > fadeTime )
	{
		if ( fadeout > 0 )
			fadeBlend = 1 - ((localTime - fadeTime) / fadeout);
		else
			fadeBlend = 0;
	}
	else
		fadeBlend = 1;

	return fadeBlend;
}

int CHudMessage::XPosition( float x, int width, int totalWidth, int clientId )
{
	int xPos;

	if ( x == -1 )
	{
		xPos = (ScreenWidth - width) / 2;
	}
	else
	{
		if ( x < 0 )
			xPos = (1.0f + x) * ScreenWidth - totalWidth - 8;	// Alight right
		else
			xPos = x * ScreenWidth + 8;
	}

	if ( xPos + width > ScreenWidth - 8 )
		xPos = ScreenWidth - width - 8;
	else if ( xPos < 8 )
		xPos = 8;

	return xPos;
}

int CHudMessage::YPosition( float y, int height, int clientId )
{
	int yPos;

	if ( y == -1 )	// Centered?
		yPos = (ScreenHeight - height) * 0.5f;
	else
	{
		// Alight bottom?
		if ( y < 0 )
			yPos = (1.0f + y) * ScreenHeight - height - 24;	// Alight bottom
		else // align top
			yPos = y * ScreenHeight + 24;
	}

	if ( yPos + height > ScreenHeight - 24 )
		yPos = ScreenHeight - height - 24;
	else if ( yPos < 24 )
		yPos = 24;

	return yPos;
}

void CHudMessage::MessageScanNextChar( int clientId )
{
	int srcRed, srcGreen, srcBlue, destRed, destGreen, destBlue;
	int blend;

	srcRed = m_parms.pMessage->r1;
	srcGreen = m_parms.pMessage->g1;
	srcBlue = m_parms.pMessage->b1;
	blend = 0;	// Pure source

	switch( m_parms.pMessage->effect )
	{
	// Fade-in / Fade-out
	case 0:
	case 1:
		destRed = destGreen = destBlue = 0;
		blend = m_parms.fadeBlend;
		break;

	case 2:
		m_parms.charTime += m_parms.pMessage->fadein;
		if ( m_parms.charTime > m_parms.time )
		{
			srcRed = srcGreen = srcBlue = 0;
			destRed = destGreen = destBlue = 0;
			blend = 0;	// pure source
		}
		else
		{
			float deltaTime = m_parms.time - m_parms.charTime;

			destRed = destGreen = destBlue = 0;
			if ( m_parms.time > m_parms.fadeTime )
			{
				blend = m_parms.fadeBlend;
			}
			else if ( deltaTime > m_parms.pMessage->fxtime )
				blend = 0;	// pure dest
			else
			{
				destRed = m_parms.pMessage->r2;
				destGreen = m_parms.pMessage->g2;
				destBlue = m_parms.pMessage->b2;
				blend = 255 - (deltaTime * (1.0f/m_parms.pMessage->fxtime) * 255.0f + 0.5f);
			}
		}
		break;

	default:
		destRed = destGreen = destBlue = 0;
		break;
	}
	if ( blend > 255 )
		blend = 255;
	else if ( blend < 0 )
		blend = 0;

	m_parms.r = ((srcRed * (255-blend)) + (destRed * blend)) >> 8;
	m_parms.g = ((srcGreen * (255-blend)) + (destGreen * blend)) >> 8;
	m_parms.b = ((srcBlue * (255-blend)) + (destBlue * blend)) >> 8;

	if ( m_parms.pMessage->effect == 1 && m_parms.charTime != 0 )
	{
		if ( m_parms.x >= 0 && m_parms.y >= 0 && (m_parms.x + gHUD[clientId].m_scrinfo.charWidths[ m_parms.text ]) <= ScreenWidth )
			TextMessageDrawChar( m_parms.x, m_parms.y, m_parms.text, m_parms.pMessage->r2, m_parms.pMessage->g2, m_parms.pMessage->b2 );
	}
}

void CHudMessage::MessageScanStart( void )
{
	switch( m_parms.pMessage->effect )
	{
	// Fade-in / out with flicker
	case 1:
	case 0:
		m_parms.fadeTime = m_parms.pMessage->fadein + m_parms.pMessage->holdtime;

		if ( m_parms.time < m_parms.pMessage->fadein )
		{
			m_parms.fadeBlend = ((m_parms.pMessage->fadein - m_parms.time) * (1.0f/m_parms.pMessage->fadein) * 255);
		}
		else if ( m_parms.time > m_parms.fadeTime )
		{
			if ( m_parms.pMessage->fadeout > 0 )
				m_parms.fadeBlend = (((m_parms.time - m_parms.fadeTime) / m_parms.pMessage->fadeout) * 255);
			else
				m_parms.fadeBlend = 255; // Pure dest (off)
		}
		else
			m_parms.fadeBlend = 0;	// Pure source (on)
		m_parms.charTime = 0;

		if ( m_parms.pMessage->effect == 1 && (rand()%100) < 10 )
			m_parms.charTime = 1;
		break;

	case 2:
		m_parms.fadeTime = (m_parms.pMessage->fadein * m_parms.length) + m_parms.pMessage->holdtime;

		if ( m_parms.time > m_parms.fadeTime && m_parms.pMessage->fadeout > 0 )
			m_parms.fadeBlend = (((m_parms.time - m_parms.fadeTime) / m_parms.pMessage->fadeout) * 255);
		else
			m_parms.fadeBlend = 0;
		break;
	}
}

void CHudMessage::MessageDrawScan( client_textmessage_t *pMessage, float time, int clientId )
{
	int i, j, length, width;
	const char *pText;
	unsigned char line[80];

	pText = pMessage->pMessage;
	// Count lines
	m_parms.lines = 1;
	m_parms.time = time;
	m_parms.pMessage = pMessage;
	length = 0;
	width = 0;
	m_parms.totalWidth = 0;
	while ( *pText )
	{
		if ( *pText == '\n' )
		{
			m_parms.lines++;
			if ( width > m_parms.totalWidth )
				m_parms.totalWidth = width;
			width = 0;
		}
		else
			width += gHUD[clientId].m_scrinfo.charWidths[*pText];
		pText++;
		length++;
	}
	m_parms.length = length;
	m_parms.totalHeight = (m_parms.lines * gHUD[clientId].m_scrinfo.iCharHeight);

	m_parms.y = YPosition( pMessage->y, m_parms.totalHeight, clientId );
	pText = pMessage->pMessage;

	m_parms.charTime = 0;

	MessageScanStart();

	for ( i = 0; i < m_parms.lines; i++ )
	{
		m_parms.lineLength = 0;
		m_parms.width = 0;
		while ( *pText && *pText != '\n' )
		{
			unsigned char c = *pText;
			line[m_parms.lineLength] = c;
			m_parms.width += gHUD[clientId].m_scrinfo.charWidths[c];
			m_parms.lineLength++;
			pText++;
		}
		pText++;		// Skip LF
		line[m_parms.lineLength] = 0;

		m_parms.x = XPosition( pMessage->x, m_parms.width, m_parms.totalWidth, clientId );

		for ( j = 0; j < m_parms.lineLength; j++ )
		{
			m_parms.text = line[j];
			int next = m_parms.x + gHUD[clientId].m_scrinfo.charWidths[ m_parms.text ];
			MessageScanNextChar( clientId );

			if ( m_parms.x >= 0 && m_parms.y >= 0 && next <= ScreenWidth )
				TextMessageDrawChar( m_parms.x, m_parms.y, m_parms.text, m_parms.r, m_parms.g, m_parms.b );
			m_parms.x = next;
		}

		m_parms.y += gHUD[clientId].m_scrinfo.iCharHeight;
	}
}

int CHudMessage::Draw( float fTime, int clientId, int iDrawFlags )
{
	int i, drawn;
	client_textmessage_t *pMessage;
	float endTime;

	if ( cl_numlocal < 2 || ( iDrawFlags & 0x40 ) )
	{
		if ( m_gameTitleTime > 0 )
		{
			float localTime = gHUD[clientId].m_flTime - m_gameTitleTime;
			float brightness;

			// Maybe timer isn't set yet
			if ( m_gameTitleTime > gHUD[clientId].m_flTime )
				m_gameTitleTime = gHUD[clientId].m_flTime;

			if ( localTime > (m_pGameTitle->fadein + m_pGameTitle->holdtime + m_pGameTitle->fadeout) )
				m_gameTitleTime = 0;
			else
			{
				brightness = FadeBlend( m_pGameTitle->fadein, m_pGameTitle->fadeout, m_pGameTitle->holdtime, localTime );

				int halfWidth = gHUD[clientId].GetSpriteRect(m_HUD_title_half).right - gHUD[clientId].GetSpriteRect(m_HUD_title_half).left;
				int fullWidth = halfWidth + gHUD[clientId].GetSpriteRect(m_HUD_title_life).right - gHUD[clientId].GetSpriteRect(m_HUD_title_life).left;
				int fullHeight = gHUD[clientId].GetSpriteRect(m_HUD_title_half).bottom - gHUD[clientId].GetSpriteRect(m_HUD_title_half).top;

				int x = XPosition( m_pGameTitle->x, fullWidth, fullWidth, clientId );
				int y = YPosition( m_pGameTitle->y, fullHeight, clientId );

				SPR_Set( gHUD[clientId].GetSprite(m_HUD_title_half), brightness * m_pGameTitle->r1, brightness * m_pGameTitle->g1, brightness * m_pGameTitle->b1 );
				SPR_DrawAdditive( 0, x, y, &gHUD[clientId].GetSpriteRect(m_HUD_title_half) );

				SPR_Set( gHUD[clientId].GetSprite(m_HUD_title_life), brightness * m_pGameTitle->r1, brightness * m_pGameTitle->g1, brightness * m_pGameTitle->b1 );
				SPR_DrawAdditive( 0, x + halfWidth, y, &gHUD[clientId].GetSpriteRect(m_HUD_title_life) );
			}
		}
	}

	if ( !( iDrawFlags & 0x24 ) )
		return 1;

	drawn = 0;
	if ( m_gameTitleTime != 0 )
		drawn = 1;

	// Fixup level transitions
	for ( i = 0; i < maxHUDMessages; i++ )
	{
		// Assume m_parms.time contains last time
		if ( m_pMessages[i] )
		{
			pMessage = m_pMessages[i];
			if ( m_startTime[i] > gHUD[clientId].m_flTime )
				m_startTime[i] = gHUD[clientId].m_flTime + m_parms.time - m_startTime[i] + 0.2f;	// Server takes 0.2 seconds to spawn, adjust for this
		}
	}

	for ( i = 0; i < maxHUDMessages; i++ )
	{
		if ( m_pMessages[i] )
		{
			pMessage = m_pMessages[i];

			// This is when the message is over
			switch( pMessage->effect )
			{
			case 0:
			case 1:
				endTime = m_startTime[i] + pMessage->fadein + pMessage->fadeout + pMessage->holdtime;
				break;

			// Fade in is per character in scanning messages
			case 2:
				endTime = m_startTime[i] + (pMessage->fadein * strlen( pMessage->pMessage )) + pMessage->fadeout + pMessage->holdtime;
				break;

			default:
				endTime = 0;
				break;
			}

			if ( fTime <= endTime )
			{
				float messageTime = fTime - m_startTime[i];

				// Draw the message
				MessageDrawScan( pMessage, messageTime, clientId );

				drawn++;
			}
			else
			{
				// The message is over
				m_pMessages[i] = NULL;
			}
		}
	}

	// Remember the time -- to fix up level transitions
	m_parms.time = gHUD[clientId].m_flTime;
	// Don't call until we get another message
	if ( !drawn )
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}

void CHudMessage::MessageAdd( const char *pName, float time )
{
	int i;

	for ( i = 0; i < maxHUDMessages; i++ )
	{
		if ( !m_pMessages[i] )
		{
			m_pMessages[i] = TextMessageGet( pName );
			m_startTime[i] = time;
			return;
		}
	}
}

int CHudMessage::MsgFunc_HudText( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	char *pString = READ_STRING();

	MessageAdd( pString, gHUD[clientId].m_flTime );
	// Remember the time -- to fix up level transitions
	m_parms.time = gHUD[clientId].m_flTime;

	// Turn on drawing
	if ( !(m_iFlags & HUD_ACTIVE) )
		m_iFlags |= HUD_ACTIVE;

	return 1;
}

int CHudMessage::MsgFunc_GameTitle( int clientId, const char *pszName, int iSize, void *pbuf )
{
	m_pGameTitle = TextMessageGet( "GAMETITLE" );
	if ( m_pGameTitle != NULL )
	{
		m_gameTitleTime = gHUD[clientId].m_flTime;

		// Turn on drawing
		if ( !(m_iFlags & HUD_ACTIVE) )
			m_iFlags |= HUD_ACTIVE;
	}

	return 1;
}
