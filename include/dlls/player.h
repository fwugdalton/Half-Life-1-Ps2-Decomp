#ifndef DLLS_PLAYER_H
#define DLLS_PLAYER_H

/* player.h do HLSDK: só os campos de CBasePlayer que o código já usa
   (offsets do PS2; o resto é preenchimento até ser conhecido). */

#ifndef MONSTERS_H
#include "dlls/monsters.h"
#endif

#define PFLAG_ONLADDER      (1 << 0)
#define PFLAG_ONSWING       (1 << 0)
#define PFLAG_ONTRAIN       (1 << 1)
#define PFLAG_ONBARNACLE    (1 << 2)
#define PFLAG_DUCKING       (1 << 3)
#define PFLAG_USING         (1 << 4)
#define PFLAG_OBSERVER      (1 << 5)
#define PFLAG_PS2_NOPICKUP  (1 << 6)   /* PS2: não pega armas nem munição (fora do impulse 101) */

#define MAX_ITEM_TYPES      6
#define MAX_AMMO_SLOTS      32

#define WEAPON_IS_ONTARGET  0x40

class CBasePlayerItem;

class CBasePlayer : public CBaseMonster
{
public:
    byte m_ps2Unk2A0[0x468 - 0x2A0];
    BOOL m_fWeapon;                             /* 0x468 */
    byte m_ps2Unk46C[0x490 - 0x46C];
    int m_afPhysicsFlags;                       /* 0x490 */
    byte m_ps2Unk494[0x498 - 0x494];
    int m_iFOV;                                 /* 0x498 */
    int m_iClientFOV;                           /* 0x49C */
    byte m_ps2Unk4A0[0x4B8 - 0x4A0];
    CBasePlayerItem *m_pActiveItem;             /* 0x4B8 */
    CBasePlayerItem *m_pClientActiveItem;       /* 0x4BC */
    CBasePlayerItem *m_pLastItem;               /* 0x4C0 */
    int m_rgAmmo[MAX_AMMO_SLOTS];               /* 0x4C4 */
    byte m_ps2Unk544[0x5DC - 0x544];
    BOOL m_fOnTarget;                           /* 0x5DC */
    byte m_ps2Unk5E0[0x770 - 0x5E0];
    BOOL m_ps2InfiniteAmmo;                     /* 0x770: recarrega sem gastar munição */
    byte m_ps2Unk774[0x7BC - 0x774];
    char m_szAnimExtention[32];                 /* 0x7BC */

    static int GetAmmoIndex(const char *psz);
    BOOL HasPlayerItem(CBasePlayerItem *pCheckItem);
    void GiveNamedItem(const char *szName);
};

extern BOOL gEvilImpulse101;
extern int gmsgCurWeapon;

#endif
