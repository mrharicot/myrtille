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
                for (int sj = 0; sj < m_spp; ++sj)
                {
                    int sample_id = m_spp * si + sj;

                    float dx = (si + m_sampler.get(sample_id, i, j, 0)) / m_spp;
                    float dy = (sj + m_sampler.get(sample_id, i, j, 1)) / m_spp;

                    float3 direction;
                    direction.x = (j + dx - 0.5f * m_width)  / focal;
                    direction.y = (0.5f * m_height - i - dy) / focal;
                    direction.z = -1.0f;
                    direction.normalize();

                    direction = m_camera->orientation().dot(direction);

                    ray r(m_camera->position(), direction);

                    float t_max = 1e10f;
                    Hit hit = m_bvh.intersect(r, t_max);


                    if (m_verbose)
                    #pragma omp critical
                    {
                        it_done += 1;

                        int percent_done = std::floor(10 * it_done / (m_height * m_width * m_spp * m_spp));
                        if (percent_done - previous_percent == 1 )
                        {
                            std::cout << 10 * percent_done << "% done" << std::endl;
                            previous_percent = percent_done;
                        }
                    }

                    if (!hit)
                    {
                        color += float(0.0f);
                        continue;
                    }

                    float3 p = r.origin + r.direction * hit.t;

                    float3 n = m_mesh->face_normal(hit.face_id, p);



                    bool zup = std::abs(n.z) < 0.9f;

                    float3 upvec = zup ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f ,0.0f ,0.0f);

                    float3 k = upvec.cross(n);
                    float ct = upvec.dot(n);
                    float st = k.norm();
                    k = k / st;


                    //int e_face_id = m_mesh->emissive_face_index(std::rand() % m_mesh->nb_emissive_faces());
                    int e_face_id = m_mesh->emissive_face_index((int) round((m_mesh->nb_emissive_faces() - 1) * m_sampler.get(sample_id, i, j, 2)));
                    //int e_face_id = 6;

                    //std::cout << e_face_id << std::endl;

                    //Face& light_face = mesh.face(e_face_id);
                    float3 light_point = m_mesh->triangle(e_face_id).sample_point(m_sampler.get(sample_id, i, j, 2), m_sampler.get(sample_id, i, j, 3));
                    //float3 light_point = m_mesh->triangle(e_face_id).sample_point(std::rand() / (float) RAND_MAX, std::rand() / (float) RAND_MAX);
                    float3 pa = p + n * m_scene_epsilon;
                    float3 e_n = m_mesh->face_normal(e_face_id, light_point);
                    float3 pb = light_point + e_n * m_scene_epsilon;
                    bool viz = !m_bvh.visibility(pa, pb);
                    float dp = (pb-pa).normalized().dot(n);

                    //float3 pixel_value = float3(std::max(-r.direction.dot(n), 0.0f));
                    float3 pixel_value((float) viz);

                    float3 face_color = m_mesh->face_material(hit.face_id).color;

                    color += face_color * pixel_value * dp;
                }
            }

            color /= m_spp * m_spp;

            m_image.at(i * m_width + j) = color ^ (1.0f / 2.2f);

        }

    }

}
