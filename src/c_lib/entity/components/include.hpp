#pragma once

#include <entity/components/physics.hpp>
#include <entity/components/physics/position.hpp>
#include <entity/components/physics/position_momentum.hpp>
#include <entity/components/physics/verlet.hpp>
#include <entity/components/health.hpp>
#include <entity/components/health/hit_points.hpp>
#include <entity/components/health/ttl.hpp>
#include <entity/components/owner.hpp>
#include <entity/components/spawner.hpp>
#include <entity/components/spawner/monster_spawner.hpp>
#include <entity/components/spawn_child.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/targeting.hpp>
#include <entity/components/targeting/motion_targeting.hpp>
#include <entity/components/targeting/weapon_targeting.hpp>
#include <entity/components/targeting/destination_targeting.hpp>
#include <entity/components/targeting/agent_targeting.hpp>
#include <entity/components/state_machine.hpp>
#include <entity/components/waiting.hpp>
#include <entity/components/rate_limit.hpp>
#if DC_CLIENT
# include <entity/components/animation.hpp>
#endif
#if DC_SERVER
# include <entity/components/explosion.hpp>
# include <entity/components/item_drop.hpp>
# include <entity/components/spawner/agent_spawner.hpp>
# include <entity/components/healer.hpp>
#endif
