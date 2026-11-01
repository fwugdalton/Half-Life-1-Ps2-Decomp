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

typedef struct client_sprite_s
{
	char szName[64];
	char szSprite[64];
	int hspr;
	int iRes;
	wrect_t rc;
} client_sprite_t;

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
	struct client_sprite_s *(*pfnSPR_GetList)( char *psz, int *piCount );	/* 0x28 */
	void	(*pfnFillRGBA)( int x, int y, int width, int height, int r, int g, int b, int a );	/* 0x2C */
	int		(*pfnGetScreenInfo)( SCREENINFO *pscrinfo );					/* 0x30 */
	void	*pfnSetCrosshair;												/* 0x34 */
	struct cvar_s *(*pfnRegisterVariable)( char *szName, char *szValue, int flags );	/* 0x38 */
	float	(*pfnGetCvarFloat)( char *szName );								/* 0x3C */
	char*	(*pfnGetCvarString)( char *szName );							/* 0x40 */
	int		(*pfnAddCommand)( char *cmd_name, void (*function)(void) );		/* 0x44 */
	int		(*pfnHookUserMsg)( char *szMsgName, pfnUserMsgHook pfn, int clientId );	/* 0x48 */
	int		(*pfnServerCmd)( char *szCmdString );							/* 0x4C */
	int		(*pfnClientCmd)( char *szCmdString );							/* 0x50 */
	char	*(*pfnCmd_Argv)( int arg );										/* 0x54 */
	void	*pfnPs2Unk58;													/* 0x58 */
	void	(*pfnGetPlayerInfo)( int ent_num, struct hud_player_info_s *pinfo, int iPs2Unk );	/* 0x5C */
	void	(*pfnPlaySoundByName)( char *szSound, float volume, int clientId );	/* 0x60 */
	void	(*pfnPlaySoundPs2)( char *szSound, float volume, int clientId );	/* 0x64: som com volume (geiger) */
	void	*pfnPs2Unk68;													/* 0x68 */
	void	(*pfnAngleVectors)( const float *vecAngles, float *forward, float *right, float *up );	/* 0x6C */
	struct client_textmessage_s *(*pfnTextMessageGet)( const char *pName );	/* 0x70 */
	int		(*pfnDrawCharacter)( int x, int y, int number, int r, int g, int b );	/* 0x74: TextMessageDrawChar */
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
	byte bPs2Unk07;
	int iClient;		/* 0x8: PS2, jogador local (clientId) dono desta entrada */
	byte spectator;
	byte packetloss;	/* 0xD */
	short sPs2Unk0E;
	char *model;
	short topcolor;
	short bottomcolor;
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
#define ClientCmd (*gEngfuncs.pfnClientCmd)
#define ConsolePrint (*gEngfuncs.pfnConsolePrint)
#define CenterPrint (*gEngfuncs.pfnCenterPrint)
#define PlaySound (*gEngfuncs.pfnPlaySoundByName)

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

#define HOOK_COMMAND(x, y) gEngfuncs.pfnAddCommand( x, __CmdFunc_##y );

