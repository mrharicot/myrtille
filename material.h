#ifndef MATERIAL_H
#define MATERIAL_H

#include "math_tools.h"

class Material
{
public:
    Material() : name("default"), albedo(1.0f) {}
    Material(std::string name, float3 a) : name(name), albedo(a) {}

    std::string name;
    float3 albedo;

     friend std::ostream& operator<<(std::ostream& os, const Material &m);

private:

};

#endif // MATERIAL_H
