#include <algorithm>
#include <numeric>

#include "bvh.h"

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

BVH::BVH(Mesh *mesh) : m_mesh(mesh)
{
    indices().resize(m_mesh->nb_faces());
    std::iota(indices().begin(), indices().end(), 0);
    build_tree(0, 0, m_mesh->nb_faces());
}

std::pair<AABB, AABB> BVH::compute_bbs(int start_index, int end_index)
{
    float3 f_bb_min( 1e32f);
    float3 f_bb_max(-1e32f);

    float3 c_bb_min( 1e32f);
    float3 c_bb_max(-1e32f);

    for (int ii = start_index; ii < end_index; ++ii)
    {
        int i = m_indices[ii];
        float3 c = m_mesh->face(i).centroid();
        f_bb_min = min(f_bb_min, c);
        f_bb_max = max(f_bb_max, c);

        auto t_bb = m_mesh->face(i).bb();
        c_bb_min = min(c_bb_min, t_bb.min);
        c_bb_max = max(c_bb_max, t_bb.max);
    }
    return std::make_pair(AABB(f_bb_min, f_bb_max), AABB(c_bb_min, c_bb_max));
}

void BVH::sort(int start_index, int end_index, int axis)
{
    std::sort(m_indices.begin() + start_index, m_indices.end() + end_index, face_comparator(&m_mesh->faces(), axis));
}

std::pair<int, int> BVH::choose_split(int start_index, int end_index)
{
    std::pair<float, int> best_split(1e32f, -1);
    int best_axis = -1;
    for (int i = 0; i < 3; ++i)
    {
        auto current_split = sah_cost(start_index, end_index, i);
        if (current_split.first < best_split.first)
        {
            best_split = current_split;
            best_axis  = i;
        }
    }
    return std::make_pair(best_split.second, best_axis);
}

std::pair<float, int> BVH::sah_cost(int start_index, int end_index, int axis)
{
    sort(start_index, end_index, axis);

    std::vector<float> left_surfaces;
    std::vector<float> right_surfaces;
    int nb_nodes = end_index - start_index;
    for (int i = 1; i < nb_nodes; ++i)
    {
        float left_aabb_surface  = compute_bbs(start_index, start_index + i).second.surface();
        float right_aabb_surface = compute_bbs(start_index + i, end_index).second.surface();

        left_surfaces.push_back(left_aabb_surface);
        right_surfaces.push_back(right_aabb_surface);
    }

    std::vector<float> sah_costs;
    for (int i = 1; i < nb_nodes; ++i)
    {
        sah_costs.push_back(left_surfaces[i] * i + right_surfaces[nb_nodes - i] * (nb_nodes - i));
    }

    int max_index = std::distance(sah_costs.begin(), std::max_element(sah_costs.begin(), sah_costs.end()));

    return std::make_pair(max_index, sah_costs[max_index]);
}

void BVH::build_tree(int current_node, int start_index, int end_index)
{
    if (end_index - start_index > MAX_FACES_PER_LEAF)
    {
        auto best_split  = choose_split(start_index, end_index);
        int  split_index = best_split.first;

        sort(start_index, end_index, best_split.second);

        int left_index = m_nodes.size();
        m_nodes[current_node].left  = left_index;
        m_nodes[current_node].right = left_index + 1;

        AABB aabb = compute_bbs(start_index, split_index).first;
        m_nodes.push_back(Node(aabb, start_index, split_index));

        aabb = compute_bbs(split_index, end_index).first;
        m_nodes.push_back(Node(aabb, split_index, end_index));

        build_tree(left_index    , start_index, best_split.first);
        build_tree(left_index + 1, best_split.first, end_index);
    }

    m_nodes[current_node].left  = -start_index;
    m_nodes[current_node].right = -end_index;
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
