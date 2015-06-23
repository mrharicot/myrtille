#ifndef CAMERA_H
#define CAMERA_H

#include "math_tools.h"

class Camera
{
public:
    Camera() : m_position(0.0f), m_orientation(eye()), m_fov(0.785398163f), m_width(512), m_height(512) {}
    //Camera(float fov, float aspec_ratio) : m_fov(fov), m_aspect_ratio(aspec_ratio) {}

    inline void          position(const float3& position)      { m_position = position; }
    inline const float3& position(void) const                  { return m_position; }
    inline void          orientation(const mat3f& orientation) { m_orientation = orientation; }
    inline const mat3f&  orientation(void) const               { return m_orientation; }
    inline void          fov(float fov)                        { m_fov = fov; }
    inline const float   fov(void) const                       { return m_fov; }
    inline const float   focal() const                         { return 0.5f * m_height / std::tan(0.5f * m_fov); }



private:
    float3 m_position;
    mat3f  m_orientation;

    float m_fov;
    int   m_width, m_height;
};

#endif
