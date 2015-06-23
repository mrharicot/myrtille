#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <random>

#include "string_tools.h"
#include "math_tools.h"
#include "mesh.h"
#include "geometry.h"
#include "camera.h"

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
    int width  = 128;
    int height = width;
    int nb_ao_samples = 16;

    std::vector<float3> image;
    image.reserve(height * width);

    Mesh mesh = read_ply("cornell_box.ply");

    //triangle t(float3(0.0f, 0.0f, -5.0f), float3(1.0f, 0.0f, -5.0f), float3(0.0f, 1.0f, -5.0f));
    // ray r(float3(0.f, 0.f, 0.f), float3(0.0f, 0.0f, -1.0f));
    //
    // auto bob = t.intersect(r);

    //Camera camera();

    float fov   = 39.3076f * pi / 180.0f;
    float focal = 0.5f * height / std::tan(0.5f * fov);

    float3 origin(278.0f, 273.0f, -800.0f);
    //float3 origin(0.0f, 0.0f, 10.0f);
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

            Hit hit = mesh.intersect(r);

            //std::cout << "(" << i << "," << j << ") hit " << hit.did_hit << ", " << hit.t << ", " << hit.face_id << std::endl;

            //float3 out(float(hit.did_hit));
            if (!hit)
            {
                image.push_back(float(hit));
                continue;
            }

            float3 p = r.origin + r.direction * hit.t;
            float3 n(mesh.face(hit.face_id).normal());
            float3 upvec = n.z < 0.9f ? float3(0.0,0.0,1.0) : float3(1.0,0.0,0.0);

            float3 k = upvec.cross(n);
            float ct = upvec.dot(n);
            float st = k.norm();
            k = k / st;

            float ao_grid_step = 1.0f / nb_ao_samples;

            float ao_value = 0.0f;
            for (int ii = 0; ii < nb_ao_samples; ++ii)
            {
                for (int jj = 0; jj < nb_ao_samples; ++jj)
                {
                    float r1, r2;
                    r1 = ao_grid_step * (ii + std::rand() / RAND_MAX);
                    r2 = ao_grid_step * (jj + std::rand() / RAND_MAX);
                    float3 v(std::cos(2.0f * pi * r1) * std::sqrt(1.0f - r2), std::sin(2.0f * pi * r1) * std::sqrt(1.0f - r2), std::sqrt(r2));

                    float3 rv = v * ct + k.cross(v) * st + k * k.dot(v) * (1.0f - ct);
                    //float3 rv = n;

                    ray ao_r(p + n, rv);

                    Hit ao_hit = mesh.intersect(ao_r, 0.0f, 5000.0f);
                    if (ao_hit)
                        ao_value += 1.0f;


                }
            }

            ao_value /= (nb_ao_samples * nb_ao_samples);
            //std:: cout << ao_value << std::endl;


            float dp = std::max(-r.direction.dot(n), 0.0f);
            float3 out(1.0f - ao_value);
            image.push_back(out);

        }
    }


    write_ppm(image, height, width, std::string("out.ppm"));

    return 0;
}
