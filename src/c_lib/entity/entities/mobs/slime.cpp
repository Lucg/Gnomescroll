#include "slime.hpp"

#include <entity/entity/entity.hpp>
#include <entity/constants.hpp>
#include <entity/entities/mobs/constants.hpp>
#include <entity/components/health.hpp>
#include <entity/components/dimension.hpp>
#include <entity/components/voxel_model.hpp>
#if DC_SERVER
# include <entity/entities/mobs/state_machines.hpp>
#endif

namespace Entities
{

static void slime_state_router(class Entity*, EntityState state);

void load_mob_slime_data()
{
    EntityType type = ENTITY_MONSTER_SLIME;

    entity_data->begin_attaching_to(type);

    ADD_COMPONENT(PositionMomentum);

    auto dims = ADD_COMPONENT(Dimension);
    dims->height = MONSTER_SLIME_HEIGHT;

    auto vox = ADD_COMPONENT(VoxelModel);
    vox->vox_dat = &VoxDats::slime;
    vox->init_hitscan = MONSTER_SLIME_INIT_WITH_HITSCAN;
    vox->init_draw = MONSTER_SLIME_INIT_WITH_DRAW;

    #if DC_CLIENT
    ADD_COMPONENT(HitPoints);
    #endif
    #if DC_SERVER   // health will be set by packet initializer in client, so dont initialize it here
    auto health = ADD_COMPONENT(HitPoints);
    int health_amt = randrange(MONSTER_SLIME_HEALTH_MIN, MONSTER_SLIME_HEALTH_MAX);
    health->health = health_amt;
    health->health_max = health_amt;
    #endif

    auto dest = ADD_COMPONENT(DestinationTargeting);
    dest->sight_range = MONSTER_SLIME_MOTION_PROXIMITY_RADIUS;
    dest->destination_choice_x = MONSTER_SLIME_WALK_RANGE;
    dest->destination_choice_y = MONSTER_SLIME_WALK_RANGE;
    dest->speed = MONSTER_SLIME_WALK_SPEED;
    dest->max_z_diff = MONSTER_SLIME_MOTION_MAX_Z_DIFF;

    auto agent = ADD_COMPONENT(AgentTargeting);
    agent->sight_range = MONSTER_SLIME_MOTION_PROXIMITY_RADIUS;
    agent->speed = MONSTER_SLIME_CHASE_SPEED;
    agent->max_z_diff = MONSTER_SLIME_MOTION_MAX_Z_DIFF;
    agent->max_lock_ticks = MONSTER_SLIME_MAX_TARGET_LOCK_TICKS;
    agent->proximity_radius = MONSTER_SLIME_AGENT_STOP_PROXIMITY_RADIUS;
    agent->jump_force = MONSTER_SLIME_JUMP_FORCE;
    agent->set_jump_cooldowns(MONSTER_SLIME_JUMP_COOLDOWN_EN_ROUTE,
                              MONSTER_SLIME_JUMP_COOLDOWN_NEARBY);
    agent->attack_rate = (3 * MONSTER_SLIME_JUMP_COOLDOWN_NEARBY) / 4;
    agent->attack_damage = 2;
    agent->jump_near_player = true;

    auto waiting = ADD_COMPONENT(Waiting);
    waiting->wait_time = MONSTER_SLIME_IDLE_TIME;

    #if DC_SERVER
    auto limiter = ADD_COMPONENT(RateLimit);
    limiter->limit = MOB_BROADCAST_RATE;

    auto item_drop = ADD_COMPONENT(ItemDrop);
    item_drop->drop->set_max_drop_types(2);
    item_drop->drop->set_max_drop_amounts("synthesizer_coin", 3);
    item_drop->drop->add_drop("synthesizer_coin", 1, 0.3f);
    item_drop->drop->add_drop("synthesizer_coin", 2, 0.1f);
    item_drop->drop->add_drop("synthesizer_coin", 3, 0.05f);
    item_drop->drop->set_max_drop_amounts("plasma_grenade", 10);
    item_drop->drop->add_drop_range("plasma_grenade", 1, 10, 0.8f);

    auto state = ADD_COMPONENT(StateMachine);
    state->state = STATE_WAITING;
    state->router = &slime_state_router;

    auto knockback = ADD_COMPONENT(Knockback);
    knockback->weight = 1.0f;
    #endif

    #if DC_CLIENT
    auto anim = ADD_COMPONENT(Animation);
    anim->color = MONSTER_SLIME_ANIMATION_COLOR;
    anim->count = MONSTER_SLIME_ANIMATION_COUNT;
    anim->count_max = MONSTER_SLIME_ANIMATION_COUNT_MAX;
    anim->size = MONSTER_SLIME_ANIMATION_SIZE;
    anim->force = MONSTER_SLIME_ANIMATION_FORCE;
    #endif
}

void ready_mob_slime(Entity* entity)
{

    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);

