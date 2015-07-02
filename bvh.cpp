#include <algorithm>
#include <numeric>


#include "bvh.h"

BVH::BVH(Mesh *mesh) : m_mesh(mesh)
{
    indices().resize(m_mesh->nb_faces());
    std::iota(indices().begin(), indices().end(), 0);

    int end_index = m_mesh->nb_faces();

    AABB aabb = compute_face_bb(0, end_index);
    m_nodes.push_back(Node(aabb, 0, end_index));

    std::cout << "building BVH | " << end_index << " faces" << std::endl;

    m_nodes.resize(m_mesh->nb_faces() * 2);

    nb_nodes = 1;
    build_tree(0, 0, end_index);
    m_nodes.resize(nb_nodes);

    //    std::vector<Triangle> new_faces;
    //    std::vector<int>      new_indices;
    //    std::vector<int3>     new_face_indices;
    //    for (int i = 0; i < m_mesh->nb_faces(); ++i)
    //    {
    //        int ii = m_indices[i];
    //        new_indices.push_back(i);
    //        new_faces.push_back(m_mesh->face(ii));
    //        new_face_indices.push_back(m_mesh->face_index(ii));
    //    }
    //    m_mesh->faces() = new_faces;
    //    m_mesh->face_indices() = new_face_indices;
    //    m_indices = new_indices;

//    for (auto &f : m_futures)
//        f.get();
}

Hit BVH::intersect(ray &r, float &t_max)
{
    Hit best_hit = Hit(false, 1e32f, -1);

    auto root_hit = m_nodes[0].aabb.intersect(r);
    if (!root_hit.first)
        return best_hit;

    std::vector<std::pair<int, float> > nodes_stack;
    nodes_stack.reserve(256);
    nodes_stack.push_back(std::make_pair(0, root_hit.second));

    while (!nodes_stack.empty())
    {
        auto  c_pair = nodes_stack[nodes_stack.size() - 1];

        nodes_stack.pop_back();

        if (c_pair.second > t_max)
            continue;

        int c_id = c_pair.first;
        Node&  c_node = m_nodes[c_id];

        if (c_node.is_leaf())
        {
            Hit faces_hit = intersect_faces(r, t_max, -c_node.left, -c_node.right);
            if (faces_hit.did_hit)
                best_hit = faces_hit;
            continue;
        }

        auto  left_aabb_hit = m_nodes[c_node.left].aabb.intersect(r);
        auto right_aabb_hit = m_nodes[c_node.right].aabb.intersect(r);

        int  first = c_node.left;
        int second = c_node.right;

        float first_t  = left_aabb_hit.second;
        float second_t = right_aabb_hit.second;

        if (left_aabb_hit.second > right_aabb_hit.second)
        {
            std::swap(first,   second);
            std::swap(first_t, second_t);
        }

        if (second_t <= t_max)
            nodes_stack.push_back(std::make_pair(second, second_t));

        if (first_t <= t_max)
            nodes_stack.push_back(std::make_pair(first, first_t));
    }

    return best_hit;
}

