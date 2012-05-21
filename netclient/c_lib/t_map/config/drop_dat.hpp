#pragma once

#include <t_map/config/_interface.hpp>
#include <t_map/config/drop_table.hpp>

namespace t_map
{


    //add_drop(const char* item_name, float mean, float falloff, int max_drops)
    //float mean, float falloff, int max_drops
void load_map_drop_dat()
{

    def_drop("regolith");

    add_drop("regolith", 3);
    set_drop(0.30, 1);
    set_drop(0.15, 2);
    set_drop(0.07, 3);

    add_drop("copper_ore", 4);
    set_drop(0.30, 1);
    set_drop(0.10, 2);
    set_drop(0.03, 3);
    set_drop(0.05, 5);

    add_drop("food_rock-0", 3);
    set_drop(0.20, 1);
    set_drop(0.10, 2);
    set_drop(0.5,  3);

    end_drop_dat();
    #if DC_CLIENT
    save_drop_dat_to_file();
    #endif
}


}   // t_map
