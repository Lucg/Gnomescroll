#include "box.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/entities/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#include <entity/components/targeting.hpp>
#include <entity/components/targeting/weapon_targeting.hpp>
#include <entity/components/targeting/motion_targeting.hpp>
#include <agent/_interface.hpp>

namespace Entities
{

void load_mob_robot_box_data()
{
    EntityType type = ENTITY_MONSTER_BOX;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = MONSTER_BOX_HEIGHT;
    dims->camera_height = MONSTER_BOX_CAMERA_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::robot_box;
    vox->init_hitscan = MONSTER_BOX_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_BOX_INIT_WITH_DRAW;

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer
    auto health = ADD_COMPONENT(HitPoints);
    int health_amt = randrange(MONSTER_BOX_HEALTH_MIN, MONSTER_BOX_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    auto target = ADD_COMPONENT(WeaponTargeting);
    target->target_acquisition_failure_rate = MONSTER_BOX_TARGET_ACQUISITION_FAILURE_RATE;
    target->fire_rate_limit = MONSTER_BOX_FIRE_RATE_LIMIT;
    target->uses_bias = MONSTER_BOX_USES_BIAS;
    target->accuracy_bias = MONSTER_BOX_ACCURACY_BIAS;
    target->sight_range = MONSTER_BOX_FIRING_SIGHT_RANGE;
    target->attack_at_random = MONSTER_BOX_ATTACK_AT_RANDOM;
    // we dont have ID yet, need to set that in the ready() call
    target->attacker_properties.type = ENTITY_MONSTER_BOX;
    target->attacker_properties.block_damage = MONSTER_BOX_TERRAIN_DAMAGE;
    target->attacker_properties.agent_damage_min = MONSTER_BOX_AGENT_DAMAGE_MIN;
    target->attacker_properties.agent_damage_max = MONSTER_BOX_AGENT_DAMAGE_MAX;
    //target->attacker_properties.voxel_damage_radius = MONSTER_BOX_VOXEL_DAMAGE_RADIUS;
    target->attacker_properties.agent_protection_duration = MONSTER_BOX_AGENT_IMMUNITY_DURATION;
    target->attacker_properties.terrain_modification_action = TMA_MONSTER_BOX;
    target->fire_delay_max = MONSTER_BOX_FIRE_DELAY_MAX;

    auto motion = ADD_COMPONENT(MotionTargeting);
    motion->speed = MONSTER_BOX_SPEED;
    motion->max_z_diff = MONSTER_BOX_MOTION_MAX_Z_DIFF;

    #if DC_SERVER
    ADD_COMPONENT(SpawnChild);

    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.2f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.05f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.01f);

    item_drop->drop->set_max_drop_amounts("small_charge_pack", 1);
    item_drop->drop->add_drop("small_charge_pack", 1, 0.02f);

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.5f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = MONSTER_BOX_ANIMATION_COLOR;
    anim->count = MONSTER_BOX_ANIMATION_COUNT;
    //anim->count_max = MONSTER_BOX_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_BOX_ANIMATION_SIZE;
    anim->force = MONSTER_BOX_ANIMATION_FORCE;
    #endif
}

void ready_mob_robot_box(Entity* entity)
{
    auto target = GET_COMPONENT(WeaponTargeting, entity);
    target->attacker_properties.id = entity->id;


    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_mob_robot_box(Entity* entity)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, entity);
    GS_ASSERT(item_drop != NULL);
    if (item_drop != NULL) item_drop->drop_item();

    entity->broadcastDeath();

    auto child = GET_COMPONENT(SpawnChild, entity);
    if (child != NULL) child->notify_parent_of_death();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    if (vox->vox != NULL)
    {
        auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
        anim->explode(vox->get_center());
    }
    #endif
}

