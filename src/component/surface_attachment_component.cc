/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "surface_attachment_component.h"
#include "component_system_manager.h"

extern component_system_manager component_system_man;

void
surface_attachment_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
    size = sizeof(glm::ivec3) * count + align_size<glm::ivec3>(size);
    size = sizeof(int) * count + align_size<int>(size);
    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
    new_pool.block = align_ptr((glm::ivec3 *)(new_pool.entity + count));
    new_pool.face = align_ptr((int *)(new_pool.block + count));

    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
    memcpy(new_pool.block, instance_pool.block, buffer.num * sizeof(glm::ivec3));
    memcpy(new_pool.face, instance_pool.face, buffer.num * sizeof(int));

    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
surface_attachment_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
    instance_pool.block[i.index] = instance_pool.block[last_index];
    instance_pool.face[i.index] = instance_pool.face[last_index];

    entity_instance_map[last_entity] = i.index;
    entity_instance_map.erase(current_entity);

    --buffer.num;
}

void
surface_attachment_component_manager::entity(c_entity e) {
    if (buffer.num >= buffer.allocated) {
        printf("Increasing size of surface_attachment buffer. Please adjust\n");
        create_component_instance_data(std::max(1u, buffer.allocated) * 2);
    }

    auto inst = lookup(e);

    instance_pool.entity[inst.index] = e;
}

void
surface_attachment_component_stub::assign_component_to_entity(c_entity entity) {
    auto &man = component_system_man.managers.surface_attachment_component_man;

    man.assign_entity(entity);
    auto data = man.get_instance_data(entity);        

    *data.block = glm::ivec3(0);

    *data.face = 0;
};
