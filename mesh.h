#ifndef MESH_H
#define MESH_H

#include <vector>

class Mesh {
public:
    Mesh() {}
    Mesh(std::vector<float> vertices, std::vector<int> faces)
        : m_vertices(vertices), m_faces(faces) {}

    void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    void set_normals(std::vector<float> normals)  {m_normals = normals;}
    void set_faces(std::vector<int> faces) {m_faces = faces;}

private:
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<int>   m_faces;

};

Mesh read_ply(const char* file_path);

#endif
