#pragma once

#include <physics/common.hpp>
#include <voxel/common.hpp>

#include <particle/minivox_colored.hpp>

namespace Particle
{

class TexturedMinivox: public ParticleMotion
{
    public:

        Components::TexturedVoxelComponent voxel;

        void set_size(float size);

        void tick();

        inline void reset();
        
        TexturedMinivox();
};

}

#include <common/template/object_list.hpp>

namespace Particle
{

class TexturedMinivox_list: public Simple_object_list<TexturedMinivox, MINIVOX_MAX>
{
    private:
        const char* name() { return "TexturedMinivox"; }
    public:
        void tick();

        TexturedMinivox_list() { print_list((char*)this->name(), this); }
};
    
}   // Particle
