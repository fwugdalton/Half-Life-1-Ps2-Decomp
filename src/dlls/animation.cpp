/* animation.cpp: funções de animação de modelos studio (.dol no PS2). */
typedef struct _FILE FILE;
#include "dlls/extdll.h"
#define PRECACHE_SOUND      (*g_engfuncs.pfnPrecacheSound)
#define RANDOM_LONG         (*g_engfuncs.pfnRandomLong)
#define ALERT               (*g_engfuncs.pfnAlertMessage)
#define ALLOC_STRING        (*g_engfuncs.pfnAllocString)
#define stricmp strcasecmp
extern "C" int strcasecmp( const char *, const char * );
typedef struct cache_user_s { void *data; } cache_user_t;
#include "engine/studio.h"
#define VectorCopy(a, b) {(b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2];}
/* extensão do PS2 no fim do studiohdr_t (níveis de detalhe por bodygroup) */
typedef struct
{
	int unk00;
	int lod[4];
} mstudiounkF4_t;
#define STUDIOHDR_UNKF4(h) (*(int *)((byte *)(h) + 0xF4))
#define STUDIOHDR_UNKF8(h) (*((byte *)(h) + 0xF8))
#define STUDIOHDR_UNKF9(h) (*((byte *)(h) + 0xF9))
#include "dlls/activity.h"
typedef struct { int type; char *name; } activity_map_t;
#include "dlls/activitymap.h"
#include "dlls/animation.h"
#include "dlls/scriptevent.h"
#include "include_asm.h"

extern globalvars_t *gpGlobals;
extern "C" float sqrtf( float );
extern "C" float fabsf( float );

int ExtractBbox( void *pmodel, int sequence, float *mins, float *maxs )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);
	
	mins[0] = pseqdesc[ sequence ].bbmin[0];
	mins[1] = pseqdesc[ sequence ].bbmin[1];
	mins[2] = pseqdesc[ sequence ].bbmin[2];

	maxs[0] = pseqdesc[ sequence ].bbmax[0];
	maxs[1] = pseqdesc[ sequence ].bbmax[1];
	maxs[2] = pseqdesc[ sequence ].bbmax[2];

	return 1;
}


int LookupActivity( void *pmodel, entvars_t *pev, int activity )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	int weighttotal = 0;
	int seq = ACTIVITY_NOT_AVAILABLE;
	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (pseqdesc[i].activity == activity)
		{
			weighttotal += pseqdesc[i].actweight;
			if (!weighttotal || RANDOM_LONG(0,weighttotal-1) < pseqdesc[i].actweight)
				seq = i;
		}
	}

	return seq;
}


int LookupActivityHeaviest( void *pmodel, entvars_t *pev, int activity )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr )
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	int weight = 0;
	int seq = ACTIVITY_NOT_AVAILABLE;
	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (pseqdesc[i].activity == activity)
		{
			if ( pseqdesc[i].actweight > weight )
			{
				weight = pseqdesc[i].actweight;
				seq = i;
			}
		}
	}

	return seq;
}

void GetEyePosition ( void *pmodel, float *vecEyePosition )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;

	if ( !pstudiohdr )
	{
		ALERT ( at_console, "GetEyePosition() Can't get pstudiohdr ptr!\n" );
		return;
	}

	VectorCopy ( pstudiohdr->eyeposition, vecEyePosition );
}

int LookupSequence( void *pmodel, const char *label )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	for (int i = 0; i < pstudiohdr->numseq; i++)
	{
		if (stricmp( pseqdesc[i].label, label ) == 0)
			return i;
	}

	return -1;
}


int IsSoundEvent( int eventNumber )
{
	if ( eventNumber == SCRIPT_EVENT_SOUND || eventNumber == SCRIPT_EVENT_SOUND_VOICE )
		return 1;
	return 0;
}


