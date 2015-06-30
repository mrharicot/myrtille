#ifndef BVH_H
#define BVH_H

#define MAX_FACES_PER_LEAF 8

#include <vector>
#include <utility>
#include <algorithm>

#include "math_tools.h"
#include "geometry.h"
#include "mesh.h"

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

class BVH
{
    struct Node
    {
        Node(AABB aabb, int l, int r) : aabb(aabb), left(l), right(r) {}
        AABB aabb;
        int left, right;
    };

    struct face_comparator
    {
        face_comparator(std::vector<Triangle> *faces, int axis) : faces(faces), axis(axis) {}

        inline bool operator()(int lhs, int rhs)
        {
            return faces->at(lhs).centroid().data[axis] < faces->at(rhs).centroid().data[axis];
        }

        std::vector<Triangle> *faces;
        int axis;
    };

public:
    BVH(Mesh *mesh);

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
};



void build_tree(std::vector<Triangle> &faces, std::vector<int> &indices, Node *node);

#endif // BVH_H
