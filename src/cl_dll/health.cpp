/* cl_dll/health.cpp: CHudHealth do HLSDK, versão do PS2 (cor do HUD por jogador, posições
   para a tela dividida, nada é desenhado enquanto aparece o título do jogo). */

#include "cl_dll/hud.h"
#include "inline_include_asm.h"

extern "C" {
float sqrtf( float );
float sinf( float );
void *memset( void *dst, int c, unsigned int n );
void *memcpy( void *dst, const void *src, unsigned int n );
}
typedef float vec_t;
#include "dlls/vector.h"

#define fabs(x)	__builtin_fabsf(x)
#define max(a,b)	(((a) > (b)) ? (a) : (b))
#define min(a,b)	(((a) < (b)) ? (a) : (b))

#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define AngleVectors (*gEngfuncs.pfnAngleVectors)

extern "C" int cl_numlocal;
extern "C" int D_00399564;

int READ_LONG( void );
float READ_COORD( void );

DECLARE_MESSAGE(m_Health, Health )
DECLARE_MESSAGE(m_Health, Damage )

#define PAIN_NAME "sprites/%d_pain.spz"
#define DAMAGE_NAME "sprites/%d_dmg.spz"

#define DMG_IMAGE_LIFE		2	// seconds that image is up

#define FADE_TIME	100
#define MIN_ALPHA	100

#define DMG_POISON			(1 << 17)
#define DMG_ACID			(1 << 20)
#define DMG_FREEZE			(1 << 4)
#define DMG_SLOWFREEZE		(1 << 22)
#define DMG_DROWN			(1 << 14)
#define DMG_BURN			(1 << 3)
#define DMG_SLOWBURN		(1 << 21)
#define DMG_NERVEGAS		(1 << 16)
#define DMG_RADIATION		(1 << 18)
#define DMG_SHOCK			(1 << 8)
#define DMG_CALTROP			(1<<30)
#define DMG_TRANQ			(1<<23)
#define DMG_CONCUSS			(1<<9)
#define DMG_HALLUC			(1<<31)

int giDmgFlags[NUM_DMG_TYPES] =
{
	DMG_POISON,
	DMG_ACID,
	DMG_FREEZE|DMG_SLOWFREEZE,
	DMG_DROWN,
	DMG_BURN|DMG_SLOWBURN,
	DMG_NERVEGAS,
	DMG_RADIATION,
	DMG_SHOCK,
	DMG_CALTROP,
	DMG_TRANQ,
	DMG_CONCUSS,
	DMG_HALLUC
};

int giDmgHeight = 0, giDmgWidth = 0;

int CHudHealth::Init( int clientId )
{
	HOOK_MESSAGE(Health);
	HOOK_MESSAGE(Damage);
	m_iHealth = 100;
	m_fFade = 0;
	m_iFlags = 0;
	m_bitsDamage = 0;
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
	giDmgHeight = 0;
	giDmgWidth = 0;

	memset(m_dmg, 0, sizeof(DAMAGE_IMAGE) * NUM_DMG_TYPES);

	gHUD[clientId].AddHudElem(this);
	return 1;
}

void CHudHealth::Reset( int clientId )
{
	// make sure the pain compass is cleared when the player respawns
	m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;

	// force all the flashing damage icons to expire
	m_bitsDamage = 0;
	for ( int i = 0; i < NUM_DMG_TYPES; i++ )
	{
		m_dmg[i].fExpire = 0;
	}
}

int CHudHealth::VidInit( int clientId )
{
	m_hSprite = 0;

	m_HUD_dmg_bio = gHUD[clientId].GetSpriteIndex( "dmg_bio" ) + 1;
	m_HUD_cross = gHUD[clientId].GetSpriteIndex( "cross" );

	giDmgHeight = gHUD[clientId].GetSpriteRect(m_HUD_dmg_bio).right - gHUD[clientId].GetSpriteRect(m_HUD_dmg_bio).left;
	giDmgWidth = gHUD[clientId].GetSpriteRect(m_HUD_dmg_bio).bottom - gHUD[clientId].GetSpriteRect(m_HUD_dmg_bio).top;
	return 1;
}

