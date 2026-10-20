#ifndef DLLS_GAMERULES_H
#define DLLS_GAMERULES_H

/* CGameRules do PS2 (g_pGameRules em g_pGameRules). O vptr fica em 0x10, então há
   0x10 bytes de campos antes; a ordem das virtuais ainda não foi conferida. */

class CGameRules
{
public:
    int m_ps2Unk00[4];                  /* 0x00 */

    virtual void Ps2Virt01(void);
    virtual void Ps2Virt02(void);
    virtual void Ps2Virt03(void);
    virtual void Ps2Virt04(void);
    virtual void Ps2Virt05(void);
    virtual void Ps2Virt06(void);
    virtual void Ps2Virt07(void);
    virtual void Ps2Virt08(void);
    virtual void Ps2Virt09(void);
    virtual BOOL IsMultiplayer(void);   /* slot 10 (0x50): escolhe as listas curtas de sons */
    virtual BOOL IsDeathmatch(void);
    virtual BOOL IsTeamplay(void);
    virtual BOOL IsCoOp(void);
    virtual const char *GetGameDescription(void) { return "Half-Life"; } /* a string sai no .rodata de cada arquivo */
};

extern CGameRules *g_pGameRules;

#endif
