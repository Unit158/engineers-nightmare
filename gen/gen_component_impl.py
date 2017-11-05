#!/usr/bin/env python

#
# Quick & dirty component impl generator.
# From the root of the EN tree, run:
#    gen/gen_component_impl.py
#
# (Or, on windows: python gen/gen_component_impl.py)
#

header_template_1="""#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <libconfig.h>
#include <memory>

#include "component_manager.h"

struct %s_component_manager : component_manager {
    struct instance_data {
        c_entity *entity;
"""

header_template_2="""        %(type)s *%(name)s;
"""

header_template_3="""    } instance_pool;

    void create_component_instance_data(unsigned count) override;
    void destroy_instance(instance i) override;
    void entity(c_entity e) override;

    void register_stub_generator();

"""

header_template_4=""

header_template_5="""    instance_data get_instance_data(c_entity e) {
        instance_data d{};
        auto inst = lookup(e);

        d.entity = instance_pool.entity + inst.index;
"""
header_template_6="""        d.%(name)s = instance_pool.%(name)s + inst.index;
"""

header_template_7="""
        return d;
    }

    static %s_component_manager* get_manager() {
        return dynamic_cast<%s_component_manager*>(::component_managers["%s"].get());
    }
};
"""

header_template_8="""
struct %s_component_stub : component_stub {
    %s_component_stub() : component_stub("%s") {}
"""

header_template_9="""
    %(otype)s %(name)s{};
"""

header_template_10="""
    void
    assign_component_to_entity(c_entity entity) override {
        auto man = dynamic_cast<%s_component_manager*>(std::move(::component_managers[component_name]).get());

        man->assign_entity(entity);
        auto data = man->get_instance_data(entity);        
"""

header_template_101="""
        *data.%(name)s = %(default)s;
"""

header_template_11="""
        *data.%(name)s = %(name)s%(extra)s;
"""

header_template_12="""  };
};
"""


impl_template_1_stub="""#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "%s_component.h"

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

std::unique_ptr<component_stub>
%s_stub_from_config(const config_setting_t *%s_config) {
    auto %s_stub = std::make_unique<%s_component_stub>();
"""

impl_template_2_stub="""
    auto %(name)s_member = config_setting_get_member(%(comp_name)s_config, "%(name)s");
    %(comp_name)s_stub->%(name)s = config_setting_get_%(type)s(%(name)s_member);
"""

impl_template_1="""
    return %s_stub;
};

extern std::unordered_map<std::string, std::function<std::unique_ptr<component_stub>(config_setting_t *)>> component_stub_generators;

void
%s_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
"""

impl_template_2="""    size = sizeof(%(type)s) * count + align_size<%(type)s>(size);
"""

impl_template_3="""    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
"""

impl_template_4="""    new_pool.%(name)s = align_ptr((%(type)s *)(new_pool.%(prev)s + count));
"""

impl_template_5="""
    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
"""

impl_template_6="""    memcpy(new_pool.%(name)s, instance_pool.%(name)s, buffer.num * sizeof(%(type)s));
"""

impl_template_7="""
    free(buffer.buffer);
    buffer = new_buffer;

    instance_pool = new_pool;
}

void
%s_component_manager::destroy_instance(instance i) {
    auto last_index = buffer.num - 1;
    auto last_entity = instance_pool.entity[last_index];
    auto current_entity = instance_pool.entity[i.index];

    instance_pool.entity[i.index] = instance_pool.entity[last_index];
"""

impl_template_8="""    instance_pool.%(name)s[i.index] = instance_pool.%(name)s[last_index];
"""

impl_template_9="""
    entity_instance_map[last_entity] = i.index;
    entity_instance_map.erase(current_entity);

    --buffer.num;
}

void
%s_component_manager::entity(c_entity e) {
    if (buffer.num >= buffer.allocated) {
        printf("Increasing size of %s buffer. Please adjust\\n");
        create_component_instance_data(std::max(1u, buffer.allocated) * 2);
    }

    auto inst = lookup(e);

    instance_pool.entity[inst.index] = e;
}

void
%s_component_manager::register_stub_generator() {
    component_stub_generators["%s"] = %s_stub_from_config;
}
"""

import os
import sys
import glob

def main():

    files = [f for f in glob.glob('gen/*') if not f.endswith('.py')]

    for fl in files:
        print "Genning from %s to " % fl
        stub_fields = []
        body_fields = []
        component_name = fl.split(os.sep)[-1]
        with open(fl, 'r') as f:
            prev = 'entity';
            for l in f:
                parts = l.strip().split(',')
                if parts[0] == 'body':
                    body_fields.append({'type': parts[1], 'name': parts[2], 'default': parts[3], 'prev': prev, 'comp_name': component_name})
                    prev = parts[2]
                elif parts[0] == 'stub':
                    stub_fields.append({'otype': parts[1], 'type': parts[2], 'name': parts[3], 'extra': parts[4], 'comp_name': component_name})

        print "  header: src/component/%s_component.h" % component_name
        with open("src/component/%s_component.h" % component_name, "w") as g:
            g.write(header_template_1 % component_name)
            for fi in body_fields:
                g.write(header_template_2 % fi)
            g.write(header_template_3)
            for fi in body_fields:
                g.write(header_template_4 % fi)
            g.write(header_template_5)
            for fi in body_fields:
                g.write(header_template_6 % fi)
            g.write(header_template_7 % (component_name, component_name, component_name))
            g.write(header_template_8 % (component_name, component_name, component_name))
            for fi in stub_fields:
                g.write(header_template_9 % fi)
            g.write(header_template_10 % (component_name))
            for fi in body_fields:
                g.write(header_template_101 % fi)
            for fi in stub_fields:
                g.write(header_template_11 % fi)
            g.write(header_template_12)

        print "  source: src/component/%s_component.cc" % component_name
        with open("src/component/%s_component.cc" % component_name, "w") as g:
            g.write(impl_template_1_stub % (component_name, component_name, component_name, component_name, component_name))
            for fi in stub_fields:
                g.write(impl_template_2_stub % fi)
            g.write(impl_template_1 % (component_name, component_name))
            for fi in body_fields:
                g.write(impl_template_2 % fi)
            g.write(impl_template_3)
            for fi in body_fields:
                g.write(impl_template_4 % fi)
            g.write(impl_template_5)
            for fi in body_fields:
                g.write(impl_template_6 % fi)
            g.write(impl_template_7 % component_name)
            for fi in body_fields:
                g.write(impl_template_8 % fi)
            g.write(impl_template_9 % (component_name, component_name, component_name, component_name, component_name))

    return 0;

if __name__ == '__main__':
    sys.exit(main())
