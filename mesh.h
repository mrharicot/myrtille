#ifndef MESH_H
#define MESH_H

#include <vector>

#include "math_tools.h"
#include "geometry.h"
#include "material.h"

struct Face {
    Face() : v_id(-1), t_id(-1), n_id(-1), m_id(-1) {}
    Face(int3 v, int3 t, int3 n) : v_id(v), t_id(t), n_id(n), m_id(-1)  {}
    Face(int3 v, int3 t, int3 n, int m) : v_id(v), t_id(t), n_id(n), m_id(m)  {}
    int3 v_id, t_id, n_id;
    int  m_id;
};

class Mesh {
public:
    Mesh() {}
    inline Mesh(std::vector<float3> vertices, std::vector<float2> tex_coords, std::vector<float3> normals, std::vector<Face> faces, std::vector<int> e_faces, std::vector<Material> materials)
        : m_vertices(vertices), m_tex_coords(tex_coords), m_normals(normals), m_faces(faces), m_e_faces_indices(e_faces), m_materials(materials)
    {
        for (size_t i = 0; i < m_faces.size(); ++i)
        {
            int3 v_id = m_faces[i].v_id;
            m_triangles.push_back(Triangle(m_vertices[v_id.x], m_vertices[v_id.y], m_vertices[v_id.z]));
            m_centroids.push_back(m_triangles[i].centroid());
                m_areas.push_back(m_triangles[i].area());
        }

    }

    inline const Triangle&              triangle(int i) const { return m_triangles[i]; }
    inline       Triangle&              triangle(int i)       { return m_triangles[i]; }
    inline       std::vector<Triangle>& triangles()       { return m_triangles;    }
    inline       std::vector<Face>&     faces()           { return m_faces; }
    //inline const std::vector<Triangle>& faces() const { return m_faces; }

    inline int nb_vertices(void) const  { return m_vertices.size();      }
    inline int nb_faces(void)    const  { return m_faces.size();  }
    inline int nb_emissive_faces(void)    const  { return m_e_faces_indices.size();  }

    inline Face&     face(int i)            { return m_faces[i];      }
    inline Face& emissive_face(int i)       { return m_faces[m_e_faces_indices[i]]; }
    inline int   emissive_face_index(int i) { return m_e_faces_indices[i]; }
    inline float3&   normal(int i)          { return m_normals[i];    }
    inline float3&   vertex(int i)          { return m_vertices[i];   }
    inline float3&   centroid(int i)        { return m_centroids[i];  }
    inline float     area(int i)            { return m_areas[i];      }
    inline Material& material(int i)        { return m_materials[i];  }
    inline Material& face_material(int i)   { return m_materials[m_faces[i].m_id]; }

    Hit intersect(const ray &r, float t_min = 0.0f, float t_max = 1e20f);

    inline float3 face_normal(int i, float3 &p)
    {
        if (m_faces[i].n_id.x >= 0)
        {
            float3 bc   = m_triangles[i].barycentric_coords(p);
            int3   n_id = m_faces[i].n_id;
            float3 n    = m_normals[n_id.x] * bc.x + m_normals[n_id.y] * bc.y + m_normals[n_id.z] * bc.z;
            return n.normalized();
        }
        else
        {
            return m_triangles[i].normal();
        }
    }

    inline int sample_emissive_face_id(float &r)
    {
        int nb_ef = nb_emissive_faces();
        int f_id = (int) std::min((int) floor(nb_ef * r), nb_ef - 1);

        r *= nb_ef;
        r -= f_id;

        return f_id;
    }

//    inline sample_emissive_face(float &r1, float &r2)
//    {

//    }

private:
    std::vector<float3>    m_vertices;
    std::vector<float2>    m_tex_coords;
    std::vector<float3>    m_normals;

    std::vector<Face>      m_faces;
    std::vector<int>       m_e_faces_indices;

    std::vector<Material>  m_materials;

    std::vector<Triangle>  m_triangles;
    std::vector<float3>    m_centroids;
    std::vector<float>     m_areas;

    bool has_normals;

};

Mesh read_ply(const char* file_path);
Mesh read_obj(const char* file_path);
std::vector<Material> read_mtl(const char* file_path);

#endif
