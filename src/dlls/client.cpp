/* dlls/client.cpp: funções do jogo chamadas pelo motor para os clientes
   (conexão, comandos, precache, quadro do servidor). É a versão antiga do
   HLSDK (sem ServerDeactivate, sem LinkUserMessages no ServerActivate) com
   mudanças do PS2: "goto x y z" sem trapaça, StartFrame que roda a IA do
   parceiro (cliente falso) com um controle só, modelo de jogador e pulo
   longo só precacheados fora do multijogador e uma entrada nova da tabela
   de funções que repassa ao Ps2Virt63 das regras. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/saverestore.h"
#include "dlls/player.h"
#include "dlls/soundent.h"
#include "dlls/gamerules.h"

/* ctype.h do newlib do PS2 (_ctype_ + 1 em 0x397D81) */
extern "C" char D_00397D81[];
#define isprint(c) (D_00397D81[(int)(c)] & 0x97)
#define isspace(c) (D_00397D81[(int)(c)] & 8)

#define FL_FAKECLIENT   (1 << 13)
#define FL_DORMANT      (1 << 31)

#define print_console   0
#define GIB_NEVER       1

class CBaseSpectator : public CBaseEntity
{
public:
	void Spawn();
	void SpectatorConnect(void);
	void SpectatorDisconnect(void);
	void SpectatorThink(void);

private:
	void SpectatorImpulseCommand(void);
};

/* customization_t do motor: só o que o PlayerCustomization lê */
typedef struct customization_s
{
	BOOL bInUse;
	char szFileName[64];
	unsigned char type;             /* 0x44: resource.type */
	byte pad45[0x5C - 0x45];
	int nUserData2;                 /* 0x5C */
} customization_t;

#define t_sound     0
#define t_skin      1
#define t_model     2
#define t_decal     3

extern BOOL g_fGameOver;
extern ULONG g_ulFrameCount;
extern "C" char *strstr(const char *, const char *);
char *UTIL_VarArgs(char *format, ...);
extern "C" int cl_numlocal;

extern void CopyToBodyQue(entvars_t *pev);
extern int BuildChangeList(LEVELLIST *pLevelList, int maxList);
extern int giPrecacheGrunt;
extern int gmsgSayText;
extern float g_flWeaponCheat;
typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern ps2cvar_t teamplay;

/*
 * used by kill command and disconnect command
 * ROBIN: Moved here from player.cpp, to allow multiple player models
 */
void set_suicide_frame(entvars_t *pev)
{
	if (!FStrEq(STRING(pev->model), "models/player.dol"))
		return; // allready gibbed

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_TOSS;
	pev->deadflag = DEAD_DEAD;
	pev->nextthink = -1;
}

BOOL ClientConnect(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128])
{
	return g_pGameRules->ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
}

void ClientDisconnect(edict_t *pEntity)
{
	if (g_fGameOver)
		return;

	char text[256];
	sprintf(text, "- %s has left the game\n", STRING(pEntity->v.netname));
	MESSAGE_BEGIN(MSG_ALL, gmsgSayText, NULL);
		WRITE_BYTE(ENTINDEX(pEntity));
		WRITE_STRING(text);
	MESSAGE_END();

	CSound *pSound;
	pSound = CSoundEnt::SoundPointerForIndex(CSoundEnt::ClientSoundIndex(pEntity));
	{
		if (pSound)
		{
			pSound->Reset();
		}
	}

	pEntity->v.takedamage = DAMAGE_NO;
	pEntity->v.solid = SOLID_NOT;
	UTIL_SetOrigin(&pEntity->v, pEntity->v.origin);

	g_pGameRules->ClientDisconnected(pEntity);
}

void respawn(entvars_t *pev, BOOL fCopyCorpse)
{
	if (gpGlobals->coop || gpGlobals->deathmatch)
	{
		if (fCopyCorpse)
		{
			CopyToBodyQue(pev);
		}

		GetClassPtr((CBasePlayer *)pev)->Spawn();
	}
	else
	{
		SERVER_COMMAND("reload\n");
	}
}

void ClientKill(edict_t *pEntity)
{
	entvars_t *pev = &pEntity->v;

	CBasePlayer *pl = (CBasePlayer *)CBasePlayer::Instance(pev);

	if (pl->m_fNextSuicideTime > gpGlobals->time)
		return;

	pl->m_fNextSuicideTime = gpGlobals->time + 1.0f;

	pev->health = 0;
	pl->Killed(pev, GIB_NEVER);
}

