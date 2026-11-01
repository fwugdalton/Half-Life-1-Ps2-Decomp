/* cl_dll/ammo.cpp: CHudAmmo e WeaponsResource do HLSDK, versão do PS2. Um gWR, um
   gpActiveSel e um gpLastSel por jogador local; sprites .spz e só 320/640 de resolução;
   miras novas (scrosshair, autoaimold); comandos nextslot/prevslot; mensagens TankUse e
   TankAmmo; barra de recarga (func_00113D08, chamada pelas dlls); o Mem_Free da lista
   de sprites. */

#include "cl_dll/hud.h"
#include "cl_dll/ammohistory.h"
#include "inline_include_asm.h"

extern "C" {
int sprintf( char *buf, const char *fmt, ... );
char *strcpy( char *dst, const char *src );
int strcmp( const char *a, const char *b );
void Mem_Free( void *ptr );
int func_00228EE0( char *dst, const char *fmt, ... );
void func_00113D08( float flTime, float flDelay, int clientId );
int atoi( const char *s );
}

#define TRUE	1
#define FALSE	0

int READ_CHAR( void );

#undef CVAR_GET_FLOAT
inline float CVAR_GET_FLOAT( const char *x ) { return gEngfuncs.pfnGetCvarFloat( (char*)x ); }
#define CVAR_CREATE( x, y, z ) gEngfuncs.pfnRegisterVariable( (char*)x, (char*)y, z )
#define SPR_GetList (*gEngfuncs.pfnSPR_GetList)
#define SetCrosshair (*gEngfuncs.pfnSetCrosshair)
#define ServerCmd (*gEngfuncs.pfnServerCmd)

#define FCVAR_ARCHIVE	1
#define IN_ATTACK		1
#define WEAPON_SUIT		31
#define WEAPON_FLAGS_SELECTONEMPTY	1
#define HIDEHUD_WEAPONS		1
#define HIDEHUD_ALL			4

#define MIN_ALPHA	100

extern "C" int cl_numlocal;
extern "C" int D_00399564;	/* tela 16:9 */

#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define abs(a)		(((a) < 0) ? -(a) : (a))

extern wrect_t D_003A23F8;	/* nullrc do MsgFunc_HideWeapon */
extern wrect_t D_003A2408;	/* nullrc do MsgFunc_CurWeapon */

client_sprite_t *GetSpriteList( client_sprite_t *pList, const char *psz, int iRes, int iCount );

WeaponsResource gWR[2];

WEAPON *gpActiveSel[2];	// NULL means off, 1 means just the menu bar, otherwise
						// this points to the active weapon menu item
WEAPON *gpLastSel[2];		// Last weapon menu selection

int giBucketHeight, giBucketWidth, giABHeight, giABWidth; // Ammo Bar width and height

HSPRITE ghsprBuckets;					// Sprite for top row of weapons menu

void WeaponsResource :: LoadAllWeaponSprites( void )
{
	for (int i = 0; i < MAX_WEAPONS; i++)
	{
		if ( rgWeapons[i].iId )
			LoadWeaponSprites( &rgWeapons[i] );
	}
}

int WeaponsResource :: CountAmmo( int iId )
{
	if ( iId < 0 )
		return 0;

	return riAmmo[iId];
}

int WeaponsResource :: HasAmmo( WEAPON *p )
{
	if ( !p )
		return FALSE;

	// weapons with no max ammo can always be selected
	if ( p->iMax1 == -1 )
		return TRUE;

	return (p->iAmmoType == -1) || p->iClip > 0 || CountAmmo(p->iAmmoType)
		|| CountAmmo(p->iAmmo2Type) || ( p->iFlags & WEAPON_FLAGS_SELECTONEMPTY );
}


