#ifndef BVH_H
#define BVH_H

#define MAX_FACES_PER_LEAF 8

#include <vector>
#include <utility>
#include <algorithm>

#include "math_tools.h"
#include "geometry.h"
#include "mesh.h"

class BVH
{
    struct Node
    {
        Node(AABB aabb, int l, int r) : aabb(aabb), left(l), right(r) {}
        AABB aabb;
        int left, right;

        inline bool is_leaf(void) const { return right < 0; }
    };

    struct face_comparator
    {
        face_comparator(Mesh *mesh, int axis) : mesh(mesh), axis(axis) {}

        inline bool operator()(int lhs, int rhs)
        {
            //return faces->at(lhs).centroid().data[axis] < faces->at(rhs).centroid().data[axis];
            return mesh->centroid(lhs).data[axis] < mesh->centroid(rhs).data[axis];
        }

        Mesh *mesh;
        int axis;
    };

public:
    BVH(Mesh *mesh);

    Hit intersect(ray &r, float &t_max);

    inline Node&              node(int i)   { return m_nodes[i]; }
    inline std::vector<Node>& nodes(void)   { return m_nodes;    }
    inline std::vector<int>&  indices(void) { return m_indices;  }

private:
    Mesh*             m_mesh;
    std::vector<Node> m_nodes;
    std::vector<int>  m_indices;

    void build_tree(int current_node, int start_index, int end_index);
    AABB compute_face_bb(int start_index, int end_index);
    AABB compute_centroid_bb(int start_index, int end_index);
    std::pair<int, int>   choose_split(int start_index, int end_index);
    std::pair<float, int> sah_cost(int start_index, int end_index, int axis);
    void sort(int start_index, int end_index, int axis);
    Hit  intersect_faces(ray &r, float &t_max, int start_index, int end_index);
};

#endif // BVH_H
