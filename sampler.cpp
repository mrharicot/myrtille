#include "sampler.h"

void Sampler::generate_samples()
{
    int nb_total_samples = m_spp * m_spp * m_dim;
    //m_samples.reserve(nb_total_samples);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (int i = 0; i < nb_total_samples; ++i)
    {
        m_samples.push_back(dis(gen));
        //m_samples.push_back(0.3f);
        //m_samples.push_back(std::rand() / (float) RAND_MAX);
    }

    int nb_offsets = m_width * m_height * m_dim;
    //m_offsets.reserve(nb_offsets);
    for (int i = 0; i < nb_offsets; ++i)
    {
        m_offsets.push_back(dis(gen));
    }
}