void WeaponsResource :: LoadWeaponSprites( WEAPON *pWeapon )
{
	int i, iRes;
	int clientId = 0;

	if (ScreenWidth < 512)
		iRes = 320;
	else
		iRes = 640;

	char sz[128];

	if ( !pWeapon )
		return;

	memset( &pWeapon->rcActive, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcInactive, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcAmmo, 0, sizeof(wrect_t) );
	memset( &pWeapon->rcAmmo2, 0, sizeof(wrect_t) );
	pWeapon->hInactive = 0;
	pWeapon->hActive = 0;
	pWeapon->hAmmo = 0;
	pWeapon->hAmmo2 = 0;

	sprintf(sz, "sprites/%s.txt", pWeapon->szName);
	client_sprite_t *pList = SPR_GetList(sz, &i);

	if (!pList)
		return;

	client_sprite_t *p;

	p = GetSpriteList( pList, "crosshair", iRes, i );
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hCrosshair = SPR_Load(sz);
		pWeapon->rcCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList( pList, "scrosshair", iRes, i );
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hSCrosshair = SPR_Load(sz);
		pWeapon->rcSCrosshair = p->rc;
	}
	else
		pWeapon->hCrosshair = NULL;

	p = GetSpriteList(pList, "autoaimold", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAutoaimOld = SPR_Load(sz);
		pWeapon->rcAutoaimOld = p->rc;
	}
	else
		pWeapon->hAutoaimOld = 0;

	p = GetSpriteList(pList, "autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAutoaim = SPR_Load(sz);
		pWeapon->rcAutoaim = p->rc;
	}
	else
		pWeapon->hAutoaim = 0;

	p = GetSpriteList( pList, "zoom", iRes, i );
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hZoomedCrosshair = SPR_Load(sz);
		pWeapon->rcZoomedCrosshair = p->rc;
	}
	else
	{
		pWeapon->hZoomedCrosshair = pWeapon->hCrosshair; //default to non-zoomed crosshair
		pWeapon->rcZoomedCrosshair = pWeapon->rcCrosshair;
	}

	p = GetSpriteList(pList, "zoom_autoaim", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hZoomedAutoaim = SPR_Load(sz);
		pWeapon->rcZoomedAutoaim = p->rc;
	}
	else
	{
		pWeapon->hZoomedAutoaim = pWeapon->hZoomedCrosshair;  //default to zoomed crosshair
		pWeapon->rcZoomedAutoaim = pWeapon->rcZoomedCrosshair;
	}

	p = GetSpriteList(pList, "weapon", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hInactive = SPR_Load(sz);
		pWeapon->rcInactive = p->rc;

		gHR.iHistoryGap = max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hInactive = 0;

	p = GetSpriteList(pList, "weapon_s", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hActive = SPR_Load(sz);
		pWeapon->rcActive = p->rc;
	}
	else
		pWeapon->hActive = 0;

	p = GetSpriteList(pList, "ammo", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAmmo = SPR_Load(sz);
		pWeapon->rcAmmo = p->rc;

		gHR.iHistoryGap = max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hAmmo = 0;

	p = GetSpriteList(pList, "ammo2", iRes, i);
	if (p)
	{
		sprintf(sz, "sprites/%s.spz", p->szSprite);
		pWeapon->hAmmo2 = SPR_Load(sz);
		pWeapon->rcAmmo2 = p->rc;

		gHR.iHistoryGap = max( gHR.iHistoryGap, pWeapon->rcActive.bottom - pWeapon->rcActive.top );
	}
	else
		pWeapon->hAmmo2 = 0;

	Mem_Free( pList );
}

// Returns the first weapon for a given slot.
WEAPON *WeaponsResource :: GetFirstPos( int iSlot )
{
	WEAPON *pret = NULL;

	for (int i = 0; i < MAX_WEAPON_POSITIONS; i++)
	{
		if ( rgSlots[iSlot][i] && HasAmmo( rgSlots[iSlot][i] ) )
		{
			pret = rgSlots[iSlot][i];
			break;
		}
	}

	return pret;
}


WEAPON* WeaponsResource :: GetNextActivePos( int iSlot, int iSlotPos )
{
	if ( iSlotPos >= MAX_WEAPON_POSITIONS || iSlot >= MAX_WEAPON_SLOTS )
		return NULL;

	WEAPON *p = rgSlots[ iSlot ][ iSlotPos+1 ];

	if ( !p || !HasAmmo(p) )
		return GetNextActivePos( iSlot, iSlotPos + 1 );

	return p;
}

DECLARE_MESSAGE(m_Ammo, CurWeapon );	// Current weapon and clip
DECLARE_MESSAGE(m_Ammo, WeaponList);	// new weapon type
DECLARE_MESSAGE(m_Ammo, AmmoX);			// update known ammo type's count
DECLARE_MESSAGE(m_Ammo, AmmoPickup);	// flashes an ammo pickup record
DECLARE_MESSAGE(m_Ammo, WeapPickup);    // flashes a weapon pickup record
DECLARE_MESSAGE(m_Ammo, HideWeapon);	// hides the weapon, ammo, and crosshair displays temporarily
DECLARE_MESSAGE(m_Ammo, ItemPickup);
DECLARE_MESSAGE(m_Ammo, TankUse);
DECLARE_MESSAGE(m_Ammo, TankAmmo);

DECLARE_COMMAND(m_Ammo, Slot1);
DECLARE_COMMAND(m_Ammo, Slot2);
DECLARE_COMMAND(m_Ammo, Slot3);
DECLARE_COMMAND(m_Ammo, Slot4);
DECLARE_COMMAND(m_Ammo, Slot5);
DECLARE_COMMAND(m_Ammo, Slot6);
DECLARE_COMMAND(m_Ammo, Slot7);
DECLARE_COMMAND(m_Ammo, Slot8);
DECLARE_COMMAND(m_Ammo, Slot9);
DECLARE_COMMAND(m_Ammo, Slot10);
DECLARE_COMMAND(m_Ammo, Close);
DECLARE_COMMAND(m_Ammo, NextWeapon);
DECLARE_COMMAND(m_Ammo, PrevWeapon);
DECLARE_COMMAND(m_Ammo, NextSlot);
DECLARE_COMMAND(m_Ammo, PrevSlot);

int CHudAmmo::Init( int clientId )
{
	gHUD[clientId].AddHudElem(this);

	HOOK_MESSAGE(CurWeapon);
	HOOK_MESSAGE(WeaponList);
	HOOK_MESSAGE(AmmoPickup);
	HOOK_MESSAGE(WeapPickup);
	HOOK_MESSAGE(ItemPickup);
	HOOK_MESSAGE(HideWeapon);
	HOOK_MESSAGE(AmmoX);
	HOOK_MESSAGE(TankUse);
	HOOK_MESSAGE(TankAmmo);

	if ( clientId == 0 )
	{
		HOOK_COMMAND("slot1", Slot1);
		HOOK_COMMAND("slot2", Slot2);
		HOOK_COMMAND("slot3", Slot3);
		HOOK_COMMAND("slot4", Slot4);
		HOOK_COMMAND("slot5", Slot5);
		HOOK_COMMAND("slot6", Slot6);
		HOOK_COMMAND("slot7", Slot7);
		HOOK_COMMAND("slot8", Slot8);
		HOOK_COMMAND("slot9", Slot9);
		HOOK_COMMAND("slot10", Slot10);
		HOOK_COMMAND("cancelselect", Close);
		HOOK_COMMAND("invnext", NextWeapon);
		HOOK_COMMAND("invprev", PrevWeapon);
		HOOK_COMMAND("nextslot", NextSlot);
		HOOK_COMMAND("prevslot", PrevSlot);
	}

	Reset( clientId );

	if ( clientId == 0 )
	{
		CVAR_CREATE( "hud_drawhistory_time", "5", 0 );
		CVAR_CREATE( "hud_fastswitch", "0", FCVAR_ARCHIVE );		// controls whether or not weapons can be selected in one keypress
	}

	m_iFlags |= HUD_ACTIVE; //!!!

	gWR[clientId].Init();
	gHR.Init();

	m_bTankUse[clientId] = 0;
	m_iTankAmmo[clientId] = 0;
	m_flReloadStart = 0;
	m_flReloadTime = 0;

	return 1;
};

void CHudAmmo::Reset( int clientId )
{
	m_fFade = 0;
	m_iFlags |= HUD_ACTIVE; //!!!

	gpActiveSel[clientId] = NULL;
	m_pWeapon = NULL;
	gHUD[clientId].m_iPs2Flags = 0;

	gWR[clientId].Reset();
	gHR.Reset();

	m_flReloadStart = 0;
	m_flReloadTime = 0;
	gHUD[clientId].m_Button.Reset( clientId );
	m_bTankUse[clientId] = 0;
}

int CHudAmmo::VidInit( int clientId )
{
	// Load sprites for buckets (top row of weapon menu)
	m_HUD_bucket0 = gHUD[clientId].GetSpriteIndex( "bucket1" );
	m_HUD_selection = gHUD[clientId].GetSpriteIndex( "selection" );

	ghsprBuckets = gHUD[clientId].GetSprite(m_HUD_bucket0);
	giBucketWidth = gHUD[clientId].GetSpriteRect(m_HUD_bucket0).right - gHUD[clientId].GetSpriteRect(m_HUD_bucket0).left;
	giBucketHeight = gHUD[clientId].GetSpriteRect(m_HUD_bucket0).bottom - gHUD[clientId].GetSpriteRect(m_HUD_bucket0).top;

	gHR.iHistoryGap = max( gHR.iHistoryGap, gHUD[clientId].GetSpriteRect(m_HUD_bucket0).bottom - gHUD[clientId].GetSpriteRect(m_HUD_bucket0).top);

	// If we've already loaded weapons, let's get new sprites
	gWR[clientId].LoadAllWeaponSprites();

	if (ScreenWidth >= 512)
	{
		giABWidth = 20;
		giABHeight = 4;
	}
	else
	{
		giABWidth = 10;
		giABHeight = 2;
	}

	return 1;
}

//
// Think:
//  Used for selection of weapon menu item.
//
void CHudAmmo::Think( int clientId )
{
	if ( gHUD[clientId].m_fPlayerDead )
		return;

	if ( gHUD[clientId].m_iWeaponBits != gWR[clientId].iOldWeaponBits )
	{
		gWR[clientId].iOldWeaponBits = gHUD[clientId].m_iWeaponBits;

		for (int i = MAX_WEAPONS-1; i > 0; i-- )
		{
			WEAPON *p = gWR[clientId].GetWeapon(i);

			if ( p )
			{
				if ( gHUD[clientId].m_iWeaponBits & ( 1 << p->iId ) )
					gWR[clientId].PickupWeapon( p );
				else
					gWR[clientId].DropWeapon( p );
			}
		}
	}

	if (!gpActiveSel[clientId])
		return;

	// has the player selected one?
	if (gHUD[clientId].m_iKeyBits & IN_ATTACK)
	{
		if (gpActiveSel[clientId] != (WEAPON *)1)
		{
			ServerCmd(gpActiveSel[clientId]->szName, clientId);
		}

		gpLastSel[clientId] = gpActiveSel[clientId];
		gpActiveSel[clientId] = NULL;
		gHUD[clientId].m_iKeyBits &= ~IN_ATTACK;

		PlaySound("common/wpn_select.wav", 1, clientId);
	}

}

//
// Helper function to return a Ammo pointer from id
//

HSPRITE* WeaponsResource :: GetAmmoPicFromWeapon( int iAmmoId, wrect_t& rect )
{
	for ( int i = 0; i < MAX_WEAPONS; i++ )
	{
		if ( rgWeapons[i].iAmmoType == iAmmoId )
		{
			rect = rgWeapons[i].rcAmmo;
			return &rgWeapons[i].hAmmo;
		}
		else if ( rgWeapons[i].iAmmo2Type == iAmmoId )
		{
			rect = rgWeapons[i].rcAmmo2;
			return &rgWeapons[i].hAmmo2;
		}
	}

	return NULL;
}


// Menu Selection Code

void WeaponsResource :: SelectSlot( int clientId, int iSlot, int fAdvance, int iDirection )
{
	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	if ( gHUD[clientId].m_Menu.m_fMenuDisplayed && (fAdvance == FALSE) && (iDirection == 1) )
	{ // menu is overriding slot use commands
		gHUD[clientId].m_Menu.SelectMenuItem( iSlot + 1 );  // slots are one off the key numbers
		return;
	}

	if ( iSlot > MAX_WEAPON_SLOTS )
		return;

	if ( gHUD[clientId].m_fPlayerDead || gHUD[clientId].m_iPs2Flags & ( HIDEHUD_WEAPONS | HIDEHUD_ALL ) )
		return;

	if (!(gHUD[clientId].m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return;

	if ( ! ( gHUD[clientId].m_iWeaponBits & ~(1<<(WEAPON_SUIT)) ))
		return;

	WEAPON *p = NULL;

	if ( (gpActiveSel[clientId] == NULL) || (gpActiveSel[clientId] == (WEAPON *)1) || (iSlot != gpActiveSel[clientId]->iSlot) )
	{
		PlaySound( "common/wpn_hudon.wav", 1, clientId );
		p = GetFirstPos( iSlot );

		if ( p && CVAR_GET_FLOAT( "hud_fastswitch" ) > 0 ) // check for fast weapon switch mode
		{
			// if fast weapon switch is on, then weapons can be selected in a single keypress
			// but only if there is only one item in the bucket
			WEAPON *p2 = GetNextActivePos( p->iSlot, p->iSlotPos );
			if ( !p2 )
			{	// only one active item in bucket, so change directly to weapon
				ServerCmd( p->szName, clientId );
				return;
			}
		}
	}
	else
	{
		PlaySound("common/wpn_moveselect.wav", 1, clientId);
		if ( gpActiveSel[clientId] )
			p = GetNextActivePos( gpActiveSel[clientId]->iSlot, gpActiveSel[clientId]->iSlotPos );
		if ( !p )
			p = GetFirstPos( iSlot );
	}


	if ( !p )  // no selection found
	{
		// just display the weapon list, unless fastswitch is on just ignore it
		gpActiveSel[clientId] = (WEAPON *)1;
	}
	else
		gpActiveSel[clientId] = p;
}

//------------------------------------------------------------------------
// Message Handlers
//------------------------------------------------------------------------

//
// AmmoX  -- Update the count of a known type of ammo
//
int CHudAmmo::MsgFunc_AmmoX( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	gWR[clientId].SetAmmo( iIndex, abs(iCount) );

	return 1;
}

int CHudAmmo::MsgFunc_AmmoPickup( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();
	int iCount = READ_BYTE();

	// Add ammo to the history
	gHR.AddToHistory( clientId, HISTSLOT_AMMO, iIndex, abs(iCount) );

	return 1;
}

int CHudAmmo::MsgFunc_WeapPickup( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	int iIndex = READ_BYTE();

	// Add the weapon to the history
	gHR.AddToHistory( clientId, HISTSLOT_WEAP, iIndex );

	return 1;
}

int CHudAmmo::MsgFunc_ItemPickup( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );
	const char *szName = READ_STRING();

	// Add the weapon to the history
	gHR.AddToHistory( clientId, HISTSLOT_ITEM, szName );

	return 1;
}


int CHudAmmo::MsgFunc_HideWeapon( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	gHUD[clientId].m_iPs2Flags = READ_BYTE();

	if ( gHUD[clientId].m_iPs2Flags & ( HIDEHUD_WEAPONS | HIDEHUD_ALL ) )
	{
		gpActiveSel[clientId] = NULL;
		SetCrosshair( 0, D_003A23F8, 0, 0, 0, clientId );
	}
	else
	{
		if ( m_pWeapon )
			SetCrosshair( m_pWeapon->hAutoaim, m_pWeapon->rcAutoaim, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB, clientId );
		else
			SetCrosshair( 0, D_003A23F8, 0, 0, 0, clientId );
	}

	return 1;
}

//
//  CurWeapon: Update hud state with the current weapon and clip count. Ammo
//  counts are updated with AmmoX. Server assures that the Weapon ammo type
//  numbers match a real ammo type.
//
/* NON_MATCHING: o original junta as duas chamadas SetCrosshair(nullrc) mais cedo (cross-jump) e guarda o TRUE em v0 */
int CHudAmmo::MsgFunc_CurWeapon( int clientId, const char *pszName, int iSize, void *pbuf )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/ammo", MsgFunc_CurWeapon__8CHudAmmoiPCciPv);
#else
	BEGIN_READ( pbuf, iSize );

	int iState = READ_BYTE();
	int iId = READ_CHAR();
	int iClip = READ_CHAR();

	// detect if we're also on target
	if ( iId == -1 && iClip == -1 )
	{
		gHUD[clientId].m_fPlayerDead = TRUE;
		SetCrosshair( 0, D_003A2408, 0, 0, 0, clientId );
		gpActiveSel[clientId] = NULL;
		m_pWeapon = NULL;
		return 1;
	}

	if ( iId < 1 )
	{
		SetCrosshair( 0, D_003A2408, 0, 0, 0, clientId );
		gpActiveSel[clientId] = NULL;
		m_pWeapon = NULL;
		return 1;
	}

	gHUD[clientId].m_fPlayerDead = FALSE;

	WEAPON *pWeapon = gWR[clientId].GetWeapon( iId );

	if ( !pWeapon )
		return 1;

	if ( iClip < -1 )
		pWeapon->iClip = abs(iClip);
	else
		pWeapon->iClip = iClip;


	if ( iState == 0 )	// we're not the current weapon, so update no more
		return 1;

	m_pWeapon = pWeapon;

	if ( !(gHUD[clientId].m_iPs2Flags & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
	{
		if ( m_pWeapon->hAutoaim )
		{
			if ( iState == 3 )
				SetCrosshair( m_pWeapon->hAutoaim, m_pWeapon->rcAutoaim, 0, 255, 0, clientId );
			else if ( iState == 2 )
				SetCrosshair( m_pWeapon->hAutoaim, m_pWeapon->rcAutoaim, 192, 0, 0, clientId );
			else
				SetCrosshair( m_pWeapon->hAutoaim, m_pWeapon->rcAutoaim, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB, clientId );
		}
		else
			SetCrosshair( 0, D_003A2408, 0, 0, 0, clientId );
	}

	m_fFade = 200.0f; //!!!
	m_iFlags |= HUD_ACTIVE;

	return 1;
#endif
}

//
// WeaponList -- Tells the hud about a new weapon type.
//
int CHudAmmo::MsgFunc_WeaponList( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	WEAPON Weapon;

	strcpy( Weapon.szName, READ_STRING() );
	Weapon.iAmmoType = (int)READ_CHAR();

	Weapon.iMax1 = READ_BYTE();
	if (Weapon.iMax1 == 255)
		Weapon.iMax1 = -1;

	Weapon.iAmmo2Type = READ_CHAR();
	Weapon.iMax2 = READ_BYTE();
	if (Weapon.iMax2 == 255)
		Weapon.iMax2 = -1;

	Weapon.iSlot = READ_CHAR();
	Weapon.iSlotPos = READ_CHAR();
	Weapon.iId = READ_CHAR();
	Weapon.iFlags = READ_BYTE();
	Weapon.iClip = 0;

	gWR[clientId].AddWeapon( &Weapon );

	return 1;

}

int CHudAmmo::MsgFunc_TankUse( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_bTankUse[clientId] = READ_BYTE();
	m_iTankAmmo[clientId] = READ_SHORT();
	m_iTankAmmo2[clientId] = READ_SHORT();

	return 1;
}

int CHudAmmo::MsgFunc_TankAmmo( int clientId, const char *pszName, int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	m_iTankAmmo[clientId] = READ_SHORT();

	return 1;
}

//------------------------------------------------------------------------
// Command Handlers
//------------------------------------------------------------------------
// Slot button pressed
void CHudAmmo::UserCmd_Slot1( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 0, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot2( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 1, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot3( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 2, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot4( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 3, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot5( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 4, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot6( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 5, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot7( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 6, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot8( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 7, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot9( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 8, FALSE, 1 );
}

void CHudAmmo::UserCmd_Slot10( int clientId )
{
	gWR[clientId].SelectSlot( clientId, 9, FALSE, 1 );
}

void CHudAmmo::UserCmd_Close( int clientId )
{
	if (gpActiveSel[clientId])
	{
		gpLastSel[clientId] = gpActiveSel[clientId];
		gpActiveSel[clientId] = NULL;
		PlaySound("common/wpn_hudoff.wav", 1, clientId);
	}
	else
	{
		char sz[32];
		func_00228EE0( sz, "pause %d", clientId );
		ClientCmd( sz );
	}
}


// Selects the next item in the weapon menu
void CHudAmmo::UserCmd_NextWeapon( int clientId )
{
	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	if ( gHUD[clientId].m_fPlayerDead || (gHUD[clientId].m_iPs2Flags & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[clientId] || gpActiveSel[clientId] == (WEAPON*)1 )
		gpActiveSel[clientId] = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if ( gpActiveSel[clientId] )
	{
		pos = gpActiveSel[clientId]->iSlotPos + 1;
		slot = gpActiveSel[clientId]->iSlot;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot < MAX_WEAPON_SLOTS; slot++ )
		{
			for ( ; pos < MAX_WEAPON_POSITIONS; pos++ )
			{
				WEAPON *wsp = gWR[clientId].GetWeaponSlot( slot, pos );

				if ( wsp && gWR[clientId].HasAmmo(wsp) )
				{
					gpActiveSel[clientId] = wsp;
					return;
				}
			}

			pos = 0;
		}

		slot = 0;  // start looking from the first slot again
	}

	gpActiveSel[clientId] = NULL;
}

// Selects the previous item in the menu
void CHudAmmo::UserCmd_PrevWeapon( int clientId )
{
	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	if ( gHUD[clientId].m_fPlayerDead || (gHUD[clientId].m_iPs2Flags & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[clientId] || gpActiveSel[clientId] == (WEAPON*)1 )
		gpActiveSel[clientId] = m_pWeapon;

	int pos = MAX_WEAPON_POSITIONS-1;
	int slot = MAX_WEAPON_SLOTS-1;
	if ( gpActiveSel[clientId] )
	{
		pos = gpActiveSel[clientId]->iSlotPos - 1;
		slot = gpActiveSel[clientId]->iSlot;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot >= 0; slot-- )
		{
			for ( ; pos >= 0; pos-- )
			{
				WEAPON *wsp = gWR[clientId].GetWeaponSlot( slot, pos );

				if ( wsp && gWR[clientId].HasAmmo(wsp) )
				{
					gpActiveSel[clientId] = wsp;
					return;
				}
			}

			pos = MAX_WEAPON_POSITIONS-1;
		}

		slot = MAX_WEAPON_SLOTS-1;
	}

	gpActiveSel[clientId] = NULL;
}

// PS2: primeira arma do próximo slot
void CHudAmmo::UserCmd_NextSlot( int clientId )
{
	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	if ( gHUD[clientId].m_fPlayerDead || (gHUD[clientId].m_iPs2Flags & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[clientId] || gpActiveSel[clientId] == (WEAPON*)1 )
		gpActiveSel[clientId] = m_pWeapon;

	int pos = 0;
	int slot = 0;
	if ( gpActiveSel[clientId] )
	{
		pos = 0;
		slot = gpActiveSel[clientId]->iSlot + 1;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot < MAX_WEAPON_SLOTS; slot++ )
		{
			for ( ; pos < MAX_WEAPON_POSITIONS; pos++ )
			{
				WEAPON *wsp = gWR[clientId].GetWeaponSlot( slot, pos );

				if ( wsp && gWR[clientId].HasAmmo(wsp) )
				{
					gpActiveSel[clientId] = wsp;
					return;
				}
			}

			pos = 0;
		}

		slot = 0;  // start looking from the first slot again
	}

	gpActiveSel[clientId] = NULL;
}

// PS2: primeira arma do slot anterior
/* NON_MATCHING: fluxo igual; o original põe o bloco "achou" antes do laço e o contador do laço em s8 */
void CHudAmmo::UserCmd_PrevSlot( int clientId )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/ammo", UserCmd_PrevSlot__8CHudAmmoi);
#else
	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	if ( gHUD[clientId].m_fPlayerDead || (gHUD[clientId].m_iPs2Flags & (HIDEHUD_WEAPONS | HIDEHUD_ALL)) )
		return;

	if ( !gpActiveSel[clientId] || gpActiveSel[clientId] == (WEAPON*)1 )
		gpActiveSel[clientId] = m_pWeapon;

	int slot = MAX_WEAPON_SLOTS-1;
	if ( gpActiveSel[clientId] )
	{
		slot = gpActiveSel[clientId]->iSlot - 1;
	}

	for ( int loop = 0; loop <= 1; loop++ )
	{
		for ( ; slot >= 0; slot-- )
		{
			for ( int pos = 0; pos < MAX_WEAPON_POSITIONS; pos++ )
			{
				WEAPON *wsp = gWR[clientId].GetWeaponSlot( slot, pos );

				if ( wsp && gWR[clientId].HasAmmo(wsp) )
				{
					gpActiveSel[clientId] = wsp;
					return;
				}
			}
		}

		slot = MAX_WEAPON_SLOTS-1;
	}

	gpActiveSel[clientId] = NULL;
#endif
}

//-------------------------------------------------------------------------
// Drawing code
//-------------------------------------------------------------------------

int CHudAmmo::Draw( float flTime, int clientId, int iDrawFlags )
{
	int a, x, y, r, g, b;
	int AmmoWidth;

	if (!(gHUD[clientId].m_iWeaponBits & (1<<(WEAPON_SUIT)) ))
		return 1;

	if ( !m_bTankUse[clientId] && (gHUD[clientId].m_iPs2Flags & ( HIDEHUD_WEAPONS | HIDEHUD_ALL )) )
		return 1;

	if ( gHUD[0].m_Message.m_gameTitleTime > 0 )
		return 1;

	// Draw Weapon Menu
	if ( iDrawFlags & 9 )
		DrawWList( flTime, clientId );

	// Draw ammo pickup history
	if ( iDrawFlags & 0x22 )
		gHR.DrawAmmoHistory( flTime, clientId );

	if (!(m_iFlags & HUD_ACTIVE))
		return 0;

	if (!m_pWeapon)
		return 0;

	if ( cl_numlocal >= 2 && ( iDrawFlags & 0x11 ) )
		DrawReloadBar( flTime, clientId );

	WEAPON *pw = m_pWeapon; // shorthand

	// SPR_Draw Ammo
	if ((pw->iAmmoType < 0) && (pw->iAmmo2Type < 0))
		return 0;


	int iFlags = DHN_DRAWZERO; // draw 0 values

	AmmoWidth = gHUD[clientId].GetSpriteRect(gHUD[clientId].m_HUD_number_0).right - gHUD[clientId].GetSpriteRect(gHUD[clientId].m_HUD_number_0).left;

	a = (int)max( MIN_ALPHA, m_fFade );

	if (m_fFade > 0)
		m_fFade -= (gHUD[clientId].m_flTimeDelta * 20);

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	ScaleColors(r, g, b, a );

	y = ScreenHeight - gHUD[clientId].m_iFontHeight - gHUD[clientId].m_iFontHeight/2 - 24;

	if ( m_bTankUse[clientId] )
	{
		// PS2: munição do func_tank
		x = ScreenWidth - (8 * AmmoWidth) - 24;
		x = gHUD[clientId].DrawHudNumber(x, y, iFlags | DHN_4DIGITS, m_iTankAmmo[clientId], r, g, b);

		wrect_t rc;
		rc.top = 0;
		rc.left = 0;
		rc.right = AmmoWidth;
		rc.bottom = 100;

		int iBarWidth =  AmmoWidth/10;

		x += AmmoWidth/2;

		r = gHUD[clientId].m_iColorR;
		g = gHUD[clientId].m_iColorG;
		b = gHUD[clientId].m_iColorB;

		// draw the | bar
		FillRGBA(x, y, iBarWidth, gHUD[clientId].m_iFontHeight, r, g, b, a);

		x += iBarWidth + AmmoWidth/2;;

		// GL Seems to need this
		ScaleColors(r, g, b, a );
		x = gHUD[clientId].DrawHudNumber(x, y, iFlags | DHN_4DIGITS, m_iTankAmmo2[clientId], r, g, b);
	}
	else
	{
		// Does weapon have any ammo at all?
		if ( ( iDrawFlags & 0x12 ) && m_pWeapon->iAmmoType > 0 )
		{
			int iIconWidth = m_pWeapon->rcAmmo.right - m_pWeapon->rcAmmo.left;
			
			if (pw->iClip >= 0)
			{
				// room for the number and the '|' and the current ammo
				
				x = ScreenWidth - (8 * AmmoWidth) - iIconWidth - 8;
				x = gHUD[clientId].DrawHudNumber(x, y, iFlags | DHN_3DIGITS, pw->iClip, r, g, b);

				wrect_t rc;
				rc.top = 0;
				rc.left = 0;
				rc.right = AmmoWidth;
				rc.bottom = 100;

				int iBarWidth =  AmmoWidth/10;

				x += AmmoWidth/2;

				r = gHUD[clientId].m_iColorR;
				g = gHUD[clientId].m_iColorG;
				b = gHUD[clientId].m_iColorB;

				// draw the | bar
				FillRGBA(x, y, iBarWidth, gHUD[clientId].m_iFontHeight, r, g, b, a);

				x += iBarWidth + AmmoWidth/2;;

				// GL Seems to need this
				ScaleColors(r, g, b, a );
				x = gHUD[clientId].DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR[clientId].CountAmmo(pw->iAmmoType), r, g, b);		


			}
			else
			{
				// SPR_Draw a bullets only line
				x = ScreenWidth - 4 * AmmoWidth - iIconWidth - 8;
				x = gHUD[clientId].DrawHudNumber(x, y, iFlags | DHN_3DIGITS, gWR[clientId].CountAmmo(pw->iAmmoType), r, g, b);
			}

			// Draw the ammo Icon
			int iOffset = (m_pWeapon->rcAmmo.bottom - m_pWeapon->rcAmmo.top)/8;
			SPR_Set(m_pWeapon->hAmmo, r, g, b);
			SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo);
		}

		// PS2: a mira é desenhada aqui (scrosshair na tela dividida)
		if ( iDrawFlags & 0x24 )
		{
			int cx, cy;

			if ( cl_numlocal < 2 && m_pWeapon && m_pWeapon->hCrosshair )
			{
				cx = ScreenWidth/2 - (m_pWeapon->rcCrosshair.right - m_pWeapon->rcCrosshair.left)/2;
				cy = ScreenHeight/2 - (m_pWeapon->rcCrosshair.bottom - m_pWeapon->rcCrosshair.top)/2;
				SPR_Set( m_pWeapon->hCrosshair, 255, 255, 255 );
				SPR_DrawHoles( 0, cx, cy, &m_pWeapon->rcCrosshair );
			}
			else if ( cl_numlocal >= 2 && m_pWeapon && m_pWeapon->hSCrosshair )
			{
				cx = ScreenWidth/2 - (m_pWeapon->rcSCrosshair.right - m_pWeapon->rcSCrosshair.left)/2;
				cy = ScreenHeight/2 - (m_pWeapon->rcSCrosshair.bottom - m_pWeapon->rcSCrosshair.top)/2;
				SPR_Set( m_pWeapon->hSCrosshair, 255, 255, 255 );
				SPR_DrawHoles( 0, cx, cy, &m_pWeapon->rcSCrosshair );
			}
		}
	}

	// Does weapon have seconday ammo?
	if ( ( iDrawFlags & 0x12 ) && pw->iAmmo2Type > 0 ) 
	{
		int iIconWidth = m_pWeapon->rcAmmo2.right - m_pWeapon->rcAmmo2.left;

		// Do we have secondary ammo?
		if ((pw->iAmmo2Type != 0) && (gWR[clientId].CountAmmo(pw->iAmmo2Type) > 0))
		{
			y -= gHUD[clientId].m_iFontHeight + gHUD[clientId].m_iFontHeight/4;
			x = ScreenWidth - 4 * AmmoWidth - iIconWidth - 8;
			x = gHUD[clientId].DrawHudNumber(x, y, iFlags|DHN_3DIGITS, gWR[clientId].CountAmmo(pw->iAmmo2Type), r, g, b);

			// Draw the ammo Icon
			SPR_Set(m_pWeapon->hAmmo2, r, g, b);
			int iOffset = (m_pWeapon->rcAmmo2.bottom - m_pWeapon->rcAmmo2.top)/8;
			SPR_DrawAdditive(0, x, y - iOffset, &m_pWeapon->rcAmmo2);
		}
	}
	return 1;
}

//
// Draws the ammo bar on the hud
//
int DrawBar( int x, int y, int width, int height, float f, int clientId )
{
	if (f < 0)
		f = 0;
	if (f > 1)
		f = 1;

	if (f)
	{
		int w = f * width;

		// Always show at least one pixel if we have ammo.
		if (w <= 0)
			w = 1;
		FillRGBA(x, y, w, height, 0, 160, 0, 255);
		x += w;
		width -= w;
	}

	FillRGBA(x, y, width, height, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB, 128);

	return (x + width);
}



void DrawAmmoBar( WEAPON *p, int x, int y, int width, int height, int clientId )
{
	if ( !p )
		return;
	
	if (p->iAmmoType != -1)
	{
		if (!gWR[clientId].CountAmmo(p->iAmmoType))
			return;

		float f = (float)gWR[clientId].CountAmmo(p->iAmmoType)/(float)p->iMax1;
		
		x = DrawBar(x, y, width, height, f, clientId);


		// Do we have secondary ammo too?

		if (p->iAmmo2Type != -1)
		{
			f = (float)gWR[clientId].CountAmmo(p->iAmmo2Type)/(float)p->iMax2;

			x += 5; //!!!

			DrawBar(x, y, width, height, f, clientId);
		}
	}
}

// PS2: chamada pelas dlls (weapons.cpp, shotgun.cpp) ao recarregar
void func_00113D08( float flTime, float flDelay, int clientId )
{
	gHUD[clientId].m_Ammo.m_flReloadStart = flTime;
	gHUD[clientId].m_Ammo.m_flReloadTime = flDelay;
}

// PS2: ícone da arma com a barra de recarga (tela dividida)
/* NON_MATCHING: fluxo igual; registradores (this/y/cores) e y+60 calculado antes da porcentagem */
void CHudAmmo::DrawReloadBar( float flTime, int clientId )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/ammo", DrawReloadBar__8CHudAmmofi);
#else
	int r, g, b, y;
	int bReloading;

	if ( !m_pWeapon )
		return;

	if ( gHUD[clientId].m_iPs2Flags & 0x40 )
		return;

	bReloading = 1;

	if ( D_00399564 )
		y = ScreenHeight - 150;
	else
		y = ScreenHeight - 94;

	if ( flTime < m_flReloadStart || m_flReloadStart < 0.1f || m_flReloadTime < 0.1f || m_flReloadStart + m_flReloadTime <= flTime )
	{
		m_flReloadStart = 0;
		m_flReloadTime = 0;
		bReloading = 0;
	}

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	if ( bReloading )
	{
		int iWidth = (int)( (flTime - m_flReloadStart) / m_flReloadTime * 100 );

		FillRGBA( 83, y + 58, 102, 2, r, g, b, 255 );
		FillRGBA( 83, y + 58, 2, 13, r, g, b, 255 );
		FillRGBA( 83, y + 70, 104, 2, r, g, b, 255 );
		FillRGBA( 185, y + 58, 2, 12, r, g, b, 255 );
		FillRGBA( 85, y + 60, iWidth, 10, r, g, b, 255 );

		r = 255;
		g = 16;
		b = 16;
	}
	else if ( !gWR[clientId].HasAmmo( m_pWeapon ) )
	{
		r = 255;
		g = 16;
		b = 16;
	}

	SPR_Set( m_pWeapon->hActive, r, g, b );
	SPR_DrawAdditive( 0, 50, y, &m_pWeapon->rcActive );

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	FillRGBA( 48, y - 2, 172, 2, r, g, b, 255 );
	FillRGBA( 48, y - 2, 2, 48, r, g, b, 255 );
	FillRGBA( 48, y + 45, 174, 2, r, g, b, 255 );
	FillRGBA( 220, y - 2, 2, 47, r, g, b, 255 );
#endif
}

//
// Draw Weapon Menu
//
/* NON_MATCHING: fluxo igual; o original guarda x na pilha e é 24 bytes maior (registradores) */
int CHudAmmo::DrawWList( float flTime, int clientId )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/ammo", DrawWList__8CHudAmmofi);
#else
	int r,g,b,x,y,a,i;

	if ( !gpActiveSel[clientId] )
		return 0;

	int iActiveSlot;

	if ( gpActiveSel[clientId] == (WEAPON *)1 )
		iActiveSlot = -1;	// current slot has no weapons
	else 
		iActiveSlot = gpActiveSel[clientId]->iSlot;

	x = 18; //!!!
	y = 34; //!!!
	

	// Ensure that there are available choices in the active slot
	if ( iActiveSlot > 0 )
	{
		if ( !gWR[clientId].GetFirstPos( iActiveSlot ) )
		{
			gpActiveSel[clientId] = (WEAPON *)1;
			iActiveSlot = -1;
		}
	}
		
	// Draw top line
	for ( i = 0; i < MAX_WEAPON_SLOTS; i++ )
	{
		int iWidth;

		r = gHUD[clientId].m_iColorR;
		g = gHUD[clientId].m_iColorG;
		b = gHUD[clientId].m_iColorB;
	
		if ( iActiveSlot == i )
			a = 255;
		else
			a = 192;

		ScaleColors(r, g, b, 255);
		SPR_Set(gHUD[clientId].GetSprite(m_HUD_bucket0 + i), r, g, b );

		// make active slot wide enough to accomodate gun pictures
		if ( i == iActiveSlot )
		{
			WEAPON *p = gWR[clientId].GetFirstPos(iActiveSlot);
			if ( p )
				iWidth = p->rcActive.right - p->rcActive.left;
			else
				iWidth = giBucketWidth;
		}
		else
			iWidth = giBucketWidth;

		SPR_DrawAdditive(0, x, y, &gHUD[clientId].GetSpriteRect(m_HUD_bucket0 + i));
		
		x += iWidth + 5;
	}


	a = 128; //!!!
	x = 18;

	// Draw all of the buckets
	for (i = 0; i < MAX_WEAPON_SLOTS; i++)
	{
		y = giBucketHeight + 34;

		// If this is the active slot, draw the bigger pictures,
		// otherwise just draw boxes
		if ( i == iActiveSlot )
		{
			WEAPON *p = gWR[clientId].GetFirstPos( i );
			int iWidth = giBucketWidth;
			if ( p )
				iWidth = p->rcActive.right - p->rcActive.left;

			for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
			{
				p = gWR[clientId].GetWeaponSlot( i, iPos );

				if ( !p || !p->iId )
					continue;

				r = gHUD[clientId].m_iColorR;
				g = gHUD[clientId].m_iColorG;
				b = gHUD[clientId].m_iColorB;
			
				// if active, then we must have ammo.

				if ( gpActiveSel[clientId] == p )
				{
					SPR_Set(p->hActive, r, g, b );
					SPR_DrawAdditive(0, x, y, &p->rcActive);

					SPR_Set(gHUD[clientId].GetSprite(m_HUD_selection), r, g, b );
					SPR_DrawAdditive(0, x, y, &gHUD[clientId].GetSpriteRect(m_HUD_selection));
				}
				else
				{
					// Draw Weapon if Red if no ammo

					if ( gWR[clientId].HasAmmo(p) )
						ScaleColors(r, g, b, 192);
					else
					{
						r = 255;
						g = 16;
						b = 16;
						ScaleColors(r, g, b, 128);
					}

					SPR_Set( p->hInactive, r, g, b );
					SPR_DrawAdditive( 0, x, y, &p->rcInactive );
				}

				// Draw Ammo Bar

				DrawAmmoBar(p, x + giABWidth/2, y, giABWidth, giABHeight, clientId);
				
				y += p->rcActive.bottom - p->rcActive.top + 5;
			}

			x += iWidth + 5;

		}
		else
		{
			// Draw Row of weapons.

			r = gHUD[clientId].m_iColorR;
			g = gHUD[clientId].m_iColorG;
			b = gHUD[clientId].m_iColorB;

			for ( int iPos = 0; iPos < MAX_WEAPON_POSITIONS; iPos++ )
			{
				WEAPON *p = gWR[clientId].GetWeaponSlot( i, iPos );
				
				if ( !p || !p->iId )
					continue;

				if ( gWR[clientId].HasAmmo(p) )
				{
					r = gHUD[clientId].m_iColorR;
					g = gHUD[clientId].m_iColorG;
					b = gHUD[clientId].m_iColorB;
					a = 128;
				}
				else
				{
					r = 255;
					g = 16;
					b = 16;
					a = 96;
				}

				FillRGBA( x, y, giBucketWidth, giBucketHeight, r, g, b, a );

				y += giBucketHeight + 5;
			}

			x += giBucketWidth + 5;
		}
	}	

	return 1;

#endif
}


/* =================================
	GetSpriteList

Finds and returns the matching 
sprite name 'psz' and resolution 'iRes'
in the given sprite list 'pList'
iCount is the number of items in the pList
================================= */
client_sprite_t *GetSpriteList( client_sprite_t *pList, const char *psz, int iRes, int iCount )
{
	if (!pList)
		return NULL;

	int i = iCount;
	client_sprite_t *p = pList;

	while(i--)
	{
		if (!strcmp(psz, p->szName) && (p->iRes == iRes))
			return p;
		p++;
	}

	return NULL;
}
