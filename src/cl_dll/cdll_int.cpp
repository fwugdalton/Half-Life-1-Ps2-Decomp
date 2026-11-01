/* cl_dll/cdll_int.cpp: interface do cliente do HLSDK (versão antiga), versão do PS2.
   Define gHUD (um CHud por jogador local, gHUD[2]) e gEngfuncs; as funções HUD_* recebem
   o clientId. Sem partículas, VGUI ou eventos. */

#include "cl_dll/hud.h"

extern "C" void *memcpy( void *dst, const void *src, unsigned int n );

#define CLDLL_INTERFACE_VERSION	6

CHud gHUD[2];
cl_enginefunc_t gEngfuncs;

int Initialize( cl_enginefunc_t *pEnginefuncs, int iVersion )
{
	gEngfuncs = *pEnginefuncs;

	if (iVersion != CLDLL_INTERFACE_VERSION)
		return 0;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	return 1;
}

int HUD_VidInit( int clientId )
{
	gHUD[clientId].VidInit( clientId );

	return 1;
}

int HUD_Init( int clientId )
{
	gHUD[clientId].Init( clientId );

	return 1;
}

int HUD_Redraw( float time, int intermission, int clientId, int iDrawFlags )
{
	gHUD[clientId].Redraw( time, intermission, clientId, iDrawFlags );

	return 1;
}

int HUD_UpdateClientData( client_data_t *pcldata, int clientId )
{
	return gHUD[clientId].UpdateClientData( pcldata, clientId );
}

int HUD_Reset( int clientId )
{
	gHUD[clientId].VidInit( clientId );

	return 1;
}
