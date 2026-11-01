/* cl_dll/hud_msg.cpp: mensagens do CHud do HLSDK, versão do PS2 (sem Logo; Concuss usa a
   cor do HUD; mensagem nova que troca a cor do HUD). */

#include "cl_dll/hud.h"

float READ_COORD( void );


int CHud :: MsgFunc_ResetHUD( int clientId, const char *pszName, int iSize, void *pbuf )
{
	// clear all hud data
	HUDLIST *pList = m_pHudList;

	while ( pList )
	{
		if ( pList->p )
			pList->p->Reset( clientId );
		pList = pList->pNext;
	}

	// reset sensitivity
	m_flMouseSensitivity = 0;

	// reset concussion effect
	m_iConcussionEffect = 0;

	return 1;
}

void CHud :: MsgFunc_InitHUD( int clientId, const char *pszName, int iSize, void *pbuf )
{
	// prepare all hud data
	HUDLIST *pList = m_pHudList;

	while (pList)
	{
		if ( pList->p )
			pList->p->InitHUDData( clientId );
		pList = pList->pNext;
	}
}

int CHud :: MsgFunc_GameMode( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_Teamplay = READ_BYTE();

	return 1;
}

int CHud :: MsgFunc_Damage( int clientId, const char *pszName, int iSize, void *pbuf )
{
	int		armor, blood;
	vec3_t	from;
	int		i;
	float	count;

	BEGIN_READ( pbuf, iSize );
	armor = READ_BYTE();
	blood = READ_BYTE();

	for (i=0 ; i<3 ; i++)
		from[i] = READ_COORD();

	count = (blood * 0.5f) + (armor * 0.5f);

	if (count < 10)
		count = 10;

	// TODO: kick viewangles,  show damage visually

	return 1;
}

int CHud :: MsgFunc_Concuss( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iConcussionEffect = READ_BYTE();
	if (m_iConcussionEffect)
		this->m_StatusIcons.EnableIcon("dmg_concuss", gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB, clientId);
	else
		this->m_StatusIcons.DisableIcon("dmg_concuss");
	return 1;
}

int CHud :: MsgFunc_HudColor( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	m_iColorR = READ_BYTE();
	m_iColorG = READ_BYTE();
	m_iColorB = READ_BYTE();

	return 1;
}