void SequencePrecache( void *pmodel, const char *pSequenceName )
{
	int index = LookupSequence( pmodel, pSequenceName );
	if ( index >= 0 )
	{
		studiohdr_t *pstudiohdr;
	
		pstudiohdr = (studiohdr_t *)pmodel;
		if ( !pstudiohdr || index >= pstudiohdr->numseq )
			return;

		mstudioseqdesc_t	*pseqdesc;
		mstudioevent_t		*pevent;

		pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + index;
		pevent = (mstudioevent_t *)((byte *)pstudiohdr + pseqdesc->eventindex);

		for (int i = 0; i < pseqdesc->numevents; i++)
		{
			// Don't send client-side events to the server AI
			if ( pevent[i].event >= EVENT_CLIENT )
				continue;

			// UNDONE: Add a callback to check to see if a sound is precached yet and don't allocate a copy
			// of it's name if it is.
			if ( IsSoundEvent( pevent[i].event ) )
			{
				if ( !strlen(pevent[i].options) )
				{
					ALERT( at_error, "Bad sound event %d in sequence %s :: %s (sound is \"%s\")\n", pevent[i].event, pstudiohdr->name, pSequenceName, pevent[i].options );
				}

				PRECACHE_SOUND( (char *)(gpGlobals->pStringBase + ALLOC_STRING(pevent[i].options) ) );
			}
		}
	}
}



void GetSequenceInfo( void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return;

	mstudioseqdesc_t	*pseqdesc;

	if (pev->sequence >= pstudiohdr->numseq)
	{
		*pflFrameRate = 0.0f;
		*pflGroundSpeed = 0.0f;
		return;
	}

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = sqrtf( pseqdesc->linearmovement[0]*pseqdesc->linearmovement[0]+ pseqdesc->linearmovement[1]*pseqdesc->linearmovement[1]+ pseqdesc->linearmovement[2]*pseqdesc->linearmovement[2] );
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0f;
		*pflGroundSpeed = 0.0f;
	}
}


int GetSequenceFlags( void *pmodel, entvars_t *pev )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr || pev->sequence >= pstudiohdr->numseq )
		return 0;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	return pseqdesc->flags;
}


int GetAnimationEvent( void *pmodel, entvars_t *pev, MonsterEvent_t *pMonsterEvent, float flStart, float flEnd, int index )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if ( !pstudiohdr || pev->sequence >= pstudiohdr->numseq || !pMonsterEvent )
		return 0;

	int events = 0;

	mstudioseqdesc_t	*pseqdesc;
	mstudioevent_t		*pevent;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;
	pevent = (mstudioevent_t *)((byte *)pstudiohdr + pseqdesc->eventindex);

	if (pseqdesc->numevents == 0 || index > pseqdesc->numevents )
		return 0;

	if (pseqdesc->numframes > 1)
	{
		flStart *= (pseqdesc->numframes - 1) / 256.0f;
		flEnd *= (pseqdesc->numframes - 1) / 256.0f;
	}
	else
	{
		flStart = 0;
		flEnd = 1.0f;
	}

	for (; index < pseqdesc->numevents; index++)
	{
		// Don't send client-side events to the server AI
		if ( pevent[index].event >= EVENT_CLIENT )
			continue;

		if ( (pevent[index].frame >= flStart && pevent[index].frame < flEnd) || 
			((pseqdesc->flags & STUDIO_LOOPING) && flEnd >= pseqdesc->numframes - 1 && pevent[index].frame < flEnd - pseqdesc->numframes + 1) )
		{
			pMonsterEvent->event = pevent[index].event;
			pMonsterEvent->options = pevent[index].options;
			return index + 1;
		}
	}
	return 0;
}

