/* cl_dll/button_hud.cpp: só do PS2 (sem fonte; nomes inventados). Mostra a figura do
   controle e faz piscar os botões ligados aos comandos que a mensagem ButtonHud pede
   (usado pelo tutorial). As teclas do controle do PS2 vão de 0xD2 a 0xE9. */

#include "cl_dll/hud.h"

DECLARE_MESSAGE(m_Button, ButtonHud )


int CHudButton::Init( int clientId )
{
	HOOK_MESSAGE( ButtonHud );

	m_iButtons = 0;
	m_flTime = 0;
	m_iFlags = 0;

	gHUD[clientId].AddHudElem(this);

	m_flBlinkTime = 0;
	m_iBlink = 0;

	return 1;
}

void CHudButton::Reset( int clientId )
{
	m_iButtons = 0;
	m_flTime = 0;
	m_iFlags = 0;
	m_flBlinkTime = 0;
	m_iBlink = 0;
}

int CHudButton::VidInit( int clientId )
{
	m_iFlags = 0;
	m_iButtons = 0;
	m_flTime = 0;
	m_flBlinkTime = 0;
	m_iBlink = 0;

	m_HUD_controller = gHUD[clientId].GetSpriteIndex( "ps2_controller" );
	m_HUD_select = gHUD[clientId].GetSpriteIndex( "ps2_select" );
	m_HUD_start = gHUD[clientId].GetSpriteIndex( "ps2_start" );
	m_HUD_up = gHUD[clientId].GetSpriteIndex( "ps2_up" );
	m_HUD_left = gHUD[clientId].GetSpriteIndex( "ps2_left" );
	m_HUD_right = gHUD[clientId].GetSpriteIndex( "ps2_right" );
	m_HUD_down = gHUD[clientId].GetSpriteIndex( "ps2_down" );
	m_HUD_top2 = gHUD[clientId].GetSpriteIndex( "ps2_top2" );
	m_HUD_top1 = gHUD[clientId].GetSpriteIndex( "ps2_top1" );
	m_HUD_analog_up = gHUD[clientId].GetSpriteIndex( "ps2_analog_up" );
	m_HUD_analog_up_small = gHUD[clientId].GetSpriteIndex( "ps2_analog_up_small" );
	m_HUD_analog_down = gHUD[clientId].GetSpriteIndex( "ps2_analog_down" );
	m_HUD_analog_down_small = gHUD[clientId].GetSpriteIndex( "ps2_analog_down_small" );
	m_HUD_analog_right = gHUD[clientId].GetSpriteIndex( "ps2_analog_right" );
	m_HUD_analog_right_small = gHUD[clientId].GetSpriteIndex( "ps2_analog_right_small" );
	m_HUD_analog_left = gHUD[clientId].GetSpriteIndex( "ps2_analog_left" );
	m_HUD_analog_left_small = gHUD[clientId].GetSpriteIndex( "ps2_analog_left_small" );
	m_HUD_analog_move = gHUD[clientId].GetSpriteIndex( "ps2_analog_move" );
	m_HUD_analog_press = gHUD[clientId].GetSpriteIndex( "ps2_analog_press" );
	m_HUD_button = gHUD[clientId].GetSpriteIndex( "ps2_button" );
	m_HUD_controller_top = gHUD[clientId].GetSpriteIndex( "ps2_controller_top" );

	return 1;
}

int CHudButton::MsgFunc_ButtonHud( int clientId, const char *pszName, int iSize, void *pbuf )
{
	int iButtons;

	BEGIN_READ( pbuf, iSize );
	iButtons = READ_SHORT();
	m_flTime = gHUD[clientId].m_flTime + READ_SHORT() * 0.01f;

	m_iButtons = iButtons;
	m_iFlags |= HUD_ACTIVE;

	return 1;
}

/* primeira tecla do controle do PS2 ligada ao comando */
static int FindKey( char *pszCommand )
{
	int count = 0;
	int key;
	int ret;
	int i;

	for ( i = 0; ; i++ )
	{
		ret = Key_FindBinding( pszCommand, 1, i, &count, &key );
		if ( !count )
			return 0;
		if ( key >= 0xD2 && key < 0x150 )
			return key;
		if ( ret )
			return 0;
	}
}

