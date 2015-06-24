#ifndef MATH_TOOLS_H
#define MATH_TOOLS_H

#include <iostream>
#include <cstdlib>
#include <cmath>

#define pi          3.14159265359
#define rand_factor 1.0 / RAND_MAX

struct float3
{
  float3() {}
  float3(float s) : x(s), y(s), z(s) {}
  float3(float x, float y, float z) : x(x), y(y), z(z) {}
  float3(const float* p) { x = p[0]; y = p[1]; z = p[2]; }
  float x, y, z;

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
};

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