#ifdef NON_MATCHING
/* NON_MATCHING: agendamento depois do cvt.w.s do setting (o alvo carrega -1, 255 e 1/255 depois da divisão) */
float SetController( void *pmodel, entvars_t *pev, int iController, float flValue )
{
	studiohdr_t *pstudiohdr;
	int i;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return flValue;

	mstudiobonecontroller_t	*pbonecontroller = (mstudiobonecontroller_t *)((byte *)pstudiohdr + pstudiohdr->bonecontrollerindex);

	// find first controller that matches the index
	for ( i = 0; i < pstudiohdr->numbonecontrollers; i++, pbonecontroller++)
	{
		if (pbonecontroller->index == iController)
			break;
	}
	if (i >= pstudiohdr->numbonecontrollers)
		return flValue;

	// wrap 0..360 if it's a rotational controller

	if (pbonecontroller->type & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pbonecontroller->end < pbonecontroller->start)
			flValue = -flValue;

		// does the controller not wrap?
		if (pbonecontroller->start + 359.0f >= pbonecontroller->end)
		{
			if (flValue > ((pbonecontroller->start + pbonecontroller->end) / 2.0f) + 180)
				flValue = flValue - 360;
			if (flValue < ((pbonecontroller->start + pbonecontroller->end) / 2.0f) - 180)
				flValue = flValue + 360;
		}
		else
		{
			if (flValue > 360)
				flValue = flValue - (int)(flValue / 360.0f) * 360.0f;
			else if (flValue < 0)
				flValue = flValue + (int)((flValue / -360.0f) + 1) * 360.0f;
		}
	}

	int setting = 255 * (flValue - pbonecontroller->start) / (pbonecontroller->end - pbonecontroller->start);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;
	pev->controller[iController] = setting;

	return setting * (1.0f / 255.0f) * (pbonecontroller->end - pbonecontroller->start) + pbonecontroller->start;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/animation", SetController__FPvP9entvars_sif);
#endif


#ifdef NON_MATCHING
/* NON_MATCHING: agendamento depois do cvt.w.s do setting (o alvo carrega -1, 255 e 1/255 depois da divisão) */
float SetBlending( void *pmodel, entvars_t *pev, int iBlender, float flValue )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return flValue;

	mstudioseqdesc_t	*pseqdesc;

	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->blendtype[iBlender] == 0)
		return flValue;

	if (pseqdesc->blendtype[iBlender] & (STUDIO_XR | STUDIO_YR | STUDIO_ZR))
	{
		// ugly hack, invert value if end < start
		if (pseqdesc->blendend[iBlender] < pseqdesc->blendstart[iBlender])
			flValue = -flValue;

		// does the controller not wrap?
		if (pseqdesc->blendstart[iBlender] + 359.0f >= pseqdesc->blendend[iBlender])
		{
			if (flValue > ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0f) + 180)
				flValue = flValue - 360;
			if (flValue < ((pseqdesc->blendstart[iBlender] + pseqdesc->blendend[iBlender]) / 2.0f) - 180)
				flValue = flValue + 360;
		}
	}

	int setting = 255 * (flValue - pseqdesc->blendstart[iBlender]) / (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]);

	if (setting < 0) setting = 0;
	if (setting > 255) setting = 255;

	pev->blending[iBlender] = setting;

	return setting * (1.0f / 255.0f) * (pseqdesc->blendend[iBlender] - pseqdesc->blendstart[iBlender]) + pseqdesc->blendstart[iBlender];
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/animation", SetBlending__FPvP9entvars_sif);
#endif




