#include "geometry.h"

#define EPSILON 1e-6f
#define MAX_FACES_PER_LEAF 8

//from wikipedia
std::pair<bool, float> Triangle::intersect(const ray &r) const
{
    float3 e0 = v1 - v0;
    float3 e1 = v2 - v0;

    float3 p = r.direction.cross(e1);

    float det = e0.dot(p);

    if(det > -EPSILON && det < EPSILON)
        return std::make_pair(false, 0.0f);

    float inv_det = 1.0f / det;

    float3 t = r.origin - v0;

    float u = t.dot(p) * inv_det;

    if(u < 0.0f || u > 1.0f)
        return std::make_pair(false, 1.0f);

    float3 q = t.cross(e0);

    float v = r.direction.dot(q) * inv_det;

    if(v < 0.0f || u + v  > 1.0f)
        return std::make_pair(false, 2.0f);

    float d = e1.dot(q) * inv_det;

    if(d > EPSILON)
        return std::make_pair(true, d);

    return std::make_pair(false, 3.0f);
}

std::pair <bool, float> AABB::intersect(const ray &r, float t_min)
{
    for (int i = 0; i < 3; ++i)
    {
        if (r.direction.data[i] == 0 && (r.origin.data[i] < min.data[i] || r.origin.data[i] > max.data[i]))
            return std::make_pair(false, i);
    }

    float t_start = -1e32f;
    float t_end   =  1e32f;
    for (int i = 0; i < 3; ++i)
    {
        float inv_d = 1.0f / r.direction.data[i];
        float t1 = (min.data[i] - r.origin.data[i]) * inv_d;
        float t2 = (max.data[i] - r.origin.data[i]) * inv_d;

        if (t1 > t2)
            swap(t1, t2);

        if (t1 > t_start)
            t_start = t1;

        if (t2 < t_end)
            t_end = t2;
    }

    if (t_start > t_end)
        return std::make_pair(false, 4.0f);

    if (t_end < t_min)
        return std::make_pair(false, 5.0f);

    if (t_start > t_min)
        return std::make_pair(true, t_start);

    return std::make_pair(true, t_end);
}

void Node::compute_face_bb()
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (const Triangle* t : m_faces)
    {
        auto t_bb = t->bb();
        bb_min = min(bb_min, t_bb.first);
        bb_max = max(bb_max, t_bb.second);
    }
    m_face_bb = AABB(bb_min, bb_max);
}

void Node::compute_centroid_bb()
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (const Triangle* t : m_faces)
    {
        float3 c = t->centroid();
        bb_min = min(bb_min, c);
        bb_max = max(bb_max, c);
    }
    m_centroid_bb = AABB(bb_min, bb_max);
}

void Node::choose_split()
{
    int    split_axis = m_parent == NULL ? 0 : (m_parent->split().second + 1) % 3;
    float split_value = 0.5f * (m_centroid_bb.max.data[split_axis] + m_centroid_bb.min.data[split_axis]);
    m_split = std::make_pair(split_value, split_axis);
}

void Node::partition_faces()
{
    float &split_value = m_split.first;
    int   &split_axis  = m_split.second;

    for (const Triangle* t : m_faces)
    {
        if (t->centroid().data[split_axis] > split_value)
        {
            m_right_child->faces().push_back(t);
        }
        else
        {
            m_left_child->faces().push_back(t);
        }
    }
}

Hit Node::intersect(const ray &r, float t_min)
{
    //check hit with bounding box
    auto bb_hit = m_face_bb.intersect(r, t_min);
    if (!bb_hit.first)
        return Hit(false, 0.0f, NULL);

    if (m_faces.size() == 0)
        return Hit(false, 0.0f, NULL);

    Hit hit(false, 1e32f, NULL);
    if (m_faces.size() < MAX_FACES_PER_LEAF)
    {

        for (const Triangle* tr : m_faces)
        {
            auto trit = tr->intersect(r);
            if (trit.first && trit.second < hit.t)
            {
                hit.did_hit = true;
                hit.face = tr;
                hit.t = trit.second;
            }
        }
        return hit;
    }
    else
    {
        Hit left_hit  =  left()->intersect(r, t_min);
        Hit right_hit = right()->intersect(r, t_min);

        if (!left_hit && !right_hit)
        {
            return Hit(false, 0.0f, NULL);
        }
        else if (left_hit && !right_hit)
        {
            return left_hit;
        }
        else if (!left_hit && right_hit)
        {
            return right_hit;
        }
        else
        {
            return left_hit.t < right_hit.t ? left_hit : right_hit;
        }
    }
}

void build_tree(Node* node)
{
    node->compute_face_bb();
    node->compute_centroid_bb();
    node->left(NULL);
    node->right(NULL);

    std::cout << node->id << " - " << node->faces().size() << std::endl;

    if (node->faces().size() > MAX_FACES_PER_LEAF)
    {
        node->left(new Node());
        node->right(new Node());
        node->left()->parent(node);
        node->right()->parent(node);

        node->choose_split();
        node->partition_faces();

        node->left()->id = 2 * node->id + 1;
        node->right()->id = 2 * node->id + 2;

        build_tree(node->left());
        build_tree(node->right());
    }
}




//function constructTree( list L, node N )
//{
//// must know node size, be it internal or leaf...
//N.boundingBox = computeBB( L );
//N.leftChild = N.rightChild = NULL;
//if ( L.numElements() > MAX_TRIS_PER_LEAF ) // continue recursion?
//{
//// decide how to split primitives
//plane S = chooseSplit( L );
//// perform actual split: partition L into two disjoint sets
//list leftChild, rightChild;
//partitionPrimitives( L, S, leftChild, rightChild );
//// construct left child primitivelist, recurse
//N.leftChild = new Node();
//constructTree( leftChild, N.leftChild );
//// construct left child primitivelist, recurse
//N.rightChild = new Node();
//constructTree( rightChildList, N.rightChild );
//}
//else N.primitives = L; // no: store which primitives are in this leaf
//}
//main()
//{
//Node root = new Node();
//construct( allPrimitives, root ); // recursively builds entire tree
//}
