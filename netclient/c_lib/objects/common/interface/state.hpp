#pragma once

#include <c_lib/common/enum_types.hpp>
#include <c_lib/physics/verlet.hpp>
#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/items/constants.hpp>
#include <c_lib/ray_trace/handlers.hpp>

#include <limits.h>
#include <float.h>

typedef enum
{
    COINS_NOBODY  = 0,
    COINS_ANYONE  = 1,
    COINS_ENEMIES = 2,
    COINS_ALLIES  = 4,
    COINS_OWNER   = 8,
    COINS_ANYONE_WHEN_UNOWNED = 16
} COIN_RULES;

/* Holds constants that are dat-configurable */
class ObjectData
{
    public:
        Object_types type;

        // spatial
        //float height;

        // physics
        float damp;
        float mass;
        bool broadcast_state_change;

        // tick lifespan
        int ttl_max;

        // TODO: MOVE
        // explosion
        bool blow_up_on_death;
        float explosion_radius;
        int explosion_damage;
        float explosion_proximity_radius;
        float motion_proximity_radius;
        
        // coins
        unsigned int cost;
        unsigned int reward;
        int coin_rule;

        // firing
        unsigned int fire_rate_limit;
        float sight_range;
        float accuracy_bias;
        bool attack_enemies;    // TODO -- use bit mask flags
        bool attack_random;
        bool suicidal;  // can kill owner

    //TODO:
    // object metadata store that objects can read for defaults

    ObjectData()
    :
    type(OBJ_TYPE_NONE),
    damp(1.0f), mass(1.0f), broadcast_state_change(false),
    ttl_max(100),
    blow_up_on_death(false),
    explosion_radius(1.0f),
    explosion_damage(0),
    explosion_proximity_radius(1.0f),
    motion_proximity_radius(1.0f),
    cost(999999), reward(0), coin_rule(COINS_NOBODY),
    fire_rate_limit(1)
    {}
};

// encapsulates all information needed for any object,
// merging ObjectData (dat constants) with extra non-dat state
class ObjectState: public ObjectData
{
    public:
        // general status 
        int id;

        // tick lifespan
        int ttl;

        // pickup
        bool broadcast_death;

        // firing
        unsigned int fire_tick; // dont move yet; things besides target acquisition may use this (altho they should probably be bundled into that

    ObjectState()
    : ObjectData(),
    id(-1),
    ttl(0),
    broadcast_death(false), fire_tick(0)
    {
    }
};

/*
 * NULL values. these are not like NO_OWNER, which is a legitimate value.
 * These are values that should fail in any comparison to real data
 * These values should never be sent over the network. They are not intended to support
 * the limited ranges of low-byte values used in packets.
 */
const int NULL_OWNER = INT_MAX; // owners are ids of agents. they will never be INT_MAX
const int NULL_TEAM = SHRT_MAX;  // team ids. will never be SHRT_MAX.
const unsigned int NULL_TEAM_INDEX = UINT_MAX;
const int NULL_HEALTH = INT_MAX;
const Vec3 NULL_POSITION = vec3_init(FLT_MAX,FLT_MAX,FLT_MAX);
const Vec3 NULL_MOMENTUM = vec3_init(0,0,0);
const Vec3 NULL_ANGLES = vec3_init(0,0,0);
const float NULL_HEIGHT = 1.0f;

/* provide virtual getters that support all data operations but will return invalid values */
class OwnedDefault
{
    public:
    int get_owner()
    { return NULL_OWNER; }
    void set_owner(int id, Object_types type, int owner) {}
};

class TeamDefault
{
    public:
    int get_team()
    { return NULL_TEAM; }
    void set_team(int team) {}

    unsigned int get_team_index()
    { return NULL_TEAM_INDEX; }
    void set_team_index(unsigned int team_index)
    {}
};

class HealthDefault
{
    public:
    //int get_health()
    //{ return NULL_HEALTH; }
    int take_damage(int dmg)
    { return NULL_HEALTH; }
    bool is_dead()
    { return false; }
    bool did_die()
    { return false; }
};

class SpatialDefault
{
    public:

    Vec3 get_position()
    { return NULL_POSITION; }
    Vec3 get_momentum()
    { return NULL_MOMENTUM; }
    bool set_position(float x, float y, float z)
    { return false; }
    void set_momentum(float mx, float my, float mz)
    {}
    float get_height()
    { return NULL_HEIGHT; }
    void set_angles(float theta, float phi, float rho)
    {}
    Vec3 get_angles()
    { return NULL_ANGLES; }
};
