/* cl_dll/cl_hud.cpp: o hud.cpp do HLSDK com o nome do PS2 (a string do Mem_DbgMalloc é
   "cl_dll/cl_hud.cpp"). Init/VidInit por jogador local, cor do HUD amarela por padrão,
   cvars só no jogador 0 (flags 0), sem cl_lw no SetFOV e o elemento CHudCrouch novo. */

#include "cl_dll/hud.h"

extern "C" {
int sprintf( char *buf, const char *fmt, ... );
char *strncpy( char *dst, const char *src, unsigned int n );
int strncmp( const char *a, const char *b, unsigned int n );
void *memset( void *dst, int c, unsigned int n );
void *Mem_DbgMalloc( int size, const char *file, int line );
void Mem_Free( void *ptr );
}

#undef CVAR_GET_FLOAT
inline float CVAR_GET_FLOAT( const char *x ) { return gEngfuncs.pfnGetCvarFloat( (char*)x ); }
#define CVAR_CREATE( x, y, z ) gEngfuncs.pfnRegisterVariable( (char*)x, (char*)y, z )
#define SPR_GetList (*gEngfuncs.pfnSPR_GetList)
#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)

#define MAX_SPRITE_NAME_LENGTH	24

int __MsgFunc_Logo( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_Logo( clientId, pszName, iSize, pbuf );
}

int __MsgFunc_ResetHUD( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_ResetHUD( clientId, pszName, iSize, pbuf );
}

int __MsgFunc_InitHUD( int clientId, const char *pszName, int iSize, void *pbuf )
{
	gHUD[clientId].MsgFunc_InitHUD( clientId, pszName, iSize, pbuf );
	return 1;
}

int __MsgFunc_SetFOV( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_SetFOV( clientId, pszName, iSize, pbuf );
}

int __MsgFunc_Concuss( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_Concuss( clientId, pszName, iSize, pbuf );
}

int __MsgFunc_GameMode( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_GameMode( clientId, pszName, iSize, pbuf );
}

int __MsgFunc_HudColor( int clientId, const char *pszName, int iSize, void *pbuf )
{
	return gHUD[clientId].MsgFunc_HudColor( clientId, pszName, iSize, pbuf );
}

// This is called every time the DLL is loaded
void CHud :: Init( int clientId )
{
	m_iColorR = 255;
	m_iColorG = 160;
	m_iColorB = 0;

	HOOK_MESSAGE( Logo );
	HOOK_MESSAGE( ResetHUD );
	HOOK_MESSAGE( GameMode );
	HOOK_MESSAGE( InitHUD );
	HOOK_MESSAGE( SetFOV );
	HOOK_MESSAGE( Concuss );
	HOOK_MESSAGE( HudColor );

	m_iLogo = 0;
	m_iFOV = 0;

	if ( !clientId )
	{
		CVAR_CREATE( "zoom_sensitivity_ratio", "1.2", 0 );
		CVAR_CREATE( "default_fov", "90", 0 );
	}

	m_pSpriteList = NULL;

	// Clear any old HUD list
	if ( m_pHudList )
	{
		HUDLIST *pList;
		while ( m_pHudList )
		{
			pList = m_pHudList;
			m_pHudList = m_pHudList->pNext;
			Mem_Free( pList );
		}
		m_pHudList = NULL;
	}

	// In case we get messages before the first update -- time will be valid
	m_flTime = 1.0;

	m_Ammo.Init( clientId );
	m_Health.Init( clientId );
	m_Geiger.Init( clientId );
	m_Train.Init( clientId );
	m_Battery.Init( clientId );
	m_Flash.Init( clientId );
	m_Crouch.Init( clientId );
	m_Message.Init( clientId );
	m_Scoreboard.Init( clientId );
	m_MOTD.Init( clientId );
	m_StatusBar.Init( clientId );
	m_DeathNotice.Init( clientId );
	m_AmmoSecondary.Init( clientId );
	m_TextMessage.Init( clientId );
	m_StatusIcons.Init( clientId );
	m_ISlave.Init( clientId );
	m_Button.Init( clientId );
	m_SayText.Init( clientId );
	m_Menu.Init( clientId );

	MsgFunc_ResetHUD( clientId, 0, 0, 0 );
}

// CHud destructor
// cleans up memory allocated for m_rg* arrays
CHud :: ~CHud()
{
	delete [] m_rghSprites;
	delete [] m_rgrcRects;
	delete [] m_rgszSpriteNames;
}

// GetSpriteIndex()
// searches through the sprite list loaded from hud.txt for a name matching SpriteName
// returns an index into the gHUD.m_rghSprites[] array
// returns 0 if sprite not found
int CHud :: GetSpriteIndex( const char *SpriteName )
{
	// look through the loaded sprite name list for SpriteName
	for ( int i = 0; i < m_iSpriteCount; i++ )
	{
		if ( strncmp( SpriteName, m_rgszSpriteNames + (i * MAX_SPRITE_NAME_LENGTH), MAX_SPRITE_NAME_LENGTH ) == 0 )
			return i;
	}

	return -1; // invalid sprite
}

