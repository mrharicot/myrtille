#include "geometry.h"

#define EPSILON 1e-6f

//from wikipedia
std::pair <bool, float> triangle::intersect(const ray &r)
{
  float3 e0 = v1 - v0;
  float3 e1 = v2 - v0;

  float3 p = r.direction.cross(e1);

  float det = e0.dot(p);

  if(det > -EPSILON && det < EPSILON)
    return std::make_pair(false, 0.0f);

  float inv_det = 1.0f / det;

  float3 t = r.origin - v0;

  float u = t.dot(p) * inv_det;

  if(u < 0.0f || u > 1.0f)
    return std::make_pair(false, 1.0f);

  float3 q = t.cross(e0);

  float v = r.direction.dot(q) * inv_det;

  if(v < 0.0f || u + v  > 1.0f)
    return std::make_pair(false, 2.0f);

  float d = e1.dot(q) * inv_det;

  if(d > EPSILON)
    return std::make_pair(true, d);

  return std::make_pair(false, 3.0f);
}
