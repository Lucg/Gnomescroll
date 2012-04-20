#include "_interface.hpp"

//#include <c_lib/t_item/list.hpp>

#include <c_lib/t_item/free_item.hpp>
#include <c_lib/t_item/item_container.hpp>
#include <c_lib/t_item/item.hpp>

#include <c_lib/t_item/net/StoC.hpp>

namespace t_item
{

Free_item_list* free_item_list = NULL;
ItemContainerList* item_container_list = NULL;
ItemList* item_list = NULL;

int AgentInventoryList[256];

void state_init()
{
    if (free_item_list != NULL)
    {
        printf("WARNING: attempt to call t_item::state_init() more than once\n");
        return;
    }
    free_item_list = new Free_item_list;
    item_container_list = new ItemContainerList;
    item_list = new ItemList;

    for(int i=0; i<256; i++) AgentInventoryList[i] = NO_AGENT;
}

void state_teardown()
{
    if (free_item_list != NULL) delete free_item_list;
    if (item_container_list != NULL) delete item_container_list;
    if (item_list != NULL) delete item_list;
}
 
void tick()
{
    free_item_list->tick();
#ifdef DC_SERVER
    check_item_pickups();
#endif
}

void draw()
{
    free_item_list->draw();
}

}
 
/*
CLIENT
*/
#ifdef DC_CLIENT

namespace t_item
{
    int player_inventory_id = 0xffff;   //store id of player inventory

    class ItemContainer* player_inventory = NULL;


}

#endif 

/*
SERVER
*/
#ifdef DC_SERVER

namespace t_item
{

//const int NO_AGENT = 0xffff;


void create_agent_inventory(int agent_id, int client_id)
{
    ItemContainer* ic = item_container_list->create();
    ic->init_agent_inventory();

    assert(AgentInventoryList[agent_id] == NO_AGENT);
    assert((agent_id < 255) && (agent_id > 0));

    AgentInventoryList[agent_id] = ic->id;
    
    Agent_state* a = ServerState::agent_list->get(agent_id);
    a->inventory_id = ic->id;

    assign_agent_inventory_StoC p;
    p.inventory_id = ic->id;
    p.agent_id = agent_id;
    p.sendToClient(client_id);

    printf("Sending inventory to client %i \n", client_id);
}

void delete_agent_inventory(int agent_id)
{
    assert(AgentInventoryList[agent_id] != NO_AGENT);
    item_container_list->destroy(agent_id);
    AgentInventoryList[agent_id] == NO_AGENT;
}

void check_item_pickups()
{
    #ifdef DC_CLIENT
        printf("Warning: check_item_pickups was called on client! OOPS\n");
    #endif
    free_item_list->check_item_pickups();
}

void create_free_item(int item_type, 
    float x, float y, float z, 
    float vx, float vy, float vz)
{
    
    Free_item* f = free_item_list->create();
    if(f == NULL) return;
    f->init(x,y,z,vx,vy,vz);

    class free_item_create_StoC p;

    p.type = 0;
    p.id = f->id;
    p.x = x;
    p.y = y;
    p.z = z;
    p.mx = vx;
    p.my = vy;
    p.mz = vz;

    p.broadcast();
}


void create_item(int inventory_id)
{

    
}

inline void item_create_StoC::handle()
{
// uint16_t item_id;
// uint16_t item_type;
// uint16_t inventory_id;
// uint16_t inventory_slot;



}


#endif 
