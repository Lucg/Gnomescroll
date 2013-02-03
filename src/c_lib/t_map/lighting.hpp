#pragma once

#include <t_map/_interface.hpp>
#include <t_map/common/map_element.hpp>



/*
    Set Breakpoint Programmaticly
*/

#include <signal.h>
#include <syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/user.h>

enum {
    DR7_BREAK_ON_EXEC  = 0,
    DR7_BREAK_ON_WRITE = 1,
    DR7_BREAK_ON_RW    = 3,
};

enum {
    DR7_LEN_1 = 0,
    DR7_LEN_2 = 1,
    DR7_LEN_4 = 3,
};

typedef struct {
    char l0:1;
    char g0:1;
    char l1:1;
    char g1:1;
    char l2:1;
    char g2:1;
    char l3:1;
    char g3:1;
    char le:1;
    char ge:1;
    char pad1:3;
    char gd:1;
    char pad2:2;
    char rw0:2;
    char len0:2;
    char rw1:2;
    char len1:2;
    char rw2:2;
    char len2:2;
    char rw3:2;
    char len3:2;
} dr7_t;

typedef void _sighandler_t(int, siginfo_t*, void*);

int watchpoint(void* addr, _sighandler_t handler)
{
    pid_t child;
    pid_t parent = getpid();
    struct sigaction trap_action;
    int child_stat = 0;

    sigaction(SIGTRAP, NULL, &trap_action);
    trap_action.sa_sigaction = handler;
    trap_action.sa_flags = SA_SIGINFO | SA_RESTART | SA_NODEFER;
    sigaction(SIGTRAP, &trap_action, NULL);

    if ((child = fork()) == 0)
    {
        int retval = EXIT_SUCCESS;

        dr7_t dr7 = {0};
        dr7.l0 = 1;
        dr7.rw0 = DR7_BREAK_ON_WRITE;
        dr7.len0 = DR7_LEN_4;

        if (ptrace(PTRACE_ATTACH, parent, NULL, NULL))
        {
            exit(EXIT_FAILURE);
        }

        sleep(1);

        if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[0]), addr))
        {
            retval = EXIT_FAILURE;
        }

        if (ptrace(PTRACE_POKEUSER, parent, offsetof(struct user, u_debugreg[7]), dr7))
        {
            retval = EXIT_FAILURE;
        }

        if (ptrace(PTRACE_DETACH, parent, NULL, NULL))
        {
            retval = EXIT_FAILURE;
        }

        exit(retval);
    }

    waitpid(child, &child_stat, 0);
    if (WEXITSTATUS(child_stat))
    {
        printf("child exit !0\n");
        return 1;
    }

    return 0;
}

int var;

void trap(int sig, siginfo_t* info, void* context)
{
    printf("new value: %d\n", var);
}

int breakpoint_test()
{
    int i;

    printf("init value: %d\n", var);

    watchpoint(&var, trap);

    for (i = 0; i < 100; i++) {
        var++;
        sleep(1);
    }

    return 0;
}

namespace t_map
{

//15 is now highest light level

/*
    Update skylights seperately
*/

//e[ (z<<8)+((y&15)<<4)+(x&15) ] = e;
void update_skylight(int chunk_i, int chunk_j)
{
    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    //struct MAP_e e;

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int k = map_dim.z-1;

        // get highest block
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)    //iterate until we hit top block
                break;
            //e.light = 0x0f; //clear bottom bits, set to 15
            //e.light = 0x0f; //clear bottom bits, set to 15
            e.light = 15;

            mc->set_element(i,j,k,e);
        }
        if (k < 0) return;

        // render gradient down from top block

/*
        for (int _k=0; k>=0 && _k<16; k--, _k++)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            e.light  |= 16*(15-_k)+ 0x0f; //clear upper bits
            mc->set_element(i,j,k,e);
        }
*/
        // black out everything below
        for (; k>=0; k--)
        {
            e = mc->get_element(i,j,k);
            if(e.block != 0)
                continue;
            //e.light |= 0xf0;  //clear bottom bits, set to zero
            //e.light = 0x00;     //clear bottom bits, set to zero
            e.light = 0;
            mc->set_element(i,j,k,e);
        }
    }

}