#if DC_SERVER
void server_tick_mob_robot_box(Entity* entity)
{
    // must stay on ground -- apply terrain collision
    // wander randomly (TODO: network model with destinations)
    // TODO -- aggro component

    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    const Vec3 position = physics->get_position();
    Vec3 camera_position = position;

    auto dims = GET_COMPONENT_INTERFACE(Dimension, entity);
    camera_position.z += dims->get_camera_height();

    auto weapon = GET_COMPONENT(WeaponTargeting, entity);
    auto motion = GET_COMPONENT(MotionTargeting, entity);

    Agents::Agent* agent = NULL;
    if (weapon->locked_on_target)
    {   // target locked
        // query agent
        if (weapon->target_type == ENTITY_AGENT)
            agent = Agents::get_agent((AgentID)weapon->target_id);
        // check target still exists
        if (!weapon->target_is_visible(camera_position))
            weapon->locked_on_target = false;
    }

    if (!weapon->locked_on_target)
    {   // no target found
        // look for target
        weapon->lock_target(camera_position);
        if (weapon->locked_on_target)
            agent = Agents::get_agent((AgentID)weapon->target_id);
    }

    if (weapon->target_type != NULL_ENTITY_TYPE)
    {   // target found
        // lock target
        weapon->locked_on_target = true;
        motion->en_route = false;
    }

    if (weapon->locked_on_target)
    {   // target is locked
        // face target
        if (agent != NULL) // TODO -- multiple target types
        {
            if (vec3_length_squared(weapon->target_direction))
            {
                float theta,phi;
                vec3_to_angles(weapon->target_direction, &theta, &phi);
                physics->set_angles(vec3_init(theta, phi, 0));
            }
            weapon->tick();
            if (weapon->can_lock_part())
            {
                weapon->lock_target_part(camera_position);
                weapon->add_random_fire_delay();
            }
            if (weapon->can_fire())
                weapon->fire_on_target(camera_position);
        }
    }
    else if (motion->en_route)
    {   // face destination
        float theta, phi;
        vec3_to_angles(motion->target_direction, &theta, &phi);
        physics->set_angles(vec3_init(theta, phi, 0));
    }

    if (!motion->en_route && !weapon->locked_on_target)
    {   // no destination, no target
        // choose destination
        Vec3 destination;
        const int walk_len = MONSTER_BOX_WALK_RANGE;
        int dx = randrange(0,walk_len) - walk_len/2;
        int dy = randrange(0,walk_len) - walk_len/2;
        destination = vec3_add(position, vec3_init(float(dx)+randf(), float(dy)+randf(),0));
        // clamp
        destination.z = t_map::get_nearest_surface_block(destination.x, destination.y, destination.z);
        destination.z = GS_MAX(0, destination.z);

        Vec3 direction = vec3_sub(destination, position);
        float len = vec3_length(direction);

        motion->ticks_to_destination = int(ceilf(len/motion->speed));
        motion->destination = translate_position(destination);
        motion->en_route = true;
        motion->at_destination = false;

        if (len)
        {
            direction.z = 0;
            if (vec3_length_squared(direction))
                direction = vec3_normalize(direction);
            motion->target_direction = direction;
        }
    }

    if (!motion->at_destination)
    {   // check if at destination
        float dist = vec3_distance_squared(motion->destination, position);
        if (dist < 1.0f)    // TODO Margin
        {
            motion->en_route = false;
            motion->at_destination = true;
            physics->set_momentum(vec3_init(0));
        }
    }

    if (motion->en_route)
    {   // move towards destination
        motion->move_on_surface();
        // face in direction of movement
        float theta, phi;
        vec3_to_angles(motion->target_direction, &theta, &phi);
        physics->set_angles(vec3_init(theta, phi, 0));
    }

    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    IF_ASSERT(limiter == NULL) return;
    if (limiter->allowed()) entity->broadcastState();
}
#endif

#if DC_CLIENT
void client_tick_mob_robot_box(Entity* entity)
{
}
#endif

void tick_mob_robot_box(Entity* entity)
{
    #if DC_SERVER
    server_tick_mob_robot_box(entity);
    #endif
    #if DC_CLIENT
    client_tick_mob_robot_box(entity);
    #endif
}

void update_mob_robot_box(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
}

} // Entities