    Vec3 position = physics->get_position();
    Vec3 angles = physics->get_angles();

    vox->ready(position, angles.x, angles.y);

    #if DC_SERVER
    entity->broadcastCreate();
    #endif
}

void die_mob_slime(Entity* entity)
{
    #if DC_SERVER
    // drop item
    auto item_drop = GET_COMPONENT_INTERFACE(ItemDrop, entity);
    IF_ASSERT(item_drop == NULL) return;
    item_drop->drop_item();

    // notify clients
    entity->broadcastDeath();
    #endif

    #if DC_CLIENT
    // explosion animation
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);
    IF_ASSERT(vox->vox == NULL) return;
    auto anim = GET_COMPONENT_INTERFACE(Animation, entity);
    anim->explode_random(vox->get_center());
    #endif
}

#if DC_SERVER
static void slime_state_router(class Entity* entity, EntityState state)
{
    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (state)
    {
        case STATE_CHASE_AGENT:
            if (machine->state == STATE_WAITING)
                waiting_to_chase_agent(entity);
            //else if (machine->state == STATE_IN_TRANSIT)
                //in_transit_to_chase_agent(entity);
            break;

        //case STATE_IN_TRANSIT:
            //if (machine->state == STATE_WAITING)
                //waiting_to_in_transit(entity);
            //else if (machine->state == STATE_CHASE_AGENT)
                //chase_agent_to_in_transit(entity);
            //break;

        case STATE_WAITING:
            if (machine->state == STATE_CHASE_AGENT)
                chase_agent_to_waiting(entity);
            //else if (machine->state == STATE_IN_TRANSIT)
                //in_transit_to_waiting(entity);
            break;

        case STATE_IN_TRANSIT:
        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }
}
#endif

#if DC_SERVER
void relax_slimes(Entity* entity)
{   // TODO -- this is inefficient -- we need to unpack all the physics
    // components into one array and work over that.
    const float relax_distance = 0.85f;
    const float relax_distance_sq = relax_distance * relax_distance;
    const float relax_force = 0.1f;
    int slime_count = 0;
    int slime_max = 0;
    class Entity* slimes = get_all(ENTITY_MONSTER_SLIME, slime_max, slime_count);
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    Vec3 position = physics->get_position();
    Vec3 momentum = physics->get_momentum();
    for (int i=0, j=0; i<slime_max && j<slime_count; i++)
    {
        if (slimes[i].id == EntityList::null_id)
            continue;
        j++;
        Entity* slime = &slimes[i];
        if (slime->id == entity->id) continue;
        auto slime_physics = GET_COMPONENT_INTERFACE(Physics, slime);
        Vec3 p = slime_physics->get_position();
        float dist_sq = vec3_distance_squared(position, p);
        if (dist_sq > relax_distance_sq)
            continue;
        if (dist_sq == 0)
        {
            Vec3 push = vec3_init((2*randf()-1), (2*randf()-1), 0.0f);
            push = vec3_scalar_mult(push, relax_force);
            momentum = vec3_add(momentum, push);
            continue;
        }
        Vec3 push = vec3_sub(position, p);
        push = vec3_normalize(push);
        push = vec3_scalar_mult(push, relax_force);
        momentum = vec3_add(momentum, push);
    }
    physics->set_momentum(momentum);
}
#endif

void tick_mob_slime(Entity* entity)
{
    #if DC_SERVER
    auto limiter = GET_COMPONENT_INTERFACE(RateLimit, entity);
    if (limiter->allowed()) entity->broadcastState();

    auto machine = GET_COMPONENT_INTERFACE(StateMachine, entity);

    switch (machine->state)
    {
        case STATE_WAITING:
            //waiting_for_agent(entity);
            break;

        //case STATE_IN_TRANSIT:
            //in_transit(entity);
            //break;

        case STATE_CHASE_AGENT:
            chase_agent(entity);
            break;

        case STATE_IN_TRANSIT:
        case STATE_NONE:
            GS_ASSERT(false);
            break;
    }

    if (machine->state != STATE_CHASE_AGENT)
    {   // aggro nearby agent
        auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
        Vec3 position = physics->get_position();

        auto target = GET_COMPONENT(AgentTargeting, entity);
        target->lock_target(position);

        if (target->target_type == ENTITY_AGENT)
            machine->router(entity, STATE_CHASE_AGENT);
    }

    // TODO -- call this rate limited
    relax_slimes(entity);

    #endif
}

void update_mob_slime(Entity* entity)
{
    auto physics = GET_COMPONENT_INTERFACE(Physics, entity);
    auto vox = GET_COMPONENT_INTERFACE(VoxelModel, entity);

    Vec3 angles = physics->get_angles();
    vox->update(physics->get_position(), angles.x, angles.y, physics->get_changed());
    physics->set_changed(false);  // reset changed state
    //printf("Slime position: "); vec3_print(physics->get_position());
}

} // Entities