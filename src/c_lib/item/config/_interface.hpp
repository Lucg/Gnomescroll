/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/common/enum.hpp>
#include <SDL/constants.hpp>
#include <item/properties.hpp>
#include <item/container/config/_interface.hpp>
#include <agent/attributes.hpp>
#include <common/dat/attributes.hpp>
#if DC_CLIENT
# include <SDL/texture_sheet_loader.hpp>
#endif

namespace Item
{

void iso_block_sprite_def(const char* block_name);

ItemAttribute* s = NULL;
static Modifier* _current_modifier = NULL;

#if DC_CLIENT
static SpriteSheet _item_cube_iso_spritesheet_id = NULL_SPRITE_SHEET;
#endif

bool item_def(ItemGroup group, const char* name)
{
    _current_modifier = NULL;
    if (s == NULL)
    {
        if (group != IG_ERROR)
            printf("First item MUST be the error item, with group IG_ERROR\n");
        GS_ASSERT_ABORT(group == IG_ERROR);
        IF_ASSERT(group != IG_ERROR) return false;
        #if DC_CLIENT
        GS_ASSERT_ABORT(_item_cube_iso_spritesheet_id == -1);
        _item_cube_iso_spritesheet_id = TextureSheetLoader::item_texture_alias(SCREENSHOT_PATH "fbo_test_16.png");
        #endif
    }

    GS_ASSERT_ABORT(group != IG_NONE);
    IF_ASSERT(group == IG_NONE) return false;

    s = item_attributes->get_next();
    IF_ASSERT(s == NULL) return false;

    s->group = group;
    s->set_name(name);

    return true;
}

// use in place of item_def for items that are equivalent to a block
bool item_block_def(const char* block_name)
{
    CubeType cube_type = t_map::get_cube_type(block_name);
    GS_ASSERT(t_map::isValidCube(cube_type));
    if (!t_map::isValidCube(cube_type)) return true;

    // item will have same name as the block
    if (!item_def(IG_PLACER, block_name)) return false;

    iso_block_sprite_def(block_name);
    s->cube_type = cube_type;
    s->max_stack_size = 64;
    s->particle_voxel = true;
    s->particle_voxel_texture = t_map::get_cube_primary_texture_index(block_name);
    s->cube_height = 1;

    char* pretty = make_pretty_name(block_name);
    s->set_pretty_name(pretty);
    free(pretty);

    return true;
}

// use in place of item_def for items that are equivalent to an item container block
bool item_container_def(const char* container_name)
{
    ItemContainerType container_type = ItemContainer::get_type(container_name);
    GS_ASSERT_ABORT(container_type != NULL_CONTAINER_TYPE);
    if (container_type == NULL_CONTAINER_TYPE) return false;
    GS_ASSERT_ABORT(!ItemContainer::container_type_is_attached_to_agent(container_type));
    if (ItemContainer::container_type_is_attached_to_agent(container_type)) return false;

    // container_name is synced with block_name
    if (!item_block_def(container_name)) return false;

    s->max_stack_size = 1;
    s->container_type = container_type;

    return true;
}

void block_damage_def(CubeMaterial material, int damage)
{
    GS_ASSERT_ABORT(s != NULL);
    IF_ASSERT(s == NULL) return;
    GS_ASSERT_ABORT(damage >= 0 && damage <= MAX_CUBE_DAMAGE);
    IF_ASSERT(damage < 0 || damage > MAX_CUBE_DAMAGE) return;
    bool set_any = false;
    for (int i=0; i<MAX_CUBES; i++)
        if (t_map::isInUse((CubeType)i) &&
            t_map::get_cube_material((CubeType)i) == material)
        {
            s->block_damage[i] = damage;
            set_any = true;
        }
    if (!set_any) printf("block_damage_def error: No cubes found with materal %d\n", material);
    GS_ASSERT_ABORT(set_any);
}

void block_damage_def(int damage)
{   // apply to all groups
    GS_ASSERT_ABORT(s != NULL);
    IF_ASSERT(s == NULL) return;
    GS_ASSERT_ABORT(damage >= 0);
    for (int i=0; i<MAX_CUBES; i++)
        s->block_damage[i] = damage;
}

void set_pretty_name(const char* pretty_name)
{
    GS_ASSERT_ABORT(s != NULL);
    IF_ASSERT(s == NULL) return;
    item_attributes->set_pretty_name(s->type, pretty_name);
}

#if DC_CLIENT
void sprite_def(SpriteSheet spritesheet, int ypos, int xpos)
{
    GS_ASSERT_ABORT(s != NULL);
    IF_ASSERT(s == NULL) return;
    // can't check maximums because they are unknown
    GS_ASSERT_ABORT(xpos >= 1 && ypos >= 1);
    IF_ASSERT(xpos < 1 || ypos < 1) return;

    // check if we have already set this sprite
    GS_ASSERT_ABORT(s->sprite == ERROR_SPRITE);

    int sprite = TextureSheetLoader::blit_item_texture(spritesheet, xpos, ypos);
    GS_ASSERT_ABORT(sprite != NULL_SPRITE);
    IF_ASSERT(sprite == NULL_SPRITE) return;

    s->sprite = sprite;
}

void iso_block_sprite_def(const char* block_name)
{
    GS_ASSERT_ABORT(_item_cube_iso_spritesheet_id != -1);
    IF_ASSERT(_item_cube_iso_spritesheet_id == -1) return;

    CubeType cube_type = t_map::get_cube_type(block_name);
    IF_ASSERT(!isValid(cube_type)) return;

    int xpos = (cube_type % 16) + 1;
    int ypos = (cube_type / 16) + 1;
    int sprite = TextureSheetLoader::blit_item_texture(_item_cube_iso_spritesheet_id, xpos, ypos);
    GS_ASSERT_ABORT(sprite != NULL_SPRITE);
    IF_ASSERT(sprite == NULL_SPRITE) return;
    s->sprite = sprite;
}

#else
void sprite_def(SpriteSheet spritesheet, int xpos, int ypos) {}
void iso_block_sprite_def(const char* block_name) {}
#endif

// item_def("regen_potion", IG_CONSUMABLE);
// set_modifier_amount("health", 5);
// set_modifier_periodic(ONE_MINUTE * 5, ONE_SECOND * 10);

static void _set_next_modifier(const char* name)
{
    // modifiers on groups other than these are not used yet:
    GS_ASSERT(s->group == IG_EQUIPMENT || s->group == IG_CONSUMABLE);
    _current_modifier = NULL;
    AttributeType type = Attributes::get_type(Agents::base_stats, name);
    IF_ASSERT(type == NULL_ATTRIBUTE) return;
    _current_modifier = s->modifiers.create();
    _current_modifier->set_attribute_type(type);
}

void set_modifier_amount(const char* name, int amount)
{   // for +/- value
    // EXAMPLE:
    // item takes 30 health (poisonous)
    // set_modifier_percent("health", -30);
    _set_next_modifier(name);
    IF_ASSERT(_current_modifier == NULL) return;
    _current_modifier->set_amount(amount);
}

void set_modifier_percent(const char* name, float percent)
{   // for +/- %value.
    // EXAMPLE:
    // item gives +10% health
    // set_modifier_percent("health", 10);
    _set_next_modifier(name);
    IF_ASSERT(_current_modifier == NULL) return;
    _current_modifier->set_percent(percent);
}

void set_modifier_periodic(int duration, int period)
{   // For an effect that lasts for "duration" ticks, triggering every "period" ticks
    // EXAMPLE:
    // apply modifier once every 20 seconds, for 5 minutes
    // set_modifier_periodic(ONE_MINUTE * 5, 20 * ONE_SECOND);
    IF_ASSERT(_current_modifier == NULL) return;
    _current_modifier->set_periodic(duration, period);
}

void set_modifier_duration(int duration)
{   // For an effect that lasts for "duration" ticks
    // EXAMPLE:
    // effect lasts 20 minutes
    // set_modifier_duration(ONE_MINUTE * 20);
    IF_ASSERT(_current_modifier == NULL) return;
    _current_modifier->set_duration(duration);
}

void set_modifier_instant()
{   // For an effect that occurs instantly
    // EXAMPLE
    // item_def("apple", IG_CONSUMABLE);
    // ...
    // set_modifier_amount("health", 5);
    // set_modifier_instant();
    IF_ASSERT(_current_modifier == NULL) return;
    _current_modifier->set_instant();
}

}   // Item


