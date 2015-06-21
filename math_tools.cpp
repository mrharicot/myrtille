#include "math_tools.h"

mat3f eye(void) {
  mat3f out(0.0f);
  out.data[0] = 1.0f;
  out.data[4] = 1.0f;
  out.data[8] = 1.0f;
  return out;
}
