#ifndef DC_CLIENT
    #define DC_CLIENT
#endif

/* kill the compiler */
#ifdef DC_SERVER 
    sasfsafgdg4423+3
#endif

#include <c_lib.hpp>

#include <defines.h>

/* Vectors, Ray Tracers, Physics */
#include <c_lib/physics/vector.c>
#include <c_lib/ray_trace/ray_trace.c>
#include <c_lib/physics/common.c>

/* objects */
#include <c_lib/objects/object_lib.cpp>

/* Network */
#include <net_lib/net_lib.h>

/* Agents */
#include <c_lib/agent/agent_include.h>


/* map */
#include <c_lib/t_map/t_map.c>
#include <c_lib/t_map/t_properties.c>
#ifdef DC_CLIENT
    #include <c_lib/t_map/t_viz.c>
    #include <c_lib/t_map/t_vbo.c>

    /* animations */
    #include <c_lib/animations/animations.cpp>

    /* HUD */
    #include <c_lib/hud/cube_select.cpp>
    #include <c_lib/hud/inventory.cpp>
    #include <c_lib/texture_loader.c>
    #include <c_lib/hud/hud_texture_loader.cpp>

    /* SDL */
    #include <c_lib/SDL/camera.c>
    #include <c_lib/SDL/draw_functions.c>
    #include <c_lib/SDL/particle_functions.c>
    #include <c_lib/SDL/SDL_functions.c>
    #include <c_lib/SDL/SDL_text.c>
    #include <c_lib/SDL/texture_loader.c>
    #include <c_lib/SDL/block_selector.c>

    /* input */
    #include <c_lib/input/input.cpp>

    /* camera */
    /*
    #include <c_lib/camera/camera.c>
    */

#endif

int init_c_lib() {
    printf("init c_lib\n");
    srand(time(NULL));   // seed the RNG

    #ifdef DC_CLIENT
        init_texture_loader();
        init_hud_texture_loader();
        _init_image_loader();
        _init_text();
    #endif
return 0;
}
