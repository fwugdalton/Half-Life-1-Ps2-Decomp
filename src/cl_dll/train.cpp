/* cl_dll/train.cpp: CHudTrain do HLSDK, versão do PS2 (HUD por jogador). */

#include "cl_dll/hud.h"

DECLARE_MESSAGE(m_Train, Train )


int CHudTrain::Init( int clientId )
{
	HOOK_MESSAGE( Train );

	m_iPos = 0;
	m_iFlags = 0;
	gHUD[clientId].AddHudElem(this);

	return 1;
};

int CHudTrain::VidInit( int clientId )
{
	m_hSprite = 0;

	return 1;
};

int CHudTrain::Draw( float fTime, int clientId, int iDrawFlags )
{
	if ( !(iDrawFlags & 0x24) )
		return 1;

	if ( !m_hSprite )
		m_hSprite = LoadSprite("sprites/%d_train.spz");

	if (m_iPos)
	{
		int x, y;

		SPR_Set(m_hSprite, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB );

		// This should show up to the right and part way up the armor number
		y = ScreenHeight - SPR_Height(m_hSprite,0) - gHUD[clientId].m_iFontHeight - 24;
		x = ScreenWidth/3 + SPR_Width(m_hSprite,0)/4 + 8;

		SPR_DrawAdditive( m_iPos - 1,  x, y, NULL);
	}

	return 1;
}


int CHudTrain::MsgFunc_Train( int clientId, const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iPos = READ_BYTE();

	if (m_iPos)
		m_iFlags |= HUD_ACTIVE;
	else
		m_iFlags &= ~HUD_ACTIVE;

	return 1;
}
