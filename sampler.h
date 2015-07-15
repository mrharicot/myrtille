#ifndef SAMPLER_H
#define SAMPLER_H

#include <vector>
#include <cmath>
#include <random>
#include <limits>

#include "math_tools.h"

class Sampler
{

public:
    enum Method {RANDOM, SOBOL};

    Sampler() {}
    Sampler(int spp, int dim, int height, int width, Method method) : m_spp(spp), m_dim(dim), m_height(height), m_width(width) { generate_samples(method); }

    void generate_samples(Method method);
    void generate_samples_sobol();
    void generate_samples_random();

    void generate_offsets();

    inline float get(int n, int i, int j, int d)
    {
        return wrap(m_samples[n * m_dim + d] + m_offsets[(m_width * i + j) * m_dim + d]);
    }

private:
    int m_spp, m_dim;
    int m_height, m_width;
    std::vector<float> m_samples;
    std::vector<float> m_offsets;
};

#endif // SAMPLER_H