int CHudHealth:: MsgFunc_Health( int clientId, const char *pszName,  int iSize, void *pbuf )
{
	// TODO: update local health data
	BEGIN_READ( pbuf, iSize );
	int x = READ_BYTE();

	m_iFlags |= HUD_ACTIVE;

	// Only update the fade if we've changed health
	if (x != m_iHealth)
	{
		m_fFade = FADE_TIME;
		m_iHealth = x;
	}

	return 1;
}

int CHudHealth:: MsgFunc_Damage( int clientId, const char *pszName,  int iSize, void *pbuf )
{
	BEGIN_READ( pbuf, iSize );

	int armor = READ_BYTE();	// armor
	int damageTaken = READ_BYTE();	// health
	int bitsDamage = READ_LONG(); // damage bits

	vec3_t vecFrom;

	for ( int i = 0 ; i < 3 ; i++)
		vecFrom[i] = READ_COORD();

	UpdateTiles(gHUD[clientId].m_flTime, bitsDamage, clientId);

	// Actually took damage?
	if ( damageTaken > 0 || armor > 0 )
		CalcDamageDirection(vecFrom, clientId);

	return 1;
}

// Returns back a color from the
// Green <-> Yellow <-> Red ramp
void CHudHealth::GetPainColor( int &r, int &g, int &b, int clientId )
{
	int iHealth = m_iHealth;

	if (iHealth > 25)
		iHealth -= 25;
	else if ( iHealth < 0 )
		iHealth = 0;

	if (m_iHealth > 25)
	{
		r = gHUD[clientId].m_iColorR;
		g = gHUD[clientId].m_iColorG;
		b = gHUD[clientId].m_iColorB;
	}
	else
	{
		r = 250;
		g = 0;
		b = 0;
	}
}

int CHudHealth::Draw( float flTime, int clientId, int iDrawFlags )
{
	int r, g, b;
	int a = 0, x, y;
	int HealthWidth;

	if ( gHUD[clientId].m_iPs2Flags & 8 )
		return 1;

	// nada enquanto o título do jogo aparece
	if ( gHUD[0].m_Message.m_gameTitleTime > 0 )
		return 1;

	if ( !m_hSprite )
		m_hSprite = LoadSprite(PAIN_NAME);

	// Has health changed? Flash the health #
	if (m_fFade)
	{
		m_fFade -= (gHUD[clientId].m_flTimeDelta * 20);
		if (m_fFade <= 0)
		{
			a = MIN_ALPHA;
			m_fFade = 0;
		}

		// Fade the health number back to dim

		a = MIN_ALPHA +  (m_fFade/FADE_TIME) * 128;

	}
	else
		a = MIN_ALPHA;

	// If health is getting low, make it bright red
	if (m_iHealth <= 15)
		a = 255;

	GetPainColor( r, g, b, clientId );
	ScaleColors(r, g, b, a );

	// Only draw health if we have the suit.
	if ( ( iDrawFlags & 0x14 ) && gHUD[clientId].m_iPs2Unk20E4 < 0 )
	{
		HealthWidth = gHUD[clientId].GetSpriteRect(gHUD[clientId].m_HUD_number_0).right - gHUD[clientId].GetSpriteRect(gHUD[clientId].m_HUD_number_0).left;
		int CrossWidth = gHUD[clientId].GetSpriteRect(m_HUD_cross).right - gHUD[clientId].GetSpriteRect(m_HUD_cross).left;

		y = ScreenHeight - gHUD[clientId].m_iFontHeight - gHUD[clientId].m_iFontHeight / 2 - 24;
		x = CrossWidth /2 + 8;
		if ( cl_numlocal >= 2 )
		{
			if ( D_00399564 )
				x = CrossWidth /2 + 28;
			else
				x += ScreenWidth / 4;
		}

		SPR_Set(gHUD[clientId].GetSprite(m_HUD_cross), r, g, b);
		SPR_DrawAdditive(0, x, y, &gHUD[clientId].GetSpriteRect(m_HUD_cross));

		x = CrossWidth + HealthWidth / 2 + 8;
		if ( cl_numlocal >= 2 )
		{
			if ( D_00399564 )
				x = CrossWidth + HealthWidth / 2 + 28;
			else
				x += ScreenWidth / 4;
		}

		x = gHUD[clientId].DrawHudNumber(x, y, DHN_3DIGITS | DHN_DRAWZERO, m_iHealth, r, g, b);

		x += HealthWidth/2;

		int iHeight = gHUD[clientId].m_iFontHeight;
		int iWidth = HealthWidth/10;
		FillRGBA(x, y, iWidth, iHeight, gHUD[clientId].m_iColorR, gHUD[clientId].m_iColorG, gHUD[clientId].m_iColorB, a);
	}

	if ( iDrawFlags & 0x21 )
		DrawDamage(flTime, clientId);

	if ( !( iDrawFlags & 0x24 ) )
		return 1;

	return DrawPain(flTime, clientId);
}

