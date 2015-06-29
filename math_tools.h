#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

#include <iostream>
#include <cstdlib>
#include <cmath>

#define pi          3.14159265359
#define rand_factor 1.0 / RAND_MAX

struct int3
{
    int3() {}
    int3(int x, int y, int z) : x(x), y(y), z(z) {}
    int3(int i) : x(i), y(i), z(i) {}
    int3(const int* p) { x = p[0]; y = p[1]; z = p[2]; }
    union { struct { int x, y, z; }; int data[3]; };
};

struct float3
{
    float3() {}
    float3(float s) : x(s), y(s), z(s) {}
    float3(float x, float y, float z) : x(x), y(y), z(z) {}
    float3(const float* p) { x = p[0]; y = p[1]; z = p[2]; }
    union { struct { float x; float y; float z; }; float data[3]; };
    //float x, y, z;

    inline float3 operator+(float s) const { return float3(s + x, s + y, s + z); }
    inline float3 operator-(float s) const { return float3(s - x, s - y, s - z); }
    inline float3 operator*(float s) const { return float3(s * x, s * y, s * z); }
    inline float3 operator/(float s) const { return float3(x / s, y / s, z / s); }

    inline float3 operator+(const float3& f) const { return float3(x + f.x, y + f.y, z + f.z); }
    inline float3 operator-(const float3& f) const { return float3(x - f.x, y - f.y, z - f.z); }
    inline float3 operator*(const float3& f) const { return float3(x * f.x, y * f.y, z * f.z); }

    inline float  dot(const float3& f)   const { return f.x * x + f.y * y + f.z * z; }
    inline float3 cross(const float3& f) const { return float3(y * f.z - z * f.y, z * f.x - x * f.z, x * f.y - y * f.x); }

    inline float  squared_norm(void) const { return dot(*this); }
    inline float  norm(void)         const { return std::sqrt(this->squared_norm()); }

    inline void   normalize(void)  { float s = this->norm(); x /= s; y /= s; z /= s; }
    inline float3 normalized(void) const { float s = this->norm(); return *this / s; }

    friend std::ostream& operator<<(std::ostream& os, const float3 &f);

    //inline float* data(void) { return (&x); }
};

inline float3 max(float3 lhs, float3 rhs) { return float3(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z)); }
inline float3 min(float3 lhs, float3 rhs) { return float3(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z)); }

inline void swap(float& a, float& b) { float t = a; a = b; b = t; }

struct mat3f
{
    mat3f() {}
    mat3f(float s) { for (int i = 0; i < 9; ++i) { data[i] = s; }; }
    float data[9]; //row major

    inline mat3f operator+(const mat3f& m) {
        mat3f out;
        for (int i = 0; i < 9; ++i)
            out.data[i] = data[i] + m.data[i];
        return out;
    }
    inline mat3f operator*(const mat3f& m) {
        mat3f out;
        for (int i = 0; i < 9; ++i)
            out.data[i] = data[i] * m.data[i];
        return out;
    }
    inline mat3f operator+(float s) {
        mat3f out;
        for (int i = 0; i < 9; ++i)
            out.data[i] = data[i] + s;
        return out;
    }
    inline mat3f operator*(float s) {
        mat3f out;
        for (int i = 0; i < 9; ++i)
            out.data[i] = data[i] * s;
        return out;
    }

    inline float3 dot(const float3& f) {
        float3 out(0.0f);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                *(&out.x + i) += data[3 * i + j] * *(&f.x + j);
        return out;
    }
    inline mat3f dot(const mat3f& m) {
        mat3f out(0.0f);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                for (int k = 0; k < 3; ++k)
                    out.data[3 * i + j] = data[3 * i + k] * m.data[3 * k + j];
        return out;
    }

};

mat3f eye(void);

float randf();

float wrap(const float f);


#endif
