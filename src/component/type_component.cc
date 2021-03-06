/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "type_component.h"
#include "component_system_manager.h"

extern component_system_manager component_system_man;

void
type_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
    size = sizeof(unsigned) * count + align_size<unsigned>(size);
    size = sizeof(char const *) * count + align_size<char const *>(size);
    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
    new_pool.type = align_ptr((unsigned *)(new_pool.entity + count));
    new_pool.name = align_ptr((char const * *)(new_pool.type + count));

    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
    memcpy(new_pool.type, instance_pool.type, buffer.num * sizeof(unsigned));
    memcpy(new_pool.name, instance_pool.name, buffer.num * sizeof(char const *));

    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
type_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
    instance_pool.type[i.index] = instance_pool.type[last_index];
    instance_pool.name[i.index] = instance_pool.name[last_index];

    entity_instance_map[last_entity] = i.index;
    entity_instance_map.erase(current_entity);

    --buffer.num;
}

void
type_component_manager::entity(c_entity e) {
    if (buffer.num >= buffer.allocated) {
        printf("Increasing size of type buffer. Please adjust\n");
        create_component_instance_data(std::max(1u, buffer.allocated) * 2);
    }

    auto inst = lookup(e);

    instance_pool.entity[inst.index] = e;
}

void
type_component_stub::assign_component_to_entity(c_entity entity) {
    auto &man = component_system_man.managers.type_component_man;

    man.assign_entity(entity);
    auto data = man.get_instance_data(entity);        

    *data.type = 0;

    *data.name = "";

    *data.name = name.c_str();
};