void CHudHealth::CalcDamageDirection( Vector vecFrom, int clientId )
{
#ifndef NON_MATCHING
	/* alocação diferente: o alvo guarda this em s2, &right em s0 durante o AngleVectors e tem pilha de 0xB0 */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/health", CalcDamageDirection__10CHudHealthG6Vectori );
#else
	Vector	forward, right, up;
	float	side, front;
	Vector vecOrigin, vecAngles;

	if (!vecFrom[0] && !vecFrom[1] && !vecFrom[2])
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 0;
		return;
	}

	memcpy(vecOrigin, gHUD[clientId].m_vecOrigin, sizeof(vec3_t));
	memcpy(vecAngles, gHUD[clientId].m_vecAngles, sizeof(vec3_t));

	VectorSubtract (vecFrom, vecOrigin, vecFrom);

	float flDistToTarget = vecFrom.Length();

	vecFrom = vecFrom.Normalize();
	AngleVectors (vecAngles, forward, right, up);

	front = DotProduct (vecFrom, right);
	side = DotProduct (vecFrom, forward);

	if (flDistToTarget <= 50)
	{
		m_fAttackFront = m_fAttackRear = m_fAttackRight = m_fAttackLeft = 1;
	}
	else
	{
		if (side > 0)
		{
			if (side > 0.3f)
				m_fAttackFront = max(m_fAttackFront, side);
		}
		else
		{
			float f = fabs(side);
			if (f > 0.3f)
				m_fAttackRear = max(m_fAttackRear, f);
		}

		if (front > 0)
		{
			if (front > 0.3f)
				m_fAttackRight = max(m_fAttackRight, front);
		}
		else
		{
			float f = fabs(front);
			if (f > 0.3f)
				m_fAttackLeft = max(m_fAttackLeft, f);
		}
	}
#endif
}

int CHudHealth::DrawPain( float flTime, int clientId )
{
#ifndef NON_MATCHING
	/* mesma lógica; o agendamento dos argumentos do GetPainColor e do (float)a * max() difere */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/health", DrawPain__10CHudHealthfi );
#else
	if (!(m_fAttackFront || m_fAttackRear || m_fAttackLeft || m_fAttackRight))
		return 1;

	int r, g, b;
	int x, y, a, shade;

	// TODO:  get the shift value of the health
	a = 255;	// max brightness until then

	float fFade = gHUD[clientId].m_flTimeDelta * 2;

	// SPR_Draw top
	if (m_fAttackFront > 0.4f)
	{
		GetPainColor(r,g,b,clientId);
		shade = a * max( m_fAttackFront, 0.5f );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_hSprite, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_hSprite, 0)/2;
		y = ScreenHeight/2 - SPR_Height(m_hSprite,0) * 3;
		SPR_DrawAdditive(0, x, y, NULL);
		m_fAttackFront = max( 0.0f, m_fAttackFront - fFade );
	} else
		m_fAttackFront = 0;

	if (m_fAttackRight > 0.4f)
	{
		GetPainColor(r,g,b,clientId);
		shade = a * max( m_fAttackRight, 0.5f );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_hSprite, r, g, b );

		x = ScreenWidth/2 + SPR_Width(m_hSprite, 1) * 2;
		y = ScreenHeight/2 - SPR_Height(m_hSprite,1)/2;
		SPR_DrawAdditive(1, x, y, NULL);
		m_fAttackRight = max( 0.0f, m_fAttackRight - fFade );
	} else
		m_fAttackRight = 0;

	if (m_fAttackRear > 0.4f)
	{
		GetPainColor(r,g,b,clientId);
		shade = a * max( m_fAttackRear, 0.5f );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_hSprite, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_hSprite, 2)/2;
		y = ScreenHeight/2 + SPR_Height(m_hSprite,2) * 2;
		SPR_DrawAdditive(2, x, y, NULL);
		m_fAttackRear = max( 0.0f, m_fAttackRear - fFade );
	} else
		m_fAttackRear = 0;

	if (m_fAttackLeft > 0.4f)
	{
		GetPainColor(r,g,b,clientId);
		shade = a * max( m_fAttackLeft, 0.5f );
		ScaleColors(r, g, b, shade);
		SPR_Set(m_hSprite, r, g, b );

		x = ScreenWidth/2 - SPR_Width(m_hSprite, 3) * 3;
		y = ScreenHeight/2 - SPR_Height(m_hSprite,3)/2;
		SPR_DrawAdditive(3, x, y, NULL);

		m_fAttackLeft = max( 0.0f, m_fAttackLeft - fFade );
	} else
		m_fAttackLeft = 0;

	return 1;
