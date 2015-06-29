#ifndef MESH_H
#define MESH_H

#include <vector>

#include "math_tools.h"
#include "geometry.h"

class Mesh {
public:
    Mesh() {}
    inline Mesh(std::vector<float3> vertices, std::vector<int3> face_indices, std::vector<float3> normals)
        : m_vertices(vertices), m_face_indices(face_indices), m_normals(normals)
    {
        for (int i = 0; i < m_face_indices.size(); ++i)
        {
            m_faces.push_back(Triangle(m_vertices[m_face_indices[i].x], m_vertices[m_face_indices[i].y], m_vertices[m_face_indices[i].z]));
        }

    }

    //void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    //void set_normals(std::vector<float> normals)  {m_normals = normals;}
    //void set_face_indices(std::vector<int> face_indices) {m_face_indices = face_indices;}

    inline const Triangle& face(int i) const        { return m_faces[i]; }
    inline       std::vector<Triangle>& faces()     { return m_faces;    }
    //inline const std::vector<Triangle>& faces() const { return m_faces; }

    inline int nb_vertices(void) const  { return m_vertices.size();      }
    inline int nb_faces(void)    const  { return m_face_indices.size();  }

    inline int3&   face_indices(int i)  { return m_face_indices[i];      }
    inline float3& normal(int i)        { return m_normals[i];           }
    inline float3& vertex(int i)        { return m_vertices[i];          }

    Hit intersect(const ray &r, float t_min = 0.0f, float t_max = 1e20f);

private:
    std::vector<float3>    m_vertices;
    std::vector<float3>    m_normals;
    std::vector<int3>      m_face_indices;
    std::vector<Triangle>  m_faces;

};

Mesh read_ply(const char* file_path);

#endif
