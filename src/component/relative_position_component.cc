#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "relative_position_component.h"

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

std::unique_ptr<component_stub>
relative_position_stub_from_config(const config_setting_t *relative_position_config) {
    auto relative_position_stub = std::make_unique<relative_position_component_stub>();

    return relative_position_stub;
};

void
relative_position_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
    size = sizeof(glm::vec3) * count + align_size<glm::vec3>(size);
    size = sizeof(glm::mat4) * count + align_size<glm::mat4>(size);
    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
    new_pool.position = align_ptr((glm::vec3 *)(new_pool.entity + count));
    new_pool.mat = align_ptr((glm::mat4 *)(new_pool.position + count));

    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
    memcpy(new_pool.position, instance_pool.position, buffer.num * sizeof(glm::vec3));
    memcpy(new_pool.mat, instance_pool.mat, buffer.num * sizeof(glm::mat4));

    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
relative_position_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
    instance_pool.position[i.index] = instance_pool.position[last_index];
    instance_pool.mat[i.index] = instance_pool.mat[last_index];

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
