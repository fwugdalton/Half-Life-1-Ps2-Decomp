/* animating.cpp: CBaseAnimating (animação de entidades com modelo studio). */
typedef struct _FILE FILE;
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/animation.h"
#include "dlls/saverestore.h"
#include "dlls/gamerules.h"
int GetMaxAttachment( void *pmodel );
Vector VecBModelOrigin( entvars_t *pevBModel );

// PS2: o jogador do single player não tem modelo carregado
#define NO_PLAYER_MODEL()	( IsPlayer() && !g_pGameRules->IsMultiplayer() && !g_pGameRules->Ps2Virt07() )
#include "include_asm.h"

extern "C" float fabsf(float);
extern "C" float sqrtf(float);
extern "C" float cosf(float);
extern "C" float sinf(float);
#define M_PI			3.14159265358979323846f

TYPEDESCRIPTION	CBaseAnimating::m_SaveData[] = 
{
	DEFINE_FIELD( CBaseMonster, m_flFrameRate, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flGroundSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CBaseMonster, m_flLastEventCheck, FIELD_TIME ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceFinished, FIELD_BOOLEAN ),
	DEFINE_FIELD( CBaseMonster, m_fSequenceLoops, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE( CBaseAnimating, CBaseDelay );


//=========================================================
// StudioFrameAdvance - advance the animation frame up to the current time
// if an flInterval is passed in, only advance animation that number of seconds
//=========================================================
float CBaseAnimating :: StudioFrameAdvance ( float flInterval )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	if (flInterval == 0.0f)
	{
		flInterval = (gpGlobals->time - pev->animtime);
		if (flInterval <= 0.001f)
		{
			pev->animtime = gpGlobals->time;
			return 0.0f;
		}
	}
	if (! pev->animtime)
		flInterval = 0.0f;
	
	pev->frame += flInterval * m_flFrameRate * pev->framerate;
	pev->animtime = gpGlobals->time;

	if (pev->frame < 0.0f || pev->frame >= 256.0f) 
	{
		if (m_fSequenceLoops)
			pev->frame -= (int)(pev->frame / 256.0f) * 256.0f;
		else
			pev->frame = (pev->frame < 0.0f) ? 0 : 255;
		m_fSequenceFinished = TRUE;	// just in case it wasn't caught in GetEvents
	}

	return flInterval;
}

//=========================================================
// LookupActivity
//=========================================================
int CBaseAnimating :: LookupActivity ( int activity )
{
	if ( NO_PLAYER_MODEL() )
		return 1;

	ASSERT( activity != 0 );
	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivity( pmodel, pev, activity );
}

//=========================================================
// LookupActivityHeaviest
//
// Get activity with highest 'weight'
//
//=========================================================
int CBaseAnimating :: LookupActivityHeaviest ( int activity )
{
	if ( NO_PLAYER_MODEL() )
		return 1;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupActivityHeaviest( pmodel, pev, activity );
}

//=========================================================
//=========================================================
int CBaseAnimating :: LookupSequence ( const char *label )
{
	if ( NO_PLAYER_MODEL() )
		return 1;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::LookupSequence( pmodel, label );
}


//=========================================================
//=========================================================
void CBaseAnimating :: ResetSequenceInfo ( )
{
	if ( NO_PLAYER_MODEL() )
		return;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	GetSequenceInfo( pmodel, pev, &m_flFrameRate, &m_flGroundSpeed );
	m_fSequenceLoops = ((GetSequenceFlags() & STUDIO_LOOPING) != 0);
	pev->animtime = gpGlobals->time;
	pev->framerate = 1.0f;
	m_fSequenceFinished = FALSE;
	m_flLastEventCheck = gpGlobals->time;
}



//=========================================================
//=========================================================
BOOL CBaseAnimating :: GetSequenceFlags( )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::GetSequenceFlags( pmodel, pev );
}

//=========================================================
// DispatchAnimEvents
//=========================================================
void CBaseAnimating :: DispatchAnimEvents ( float flInterval )
{
	if ( NO_PLAYER_MODEL() )
		return;

	MonsterEvent_t	event;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	if ( !pmodel )
	{
		ALERT( at_aiconsole, "Gibbed monster is thinking!\n" );
		return;
	}

	// FIXME: I have to do this or some events get missed, and this is probably causing the problem below
	flInterval = 0.1f;

	// FIX: this still sometimes hits events twice
	float flStart = pev->frame + (m_flLastEventCheck - pev->animtime) * m_flFrameRate * pev->framerate;
	float flEnd = pev->frame + flInterval * m_flFrameRate * pev->framerate;
	m_flLastEventCheck = pev->animtime + flInterval;

	m_fSequenceFinished = FALSE;
	if (flEnd >= 256 || flEnd <= 0.0f) 
		m_fSequenceFinished = TRUE;

	int index = 0;

	while ( (index = GetAnimationEvent( pmodel, pev, &event, flStart, flEnd, index ) ) != 0 )
	{
		HandleAnimEvent( &event );
	}
}


