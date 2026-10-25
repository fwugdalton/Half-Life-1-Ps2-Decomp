/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

===== lights.cpp ========================================================

  spawn and think functions for editor-placed lights

  Diferenças do PS2: a chave "_light" do CLight guarda a cor (bytes em 0x38) e o
  raio (0x3C); com spawnflags & 2 a luz manda um TE_ELIGHT (SendELight, virtual nova
  depois do Ps2Unk58, também
  chamado pelo think ELightThink) para os clientes. Existe o nome entity_light, os
  cvars do céu são cl_skycolor_* / cl_skyvec_* e o light_environment faz as contas
  em float.
*/

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"

#define SF_LIGHT_START_OFF	1
#define SF_LIGHT_ELIGHT		2
#define TE_ELIGHT		28

extern "C" int sscanf(const char *, const char *, ...);
extern "C" double pow(double, double);


class CLight : public CPointEntity
{
public:
	virtual void	KeyValue( KeyValueData* pkvd ); 
	virtual void	Spawn( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void	SendELight( void );
	void	EXPORT ELightThink( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

private:
	int		m_iStyle;
	int		m_iszPattern;
	unsigned char	m_color[3];
	int		m_iRadius;
};
LINK_ENTITY_TO_CLASS( light, CLight );

TYPEDESCRIPTION	CLight::m_SaveData[] = 
{
	DEFINE_FIELD( CLight, m_iStyle, FIELD_INTEGER ),
	DEFINE_FIELD( CLight, m_iszPattern, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CLight, CPointEntity );


//
// Cache user-entity-field values until spawn is called.
//
void CLight :: KeyValue( KeyValueData* pkvd)
{
	if (FStrEq(pkvd->szKeyName, "style"))
	{
		m_iStyle = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pitch"))
	{
		pev->angles.x = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "pattern"))
	{
		m_iszPattern = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "_light"))
	{
		int r, g, b, v, j;
		j = sscanf( pkvd->szValue, "%d %d %d %d\n", &r, &g, &b, &v );
		if (j == 1)
		{
			g = b = r;
		}
		m_color[0] = r;
		m_color[1] = g;
		m_color[2] = b;
		m_iRadius = v;
		pkvd->fHandled = TRUE;
	}
	else
	{
		pkvd->fHandled = FALSE;
	}
}

/*QUAKED light (0 1 0) (-8 -8 -8) (8 8 8) LIGHT_START_OFF
Non-displayed light.
Default light value is 300
Default style is 0
If targeted, it will toggle between on or off.
*/

void CLight :: Spawn( void )
{
	if (FBitSet(pev->spawnflags, SF_LIGHT_ELIGHT))
	{
		SetThink( &CLight::ELightThink );
		pev->nextthink = gpGlobals->time + 1;
		if (FStringNull(pev->targetname))
			return;
	}

	if (FStringNull(pev->targetname))
	{       // inert light
		REMOVE_ENTITY(ENT(pev));
		return;
	}
	
	if (m_iStyle >= 32)
	{
//		CHANGE_METHOD(ENT(pev), em_use, light_use);
		if (FBitSet(pev->spawnflags, SF_LIGHT_START_OFF))
			LIGHT_STYLE(m_iStyle, "a");
		else if (m_iszPattern)
			LIGHT_STYLE(m_iStyle, (char *)STRING( m_iszPattern ));
		else
			LIGHT_STYLE(m_iStyle, "m");
	}
}


void CLight :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (m_iStyle >= 32)
	{
		if ( !ShouldToggle( useType, !FBitSet(pev->spawnflags, SF_LIGHT_START_OFF) ) )
			return;

		if (FBitSet(pev->spawnflags, SF_LIGHT_START_OFF))
		{
			if (m_iszPattern)
				LIGHT_STYLE(m_iStyle, (char *)STRING( m_iszPattern ));
			else
				LIGHT_STYLE(m_iStyle, "m");
			ClearBits(pev->spawnflags, SF_LIGHT_START_OFF);
		}
		else
		{
			LIGHT_STYLE(m_iStyle, "a");
			SetBits(pev->spawnflags, SF_LIGHT_START_OFF);
		}
	}
}

void CLight :: SendELight( void )
{
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_ELIGHT );
		WRITE_SHORT( 0 );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		WRITE_COORD( m_iRadius );
		WRITE_BYTE( m_color[0] );
		WRITE_BYTE( m_color[1] );
		WRITE_BYTE( m_color[2] );
		WRITE_BYTE( 0 );
		WRITE_COORD( 0 );
	MESSAGE_END();
}

void CLight :: ELightThink( void )
{
	SendELight();
	if (FStringNull(pev->targetname))
		REMOVE_ENTITY(ENT(pev));
}

//
// shut up spawn functions for new spotlights
//
LINK_ENTITY_TO_CLASS( light_spot, CLight );
LINK_ENTITY_TO_CLASS( entity_light, CLight );


class CEnvLight : public CLight
{
public:
	void	KeyValue( KeyValueData* pkvd ); 
	void	Spawn( void );
};

LINK_ENTITY_TO_CLASS( light_environment, CEnvLight );

void CEnvLight::KeyValue( KeyValueData* pkvd )
{
	if (FStrEq(pkvd->szKeyName, "_light"))
	{
		int r, g, b, v, j;
		char szColor[64];
		j = sscanf( pkvd->szValue, "%d %d %d %d\n", &r, &g, &b, &v );
		if (j == 1)
		{
			g = b = r;
		}
		else if (j == 4)
		{
			r = r * (v / 255.0f);
			g = g * (v / 255.0f);
			b = b * (v / 255.0f);
		}

		// simulate qrad direct, ambient,and gamma adjustments, as well as engine scaling
		r = pow( r / 114.0f, 0.6f ) * 264.0f;
		g = pow( g / 114.0f, 0.6f ) * 264.0f;
		b = pow( b / 114.0f, 0.6f ) * 264.0f;

		pkvd->fHandled = TRUE;
		sprintf( szColor, "%d", r );
		CVAR_SET_STRING( "cl_skycolor_r", szColor );
		sprintf( szColor, "%d", g );
		CVAR_SET_STRING( "cl_skycolor_g", szColor );
		sprintf( szColor, "%d", b );
		CVAR_SET_STRING( "cl_skycolor_b", szColor );
	}
	else
	{
		CLight::KeyValue( pkvd );
	}
}


void CEnvLight :: Spawn( void )
{
	char szVector[64];
	UTIL_MakeAimVectors( pev->angles );

	sprintf( szVector, "%f", gpGlobals->v_forward.x );
	CVAR_SET_STRING( "cl_skyvec_x", szVector );
	sprintf( szVector, "%f", gpGlobals->v_forward.y );
	CVAR_SET_STRING( "cl_skyvec_y", szVector );
	sprintf( szVector, "%f", gpGlobals->v_forward.z );
	CVAR_SET_STRING( "cl_skyvec_z", szVector );

	CLight::Spawn( );
}


/* registro dos ponteiros de Think para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_0019B0D0(void)
{
	func_002397D0("170", static_cast<BASEPTR>(&CLight::ELightThink));
}
