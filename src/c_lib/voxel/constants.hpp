/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <voxel/common.hpp>
#include <physics/vec3.hpp>

static const int vertex_index2[2*12] = {
    0,1,
    1,2,
    2,3,
    3,0,

    4,5,
    5,6,
    6,7,
    7,4,

    0,4,
    1,5,
    2,6,
    3,7,
    };

static const int v_set[3*8] = {
    0,0,0,
    1,0,0,
    1,1,0,
    0,1,0,
    0,0,1,
    1,0,1,
    1,1,1,
    0,1,1
    };

static const int v_set2[3*8] = {
    -1,-1,-1,
    1,-1,-1,
    1,1,-1,
    -1,1,-1,
    -1,-1,1,
    1,-1,1,
    1,1,1,
    -1,1,1,
    };

static const int q_set[4*6] = {
    4,5,6,7,
    3,2,1,0,
    2,3,7,6,
    0,1,5,4,
    3,0,4,7,
    1,2,6,5,
    };

static const float v_normal_f[6*3] = {
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    };

static const char v_normal_b[6][3] = {
    {0, 0, 1},
    {0, 0, -1},
    {0, 1, 0},
    {0, -1, 0},
    {1, 0, 0},
    {-1, 0, 0},
    };

static const Vec3 v_normal_vec3[6] = {
    {{{ 0.0f, 0.0f, 1.0f  }}},
    {{{ 0.0f, 0.0f, -1.0f }}},
    {{{ 0.0f, 1.0f, 0.0f  }}},
    {{{ 0.0f, -1.0f, 0.0f }}},
    {{{ 1.0f, 0.0f, 0.0f  }}},
    {{{ -1.0f, 0.0f, 0.0f }}},
    };

static const struct Voxels::VoxelNormal voxel_normal_array[6] = {
    {{{0,0,1,0}}},
    {{{0,0,-1,0}}},
    {{{1,0,0,0}}},
    {{{-1,0,0,0}}},
    {{{0,1,0,0}}},
    {{{0,-1,0,0}}},
    };

static const struct Voxels::VoxelTex voxel_tex_array[4] = {
    {{{0,0,0,0}}},
    {{{0,1,0,0}}},
    {{{1,1,0,0}}},
    {{{1,0,0,0}}},
    };

static const struct Vec3 base_normals[6] = {
    {{{1,0,0}}},
    {{{0,1,0}}},
    {{{0,0,1}}},
    {{{-1,0,0}}},
    {{{0,-1,0}}},
    {{{0,0,-1}}},
    };

static const int side_v2[4*6] = {
    4,5,6,7,   //top (z=1)
    0,1,2,3,   //bottom (z=0)
    1,5,9,10,  //north (y=1)
    7,3,11,8,  //south (y=0)
    6,2,10,11, //west (x=0)
    4,0,9,8,   //east (x=1)
    };

static const int_fast8_t ao_perm[6*8*3] = {
     1,  1,  1,
     0,  1,  1,
    -1,  1,  1,
    -1,  0,  1,
    -1, -1,  1,
     0, -1,  1,
     1, -1,  1,
     1,  0,  1,

    -1,  1, -1,
     0,  1, -1,
     1,  1, -1,
     1,  0, -1,
     1, -1, -1,
     0, -1, -1,
    -1, -1, -1,
    -1,  0, -1,

     1, -1,  1,
     1, -1,  0,
     1, -1, -1,
     1,  0, -1,
     1,  1, -1,
     1,  1,  0,
     1,  1,  1,
     1,  0,  1,
    -1,  1,  1,

    -1,  1,  0,
    -1,  1, -1,
    -1,  0, -1,
    -1, -1, -1,
    -1, -1,  0,
    -1, -1,  1,
    -1,  0,  1,
     1,  1,  1,

     1,  1,  0,
     1,  1, -1,
     0,  1, -1,
    -1,  1, -1,
    -1,  1,  0,
    -1,  1,  1,
     0,  1,  1,
    -1, -1,  1,

    -1, -1,  0,
    -1, -1, -1,
     0, -1, -1,
     1, -1, -1,
     1, -1,  0,
     1, -1,  1,
     0, -1,  1,
    };

static const int_fast8_t _s_array[3*6] = {
         0, 0, 1,  //top
         0, 0,-1, //bottom
         1, 0, 0,  //north
        -1, 0, 0, //south
         0, 1, 0,  //west
         0,-1, 0, //east
    };