void ClientPutInServer(edict_t *pEntity)
{
	CBasePlayer *pPlayer;

	entvars_t *pev = &pEntity->v;

	pPlayer = GetClassPtr((CBasePlayer *)pev);
	pPlayer->SetCustomDecalFrames(-1);
	pPlayer->Spawn();
}

void Host_Say(edict_t *pEntity, int teamonly)
{
	CBasePlayer *client;
	int j;
	char *p;
	char text[128];
	char szTemp[256];
	const char *cpSay = "say";
	const char *cpSayTeam = "say_team";
	const char *pcmd = CMD_ARGV(0);

	if (CMD_ARGC() == 0)
		return;

	if (!stricmp(pcmd, cpSay) || !stricmp(pcmd, cpSayTeam))
	{
		if (CMD_ARGC() >= 2)
		{
			p = (char *)CMD_ARGS();
		}
		else
		{
			return;
		}
	}
	else
	{
		if (CMD_ARGC() >= 2)
		{
			sprintf(szTemp, "%s %s", (char *)pcmd, (char *)CMD_ARGS());
		}
		else
		{
			sprintf(szTemp, "%s", (char *)pcmd);
		}
		p = szTemp;
	}

	if (*p == '"')
	{
		p++;
		p[strlen(p) - 1] = 0;
	}

	char *pc;
	for (pc = p; pc != NULL && *pc != 0; pc++)
	{
		if (isprint(*pc) && !isspace(*pc))
		{
			pc = NULL;
			break;
		}
	}
	if (pc != NULL)
		return;

	if (teamonly)
		sprintf(text, "%c(TEAM) %s: ", 2, STRING(pEntity->v.netname));
	else
		sprintf(text, "%c%s: ", 2, STRING(pEntity->v.netname));

	j = sizeof(text) - 2 - strlen(text);
	if ((int)strlen(p) > j)
		p[j] = 0;

	strcat(text, p);
	strcat(text, "\n");

	client = NULL;
	while (((client = (CBasePlayer *)UTIL_FindEntityByClassname(client, "player")) != NULL) && (!FNullEnt(client->edict())))
	{
		if (!client->pev)
			continue;

		if (client->edict() == pEntity)
			continue;

		if (!(client->IsNetClient()))
			continue;

		if (teamonly && g_pGameRules->PlayerRelationship(client, CBaseEntity::Instance(pEntity)) != GR_TEAMMATE)
			continue;

		MESSAGE_BEGIN(MSG_ONE, gmsgSayText, NULL, client->pev);
			WRITE_BYTE(ENTINDEX(client->edict()));
			WRITE_STRING(text);
		MESSAGE_END();
	}

	MESSAGE_BEGIN(MSG_ONE, gmsgSayText, NULL, &pEntity->v);
		WRITE_BYTE(ENTINDEX(pEntity));
		WRITE_STRING(text);
	MESSAGE_END();

	g_engfuncs.pfnServerPrint(text);
}

void ClientCommand(edict_t *pEntity)
{
	const char *pcmd = CMD_ARGV(0);
	const char *pstr;

	if (!pEntity->pvPrivateData)
		return;

	entvars_t *pev = &pEntity->v;

	if (FStrEq(pcmd, "say"))
	{
		Host_Say(pEntity, 0);
	}
	else if (FStrEq(pcmd, "say_team"))
	{
		Host_Say(pEntity, 1);
	}
	else if (FStrEq(pcmd, "give"))
	{
		if (g_flWeaponCheat != 0.0f)
		{
			int iszItem = ALLOC_STRING(CMD_ARGV(1));
			GetClassPtr((CBasePlayer *)pev)->GiveNamedItem(STRING(iszItem));
		}
	}
	else if (FStrEq(pcmd, "drop"))
	{
		GetClassPtr((CBasePlayer *)pev)->DropPlayerItem((char *)CMD_ARGV(1));
	}
	else if (FStrEq(pcmd, "goto"))
	{
		float x = atof(CMD_ARGV(1));
		float y = atof(CMD_ARGV(2));
		float z = atof(CMD_ARGV(3));
		UTIL_SetOrigin(pev, Vector(x, y, z));
	}
	else if (FStrEq(pcmd, "fov"))
	{
		if (g_flWeaponCheat && CMD_ARGC() > 1)
		{
			GetClassPtr((CBasePlayer *)pev)->m_iFOV = atoi(CMD_ARGV(1));
		}
		else
		{
			CLIENT_PRINTF(pEntity, print_console, UTIL_VarArgs("\"fov\" is \"%d\"\n", (int)GetClassPtr((CBasePlayer *)pev)->m_iFOV));
		}
	}
	else if (FStrEq(pcmd, "use"))
	{
		GetClassPtr((CBasePlayer *)pev)->SelectItem((char *)CMD_ARGV(1));
	}
	else if (((pstr = strstr(pcmd, "weapon_")) != NULL) && (pstr == pcmd))
	{
		GetClassPtr((CBasePlayer *)pev)->SelectItem(pcmd);
	}
	else if (FStrEq(pcmd, "lastinv"))
	{
		GetClassPtr((CBasePlayer *)pev)->SelectLastItem();
	}
	else if (g_pGameRules->ClientCommand(GetClassPtr((CBasePlayer *)pev), pcmd))
	{
	}
	else
	{
		ClientPrint(&pEntity->v, HUD_PRINTCONSOLE, UTIL_VarArgs("Unknown command: %s\n", pcmd));
	}
}

