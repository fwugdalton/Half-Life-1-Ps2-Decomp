/* cl_dll/ammohistory.cpp: histórico de itens pegos do HLSDK, versão do PS2 (cada item
   guarda o clientId; gWR por jogador; altura máxima ScreenHeight - 132; tudo 24 pixels
   mais alto; munição desenhada em ScreenWidth - 32). */

#include "cl_dll/hud.h"
#include "cl_dll/ammohistory.h"
#include "inline_include_asm.h"

#define min(a,b)	(((a) < (b)) ? (a) : (b))

HistoryResource gHR;

#define AMMO_PICKUP_GAP (gHR.iHistoryGap+5)
#define AMMO_PICKUP_PICK_HEIGHT		(32 + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX		(ScreenHeight - 132)

int HISTORY_DRAW_TIME = 5;

/* PS2: inicializa os dois gWR (o WeaponsResource::Init de cada jogador) */
extern "C" void InitWeaponsResources( void )
{
	gWR[0].Init();
	gWR[1].Init();
}

void HistoryResource :: AddToHistory( int clientId, int iType, int iId, int iCount )
{
	if ( iType == HISTSLOT_AMMO && !iCount )
		return;  // no amount, so don't add

	if ( (((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot
	HISTORY_DRAW_TIME = CVAR_GET_FLOAT( "hud_drawhistory_time" );

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gHUD[clientId].m_flTime + HISTORY_DRAW_TIME;
	freeslot->iClient = clientId;
}

void HistoryResource :: AddToHistory( int clientId, int iType, const char *szName, int iCount )
{
	if ( iType != HISTSLOT_ITEM )
		return;

	if ( (((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY) )
	{	// the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++];  // default to just writing to the first slot

	// I am really unhappy with all the code in this file

	int i = gHUD[clientId].GetSpriteIndex( szName );
	if ( i == -1 )
		return;  // unknown sprite name, don't add it to history

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	HISTORY_DRAW_TIME = CVAR_GET_FLOAT( "hud_drawhistory_time" );
	freeslot->DisplayTime = gHUD[clientId].m_flTime + HISTORY_DRAW_TIME;
	freeslot->iClient = clientId;
}

void HistoryResource :: CheckClearHistory( void )
{
	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
			return;
	}

	iCurrentHistorySlot = 0;
}

//
// Draw Ammo pickup history
//
int HistoryResource :: DrawAmmoHistory( float flTime, int clientId )
{
#ifndef NON_MATCHING
	INLINE_INCLUDE_ASM("asm/nonmatchings/cl_dll/ammohistory", DrawAmmoHistory__15HistoryResourcefi);
#else

	for ( int i = 0; i < MAX_HISTORY; i++ )
	{
		if ( rgAmmoHistory[i].type )
		{
			if ( rgAmmoHistory[i].iClient != clientId )
				continue;

			rgAmmoHistory[i].DisplayTime = min( rgAmmoHistory[i].DisplayTime, gHUD[clientId].m_flTime + HISTORY_DRAW_TIME );

			if ( rgAmmoHistory[i].DisplayTime <= flTime )
			{  // pic drawing time has expired
				memset( &rgAmmoHistory[i], 0, sizeof(HIST_ITEM) );
				CheckClearHistory();
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_AMMO )
			{
				wrect_t rcPic;
				HSPRITE *spr = gWR[clientId].GetAmmoPicFromWeapon( rgAmmoHistory[i].iId, rcPic );

				int r, g, b;
				r = gHUD[clientId].m_iColorR;
				g = gHUD[clientId].m_iColorG;
				b = gHUD[clientId].m_iColorB;
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, (int)min(scale, 255) );

				// Draw the pic
				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i)) - 24;
				int xpos = ScreenWidth - 32;
				if ( spr && *spr )    // weapon isn't loaded yet so just don't draw the pic
				{ // the dll has to make sure it has sent info the weapons you need
					SPR_Set( *spr, r, g, b );
					SPR_DrawAdditive( 0, xpos, ypos, &rcPic );
				}

				// Draw the number
				gHUD[clientId].DrawHudNumberString( xpos - 10, ypos, xpos - 100, rgAmmoHistory[i].iCount, r, g, b, clientId );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_WEAP )
			{
				WEAPON *weap = gWR[clientId].GetWeapon( rgAmmoHistory[i].iId );

				if ( !weap )
					return 1;  // we don't know about the weapon yet, so don't draw anything

				int r, g, b;
				r = gHUD[clientId].m_iColorR;
				g = gHUD[clientId].m_iColorG;
				b = gHUD[clientId].m_iColorB;

				if ( !gWR[clientId].HasAmmo( weap ) )
				{
					// if the weapon doesn't have ammo, display it as red
					r = 255;
					g = 16;
					b = 16;
				}

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, (int)min(scale, 255) );

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i)) - 24;
				int xpos = ScreenWidth - (weap->rcInactive.right - weap->rcInactive.left) - 8;
				SPR_Set( weap->hInactive, r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &weap->rcInactive );
			}
			else if ( rgAmmoHistory[i].type == HISTSLOT_ITEM )
			{
				int r, g, b;

				if ( !rgAmmoHistory[i].iId )
					continue;  // sprite not loaded

				wrect_t rect = gHUD[clientId].GetSpriteRect( rgAmmoHistory[i].iId );

				r = gHUD[clientId].m_iColorR;
				g = gHUD[clientId].m_iColorG;
				b = gHUD[clientId].m_iColorB;
				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				ScaleColors(r, g, b, (int)min(scale, 255) );

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i)) - 24;
				int xpos = ScreenWidth - (rect.right - rect.left) - 18;

				SPR_Set( gHUD[clientId].GetSprite( rgAmmoHistory[i].iId ), r, g, b );
				SPR_DrawAdditive( 0, xpos, ypos, &rect );
			}
		}
	}

	return 1;
#endif
}
