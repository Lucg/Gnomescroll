/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <physics/common.hpp>
#include <entity/constants.hpp>
#include <entity/components/physics.hpp>
#include <physics/quadrant.hpp>

namespace Components
{

class PositionComponent: public PhysicsComponent
{
    protected:
        Vec3 position;
        Vec3 angles;
        bool position_changed;
        bool angles_changed;

        #if DC_CLIENT
        // interpolation
        int tick;
        int previous_tick;
        Vec3 previous_position;
        Vec3 computed_position;
        #endif

    public:

    virtual bool get_position_changed()
    {
        return this->position_changed;
    }

    virtual void set_position_changed(bool changed)
    {
        this->position_changed = changed;
    }

    virtual bool get_angles_changed()
    {
        return this->angles_changed;
    }

    virtual void set_angles_changed(bool changed)
    {
        this->angles_changed = changed;
    }

    virtual bool get_momentum_changed()
    {
        return false;
    }

    virtual void set_momentum_changed(bool changed)
    {
    }

    virtual Vec3 get_position()
    {
        #if DC_SERVER
        return this->position;
        #endif
        #if DC_CLIENT
        return this->computed_position;
        #endif
    }

    virtual bool set_position(const Vec3& position)
    {
        Vec3 p = translate_position(position);
        #if DC_CLIENT
        this->previous_position = this->position;
        this->previous_tick = this->tick;
        #endif
        if (vec3_equal_approximate(this->position, p))
            return false;
        this->position = p;
        #if DC_SERVER
        this->position_changed = true;
        #endif
        return true;
    }

    virtual Vec3 get_momentum()
    {
        return NULL_MOMENTUM;
    }

    virtual bool set_momentum(const Vec3& momentum)
    {
        return false;
    }

    virtual Vec3 get_angles()
    {
        return this->angles;
    }

    virtual bool set_angles(const Vec3& angles)
    {
        if (vec3_equal_approximate(this->angles, angles))
            return false;
        this->angles = angles;
        this->angles_changed = true;
        return true;
    }

    #if DC_CLIENT
    void call()
    {
        float delta = float(this->tick - this->previous_tick) / float(MOB_BROADCAST_RATE);
        Vec3 end = quadrant_translate_position(this->previous_position, this->position);
        const float snap_distance = 16.0f;
        const float snap_distance_sq = snap_distance * snap_distance;
        if (vec3_distance_squared(this->previous_position, end) > snap_distance_sq)
            this->computed_position = this->position;
        else
        {
            Vec3 new_position = vec3_interpolate(this->previous_position, end, delta);
            new_position = translate_position(new_position);
            if (vec3_equal_approximate(new_position, this->computed_position))
            {
                this->tick++;
                return;
            }
            this->computed_position = new_position;
        }
        this->position_changed = true;
        GS_ASSERT(vec3_is_valid(this->computed_position));
        GS_ASSERT(is_boxed_position(this->computed_position));
        this->tick++;
    }
    #endif

    void load_settings_from(const Component* component)
    {
        // no settings
    }

    PositionComponent() :
        PhysicsComponent(COMPONENT_Position)
    {
        this->_init();
    }

    explicit PositionComponent(ComponentType type) :
        PhysicsComponent(type)
    {
        this->_init();
    }

    virtual ~PositionComponent() {}

    private:
    void _init()
    {
        this->position = NULL_POSITION;
        this->angles = NULL_ANGLES;
        this->position_changed = true;
        this->angles_changed = true;

        #if DC_CLIENT
        this->tick = 0;
        this->previous_tick = 0;
        this->previous_position = NULL_POSITION;
        this->computed_position = NULL_POSITION;
        #endif
    }
};

} // Components
