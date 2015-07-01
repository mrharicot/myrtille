#include <algorithm>

#include "geometry.h"

#define EPSILON 1e-10f


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

std::pair <bool, float> AABB::intersect(const ray &r, float t_min)
{
    for (int i = 0; i < 3; ++i)
    {
        if (r.direction.data[i] == 0 && (r.origin.data[i] < mini.data[i] || r.origin.data[i] > maxi.data[i]))
            return std::make_pair(false, 1e32f);
    }

    float t_start = -1e32f;
    float t_end   =  1e32f;
    for (int i = 0; i < 3; ++i)
    {
        float t1 = (mini.data[i] - r.origin.data[i]) * r.inv_d.data[i];
        float t2 = (maxi.data[i] - r.origin.data[i]) * r.inv_d.data[i];

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