void ClientUserInfoChanged(edict_t *pEntity, char *infobuffer)
{
	if (!pEntity->pvPrivateData)
		return;

	g_pGameRules->ClientUserInfoChanged(GetClassPtr((CBasePlayer *)&pEntity->v), infobuffer);
}

void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
	int i;
	CBaseEntity *pClass;

	for (i = 0; i < edictCount; i++)
	{
		if (pEdictList[i].free)
			continue;

		if (i < clientMax || !pEdictList[i].pvPrivateData)
			continue;

		pClass = CBaseEntity::Instance(&pEdictList[i]);
		if (pClass && !(pClass->pev->flags & FL_DORMANT))
		{
			pClass->Activate();
		}
		else
		{
			ALERT(at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname));
		}
	}
}

void PlayerPreThink(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PreThink();
}

void PlayerPostThink(edict_t *pEntity)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PostThink();
}

void ParmsNewLevel(void)
{
}

void ParmsChangeLevel(void)
{
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if (pSaveData)
		pSaveData->connectionCount = BuildChangeList(pSaveData->levelList, MAX_LEVEL_CONNECTIONS);
}

void StartFrame(void)
{
	if (cl_numlocal < 2)
	{
		for (int i = 1; i <= gpGlobals->maxClients; i++)
		{
			CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex(i);

			if (pPlayer && (pPlayer->pev->flags & FL_FAKECLIENT))
				pPlayer->Ps2AIThink();
		}
	}

	if (g_pGameRules)
		g_pGameRules->Think();

	if (g_fGameOver)
		return;

	gpGlobals->teamplay = teamplay.value;
	g_ulFrameCount++;
}

