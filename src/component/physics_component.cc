#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "physics_component.h"

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

std::unique_ptr<component_stub>
physics_stub_from_config(const config_setting_t *physics_config) {
    auto physics_stub = std::make_unique<physics_component_stub>();

    auto mesh_member = config_setting_get_member(physics_config, "mesh");
    physics_stub->mesh = config_setting_get_string(mesh_member);

    return physics_stub;
};

void
physics_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
    size = sizeof(const char*) * count + align_size<const char*>(size);
    size = sizeof(btRigidBody *) * count + align_size<btRigidBody *>(size);
    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
    new_pool.mesh = align_ptr((const char* *)(new_pool.entity + count));
    new_pool.rigid = align_ptr((btRigidBody * *)(new_pool.mesh + count));

    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
    memcpy(new_pool.mesh, instance_pool.mesh, buffer.num * sizeof(const char*));
    memcpy(new_pool.rigid, instance_pool.rigid, buffer.num * sizeof(btRigidBody *));

    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
physics_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
    instance_pool.mesh[i.index] = instance_pool.mesh[last_index];
    instance_pool.rigid[i.index] = instance_pool.rigid[last_index];

    entity_instance_map[last_entity] = i.index;
    entity_instance_map.erase(current_entity);

    --buffer.num;
}

void
%s_component_manager::entity(c_entity e) {
    if (buffer.num >= buffer.allocated) {
        printf("Increasing size of %s buffer. Please adjust\n");
        create_component_instance_data(std::max(1u, buffer.allocated) * 2);
    }

    auto inst = lookup(e);

    instance_pool.entity[inst.index] = e;
}
