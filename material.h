#ifndef MATERIAL_H
#define MATERIAL_H

#include "math_tools.h"

struct Light
{
    Light() : face_id(-1), emission(1.0f) {}
    Light(int face_id, float3 emission) : face_id(face_id), emission(emission) {}

    int    face_id;
    float3 emission;
};

//inspired by both Disney's and Epic's physically based materials
class Material
{
public:
    Material()
        : name("default"),
          base_color(1.0f),
          metallic(0.0f),
          roughness(0.2f),
          emission_color(0.0f),
          is_emissive(false) {}

    Material(std::string name,
             float3 base_color,
             float  metallic,
             float  roughness,
             float3 emission_color = 0.0f)
        : name(name),
          base_color(base_color),
          metallic(metallic),
          roughness(roughness),
          emission_color(emission_color)
    {
        is_emissive = emission_color.norm() > 0.0f;
    }

    std::string name;
    float3 base_color;
    float  metallic;
    float  roughness;
    float3 emission_color;
    bool is_emissive;

    friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
