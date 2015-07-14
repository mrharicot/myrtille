#ifndef MATERIAL_H
#define MATERIAL_H

#include "math_tools.h"

struct Light
{
    Light() : face_id(-1), emission(1.0f) {}
    Light(int face_id, float3 color) : face_id(face_id), emission(emission) {}

    int    face_id;
    float3 emission;
};

class Material
{
public:
    Material() : name("default"), color(1.0f), emission(0.0f), is_emissive(false) {}
    Material(std::string name, float3 color, float3 emission = 0.0f) : name(name), color(color), emission(emission)
    {
        is_emissive = emission.norm() > 0.0f;
    }

    std::string name;
    float3 color;
    float3 emission;
    bool is_emissive;

    friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
