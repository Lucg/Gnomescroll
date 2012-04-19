#pragma once

#include <c_lib/entity/constants.hpp>
#include <c_lib/entity/components/spawner.hpp>

namespace Components
{

class AgentSpawnerComponent: public SpawnerComponent
{
    public:
        float radius;
        void get_spawn_point(Vec3 position, int spawned_object_height, Vec3* spawn_point);
    
    AgentSpawnerComponent()
    : SpawnerComponent(COMPONENT_AGENT_SPAWNER),
    radius(1.0f)
    {}
};

}; // Objects
