#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <cstdlib>

#include "string_tools.h"
#include "math_tools.h"
#include "mesh.h"
#include "geometry.h"
#include "camera.h"
#include "time_tools.h"

typedef unsigned char uchar;

struct color {uchar r; uchar g; uchar b;};


void write_ppm(std::vector<float3> &image, int height, int width, std::string file_path)
{

    std::ofstream file(file_path.c_str(), std::ofstream::out | std::ofstream::binary);

    file << "P6 " << std::to_string(width) << " "   << std::to_string(height) << " 255\n";

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            float3 c = image.at(width * i + j);
            color out_c;
            out_c.r = (uchar) std::round(c.x * 255);
            out_c.g = (uchar) std::round(c.y * 255);
            out_c.b = (uchar) std::round(c.z * 255);
            file.write((const char*) &out_c.r, sizeof(color));
            //std::cout << out_c.r << out_c.g << out_c.b << std::endl;
        }
        //file << "\n";
    }


    file.close();

    std::cout << "Wrote a " << width << " by " << height << " image." << std::endl;
}

int main()
{
    int width  = 1024;
    int height = width;
    int nb_ao_samples = 8;

    std::vector<float3> image;
    image.resize(height * width);

    std::string filename = "bunny.ply";
    Mesh mesh = read_ply(filename.c_str());

    //triangle t(float3(0.0f, 0.0f, -5.0f), float3(1.0f, 0.0f, -5.0f), float3(0.0f, 1.0f, -5.0f));
    // ray r(float3(0.f, 0.f, 0.f), float3(0.0f, 0.0f, -1.0f));
    //
    // auto bob = t.intersect(r);

    //Camera camera();

    float fov   = 39.3076f * pi / 180.0f;
    float focal = 0.5f * height / std::tan(0.5f * fov);

    float3 origin;
    if (filename == "cornell_box.ply")
    {
        origin = float3(0.278f, 0.273f, -0.8f);
    }
    else
    {
        origin = float3(0.0f, 0.0f, -2.0f);
    }


    int it_done = 0;
    int previous_percent = 0;



    //generate ao_samples
    std::vector<float> ao_samples;
    ao_samples.reserve(nb_ao_samples * nb_ao_samples);

    std::random_device rd;
    std::mt19937 gen(rd());
    for (int i = 0; i < nb_ao_samples * nb_ao_samples * 2; ++i)
    {
        ao_samples.push_back(std::generate_canonical<float, std::numeric_limits<float>::digits>(gen));
    }

    std::vector<float> pixel_offsets;
    pixel_offsets.reserve(width * height * 2);
    for (int i = 0; i < width * height * 2; ++i)
    {
        pixel_offsets.push_back(std::generate_canonical<float, std::numeric_limits<float>::digits>(gen));
    }

    float scene_epsilon = 1e-6f;

    Node root;
    //    std::vector<const Triangle*>& faces
    //    for (int i = 0; i < mesh.nb_faces(); ++i)
    //    {
    //        root.faces().push_back(mesh.face(i));
    //    }

    root.id = 0;
    root.start_index = 0;
    root.end_index   = mesh.nb_faces();
    build_tree(mesh.faces(), &root);

    ray r(origin, float3(0,0,1));

    //float t_min = 0;
    //Hit hit = root.intersect(mesh.faces(), r, t_min);
    //std::cout << hit.did_hit << " - " << hit.t << std::endl;
    //return 0;


    Timer timer;

    //#pragma omp parallel for shared(it_done, previous_percent) num_threads(8)
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            float3 direction;
            direction.x = (j + 0.5f - 0.5f * width)  / focal;
            direction.y = (0.5f * height - i - 0.5f) / focal;
            direction.z = 1.0f;
            direction.normalize();

            ray r(origin, direction);

            float t_min = 0.0f;
            Hit hit = root.intersect(mesh.faces(), r, t_min);

            if (!hit)
            {
                image.at(i * width + j) = float(hit);
                continue;
            }

            float3 p = r.origin + r.direction * hit.t;
            float3 n(hit.face->normal());

            bool zup = std::abs(n.z) < 0.9f;

            float3 upvec = zup ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f ,0.0f ,0.0f);

            float3 k = upvec.cross(n);
            float ct = upvec.dot(n);
            float st = k.norm();
            k = k / st;
    /*

            float ao_grid_step = 1.0f / nb_ao_samples;

            float ao_value = 0.0f;

            for (int ii = 0; ii < nb_ao_samples; ++ii)
            {
                for (int jj = 0; jj < nb_ao_samples; ++jj)
                {
                    float r1 = wrap(ao_grid_step * (ii + ao_samples[ii * nb_ao_samples * 2 + jj * 2 + 0]) + pixel_offsets[i * width * 2 + j * 2 + 0]);
                    float r2 = wrap(ao_grid_step * (jj + ao_samples[ii * nb_ao_samples * 2 + jj * 2 + 1]) + pixel_offsets[i * width * 2 + j * 2 + 1]);

                    float3 v;

                    float sq = std::sqrt(1.0f - r2);

                    if (zup)
                    {
                        v = float3(std::cos(2.0f * pi * r1) * sq,
                                   std::sin(2.0f * pi * r1) * sq,
                                   std::sqrt(r2));
                    }
                    else
                    {
                        v = float3(std::sqrt(r2),
                                   std::cos(2.0f * pi * r1) * sq,
                                   std::sin(2.0f * pi * r1) * sq);
                    }

                    float3 rv = v * ct + k.cross(v) * st + k * k.dot(v) * (1.0f - ct);

                    ray ao_r(p + n * scene_epsilon, rv);

                    float ao_sigma = 0.1f;
                    float t_min = 0.0f;
                    Hit ao_hit = root.intersect(mesh.faces(), ao_r, t_min);//, 0.0f, 3.0f * ao_sigma);
                    if (ao_hit)
                        ao_value += std::exp(-0.5f * ao_hit.t * ao_hit.t / (ao_sigma * ao_sigma));


                }
            }

            ao_value /= (nb_ao_samples * nb_ao_samples);

*/

            float dp = std::max(-r.direction.dot(n), 0.0f);
            //float3 out(1.0f - ao_value);
            float3 out(dp);
            image.at(i * width + j) = out;

            it_done += 1;
            int percent_done = std::floor(100 * it_done / ((float) (height - 1) * (width - 1)));
            if (percent_done - previous_percent == 1 )
            {
                std::cout << percent_done << "% done" << std::endl;
                previous_percent = percent_done;
            }

        }

    }





    std::cout << timer.elapsed() / 1e6f << "s elapsed." << std::endl;

    write_ppm(image, height, width, std::string("out.ppm"));

    std::cout << "converting to png" << std::endl;

    std::system("/usr/local/bin/convert out.ppm out.png");

    return 0;
}