void CHud :: VidInit( int clientId )
{
	m_scrinfo.iSize = sizeof(m_scrinfo);
	GetScreenInfo(&m_scrinfo);

	// ----------
	// Load Sprites
	// ---------
	m_hsprLogo = 0;

	if (ScreenWidth < 512)
		m_iRes = 320;
	else
		m_iRes = 640;

	// Only load this once
	if ( !m_pSpriteList )
	{
		// we need to load the hud.txt, and all sprites within
		m_pSpriteList = SPR_GetList("sprites/hud.txt", &m_iSpriteCountAllRes);

		if (m_pSpriteList)
		{
			// count the number of sprites of the appropriate res
			m_iSpriteCount = 0;
			client_sprite_t *p = m_pSpriteList;
			int j;
			for ( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
					m_iSpriteCount++;
				p++;
			}

			// allocated memory for sprite handle arrays
			m_rghSprites = new HSPRITE[m_iSpriteCount];
			m_rgrcRects = new wrect_t[m_iSpriteCount];
			m_rgszSpriteNames = new char[m_iSpriteCount * MAX_SPRITE_NAME_LENGTH];

			p = m_pSpriteList;
			int index = 0;
			for ( j = 0; j < m_iSpriteCountAllRes; j++ )
			{
				if ( p->iRes == m_iRes )
				{
					char sz[256];
					sprintf(sz, "sprites/%s.spz", p->szSprite);
					m_rghSprites[index] = SPR_Load(sz);
					m_rgrcRects[index] = p->rc;
					strncpy( &m_rgszSpriteNames[index * MAX_SPRITE_NAME_LENGTH], p->szName, MAX_SPRITE_NAME_LENGTH );

					index++;
				}

				p++;
			}
		}
	}
	else
	{
		// we have already have loaded the sprite reference from hud.txt, but
		// we need to make sure all the sprites have been loaded (we've gone through a transition, or loaded a save game)
		client_sprite_t *p = m_pSpriteList;
		int index = 0;
		for ( int j = 0; j < m_iSpriteCountAllRes; j++ )
		{
			if ( p->iRes == m_iRes )
			{
				char sz[256];
				sprintf( sz, "sprites/%s.spz", p->szSprite );
				m_rghSprites[index] = SPR_Load(sz);
				index++;
			}

			p++;
		}
	}

	// assumption: number_1, number_2, etc, are all listed and loaded sequentially
	m_HUD_number_0 = GetSpriteIndex( "number_0" );

	m_iFontHeight = m_rgrcRects[m_HUD_number_0].bottom - m_rgrcRects[m_HUD_number_0].top;

	m_Ammo.VidInit( clientId );
	m_Health.VidInit( clientId );
	m_Geiger.VidInit( clientId );
	m_Train.VidInit( clientId );
	m_Battery.VidInit( clientId );
	m_Flash.VidInit( clientId );
	m_Crouch.VidInit( clientId );
	m_Message.VidInit( clientId );
	m_Scoreboard.VidInit( clientId );
	m_MOTD.VidInit( clientId );
	m_StatusBar.VidInit( clientId );
	m_DeathNotice.VidInit( clientId );
	m_SayText.VidInit( clientId );
	m_Menu.VidInit( clientId );
	m_AmmoSecondary.VidInit( clientId );
	m_StatusIcons.VidInit( clientId );
	m_ISlave.VidInit( clientId );
	m_Button.VidInit( clientId );
}

int CHud::MsgFunc_Logo( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	// update Train data
	m_iLogo = READ_BYTE();

	return 1;
}

int CHud::MsgFunc_SetFOV( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int newfov = READ_BYTE();
	int def_fov = CVAR_GET_FLOAT( "default_fov" );

	if ( newfov == 0 )
	{
		m_iFOV = def_fov;
	}
	else
	{
		m_iFOV = newfov;
	}

	// the clients fov is actually set in the client data update section of the hud

	// Set a new sensitivity
	if ( m_iFOV == def_fov )
	{
		// reset to saved sensitivity
		m_flMouseSensitivity = 0;
	}
	else
	{
		// set a new sensitivity that is proportional to the change from the FOV default
		m_flMouseSensitivity = CVAR_GET_FLOAT("sensitivity") * ((float)newfov / (float)def_fov) * CVAR_GET_FLOAT("zoom_sensitivity_ratio");
	}

	return 1;
}

void CHud::AddHudElem( CHudBase *phudelem )
{
	HUDLIST *pdl, *ptemp;

	if (!phudelem)
		return;

	pdl = (HUDLIST *)Mem_DbgMalloc( sizeof(HUDLIST), "cl_dll/cl_hud.cpp", 318 );
	if (!pdl)
		return;

	memset(pdl, 0, sizeof(HUDLIST));
	pdl->p = phudelem;

	if (!m_pHudList)
	{
		m_pHudList = pdl;
		return;
	}

	ptemp = m_pHudList;

	while (ptemp->pNext)
		ptemp = ptemp->pNext;

	ptemp->pNext = pdl;
}

HSPRITE LoadSprite( const char *pszName )
{
	int i;
	char sz[256];

	if (gHUD[0].m_scrinfo.iWidth < 512)
		i = 320;
	else
		i = 640;

	sprintf(sz, pszName, i);

	return SPR_Load(sz);
}
