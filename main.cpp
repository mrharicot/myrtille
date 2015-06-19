#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

typedef unsigned char uchar;
typedef unsigned int  uint;

struct color {uchar r; uchar g; uchar b;};
struct vec3f {float x; float y; float z;};

std::vector<std::string> split_whitespaces(std::string str)
{
    std::stringstream ss(str);
    std::string       buf;

    std::vector<std::string> tokens;

    while (ss >> buf)
        tokens.push_back(buf);

    return tokens;
}

class Mesh {
public:
    Mesh() {}
    Mesh(std::vector<float> vertices, std::vector<uint> triangles)
        : m_vertices(vertices), m_triangles(triangles) {}

    void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    void set_triangles(std::vector<uint> triangles) {m_triangles = triangles;}

private:
    std::vector<float> m_vertices;
    std::vector<uint>  m_triangles;

};

Mesh read_ply(const char* file_path)
{
    enum Read_mode {ASCII, BINARY};

    Mesh mesh;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<uint>  triangles;

    std::ifstream file(file_path, std::ifstream::in | std::ifstream::binary);

    std::string line;
    std::vector<std::string> tokens;

    Read_mode read_mode;

    std::getline(file, line);
    if (line != "ply")
    {
        std::cout << "File is not ply" << std::endl;
        file.close();
        return mesh;
    }

    std::getline(file, line);
    tokens = split_whitespaces(line);
    if (tokens[1] == "ascii")
    {
        read_mode = ASCII;
    }
    else if (tokens[1] == "binary_little_endian")
    {
        read_mode = BINARY;
    }

    int nb_verts = 0;
    int nb_triangles = 0;
    std::vector<std::string> vert_properties;

    while (std::getline(file, line))
    {
        tokens = split_whitespaces(line);

        if (tokens[0] == "end_header")
            break;

        if (tokens[0] == "comment")
            continue;

        if (tokens[0] == "element")
        {
            if (tokens[1] == "vertex")
            {
                nb_verts = std::stoi(tokens[2]);
            }
            else if (tokens[1] == "face")
            {
                nb_triangles = std::stoi(tokens[2]);
            }
        }

        if (tokens[0] == "property")
        {
            if (nb_verts && !nb_triangles)
            {
                vert_properties.push_back(tokens[2]);
            }
            else
            {
                if (tokens[2] != "uchar" && tokens[3] != "int")
                    std::cout << "list type not supported yet" << std::endl;
            }
        }
    }

    //read vertices
    vertices.reserve(3 * nb_verts);
    normals.reserve (3 * nb_verts);
    float buf[6 * nb_verts];
    file.read((char*) buf, 6 * nb_verts * sizeof(float) / sizeof(char));

    for (int i = 0; i < nb_verts; ++i)
    {
        vertices.push_back(buf[6 * i + 0]);
        vertices.push_back(buf[6 * i + 1]);
        vertices.push_back(buf[6 * i + 2]);

        normals.push_back(buf[6 * i + 3]);
        normals.push_back(buf[6 * i + 4]);
        normals.push_back(buf[6 * i + 5]);
    }
    //vertices.insert()

    file.close();

}

void write_ppm(std::vector<vec3f> &image, int height, int width, std::string file_path)
{

    std::ofstream file(file_path.c_str(), std::ofstream::out | std::ofstream::binary);

    file << "P6 " << std::to_string(width) << " "   << std::to_string(height) << " 255\n";

    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            vec3f c = image.at(width * i + j);
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
    uint width  = 512;
    uint height = width;

    std::vector<vec3f> image;
    image.reserve(height * width);


    Mesh mesh = read_ply("mesh.ply");

    //write_ppm(image, height, width, std::string("out.ppm"));

    return 0;
}