namespace Item
{

int crafting_recipe_count = 0;
CraftingRecipe _cr;

void end_crafting_recipe();

void def_crafting_recipe(const char* item_name, int amount)
{
    if (_cr.reagent_num > 0) end_crafting_recipe();
    _cr.output = get_item_type(item_name);
    _cr.output_stack = amount;
}

void def_crafting_recipe(const char* item_name)
{
    def_crafting_recipe(item_name, 1);
}

void set_crafting_reagent(const char* item_name, int quantity)
{
    GS_ASSERT_ABORT(crafting_recipe_count < MAX_CRAFTING_RECIPE);
    if (crafting_recipe_count >= MAX_CRAFTING_RECIPE) return;

    GS_ASSERT_ABORT(_cr.reagent_num < CRAFT_BENCH_INPUTS_MAX);
    if (_cr.reagent_num >= CRAFT_BENCH_INPUTS_MAX) return;

    ItemType type = get_item_type(item_name);
    GS_ASSERT_ABORT(type != NULL_ITEM_TYPE);

    // Make sure we aren't adding two types of different stack values
    // Why? our sorting methods for doing recipe matches do not sort by
    // stack values per type, so there will be undefined behaviour
    for (int i=0; i<_cr.reagent_num; i++)
        GS_ASSERT_ABORT(_cr.reagent[i] != type || _cr.reagent_count[i] == quantity);

    // insert reagents sorted by type
    if (_cr.reagent_num == 0)
    {   // degenerate case
        _cr.reagent[_cr.reagent_num] = type;
        _cr.reagent_count[_cr.reagent_num] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_cr.reagent_num; i++)
        {
            if (_cr.reagent[i] <= type) continue;

            // shift forward
            for (int j=_cr.reagent_num; j>i; j--) _cr.reagent[j] = _cr.reagent[j-1];
            for (int j=_cr.reagent_num; j>i; j--) _cr.reagent_count[j] = _cr.reagent_count[j-1];

            // insert
            _cr.reagent[i] = type;
            _cr.reagent_count[i] = quantity;
            break;
        }

        if (i == _cr.reagent_num)
        {   // append to end
            _cr.reagent[_cr.reagent_num] = type;
            _cr.reagent_count[_cr.reagent_num] = quantity;
        }
    }

