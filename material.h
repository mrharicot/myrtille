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
    Material() : name("default"), albedo(1.0f) {}
    Material(std::string name, float3 a) : name(name), albedo(a) {}

    std::string name;
    float3 albedo;
    float emission;

    friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
