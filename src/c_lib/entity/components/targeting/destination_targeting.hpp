#pragma once

#include <entity/component/component.hpp>

namespace Components
{

class DestinationTargetingComponent: public TargetingComponent
{
    public:
        float stop_proximity;
    
        float speed;
        int max_z_diff;

        float destination_choice_x;
        float destination_choice_y;

        Vec3 destination;
        bool at_destination;
        bool en_route;
        int ticks_to_destination;

        int max_lock_ticks;
        int ticks_locked;

        void check_target_alive();

        void choose_destination();

        void check_at_destination();

        void orient_to_target(Vec3 camera_position);
        
        void lock_target(Vec3 camera_position){}    // TODO -- change targeting interface to not require this

        bool move_on_surface();

        void broadcast_target_choice();
        void broadcast_destination();
        void broadcast_remove_target();

        void call();
    
    virtual ~DestinationTargetingComponent() {}
    
    DestinationTargetingComponent()
    : TargetingComponent(COMPONENT_DESTINATION_TARGETING),
    stop_proximity(3.0f),
    speed(1.0f), max_z_diff(128),
    destination_choice_x(1.0f), destination_choice_y(1.0f), 
    destination(vec3_init(0,0,0)), at_destination(false), en_route(false),
    ticks_to_destination(1),
    max_lock_ticks(0), ticks_locked(0)
    {}
};

} // Components
