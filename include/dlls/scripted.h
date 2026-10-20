#ifndef DLLS_SCRIPTED_H
#define DLLS_SCRIPTED_H

/* scripted.h do HLSDK. No PS2 o SequenceDone é virtual (slot 131). */

#define SF_SCRIPT_WAITTILLSEEN		1
#define SF_SCRIPT_EXITAGITATED		2
#define SF_SCRIPT_REPEATABLE		4
#define SF_SCRIPT_LEAVECORPSE		8
//#define SF_SCRIPT_INTERPOLATE		16 // don't use, old bug
#define SF_SCRIPT_NOINTERRUPT		32
#define SF_SCRIPT_OVERRIDESTATE		64
#define SF_SCRIPT_NOSCRIPTMOVEMENT	128

#define SCRIPT_BREAK_CONDITIONS		(bits_COND_LIGHT_DAMAGE|bits_COND_HEAVY_DAMAGE)

enum SS_INTERRUPT
{
	SS_INTERRUPT_IDLE = 0,
	SS_INTERRUPT_BY_NAME,
	SS_INTERRUPT_AI,
};

// when a monster finishes an AI scripted sequence, we can choose
// a schedule to place them in. These defines are the aliases to
// resolve worldcraft input to real schedules (sjb)
#define SCRIPT_FINISHSCHED_DEFAULT	0
#define SCRIPT_FINISHSCHED_AMBUSH	1

class CCineMonster : public CBaseMonster
{
public:
	void Spawn( void );
	virtual void KeyValue( KeyValueData *pkvd );
	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void Blocked( CBaseEntity *pOther );
	virtual void Touch( CBaseEntity *pOther );
	virtual int	 ObjectCaps( void ) { return (CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION); }
	virtual void Activate( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	
	static	TYPEDESCRIPTION m_SaveData[];

	// void EXPORT CineSpawnThink( void );
	void EXPORT CineThink( void );
	void Pain( void );
	void Die( void );
	void DelayStart( int state );
	BOOL FindEntity( void );
	virtual void PossessEntity( void );

	void ReleaseEntity( CBaseMonster *pEntity );
	void CancelScript( void );
	virtual BOOL StartSequence( CBaseMonster *pTarget, int iszSeq, BOOL completeOnEmpty );
	virtual BOOL FCanOverrideState ( void );
	virtual void SequenceDone ( CBaseMonster *pMonster );
	virtual void FixScriptMonsterSchedule( CBaseMonster *pMonster );
	BOOL	CanInterrupt( void );
	void	AllowInterrupt( BOOL fAllow );
	int		IgnoreConditions( void );

	int	m_iszIdle;		// string index for idle animation          /* 0x2A0 */
	int	m_iszPlay;		// string index for scripted animation      /* 0x2A4 */
	int m_iszEntity;	// entity that is wanted for this script        /* 0x2A8 */
	int m_fMoveTo;                                                      /* 0x2AC */
	int m_iFinishSchedule;                                              /* 0x2B0 */
	float m_flRadius;		// range to search                          /* 0x2B4 */
	float m_flRepeat;	// repeat rate                                  /* 0x2B8 */

	int m_iDelay;                                                       /* 0x2BC */
	float m_startTime;                                                  /* 0x2C0 */

	int	m_saved_movetype;                                               /* 0x2C4 */
	int	m_saved_solid;                                                  /* 0x2C8 */
	int m_saved_effects;                                                /* 0x2CC */
//	Vector m_vecOrigOrigin;
	BOOL m_interruptable;                                               /* 0x2D0 */
};

class CCineAI : public CCineMonster
{
	BOOL StartSequence( CBaseMonster *pTarget, int iszSeq, BOOL completeOnEmpty );
	void PossessEntity( void );
	BOOL FCanOverrideState ( void );
	virtual void FixScriptMonsterSchedule( CBaseMonster *pMonster );
};

#endif
