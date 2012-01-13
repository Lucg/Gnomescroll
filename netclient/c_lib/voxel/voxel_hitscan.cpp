#include "voxel_hitscan.hpp"

void Voxel_hitscan_list::register_voxel_volume(Voxel_volume* vv, int entity_id, int entity_type)
{
    if(num_elements >= VOXEL_HITSCAN_LIST_SIZE)
    {
        printf("Voxel_hitscan_list Error: number of voxel models exceeds VOXEL_HITSCAN_LIST_SIZE \n");
        return;
    }
    int i;
    for(i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if(hitscan_list[i] == NULL)
        {
            hitscan_list[i] = new Voxel_hitscan_element;
            num_elements++;
            hitscan_list[i]->vv = vv;
            hitscan_list[i]->entity_id = entity_id;
            hitscan_list[i]->entity_type = entity_type;
                      
            vv->voxel_hitscan_list = this;
            break;
        }
    }
    if (i == VOXEL_HITSCAN_LIST_SIZE) printf("WARNING: register_voxel_hitscan - no space available\n");

}

void Voxel_hitscan_list::unregister_voxel_volume(Voxel_volume* vv)
{
    for(int i=0; i < VOXEL_HITSCAN_LIST_SIZE; i++)
    {
        if(hitscan_list[i]->vv == vv)
        {
            num_elements--;
            delete hitscan_list[i];
            hitscan_list[i] = NULL;
            printf("Removed voxel volume %i \n", i);
            vv->voxel_hitscan_list = NULL;
            return;
        }
    }

    printf("Voxel_hitscan_list::unregister_voxel_hitscan error, volume was not on list \n");
    vv->id = -1;
    vv->voxel_hitscan_list = NULL;
}