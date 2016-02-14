//
// Created by clement on 14/02/16.
//

#ifndef MYRTILLE_SCENE_H
#define MYRTILLE_SCENE_H

#include "camera.h"
#include "mesh.h"
#include "material.h"
#include "bvh.h"

class Scene {
public:
    Scene() : m_bvh(nullptr), m_mesh(nullptr), m_camera(nullptr) {}

    inline void set_camera(Camera &camera) { m_camera = &camera; }
    inline void set_mesh(Mesh &mesh)       { m_mesh = &mesh; m_bvh = new BVH(m_mesh); }

    BVH    *bvh()    const { return m_bvh;    }
    Mesh   *mesh()   const { return m_mesh;   }
    Camera *camera() const { return m_camera; }

private:
    BVH      *m_bvh;
    Mesh     *m_mesh;
    Camera   *m_camera;
};


#endif //MYRTILLE_SCENE_H
