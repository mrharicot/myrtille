#include "file_tools.h"

void write_ppm(std::vector<float3> &image, int height, int width, std::string &file_path)
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
        }
    }


    file.close();

    std::cout << "Wrote a " << width << " by " << height << " image." << std::endl;
}
