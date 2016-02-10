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
#include "file_tools.h"

int main(int argc, char** argv)
{
    if (argc != 4)
    {
      std::cerr << "Usage: " << argv[0] << " [filename] [image width] [spp]"<< std::endl;
      return 1;
    }

    std::string filename = std::string(argv[1]);
    Mesh mesh = read_obj(filename.c_str());

    int width  = std::atoi(argv[2]);
    int height = width;
    int spp = std::atoi(argv[3]);
    //spp *= spp;
    int path_depth = 15;

    float fov   = 39.3076f * pi / 180.0f;
    //float fov   = 60.0f * pi / 180.0f;

    float3 origin(0.0f);

    origin = float3(0.278f, 0.273f, -0.8f) * 1000.0f;
    //origin = float3(0,0.1,0.1);
    //origin = float3(-8,1,14);

    //origin = float3(0.0f, 91.0f, -50.0f);

    mat3f R(0.0f);
    R(0,0) = -1.0f;
    R(1,1) =  1.0f;
    R(2,2) = -1.0f;

    Camera camera(origin, R, fov);

    Timer timer;

    Renderer renderer(height, width, spp, path_depth);

    renderer.set_mesh(mesh);

    renderer.set_camera(camera);

    std::cout << "done in " << timer.elapsed(1) * 1e-6 << "s." << std::endl;

    renderer.render();

    std::cout << timer.elapsed() / 1e6f << "s elapsed." << std::endl;

    std::string out_file("out.ppm");
    write_ppm(renderer.get_image(), height, width, out_file);

    return 0;


}
