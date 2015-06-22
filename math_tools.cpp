#include "math_tools.h"

std::ostream& operator<<(std::ostream& os, const float3 &f)
{
  os << f.x << ", " << f.y << ", " << f.z;
  return os;
}

mat3f eye(void) {
  mat3f out(0.0f);
  out.data[0] = 1.0f;
  out.data[4] = 1.0f;
  out.data[8] = 1.0f;
  return out;
}