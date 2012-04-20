#pragma once

#include <c_lib/voxel/voxel_model.hpp>
#include <c_lib/entity/component/component.hpp>

namespace Components
{

class VoxelModelComponent: public Component
{
    private:
            // Initializes vox model properties. almost always desired
        void set_properties()
        {
            this->vox->set_hitscan(this->init_hitscan);
            this->vox->register_hitscan();
            #if DC_CLIENT
            this->vox->set_draw(this->init_draw);
            #endif
        }

    public:
        Voxel_model* vox;
        VoxDat* vox_dat;
        bool init_hitscan;
        bool init_draw;

        float get_radius()
        {
            return vox->get_part(0)->radius;
        }
        
        float get_radius(int part)
        {
            return vox->get_part(part)->radius;
        }

        Vec3 get_center()
        {
            return this->vox->get_part(0)->get_center();
        }
        
        Vec3 get_center(int part)
        {
            return this->vox->get_part(part)->get_center();
        }

        void ready(Vec3 position, float theta, float phi)
        {
            this->vox = new Voxel_model(this->vox_dat, this->object->id, this->object->type);
            this->set_properties();
            vox->update(position.x, position.y, position.z, theta, phi);
            vox->set_hitscan(this->init_hitscan);
            vox->set_draw(this->init_draw);
        }

        void ready(Vec3 position, float theta, float phi, int team)
        {
            this->vox = new Voxel_model(this->vox_dat, this->object->id, this->object->type, team);
            this->set_properties();
            vox->update(position.x, position.y, position.z, theta, phi);
            vox->set_hitscan(this->init_hitscan);
            vox->set_draw(this->init_draw);
        }

        void freeze()
        {
            this->vox->freeze();
        }

        void update(Vec3 position, float theta, float phi, bool state_changed);
        void force_update(Vec3 position, float theta, float phi, bool state_changed);

        

    ~VoxelModelComponent()
    {
        if (this->vox != NULL) delete this->vox;
    }

    VoxelModelComponent()
    : Component(COMPONENT_VOXEL_MODEL, COMPONENT_INTERFACE_VOXEL_MODEL),
    vox(NULL), vox_dat(NULL),
    init_hitscan(false), init_draw(false)
    {}
};

} // Objects
