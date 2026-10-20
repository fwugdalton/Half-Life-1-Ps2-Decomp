#ifndef DLLS_SOUNDENT_H
#define DLLS_SOUNDENT_H

/* soundent.h do HLSDK. CSoundEnt: m_iFreeSound em 0x30, m_SoundPool em 0x40. */

#define	MAX_WORLD_SOUNDS	64 // maximum number of sounds handled by the world at one time.

#define bits_SOUND_NONE     0
#define bits_SOUND_COMBAT   (1 << 0)
#define bits_SOUND_WORLD    (1 << 1)
#define bits_SOUND_PLAYER   (1 << 2)
#define bits_SOUND_CARCASS  (1 << 3)
#define bits_SOUND_MEAT     (1 << 4)
#define bits_SOUND_DANGER   (1 << 5)
#define bits_SOUND_GARBAGE  (1 << 6)

#define bits_ALL_SOUNDS     0xFFFFFFFF

#define SOUNDLIST_EMPTY	-1

#define SOUNDLISTTYPE_FREE	1// identifiers passed to functions that can operate on either list, to indicate which list to operate on.
#define SOUNDLISTTYPE_ACTIVE 2

#define	SOUND_NEVER_EXPIRE	-1 // with this set as a sound's ExpireTime, the sound will never expire.

class CSound
{
public:
    void Clear(void);
    void Reset(void);

    Vector m_vecOrigin;         /* 0x00 */
    int m_iType;                /* 0x0C */
    int m_iVolume;              /* 0x10 */
    float m_flExpireTime;       /* 0x14 */
    int m_iNext;                /* 0x18 */
    int m_iNextAudible;         /* 0x1C */

    BOOL FIsSound(void);
    BOOL FIsScent(void);
};

class CSoundEnt : public CBaseEntity
{
public:

	void Precache ( void );
	void Spawn( void );
	void Think( void );
	void Initialize ( void );
	
	static void		InsertSound ( int iType, const Vector &vecOrigin, int iVolume, float flDuration );
	static void		FreeSound ( int iSound, int iPrevious );
	static int		ActiveList( void );// return the head of the active list
	static int		FreeList( void );// return the head of the free list
	static CSound*	SoundPointerForIndex( int iIndex );// return a pointer for this index in the sound list
	static int		ClientSoundIndex ( edict_t *pClient );

	BOOL	IsEmpty( void ) { return m_iActiveSound == SOUNDLIST_EMPTY; }
	int		ISoundsInList ( int iListType );
	int		IAllocSound ( void );
	virtual int		ObjectCaps( void ) { return FCAP_DONT_SAVE; }
	
	int		m_iFreeSound;	// index of the first sound in the free sound list
	int		m_iActiveSound; // indes of the first sound in the active sound list
	int		m_cLastActiveSounds; // keeps track of the number of active sounds at the last update. (for diagnostic work)
	BOOL	m_fShowReport; // if true, dump information about free/active sounds.

private:
	CSound		m_SoundPool[ MAX_WORLD_SOUNDS ];
};

#endif
