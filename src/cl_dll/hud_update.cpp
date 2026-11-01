/* cl_dll/hud_update.cpp: CHud::UpdateClientData do HLSDK, versão do PS2 (o client_data_t
   do PS2 leva a concussão e a sensibilidade do mouse; sem CL_ButtonBits). */

#include "cl_dll/hud.h"

extern "C" void *memcpy( void *dst, const void *src, unsigned int n );

int CHud::UpdateClientData( client_data_t *cdata, int clientId )
{
	memcpy( m_vecOrigin, cdata->origin, sizeof(vec3_t) );
	memcpy( m_vecAngles, cdata->viewangles, sizeof(vec3_t) );

	m_iWeaponBits = cdata->iWeaponBits;
	m_iPs2Unk20E4 = cdata->iPs2Unk30;

	gHUD[clientId].Think( clientId );

	cdata->iWeaponBits = m_iWeaponBits;
	cdata->fov = m_iFOV;
	cdata->idlescale = m_iConcussionEffect;

	if ( m_flMouseSensitivity )
		cdata->mouse_sensitivity = m_flMouseSensitivity;

	return 1;
}
