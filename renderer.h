#ifndef RENDERER_H
#define RENDERER_H

#include "bvh.h"
#include "camera.h"
#include "sampler.h"

class Renderer
{
public:
    Renderer(int height, int width, int spp, int max_nb_bounces) : m_height(height), m_width(width), m_spp(spp), m_max_nb_bounces(max_nb_bounces)
    {
        m_sampler = Sampler(m_spp, 2 + 4 * m_max_nb_bounces, m_height, m_width, Sampler::SOBOL);
        m_image.resize(m_height * m_width, float3(0.0f));
        m_verbose = true;
        m_scene_epsilon = 1e-3f;
    }

    inline void set_camera(Camera &camera) { m_camera = &camera; }
    inline void set_mesh(Mesh &mesh)       { m_mesh = &mesh; m_bvh = BVH(m_mesh); }

    void render();
    float3 sample_ray(ray r, int sp, int sample_id, int i, int j);
    std::vector<float3> &get_image() { return m_image; }

private:
    int m_height, m_width;
    int m_spp, m_max_nb_bounces;

    std::vector<float3> m_image;

    BVH     m_bvh;
    Mesh   *m_mesh;
    Camera *m_camera;
    Sampler m_sampler;

    bool    m_verbose;
    float   m_scene_epsilon;
};

#endif // RENDERER_H