void ClientPrecache(void)
{
	PRECACHE_SOUND("player/sprayer.wav");
	PRECACHE_SOUND("player/pl_fallpain2.wav");
	PRECACHE_SOUND("player/pl_fallpain3.wav");

	PRECACHE_SOUND("player/pl_step1.wav");
	PRECACHE_SOUND("player/pl_step2.wav");
	PRECACHE_SOUND("player/pl_step3.wav");
	PRECACHE_SOUND("player/pl_step4.wav");

	PRECACHE_SOUND("common/npc_step1.wav");
	PRECACHE_SOUND("common/npc_step2.wav");
	PRECACHE_SOUND("common/npc_step3.wav");
	PRECACHE_SOUND("common/npc_step4.wav");

	PRECACHE_SOUND("player/pl_metal1.wav");
	PRECACHE_SOUND("player/pl_metal2.wav");
	PRECACHE_SOUND("player/pl_metal3.wav");
	PRECACHE_SOUND("player/pl_metal4.wav");

	PRECACHE_SOUND("player/pl_dirt1.wav");
	PRECACHE_SOUND("player/pl_dirt2.wav");
	PRECACHE_SOUND("player/pl_dirt3.wav");
	PRECACHE_SOUND("player/pl_dirt4.wav");

	PRECACHE_SOUND("player/pl_duct1.wav");
	PRECACHE_SOUND("player/pl_duct2.wav");
	PRECACHE_SOUND("player/pl_duct3.wav");
	PRECACHE_SOUND("player/pl_duct4.wav");

	PRECACHE_SOUND("player/pl_grate1.wav");
	PRECACHE_SOUND("player/pl_grate2.wav");
	PRECACHE_SOUND("player/pl_grate3.wav");
	PRECACHE_SOUND("player/pl_grate4.wav");

	PRECACHE_SOUND("player/pl_slosh1.wav");
	PRECACHE_SOUND("player/pl_slosh2.wav");
	PRECACHE_SOUND("player/pl_slosh3.wav");
	PRECACHE_SOUND("player/pl_slosh4.wav");

	PRECACHE_SOUND("player/pl_tile1.wav");
	PRECACHE_SOUND("player/pl_tile2.wav");
	PRECACHE_SOUND("player/pl_tile3.wav");
	PRECACHE_SOUND("player/pl_tile4.wav");
	PRECACHE_SOUND("player/pl_tile5.wav");

	PRECACHE_SOUND("player/pl_swim1.wav");
	PRECACHE_SOUND("player/pl_swim2.wav");
	PRECACHE_SOUND("player/pl_swim3.wav");
	PRECACHE_SOUND("player/pl_swim4.wav");

	PRECACHE_SOUND("player/pl_ladder1.wav");
	PRECACHE_SOUND("player/pl_ladder2.wav");
	PRECACHE_SOUND("player/pl_ladder3.wav");
	PRECACHE_SOUND("player/pl_ladder4.wav");

	PRECACHE_SOUND("player/pl_wade1.wav");
	PRECACHE_SOUND("player/pl_wade2.wav");
	PRECACHE_SOUND("player/pl_wade3.wav");
	PRECACHE_SOUND("player/pl_wade4.wav");

	PRECACHE_SOUND("debris/wood1.wav");
	PRECACHE_SOUND("debris/wood2.wav");
	PRECACHE_SOUND("debris/wood3.wav");

	PRECACHE_SOUND("plats/train_use1.wav");

	PRECACHE_SOUND("buttons/spark5.wav");
	PRECACHE_SOUND("buttons/spark6.wav");
	PRECACHE_SOUND("debris/glass1.wav");
	PRECACHE_SOUND("debris/glass2.wav");
	PRECACHE_SOUND("debris/glass3.wav");

	PRECACHE_SOUND("items/flashlight1.wav");
	PRECACHE_SOUND("items/flashlight1.wav");

	PRECACHE_SOUND("common/bodysplat.wav");

	PRECACHE_SOUND("player/pl_pain2.wav");
	PRECACHE_SOUND("player/pl_pain4.wav");
	PRECACHE_SOUND("player/pl_pain5.wav");
	PRECACHE_SOUND("player/pl_pain6.wav");
	PRECACHE_SOUND("player/pl_pain7.wav");

	if (!g_pGameRules->IsMultiplayer())
		PRECACHE_MODEL("models/player.dol");

	PRECACHE_SOUND("common/wpn_hudoff.wav");
	PRECACHE_SOUND("common/wpn_hudon.wav");
	PRECACHE_SOUND("common/wpn_moveselect.wav");
	PRECACHE_SOUND("common/wpn_select.wav");
	PRECACHE_SOUND("common/wpn_denyselect.wav");

	if (!g_pGameRules->IsMultiplayer())
		PRECACHE_SOUND("player/pl_long_jump.wav");

	PRECACHE_SOUND("player/geiger6.wav");
	PRECACHE_SOUND("player/geiger5.wav");
	PRECACHE_SOUND("player/geiger4.wav");
	PRECACHE_SOUND("player/geiger3.wav");
	PRECACHE_SOUND("player/geiger2.wav");
	PRECACHE_SOUND("player/geiger1.wav");

	if (giPrecacheGrunt)
		UTIL_PrecacheOther("monster_human_grunt");
}

const char *GetGameDescription()
{
	if (g_pGameRules)
		return g_pGameRules->GetGameDescription();
	else
		return "Half-Life";
}

void PlayerCustomization(edict_t *pEntity, customization_t *pCust)
{
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2);
		break;
	case t_sound:
	case t_skin:
	case t_model:
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

void SpectatorConnect(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorConnect();
}

void SpectatorDisconnect(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorDisconnect();
}

void SpectatorThink(edict_t *pEntity)
{
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorThink();
}

/* PS2: entrada nova da tabela de funções, logo depois do SpectatorThink */
void Ps2PlayerCommand(int iPlayerIndex, int a2, int a3)
{
	if (g_pGameRules)
		g_pGameRules->Ps2Virt63(iPlayerIndex, a2, a3);
}
