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
#include "bvh.h"
#include "sampler.h"
#include "renderer.h"

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
            out_c.r = (uchar) std::round(std::min(c.x, 1.0f) * 255);
            out_c.g = (uchar) std::round(std::min(c.y, 1.0f) * 255);
            out_c.b = (uchar) std::round(std::min(c.z, 1.0f) * 255);
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
    int width  = 512;
    int height = width;
    int spp = 32;
    int path_depth = 4;

    std::string filename = "cornell_box.obj";
    Mesh mesh = read_obj(filename.c_str());


    float fov   = 39.3076f * pi / 180.0f;

    float3 origin(0.0f);

    origin = float3(0.278f, 0.273f, -0.8f) * 1000.0f;
    //origin = float3(0,0.1,0.1);
    //origin = float3(-8,1,14);

    //origin = float3(0.0f, 0.05f, 0.05f);

    mat3f R(0.0f);
    R(0,0) = -1.0f;
    R(1,1) =  1.0f;
    R(2,2) = -1.0f;

    Camera camera(origin, R, fov);


    Renderer renderer(height, width, spp, path_depth);
    renderer.set_mesh(mesh);
    renderer.set_camera(camera);

    Timer timer;

    std::cout << "done in " << timer.elapsed(1) * 1e-6 << "s." << std::endl;

    renderer.render();


    std::cout << timer.elapsed() / 1e6f << "s elapsed." << std::endl;

    write_ppm(renderer.get_image(), height, width, std::string("out.ppm"));

    //std::cout << "converting to png" << std::endl;

    //std::system("/usr/local/bin/convert out.ppm out.png");

    return 0;


}
