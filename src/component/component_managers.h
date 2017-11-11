#pragma once

#include <unordered_map>
#include <memory>

#include "component_manager.h"
#include "door_component.h"
#include "gas_producer_component.h"
#include "light_component.h"
#include "physics_component.h"
#include "power_component.h"
#include "power_provider_component.h"
#include "pressure_sensor_component.h"
#include "proximity_sensor_component.h"
#include "reader_component.h"
#include "relative_position_component.h"
#include "renderable_component.h"
#include "sensor_comparator_component.h"
#include "surface_attachment_component.h"
#include "switch_component.h"
#include "type_component.h"

struct component_managers {
    component_managers() = default;

    door_component_manager door_component_man{};
    gas_producer_component_manager gas_producer_component_man{};
    light_component_manager light_component_man{};
    physics_component_manager physics_component_man{};
    power_component_manager power_component_man{};
    power_provider_component_manager power_provider_component_man{};
    pressure_sensor_component_manager pressure_sensor_component_man{};
    proximity_sensor_component_manager proximity_sensor_component_man{};
    reader_component_manager reader_component_man{};
    relative_position_component_manager relative_position_component_man{};
    renderable_component_manager renderable_component_man{};
    sensor_comparator_component_manager sensor_comparator_component_man{};
    surface_attachment_component_manager surface_attachment_component_man{};
    switch_component_manager switch_component_man{};
    type_component_manager type_component_man{};
};
