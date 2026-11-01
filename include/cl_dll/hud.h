#ifndef CL_DLL_HUD_H
#define CL_DLL_HUD_H

/* hud.h do cl_dll do HLSDK, versão do PS2 (tela dividida): um CHud por jogador
   (gHUD[clientId], 0x20F4 bytes cada, em 0x308988). Os elementos recebem o clientId
   em Init/Draw/MsgFunc_*. Só os campos já usados têm nome; o resto é preenchimento. */

#ifndef NULL
#define NULL 0
#endif

typedef int HSPRITE;
typedef unsigned char byte;

typedef struct rect_s
{
	int left, right, top, bottom;
} wrect_t;

typedef struct
{
	int iSize;
	int iWidth;
	int iHeight;
	int iFlags;
	int iCharHeight;
	short charWidths[256];
} SCREENINFO;

/* cl_enginefunc_t do HLSDK (cópia do cliente em 0x30CB70, gEngfuncs) */
typedef int (*pfnUserMsgHook)( int clientId, const char *pszName, int iSize, void *pbuf );

typedef struct cl_enginefuncs_s
{
	HSPRITE	(*pfnSPR_Load)( const char *szPicName );						/* 0x00 */
	int		(*pfnSPR_Frames)( HSPRITE hPic );								/* 0x04 */
	int		(*pfnSPR_Height)( HSPRITE hPic, int frame );					/* 0x08 */
	int		(*pfnSPR_Width)( HSPRITE hPic, int frame );						/* 0x0C */
	void	(*pfnSPR_Set)( HSPRITE hPic, int r, int g, int b );				/* 0x10 */
	void	(*pfnSPR_Draw)( int frame, int x, int y, const wrect_t *prc );	/* 0x14 */
	void	(*pfnSPR_DrawHoles)( int frame, int x, int y, const wrect_t *prc );	/* 0x18 */
	void	(*pfnSPR_DrawAdditive)( int frame, int x, int y, const wrect_t *prc );	/* 0x1C */
	void	(*pfnSPR_EnableScissor)( int x, int y, int width, int height );	/* 0x20 */
	void	(*pfnSPR_DisableScissor)( void );								/* 0x24 */
	void	*pfnSPR_GetList;												/* 0x28 */
	void	(*pfnFillRGBA)( int x, int y, int width, int height, int r, int g, int b, int a );	/* 0x2C */
	int		(*pfnGetScreenInfo)( SCREENINFO *pscrinfo );					/* 0x30 */
	void	*pfnSetCrosshair;												/* 0x34 */
	struct cvar_s *(*pfnRegisterVariable)( char *szName, char *szValue, int flags );	/* 0x38 */
	float	(*pfnGetCvarFloat)( char *szName );								/* 0x3C */
	char*	(*pfnGetCvarString)( char *szName );							/* 0x40 */
	int		(*pfnAddCommand)( char *cmd_name, void (*function)(void) );		/* 0x44 */
	int		(*pfnHookUserMsg)( char *szMsgName, pfnUserMsgHook pfn, int clientId );	/* 0x48 */
	void	*pfnPs2Unk4C[4];												/* 0x4C */
	void	(*pfnGetPlayerInfo)( int ent_num, struct hud_player_info_s *pinfo, int iPs2Unk );	/* 0x5C */
	void	*pfnPs2Unk60[4];												/* 0x60 */
	struct client_textmessage_s *(*pfnTextMessageGet)( const char *pName );	/* 0x70 */
	void	*pfnPs2Unk74;													/* 0x74 */
	int		(*pfnDrawConsoleString)( int x, int y, char *string );			/* 0x78 */
	void	(*pfnDrawConsoleStringLen)( const char *string, int *length, int *height );	/* 0x7C */
	void	(*pfnConsolePrint)( const char *string );						/* 0x80 */
	void	(*pfnCenterPrint)( const char *string, int clientId );			/* 0x84 */
} cl_enginefunc_t;

extern cl_enginefunc_t gEngfuncs;

typedef struct client_textmessage_s
{
	int		effect;
	byte	r1, g1, b1, a1;
	byte	r2, g2, b2, a2;
	float	x;
	float	y;
	float	fadein;
	float	fadeout;
	float	holdtime;
	float	fxtime;
	const char *pName;
	const char *pMessage;
} client_textmessage_t;

/* PS2: 0x18 bytes */
typedef struct hud_player_info_s
{
	char *name;
	short ping;
	byte thisplayer;
	byte spectator;
	byte packetloss;
	char *model;
	short topcolor;
	short bottomcolor;
	int iPs2Unk14;
} hud_player_info_t;

#define TextMessageGet (*gEngfuncs.pfnTextMessageGet)
/* cl_util.h */
inline int DrawConsoleString( int x, int y, const char *string )
{
	return gEngfuncs.pfnDrawConsoleString( x, y, (char*) string );
}

