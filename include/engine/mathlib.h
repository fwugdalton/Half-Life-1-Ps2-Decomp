#ifndef ENGINE_MATHLIB_H
#define ENGINE_MATHLIB_H

#include "common.h"

typedef float vec_t;
typedef vec_t vec3_t[3];

/* no PS2 M_PI é float: as contas de ângulo são todas em precisão simples */
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define PITCH 0
#define YAW 1
#define ROLL 2

typedef struct mplane_s
{
    vec3_t normal;
    float dist;           /* 0x0C */
    byte type;            /* 0x10 */
    byte signbits;        /* 0x11 */
    byte pad[2];
} mplane_t;

extern vec3_t vec3_origin;
extern int nanmask;

#define IS_NAN(x) (((*(int *)&x) & nanmask) == nanmask)

#define DotProduct(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define VectorSubtract(a, b, c) {(c)[0] = (a)[0] - (b)[0]; (c)[1] = (a)[1] - (b)[1]; (c)[2] = (a)[2] - (b)[2];}
#define VectorAdd(a, b, c) {(c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2];}
#define VectorCopy(a, b) {(b)[0] = (a)[0]; (b)[1] = (a)[1]; (b)[2] = (a)[2];}

float sinf(float x);
float cosf(float x);
float atan2f(float y, float x);
float floorf(float x);
float sqrtf(float x);

float anglemod(float a);
void BOPS_Error(void);
int BoxOnPlaneSide(vec3_t emins, vec3_t emaxs, mplane_t *p);
void AngleVectors(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleVectorsTranspose(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
void AngleMatrix(const vec3_t angles, float (*matrix)[4]);
void AngleIMatrix(const vec3_t angles, float (*matrix)[4]);
void VectorTransform(const vec3_t in1, float in2[3][4], vec3_t out);
int VectorCompare(const vec3_t v1, const vec3_t v2);
void VectorMA(const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc);
vec_t _DotProduct(vec3_t v1, vec3_t v2);
void _VectorSubtract(vec3_t veca, vec3_t vecb, vec3_t out);
void _VectorAdd(vec3_t veca, vec3_t vecb, vec3_t out);
void _VectorCopy(vec3_t in, vec3_t out);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
float Length(const vec3_t v);
float VectorNormalize(vec3_t v);
void VectorInverse(vec3_t v);
void VectorScale(const vec3_t in, vec_t scale, vec3_t out);
int Q_log2(int val);
void VectorMatrix(vec3_t forward, vec3_t right, vec3_t up);
void VectorAngles(const vec3_t forward, vec3_t angles);
void R_ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]);
void R_ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);
void FloorDivMod(float numer, float denom, int *quotient, int *rem);
int GreatestCommonDivisor(int i1, int i2);
int Invert24To16(int val);

#endif /* ENGINE_MATHLIB_H */