int CHudButton::Draw( float flTime, int clientId, int iDrawFlags )
{
	int keys[13];
	int n, i, j, x;
	wrect_t rc;

	if ( !(iDrawFlags & 0xC) )
		return 1;

	if ( !m_iFlags )
		return 0;

	if ( m_flTime <= flTime )
		return 0;

	if ( m_flBlinkTime <= flTime )
	{
		m_iBlink++;
		if ( m_iBlink >= 3 )
			m_iBlink = 0;
		m_flBlinkTime = flTime + 0.5f;
	}

	n = 0;
	for ( j = 0; j < 13; j++ )
		keys[j] = 0;

	rc = gHUD[clientId].GetSpriteRect( m_HUD_controller );
	x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
	SPR_Set( gHUD[clientId].GetSprite( m_HUD_controller ), 255, 255, 255 );
	SPR_DrawHoles( 0, x, 100, &rc );

	SPR_Set( gHUD[clientId].GetSprite( m_HUD_controller_top ), 255, 255, 255 );
	SPR_DrawHoles( 0, x + 17, 73, &gHUD[clientId].GetSpriteRect( m_HUD_controller_top ) );
	SPR_Set( gHUD[clientId].GetSprite( m_HUD_controller_top ), 255, 255, 255 );
	SPR_DrawHoles( 0, x + 92, 73, &gHUD[clientId].GetSpriteRect( m_HUD_controller_top ) );

	if ( !m_iBlink )
		return 0;

	if ( m_iButtons & 0x1 )
		keys[n++] = FindKey( "+attack 0" );
	if ( m_iButtons & 0x2 )
		keys[n++] = FindKey( "+attack2 0" );
	if ( m_iButtons & 0x4 )
		keys[n++] = FindKey( "+forward 0" );
	if ( m_iButtons & 0x8 )
		keys[n++] = FindKey( "+back 0" );
	if ( m_iButtons & 0x10 )
		keys[n++] = FindKey( "+moveleft 0" );
	if ( m_iButtons & 0x20 )
		keys[n++] = FindKey( "+moveright 0" );
	if ( m_iButtons & 0x40 )
		keys[n++] = FindKey( "+jump 0" );
	if ( m_iButtons & 0x80 )
		keys[n++] = FindKey( "+duck 0" );
	if ( m_iButtons & 0x100 )
		keys[n++] = FindKey( "+reload 0" );
	if ( m_iButtons & 0x200 )
		keys[n++] = FindKey( "+use 0" );
	if ( m_iButtons & 0x400 )
		keys[n++] = FindKey( "+alt1 0" );
	if ( m_iButtons & 0x800 )
	{
		keys[n++] = FindKey( "invnext 0" );
		keys[n++] = FindKey( "invprev 0" );
	}
	if ( m_iButtons & 0x1000 )
		keys[n++] = FindKey( "impulse 0 100" );
	for ( i = 0; i < 13 && keys[i]; i++ )
	{
		switch ( keys[i] )
		{
		case 0xD2:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_up ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 24, 115, &gHUD[clientId].GetSpriteRect( m_HUD_up ) );
			break;
		case 0xD3:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_down ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 24, 127, &gHUD[clientId].GetSpriteRect( m_HUD_down ) );
			break;
		case 0xD4:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_right ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 17, 121, &gHUD[clientId].GetSpriteRect( m_HUD_right ) );
			break;
		case 0xD5:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_left ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 30, 121, &gHUD[clientId].GetSpriteRect( m_HUD_left ) );
			break;
		case 0xD6:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_button ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 95, 110, &gHUD[clientId].GetSpriteRect( m_HUD_button ) );
			break;
		case 0xD7:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_button ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 96, 128, &gHUD[clientId].GetSpriteRect( m_HUD_button ) );
			break;
		case 0xD8:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_button ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 85, 119, &gHUD[clientId].GetSpriteRect( m_HUD_button ) );
			break;
		case 0xD9:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_button ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 106, 117, &gHUD[clientId].GetSpriteRect( m_HUD_button ) );
			break;
		case 0xDA:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_top1 ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 17, 87, &gHUD[clientId].GetSpriteRect( m_HUD_top1 ) );
			break;
		case 0xDB:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_top2 ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 19, 75, &gHUD[clientId].GetSpriteRect( m_HUD_top2 ) );
			break;
		case 0xDC:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 40, 136, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 41, 141, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down_small ) );
			}
			break;
		case 0xE2:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_up ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 40, 128, &gHUD[clientId].GetSpriteRect( m_HUD_analog_up ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_up_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 41, 123, &gHUD[clientId].GetSpriteRect( m_HUD_analog_up_small ) );
			}
			break;
		case 0xE3:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 40, 146, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 42, 151, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down_small ) );
			}
			break;
		case 0xE5:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_right ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 53, 133, &gHUD[clientId].GetSpriteRect( m_HUD_analog_right ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_right_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 60, 135, &gHUD[clientId].GetSpriteRect( m_HUD_analog_right_small ) );
			}
			break;
		case 0xE4:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_left ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 33, 134, &gHUD[clientId].GetSpriteRect( m_HUD_analog_left ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_left_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 28, 135, &gHUD[clientId].GetSpriteRect( m_HUD_analog_left_small ) );
			}
			break;
		case 0xDD:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_top1 ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 94, 87, &gHUD[clientId].GetSpriteRect( m_HUD_top1 ) );
			break;
		case 0xDE:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_top2 ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 94, 75, &gHUD[clientId].GetSpriteRect( m_HUD_top2 ) );
			break;
		case 0xDF:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 77, 136, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 78, 141, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down_small ) );
			}
			break;
		case 0xE6:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_up ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 78, 128, &gHUD[clientId].GetSpriteRect( m_HUD_analog_up ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_up_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 79, 123, &gHUD[clientId].GetSpriteRect( m_HUD_analog_up_small ) );
			}
			break;
		case 0xE7:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 76, 146, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_down_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 78, 151, &gHUD[clientId].GetSpriteRect( m_HUD_analog_down_small ) );
			}
			break;
		case 0xE9:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_right ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 90, 133, &gHUD[clientId].GetSpriteRect( m_HUD_analog_right ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_right_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 97, 135, &gHUD[clientId].GetSpriteRect( m_HUD_analog_right_small ) );
			}
			break;
		case 0xE8:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_left ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 70, 135, &gHUD[clientId].GetSpriteRect( m_HUD_analog_left ) );
			if ( m_iBlink == 2 )
			{
				SPR_Set( gHUD[clientId].GetSprite( m_HUD_analog_left_small ), 255, 255, 255 );
				SPR_DrawHoles( 0, x + 65, 136, &gHUD[clientId].GetSpriteRect( m_HUD_analog_left_small ) );
			}
			break;
		case 0xE0:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_select ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 50, 123, &gHUD[clientId].GetSpriteRect( m_HUD_select ) );
			break;
		case 0xE1:
			SPR_Set( gHUD[clientId].GetSprite( m_HUD_start ), 255, 255, 255 );
			SPR_DrawHoles( 0, x + 72, 123, &gHUD[clientId].GetSpriteRect( m_HUD_start ) );
			break;
		}
	}

	return 0;
}