    _cr.reagent_num++;
}

void end_crafting_recipe()
{
    GS_ASSERT_ABORT(crafting_recipe_count < MAX_CRAFTING_RECIPE);

    GS_ASSERT_ABORT(_cr.reagent_num > 0);
    GS_ASSERT_ABORT(_cr.output != NULL_ITEM_TYPE);
    GS_ASSERT_ABORT(_cr.output_stack >= 1);
    for (int i=0; i<_cr.reagent_num; i++)
        GS_ASSERT_ABORT(_cr.reagent[i] != NULL_ITEM_TYPE);

    IF_ASSERT(_cr.output_stack > get_max_stack_size(_cr.output))
    {
        printf("Recipe output item \"%s\"'s stack size is greater than its defined max stack size\n", get_item_name(_cr.output));
        GS_ABORT();
    }

    // check that adding this recipe will not increase the total outputs
    // per recipe above limit
    int matching_recipes = 0;
    for (int i=0; i<crafting_recipe_count; i++)
    {
        if (crafting_recipe_array[i].reagent_num != _cr.reagent_num) continue;
        int j=0;
        for (; j<crafting_recipe_array[i].reagent_num; j++)
            if (crafting_recipe_array[i].reagent[j] != _cr.reagent[j]) break;
        if (j == crafting_recipe_array[i].reagent_num)
            matching_recipes++;
    }
    if (matching_recipes >= CRAFT_BENCH_OUTPUTS_MAX)
        printf("ERROR: number of resultant crafting recipes exceeds CRAFT_BENCH_OUTPUTS_MAX for %s\n", get_item_name(_cr.output));
    GS_ASSERT_ABORT(matching_recipes < CRAFT_BENCH_OUTPUTS_MAX);

    // check that this recipe's type signature (input+output)
    // has not already been defined
    for (int i=0; i<crafting_recipe_count; i++)
    {
        if (crafting_recipe_array[i].reagent_num != _cr.reagent_num) continue;
        int j=0;
        for (; j<crafting_recipe_array[i].reagent_num; j++)
            if (crafting_recipe_array[i].reagent[j] != _cr.reagent[j]) break;
        if (j < crafting_recipe_array[i].reagent_num) continue;
        // reagents matched, check products
        GS_ASSERT_ABORT(crafting_recipe_array[i].output != _cr.output);
    }

    // check if recursive recipe (reagents == output)
    if (_cr.reagent_num == 1) GS_ASSERT_ABORT(_cr.reagent[0] != _cr.output);

    _cr.id = crafting_recipe_count;
    crafting_recipe_array[crafting_recipe_count] = _cr;
    _cr.init();
    crafting_recipe_count++;
}

}   // Item


