/* dlls/teamplay_gamerules.h: a classe CHalfLifeTeamplay (o hlg_world.cpp também a inclui,
   por isso a string "HL Teamplay" sai na .rodata dele). */
#ifndef TEAMPLAY_GAMERULES_H
#define TEAMPLAY_GAMERULES_H

#define MAX_TEAMNAME_LENGTH         16
#define MAX_TEAMS                   32
#define TEAMPLAY_TEAMLISTLENGTH     MAX_TEAMS * MAX_TEAMNAME_LENGTH

class CHalfLifeTeamplay : public CHalfLifeMultiplay
{
public:
	CHalfLifeTeamplay();

	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual const char *GetGameDescription( void ) { return "HL Teamplay"; }
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Think ( void );
	virtual int GetTeamIndex( const char *pTeamName );
	virtual const char *GetIndexedTeamName( int teamIndex );
	virtual BOOL IsValidTeam( const char *pTeamName );
	const char *SetDefaultPlayerTeam( CBasePlayer *pPlayer );
	virtual void ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib );

private:
	void RecountTeams( void );
	const char *TeamWithFewestPlayers( void );

	BOOL m_DisableDeathMessages;        /* 0x1C */
	BOOL m_DisableDeathPenalty;         /* 0x20 */
	BOOL m_teamLimit;                   /* 0x24 */
	char m_szTeamList[TEAMPLAY_TEAMLISTLENGTH]; /* 0x28 */
};

#endif