inline void GetConsoleStringSize( const char *string, int *width, int *height )
{
	gEngfuncs.pfnDrawConsoleStringLen( string, width, height );
}
#define CVAR_GET_FLOAT (*gEngfuncs.pfnGetCvarFloat)
#define ConsolePrint (*gEngfuncs.pfnConsolePrint)
#define CenterPrint (*gEngfuncs.pfnCenterPrint)

#define SPR_Load (*gEngfuncs.pfnSPR_Load)
#define SPR_Set (*gEngfuncs.pfnSPR_Set)
#define SPR_Frames (*gEngfuncs.pfnSPR_Frames)
#define SPR_Height (*gEngfuncs.pfnSPR_Height)
#define SPR_Width (*gEngfuncs.pfnSPR_Width)
#define SPR_Draw (*gEngfuncs.pfnSPR_Draw)
#define SPR_DrawHoles (*gEngfuncs.pfnSPR_DrawHoles)
#define SPR_DrawAdditive (*gEngfuncs.pfnSPR_DrawAdditive)
#define FillRGBA (*gEngfuncs.pfnFillRGBA)
#define HOOK_MESSAGE(x) gEngfuncs.pfnHookUserMsg( #x, __MsgFunc_##x, clientId );

/* PS2: o gancho recebe o clientId e chama o elemento do HUD daquele jogador */
#define DECLARE_MESSAGE(y, x) int __MsgFunc_##x( int clientId, const char *pszName, int iSize, void *pbuf ) \
	{ \
		return gHUD[clientId].y.MsgFunc_##x( clientId, pszName, iSize, pbuf ); \
	}

#define HUD_ACTIVE		1
#define HUD_INTERMISSION	2

#define MAX_PLAYER_NAME_LENGTH	32

typedef struct {
	int x, y;
} POSITION;

/* sem destrutor virtual no PS2: o vtable é {0,0}, Init, VidInit, Draw, Think, Reset, InitHUDData */
class CHudBase
{
public:
	POSITION  m_pos;		/* 0x0 */
	int   m_type;			/* 0x8 */
	int	  m_iFlags;			/* 0xC */
	/* vptr em 0x10 */
	virtual int Init( int clientId ) {return 0;}
	virtual int VidInit( int clientId ) {return 0;}
	virtual int Draw( float flTime, int clientId, int iDrawFlags ) {return 0;}
	virtual void Think( int clientId ) {return;}
	virtual void Reset( int clientId ) {return;}
	virtual void InitHUDData( int clientId ) {}
};

class CHudStatusBar: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Reset( int clientId );
	void ParseStatusString( int line_num, int clientId );

	int MsgFunc_StatusText( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_StatusValue( int clientId, const char *pszName, int iSize, void *pbuf );

protected:
	enum {
		MAX_STATUSTEXT_LENGTH = 128,
		MAX_STATUSBAR_VALUES = 8,
		MAX_STATUSBAR_LINES = 2,	/* PS2: 2 */
	};

	char m_szStatusText[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];	/* 0x14 */
	char m_szStatusBar[MAX_STATUSBAR_LINES][MAX_STATUSTEXT_LENGTH];		/* 0x114 */
	int m_iStatusValues[MAX_STATUSBAR_VALUES];	/* 0x214 */

	int m_bReparseString;	/* 0x234 */
};

class CHudSayText: public CHudBase
{
public:
	int SayTextPrint( const char *pszBuf, int iBufSize, int clientId );
};

class CHudTextMessage: public CHudBase
{
public:
	int Init( int clientId );
	char *LocaliseTextString( const char *msg, char *dst_buffer, int buffer_size );
	char *BufferedLocaliseTextString( const char *msg );
	char *LookupString( const char *msg_name, int *msg_dest = NULL );
	int MsgFunc_TextMsg( int clientId, const char *pszName, int iSize, void *pbuf );
};

class CHudTrain: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_Train( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	HSPRITE m_hSprite;		/* 0x14 */
	int m_iPos;				/* 0x18 */
};

/* PS2: HUD do vortigaunt jogável (energia do ataque e vida), sem fonte; nome inventado */
class CHudISlave: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Reset( int clientId );
	void DrawCharge( int clientId );
	int MsgFunc_ISlaveHud( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_center;		/* 0x14 */
	int m_HUD_charged;		/* 0x18 */
	int m_HUD_outer;		/* 0x1C */
	int m_HUD_inner;		/* 0x20 */
	int m_iState;			/* 0x24 */
	int m_HUD_health;		/* 0x28 */
	int m_iHealth;			/* 0x2C */
	int m_iPs2Unk30;		/* 0x30 */
};

