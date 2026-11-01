/* cl_dll/crouch.cpp: elemento do HUD só do PS2 que mostra se o jogador está agachado,
   andando ou correndo (mensagem CrouchState; sprites crouch_on, walk e run). O nome do
   arquivo é um palpite (fica entre cl_hud.cpp e death.cpp na ROM). */

#include "cl_dll/hud.h"

DECLARE_MESSAGE( m_Crouch, CrouchState )

int CHudCrouch::Init( int clientId )
{
	m_iState = 0;

	HOOK_MESSAGE( CrouchState );

	m_iFlags |= HUD_ACTIVE;

	gHUD[clientId].AddHudElem( this );

	return 1;
}

void CHudCrouch::Reset( int clientId )
{
	m_iState = 0;
}

int CHudCrouch::VidInit( int clientId )
{
	int HUD_crouch_on = gHUD[clientId].GetSpriteIndex( "crouch_on" );
	int HUD_walk = gHUD[clientId].GetSpriteIndex( "walk" );
	int HUD_run = gHUD[clientId].GetSpriteIndex( "run" );

	m_hCrouch = gHUD[clientId].GetSprite( HUD_crouch_on );
	m_hWalk = gHUD[clientId].GetSprite( HUD_walk );
	m_hRun = gHUD[clientId].GetSprite( HUD_run );
	m_prcCrouch = &gHUD[clientId].GetSpriteRect( HUD_crouch_on );
	m_prcWalk = &gHUD[clientId].GetSpriteRect( HUD_walk );
	m_prcRun = &gHUD[clientId].GetSpriteRect( HUD_run );
	m_iWidth = m_prcWalk->right - m_prcWalk->left;

	return 1;
}

int CHudCrouch::MsgFunc_CrouchState( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iState = READ_BYTE();

	return 1;
}

int CHudCrouch::Draw( float flTime, int clientId, int iDrawFlags )
{
	int r, g, b, x, y;

	if ( !( iDrawFlags & 0xA ) )
		return 1;

	if ( gHUD[clientId].m_iPs2Flags & 4 )
		return 1;

	if ( gHUD[clientId].m_iPs2Flags & 8 )
		return 1;

	if ( gHUD[0].m_Message.m_gameTitleTime > 0 )
		return 1;

	if ( gHUD[clientId].m_iWeaponBits >= 0 )
		return 1;

	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return 1;

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	ScaleColors( r, g, b, 225 );

	y = (m_prcCrouch->bottom - m_prcWalk->top)/2 + 56;
	x = ScreenWidth - m_iWidth - m_iWidth/2 - 8;

	switch ( m_iState )
	{
	case 1:
		SPR_Set( m_hRun, r, g, b );
		SPR_DrawAdditive( 0, x, y, m_prcRun );
		break;
	case 2:
		SPR_Set( m_hCrouch, r, g, b );
		SPR_DrawAdditive( 0, x, y, m_prcCrouch );
		break;
	default:
		SPR_Set( m_hWalk, r, g, b );
		SPR_DrawAdditive( 0, x, y, m_prcWalk );
		break;
	}

	return 1;
}
