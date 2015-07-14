#ifndef MATERIAL_H
#define MATERIAL_H

#include "math_tools.h"

struct Light
{
    Light() : face_id(-1), color(1.0f), intensity(0.0f) {}
    Light(int face_id, float3 color, float intensity) : face_id(face_id), color(color), intensity(intensity) {}

    int    face_id;
    float3 color;
    float  intensity;
};

class Material
{
public:
    Material() : name("default"), color(1.0f), emission(0.0f) {}
    Material(std::string name, float3 color, float emission = 0.0f) : name(name), color(color), emission(emission) {}

    std::string name;
    float3 color;
    float emission;

    inline bool is_emissive(void) { return emission > 0.0f; }

    friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
