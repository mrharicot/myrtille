//
// Created by clement on 14/02/16.
//

#ifndef MYRTILLE_BSDF_H
#define MYRTILLE_BSDF_H

#include "math_tools.h"
#include "geometry.h"
#include "material.h"

class BSDF {
    enum Sample_type {DIFFUSE, SPECULAR};
public:
    BSDF(const ray& r, const float3& n, Material *mat) : i(-r.direction), n(n), frame(Frame(n)), material(mat) {
    }

    float3 sample(float r1, float r2, float r);
    float3 evaluate();
    float  probability();


private:

    float3  sample_diffuse(float r1, float r2);
    float3 sample_specular(float r1, float r2);

    float3       i;
    float3       n;
    Frame        frame;
    Material    *material;
    Sample_type  sample_type;
};


#endif //MYRTILLE_BSDF_H
