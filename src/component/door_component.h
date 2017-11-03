#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <libconfig.h>
#include <memory>

#include "component_manager.h"

struct door_component_manager : component_manager {
    struct instance_data {
        c_entity *entity;
        hw_mesh * *mesh;
        float *pos;
        float *desired_pos;
        int *height;
    } instance_pool;

    void create_component_instance_data(unsigned count) override;
    void destroy_instance(instance i) override;
    void entity(c_entity e) override;

    void register_stub_generator();

    instance_data get_instance_data(c_entity e) {
        instance_data d{};
        auto inst = lookup(e);

        d.entity = instance_pool.entity + inst.index;
        d.mesh = instance_pool.mesh + inst.index;
        d.pos = instance_pool.pos + inst.index;
        d.desired_pos = instance_pool.desired_pos + inst.index;
        d.height = instance_pool.height + inst.index;

        return d;
    }

    static door_component_manager* get_manager() {
        return dynamic_cast<door_component_manager*>(component_managers["door"].get());
    }
};

struct door_component_stub : component_stub {
    door_component_stub() : component_stub("door") {}

    void
    assign_component_to_entity(c_entity entity) {
        std::shared_ptr<component_manager> m = std::move(component_managers[name]);
        std::shared_ptr<door_component_manager> man = std::dynamic_pointer_cast<door_component_manager>(m);

        man->assign_entity(entity);
        auto data = man->get_instance_data(entity);        

        *data.mesh = nullptr;

        *data.pos = 0;

        *data.desired_pos = 0;

        *data.height = 0;
  };
};