namespace Item
{

int smelting_recipe_count = 0;
SmeltingRecipe _sr;

void end_smelting_recipe();

void add_smelting_product(const char* item_name, int amount)
{
    GS_ASSERT_ABORT(_sr.output_num < SMELTER_OUTPUTS_MAX);
    _sr.output[_sr.output_num] = get_item_type(item_name);
    _sr.output_stack[_sr.output_num] = amount;
    _sr.output_num++;
}

void add_smelting_product(const char* item_name)
{
    add_smelting_product(item_name, 1);
}

void def_smelting_recipe(const char* item_name, int amount)
{
    if (_sr.reagent_num > 0) end_smelting_recipe();
    add_smelting_product(item_name, amount);
}

void def_smelting_recipe(const char* item_name)
{
    if (_sr.reagent_num > 0) end_smelting_recipe();
    def_smelting_recipe(item_name, 1);
}

void set_smelting_reagent(const char* item_name, int quantity)
{
    GS_ASSERT_ABORT(_sr.reagent_num < SMELTER_INPUTS_MAX);
    GS_ASSERT_ABORT(_sr.output_num < SMELTER_OUTPUTS_MAX);

    ItemType type = get_item_type(item_name);
    GS_ASSERT_ABORT(isValid(type));
    IF_ASSERT(!isValid(type)) return;

    // Make sure we aren't adding two types of different stack values
    // Why? our sorting methods for doing recipe matches do not sort by
    // stack values per type, so there will be undefined behaviour
    for (int i=0; i<_sr.reagent_num; i++)
        GS_ASSERT_ABORT(_sr.reagent[i] != type || _sr.reagent_count[i] == quantity);

    // insert reagents sorted by type
    if (_sr.reagent_num == 0)
    {   // degenerate case
        _sr.reagent[_sr.reagent_num] = type;
        _sr.reagent_count[_sr.reagent_num] = quantity;
    }
    else
    {   // keep reagents sorted by type
        int i=0;
        for (; i<_sr.reagent_num; i++)
        {
            if (_sr.reagent[i] <= type) continue;

            // shift forward
            for (int j=_sr.reagent_num; j>i; j--) _sr.reagent[j] = _sr.reagent[j-1];
            for (int j=_sr.reagent_num; j>i; j--) _sr.reagent_count[j] = _sr.reagent_count[j-1];

            // insert
            _sr.reagent[i] = type;
            _sr.reagent_count[i] = quantity;
            break;
        }

        if (i == _sr.reagent_num)
        {   // append to end
            _sr.reagent[_sr.reagent_num] = type;
            _sr.reagent_count[_sr.reagent_num] = quantity;
        }
    }

    _sr.reagent_num++;
}

// in total ticks to synthesize
void set_smelting_creation_time(int creation_time)
{
    _sr.creation_time = creation_time;
}

void end_smelting_recipe()
{
    GS_ASSERT_ABORT(smelting_recipe_count <= MAX_SMELTING_RECIPE);

    GS_ASSERT_ABORT(_sr.reagent_num > 0);
    GS_ASSERT_ABORT(_sr.output_num > 0);

    // make sure no other recipe has same type signature
    for (int i=0; i<smelting_recipe_count; i++)
    {
        if (smelting_recipe_array[i].reagent_num != _sr.reagent_num) continue;
        int j=0;
        for (; j<smelting_recipe_array[i].reagent_num; j++)
            if (smelting_recipe_array[i].reagent[j] != _sr.reagent[j])
                break;
        GS_ASSERT_ABORT(j < smelting_recipe_array[i].reagent_num);
    }

    // check that inputs != outputs
    if (_sr.reagent_num == _sr.output_num)
    {
        int i=0;
        for (; i<_sr.reagent_num; i++)
            if (_sr.reagent[i] != _sr.output[i]) break;
        GS_ASSERT_ABORT(i < _sr.reagent_num);
    }

    _sr.reagent_num = _sr.reagent_num;
    _sr.id = smelting_recipe_count;
    smelting_recipe_array[smelting_recipe_count] = _sr;
    _sr.init();
    smelting_recipe_count++;
}

}   // Item


