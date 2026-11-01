/* cl_dll/flashlight.cpp: CHudFlashlight do HLSDK, versão do PS2 (só desenha com os flags
   0xA do Draw, esconde com os bits 6 e 0x40 do m_iPs2Flags ou com m_iWeaponBits >= 0,
   cor do HUD do jogador no lugar do RGB_YELLOWISH e 8/24 pixels de margem). */

#include "cl_dll/hud.h"

DECLARE_MESSAGE(m_Flash, FlashBat)
DECLARE_MESSAGE(m_Flash, Flashlight)

int CHudFlashlight::Init( int clientId )
{
	m_fFade = 0;
	m_fOn = 0;

	HOOK_MESSAGE(Flashlight);
	HOOK_MESSAGE(FlashBat);

	m_iFlags |= HUD_ACTIVE;

	gHUD[clientId].AddHudElem(this);

	return 1;
}

void CHudFlashlight::Reset( int clientId )
{
	m_fFade = 0;
	m_fOn = 0;
}

int CHudFlashlight::VidInit( int clientId )
{
	int HUD_flash_empty = gHUD[clientId].GetSpriteIndex( "flash_empty" );
	int HUD_flash_full = gHUD[clientId].GetSpriteIndex( "flash_full" );
	int HUD_flash_beam = gHUD[clientId].GetSpriteIndex( "flash_beam" );

	m_hSprite1 = gHUD[clientId].GetSprite(HUD_flash_empty);
	m_hSprite2 = gHUD[clientId].GetSprite(HUD_flash_full);
	m_hBeam = gHUD[clientId].GetSprite(HUD_flash_beam);
	m_prc1 = &gHUD[clientId].GetSpriteRect(HUD_flash_empty);
	m_prc2 = &gHUD[clientId].GetSpriteRect(HUD_flash_full);
	m_prcBeam = &gHUD[clientId].GetSpriteRect(HUD_flash_beam);
	m_iWidth = m_prc2->right - m_prc2->left;

	return 1;
}

int CHudFlashlight::MsgFunc_FlashBat( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int x = READ_BYTE();
	m_iBat = x;
	m_flBat = ((float)x)/100.0f;

	return 1;
}

int CHudFlashlight::MsgFunc_Flashlight( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_fOn = READ_BYTE();
	int x = READ_BYTE();
	m_iBat = x;
	m_flBat = ((float)x)/100.0f;

	return 1;
}

int CHudFlashlight::Draw( float flTime, int clientId, int iDrawFlags )
{
	if ( !( iDrawFlags & 0xA ) )
		return 1;

	if ( gHUD[clientId].m_iPs2Flags & 6 )
		return 1;

	int r, g, b, x, y, a;
	wrect_t rc;

	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return 1;

	if ( gHUD[clientId].m_iWeaponBits >= 0 )
		return 1;

	if (m_fOn)
		a = 225;
	else
		a = 100;	// MIN_ALPHA

	if (m_flBat < 0.20f)
	{
		r = 255;
		g = 16;
		b = 16;
	}
	else
	{
		r = gHUD[clientId].m_iColorR;
		g = gHUD[clientId].m_iColorG;
		b = gHUD[clientId].m_iColorB;
	}

	ScaleColors(r, g, b, a);

	y = (m_prc1->bottom - m_prc2->top)/2 + 24;
	x = ScreenWidth - m_iWidth - m_iWidth/2 - 8;

	// Draw the flashlight casing
	SPR_Set(m_hSprite1, r, g, b );
	SPR_DrawAdditive( 0,  x, y, m_prc1);

	if ( m_fOn )
	{  // draw the flashlight beam
		x = ScreenWidth - m_iWidth/2 - 8;

		SPR_Set( m_hBeam, r, g, b );
		SPR_DrawAdditive( 0, x, y, m_prcBeam );
	}

	// draw the flashlight energy level
	x = ScreenWidth - m_iWidth - m_iWidth/2 - 8;
	int iOffset = m_iWidth * (1.0f - m_flBat);
	if (iOffset < m_iWidth)
	{
		rc = *m_prc2;
		rc.left += iOffset;

		SPR_Set(m_hSprite2, r, g, b );
		SPR_DrawAdditive( 0, x + iOffset, y, &rc);
	}

	return 1;
}
