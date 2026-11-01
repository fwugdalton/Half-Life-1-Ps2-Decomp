/* cl_dll/battery.cpp: CHudBattery do HLSDK, versão do PS2 (só com os flags 0x14 do Draw;
   some com m_iPs2Flags & 8, o título do jogo ou m_iPs2Unk20E4 >= 0; cor do HUD do
   jogador; posição na tela dividida igual ao health.cpp). */

#include "cl_dll/hud.h"

extern "C" int cl_numlocal;
extern "C" int D_00399564;

#define FADE_TIME	100
#define MIN_ALPHA	100

#define min(a,b)	(((a) < (b)) ? (a) : (b))

DECLARE_MESSAGE(m_Battery, Battery)

int CHudBattery::Init( int clientId )
{
	m_iBat = 0;
	m_fFade = 0;
	m_iFlags = 0;

	HOOK_MESSAGE(Battery);

	gHUD[clientId].AddHudElem(this);

	return 1;
}

int CHudBattery::VidInit( int clientId )
{
	int HUD_suit_empty = gHUD[clientId].GetSpriteIndex( "suit_empty" );
	int HUD_suit_full = gHUD[clientId].GetSpriteIndex( "suit_full" );

	m_hSprite1 = m_hSprite2 = 0;  // delaying get sprite handles until we know the sprites are loaded
	m_prc1 = &gHUD[clientId].GetSpriteRect( HUD_suit_empty );
	m_prc2 = &gHUD[clientId].GetSpriteRect( HUD_suit_full );
	m_iHeight = m_prc2->bottom - m_prc1->top;
	m_fFade = 0;
	return 1;
}

int CHudBattery::MsgFunc_Battery( int clientId, const char *pszName, int iSize, void *pbuf )
{
	m_iFlags |= HUD_ACTIVE;

	BEGIN_READ( pbuf, iSize );
	int x = READ_SHORT();

	if ( x != m_iBat )
	{
		m_fFade = FADE_TIME;
		m_iBat = x;
	}

	return 1;
}

int CHudBattery::Draw( float flTime, int clientId, int iDrawFlags )
{
	if ( !( iDrawFlags & 0x14 ) )
		return 1;

	if ( gHUD[clientId].m_iPs2Flags & 8 )
		return 1;

	if ( gHUD[0].m_Message.m_gameTitleTime > 0 )
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	rc = *m_prc2;

	rc.top  += (int)(m_iHeight * ((float)(100-(min(100,m_iBat))) * 0.01f));	// battery can go from 0 to 100 so * 0.01 goes from 0 to 1

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	if ( gHUD[clientId].m_iPs2Unk20E4 >= 0 )
		return 1;

	// Has health changed? Flash the health #
	if (m_fFade)
	{
		if (m_fFade > FADE_TIME)
			m_fFade = FADE_TIME;

		m_fFade -= (gHUD[clientId].m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = 128;
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	ScaleColors(r, g, b, a );

	int iOffset = (m_prc1->bottom - m_prc1->top)/6;

	y = ScreenHeight - gHUD[clientId].m_iFontHeight - gHUD[clientId].m_iFontHeight / 2 - 24;

	if ( cl_numlocal >= 2 && !D_00399564 )
		x = ScreenWidth / 2 + 8;
	else
		x = ScreenWidth / 4 + 8;

	// make sure we have the right sprite handles
	if ( !m_hSprite1 )
		m_hSprite1 = gHUD[clientId].GetSprite( gHUD[clientId].GetSpriteIndex( "suit_empty" ) );
	if ( !m_hSprite2 )
		m_hSprite2 = gHUD[clientId].GetSprite( gHUD[clientId].GetSpriteIndex( "suit_full" ) );

	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y - iOffset, m_prc1);

	if (rc.bottom > rc.top)
	{
		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x, y - iOffset + (rc.top - m_prc2->top), &rc);
	}

	x += (m_prc1->right - m_prc1->left);
	x = gHUD[clientId].DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iBat, r, g, b);

	return 1;
}
