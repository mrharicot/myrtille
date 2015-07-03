#ifndef MESH_H
#define MESH_H

#include <vector>

#include "math_tools.h"
#include "geometry.h"

class Mesh {
public:
    Mesh() {}
    inline Mesh(std::vector<float3> vertices, std::vector<int3> face_indices, std::vector<float3> normals, bool has_normals)
        : m_vertices(vertices), m_face_indices(face_indices), m_normals(normals), has_normals(has_normals)
    {
        for (size_t i = 0; i < m_face_indices.size(); ++i)
        {
            m_faces.push_back(Triangle(m_vertices[m_face_indices[i].x], m_vertices[m_face_indices[i].y], m_vertices[m_face_indices[i].z]));
            m_centroids.push_back(m_faces[i].centroid());
        }

    }

    //void set_vertices(std::vector<float> vertices)  {m_vertices = vertices;}
    //void set_normals(std::vector<float> normals)  {m_normals = normals;}
    //void set_face_indices(std::vector<int> face_indices) {m_face_indices = face_indices;}

    inline const Triangle& face(int i) const        { return m_faces[i]; }
    inline       std::vector<Triangle>& faces()     { return m_faces;    }
    inline       std::vector<int3>& face_indices()  { return m_face_indices; }
    //inline const std::vector<Triangle>& faces() const { return m_faces; }

    inline int nb_vertices(void) const  { return m_vertices.size();      }
    inline int nb_faces(void)    const  { return m_face_indices.size();  }

    inline int3&   face_index(int i)  { return m_face_indices[i];      }
    inline float3& normal(int i)        { return m_normals[i];           }
    inline float3& vertex(int i)        { return m_vertices[i];          }
    inline float3& centroid(int i)      { return m_centroids[i];         }

    Hit intersect(const ray &r, float t_min = 0.0f, float t_max = 1e20f);

    inline float3 face_normal(int i, float3 &p)
    {
        if (has_normals)
        {
            float3 bc  = m_faces[i].barycentric_coords(p);
            int3   fvi = m_face_indices[i];
            float3 n   = m_normals[fvi.x] * bc.x + m_normals[fvi.y] * bc.y + m_normals[fvi.z] * bc.z;
            return n.normalized();
        }
        else
        {
            return m_faces[i].normal();
        }
    }

private:
    std::vector<float3>    m_vertices;
    std::vector<int3>      m_face_indices;
    std::vector<float3>    m_normals;
    std::vector<Triangle>  m_faces;
    std::vector<float3>    m_centroids;

    bool has_normals;

};

Mesh read_ply(const char* file_path);

#endif
