/* cl_dll/islave_hud.cpp: HUD do vortigaunt jogável, só do PS2 (sem fonte; nomes inventados).
   Mostra a carga do ataque elétrico no centro da tela e a vida embaixo. */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

DECLARE_MESSAGE(m_ISlave, ISlaveHud )


int CHudISlave::Init( int clientId )
{
	HOOK_MESSAGE( ISlaveHud );

	gHUD[clientId].AddHudElem(this);

	m_iFlags |= HUD_ACTIVE;
	m_iHealth = 100;
	m_iPs2Unk30 = 0;

	return 1;
}

void CHudISlave::Reset( int clientId )
{
	m_iFlags |= HUD_ACTIVE;
	m_iHealth = gHUD[clientId].m_iPs2Health;
	m_iPs2Unk30 = 0;
}

int CHudISlave::VidInit( int clientId )
{
	m_iFlags |= HUD_ACTIVE;

	m_HUD_center = gHUD[clientId].GetSpriteIndex( "islave_center" );
	m_HUD_charged = gHUD[clientId].GetSpriteIndex( "islave_charged" );
	m_HUD_inner = gHUD[clientId].GetSpriteIndex( "islave_inner" );
	m_HUD_outer = gHUD[clientId].GetSpriteIndex( "islave_outer" );
	m_HUD_health = gHUD[clientId].GetSpriteIndex( "islave_health" );

	return 1;
}

int CHudISlave::MsgFunc_ISlaveHud( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iState = READ_BYTE();

	m_iFlags |= HUD_ACTIVE;

	return 1;
}

void CHudISlave::DrawCharge( int clientId )
{
	int r = 0x60, g = 0x80, b = 0x10;
	wrect_t rc;
	int y, x;

	switch ( m_iState )
	{
	case 3:
		rc = gHUD[clientId].GetSpriteRect( m_HUD_charged );
		x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
		y = ScreenHeight / 2 - (rc.bottom - rc.top) / 2;
		SPR_Set( gHUD[clientId].GetSprite( m_HUD_charged ), 255, 0, 0 );
		SPR_DrawAdditive( 0, x, y, &rc );
	case 1:
		rc = gHUD[clientId].GetSpriteRect( m_HUD_inner );
		y = ScreenHeight / 2 - (rc.bottom - rc.top) / 2;
		y -= 2;
		x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
		SPR_Set( gHUD[clientId].GetSprite( m_HUD_inner ), 0x60, 0x80, 0x10 );
		SPR_DrawAdditive( 0, x, y, &rc );
	case 2:
		rc = gHUD[clientId].GetSpriteRect( m_HUD_outer );
		y = ScreenHeight / 2 - (rc.bottom - rc.top) / 2;
		y -= 2;
		x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
		SPR_Set( gHUD[clientId].GetSprite( m_HUD_outer ), 0x60, 0x80, 0x10 );
		SPR_DrawAdditive( 0, x, y, &rc );
	case 0:
		if ( m_iState == 3 )
		{
			r = 255;
			b = 0;
			g = 0;
		}
		rc = gHUD[clientId].GetSpriteRect( m_HUD_center );
		x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
		y = ScreenHeight / 2 - (rc.bottom - rc.top) / 2;
		SPR_Set( gHUD[clientId].GetSprite( m_HUD_center ), r, g, b );
		SPR_DrawAdditive( 0, x, y, &rc );
		break;
	}
}

int CHudISlave::Draw( float fTime, int clientId, int iDrawFlags )
{
#ifndef NON_MATCHING
	/* não casa: o alvo repete a conversão (int)flCut três vezes (cvt.w.s + mfc1 cada);
	   o nosso gcc junta as três numa só (mesmo problema das conversões float->int das dlls) */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/islave_hud", Draw__10CHudISlavefii );
#endif
	wrect_t rc;
	int x, y;
	float flWidth, flCut;

	if ( !(gHUD[clientId].m_iPs2Flags & 0x40) )
		return 0;

	if ( iDrawFlags & 0x24 )
		DrawCharge( clientId );

	y = ScreenHeight - gHUD[clientId].m_iFontHeight - gHUD[clientId].m_iFontHeight / 2;

	if ( !(iDrawFlags & 0x14) )
		return 1;

	m_iHealth = gHUD[clientId].m_iPs2Health;

	rc = gHUD[clientId].GetSpriteRect( m_HUD_health );
	x = ScreenWidth / 2 - (rc.right - rc.left) / 2;
	SPR_Set( gHUD[clientId].GetSprite( m_HUD_health ), 0x30, 0x40, 0x08 );
	SPR_DrawAdditive( 0, x, y, &rc );

	flWidth = rc.right - rc.left;
	flCut = m_iHealth * 0.01f;
	flCut = flWidth - flCut * flWidth;
	if ( flCut == flWidth )
		return 0;

	flCut *= 0.5f;
	rc.left += (int)flCut;
	x += (int)flCut;
	rc.right -= (int)flCut;

	SPR_Set( gHUD[clientId].GetSprite( m_HUD_health ), 0x60, 0x80, 0x10 );
	SPR_DrawAdditive( 0, x, y, &rc );

	return 0;
}
