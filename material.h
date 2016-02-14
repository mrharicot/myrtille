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
          albedo(1.0f),
          metallic(0.0f),
          roughness(0.05f),
          emission(0.0f),
          is_emissive(false) {}

    Material(std::string name,
             float3 albedo,
             float  metallic,
             float  roughness,
             float3 emission = 0.0f)
        : name(name),
          albedo(albedo),
          metallic(metallic),
          roughness(roughness),
          emission(emission)
    {
        is_emissive = emission.norm() > 0.0f;
    }

    std::string name;
    float3 albedo;
    float  metallic;
    float  roughness;
    float3 emission;
    bool is_emissive;

    friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
