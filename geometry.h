#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>
#include <utility>

#include "math_tools.h"

struct ray
{
    float3 origin;
    float3 direction;

    ray(float3 origin, float3 direction) : origin(origin), direction(direction) {}
};

class Triangle
{
public:

    Triangle() {}
    Triangle(float3 v0, float3 v1, float3 v2) : v0(v0), v1(v1), v2(v2) {}

    inline float3 normal()   const { return (v1 - v0).cross(v2 - v0).normalized(); }
    inline float3 centroid() const { return (v0 + v1 + v2) / 3.0f; }

    inline std::pair<float3, float3> bb() const { return std::make_pair(min(min(v0, v1), v2), max(max(v0, v1), v2)); }

    std::pair<bool, float> intersect(const ray &r) const;

    float3 v0;
    float3 v1;
    float3 v2;
};

struct Hit
{
    bool did_hit;
    float t;
    const Triangle* face;

    Hit() {}
    Hit(bool did_hit, float t, const Triangle* face) : did_hit(did_hit), t(t), face(face) {}

    inline operator bool() const { return did_hit; }
};

class AABB
{
public:
    AABB() {}
    AABB(float3 min, float3 max) : min(min), max(max) {}

    std::pair<bool, float> intersect(const ray &r, float t_min = 0.0f);

    float3 min;
    float3 max;
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

    inline std::vector<const Triangle*>& faces(void) { return m_faces; }

    void compute_face_bb();
    void compute_centroid_bb();
    void choose_split();
    void partition_faces();

    Hit intersect(const ray &r, float t_min = 0.0f);

    int id;


private:
    AABB m_face_bb, m_centroid_bb;
    Node* m_left_child  = NULL;
    Node* m_right_child = NULL;
    Node* m_parent      = NULL;
    std::vector<const Triangle*> m_faces;
    std::pair<float, int> m_split;
};

void build_tree(Node *node);




#endif
