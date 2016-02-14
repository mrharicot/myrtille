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
        return std::make_pair(false, 1e32f);

    float inv_det = 1.0f / det;

    float3 t = r.origin - v0;

    float u = t.dot(p) * inv_det;

    if(u < 0.0f || u > 1.0f)
        return std::make_pair(false, 1e32f);

    float3 q = t.cross(e0);

    float v = r.direction.dot(q) * inv_det;

    if(v < 0.0f || u + v  > 1.0f)
        return std::make_pair(false, 1e32f);

    float d = e1.dot(q) * inv_det;

    if(d > EPSILON)
        return std::make_pair(true, d);

    return std::make_pair(false, 1e32f);
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

    float3 t1((mini.x - r.origin.x) * r.inv_d.x,
              (mini.y - r.origin.y) * r.inv_d.y,
              (mini.z - r.origin.z) * r.inv_d.z);

    float3 t2((maxi.x - r.origin.x) * r.inv_d.x,
              (maxi.y - r.origin.y) * r.inv_d.y,
              (maxi.z - r.origin.z) * r.inv_d.z);

    for (int i = 0; i < 3; ++i)
        if (t1.data[i] > t2.data[i])
            std::swap(t1.data[i], t2.data[i]);

    float t_end   = std::min(std::min(t2.x, t2.y), t2.z);
    if (t_end < t_min)
        return std::make_pair(false, 1e32f);

    float t_start = std::max(std::max(t1.x, t1.y), t1.z);
    if (t_start > t_end)
        return std::make_pair(false, 1e32f);

    return std::make_pair(true, t_start);
}

/*
float3 sample_diffuse_ray(float3 &n, float r1, float r2)
{
    bool zup = std::abs(n.z) < 0.9f;

    float3 upvec = zup ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);

    float3 k = upvec.cross(n);
    float ct = upvec.dot(n);
    float st = k.norm();
    k /= st;

    float sq = std::sqrt(1.0f - r2);

    float3 v;
    if (zup)
    {
        v = float3(std::cos(2.0f * pi * r1) * sq,
                   std::sin(2.0f * pi * r1) * sq,
                   std::sqrt(r2));
    }
    else
    {
        v = float3(std::sqrt(r2),
                   std::cos(2.0f * pi * r1) * sq,
                   std::sin(2.0f * pi * r1) * sq);
    }

    return v * ct + k.cross(v) * st + k * k.dot(v) * (1.0f - ct);
}


float3 sample_diffuse_ray(float3 &n, float r1, float r2)
{
    float sq = std::sqrt(1.0f - r2);

    float3 v = float3(std::cos(2.0f * pi * r1) * sq,
                      std::sin(2.0f * pi * r1) * sq,
                      std::sqrt(r2));

    Frame frame(n);

    return frame.to_world(v);
}


float3 sample_specular_ray(float3 &n, float r1, float r2, float alpha)
{
    bool zup = std::abs(n.z) < 0.9f;

    float3 upvec = zup ? float3(0.0f, 0.0f, 1.0f) : float3(1.0f, 0.0f, 0.0f);

    float3 k = upvec.cross(n);
    float ct = upvec.dot(n);
    float st = k.norm();
    k /= st;

    float theta = std::atan(alpha * std::sqrt(r1) / std::sqrt(1.0f - r1));
    float phi   = 2.0f * pi * r2;

    float3 v;
    if (zup)
        v = float3(std::sin(theta) * std::cos(phi),
                   std::sin(theta) * std::sin(phi),
                   std::cos(theta));
    else
        v = float3(std::cos(theta),
                   std::sin(theta) * std::cos(phi),
                   std::sin(theta) * std::sin(phi));

    return v * ct + k.cross(v) * st + k * k.dot(v) * (1.0f - ct);
}

 */