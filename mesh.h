#ifndef MESH_H
#define MESH_H

#include <vector>

#include "geometry.h"

struct Hit
{
    bool did_hit;
    float t;
    int face_id;

    Hit() {}
    Hit(bool did_hit, float t, int face_id) : did_hit(did_hit), t(t), face_id(face_id) {}

    inline operator bool() const { return did_hit; }
};

class Mesh {
public:
    Mesh() {}
    inline Mesh(std::vector<float> vertices, std::vector<int> face_indices)
        : m_vertices(vertices), m_face_indices(face_indices)
    {
        for (int i = 0; i < m_face_indices.size() / 3; ++i)
        {
            m_faces.push_back(Triangle(&vertices[3 * m_face_indices[3 * i + 0]],
                                       &vertices[3 * m_face_indices[3 * i + 1]],
                                       &vertices[3 * m_face_indices[3 * i + 2]]));
        }

    }

    void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    void set_normals(std::vector<float> normals)  {m_normals = normals;}
    void set_face_indices(std::vector<int> face_indices) {m_face_indices = face_indices;}

    const Triangle &face(int i) const;

    inline int nb_vertices(void) const { return m_vertices.size() / 3; }
    inline int nb_faces(void)    const { return m_face_indices.size() / 3;    }

    Hit intersect(const ray &r, float t_min = 0.0f, float t_max = 1e20f);

private:
    std::vector<float>    m_vertices;
    std::vector<float>    m_normals;
    std::vector<int>      m_face_indices;
    std::vector<Triangle> m_faces;

};

Mesh read_ply(const char* file_path);

#endif
