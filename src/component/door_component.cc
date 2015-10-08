#include <algorithm>
#include <string.h>
#include "../memory.h"
#include "door_component.h"

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

void
door_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer;
    instance_data new_pool;

    size_t size = sizeof(c_entity) * count;
    size = sizeof(hw_mesh *) * count + align_size<hw_mesh *>(size);
    size = sizeof(float) * count + align_size<float>(size);
    size = sizeof(float) * count + align_size<float>(size);
    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
    new_pool.mesh = align_ptr((hw_mesh * *)(new_pool.entity + count));
    new_pool.pos = align_ptr((float *)(new_pool.mesh + count));
    new_pool.desired_pos = align_ptr((float *)(new_pool.pos + count));

    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
    memcpy(new_pool.mesh, instance_pool.mesh, buffer.num * sizeof(hw_mesh *));
    memcpy(new_pool.pos, instance_pool.pos, buffer.num * sizeof(float));
    memcpy(new_pool.desired_pos, instance_pool.desired_pos, buffer.num * sizeof(float));

    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
door_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
    instance_pool.mesh[i.index] = instance_pool.mesh[last_index];
    instance_pool.pos[i.index] = instance_pool.pos[last_index];
    instance_pool.desired_pos[i.index] = instance_pool.desired_pos[last_index];

    entity_instance_map[last_entity] = i.index;
    entity_instance_map.erase(current_entity);

    --buffer.num;
}

void
door_component_manager::entity(c_entity e) {
    if (buffer.num >= buffer.allocated) {
        printf("Increasing size of door buffer. Please adjust\n");
        create_component_instance_data(std::max(1u, buffer.allocated) * 2);
    }

    auto inst = lookup(e);

    instance_pool.entity[inst.index] = e;
}
