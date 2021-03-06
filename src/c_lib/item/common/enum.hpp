/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

typedef enum
{
    NULL_ITEM = 0xFFFF
} ItemID;

typedef enum
{
    NULL_ITEM_TYPE = 0xFF
} ItemType;

typedef enum
{
    NULL_CONTAINER = 0xFFFF
} ItemContainerID;

typedef enum
{
    NULL_CONTAINER_TYPE = 0
} ItemContainerType;

typedef enum
{
    NULL_PARTICLE = 0xFFFF
} ItemParticleID;

typedef enum
{
    CONTAINER_ACTION_NONE = 0,      // null

    FULL_HAND_TO_WORLD,             // remove
    PARTIAL_HAND_TO_WORLD,

    // inserts
    FULL_HAND_TO_EMPTY_SLOT,        // drop
    FULL_HAND_TO_OCCUPIED_SLOT,     // merge full stack

    PARTIAL_HAND_TO_EMPTY_SLOT,     // insert partial stack
    PARTIAL_HAND_TO_OCCUPIED_SLOT,  // merge partial stack

    // pickups
    PARTIAL_SLOT_TO_EMPTY_HAND,     // create partial stack in hand
    FULL_SLOT_TO_EMPTY_HAND,        // pickup

    // both
    FULL_HAND_SWAP_WITH_SLOT,       // swap hand with slot

    // synthesizer shopping
    PURCHASE_ITEM_FROM_SYNTHESIZER,

    // crafting
    CRAFT_ITEM_FROM_BENCH,

    // recycling
    CRUSHER_CRUSH_ITEM,

    // pickup
    FULL_WORLD_TO_EMPTY_SLOT,
    FULL_WORLD_TO_OCCUPIED_SLOT,
    PARTIAL_WORLD_TO_OCCUPIED_SLOT,

} ContainerActionType;

typedef enum
{
    IG_NONE = 0,    // null value
    IG_ERROR,
    IG_FIST,
    IG_RESOURCE,    //does nothing, resources, stackable
    IG_PLACER,      //consumed to create block
    IG_MECH_PLACER, //consued to create mech
    IG_MECH_PLACER_ORIENTED, //places oriented mechs (ex signs)
    IG_PLANT_PLACER, //places plant objects
    IG_HITSCAN_WEAPON,
    IG_MELEE_WEAPON,
    IG_MINING_LASER,
    IG_SHOVEL,
    IG_GRENADE_LAUNCHER,
    IG_SYNTHESIZER_COIN,
    IG_DEBUG,       // debug only items
    IG_UNKNOWN,     // used by recipes
    IG_CONSUMABLE,
    IG_ENERGY_TANK,
    IG_AGENT_SPAWNER,
    IG_ENERGY_CORE,
    IG_SPECIAL,
    IG_EQUIPMENT,
    IG_TOOL,
} ItemGroup;

typedef enum
{
    IL_NOWHERE = 0,
    IL_PARTICLE,
    IL_CONTAINER,
    IL_HAND,
} ItemLocationType;

typedef enum
{
    NULL_EQUIPMENT_TYPE = 0,
    EQUIPMENT_TYPE_BODY,
    EQUIPMENT_TYPE_HEAD,
    EQUIPMENT_TYPE_FEET,
    EQUIPMENT_TYPE_HANDS,
    EQUIPMENT_TYPE_ACCESSORY,

    // keep this at the end
    N_EQUIPMENT_TYPES
} EquipmentType;

/* No typedef for ItemType, ItemContainerType, use string or unpacked names */
