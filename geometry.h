#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <utility>

#include "math_tools.h"

struct ray
{
    float3 origin;
    float3 direction;
    float3 inv_d;

    ray(float3 origin, float3 direction) : origin(origin), direction(direction) {
        inv_d = float3(1.0f / direction.x, 1.0f / direction.y, 1.0f / direction.z);
    }
};

class AABB
{
public:
    AABB() {}
    AABB(float3 min, float3 max) : min(min), max(max) {}

    std::pair<bool, float> intersect(const ray &r, float t_min);

    float3 min;
    float3 max;
};

class Triangle
{
public:

    Triangle() {}
    Triangle(float3 v0, float3 v1, float3 v2) : v0(v0), v1(v1), v2(v2) {}
    //union { struct { float3 v0; float3 v1; float3 v2; }; float3 data[3]; };
    float3 v0, v1, v2;

    inline const float3 normal(void)   const { return (v1 - v0).cross(v2 - v0).normalized(); }
    inline const float3 centroid(void) const { return (v0 + v1 + v2) / 3.0f; }

    //inline       AABB& bb()       { return m_bounding_box; }
    inline       AABB bb()       { return AABB(min(min(v0, v1), v2), max(max(v0, v1), v2)); }
    inline const AABB bb() const { return AABB(min(min(v0, v1), v2), max(max(v0, v1), v2)); }

    std::pair<bool, float> intersect(const ray &r) const;
    float3 barycentric_coords(float3 &p) const;
};

inline bool compare_x(const Triangle& t1, const Triangle& t2)
{
    return t1.centroid().x < t2.centroid().x;
}

inline bool compare_y(const Triangle& t1, const Triangle& t2)
{
    return t1.centroid().y < t2.centroid().y;
}

inline bool compare_z(const Triangle& t1, const Triangle& t2)
{
    return t1.centroid().z < t2.centroid().z;
}

struct face_comparator
{
    face_comparator(std::vector<Triangle> *faces, int axis) : faces(faces), axis(axis) {}

    inline bool operator()(int lhs, int rhs) {
        if (axis == 0)
            return faces->at(lhs).centroid().x < faces->at(rhs).centroid().x;

        if (axis == 1)
            return faces->at(lhs).centroid().y < faces->at(rhs).centroid().y;

        return faces->at(lhs).centroid().z < faces->at(rhs).centroid().z;
    }

    std::vector<Triangle> *faces;
    int axis;
};

struct Hit
{
    bool  did_hit;
    float t;
    int   face_id;

    Hit() {}
    Hit(bool did_hit, float t, int face_id) : did_hit(did_hit), t(t), face_id(face_id) {}

    inline operator bool() const { return did_hit; }
};



class Node
{
public:
    Node() {}

    inline Node* left(void) const  { return m_left_child;  }
    inline void  left(Node* node) { m_left_child = node; }

    inline Node* right(void) const  { return m_right_child; }
    inline void  right(Node* node) { m_right_child = node; }

    inline Node* parent(void) const { return m_parent;      }
    inline void  parent(Node* node) { m_parent = node; }

    inline std::pair<float, int> split(void) const { return m_split; }

    //inline std::vector<const Triangle*>& faces(void) { return m_faces; }
    inline int nb_faces(void) const { return end_index - start_index; }
    inline       AABB& face_bb(void)       { return m_face_bb; }
    inline const AABB& face_bb(void) const { return m_face_bb; }

    void     compute_face_bb(std::vector<Triangle> &faces, std::vector<int> &indices);
    void compute_centroid_bb(std::vector<Triangle> &faces, std::vector<int> &indices);
    void                sort(std::vector<Triangle> &faces, std::vector<int> &indices);
    void        choose_split();
    void     partition_faces(std::vector<Triangle> &faces , std::vector<int> &indices);

    Hit intersect(std::vector<Triangle> &faces, std::vector<int> &indices, const ray &r, float &t_max);

    int id;
    int start_index, end_index;

private:
    AABB m_face_bb, m_centroid_bb;
    Node* m_left_child  = NULL;
    Node* m_right_child = NULL;
    Node* m_parent      = NULL;
    std::pair<float, int> m_split;


};

void build_tree(std::vector<Triangle> &faces, std::vector<int> &indices, Node *node);

#endif
