#ifndef DLLS_EFFECTS_H
#define DLLS_EFFECTS_H

/* só a parte do CSprite usada até agora (effects.h do HLSDK) */

class CSprite : public CPointEntity
{
public:
    void Spawn(void);
    void Precache(void);

    int ObjectCaps(void);
    void EXPORT AnimateThink(void);
    void EXPORT ExpandThink(void);
    void EXPORT AnimateUntilDead(void);
    void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void Animate(float frames);
    void Expand(float scaleSpeed, float fadeSpeed);
    void SpriteInit(const char *pSpriteName, const Vector &origin);

    inline void SetAttachment(edict_t *pEntity, int attachment)
    {
        if (pEntity)
        {
            pev->skin = ENTINDEX(pEntity);
            pev->body = attachment;
            pev->aiment = pEntity;
            pev->movetype = MOVETYPE_FOLLOW;
        }
    }
    void TurnOff(void);
    void TurnOn(void);
    inline float Frames(void) { return m_maxFrame; }
    inline void SetTransparency(int rendermode, int r, int g, int b, int a, int fx)
    {
        pev->rendermode = rendermode;
        pev->rendercolor.x = r;
        pev->rendercolor.y = g;
        pev->rendercolor.z = b;
        pev->renderamt = a;
        pev->renderfx = fx;
    }

    inline void AnimateAndDie(float framerate)
    {
        SetThink(&CSprite::AnimateUntilDead);
        pev->framerate = framerate;
        pev->dmgtime = gpGlobals->time + (m_maxFrame / framerate);
        pev->nextthink = gpGlobals->time;
    }

    static CSprite *SpriteCreate(const char *pSpriteName, const Vector &origin, BOOL animate);

    float m_lastTime;
    float m_maxFrame;
};

/* o gcc 2.95 põe no .rodata as constantes de ponteiro para membro das
   funções inline (mesmo sem uso): por isso o LiveForTime está aqui */
#define SF_BEAM_TEMPORARY   0x8000

class CBeam : public CBaseEntity
{
public:
    inline void SetEndAttachment(int attachment) { pev->skin = (pev->skin & 0x0FFF) | ((attachment & 0xF) << 12); }
    inline void SetColor(int r, int g, int b)
    {
        pev->rendercolor.x = r;
        pev->rendercolor.y = g;
        pev->rendercolor.z = b;
    }
    inline void SetBrightness(int brightness) { pev->renderamt = brightness; }
    inline void SetNoise(int amplitude) { pev->body = amplitude; }
    inline int GetBrightness(void) { return pev->renderamt; }

    static CBeam *BeamCreate(const char *pSpriteName, int width);
    void PointEntInit(const Vector &start, int endIndex);

    inline void LiveForTime(float time)
    {
        SetThink(&CBeam::SUB_Remove);
        pev->nextthink = gpGlobals->time + time;
    }
};

#endif
