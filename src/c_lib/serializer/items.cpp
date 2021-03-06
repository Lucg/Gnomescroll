/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "items.hpp"

#include <item/_interface.hpp>
#include <item/container/_interface.hpp>
#include <item/common/constants.hpp>
#include <serializer/redis.hpp>
#include <serializer/_state.hpp>
#include <serializer/uuid.hpp>

namespace serializer
{

class ParsedItemDataList* item_load_data_list = NULL;

void init_items()
{
    GS_ASSERT(item_load_data_list == NULL);
    item_load_data_list = new ParsedItemDataList(PARSED_ITEM_DATA_LIST_MAX);
}

void teardown_items()
{
    delete item_load_data_list;
}

// WARNING -- modifies char* str
bool parse_item_token(const char* key, const char* val, class ParsedItemData* data)
{
    bool err = false;
    if (strcmp(UUID_TAG, key) == 0)
    {
        int ret = uuid_parse(val, data->uuid);
        IF_ASSERT(ret) return false;
        ret = uuid_is_null(data->uuid);
        IF_ASSERT(ret) return false;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = is_valid_name(val);
        IF_ASSERT(!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(DURABILITY_TAG, key) == 0)
    {
        long long durability = parse_int(val, err);
        IF_ASSERT(err || durability <= 0 || durability > MAX_DURABILITY) return false;
        data->durability = (int)durability;
    }
    else
    if (strcmp(STACK_SIZE_TAG, key) == 0)
    {
        long long stack_size = parse_int(val, err);
        IF_ASSERT(err || stack_size <= 0 || stack_size > MAX_STACK_SIZE) return false;
        data->stack_size = (int)stack_size;
    }
    else
    if (strcmp(CONTAINER_SLOT_TAG, key) == 0)
    {
        long long container_slot = parse_int(val, err);
        IF_ASSERT(err || ((container_slot < 0 || container_slot > MAX_CONTAINER_SIZE) && container_slot != NULL_SLOT)) return false;
        data->container_slot = container_slot;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static inline bool item_valid(class Item::Item* item)
{
    // the item valid check occurs every frame and covers most item state
    // it doesnt check uuids, as they are serializer specific and expensive to check, and won't change
    if (!item->valid) return false;
    if (item->location == IL_PARTICLE) return false;    // We don't support item particle saving yet
    // recover from missing UUID error. this is a programming error, but is not fatal.
    bool uuid_valid = (!uuid_is_null(item->uuid));
    IF_ASSERT(!uuid_valid) uuid_generate(item->uuid);
    return true;
}

size_t write_item_string(char* buf, size_t buffer_size, ItemID item_id)
{
    class Item::Item* item = Item::get_item(item_id);
    IF_ASSERT(item == NULL) return 0;

    bool valid = item_valid(item);
    IF_ASSERT(!valid) return 0;

    const char* item_name = Item::get_item_name(item->type);
    IF_ASSERT(item_name == NULL) return 0;

    static char uuid_buf[UUID_STRING_LENGTH+1];
    size_t wrote = write_uuid(uuid_buf, UUID_STRING_LENGTH+1, item->uuid);
    if (wrote != UUID_STRING_LENGTH) return 0;

    int could_write = snprintf(buf, buffer_size, ITEM_FMT,
                               item_name,
                               item->durability,
                               item->stack_size,
                               item->container_slot,
                               uuid_buf);

    if (could_write < 0) return could_write;
    if ((size_t)could_write >= buffer_size)
    {
        buf[0] = '\0';
        return 0;
    }

    buf[could_write++] = '\n';

    return (size_t)could_write;
}

// returns 0 on failure. can also return 0 if there are no container contents, so check the container first
int write_container_contents_string(char* buf, size_t buffer_size, const class ItemContainer::ItemContainerInterface* container)
{
    size_t ibuf = 0;
    for (int i=0; i<container->slot_max; i++)
        if (container->slot[i] != NULL_ITEM)
        {
            int could_write = write_item_string(&buf[ibuf], buffer_size - ibuf, container->slot[i]);
            if (could_write < 0) return could_write;                    // c lib printf returned error
            if ((size_t)could_write >= buffer_size - ibuf) return 0;    // ran out of buffer space
            ibuf += (size_t)could_write;
        }
    buf[ibuf] = '\0';
    return (int)ibuf;
}

static class Item::Item* make_item(class ParsedItemData* data)
{
    class Item::Item* item = Item::create_item(data->item_type);
    IF_ASSERT(item == NULL) return NULL; // This is very bad. Has to be impossible

    uuid_copy(item->uuid, data->uuid);
    item->durability = data->durability;
    item->stack_size = data->stack_size;
    item->container_slot = data->container_slot;
    item->location = data->item_location;

    item->init_from_loading();

    return item;
}

bool create_container_items_from_data(ItemContainerID container_id)
{
    IF_ASSERT(!isValid(container_id)) return false;

    unsigned int item_space = Item::item_space();
    IF_ASSERT(item_load_data_list->ct > item_space) return false;

    for (size_t i=0; i<item_load_data_list->max; i++)
    {
        class ParsedItemData* data = &item_load_data_list->objects[i];
        if (data->id == item_load_data_list->null_id) continue;

        class Item::Item* item = make_item(data);
        IF_ASSERT(item == NULL) return false; // Can/should never happen. Critical part
        IF_ASSERT(item->location != IL_CONTAINER) return false;

        ItemContainer::load_item_into_container(item->id, container_id, item->container_slot);
    }

    clear_item_data();

    return true;
}

bool create_player_container_items_from_data(AgentID agent_id, ItemContainerID* containers, size_t n_containers)
{
    for (size_t i=0; i<n_containers; i++)
        IF_ASSERT(!isValid(containers[i])) return false;

    MALLOX(ItemContainerType, container_types, n_containers);
    for (size_t i=0; i<n_containers; i++)
    {
        ItemContainerType container_type = ItemContainer::get_container_type(containers[i]);
        if (container_type == NULL_CONTAINER_TYPE) return false;
        GS_ASSERT(container_type != NULL_CONTAINER_TYPE);
        container_types[i] = container_type;
    }

    unsigned int item_space = Item::item_space();
    IF_ASSERT(item_load_data_list->ct > item_space) return false;

    for (size_t i=0; i<item_load_data_list->max; i++)
    {
        class ParsedItemData* data = &item_load_data_list->objects[i];
        if (data->id == item_load_data_list->null_id) continue;

        class Item::Item* item = make_item(data);
        IF_ASSERT(item == NULL) break; // Can/should never happen. Critical part

        //GS_ASSERT(item->location == IL_HAND || item->location == IL_CONTAINER);
        GS_ASSERT(item->location == IL_CONTAINER);

        bool loaded = false;
        //if (item->location == IL_HAND)
        //{
            //item->location_id = agent_id;
            //loaded = ItemContainer::load_item_into_hand(item->id, agent_id);
        //}
        //else
        if (item->location == IL_CONTAINER)
        {
            ItemContainerID container_id = NULL_CONTAINER;
            for (size_t j=0; j<n_containers; j++)
                if (container_types[j] == data->item_container_type)
                {
                    container_id = containers[j];
                    break;
                }
            IF_ASSERT(container_id == NULL_CONTAINER) continue;
            item->location_id = container_id;
            loaded = ItemContainer::load_item_into_container(item->id, container_id, item->container_slot);
        }
        IF_ASSERT(!loaded)
            Item::destroy_item(item->id);
    }

    clear_item_data();

    return true;
}

class ParsedItemData* create_item_data()
{
    return item_load_data_list->create();
}

void destroy_item_data(ItemID id)
{
    item_load_data_list->destroy(id);
}

void clear_item_data()
{
    item_load_data_list->clear();
}

}   // serializer
