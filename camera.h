#ifndef CAMERA_H
#define CAMERA_H

#include "math_tools.h"

class Camera
{
public:
  Camera() { m_position = float3(0.0f); m_orientation = eye(); };
  Camera(float fov, float aspec_ratio) : m_fov(fov), m_aspect_ratio(aspec_ratio) {}

  inline void          position(const float3& position)      { m_position = position; }
  inline const float3& position(void) const                  { return m_position; }
  inline void          orientation(const mat3f& orientation) { m_orientation = orientation; }
  inline const mat3f&  orientation(void) const               { return m_orientation; }
  inline void          fov(float fov)                        { m_fov = fov; }
  inline const float   fov(void) const                       { return m_fov; }
  inline void          aspect_ratio(float aspect_ratio)      { m_aspect_ratio = aspect_ratio; }
  inline const float   aspect_ratio(void) const              { return m_aspect_ratio; }


private:
  float3 m_position;
  mat3f  m_orientation;

  float m_fov;
  float m_aspect_ratio;
};

#endif
