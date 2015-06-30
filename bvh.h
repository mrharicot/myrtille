#ifndef BVH_H
#define BVH_H

#define MAX_FACES_PER_LEAF 8

#include <vector>
#include <utility>
#include <algorithm>

#include "math_tools.h"
#include "geometry.h"

class BVH
{
public:
    BVH();
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

#endif // BVH_H