namespace Item
{

int _current_synthesizer_item = 0;
ItemType _current_synthesizer_item_type = NULL_ITEM_TYPE;
int _current_synthesizer_item_cost = NULL_COST;

void synthesizer_item_def(const char* item_name, int cost);
void synthesizer_item_set(int xslot, int yslot);


void synthesizer_item_def(const char* item_name, int cost)
{
    GS_ASSERT_ABORT(cost != NULL_COST && cost > 0 && count_digits(cost) <= SYNTHESIZER_ITEM_COST_MAX_STRLEN);
    ItemType item_type = get_item_type(item_name);
    GS_ASSERT_ABORT(item_type != NULL_ITEM_TYPE);
    _current_synthesizer_item_type = item_type;
    _current_synthesizer_item_cost = cost;
}

void synthesizer_item_set(int xslot, int yslot)
{
    GS_ASSERT_ABORT(_current_synthesizer_item < ItemContainer::get_container_alt_max_slots(ItemContainer::name::synthesizer));

    SynthesizerItem* n = &synthesizer_item_array[_current_synthesizer_item];

    GS_ASSERT_ABORT(_current_synthesizer_item_type != NULL_ITEM_TYPE);
    GS_ASSERT_ABORT(xslot >= 0 && xslot < ItemContainer::get_container_alt_xdim(ItemContainer::name::synthesizer));
    GS_ASSERT_ABORT(yslot >= 0 && yslot < ItemContainer::get_container_alt_ydim(ItemContainer::name::synthesizer));
    GS_ASSERT_ABORT(_current_synthesizer_item_cost > 0);
    GS_ASSERT_ABORT(count_digits(_current_synthesizer_item_cost) <= SYNTHESIZER_ITEM_COST_MAX_STRLEN);

    // make sure we are not defining a purchase twice for the same item
    // or overwriting an existing purchase slot
    for (int i=0; i<_current_synthesizer_item; i++)
    {
        GS_ASSERT_ABORT(synthesizer_item_array[i].item_type != _current_synthesizer_item_type);
        GS_ASSERT_ABORT(synthesizer_item_array[i].xslot != xslot || synthesizer_item_array[i].yslot != yslot);
    }

    n->item_type = (ItemType)_current_synthesizer_item_type;
    n->cost = _current_synthesizer_item_cost;
    n->xslot = xslot;
    n->yslot = yslot;

    _current_synthesizer_item_type = NULL_ITEM_TYPE;
    _current_synthesizer_item_cost = NULL_COST;
    _current_synthesizer_item++;
}

}   // Item
