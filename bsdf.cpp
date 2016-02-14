//
// Created by clement on 14/02/16.
//

#include "bsdf.h"

float3 BSDF::sample(float r1, float r2, float r) {

    if (r > material->metallic)
        sample_type = DIFFUSE;
    else
        sample_type = SPECULAR;

    if (sample_type == DIFFUSE)
        return  sample_diffuse(r1, r2);
    else if (sample_type == SPECULAR)
        return sample_specular(r1, r2);

    std::cerr << "problem with bsdf sampling" << std::endl;
    return float3();
}

float3 BSDF::evaluate() {
    return float3();
}

float BSDF::probability() {
    return 0;
}

float3 BSDF::sample_diffuse(float r1, float r2) {

    float sq = std::sqrt(1.0f - r2);

    return frame.to_world(
            float3(std::cos(2.0f * pi * r1) * sq,
                   std::sin(2.0f * pi * r1) * sq,
                   std::sqrt(r2)));
}

float3 BSDF::sample_specular(float r1, float r2) {
    float theta = std::atan(material->roughness * material->roughness * std::sqrt(r1) / std::sqrt(1.0f - r1));
    float phi   = 2.0f * pi * r2;

    return reflect(i, frame.to_world(
            float3(std::sin(theta) * std::cos(phi),
                   std::sin(theta) * std::sin(phi),
                   std::cos(theta))));
}