#endif
}

int CHudHealth::DrawDamage( float flTime, int clientId )
{
#ifndef NON_MATCHING
	/* no alvo o "a" fica na pilha (sp+0xC) e o gEngfuncs num registro salvo */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/cl_dll/health", DrawDamage__10CHudHealthfi );
#else
	int r, g, b, a;
	DAMAGE_IMAGE *pdmg;

	if (!m_bitsDamage)
		return 1;

	r = gHUD[clientId].m_iColorR;
	g = gHUD[clientId].m_iColorG;
	b = gHUD[clientId].m_iColorB;

	a = (int)( fabs(sinf(flTime*2)) * 256.0f);

	ScaleColors(r, g, b, a);

	// Draw all the items
	int i;
	for ( i = 0; i < NUM_DMG_TYPES; i++)
	{
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg = &m_dmg[i];
			SPR_Set(gHUD[clientId].GetSprite(m_HUD_dmg_bio + i), r, g, b );
			SPR_DrawAdditive(0, pdmg->x, pdmg->y, &gHUD[clientId].GetSpriteRect(m_HUD_dmg_bio + i));
		}
	}

	// check for bits that should be expired
	for ( i = 0; i < NUM_DMG_TYPES; i++ )
	{
		DAMAGE_IMAGE *pdmg = &m_dmg[i];

		if ( m_bitsDamage & giDmgFlags[i] )
		{
			pdmg->fExpire = min( flTime + DMG_IMAGE_LIFE, pdmg->fExpire );

			if ( pdmg->fExpire <= flTime		// when the time has expired
				&& a < 40 )						// and the flash is at the low point of the cycle
			{
				pdmg->fExpire = 0;

				int y = pdmg->y;
				pdmg->x = pdmg->y = 0;

				// move everyone above down
				for (int j = 0; j < NUM_DMG_TYPES; j++)
				{
					pdmg = &m_dmg[j];
					if ((pdmg->y) && (pdmg->y < y))
						pdmg->y += giDmgHeight;

				}

				m_bitsDamage &= ~giDmgFlags[i];  // clear the bits
			}
		}
	}

	return 1;
#endif
}

void CHudHealth::UpdateTiles( float flTime, int bitsDamage, int clientId )
{
	DAMAGE_IMAGE *pdmg;

	// Which types are new?
	int bitsOn = ~m_bitsDamage & bitsDamage;

	for (int i = 0; i < NUM_DMG_TYPES; i++)
	{
		pdmg = &m_dmg[i];

		// Is this one already on?
		if (m_bitsDamage & giDmgFlags[i])
		{
			pdmg->fExpire = flTime + DMG_IMAGE_LIFE; // extend the duration
			if (!pdmg->fBaseline)
				pdmg->fBaseline = flTime;
		}

		// Are we just turning it on?
		if (bitsOn & giDmgFlags[i])
		{
			// put this one at the bottom
			pdmg->x = giDmgWidth/8 + 8;
			pdmg->y = ScreenHeight - giDmgHeight * 2 - 24;
			pdmg->fExpire=flTime + DMG_IMAGE_LIFE;

			// move everyone else up
			for (int j = 0; j < NUM_DMG_TYPES; j++)
			{
				if (j == i)
					continue;

				pdmg = &m_dmg[j];
				if (pdmg->y)
					pdmg->y -= giDmgHeight;

			}
			pdmg = &m_dmg[i];
		}
	}

	// damage bits are only turned on here;  they are turned off when the draw time has expired (in DrawDamage())
	m_bitsDamage |= bitsDamage;
}