/* PS2: o comando recebe o clientId como 1º argumento */
#define DECLARE_COMMAND(y, x) void __CmdFunc_##x( void ) \
	{ \
		int clientId = atoi( gEngfuncs.pfnCmd_Argv( 1 ) ); \
		gHUD[clientId].y.UserCmd_##x( clientId ); \
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
	int Init( int clientId );
	void InitHUDData( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_SayText( int clientId, const char *pszName, int iSize, void *pbuf );
	void SayTextPrint( const char *pszBuf, int iBufSize, int clientId );
	void EnsureTextFitsInOneLineAndWrapIfHaveTo( int line, int clientId );
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

#define MAX_SPRITE_NAME_LENGTH	24

class CHudStatusIcons: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	void Reset( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_StatusIcon( int clientId, const char *pszName, int iSize, void *pbuf );

	enum {
		MAX_ICONSPRITENAME_LENGTH = MAX_SPRITE_NAME_LENGTH,
		MAX_ICONSPRITES = 4,
	};

	void EnableIcon( char *pszIconName, unsigned char red, unsigned char green, unsigned char blue, int clientId );
	void DisableIcon( char *pszIconName );

private:

	typedef struct
	{
		char szSpriteName[MAX_ICONSPRITENAME_LENGTH];
		HSPRITE spr;
		wrect_t rc;
		unsigned char r, g, b;
	} icon_sprite_t;

	icon_sprite_t m_IconList[MAX_ICONSPRITES];	/* 0x14 */
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

#define MAX_PLAYERS		64
#define MAX_TEAMS		64
#define MAX_TEAM_NAME	16

/* PS2: 0x1A bytes; os nomes de damage/shots/hits vêm das colunas SCORE_* do placar */
struct extra_player_info_t
{
	short frags;		/* 0x0 */
	short deaths;		/* 0x2 */
	short damage;		/* 0x4 */
	short shots;		/* 0x6 */
	short hits;			/* 0x8 */
	char teamname[MAX_TEAM_NAME];	/* 0xA */
};

struct team_info_t
{
	char name[MAX_TEAM_NAME];	/* 0x0 */
	short frags;		/* 0x10 */
	short deaths;		/* 0x12 */
	short ping;			/* 0x14 */
	short packetloss;	/* 0x16 */
	short ownteam;		/* 0x18 */
	short players;		/* 0x1A */
	int already_drawn;	/* 0x1C */
	int scores_overriden;	/* 0x20 */
};

/* PS2: as listas g_PlayerInfoList/g_PlayerExtraInfo/g_TeamInfo do HLSDK ficam dentro
   do placar de cada jogador */
class CHudScoreboard: public CHudBase
{
public:
	int Init( int clientId );
	void InitHUDData( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int DrawPlayers( int clientId, int iDrawFlags, int xoffset, float listslot, int nameoffset = 0, char *team = NULL );
	void UserCmd_ShowScores( int clientId );
	void UserCmd_HideScores( int clientId );
	int MsgFunc_ScoreInfo( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamInfo( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_TeamScore( int clientId, const char *pszName, int iSize, void *pbuf );
	void DeathMsg( int killer, int victim, int clientId );

	hud_player_info_t m_PlayerInfoList[MAX_PLAYERS+1];		/* 0x14 */
	extra_player_info_t m_PlayerExtraInfo[MAX_PLAYERS+1];	/* 0x62C */
	team_info_t m_TeamInfo[MAX_TEAMS+1];	/* 0xCC8 */

	int m_iNumTeams;		/* 0x15EC */
	int m_iLastKilledBy;	/* 0x15F0 */
	int m_fLastKillTime;	/* 0x15F4: int no PS2 */
	int m_iPlayerNum;		/* 0x15F8 */
	int m_iShowscoresHeld;	/* 0x15FC */

	void GetAllPlayersInfo( int clientId );
};

#define MAX_MOTD_LENGTH   241

class CHudMOTD : public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Reset( int clientId );

	int MsgFunc_MOTD( int clientId, const char *pszName, int iSize, void *pbuf );

protected:
	static int MOTD_DISPLAY_TIME;
	char m_szMOTD[ MAX_MOTD_LENGTH ];	/* 0x14 */
	float m_flActiveTill;	/* 0x108 */
	int m_iLines;			/* 0x10C */
};

typedef struct
{
	client_textmessage_t *pMessage;	/* 0x0 */
	float time;			/* 0x4 */
	int x, y;			/* 0x8 */
	int totalWidth, totalHeight;	/* 0x10 */
	int width;			/* 0x18 */
	int lines;			/* 0x1C */
	int lineLength;		/* 0x20 */
	int length;			/* 0x24 */
	int r, g, b;		/* 0x28 */
	int text;			/* 0x34 */
	int fadeBlend;		/* 0x38 */
	float charTime;		/* 0x3C */
	float fadeTime;		/* 0x40 */
} message_parms_t;

#define maxHUDMessages 16

class CHudFlashlight: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Reset( int clientId );
	int MsgFunc_Flashlight( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_FlashBat( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	HSPRITE m_hSprite1;		/* 0x14 */
	HSPRITE m_hSprite2;		/* 0x18 */
	HSPRITE m_hBeam;		/* 0x1C */
	wrect_t *m_prc1;		/* 0x20 */
	wrect_t *m_prc2;		/* 0x24 */
	wrect_t *m_prcBeam;		/* 0x28 */
	float m_flBat;			/* 0x2C */
	int m_iBat;				/* 0x30 */
	int m_fOn;				/* 0x34 */
	float m_fFade;			/* 0x38 */
	int m_iWidth;			/* 0x3C */
};

class CHudDeathNotice: public CHudBase
{
public:
	int Init( int clientId );
	void InitHUDData( void );	/* PS2: sem clientId, não sobrescreve o virtual */
	int VidInit( int clientId );
	void Reset( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_DeathMsg( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_d_skull;		/* 0x14 */
};

/* PS2: ícone de agachar/andar/correr (sem equivalente no HLSDK) */
class CHudCrouch: public CHudBase
{
public:
	int Init( int clientId );
	void Reset( int clientId );
	int VidInit( int clientId );
	int MsgFunc_CrouchState( int clientId, const char *pszName, int iSize, void *pbuf );
	int Draw( float flTime, int clientId, int iDrawFlags );

private:
	HSPRITE m_hCrouch;		/* 0x14 */
	HSPRITE m_hWalk;		/* 0x18 */
	HSPRITE m_hRun;			/* 0x1C */
	wrect_t *m_prcCrouch;	/* 0x20 */
	wrect_t *m_prcWalk;		/* 0x24 */
	wrect_t *m_prcRun;		/* 0x28 */
	int m_iState;			/* 0x2C: 1 correndo, 2 agachado, senão andando */
	int m_iWidth;			/* 0x30 */
};

/* ammo.cpp, ammo_secondary.cpp e battery.cpp ainda não decompilados: só o tamanho e os virtuais */
class CHudAmmo: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	void Think( int clientId );
	void Reset( int clientId );

private:
	byte m_ps2Unk14[0x44 - 0x14];
};

#define MAX_SEC_AMMO_VALUES	4

class CHudAmmoSecondary: public CHudBase
{
public:
	int Init( int clientId );
	void Reset( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );

	int MsgFunc_SecAmmoVal( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_SecAmmoIcon( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	int m_HUD_ammoicon;		/* 0x14: sprite indices */
	int m_iAmmoAmounts[MAX_SEC_AMMO_VALUES];	/* 0x18 */
	float m_fFade;			/* 0x28 */
};

class CHudBattery: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_Battery( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	HSPRITE m_hSprite1;		/* 0x14 */
	HSPRITE m_hSprite2;		/* 0x18 */
	wrect_t *m_prc1;		/* 0x1C */
	wrect_t *m_prc2;		/* 0x20 */
	int m_iBat;				/* 0x24 */
	float m_fFade;			/* 0x28 */
	int m_iHeight;			/* 0x2C */
};

class CHudGeiger: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_Geiger( int clientId, const char *pszName, int iSize, void *pbuf );

private:
	int m_iGeigerRange;		/* 0x14 */
};

#define NUM_DMG_TYPES 12

typedef struct
{
	float fExpire;
	float fBaseline;
	int x, y;
} DAMAGE_IMAGE;

class Vector;

class CHudHealth: public CHudBase
{
public:
	virtual int Init( int clientId );
	virtual int VidInit( int clientId );
	virtual int Draw( float fTime, int clientId, int iDrawFlags );
	virtual void Reset( int clientId );
	int MsgFunc_Health( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Damage( int clientId, const char *pszName, int iSize, void *pbuf );
	int m_iHealth;			/* 0x14 */
	int m_HUD_dmg_bio;		/* 0x18 */
	int m_HUD_cross;		/* 0x1C */
	void GetPainColor( int &r, int &g, int &b, int clientId );

private:
	HSPRITE m_hSprite;		/* 0x20 */
	HSPRITE m_hDamage;		/* 0x24 */

	DAMAGE_IMAGE m_dmg[NUM_DMG_TYPES];	/* 0x28 */
	int	m_bitsDamage;		/* 0xE8 */
public:
	float m_fFade;			/* 0xEC */
	float m_fAttackFront, m_fAttackRear, m_fAttackLeft, m_fAttackRight;	/* 0xF0 */
private:
	int DrawPain( float fTime, int clientId );
	int DrawDamage( float fTime, int clientId );
	void CalcDamageDirection( Vector vecFrom, int clientId );
	void UpdateTiles( float fTime, int bits, int clientId );
};

/* PS2: m_gameTitleTime vem antes da lista e não existe m_bEndAfterMessage */
class CHudMessage: public CHudBase
{
public:
	int Init( int clientId );
	int VidInit( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_HudText( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_GameTitle( int clientId, const char *pszName, int iSize, void *pbuf );

	float FadeBlend( float fadein, float fadeout, float hold, float localTime );
	int XPosition( float x, int width, int lineWidth, int clientId );
	int YPosition( float y, int height, int clientId );

	void MessageAdd( const char *pName, float time );
	void MessageDrawScan( client_textmessage_t *pMessage, float time, int clientId );
	void MessageScanStart( void );
	void MessageScanNextChar( int clientId );
	void Reset( int clientId );

public:
	float m_gameTitleTime;	/* 0x14 */
private:
	client_textmessage_t *m_pMessages[maxHUDMessages];	/* 0x18 */
	float m_startTime[maxHUDMessages];	/* 0x58 */
	message_parms_t m_parms;	/* 0x98 */
	client_textmessage_t *m_pGameTitle;	/* 0xDC */

	int m_HUD_title_life;	/* 0xE0 */
	int m_HUD_title_half;	/* 0xE4 */
};

class CHudMenu : public CHudBase
{
public:
	int Init( int clientId );
	void InitHUDData( int clientId );
	int VidInit( int clientId );
	void Reset( int clientId );
	int Draw( float flTime, int clientId, int iDrawFlags );
	int MsgFunc_ShowMenu( int clientId, const char *pszName, int iSize, void *pbuf );

	void SelectMenuItem( int menu_item );

	int m_fMenuDisplayed;	/* 0x14 */
	int m_bitsValidSlots;	/* 0x18 */
	float m_flShutoffTime;	/* 0x1C */
	int m_fWaitingForMore;	/* 0x20 */
};

struct HUDLIST {
	CHudBase *p;
	HUDLIST *pNext;
};

typedef float vec3_t[3];

/* client_data_t do PS2 (0x40 bytes?) */
typedef struct client_data_s
{
	vec3_t origin;			/* 0x0 */
	byte ps2Unk0C[0x14 - 0xC];
	vec3_t viewangles;		/* 0x14 */
	byte ps2Unk20[0x2C - 0x20];
	int iWeaponBits;		/* 0x2C */
	int iPs2Unk30;			/* 0x30 */
	float fov;				/* 0x34 */
	float idlescale;		/* 0x38 */
	float mouse_sensitivity;	/* 0x3C */
} client_data_t;

class CHud
{
public:
	HUDLIST *m_pHudList;	/* 0x0 */
	HSPRITE m_hsprLogo;		/* 0x4 */
	int m_iLogo;			/* 0x8 */
	client_sprite_t *m_pSpriteList;	/* 0xC */
	int m_iSpriteCount;	/* 0x10 */
	int m_iSpriteCountAllRes;	/* 0x14 */
	float m_flMouseSensitivity;	/* 0x18 */
	int m_iConcussionEffect;	/* 0x1C */
	float m_flTime;			/* 0x20 */
	float m_fOldTime;		/* 0x24 */
	float m_flTimeDelta;	/* 0x28 */
	vec3_t m_vecOrigin;		/* 0x2C */
	vec3_t m_vecAngles;		/* 0x38 */
	int m_iWeaponBits;		/* 0x44 */
	int m_iPs2Flags;		/* 0x48: 0x40 = jogador é vortigaunt, 4 = esconde o HUD */
	int m_iFOV;				/* 0x4C */
	int m_Teamplay;			/* 0x50 */
	int m_iRes;				/* 0x54 */
	int m_iColorR;			/* 0x58: cor do HUD (no lugar do UnpackRGB(RGB_YELLOWISH)) */
	int m_iColorG;			/* 0x5C */
	int m_iColorB;			/* 0x60 */
	int m_iFontHeight;		/* 0x64 */
	HSPRITE *m_rghSprites;	/* 0x68 */
	wrect_t *m_rgrcRects;	/* 0x6C */
	char *m_rgszSpriteNames;	/* 0x70 */
	CHudAmmo m_Ammo;		/* 0x74 */
	CHudHealth m_Health;	/* 0xB8 */
	CHudGeiger m_Geiger;	/* 0x1B8 */
	CHudBattery m_Battery;	/* 0x1D0 */
	CHudTrain m_Train;		/* 0x200 */
	CHudFlashlight m_Flash;	/* 0x21C */
	CHudCrouch m_Crouch;	/* 0x25C */
	CHudMessage m_Message;	/* 0x290 */
	CHudScoreboard m_Scoreboard;	/* 0x378 */
	CHudMOTD m_MOTD;		/* 0x1978 */
	CHudStatusBar m_StatusBar;	/* 0x1A88 */
	CHudDeathNotice m_DeathNotice;	/* 0x1CC0 */
	CHudSayText m_SayText;	/* 0x1CD8 */
	CHudMenu m_Menu;		/* 0x1CEC */
	CHudAmmoSecondary m_AmmoSecondary;	/* 0x1D10 */
	CHudTextMessage m_TextMessage;	/* 0x1D3C */
	CHudStatusIcons m_StatusIcons;	/* 0x1D50 */
	CHudISlave m_ISlave;	/* 0x1E24 */
	CHudButton m_Button;	/* 0x1E58 */
	SCREENINFO m_scrinfo;	/* 0x1ED0 */
	int m_iPs2Unk20E4;		/* 0x20E4: copiado do client_data_t (0x30) */
	int m_iPs2Unk20E8;
	int m_iIntermission;	/* 0x20EC */
	int m_HUD_number_0;		/* 0x20F0 */

	CHud() : m_pHudList(0), m_iSpriteCount(0) {}
	~CHud();

	void Init( int clientId );
	void VidInit( int clientId );
	void Think( int clientId );
	int Redraw( float flTime, int intermission, int clientId, int iDrawFlags );
	int UpdateClientData( client_data_t *cdata, int clientId );

	void AddHudElem( CHudBase *p );
	int GetSpriteIndex( const char *SpriteName );
	int DrawHudString( int x, int y, int iMaxX, char *szString, int r, int g, int b, int clientId );
	int DrawHudNumberString( int xpos, int ypos, int iMinX, int iNumber, int r, int g, int b, int clientId );
	int DrawHudStringReverse( int xpos, int ypos, int iMinX, char *szString, int r, int g, int b, int clientId );
	int DrawHudNumber( int x, int y, int iFlags, int iNumber, int r, int g, int b );
	int GetNumWidth( int iNumber, int iFlags );

	int MsgFunc_ResetHUD( int clientId, const char *pszName, int iSize, void *pbuf );
	void MsgFunc_InitHUD( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_GameMode( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Damage( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Concuss( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_HudColor( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_Logo( int clientId, const char *pszName, int iSize, void *pbuf );
	int MsgFunc_SetFOV( int clientId, const char *pszName, int iSize, void *pbuf );
	HSPRITE GetSprite( int index ) { return (index < 0) ? 0 : m_rghSprites[index]; }
	wrect_t& GetSpriteRect( int index ) { return m_rgrcRects[index]; }
};

extern CHud gHUD[];

/* keys.c do motor */
extern "C" int Key_FindBinding( char *binding, int max, int skip, int *count, int *keys );

/* cl_util.h */
HSPRITE LoadSprite( const char *pszName );
void ScaleColors( int &r, int &g, int &b, int a );

inline int TextMessageDrawChar( int x, int y, int number, int r, int g, int b )
{
	return gEngfuncs.pfnDrawCharacter( x, y, number, r, g, b );
}

#define DHN_DRAWZERO 1
#define DHN_2DIGITS  2
#define DHN_3DIGITS  4
#define DHN_4DIGITS  8	/* PS2 */

#define ScreenHeight (gHUD[clientId].m_scrinfo.iHeight)
#define ScreenWidth (gHUD[clientId].m_scrinfo.iWidth)

/* parsemsg.h */
void BEGIN_READ( void *buf, int size );
int READ_BYTE( void );
int READ_SHORT( void );
char *READ_STRING( void );

#endif
