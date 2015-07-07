#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>

#include "mesh.h"

#include "string_tools.h"

typedef unsigned char uchar;


Hit Mesh::intersect(const ray &r, float t_min, float t_max)
{

    std::pair<bool, float> hit;
    int face_id = -1;
    float min_depth = t_max;
    bool did_hit = false;
    for (int k = 0; k < nb_faces(); ++k)
    {
        hit = m_triangles[k].intersect(r);
        if (hit.first && hit.second < min_depth)
        {
            face_id = k;
            min_depth = hit.second;
            did_hit = true;
        }
    }
    return Hit(did_hit, min_depth, face_id);
}

Mesh read_ply(const char* file_path)
{
    enum Read_mode {ASCII, BINARY, ELSE};

    std::vector<float3> vertices;
    std::vector<float2> tex_coords;
    std::vector<float3> normals;
    std::vector<Face>   faces;

    std::ifstream file(file_path, std::ifstream::in | std::ifstream::binary);

    std::string line;
    std::vector<std::string> tokens;

    Read_mode read_mode;

    std::getline(file, line);
    if (line != "ply")
    {
        std::cout << "File is not ply" << std::endl;
        file.close();
        return Mesh();
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
    else
    {
        read_mode = ELSE;
    }

    if (read_mode == ASCII || read_mode == ELSE)
        return Mesh();

    int nb_verts = 0;
    int nb_faces = 0;
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
                nb_faces = std::stoi(tokens[2]);
            }
        }

        if (tokens[0] == "property")
        {
            if (nb_verts && !nb_faces)
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
    vertices.reserve(nb_verts);
    normals.reserve (nb_verts);
    float buff[3];

    bool has_normals = vert_properties.size() == 6;


    for (int i = 0; i < nb_verts; ++i)
    {
        file.read((char*) buff, 3 * sizeof(float) / sizeof(char));
        vertices.push_back(float3(buff));

        if (has_normals)
        {
            file.read((char*) buff, 3 * sizeof(float) / sizeof(char));
            normals.push_back(float3(buff));
        }
    }

    //std::vector<Face> faces;

    //read faces
    for (int i = 0; i < nb_faces; ++i)
    {
        uchar nb_el;
        file.read((char*) &nb_el, 1);
        if (nb_el != 3)
        {
            std::cout << "Error: " << (int) nb_el << " vertices on face " << i << std::endl;
            break;
        }

        int idxs[3];
        file.read((char*) &idxs[0], 3 * sizeof(int) / sizeof(char));
        //for (int j = 0; j < 3; ++j)

        Face face(&idxs[0], -1, &idxs[0]);
        //faces.push_back(&idxs[0]);
        faces.push_back(face);
    }

    file.close();

    std::cout << "PLY file loaded: " << nb_verts << " vertices and " << nb_faces << " faces." << std::endl;

    //    mesh.set_face_indices(faces);
    //    mesh.set_vertices(vertices);
    //    mesh.set_normals(normals);

    //return Mesh(vertices, faces, normals, has_normals);

    return Mesh(vertices, tex_coords, normals, faces);

}

Mesh read_obj(const char* file_path)
{
    std::vector<float3> vertices;
    std::vector<float2> texture_coordinates;
    std::vector<float3> normals;
    std::vector<Face>   faces;

    std::ifstream file(file_path, std::ifstream::in);

    std::string line;
    std::vector<std::string> tokens;

    while (std::getline(file, line))
    {
        tokens = split_whitespaces(line);
        if (tokens.size() == 0)
            continue;

        if (tokens[0] == "#")
            std::cout << "comment" << std::endl;

        if (tokens[0] == "v")
        {
            float3 v(std::atof(tokens[1].c_str()), std::atof(tokens[2].c_str()), std::atof(tokens[3].c_str()));
            vertices.push_back(v);
        }

        if (tokens[0] == "vt")
        {
            float2 uv(std::atof(tokens[1].c_str()), std::atof(tokens[2].c_str()));
            texture_coordinates.push_back(uv);
        }

        if (tokens[0] == "vn")
        {
            float3 n(std::atof(tokens[1].c_str()), std::atof(tokens[2].c_str()), std::atof(tokens[3].c_str()));
            normals.push_back(n);
        }

        if (tokens[0] == "f")
        {
            std::vector<std::string> split_token = split(tokens[1], '/');
            int nb_face_params   = split_token.size();
            int nb_face_vertices = tokens.size() - 1;

            bool has_normals = nb_face_params == 3;
            bool has_uv      = nb_face_params == 2 || (nb_face_params == 3 && split_token[1] != "");




            //std::cout << line << std::endl;
            std::vector<int> indices;
            std::vector<std::string> index_string;
            for (int i = 0; i < nb_face_vertices; ++i)
            {
                index_string = split(tokens[i + 1], '/');
                for (size_t j = 0; j < index_string.size(); ++j)
                {
                    if (!index_string[j].empty())
                        indices.push_back(std::atoi(index_string[j].c_str()) - 1);
                }
            }

            Face face, face2;

            bool is_quad = nb_face_vertices == 4;

            if (!has_normals && !has_uv)
            {
                face.v_id = int3(indices[0], indices[1], indices[2]);

                if (is_quad)
                {
                    face2.v_id = int3(indices[2], indices[3], indices[0]);
                }
            }
            else if (!has_normals && has_uv)
            {
                face.v_id = int3(indices[0], indices[2], indices[4]);
                face.t_id = int3(indices[1], indices[3], indices[5]);

                if (is_quad)
                {
                    face2.v_id = int3(indices[4], indices[6], indices[0]);
                    face2.t_id = int3(indices[5], indices[7], indices[1]);
                }
            }
            else if (has_normals && !has_uv)
            {
                face.v_id = int3(indices[0], indices[2], indices[4]);
                face.n_id = int3(indices[1], indices[3], indices[5]);

                if (is_quad)
                {
                    face2.v_id = int3(indices[4], indices[6], indices[0]);
                    face2.n_id = int3(indices[5], indices[7], indices[1]);
                }
            }
            else
            {
                face.v_id = int3(indices[0], indices[3], indices[6]);
                face.t_id = int3(indices[1], indices[4], indices[7]);
                face.n_id = int3(indices[2], indices[5], indices[8]);

                if (is_quad)
                {
                    face2.v_id = int3(indices[6], indices[9],  indices[0]);
                    face2.t_id = int3(indices[7], indices[10], indices[1]);
                    face2.n_id = int3(indices[8], indices[11], indices[2]);
                }
            }

            faces.push_back(face);

            if (is_quad)
                faces.push_back(face2);

        }
    }

    std::cout << vertices.size() << " vertices" << std::endl;
    std::cout << faces.size()    << " faces"    << std::endl;
    std::cout << normals.size()  << " normals"  << std::endl;

    return Mesh(vertices, texture_coordinates, normals, faces);
}
