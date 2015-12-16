#ifndef FILE_TOOLS_H
#define FILE_TOOLS_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <random>
#include <limits>
#include <cstdlib>

#include "math_tools.h"

typedef unsigned char uchar;
struct color {uchar r; uchar g; uchar b;};

void write_ppm(std::vector<float3> &image, int height, int width, std::string &file_path);

#endif // FILE_TOOLS_H
