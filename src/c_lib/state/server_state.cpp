#include "server_state.hpp"

#if DC_CLIENT
dont_include_this_file_in_client
#endif

#include <physics/vec3.hpp>
#include <agent/agent.hpp>
#include <agent/_interface.hpp>
#include <chat/_interface.hpp>
#include <entity/objects.hpp>
#include <entity/components.hpp>

namespace ServerState
{
    bool main_inited = false;
    bool signal_exit = false;
    bool reload_settings = false;
    
    VoxelHitscanList* voxel_hitscan_list = NULL;

    class Entities::Entity* base;

    void init_lists()
    {
        GS_ASSERT(voxel_hitscan_list == NULL);
        voxel_hitscan_list = new VoxelHitscanList;
    }

    void teardown_voxel_lists()
    {
        if (voxel_hitscan_list != NULL) delete voxel_hitscan_list; // must go last
    }

    struct Vec3 get_base_spawn_position()
    {
        // always start the base at the map center in fast map mode
        if (strcmp(Options::map, "art") == 0) return vec3_init(0,0,0);
        
        GS_ASSERT(base != NULL);
        if (base == NULL) return vec3_init(0,0,0);
        
        using Components::DimensionComponent;
        DimensionComponent* dims = (DimensionComponent*)base->get_component_interface(COMPONENT_INTERFACE_DIMENSION);
        GS_ASSERT(dims != NULL);
        int h = 1;
        if (dims != NULL) h = (int)ceil(dims->get_height());
        float x = randrange(0, t_map::map_dim.x-1);
        float y = randrange(0, t_map::map_dim.y-1);
        float z = t_map::get_highest_open_block(x,y, h);
        struct Vec3 p = vec3_init(x+0.5f,y+0.5f,z);
        return p;
    }

    void init_base()
    {
        base = Entities::create(OBJECT_BASE);
        GS_ASSERT(base != NULL);
        if (base == NULL) return;
        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)base->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics != NULL)
            physics->set_position(get_base_spawn_position());
        Entities::ready(base);
    }

    void move_base()
    {
        static int tick = 0;
        if (base == NULL) return;
        tick++;
        if (tick % Options::base_move_rate != 0) return;
        typedef Components::PositionChangedPhysicsComponent PCP;
        PCP* physics = (PCP*)base->get_component(COMPONENT_POSITION_CHANGED);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return;

        int tries = 0;
        static const int MAX_TRIES = 100;
        do
        {
            tries++;
            struct Vec3 new_pos = get_base_spawn_position();
            physics->set_position(new_pos);
        } while (!physics->changed && tries < MAX_TRIES);
        base->broadcastState();
    }

    void check_agents_at_base()
    {
        using Agents::agent_list;
        
        Entities::Entity* base = Entities::get(OBJECT_BASE, 0);
        GS_ASSERT(base != NULL);
        if (base == NULL) return;

        using Components::PhysicsComponent;
        PhysicsComponent* physics = (PhysicsComponent*)base->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
        GS_ASSERT(physics != NULL);
        if (physics == NULL) return;
        Vec3 p = physics->get_position();
        
        using Components::VoxelModelComponent;
        float r = 1.0f;
        VoxelModelComponent* vox = (VoxelModelComponent*)base->get_component_interface(COMPONENT_INTERFACE_VOXEL_MODEL);
        GS_ASSERT(vox != NULL);
        if (vox != NULL) r = vox->get_radius();
        
        agent_list->objects_within_sphere(p.x, p.y, p.z, r);
        for (unsigned int i=0; i<agent_list->n_filtered; i++)
        {
            Agents::Agent* a = agent_list->filtered_objects[i];
            GS_ASSERT(a->id != agent_list->null_id);
            if (a->id == agent_list->null_id) continue;
            a->status.at_base();
        }
    }

    void damage_objects_within_sphere(
        struct Vec3 p, float radius,
        int damage, AgentID owner,
        EntityType inflictor_type, int inflictor_id,
        bool suicidal)   // defaults to true; if not suicidal, agent's with id==owner will be skipped
    {   // agents
        using Agents::agent_list;
        agent_list->objects_within_sphere(p.x, p.y, p.z, radius);
        const float blast_mean = 0;
        const float blast_stddev = 1.0f;
        for (unsigned int i=0; i<agent_list->n_filtered; i++)
        {
            Agents::Agent* a = agent_list->filtered_objects[i];
            if (a->id == agent_list->null_id) continue;
            if (!suicidal && a->id == owner) continue;
            if (!a->point_can_cast(p.x, p.y, p.z, radius)) continue;  // cheap terrain cover check
            int dmg = ((float)damage)*gaussian_value(blast_mean, blast_stddev, agent_list->filtered_object_distances[i] / radius);
            a->status.apply_damage(dmg, owner, inflictor_type);
        }

        const int n_types = 3;
        const EntityType types[n_types] = {
            OBJECT_MONSTER_BOMB, OBJECT_MONSTER_BOX, OBJECT_MONSTER_SPAWNER,
        };
        Entities::damage_objects_within_sphere(types, n_types, p, radius, damage);

    }
        
    // TODO -- move this test/convenince method
    void spawn_items(int n)
    {
        int type = randrange(1,8);
        float x = randf() * t_map::map_dim.x;
        float y = randf() * t_map::map_dim.y;
        float z = 128.0f;
        ItemParticle::create_item_particle(type, x,y,z, 0,0,-3);
    }

    // TODO -- move this
    void spawn_monsters(EntityType type, int n)
    {
        int count = Entities::count(type);
        for (int i=0; i<n-count; i++)
        {
            Entities::Entity* obj = Entities::create(type);
            if (obj == NULL) break;

            Vec3 position;
            position.x = randrange(0, t_map::map_dim.x-1);
            position.y = randrange(0, t_map::map_dim.y-1);
            position.z = t_map::get_highest_open_block(position.x, position.y);

            using Components::PhysicsComponent;
            PhysicsComponent* physics = (PhysicsComponent*)obj->get_component_interface(COMPONENT_INTERFACE_PHYSICS);
            GS_ASSERT(physics != NULL);
            if (physics == NULL)
            {
                Entities::ready(obj);    // sets id
                Entities::destroy(obj);
                break;
            }
            physics->set_position(position);
            Entities::ready(obj);
        }
    }

}   // ServerState

