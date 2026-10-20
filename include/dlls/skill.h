#ifndef DLLS_SKILL_H
#define DLLS_SKILL_H

/* skill.h do HLSDK: gSkillData em D_00318xxx (a ver o início exato) */

struct skilldata_t
{
    int iSkillLevel;

    float agruntHealth;
    float agruntDmgPunch;

    float apacheHealth;

    float barneyHealth;

    float bigmommaHealthFactor;
    float bigmommaDmgSlash;
    float bigmommaDmgBlast;
    float bigmommaRadiusBlast;

    float bullsquidHealth;
    float bullsquidDmgBite;
    float bullsquidDmgWhip;
    float bullsquidDmgSpit;

    float gargantuaHealth;
    float gargantuaDmgSlash;
    float gargantuaDmgFire;
    float gargantuaDmgStomp;

    float hassassinHealth;

    float headcrabHealth;
    float headcrabDmgBite;

    float hgruntHealth;
    float hgruntDmgKick;
    float hgruntShotgunPellets;
    float hgruntGrenadeSpeed;

    float houndeyeHealth;
    float houndeyeDmgBlast;

    float slaveHealth;
    float slaveDmgClaw;
    float slaveDmgClawrake;
    float slaveDmgZap;

    float ichthyosaurHealth;
    float ichthyosaurDmgShake;

    float leechHealth;
    float leechDmgBite;

    float controllerHealth;
    float controllerDmgZap;
    float controllerSpeedBall;
    float controllerDmgBall;

    float nihilanthHealth;
    float nihilanthZap;

    float scientistHealth;

    float snarkHealth;
    float snarkDmgBite;
    float snarkDmgPop;

    float zombieHealth;
    float zombieDmgOneSlash;
    float zombieDmgBothSlash;

    float turretHealth;
    float miniturretHealth;
    float sentryHealth;

    float plrDmgCrowbar;
    float plrDmg9MM;
    float plrDmg357;
    float plrDmgMP5;
    float plrDmgM203Grenade;
    float plrDmgBuckshot;
    float plrDmgCrossbowClient;
    float plrDmgCrossbowMonster;
    float plrDmgRPG;
    float plrDmgGauss;
    float plrDmgEgonNarrow;
    float plrDmgEgonWide;
    float plrDmgHornet;
    float plrDmgHandGrenade;
    float plrDmgSatchel;
    float plrDmgTripmine;

    float monDmg9MM;
    float monDmgMP5;
    float monDmg12MM;
    float monDmgHornet;

    float suitchargerCapacity;
    float batteryCapacity;
    float healthchargerCapacity;
    float healthkitCapacity;
    float scientistHeal;

    float monHead;
    float monChest;
    float monStomach;
    float monLeg;
    float monArm;

    float plrHead;
    float plrChest;
    float plrStomach;
    float plrLeg;
    float plrArm;
};

extern skilldata_t gSkillData;

#endif
