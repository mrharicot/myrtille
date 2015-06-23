#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
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

    std::cout << "Wrote an " << width << " by " << height << " image." << std::endl;
}

int main()
{
    int width  = 512;
    int height = width;

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

            std::pair<bool, float> hit;
            int face_id = -1;
            triangle t;
            float min_depth = 1e20f;
            for (int k = 0; k < mesh.nb_faces(); ++k)
            {
                t = mesh.face(k);
                hit = t.intersect(r);
                if (hit.first && hit.second < min_depth)
                {
                    face_id = k;
                    min_depth = hit.second;
                }

            }



            float3 out(hit.first);
            if (face_id != -1)
            {
                float dp = std::max(-r.direction.dot(mesh.face(face_id).normal()), 0.0f);
                out = float3(dp);
            }

            image.push_back(out);
        }
    }


    write_ppm(image, height, width, std::string("out.ppm"));

    return 0;
}
