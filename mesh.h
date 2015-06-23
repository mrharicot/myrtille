#ifndef MESH_H
#define MESH_H

#include <vector>

#include "geometry.h"

struct Hit
{
    bool did_hit;
    float t;
    int face_id;

    Hit(bool did_hit, float t, int face_id) : did_hit(did_hit), t(t), face_id(face_id) {}

    inline operator bool() const { return did_hit; }
};

class Mesh {
public:
    Mesh() {}
    Mesh(std::vector<float> vertices, std::vector<int> faces)
        : m_vertices(vertices), m_faces(faces) {}

    void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    void set_normals(std::vector<float> normals)  {m_normals = normals;}
    void set_faces(std::vector<int> faces) {m_faces = faces;}

    triangle face(int i) const;

    inline int nb_vertices(void) const { return m_vertices.size() / 3; }
    inline int nb_faces(void)    const { return m_faces.size() / 3;    }

    Hit intersect(ray r, float t_min = 0.0f, float t_max = 1e20f);

private:
    std::vector<float> m_vertices;
    std::vector<float> m_normals;
    std::vector<int>   m_faces;

};

Mesh read_ply(const char* file_path);

#endif