/* PS2: figura do controle com os botões ligados aos comandos pedidos piscando (tutorial
   do Hazard Course), sem fonte; nome inventado */
class CHudButton: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Reset( int clientId );
	int MsgFunc_ButtonHud( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	int m_iButtons;			/* 0x14: bits dos comandos (+attack, +attack2, ...) */
	int m_HUD_controller;	/* 0x18 */
	int m_HUD_select;		/* 0x1C */
	int m_HUD_start;		/* 0x20 */
	int m_HUD_up;			/* 0x24 */
	int m_HUD_left;			/* 0x28 */
	int m_HUD_right;		/* 0x2C */
	int m_HUD_down;			/* 0x30 */
	int m_HUD_top2;			/* 0x34 */
	int m_HUD_top1;			/* 0x38 */
	int m_HUD_analog_up;	/* 0x3C */
	int m_HUD_analog_down;	/* 0x40 */
	int m_HUD_analog_right;	/* 0x44 */
	int m_HUD_analog_left;	/* 0x48 */
	int m_HUD_analog_up_small;		/* 0x4C */
	int m_HUD_analog_down_small;	/* 0x50 */
	int m_HUD_analog_right_small;	/* 0x54 */
	int m_HUD_analog_left_small;	/* 0x58 */
	int m_HUD_analog_move;	/* 0x5C */
	int m_HUD_analog_press;	/* 0x60 */
	int m_HUD_button;		/* 0x64 */
	int m_HUD_controller_top;	/* 0x68 */
	float m_flTime;			/* 0x6C: some depois deste tempo */
	float m_flBlinkTime;	/* 0x70 */
	int m_iBlink;			/* 0x74: 0..2 */
};

class CHud
{
public:
	byte m_ps2Unk000[0x20];
	float m_flTime;			/* 0x20 */
	byte m_ps2Unk024[0x48 - 0x24];
	int m_iPs2Flags;		/* 0x48: 0x40 = jogador é vortigaunt */
	byte m_ps2Unk04C[0x58 - 0x4C];
	int m_iColorR;			/* 0x58: cor do HUD (no lugar do UnpackRGB(RGB_YELLOWISH)) */
	int m_iColorG;			/* 0x5C */
	int m_iColorB;			/* 0x60 */
	int m_iFontHeight;		/* 0x64 */
	HSPRITE *m_rghSprites;	/* 0x68 */
	wrect_t *m_rgrcRects;	/* 0x6C */
	byte m_ps2Unk070[0xCC - 0x70];
	int m_iPs2Health;		/* 0xCC: vida mostrada pelo HUD do vortigaunt */
	byte m_ps2Unk0D0[0x200 - 0xD0];
	CHudTrain m_Train;		/* 0x200 */
	byte m_ps2Unk21C[0x378 - 0x21C];
	struct
	{
		byte unk00[0x14];
		hud_player_info_t list[33];	/* 0x38C: g_PlayerInfoList do HLSDK, por jogador (tamanho suposto) */
	} m_ps2PlayerInfo;		/* 0x378 */
	byte m_ps2Unk6A4[0x1A88 - 0x6A4];
	CHudStatusBar m_StatusBar;	/* 0x1A88 */
	byte m_ps2Unk1CC0[0x1CD8 - 0x1CC0];
	CHudSayText m_SayText;	/* 0x1CD8 */
	byte m_ps2Unk1CD8[0x1D3C - 0x1CD8 - sizeof(CHudSayText)];
	CHudTextMessage m_TextMessage;	/* 0x1D3C */
	byte m_ps2Unk1D50[0x1E24 - 0x1D50];
	CHudISlave m_ISlave;	/* 0x1E24 */
	CHudButton m_Button;	/* 0x1E58 */
	SCREENINFO m_scrinfo;	/* 0x1ED0 */
	byte m_ps2UnkPad[0x20F4 - 0x1ED0 - sizeof(SCREENINFO)];

	void AddHudElem( CHudBase *p );
	int GetSpriteIndex( const char *SpriteName );
	HSPRITE GetSprite( int index ) { return (index < 0) ? 0 : m_rghSprites[index]; }
	wrect_t& GetSpriteRect( int index ) { return m_rgrcRects[index]; }
};

extern CHud gHUD[];

/* keys.c do motor */
extern "C" int Key_FindBinding( char *binding, int max, int skip, int *count, int *keys );

/* cl_util.h */
HSPRITE LoadSprite( const char *pszName );

#define ScreenHeight (gHUD[clientId].m_scrinfo.iHeight)
#define ScreenWidth (gHUD[clientId].m_scrinfo.iWidth)

/* parsemsg.h */
void BEGIN_READ( void *buf, int size );
int READ_BYTE( void );
int READ_SHORT( void );
char *READ_STRING( void );

#endif
