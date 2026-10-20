#ifndef DLLS_MONSTERS_H
#define DLLS_MONSTERS_H

/* monsters.h do HLSDK (parcial) */

#include "dlls/skill.h"
#include "dlls/basemonster.h"

#define R_AL    -2
#define R_FR    -1
#define R_NO    0
#define R_DL    1
#define R_HT    2
#define R_NM    3

#define bits_CAP_DUCK           (1 << 0)
#define bits_CAP_JUMP           (1 << 1)
#define bits_CAP_STRAFE         (1 << 2)
#define bits_CAP_SQUAD          (1 << 3)
#define bits_CAP_SWIM           (1 << 4)
#define bits_CAP_CLIMB          (1 << 5)
#define bits_CAP_USE            (1 << 6)
#define bits_CAP_HEAR           (1 << 7)
#define bits_CAP_AUTO_DOORS     (1 << 8)
#define bits_CAP_OPEN_DOORS     (1 << 9)
#define bits_CAP_TURN_HEAD      (1 << 10)
#define bits_CAP_RANGE_ATTACK1  (1 << 11)
#define bits_CAP_RANGE_ATTACK2  (1 << 12)
#define bits_CAP_MELEE_ATTACK1  (1 << 13)
#define bits_CAP_MELEE_ATTACK2  (1 << 14)
#define bits_CAP_FLY            (1 << 15)
#define bits_CAP_DOORS_GROUP    (bits_CAP_USE | bits_CAP_AUTO_DOORS | bits_CAP_OPEN_DOORS)

#define SF_MONSTER_WAIT_TILL_SEEN   1
#define SF_MONSTER_GAG              2
#define SF_MONSTER_HITMONSTERCLIP   4
#define SF_MONSTER_PRISONER         16
#define SF_MONSTER_WAIT_FOR_SCRIPT  128
#define SF_MONSTER_PREDISASTER      256
#define SF_MONSTER_FADECORPSE       512
#define SF_MONSTER_FALL_TO_GROUND   0x80000000

#define MONSTER_CUT_CORNER_DIST     8

#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD       1
#define HITGROUP_CHEST      2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4
#define HITGROUP_RIGHTARM   5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG   7

#endif
