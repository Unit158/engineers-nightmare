#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <libconfig.h>
#include <memory>

#include "component_manager.h"

struct type_component_manager : component_manager {
    struct instance_data {
        c_entity *entity;
        unsigned *type;
        char const * *name;
    } instance_pool;

    void create_component_instance_data(unsigned count) override;
    void destroy_instance(instance i) override;
    void entity(c_entity e) override;

    void register_stub_generator();

    instance_data get_instance_data(c_entity e) {
        instance_data d{};
        auto inst = lookup(e);

        d.entity = instance_pool.entity + inst.index;
        d.type = instance_pool.type + inst.index;
        d.name = instance_pool.name + inst.index;

        return d;
    }

    static type_component_manager* get_manager() {
        return dynamic_cast<type_component_manager*>(::component_managers["type"].get());
    }
};

struct type_component_stub : component_stub {
    type_component_stub() : component_stub("type") {}

    std::string name{};

    void
    assign_component_to_entity(c_entity entity) override {
        auto man = dynamic_cast<type_component_manager*>(std::move(::component_managers[component_name]).get());

        man->assign_entity(entity);
        auto data = man->get_instance_data(entity);        

        *data.type = 0;

        *data.name = "";

        *data.name = name.c_str();
  };
};