Hit BVH::intersect_faces(ray &r, float &t_max, int start_index, int end_index)
{
    Hit hit(false, 1e32f, -1);
    for (int ii = start_index; ii < end_index; ++ii)
    {
        int i = m_indices[ii];
        auto trit = m_mesh->face(i).intersect(r);
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

AABB BVH::compute_face_bb(int start_index, int end_index)
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (int ii = start_index; ii < end_index; ++ii)
    {
        auto t_bb = m_mesh->face(m_indices[ii]).bb();
        bb_min = min(bb_min, t_bb.mini);
        bb_max = max(bb_max, t_bb.maxi);
    }
    return AABB(bb_min, bb_max);
}

AABB BVH::compute_centroid_bb(int start_index, int end_index)
{
    float3 bb_min( 1e32f);
    float3 bb_max(-1e32f);

    for (int ii = start_index; ii < end_index; ++ii)
    {
        //float3 c = m_mesh->face(m_indices[ii]).centroid();
        float3 c = m_mesh->centroid(m_indices[ii]);
        bb_min = min(bb_min, c);
        bb_max = max(bb_max, c);
    }
    return AABB(bb_min, bb_max);
}

void BVH::sort(int start_index, int end_index, int axis)
{
    std::sort(m_indices.begin() + start_index, m_indices.begin() + end_index, face_comparator(m_mesh, axis));
}

std::pair<int, int> BVH::choose_split(int start_index, int end_index)
{
    std::pair<float, int> best_split(1e32f, -1);
    int best_axis = -1;
    const char* axes = "xyz";
    for (int i = 0; i < 3; ++i)
    {
        auto current_split = sah_cost(start_index, end_index, i);
        //std::cout << axes[i] << " split: " << current_split.first << ", " << current_split.second << std::endl;
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

    int nb_faces = end_index - start_index;
    std::vector<float>  left_surfaces(nb_faces);
    std::vector<float> right_surfaces(nb_faces);

    AABB left_aabb = m_mesh->face(m_indices[start_index]).bb();
    left_surfaces[0] = left_aabb.surface();

    AABB right_aabb = m_mesh->face(m_indices[end_index - 1]).bb();
    right_surfaces[0] = right_aabb.surface();

    for (int i = 1; i < nb_faces; ++i)
    {
        AABB face_aabb = m_mesh->face(m_indices[start_index + i]).bb();
        left_aabb.extend(face_aabb);

        face_aabb = m_mesh->face(m_indices[end_index - 1 - i]).bb();
        right_aabb.extend(face_aabb);

        left_surfaces[i]  =  left_aabb.surface();
        right_surfaces[i] = right_aabb.surface();
    }

    float min_cost = 1e32f;
    float min_id   = -1;

    for (int i = 1; i < nb_faces; ++i)
    {
        float current_cost = left_surfaces[i] * i + right_surfaces[nb_faces - i] * (nb_faces - i);
        if (current_cost < min_cost)
        {
            min_id   = i;
            min_cost = current_cost;
        }
    }


    //int max_index = std::distance(sah_costs.begin(), std::max_element(sah_costs.begin(), sah_costs.end()));

    //return std::make_pair(sah_costs[max_index], start_index + max_index + 1);


    //int split_index = nb_faces / 2;

    //float cost = compute_face_bb(start_index, start_index + split_index).surface() * (split_index) +
    //             compute_face_bb(start_index + split_index, end_index).surface() * (nb_faces - split_index);
    return std::make_pair(min_cost, start_index + min_id);
}

void BVH::build_tree(int current_node, int start_index, int end_index)
{
    //std::cout << "node " << current_node << " | " << std::flush;
    if (end_index - start_index > MAX_FACES_PER_LEAF)
    {
        auto best_split  = choose_split(start_index, end_index);
        int  split_index = best_split.first;

        sort(start_index, end_index, best_split.second);

        //const char* axes = "xyz";
        //std::cout << "splitting: " << axes[best_split.second] << " | " << std::flush;

        AABB left_aabb  = compute_face_bb(start_index, split_index);
        AABB right_aabb = compute_face_bb(split_index, end_index);


        int left_index = __sync_fetch_and_add(&nb_nodes, 2);

        m_nodes[left_index    ] = Node(left_aabb,  -1, -1);
        m_nodes[left_index + 1] = Node(right_aabb, -1, -1);


        m_nodes[current_node].left  = left_index;
        m_nodes[current_node].right = left_index + 1;


        if (end_index - start_index > m_mesh->nb_faces() / 12)
        {
            auto left = std::async(std::launch::async, [&]() {
                  return build_tree(left_index    , start_index, split_index);
                });
            build_tree(left_index + 1, split_index,   end_index);
        }
        else
        {
            build_tree(left_index    , start_index, split_index);
            build_tree(left_index + 1, split_index,   end_index);
        }

    }
    else
    {
        m_nodes[current_node].left  = -start_index;
        m_nodes[current_node].right = -end_index;
    }
}
