/* cl_dll/ammohistory.h (+ o WEAPON do ammo.h) do HLSDK, versão do PS2: gWR[2] (um por
   jogador local, 0x2F14 bytes, em 0x302A60), WEAPON de 0x170 bytes, histórico com o
   clientId de cada item e 10 itens. */

#ifndef AMMOHISTORY_H
#define AMMOHISTORY_H

extern "C" void *memset( void *dst, int c, unsigned int n );

#define MAX_WEAPON_NAME		128
#define MAX_WEAPONS			32
#define MAX_WEAPON_SLOTS	5
#define MAX_AMMO_TYPES		32

typedef int AMMO;

typedef struct
{
	char	szName[MAX_WEAPON_NAME];	/* 0x0 */
	int		iAmmoType;			/* 0x80 */
	int		iAmmo2Type;
	int		iMax1;
	int		iMax2;
	int		iSlot;				/* 0x90 */
	int		iSlotPos;
	int		iFlags;
	int		iId;
	int		iClip;				/* 0xA0 */
	int		iCount;

	HSPRITE hActive;			/* 0xA8 */
	wrect_t rcActive;
	HSPRITE hInactive;			/* 0xBC */
	wrect_t rcInactive;			/* 0xC0 */
	HSPRITE	hAmmo;
	wrect_t rcAmmo;
	HSPRITE hAmmo2;
	wrect_t rcAmmo2;
	HSPRITE hCrosshair;
	wrect_t rcCrosshair;
	HSPRITE hAutoaim;
	wrect_t rcAutoaim;
	HSPRITE hZoomedCrosshair;
	wrect_t rcZoomedCrosshair;
	HSPRITE hZoomedAutoaim;
	wrect_t rcZoomedAutoaim;	/* 0x138 */
	byte	ps2Unk148[0x170 - 0x148];
} WEAPON;

#define MAX_WEAPON_POSITIONS		MAX_WEAPON_SLOTS

class WeaponsResource
{
private:
	// Information about weapons & ammo
	WEAPON		rgWeapons[MAX_WEAPONS];	// Weapons Array	/* 0x0 */

	// counts of weapons * ammo
	WEAPON*		rgSlots[MAX_WEAPON_SLOTS+1][MAX_WEAPON_POSITIONS+1];	/* 0x2E00 */
	int			riAmmo[MAX_AMMO_TYPES];							/* 0x2E90 */

public:
	void Init( void )
	{
		memset( rgWeapons, 0, sizeof rgWeapons );
		Reset();
	}

	void Reset( void )
	{
		iOldWeaponBits = 0;
		memset( rgSlots, 0, sizeof rgSlots );
		memset( riAmmo, 0, sizeof riAmmo );
	}

	int			iOldWeaponBits;		/* 0x2F10 */

	WEAPON *GetWeapon( int iId ) { return &rgWeapons[iId]; }

	int HasAmmo( WEAPON *p );

	HSPRITE* GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect );
};

extern WeaponsResource gWR[];

#define MAX_HISTORY 10
enum {
	HISTSLOT_EMPTY,
	HISTSLOT_AMMO,
	HISTSLOT_WEAP,
	HISTSLOT_ITEM,
};

class HistoryResource
{
private:
	struct HIST_ITEM {
		int type;
		float DisplayTime;  // the time at which this item should be removed from the history
		int iCount;
		int iId;
		int iClient;		/* 0x10: PS2, jogador local dono do item */
	};

	HIST_ITEM rgAmmoHistory[MAX_HISTORY];

public:

	void Init( void )
	{
		Reset();
	}

	void Reset( void )
	{
		memset( rgAmmoHistory, 0, sizeof rgAmmoHistory );
	}

	int iHistoryGap;			/* 0xC8 */
	int iCurrentHistorySlot;	/* 0xCC */

	void AddToHistory( int clientId, int iType, int iId, int iCount = 0 );
	void AddToHistory( int clientId, int iType, const char *szName, int iCount = 0 );

	void CheckClearHistory( void );
	int DrawAmmoHistory( float flTime, int clientId );
};

extern HistoryResource gHR;

#endif
