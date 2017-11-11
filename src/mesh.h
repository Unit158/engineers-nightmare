#pragma once

#include <string>
#include <glm/glm.hpp>
#include <epoxy/gl.h>
#include <BulletCollision/CollisionShapes/btTriangleMesh.h>
#include <BulletCollision/CollisionShapes/btCollisionShape.h>

#include "wiring/wiring_data.h"
#include "physics.h"

struct hw_mesh {
    GLuint vbo;
    GLuint ibo;
    GLuint vao;
    GLuint num_indices;
};


/* TODO: pack this a bit better */
/* TODO: use packed normals on drivers that will accept them reliably. This
 * provokes a crash on AMD/windows.
 */
struct vertex {
    float x, y, z;
    float nx, ny, nz;
    //uint32_t normal_packed;
    uint32_t mat;

    vertex() : x(0), y(0), z(0), mat(0) {}

    vertex(float x, float y, float z, float nx, float ny, float nz, int mat)
        : x(x), y(y), z(z),
          nx(nx), ny(ny), nz(nz),
          //normal_packed(glm::packSnorm3x10_1x2(glm::vec4(nx, ny, nz, 0))),
          mat(mat)
    {
    }
};

struct sw_mesh {
    vertex *verts;
    unsigned int *indices;
    unsigned int num_vertices;
    unsigned int num_indices;

    glm::mat4 *attach_points[num_wire_types];
    unsigned int num_attach_points[num_wire_types];
};

sw_mesh *load_mesh(char const *filename);
hw_mesh *upload_mesh(sw_mesh *mesh);
void set_mesh_material(sw_mesh *m, int material);
void draw_mesh(hw_mesh *m);
void free_mesh(hw_mesh *m);
void draw_mesh_instanced(hw_mesh *m, unsigned num_instances);

struct mesh_data {
    std::string mesh{};
    sw_mesh *sw = nullptr;
    hw_mesh *hw = nullptr;
    btTriangleMesh *phys_mesh = nullptr;
    btCollisionShape *phys_shape = nullptr;

    mesh_data() = default;

    explicit mesh_data(const std::string &mesh) : mesh(mesh) {
        sw = load_mesh(mesh.c_str());
    }

    void upload_mesh() {
        assert(sw);

        hw = ::upload_mesh(sw);
    }

    void load_physics() {
        assert(sw);

        build_static_physics_mesh(sw, &phys_mesh, &phys_shape);
    }
};