int FindTransition( void *pmodel, int iEndingAnim, int iGoalAnim, int *piDir )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return iGoalAnim;

	mstudioseqdesc_t	*pseqdesc;
	pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex);

	// bail if we're going to or from a node 0
	if (pseqdesc[iEndingAnim].entrynode == 0 || pseqdesc[iGoalAnim].entrynode == 0)
	{
		return iGoalAnim;
	}

	int	iEndNode;

	// ALERT( at_console, "from %d to %d: ", pEndNode->iEndNode, pGoalNode->iStartNode );

	if (*piDir > 0)
	{
		iEndNode = pseqdesc[iEndingAnim].exitnode;
	}
	else
	{
		iEndNode = pseqdesc[iEndingAnim].entrynode;
	}

	if (iEndNode == pseqdesc[iGoalAnim].entrynode)
	{
		*piDir = 1;
		return iGoalAnim;
	}

	byte *pTransition = ((byte *)pstudiohdr + pstudiohdr->transitionindex);

	int iInternNode = pTransition[(iEndNode-1)*pstudiohdr->numtransitions + (pseqdesc[iGoalAnim].entrynode-1)];

	if (iInternNode == 0)
		return iGoalAnim;

	int i;

	// look for someone going
	for (i = 0; i < pstudiohdr->numseq; i++)
	{
		if (pseqdesc[i].entrynode == iEndNode && pseqdesc[i].exitnode == iInternNode)
		{
			*piDir = 1;
			return i;
		}
		if (pseqdesc[i].nodeflags)
		{
			if (pseqdesc[i].exitnode == iEndNode && pseqdesc[i].entrynode == iInternNode)
			{
				*piDir = -1;
				return i;
			}
		}
	}

	ALERT( at_console, "error in transition graph" );
	return iGoalAnim;
}

int GetMaxAttachment( void *pmodel )
{
	studiohdr_t *pstudiohdr;

	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return -1;

	if (! pstudiohdr->numattachments)
		return -1;

	return pstudiohdr->numattachments - 1;
}

void SetBodygroup( void *pmodel, entvars_t *pev, int iGroup, int iValue )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return;

	if (iGroup > pstudiohdr->numbodyparts)
		return;

	if (STUDIOHDR_UNKF8(pstudiohdr))
	{
		mstudiounkF4_t *p = (mstudiounkF4_t *)((byte *)pstudiohdr + STUDIOHDR_UNKF4(pstudiohdr));
		int total = 0;
		int i;

		for (i = 0; i < STUDIOHDR_UNKF9(pstudiohdr); i++)
		{
			if (i == iValue)
				break;
			total += 1 + p[iGroup * STUDIOHDR_UNKF8(pstudiohdr) + i].unk00;
		}

		mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + iGroup;

		if (total >= pbodypart->nummodels)
			return;

		int iCurrent = (pev->body / pbodypart->base) % pbodypart->nummodels;

		pev->body = (pev->body - (iCurrent * pbodypart->base) + (total * pbodypart->base));
	}
	else
	{
		mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + iGroup;

		if (iValue >= pbodypart->nummodels)
			return;

		int iCurrent = (pev->body / pbodypart->base) % pbodypart->nummodels;

		pev->body = (pev->body - (iCurrent * pbodypart->base) + (iValue * pbodypart->base));
	}
}


int GetBodygroup( void *pmodel, entvars_t *pev, int iGroup )
{
	studiohdr_t *pstudiohdr;
	
	pstudiohdr = (studiohdr_t *)pmodel;
	if (! pstudiohdr)
		return 0;

	if (iGroup > pstudiohdr->numbodyparts)
		return 0;

	mstudiobodyparts_t *pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + iGroup;

	if (pbodypart->nummodels <= 1)
		return 0;

	int iCurrent = (pev->body / pbodypart->base) % pbodypart->nummodels;

	if (STUDIOHDR_UNKF8(pstudiohdr))
	{
		mstudiounkF4_t *p = (mstudiounkF4_t *)((byte *)pstudiohdr + STUDIOHDR_UNKF4(pstudiohdr));
		int total = 0;
		int i;

		for (i = 0; i < STUDIOHDR_UNKF9(pstudiohdr); i++)
		{
			total += 1 + p[iGroup * STUDIOHDR_UNKF8(pstudiohdr) + i].unk00;
			if (iCurrent < total)
			{
				iCurrent = i;
				break;
			}
		}
	}

	return iCurrent;
}
