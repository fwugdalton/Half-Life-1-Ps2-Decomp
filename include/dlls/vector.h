#ifndef DLLS_VECTOR_H
#define DLLS_VECTOR_H

/* Vector do HLSDK (dlls/vector.h); no PS2 a raiz é sqrtf (sqrt.s direto) */

class Vector2D
{
public:
    inline Vector2D(void) { }
    inline Vector2D(float X, float Y) { x = X; y = Y; }
    inline Vector2D operator+(const Vector2D &v) const { return Vector2D(x + v.x, y + v.y); }
    inline Vector2D operator-(const Vector2D &v) const { return Vector2D(x - v.x, y - v.y); }
    inline Vector2D operator*(float fl) const { return Vector2D(x * fl, y * fl); }
    inline Vector2D operator/(float fl) const { return Vector2D(x / fl, y / fl); }

    inline float Length(void) const { return sqrtf(x * x + y * y); }

    inline Vector2D Normalize(void) const
    {
        Vector2D vec2;

        float flLen = Length();
        if (flLen == 0)
        {
            return Vector2D(0, 0);
        }
        else
        {
            flLen = 1 / flLen;
            return Vector2D(x * flLen, y * flLen);
        }
    }

    vec_t x, y;
};

inline float DotProduct(const Vector2D &a, const Vector2D &b) { return (a.x * b.x + a.y * b.y); }
inline Vector2D operator*(float fl, const Vector2D &v) { return v * fl; }

class Vector
{
public:
    inline Vector(void) { }
    inline Vector(float X, float Y, float Z) { x = X; y = Y; z = Z; }
    inline Vector(const Vector &v) { x = v.x; y = v.y; z = v.z; }
    inline Vector(float rgfl[3]) { x = rgfl[0]; y = rgfl[1]; z = rgfl[2]; }

    inline int operator==(const Vector &v) const { return x == v.x && y == v.y && z == v.z; }
    inline int operator!=(const Vector &v) const { return !(*this == v); }
    inline Vector operator+(const Vector &v) const { return Vector(x + v.x, y + v.y, z + v.z); }
    inline Vector operator-(const Vector &v) const { return Vector(x - v.x, y - v.y, z - v.z); }
    inline Vector operator*(float fl) const { return Vector(x * fl, y * fl, z * fl); }
    inline Vector operator/(float fl) const { return Vector(x / fl, y / fl, z / fl); }
    inline Vector operator-(void) const { return Vector(-x, -y, -z); }

    inline void CopyToArray(float *rgfl) const { rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; }
    inline float Length(void) const { return sqrtf(x * x + y * y + z * z); }
    operator float *() { return &x; }
    operator const float *() const { return &x; }
    inline Vector Normalize(void) const
    {
        float flLen = Length();
        if (flLen == 0)
            return Vector(0, 0, 1);
        flLen = 1 / flLen;
        return Vector(x * flLen, y * flLen, z * flLen);
    }

    inline Vector2D Make2D(void) const
    {
        Vector2D Vec2;

        Vec2.x = x;
        Vec2.y = y;

        return Vec2;
    }
    inline float Length2D(void) const { return sqrtf(x * x + y * y); }

    vec_t x, y, z;
};

inline Vector operator*(float fl, const Vector &v) { return v * fl; }
inline float DotProduct(const Vector &a, const Vector &b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }
inline Vector CrossProduct(const Vector &a, const Vector &b) { return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

#endif
