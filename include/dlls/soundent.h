#ifndef DLLS_SOUNDENT_H
#define DLLS_SOUNDENT_H

/* soundent.h do HLSDK (parcial) */

#define bits_SOUND_NONE     0
#define bits_SOUND_COMBAT   (1 << 0)
#define bits_SOUND_WORLD    (1 << 1)
#define bits_SOUND_PLAYER   (1 << 2)
#define bits_SOUND_CARCASS  (1 << 3)
#define bits_SOUND_MEAT     (1 << 4)
#define bits_SOUND_DANGER   (1 << 5)
#define bits_SOUND_GARBAGE  (1 << 6)

#define bits_ALL_SOUNDS     0xFFFFFFFF

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

/* só as estáticas usadas pelas outras unidades */
class CSoundEnt : public CBaseEntity
{
public:
    static void InsertSound(int iType, const Vector &vecOrigin, int iVolume, float flDuration);
};

#endif
