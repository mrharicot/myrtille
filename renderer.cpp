#include "renderer.h"

void Renderer::render()
{

    float focal = m_height * m_camera->focal();
    int it_done = 0;
    int previous_percent = 0;

#pragma omp parallel for shared(it_done, previous_percent) num_threads(12) schedule(dynamic, 2)
    for (int i = 0; i < m_height; ++i)
    {
        for (int j = 0; j < m_width; ++j)
        {
            float3 color(0.0f);

            for (int si = 0; si < m_spp; ++si)
            {
                float dx = m_sampler.get(si, i, j, 0);
                float dy = m_sampler.get(si, i, j, 1);

                float3 direction;
                direction.x = (j + dx - 0.5f * m_width)  / focal;
                direction.y = (0.5f * m_height - i - dy) / focal;
                direction.z = -1.0f;
                direction.normalize();

                direction = m_camera->orientation().dot(direction);

                ray r(m_camera->position(), direction);

                float3 path_color = sample_ray(r, 0, si, i, j);

                color += path_color;
            }

            if (m_verbose)
            #pragma omp critical
            {
                it_done += 1;

                int percent_done = std::floor(10 * it_done / (m_height * m_width));
                if (percent_done - previous_percent == 1 )
                {
                    std::cout << 10 * percent_done << "% done" << std::endl;
                    previous_percent = percent_done;
                }
            }

            color /= m_spp;

            m_image.at(i * m_width + j) = color ^ (1.0f / 2.2f);

        }

    }

}

float3 Renderer::sample_ray(ray r, int sp, int sample_id, int i, int j)
{
    float3 out_color(0.0f);

    if (sp >= m_path_depth)
        return out_color;

    float t_max = 1e10f;
    Hit hit = m_bvh.intersect(r, t_max);

    if (!hit)
    {
        return out_color;
    }

    if (sp == 0)
    {
        out_color += m_mesh->face_material(hit.face_id).emission;
    }

    float3 p = r.origin + r.direction * hit.t;

    float3 n = m_mesh->face_normal(hit.face_id, p);

    float r1_light = m_sampler.get(sample_id, i, j, 2 + 5 * sp + 0);
    float r2_light = m_sampler.get(sample_id, i, j, 2 + 5 * sp + 1);
    float  r1_path = m_sampler.get(sample_id, i, j, 2 + 5 * sp + 2);
    float  r2_path = m_sampler.get(sample_id, i, j, 2 + 5 * sp + 3);


    int e_face_id = m_mesh->emissive_face_index((int) round((m_mesh->nb_emissive_faces() - 1) * r1_light));

    float3 light_point = m_mesh->triangle(e_face_id).sample_point(r1_light, r2_light);
    float3 pa = p + n * m_scene_epsilon;
    float3 e_n = m_mesh->face_normal(e_face_id, light_point);
    float3 pb = light_point + e_n * m_scene_epsilon;
    float3 light_color = m_mesh->face_material(e_face_id).emission;

    float3 light_dir   = pb - pa;
    float light_t_max = light_dir.norm();
    light_dir /= light_t_max;
    ray sr(pa, light_dir);

    bool       viz = !m_bvh.visibility(sr, light_t_max);
    float light_dp = std::max(light_dir.dot(n), 0.0f);

    float3 face_color = m_mesh->face_material(hit.face_id).color;

    float3 reflection_direction = sample_around_normal(n, r1_path, r2_path);
    float  reflection_dp = reflection_direction.dot(n);
    float  light_pdf     = 1.0f / ( m_mesh->nb_emissive_faces() * m_mesh->area(e_face_id));
    float  G             = m_mesh->face_normal(e_face_id, pb).dot(light_dir * -1.0f) * light_dp / (light_t_max * light_t_max);

    float3 light_contribution = float3(viz) * face_color * light_dp * light_color * G / light_pdf;
    float3 reflection_pdf = reflection_dp / pi;

    float3 reflection_contribution = sample_ray(ray(pa, reflection_direction), sp + 1, sample_id, i, j) * face_color;

    out_color += light_contribution;

    float  russian_roulette = m_sampler.get(sample_id, i, j, 2 + 5 * sp + 4);

    if (russian_roulette > 0.5f)
        out_color += reflection_contribution * 2.0f;

    return out_color;

}
