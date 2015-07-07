#include "math_tools.h"

std::ostream& operator<<(std::ostream& os, const int3 &i)
{
    os << i.x << ", " << i.y << ", " << i.z;
    return os;
}

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

float randf()
{
    return std::rand() * rand_factor;
}

float wrap(const float f)
{
    return f > 1.0f ? f - 1.0f : f;
}
