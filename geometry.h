#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <utility>

#include "math_tools.h"

struct ray
{
  float3 origin;
  float3 direction;

  ray(float3 origin, float3 direction) : origin(origin), direction(direction) {}
};

struct triangle
{
  float3 v0;
  float3 v1;
  float3 v2;

  triangle() {}
  triangle(float3 v0, float3 v1, float3 v2) : v0(v0), v1(v1), v2(v2) {}

  inline float3 normal() { return (v1 - v0).cross(v2 - v0).normalized(); }
  
  std::pair <bool, float> intersect(ray r);
};


#endif