//for comparision purpose
int get_skylight(int x, int y, int z)
{

    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 16;  //so it does not try to update

    //return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;

    return mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light & 0x0f;  //bottom half
}

void _set_skylight(int x, int y, int z, int value)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;
    GS_ASSERT(mc != NULL);
    GS_ASSERT( (y >> 4) < 32);
    GS_ASSERT( (x >> 4) < 32);
    GS_ASSERT(value < 16 && value > 0);
    //printf("%i\n", (z<<8)+((y&15)<<4)+(x&15) );

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;
    //light = value;
    //light &= 0xf0;          //clear lower byte
    //light |= (value & 0x0f); //set lower byte
    light = (light & 0xf0) | (value & 0x0f);
    //light = (light & 0xf0)
    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;
    //mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light &= (value & 0x0f);  //bottom half
}

int get_envlight(int x, int y, int z);

void set_skylight(int x, int y, int z, int value)
{
    int env_light = get_envlight(x,y,z);
    _set_skylight(x,y,z,value);
    GS_ASSERT(env_light == get_envlight(x,y,z));
}

//proprogate out
void update_skylight_out(int x, int y, int z)
{
    int li = get_skylight(x,y,z);

    //GS_ASSERT(! isSolid(x,y,z));
    if(li-1 <= 0) return;

/*
    !isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 ) {
        set_skylight(_x,_y,_z, li-1);
    These can be combined into a single function
*/

/*
    Recursion can be replaced by poping onto a circular buffer
*/

    int _x,_y,_z;

    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )   //do a single get block for this!!
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //GS_ASSERT( ((x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2) == (x+512-1) % 512);

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;
    //x
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    //y
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

    _x = x;
    _y = y;
    _z = (z-1+map_dim.z)%map_dim.z; //z -1
    if(!isSolid(_x,_y,_z) && get_skylight(_x,_y,_z) < li-1 )
    {
        set_skylight(_x,_y,_z, li-1);
        update_skylight_out(_x,_y,_z);
    }

}

