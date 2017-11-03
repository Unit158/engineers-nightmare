#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <libconfig.h>
#include <memory>

#include "component_manager.h"

struct reader_component_manager : component_manager {
    struct instance_data {
        c_entity *entity;
        char const * *name;
        c_entity *source;
        char const * *desc;
        float *data;
    } instance_pool;

    void create_component_instance_data(unsigned count) override;
    void destroy_instance(instance i) override;
    void entity(c_entity e) override;

    void register_stub_generator();

    instance_data get_instance_data(c_entity e) {
        instance_data d{};
        auto inst = lookup(e);

        d.entity = instance_pool.entity + inst.index;
        d.name = instance_pool.name + inst.index;
        d.source = instance_pool.source + inst.index;
        d.desc = instance_pool.desc + inst.index;
        d.data = instance_pool.data + inst.index;

        return d;
    }

    static reader_component_manager* get_manager() {
        return dynamic_cast<reader_component_manager*>(component_managers["reader"].get());
    }
};

struct reader_component_stub : component_stub {
    reader_component_stub() : component_stub("reader") {}

    std::string name{};

    std::string desc{};

    void
    assign_component_to_entity(c_entity entity) {
        std::shared_ptr<component_manager> m = std::move(component_managers[name]);
        std::shared_ptr<reader_component_manager> man = std::dynamic_pointer_cast<reader_component_manager>(m);

        man->assign_entity(entity);
        auto data = man->get_instance_data(entity);        

        *data.name = "";

        *data.source = {};

        *data.desc = "";

        *data.data = 0;

        *data.name = name.c_str();

        *data.desc = desc.c_str();
  };
};
