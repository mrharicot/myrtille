#include <algorithm>

#include "geometry.h"

#define EPSILON 1e-7f
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

float3 Triangle::barycentric_coords(float3 &p) const
{
    float3 e0 = v1 - v0, e1 = v2 - v0, e2 = p - v0;

    float d00 = e0.dot(e0);
    float d01 = e0.dot(e1);
    float d11 = e1.dot(e1);
    float d20 = e2.dot(e0);
    float d21 = e2.dot(e1);
    float denom = 1.0f / (d00 * d11 - d01 * d01);

    float beta  = (d11 * d20 - d01 * d21) * denom;
    float gamma  = (d00 * d21 - d01 * d20) * denom;
    return float3(1.0f - beta - gamma, beta, gamma);
}

std::pair <bool, float> AABB::intersect(const ray &r, float t_min = 0.0f)
{

    for (int i = 0; i < 3; ++i)
    {
        if (r.direction.data[i] == 0 && (r.origin.data[i] < min.data[i] || r.origin.data[i] > max.data[i]))
            return std::make_pair(false, 1e32f);
    }

    float t_start = -1e32f;
    float t_end   =  1e32f;
    for (int i = 0; i < 3; ++i)
    {
        float t1 = (min.data[i] - r.origin.data[i]) * r.inv_d.data[i];
        float t2 = (max.data[i] - r.origin.data[i]) * r.inv_d.data[i];

        if (t1 > t2)
            swap(t1, t2);

        if (t1 > t_start)
            t_start = t1;

        if (t2 < t_end)
            t_end = t2;
    }

    if (t_start > t_end)
        return std::make_pair(false, 1e32f);

    if (t_end < t_min)
        return std::make_pair(false, 1e32f);

    if (t_start > t_min)
    {
        return std::make_pair(true, t_start);
    }
    else
    {
        return std::make_pair(true, t_end);
    }
}

void Node::compute_face_bb(std::vector<Triangle> &faces, std::vector<int> &indices)
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (int ii = start_index; ii < end_index; ++ii)
    {
        int i = indices[ii];
        auto t_bb = faces[i].bb();
        bb_min = min(bb_min, t_bb.min);
        bb_max = max(bb_max, t_bb.max);
    }
    m_face_bb = AABB(bb_min, bb_max);
}

void Node::compute_centroid_bb(std::vector<Triangle>& faces, std::vector<int> &indices)
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (int ii = start_index; ii < end_index; ++ii)
    {
        int i = indices[ii];
        float3 c = faces[i].centroid();
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

void Node::sort(std::vector<Triangle> &faces, std::vector<int> &indices)
{
    std::sort(indices.begin() + start_index, indices.begin() + end_index, face_comparator(&faces, m_split.second));
}

void Node::partition_faces(std::vector<Triangle> &faces, std::vector<int> &indices)
{
    int median_index = (start_index + end_index) / 2;

    left()->start_index  = start_index;
    left()->end_index    = median_index;

    right()->start_index = median_index;
    right()->end_index   = end_index;
}

Hit Node::intersect(std::vector<Triangle> &faces, std::vector<int> &indices, const ray &r, float &t_max)
{
    if (parent() == NULL)
    {
        auto bb_hit = face_bb().intersect(r);
        if (!bb_hit.first)
            return Hit(false, 1e32f, -1);
    }

    Hit hit(false, 1e32f, -1);
    if (nb_faces() < MAX_FACES_PER_LEAF)
    {
        for (int ii = start_index; ii < end_index; ++ii)
        {
            int i = indices[ii];
            auto trit = faces[i].intersect(r);
            if (trit.first && trit.second < hit.t && trit.second < t_max)
            {
                hit.did_hit = true;
                hit.face_id = i;
                hit.t = trit.second;
                t_max = hit.t;

            }
        }

        return hit;
    }
    else
    {
        auto left_bb_hit  =  left()->face_bb().intersect(r);
        auto right_bb_hit = right()->face_bb().intersect(r);

        Hit  first_hit(false, 1e32f, -1);
        Hit second_hit(false, 1e32f, -1);

        Node* first_node  =  left();
        Node* second_node = right();

        if (!left_bb_hit.first && !right_bb_hit.first)
            return Hit(false, 1e32f, -1);

        if (left_bb_hit.first && !right_bb_hit.first)
            return first_node->intersect(faces, indices, r, t_max);

        if (!left_bb_hit.first && right_bb_hit.first)
            return second_node->intersect(faces, indices, r, t_max);

        if (left_bb_hit.second > right_bb_hit.second)
        {
            first_node  = right();
            second_node =  left();
        }

        first_hit  =  first_node->intersect(faces, indices, r, t_max);
        second_hit = second_node->intersect(faces, indices, r, t_max);

        return first_hit.t < second_hit.t ? first_hit : second_hit;
    }
}

void build_tree(std::vector<Triangle> &faces, std::vector<int> &indices, Node* node)
{
    node->compute_face_bb(faces, indices);
    node->compute_centroid_bb(faces, indices);
    node->left(NULL);
    node->right(NULL);

    if (node->nb_faces() >= MAX_FACES_PER_LEAF)
    {
        node->left( new Node());
        node->right(new Node());
        node->left()->parent(node);
        node->right()->parent(node);

        node->choose_split();
        node->sort(faces, indices);
        node->partition_faces(faces, indices);

        node->left()->id  = 2 * node->id + 1;
        node->right()->id = 2 * node->id + 2;

        build_tree(faces, indices, node->left());
        build_tree(faces, indices, node->right());
    }
}
