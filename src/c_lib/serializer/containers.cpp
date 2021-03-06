/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#include "containers.hpp"

#include <stdio.h>

#include <item/container/container.hpp>
#include <item/container/_interface.hpp>
#include <item/container/_state.hpp>
#include <serializer/items.hpp>
#include <serializer/_state.hpp>
#include <serializer/parse.hpp>

namespace serializer
{

/* Loading */

static bool parse_container_file_header_token(const char* key, const char* val, class ParsedContainerFileData* data)
{
    bool err = false;
    if (strcmp(VERSION_TAG, key) == 0)
    {
        long long version = parse_int(val, err);
        IF_ASSERT(err || version <= 0 || version > GS_VERSION) return false;
        data->version = version;
    }
    else
    if (strcmp(CONTAINER_COUNT_TAG, key) == 0)
    {
        long long container_count = parse_int(val, err);
        IF_ASSERT(err || container_count < 0 || container_count > MAX_CONTAINERS) return false;
        data->container_count = container_count;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool parse_container_token(const char* key, const char* val, class ParsedContainerData* data)
{
    bool err = false;
    if (strcmp(CONTAINER_ID_TAG, key) == 0)
    {
        long long container_id = parse_int(val, err);
        IF_ASSERT(err || container_id < 0 || container_id >= MAX_CONTAINERS) return false;
        data->container_id = (ItemContainerID)container_id;
    }
    else
    if (strcmp(NAME_TAG, key) == 0)
    {
        bool valid_name = ItemContainer::is_valid_container_name(val);
        IF_ASSERT(!valid_name) return false;
        strncpy(data->name, val, DAT_NAME_MAX_LENGTH);
        data->name[DAT_NAME_MAX_LENGTH] = '\0';
    }
    else
    if (strcmp(CONTAINER_ITEM_COUNT_TAG, key) == 0)
    {
        long long item_count = parse_int(val, err);
        IF_ASSERT(err || item_count < 0 || item_count > MAX_CONTAINER_SIZE) return false;
        data->item_count = item_count;
    }
    else
    if (strcmp(MAP_POSITION_TAG, key) == 0)
    {
        if (!parse_map_position(val, &data->position))
            return false;
    }
    else
    {   // unrecognized field
        GS_ASSERT(false);
        return false;
    }
    return true;
}

static bool process_container_blob(const char* str, size_t filesize)
{
    // allocate scratch buffer
    static const size_t LONGEST_LINE_A = GS_MAX(CONTAINER_LINE_LENGTH, CONTAINER_FILE_LINE_LENGTH);
    static const size_t LONGEST_LINE_B = GS_MAX(ITEM_LINE_LENGTH, CONTAINER_SEPARATOR_LENGTH);
    static const size_t LONGEST_LINE = GS_MAX(LONGEST_LINE_A, LONGEST_LINE_B);

    char buf[LONGEST_LINE+1] = {'\0'};

    // copy file header line
    size_t i = 0;
    char c = '\0';
    while ((c = str[i]) != '\0' && c != '\n' && i < LONGEST_LINE)
        buf[i++] = c;
    buf[i] = '\0';
    IF_ASSERT(c != '\n') return false;

    // read file header
    class ParsedContainerFileData container_file_data;
    parse_line<class ParsedContainerFileData>(&parse_container_file_header_token, buf, i, &container_file_data);
    IF_ASSERT(!container_file_data.valid) return false;
    i++;

    class ParsedContainerData container_data;
    for (int m=0; m<container_file_data.container_count; m++)
    {
        IF_ASSERT(c == '\0') return false;

        // copy ctr header line to buffer
        size_t k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
        buf[k] = '\0';
        IF_ASSERT(c != '\n') return false;

        // parse ctr header
        container_data.reset();
        parse_line<class ParsedContainerData>(&parse_container_token, buf, k, &container_data);
        IF_ASSERT(!container_data.valid) return false;
        GS_ASSERT(container_data.container_id == m+1);  // not fatal, but indicates a problem with container serializer

        // get container by map position
        ItemContainerID container_id = t_map::get_block_item_container(container_data.position);
        IF_ASSERT(container_id == NULL_CONTAINER) return false;

        ItemContainer::ItemContainerInterface* container = ItemContainer::get_container(container_id);
        IF_ASSERT(container == NULL) return false;

        // check container type
        const char* actual_name = ItemContainer::get_compatible_name(container_data.name);
        IF_ASSERT(actual_name == NULL) return false;

        ItemContainerType container_type = ItemContainer::get_type(actual_name);
        IF_ASSERT(container_type != container->type) return false;

        // check items will fit
        IF_ASSERT(container_data.item_count > container->slot_max) return false;

        clear_slot_checker();

        // parse contents
        for (int n=0; n<container_data.item_count; n++)
        {
            // copy item line to buffer
            k = 0;
            while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
                buf[k++] = c;
            buf[k] = '\0';
            IF_ASSERT(c != '\n') return false;

            class ParsedItemData* item_data = create_item_data();
            parse_line<class ParsedItemData>(&parse_item_token, buf, k, item_data);
            IF_ASSERT(!item_data->valid) return false;

            item_data->item_location = IL_CONTAINER;

            ItemType item_type = Item::get_item_type(item_data->name);
            IF_ASSERT(item_type == NULL_ITEM_TYPE) return false;

            item_data->item_type = item_type;

            IF_ASSERT(slot_checker[item_data->container_slot] != NULL_ITEM) return false;
            slot_checker[item_data->container_slot] = item_data->id;
        }

        if (!create_container_items_from_data(container->id)) return false;

        // mark container with next phase of loaded status
        loaded_containers[container->id] = CONTAINER_LOAD_ITEMS;

        // check separator
        k = 0;
        while ((c = str[i++]) != '\0' && c != '\n' && k < LONGEST_LINE)
            buf[k++] = c;
        buf[k] = '\0';
        IF_ASSERT(c != '\n') return false;

        int sepmatch = strcmp(buf, CONTAINER_SEPARATOR);
        IF_ASSERT(sepmatch != 0) return false;
    }

    // make sure that we reached the end of the file. otherwise the reported container count is too low
    IF_ASSERT(str[i] != '\0') return false;

    return true;
}

static bool load_container_file(const char* filename)
{
    printf("Loading container file %s\n", filename);

    size_t filesize = 0;
    char* buffer = read_file_to_buffer(filename, &filesize);
    IF_ASSERT(buffer == NULL) return false;

    bool success = process_container_blob(buffer, filesize);
    free(buffer);
    IF_ASSERT(!success)
    {
        log_container_load_error("Error processing entire file");
        return false;
    }

    bool err = false;
    // check that all containers loaded from map were also found in the flat file
    for (int i=0; i<MAX_CONTAINERS; i++)
    {
        ContainerLoadState state = loaded_containers[i];
        IF_ASSERT(state != CONTAINER_LOAD_UNUSED && state != CONTAINER_LOAD_ITEMS)
            err = true;
    }

    clear_loaded_containers();

    IF_ASSERT(err)
        log_container_load_error("Containers defined in map not found in container data file");

    return (!err);
}

/* Saving */

const char* write_container_string(const class ItemContainer::ItemContainerInterface* container, int container_entry, size_t* wrote)
{
    *wrote = 0;
    size_t ibuf = 0;

    // get container data
    const char* container_name = ItemContainer::get_container_name(container->type);
    IF_ASSERT(container_name == NULL)
    {
        log_container_save_error("Failed to acquire container name");
        return NULL;
    }

    Vec3i b;
    bool found = t_map::get_container_location(container->id, b);
    IF_ASSERT(!found) return NULL;
    IF_ASSERT(!is_boxed_position(b))
    {
        log_container_save_error("Container dimensions outside map boundaries");
        return NULL;
    }

    // write header
    int could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, CONTAINER_HEADER_FMT,
        container_name, container->slot_count, b.x, b.y, b.z, container_entry);
    IF_ASSERT(could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf)
    {
        log_container_save_error("Buffer overrun in writing container entry header");
        return NULL;
    }
    ibuf += (size_t)could_write;

    _buffer[ibuf++] = '\n';
    IF_ASSERT(ibuf >= BUF_SIZE)
    {
        log_container_save_error("Buffer overrun in writing container entry header");
        return NULL;
    }

    // write contents
    if (container->slot_count > 0)
    {   // we must check the slot count, because write_container_contents_string returns 0 on error or if the container is empty
        size_t wrote = write_container_contents_string(&_buffer[ibuf], BUF_SIZE - ibuf, container);
        IF_ASSERT(wrote <= 0)
        {
            log_container_save_error("Failed to write container contents string");
            return NULL;
        }
        ibuf += wrote;
    }

    // write separator between container entries
    could_write = snprintf(&_buffer[ibuf], BUF_SIZE - ibuf, CONTAINER_SEPARATOR "\n");
    IF_ASSERT(could_write <= 0 || (size_t)could_write >= BUF_SIZE - ibuf) return NULL;
    ibuf += (size_t)could_write;

    _buffer[ibuf] = '\0';

    *wrote = ibuf;
    return _buffer;
}

static bool save_container(FILE* f, const class ItemContainer::ItemContainerInterface* container, int container_entry)
{
    size_t container_string_length = 0;
    const char* container_string = write_container_string(container, container_entry, &container_string_length);

    IF_ASSERT(container_string == NULL) return false;
    IF_ASSERT(container_string_length <= 0) return false;

    size_t wrote = fwrite(container_string, sizeof(char), container_string_length, f);

    GS_ASSERT(wrote == container_string_length);
    return (wrote == container_string_length);
}

static bool save_container_file_header(FILE* f, int container_ct)
{
    int ret = fprintf(f, CONTAINER_FILE_HEADER_FMT, GS_VERSION, container_ct);
    IF_ASSERT(ret <= 0)
    {
        log_container_save_error("Failed to write temporary container header");
        return false;
    }
    ret = fprintf(f, "\n");
    IF_ASSERT(ret <= 0)
    {
        log_container_save_error("Failed to write temporary container header");
        return false;
    }
    return true;
}


/* Public Interface */

bool save_containers()
{
    bool err = false;
    int ct = 0;

    using ItemContainer::item_container_list;
    IF_ASSERT(item_container_list == NULL)
    {
        log_container_save_error("Item Container List was NULL");
        return false;
    }

    FILE* f = fopen(container_path_tmp, "w");
    IF_ASSERT(f == NULL)
    {
        log_container_save_error("Failed to open tmp container file for writing");
        return false;
    }

    // write a temporary file header. the container count is wrong, so we have to rewrite it later
    err = (!save_container_file_header(f, item_container_list->n_max));
    IF_ASSERT(err) goto Error;

    // write the container contents
    for (unsigned i=0; i<item_container_list->n_max; i++)
        if (item_container_list->a[i] != NULL && !item_container_list->a[i]->attached_to_agent)
        {
            bool success = save_container(f, item_container_list->a[i], ct+1);
            GS_ASSERT(success);
            if (success) ct++;
            else log_container_save_error("Failed to save container contents");
        }

    // rewind to beginning of file
    rewind(f);

    // write the actual file header
    err = (!save_container_file_header(f, ct));
    IF_ASSERT(err) goto Error;

    Error:  // GOTO LABEL
        int ferr = fclose(f);
        IF_ASSERT(ferr)
            log_container_save_error("Container file close error");

    return (!err && !ferr);
}

bool load_containers()
{
    if (file_exists(container_path) && get_filesize(container_path) > 0)
        return load_container_file(container_path);
    else
    if (file_exists(container_path_bak) && get_filesize(container_path_bak) > 0)
        return load_container_file(container_path_bak);
    else
        printf("WARNING: No container file found\n");
    return true;
}

}   // serializer
