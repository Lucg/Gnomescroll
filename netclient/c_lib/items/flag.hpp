#pragma once

#include <c_lib/game/game.hpp>
#include <voxel/voxel_model.hpp>
#include <common/enum_types.hpp>

const int N_FLAGS = N_TEAMS;

const int FLAG_PART_NUM = 1;
extern VoxDat flag_vox_dat;

enum FLAG_PARTS {
    FLAG_PART_CLOTH
};

const float FLAG_HEIGHT = 1.0f;

class Flag;

class Flag {
    public:
        float x,y,z;
        //float vx,vy,vz;
        float theta,phi;

        int id;
        int team;
        Object_types type;

        Voxel_model* vox;

        void set_position(float x, float y, float z);
        void tick();
        
        Flag(int id, int team);
        ~Flag();
};