//call this when removing a block
void update_skylight_in(int x, int y, int z)
{
    //int li = get_skylight(x,y,z);

    if(isSolid(x,y,z))
    {
        GS_ASSERT(false);
        return;
    }
    //technically, if top block of height map
    if( get_skylight(x,y,z+1) == 15 )
    {
        GS_ASSERT( z+1 == main_map->get_height(x,y) );

        /*
            BUG:
            Fails on removing block capping a column?
        */

        int _z = z;
        //assume first element is not solid
        while(1)
        {
            set_skylight(x,y,_z,15);
            if(isSolid(x,y,_z-1) || _z==0)
                break;
            _z--;
        }

        for(int tz=_z; tz <= z; tz++)
            update_skylight_out(x,y,tz);

        return;
    }
    int li = 0;

    int _x,_y,_z, tli;

    //x
    _x = (x+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = (x-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _y = y;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //y
    _x = x;
    _y = (y+1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = (y-1) & TERRAIN_MAP_WIDTH_BIT_MASK2;
    _z = z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    //z
    _x = x;
    _y = y;
    _z = (z+1) % map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;

    _x = x;
    _y = y;
    _z = (z+(map_dim.z-1))%map_dim.z;

    tli = get_skylight(_x,_y,_z);
    if(tli > li )
        li = tli;


    if(li != 0)
    {
        set_skylight(x,y,z, li);
        update_skylight_out(x,y,z);
    }
}

void update_skylight_boundary(int _ci, int _cj)
{
    class MAP_CHUNK* mc;
    int ci, cj;

    //north?
    ci = (_ci + 1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 0;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //south?
    ci = (_ci + -1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 15;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

    //west?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + 1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 0;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }


    //east?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + -1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 15;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
                continue;
            update_skylight_out(16*ci+i,16*cj+j,k);
        }
    }

}

void update_skylight2(int ci, int cj)
{
    update_skylight_boundary(ci, cj);

    class MAP_CHUNK* mc = main_map->chunk[32*cj + ci];

    mc->refresh_height_cache();

    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    for(int k=0; k<map_dim.z; k++)
    {
        if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_skylight(16*ci+i,16*cj+j,k) != 15) // || get_skylight(i,j,k) < 16)
            continue;

        update_skylight_out(16*ci+i,16*cj+j,k);

    }

}

/*
    Env Light
*/


//for comparision purpose
int get_envlight(int x, int y, int z)
{
    if( (z & TERRAIN_MAP_HEIGHT_BIT_MASK) != 0)
        return 0;

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return 0;  //so it does not try to update

    return (mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light >> 4);  //upper half
}

void set_envlight(int x, int y, int z, int value)
{
    GS_ASSERT((z & TERRAIN_MAP_HEIGHT_BIT_MASK) == 0);

    x &= TERRAIN_MAP_WIDTH_BIT_MASK2;
    y &= TERRAIN_MAP_WIDTH_BIT_MASK2;

    printf("set_envlight: %d %d %d, value= %d \n", x,y,z,value);

    class MAP_CHUNK* mc = main_map->chunk[ 32*(y >> 4) + (x >> 4) ];
    if(mc == NULL)
        return;

    GS_ASSERT(mc != NULL);
    GS_ASSERT( (y >> 4) < 32);
    GS_ASSERT( (x >> 4) < 32);
    GS_ASSERT(value < 16 && value >= 0);
    //printf("%i\n", (z<<8)+((y&15)<<4)+(x&15) );

    int light = mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light;

    //light &= 0x0f;          //clear upper nibble
    //light |= (value << 4);  //set upper nibble

    light = (light & 0x0f) | (value << 4); // clear upper nib, set upper nib

    mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light = light;
    printf("ptr value = %p \n", &(mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light) );
    //printf("")
    //mc->e[ (z<<8)+((y&15)<<4)+(x&15) ].light &= (value << 4);  //upper half

    main_map->set_update(x,y);

    GS_ASSERT(value == get_envlight(x,y,z));
}

struct LightUpdateElement
{
    int x,y,z;
};

struct LightUpdateElement* light_update_array = NULL;
int light_update_array_max      = 1024;
int light_update_array_index    = 0;

void _push_envlight_update2(int x, int y, int z)
{
    //move to init
    if(light_update_array == NULL)
    {
        light_update_array_max = 1024;
        light_update_array_index = 0;
        light_update_array = (struct LightUpdateElement*) malloc(light_update_array_max* sizeof(struct LightUpdateElement));
    }

    //cannot update light value of solid block!
    struct MAP_ELEMENT e = get_element(x,y,z);
    if(fast_cube_properties[e.block].solid == true)
    {
        GS_ASSERT(false);
        return;
    }

    if(light_update_array_index == light_update_array_max)
    {
        light_update_array_max *= 2;
        light_update_array = (struct LightUpdateElement*) realloc(light_update_array, light_update_array_max* sizeof(struct LightUpdateElement));
        printf("reallocing light array to: %d \n", light_update_array_max);
    }

    light_update_array[light_update_array_index].x = x;
    light_update_array[light_update_array_index].y = y;
    light_update_array[light_update_array_index].z = z;

    light_update_array_index++;
}

/*
    Array
*/
static const int va[3*6] =
{
    0,0,1,
    0,0,-1,
    1,0,0,
    -1,0,0,
    0,1,0,
    0,-1,0
};

void _envlight_update_core();


void _envlight_update2(int _x, int _y, int _z)
{
    GS_ASSERT(light_update_array_index == 0);
    GS_ASSERT(isSolid(_x,_y,_z) == false);

    _push_envlight_update2(_x,_y,_z);
    _envlight_update_core();
}

void _t_break()
{

}


void _envlight_update_core()
{

    if(light_update_array_index == 0)
        return;

    for(int i=0; i<light_update_array_index; i++)
    { 
        GS_ASSERT(isSolid(light_update_array[i].x, light_update_array[i].y, light_update_array[i].z) == false);
    }

    int index = 0;
    while(index != light_update_array_index)
    {
        int x = light_update_array[index].x;
        int y = light_update_array[index].y;
        int z = light_update_array[index].z;

        struct MAP_ELEMENT e = get_element(x,y,z);
        int li = (e.light >> 4);
        GS_ASSERT(li == get_envlight(x,y,z));

        if(fast_cube_properties[e.block].light_source == true)
        {
            //this code path is deprecated
            GS_ASSERT(li == fast_cube_attributes[e.block].light_value);

            for(int i=0; i<6; i++)
            {
                struct MAP_ELEMENT _e = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                if( (_e.light >> 4) < li -1 && fast_cube_properties[_e.block].solid == false)
                    _push_envlight_update2(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
            }
            index++;
            continue;
        }
        else
        {
    /*
        ASSERT!!!
        - If a cube is not solid, the max difference between min adn non-light source, non-solid blocks adjacent blocks
        - is 2
        - if difference is more than 2, then suggests removal of block
    */
            //solid non-light source

            if(fast_cube_properties[e.block].solid == true)
            {
                GS_ASSERT(false);
                if(index != 0)
                    printf("ERROR: env_light called on solid block on index %d of %d \n", index, light_update_array_index);
                index++;
                continue;
            }

            //non-solid non-light source
            // 1> check to make sure light value is correct; not too high, or too low
            // 2> If light value in adjacent block is

            for(int i=0; i<6; i++)
            {
                struct MAP_ELEMENT _e = get_element(x+va[3*i+0] ,y+va[3*i+1] , z+va[3*i+2]);
                //set light at current position if neighorbing positions are brighter
                if(fast_cube_properties[_e.block].solid == true && fast_cube_properties[_e.block].light_source == false)
                {
                    GS_ASSERT( (_e.light >> 4) == 0);
                }

                if( (_e.light >> 4) > li + 1 )
                {
                    if(fast_cube_properties[_e.block].solid == true)
                    {
                        GS_ASSERT(fast_cube_properties[_e.block].light_source == true);
                    }

                    printf("light_set index: %d, at %d %d %d, was %d, now %d \n", index, x,y,z, li, (_e.light >> 4) -1 );

                    GS_ASSERT(li+1 < (_e.light >> 4) );

                    li = (_e.light >> 4) -1;

                    GS_ASSERT(li >= 0 && li <= 15)
                    set_envlight(x,y,z, li);

                    //update neighboring blocks if current block light value is updated
                    for(int j=0; j<6; j++)
                    {
                        struct MAP_ELEMENT _e2 = get_element(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                        if( (_e2.light >> 4) +1 < li && fast_cube_properties[_e2.block].solid == false)
                            _push_envlight_update2(x+va[3*j+0], y+va[3*j+1], z+va[3*j+2]);
                    }
                }
            }
        }

            
        index++;
        continue;
    }

    light_update_array_index = 0;


    if(index > 1)
    printf("light_index= %d \n", index); 


#if DC_CLIENT
if(index > 256) _t_break();
if(index > 256) breakpoint_test();
    //if(index > 1000)
    //    GS_ASSERT(false);
#endif
}

/*
    Each block should only set its own values
*/

void _envlight_update2(int x, int y, int z);

//handle block addition
void light_add_block(int x, int y, int z)
{

    #if DC_CLIENT
        printf("light_add_block: %d %d %d \n", x,y,z);
    #endif


    struct MAP_ELEMENT e = get_element(x,y,z);

    if( (e.light >> 4) != fast_cube_attributes[e.block].light_value )
    {
        //GS_ASSERT(false) //should not happen!
        set_envlight(x,y,z, fast_cube_attributes[e.block].light_value);
    }

    //placed solid block
    if(fast_cube_properties[e.block].solid == true)
    {
        for(int j=0; j<6; j++)
        {
            struct MAP_ELEMENT e = get_element(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
            if( fast_cube_properties[e.block].solid == false )
                _push_envlight_update2(x+va[3*j+0] ,y+va[3*j+1] , z+va[3*j+2]);
        }
        _envlight_update_core();
    }
    else
    {
        GS_ASSERT(isSolid(x,y,z) == false);
        _envlight_update2(x,y,z);
    }
}

/*
void _envlight_update(int x, int y, int z)
{
    _envlight_update2(x,y,z);
    return;
}
*/

void update_envlight_boundary(int _ci, int _cj);

//checks that block lighting values are correct
void asssert_envlight_0(int chunk_i, int chunk_j)
{
    return; 

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    for(int k=0; k<128; k++)
    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int x = 16*chunk_i + i;
        int y = 16*chunk_j + j;

        e = mc->get_element(i,j,k);

        if(fast_cube_properties[e.block].light_source == true)
        {
            //assert that light source blocks have the proper light value
            GS_ASSERT( get_envlight(x,y,k) == fast_cube_attributes[e.block].light_value);
        }
        else
        {
            if(fast_cube_properties[e.block].solid == true)
            {
                //assert that solid blocks have lighting 0
                GS_ASSERT( get_envlight(x,y,k) == 0 );
            }
        }
    }

}

//call on chunk init
void init_update_envlight(int chunk_i, int chunk_j)
{
    return;
    //printf("init_update_envlight: %d %d \n", chunk_i, chunk_j);

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    for(int k=0; k<128; k++)
    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {
        int x = 16*chunk_i + i;
        int y = 16*chunk_j + j;

        e = mc->get_element(i,j,k);

        if(fast_cube_properties[e.block].light_source == true)
        {
            set_envlight(x,y,k, fast_cube_attributes[e.block].light_value);
        }
        else
        {
        
            if(fast_cube_properties[e.block].solid == true)
            {
                GS_ASSERT(get_envlight(x,y,k) == 0);
                //set_envlight(x,y,k, 0);
            }
            else
            {
                //set_envlight(x,y,k, 0);
            }
        
        }
    }
}

void update_envlight(int chunk_i, int chunk_j)
{

    return;

    printf("update_envlight: %d %d \n", chunk_i, chunk_j);

    class MAP_CHUNK* mc = main_map->chunk[32*chunk_j + chunk_i];
    struct MAP_ELEMENT e;

    for(int k=0; k<128; k++)
    for(int i=0; i<16; i++)
    for(int j=0; j<16; j++)
    {

        int x = 16*chunk_i + i;
        int y = 16*chunk_j + j;

        e = mc->get_element(i,j,k);
    /*
        if(fast_cube_properties[e.block].solid == false ||
            fast_cube_properties[e.block].light_source == true)
        _envlight_update2(x,y,k);
    */
        if(fast_cube_properties[e.block].solid == false)
            _envlight_update2(x,y,k);

    }


    //update_envlight_boundary(chunk_i, chunk_j);


    asssert_envlight_0(chunk_i, chunk_j);


}

/*
void update_envlight_boundary(int _ci, int _cj)
{
    class MAP_CHUNK* mc;
    int ci, cj;

    //north?
    ci = (_ci + 1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 0;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            _envlight_update2(16*ci+i,16*cj+j,k);
        }
    }


    //south?
    ci = (_ci + -1 +32 ) % 32;
    cj = (_cj + 0 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int i = 15;
        for(int j=0; j<16; j++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            _envlight_update2(16*ci+i,16*cj+j,k);
        }
    }

    //west?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + 1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 0;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            _envlight_update2(16*ci+i,16*cj+j,k);
        }
    }


    //east?
    ci = (_ci + 0 +32 ) % 32;
    cj = (_cj + -1 +32 ) % 32;
    mc = main_map->chunk[32*cj + ci];

    if(mc != NULL)
    {
        const int j = 15;
        for(int i=0; i<16; i++)
        for(int k=0; k<map_dim.z; k++)
        {
            if(isSolid(16*ci+i,16*cj+j,k) ) //|| get_envlight(16*ci+i,16*cj+j,k) != 15) // || get_envlight(i,j,k) < 16)
                continue;
            _envlight_update2(16*ci+i,16*cj+j,k);
        }
    }

}
*/

}   // t_map