//=========================================================
//=========================================================
float CBaseAnimating :: SetBoneController ( int iController, float flValue )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return SetController( pmodel, pev, iController, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: InitBoneControllers ( void )
{
	if ( NO_PLAYER_MODEL() )
		return;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	SetController( pmodel, pev, 0, 0.0f );
	SetController( pmodel, pev, 1, 0.0f );
	SetController( pmodel, pev, 2, 0.0f );
	SetController( pmodel, pev, 3, 0.0f );
}

//=========================================================
//=========================================================
float CBaseAnimating :: SetBlending ( int iBlender, float flValue )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );

	return ::SetBlending( pmodel, pev, iBlender, flValue );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetBonePosition ( int iBone, Vector &origin, Vector &angles )
{
	if ( NO_PLAYER_MODEL() )
		return;

	GET_BONE_POSITION( ENT(pev), iBone, origin, angles );
}

//=========================================================
//=========================================================
// PS2: posição do último attachment do modelo (o jogador só tem no multiplayer)
int CBaseAnimating :: GetLastAttachment( Vector &origin )
{
	Vector angles;

	if ( NO_PLAYER_MODEL() )
		return 0;

	if ( STRING( pev->model ) && (STRING( pev->model ))[0] == '*' )
	{
		origin = VecBModelOrigin( pev );
		return 0;
	}

	int iAttachment = GetMaxAttachment( GET_MODEL_PTR( ENT(pev) ) );

	if ( iAttachment >= 0 )
		GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
	else
		GET_ATTACHMENT( ENT(pev), 0, origin, angles );

	return iAttachment;
}

int CBaseAnimating :: GetLastAttachmentIndex( void )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	if ( STRING( pev->model ) && (STRING( pev->model ))[0] == '*' )
		return 0;

	return GetMaxAttachment( GET_MODEL_PTR( ENT(pev) ) );
}

void CBaseAnimating :: GetAttachment ( int iAttachment, Vector &origin, Vector &angles )
{
	if ( NO_PLAYER_MODEL() )
		return;

	GET_ATTACHMENT( ENT(pev), iAttachment, origin, angles );
}

//=========================================================
//=========================================================
int CBaseAnimating :: FindTransition( int iEndingSequence, int iGoalSequence, int *piDir )
{
	if ( NO_PLAYER_MODEL() )
		return 1;

	void *pmodel = GET_MODEL_PTR( ENT(pev) );
	
	if (piDir == NULL)
	{
		int iDir;
		int sequence = ::FindTransition( pmodel, iEndingSequence, iGoalSequence, &iDir );
		if (iDir != 1)
			return -1;
		else
			return sequence;
	}

	return ::FindTransition( pmodel, iEndingSequence, iGoalSequence, piDir );
}

//=========================================================
//=========================================================
void CBaseAnimating :: GetAutomovement( Vector &origin, Vector &angles, float flInterval )
{

}

void CBaseAnimating :: SetBodygroup( int iGroup, int iValue )
{
	if ( NO_PLAYER_MODEL() )
		return;

	::SetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup, iValue );
}

int CBaseAnimating :: GetBodygroup( int iGroup )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	return ::GetBodygroup( GET_MODEL_PTR( ENT(pev) ), pev, iGroup );
}


int CBaseAnimating :: ExtractBbox( int sequence, float *mins, float *maxs )
{
	if ( NO_PLAYER_MODEL() )
		return 0;

	return ::ExtractBbox( GET_MODEL_PTR( ENT(pev) ), sequence, mins, maxs );
}

//=========================================================
//=========================================================

#ifdef NON_MATCHING
/* NON_MATCHING: no alvo o laço relê xvector/yvector e base da pilha a cada volta; o nosso guarda em registradores */
void CBaseAnimating :: SetSequenceBox( void )
{
	if ( NO_PLAYER_MODEL() )
		return;

	Vector mins, maxs;

	// Get sequence bbox
	if ( ExtractBbox( pev->sequence, mins, maxs ) )
	{
		// expand box for rotation
		// find min / max for rotations
		float yaw = pev->angles.y * (M_PI / 180.0f);
		
		Vector xvector, yvector;
		xvector.x = cosf(yaw);
		xvector.y = sinf(yaw);
		yvector.x = -sinf(yaw);
		yvector.y = cosf(yaw);
		Vector bounds[2];

		bounds[0] = mins;
		bounds[1] = maxs;
		
		Vector rmin( 9999, 9999, 9999 );
		Vector rmax( -9999, -9999, -9999 );
		Vector base, transformed;

		for (int i = 0; i <= 1; i++ )
		{
			base.x = bounds[i].x;
			for ( int j = 0; j <= 1; j++ )
			{
				base.y = bounds[j].y;
				for ( int k = 0; k <= 1; k++ )
				{
					base.z = bounds[k].z;
					
				// transform the point
					transformed.x = xvector.x*base.x + yvector.x*base.y;
					transformed.y = xvector.y*base.x + yvector.y*base.y;
					transformed.z = base.z;
					
					if (transformed.x < rmin.x)
						rmin.x = transformed.x;
					if (transformed.x > rmax.x)
						rmax.x = transformed.x;
					if (transformed.y < rmin.y)
						rmin.y = transformed.y;
					if (transformed.y > rmax.y)
						rmax.y = transformed.y;
					if (transformed.z < rmin.z)
						rmin.z = transformed.z;
					if (transformed.z > rmax.z)
						rmax.z = transformed.z;
				}
			}
		}
		rmin.z = 0;
		rmax.z = rmin.z + 1;
		UTIL_SetSize( pev, rmin, rmax );
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/animating", SetSequenceBox__14CBaseAnimating);
#endif

