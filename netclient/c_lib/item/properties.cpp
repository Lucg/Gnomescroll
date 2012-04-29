#include "properties.hpp"

#include <item/_interface.hpp>
#include <item/data/constant.hpp>

namespace Item
{


int sprite_array[MAX_ITEMS]; //maps item id to sprite
int group_array[MAX_ITEMS];

void init_properties()
{
    for (int i=0; i<MAX_ITEMS; group_array[i++] = IG_ERROR);
    for (int i=0; i<MAX_ITEMS; sprite_array[i++] = 0);
}
int get_sprite_index_for_id(ItemID id)
{
    assert(id < MAX_ITEMS && id >= 0);
    int type = get_item_type(id);
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

int get_sprite_index_for_type(int type)
{
    if (type == NULL_ITEM_TYPE) return ERROR_SPRITE;
    assert(type >= 0 && type < MAX_ITEMS);
    return sprite_array[type];
}

/*
void set_sprite_ids()
{
    for (int i=0; i<MAX_ITEMS; sprite_array[i++] = ERROR_SPRITE);
    for (int i=0; i<8; i++) sprite_array[i] = i;
    for (int i=16; i<24; i++) sprite_array[i] = i;
    for (int i=32; i<40; i++) sprite_array[i] = i;
}
*/
/*
Names
*/


const int ITEM_NAME_MAX_LENGTH = 64;
char item_names[MAX_ITEMS*ITEM_NAME_MAX_LENGTH];
int item_name_index[MAX_ITEMS];


/*
struct cubeProperties* get_cube(int id)
{
    if (id < 0) printf("get_cube: error id less than zero \n");
    if (id >= MAX_ITEMS ) printf("get_cube: error id exceeds MAX_ITEMS \n");
    return &cube_list[id];
}
*/

void set_item_name(int id, char* name, int length)
{
    assert(length > 0);
    assert(id < 0 || id >= MAX_ITEMS);
    
    if (length >= ITEM_NAME_MAX_LENGTH)
    {
        printf("Error: %s, name length greater than 63 characters \n", __func__ );
        assert(length < ITEM_NAME_MAX_LENGTH);
    }

    static int index = 0;

    item_name_index[id] = index;

    memcpy(item_names+index, name, length);
    index += length;
    item_names[index] = '\0';
    index++;
}

char* get_item_name(int id)
{
    assert(id < 0 || id >= MAX_ITEMS);
    return (item_names + item_name_index[id]);
}

int get_item_id(char* name)
{
    for (int i=0; i<MAX_ITEMS; i++)
        if (strcmp(name, get_item_name(i)) == 0)
            return i;
    return -1;
}

}