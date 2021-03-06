#!/usr/bin/env python

from __future__ import print_function

#
# Quick & dirty component impl generator.
# From the root of the EN tree, run:
#    gen/gen_component_impl.py
#
# (Or, on windows: python gen/gen_component_impl.py)
#

man_header_1="""#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <unordered_map>
#include <memory>

#include "component_manager.h"

"""

man_header_2_each="""#include "%s_component.h"
"""

man_header_3="""
#define INITIAL_MAX_COMPONENTS 20

struct component_managers {
    component_managers() {"""

man_header_4_each="""
        %s_component_man.create_component_instance_data(INITIAL_MAX_COMPONENTS);"""

man_header_5="""
    }
"""

man_header_6_each="""
    %s_component_manager %s_component_man{};"""

man_header_7="""

    std::unique_ptr<component_stub> get_stub(const char*comp_name, const config_setting_t *config) {"""

man_header_8_each="""
        if (strcmp(comp_name, "%s") == 0) {
            return %s_component_stub::from_config(config);
        }"""

man_header_9="""
        assert(false);
        return nullptr;
    }

    void destroy_entity_instance(c_entity ce) {"""

man_header_10_each="""
        %s_component_man.destroy_entity_instance(ce);"""

man_header_11="""
    }
};
"""

header_template_1="""#pragma once

/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <libconfig.h>
#include <memory>

#include "component_manager.h"

struct %s_component_manager : component_manager {
    struct instance_data {
        c_entity *entity;
"""

header_template_2_each_body="""        %(type)s *%(name)s;
"""

header_template_3="""    } instance_pool;

    void create_component_instance_data(unsigned count) override;
    void destroy_instance(instance i) override;
    void entity(c_entity e) override;

    instance_data get_instance_data(c_entity e) {
        instance_data d{};
        auto inst = lookup(e);

        d.entity = instance_pool.entity + inst.index;"""

header_template_4_each_body="""
        d.%(name)s = instance_pool.%(name)s + inst.index;
"""

header_template_5="""
        return d;
    }
};
"""

header_template_6="""
struct %s_component_stub : component_stub {
    %s_component_stub() = default;
"""

header_template_7_each_stub="""
    %(otype)s %(name)s{};
"""

header_template_8="""
    void
    assign_component_to_entity(c_entity entity) override;

    static
    std::unique_ptr<component_stub>
    from_config(const config_setting_t *%s_config) {
        auto %s_stub = std::make_unique<%s_component_stub>();
"""

header_template_9_each_stub="""
        auto %(name)s_member = config_setting_get_member(%(comp_name)s_config, "%(name)s");
        %(comp_name)s_stub->%(name)s = config_setting_get_%(type)s(%(name)s_member);
"""

header_template_10="""
        return std::move(%s_stub);
    }
};
"""


impl_template_1="""/* THIS FILE IS AUTOGENERATED BY gen/gen_component_impl.py; DO NOT HAND-MODIFY */

#include <algorithm>
#include <string.h>
#include <memory>

#include "../memory.h"
#include "%s_component.h"
#include "component_system_manager.h"

extern component_system_manager component_system_man;

void
%s_component_manager::create_component_instance_data(unsigned count) {
    if (count <= buffer.allocated)
        return;

    component_buffer new_buffer{};
    instance_data new_pool{};

    size_t size = sizeof(c_entity) * count;
"""

impl_template_2_each_body="""    size = sizeof(%(type)s) * count + align_size<%(type)s>(size);
"""

impl_template_3="""    size += 16;   // for worst-case misalignment of initial ptr

    new_buffer.buffer = malloc(size);
    new_buffer.num = buffer.num;
    new_buffer.allocated = count;
    memset(new_buffer.buffer, 0, size);

    new_pool.entity = align_ptr((c_entity *)new_buffer.buffer);
"""

impl_template_4_each_body="""    new_pool.%(name)s = align_ptr((%(type)s *)(new_pool.%(prev)s + count));
"""

impl_template_5="""
    memcpy(new_pool.entity, instance_pool.entity, buffer.num * sizeof(c_entity));
"""

impl_template_6_each_body="""    memcpy(new_pool.%(name)s, instance_pool.%(name)s, buffer.num * sizeof(%(type)s));
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

impl_template_8_each_body="""    instance_pool.%(name)s[i.index] = instance_pool.%(name)s[last_index];
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
%s_component_stub::assign_component_to_entity(c_entity entity) {
    auto &man = component_system_man.managers.%s_component_man;

    man.assign_entity(entity);
    auto data = man.get_instance_data(entity);        
"""

impl_template_10_each_body="""
    *data.%(name)s = %(default)s;
"""

impl_template_11_each_stub="""
    *data.%(name)s = %(pre)s%(name)s%(extra)s;
"""

impl_template_12="""};
"""


import os
import sys
import glob

def main():

    files = [f for f in glob.glob('gen/*') if not f.endswith('.py')]

    comp_names = sorted([f.replace('gen/', '').replace('gen\\', '') for f in files])
    print("  header: src/component/component_managers.h")
    with open("src/component/component_managers.h", "w") as g:
            g.write(man_header_1)
            for comp_name in comp_names:
                g.write(man_header_2_each % comp_name)
            g.write(man_header_3)
            for comp_name in comp_names:
                g.write(man_header_4_each % (comp_name))
            g.write(man_header_5)
            for comp_name in comp_names:
                g.write(man_header_6_each % (comp_name, comp_name))
            g.write(man_header_7)
            for comp_name in comp_names:
                g.write(man_header_8_each % (comp_name, comp_name))
            g.write(man_header_9)
            for comp_name in comp_names:
                g.write(man_header_10_each % comp_name)
            g.write(man_header_11)

    for fl in files:
        print("Genning from %s to " % fl)
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
                    stub_fields.append({'otype': parts[1], 'type': parts[2], 'pre': parts[3], 'name': parts[4], 'extra': parts[5], 'comp_name': component_name})

        print("  header: src/component/%s_component.h" % component_name)
        with open("src/component/%s_component.h" % component_name, "w") as g:
            g.write(header_template_1 % component_name)
            for fi in body_fields:
                g.write(header_template_2_each_body % fi)
            g.write(header_template_3)
            for fi in body_fields:
                g.write(header_template_4_each_body % fi)
            g.write(header_template_5)
            g.write(header_template_6 % (component_name, component_name))
            for fi in stub_fields:
                g.write(header_template_7_each_stub % fi)
            g.write(header_template_8 % (component_name, component_name, component_name))
            for fi in stub_fields:
                g.write(header_template_9_each_stub % fi)
            g.write(header_template_10 % component_name)

        print("  source: src/component/%s_component.cc" % component_name)
        with open("src/component/%s_component.cc" % component_name, "w") as g:
            g.write(impl_template_1 % (component_name, component_name))
            for fi in body_fields:
                g.write(impl_template_2_each_body % fi)
            g.write(impl_template_3)
            for fi in body_fields:
                g.write(impl_template_4_each_body % fi)
            g.write(impl_template_5)
            for fi in body_fields:
                g.write(impl_template_6_each_body % fi)
            g.write(impl_template_7 % component_name)
            for fi in body_fields:
                g.write(impl_template_8_each_body % fi)
            g.write(impl_template_9 % (component_name, component_name, component_name, component_name))
            for fi in body_fields:
                print (body_fields)
                g.write(impl_template_10_each_body % fi)
            for fi in stub_fields:
                g.write(impl_template_11_each_stub % fi)
            g.write(impl_template_12)

    return 0;

if __name__ == '__main__':
    sys.exit(main())
