/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <agent/agent.hpp>
#include <agent/constants.hpp>
#include <physics/common.hpp>


typedef enum
{
    COLLISION_STATUS_STUCK,
    COLLISION_STATUS_COLLIDED,
    COLLISION_STATUS_CLEAR
} CollisionStatus;

bool object_collides_terrain(Vec3 position, float height, float radius);
inline bool collides_with_terrain(float radius, float box_h, float x, float y, float z);
inline bool on_ground(float radius, const struct Vec3& p);
inline int clamp_to_ground(float radius, float x, float y, float z);
inline bool can_stand_up(float radius, float box_h, float x, float y, float z);

void apply_control_state(const ControlState& cs, float speed, float jump_force,
                         Vec3& position, Vec3& velocity, float ground_distance,
                         CollisionStatus collision);

CollisionStatus move_with_terrain_collision(
    const BoundingBox& box, Vec3& position, Vec3& velocity, float& ground_distance);

CollisionStatus move_with_terrain_collision(
    const BoundingBox& box, Vec3& position, Vec3& velocity,
    float& ground_distance, Agents::Agent** collided_agent);

bool collides_with_agents(const BoundingBox& box, const Vec3& position,
                          Agents::Agent*& agent);

namespace Agents
{

bool agent_collides_terrain(class Agent* a);

class AgentState agent_tick(const struct ControlState& _cs,
                            const struct BoundingBox& box,
                            class AgentState as);

}   // Aents
