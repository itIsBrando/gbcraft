#include <stdlib.h>
#include <string.h>
#include "text.h"
#include "obj.h"

#include "menu.h"
#include "item.h"
#include "entity.h"
#include "level.h"
#include "tile.h"

// order does matter. Just append to end
item_event_t ITEM_EVENTS[] = 
{
    [0]={ // wood
        .interact=item_wood_interact
    },
    [1]={ // stone
        .interact=item_stone_interact
    },
    [2]={ // tools
        .canattack=item_can_attack_all,
        .interact=item_tool_interact
    },
    [3]={ // furniture
        .interact=item_furniture_interact
    },
    [4]={ // ore, coal, & gem
        
    },
    [5]={ //door
        .interact=item_door_interact
    },
    [6]={// wooden floor
        .interact=item_floor_interact
    },
    [7]={ // sapling
        .interact=item_sapling_interact
    },
    [8]={ // seed
        .interact=item_seed_interact
    },
    [9]={ // material (has no events)

    },
    [10]={ // food
        .interact=item_food_interact
    }
};

/**
 * @param name name of the item 
 * @param tile tile index of item
 * @param type item_type_t
 * @param eventNum index of the events in ITEM_EVENTS
 * @param level Should be 1 for regular items or equal to the level of a tool if the item is a tool
 * @param __VA_ARGS__ extra properties
 */
#define DEFINE_ITEM(_name, _tile, _type, _eventNum, _level, ...) { \
 .tile=(_tile),\
 .count=(_level),\
 .event=&ITEM_EVENTS[_eventNum],\
 .type=_type,\
 .name=_name,\
 __VA_ARGS__\
}

/** @note these can be in any order, so long as indexes are changed in `item.h` **/
const item_t ALL_ITEMS[] = {
    DEFINE_ITEM("WOOD", 9, ITEM_TYPE_WOOD, 0, 1),
    DEFINE_ITEM("STONE", 10, ITEM_TYPE_STONE, 1, 1),
    DEFINE_ITEM("STONE AXE", 17, ITEM_TYPE_TOOL, 2, 2, .tooltype=TOOL_TYPE_AXE, .palette=0),
    DEFINE_ITEM("STONE PICK", 19, ITEM_TYPE_TOOL, 2, 2, .tooltype=TOOL_TYPE_PICKAXE, .palette=0),
    DEFINE_ITEM("STONE SWORD", 18, ITEM_TYPE_TOOL, 2, 2, .tooltype=TOOL_TYPE_SWORD, .palette=0),
    DEFINE_ITEM("PICKUP TOOL", 44, ITEM_TYPE_TOOL, 2, 1, .tooltype=TOOL_TYPE_PICKUP, .palette=0),
    DEFINE_ITEM("BENCH", 34, ITEM_TYPE_FURNITURE, 3, 1, .furnituretype=FURNITURE_TYPE_CRAFTING),
    DEFINE_ITEM("WOOD AXE", 17, ITEM_TYPE_TOOL, 2, 1, .tooltype=TOOL_TYPE_AXE, .palette=1),
    DEFINE_ITEM("WOOD PICK", 19, ITEM_TYPE_TOOL, 2, 1, .tooltype=TOOL_TYPE_PICKAXE, .palette=1),
    DEFINE_ITEM("WOOD SWORD", 18, ITEM_TYPE_TOOL, 2, 1, .tooltype=TOOL_TYPE_SWORD, .palette=1),
    DEFINE_ITEM("CHEST", 33, ITEM_TYPE_FURNITURE, 3, 1, .furnituretype=FURNITURE_TYPE_CHEST),
    DEFINE_ITEM("IRON ORE", 12, ITEM_TYPE_IRON_ORE, 4, 1),
    DEFINE_ITEM("GOLD ORE", 2, ITEM_TYPE_GOLD_ORE, 4, 1),
    DEFINE_ITEM("IRON", 13, ITEM_TYPE_IRON, 4, 1),
    DEFINE_ITEM("GOLD", 2, ITEM_TYPE_GOLD, 4, 1),
    DEFINE_ITEM("FURNACE", 35, ITEM_TYPE_FURNITURE, 3, 1, .furnituretype=FURNITURE_TYPE_FURNACE),
    DEFINE_ITEM("COAL", 11, ITEM_TYPE_COAL, 4, 1),
    DEFINE_ITEM("DOOR", 36, ITEM_TYPE_DOOR, 5, 1),
    DEFINE_ITEM("WOOD FLR", 37, ITEM_TYPE_FLOOR, 6, 1),
    DEFINE_ITEM("WOOD HOE", 20, ITEM_TYPE_TOOL, 2, 1, .tooltype=TOOL_TYPE_HOE),
    DEFINE_ITEM("SEED", 38, ITEM_TYPE_SEED, 8, 1),
    DEFINE_ITEM("SAPLING", 39, ITEM_TYPE_SAPLING, 7, 1),
    DEFINE_ITEM("WHEAT", 40, ITEM_TYPE_WHEAT, 9, 1),
    DEFINE_ITEM("BREAD", 41, ITEM_TYPE_BREAD, 10, 1),
    DEFINE_ITEM("ANVIL", 42, ITEM_TYPE_FURNITURE, 3, 1, .furnituretype=FURNITURE_TYPE_ANVIL),
    DEFINE_ITEM("IRON AXE", 17, ITEM_TYPE_TOOL, 2, 3, .tooltype=TOOL_TYPE_AXE, .palette=2),
    DEFINE_ITEM("IRON PICK", 19, ITEM_TYPE_TOOL, 2, 3, .tooltype=TOOL_TYPE_PICKAXE, .palette=2),
    DEFINE_ITEM("IRON SWORD", 18, ITEM_TYPE_TOOL, 2, 3, .tooltype=TOOL_TYPE_SWORD, .palette=2),
    DEFINE_ITEM("APPLE", 43, ITEM_TYPE_APPLE, 10, 1),
    DEFINE_ITEM("LANTERN", 35, ITEM_TYPE_FURNITURE, 3, 1, .furnituretype=FURNITURE_TYPE_LANTERN),
    DEFINE_ITEM("SLIME", 12, ITEM_TYPE_SLIME, 4, 1, .palette=1),
};


const char *item_lookup_name(item_t *item)
{
    uint size = sizeof(ALL_ITEMS) / sizeof(ALL_ITEMS[0]);
    
    return item_get_perfect_match(item, ALL_ITEMS, size)->name;
}


const item_t *item_get_from_type(item_type_t type, s16 data)
{
    for(uint i = 0; i < sizeof(ALL_ITEMS) / sizeof(ALL_ITEMS[0]); i++)
    {
        if(type == ALL_ITEMS[i].type)
        {
            if(data == -1)
                return &ALL_ITEMS[i];
            else if(data == ALL_ITEMS[i].tooltype)
                return &ALL_ITEMS[i];
        }
    }

    return NULL;
}


item_t *item_add_to_inventory(const item_t *item, inventory_t *inv)
{
    item_t *i = item_get_from_inventory_matching(item, inv);
    // check if we already have this item in our inventory
    if(i) {
        i->count += item->count;
        return i;
    } else {
        item_t *inv_item = &inv->items[inv->size++];
        *inv_item = *item;
        inv_item->parent = inv;
        return inv_item;
    }
}


bool item_remove_from_inventory(item_t *item)
{
    inventory_t *inv = item->parent;

    if(!inv)
        return false;

    for(uint i = 0; i < inv->size; i++)
    {
        if(&inv->items[i] == item)
        {
            memcpy(&inv->items[i], &inv->items[i + 1], (30 - i) * sizeof(item_t));
            inv->size--;
            return true;
        }
    }

    return false;
}

item_t *item_get_perfect_match(const item_t *item, const item_t *list, const uint size)
{

    for(uint j = 0; j < size; j++)
    {
        if(list[j].type == item->type && list[j].tooltype == item->tooltype)
        {
            if(item->type == ITEM_TYPE_TOOL && item->level == list[j].level) {
                return (item_t*)&list[j];
            } else if(item->type != ITEM_TYPE_TOOL)
                return (item_t*)&list[j];
        }
    }

    return NULL;
}


inline item_t *item_get_from_inventory_matching(const item_t *item, const inventory_t *inv)
{
    return item_get_perfect_match(item, inv->items, inv->size);

}


item_t *item_get_from_inventory(const item_type_t type, const inventory_t *inv)
{
    for(uint i = 0; i < inv->size; i++)
    {
        if(inv->items[i].type == type)
            return (item_t*)&inv->items[i];
    }

    return NULL;
}


bool item_floor_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_GRASS)
        return false;
    
    item_change_count(item, -1);

    lvl_set_tile(plr->level, x, y, tile_get(TILE_FLOOR_WOOD));

    return true;
}


bool item_door_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_GRASS)
        return false;

    item_change_count(item, -1);

    lvl_set_tile(plr->level, x, y, tile_get(TILE_DOOR_CLOSED));

    return true;
}


bool item_wood_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_GRASS)
        return false;

    item_change_count(item, -1);

    lvl_set_tile(plr->level, x, y, tile_get(TILE_WOOD));

    return true;
}


bool item_stone_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_GRASS && tile->type != TILE_MUD)
        return false;

    item_change_count(item, -1);
    lvl_set_tile(plr->level, x, y, tile_get(TILE_STONE));
    
    return true;
}


static const item_t *_furn_items[] = {
    &ITEM_BENCH,
    &ITEM_CHEST,
    &ITEM_FURNACE,
    &ITEM_ANVIL,
    &ITEM_LANTERN,
};


bool item_seed_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_MUD || plr->level->layer)
        return false;

    item_change_count(item, -1);
    lvl_set_tile(plr->level, x, y, tile_get(TILE_SEEDED_MUD));

    return true;
}


bool item_sapling_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(tile->type != TILE_GRASS)
        return false;

    lvl_set_tile(plr->level, x, y, tile_get(TILE_SAPLING));
    item_change_count(item, -1);
    return true;
}


bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    // pick up furniture entities
    if(item && item->tooltype == TOOL_TYPE_PICKUP)
    {
        x = lvl_to_pixel_x(x) + 8;
        y = lvl_to_pixel_y(y) + 8;
        ent_t *e[5];
        uint s = ent_get_all_stack(plr->level, plr, e, x, y, 5);

        for(uint i = 0; i < s; i++)
        {
            if(e[i]->type == ENT_TYPE_FURNITURE)
            {
                item_add_to_inventory(_furn_items[e[i]->furniture.type], &plr->player.inventory);
                ent_remove(e[i]->level, e[i]);
                return true;
            }
        }

        return false;
    }

    if(tile->event->interact)
        tile->event->interact(plr, item, x, y);

    return true;
}


bool item_furniture_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    // create furniture item
    x = (x << 4) - bg_get_scx(main_background);
    y = (y << 4) - bg_get_scy(main_background);
    x += dir_get_x(plr->dir);
    y += dir_get_y(plr->dir);

    // if there are any entities on our tile, then return
    ent_t *ents[2];
    if(ent_get_all_stack(plr->level, NULL, ents, lvl_to_pixel_x(x), lvl_to_pixel_y(y), 2) > 0)
        return false;

    ent_t *e = ent_add(plr->level, ENT_TYPE_FURNITURE, x, y);
    e->furniture.type = item->furnituretype;
    ent_furniture_set_tile(e);

    if(tile->event->maypass && !tile->event->maypass(e))
    {
        ent_remove(plr->level, e);
        return false;
    }

    item_change_count(item, -1);

    return true;
}


bool item_food_interact(item_t *item, ent_t *plr, const tile_t *tile, uint x, uint y)
{
    if(plr_heal(plr, 2))
    {
        item_change_count(item, -1);
        return true;
    }

    return false;
}


bool item_can_attack_all(item_t *item, ent_t *ent)
{
    return true;
}


/**
 * Increases or decreases the count of this `item_t`
 */
void item_change_count(item_t *item, const s8 change)
{
    if(item->type == ITEM_TYPE_TOOL && change < 0)
    {
        item_remove_from_inventory(item);
        return;
    }

    if(item->count + change > 64)
        item->count = 64;
    else
        item->count += change;

    if(item->parent->parent->type == ENT_TYPE_PLAYER && plr_get_active_item(item->parent->parent) == item)
    {

        if(item->count > 0) {
            mnu_draw_item(item, 1, 2);
        } else {
            ent_player_set_active_item(item->parent->parent, NULL);
        }
    }

    if(item->count <= 0)
        item_remove_from_inventory(item);

}

/** palette numbers for tools
 * - wood
 * - stone
 * - iron
 * - gold
 * - gem
*/

obj_t *item_new_icon(item_t *item , uint x, uint y)
{
    obj_t *s = spr_alloc(x, y, item->tile);
    item_set_icon(s, item);
    
    return s;
}


/** @see item_new_icon */
void item_set_icon(obj_t *obj, const item_t *item)
{
    spr_set_tile(obj,  item ? item->tile : 15);
    spr_set_size(obj, SPR_SIZE_8x8);
    spr_set_priority(obj, SPR_PRIORITY_LOWEST);
    spr_show(obj);

    spr_set_pal(obj, item->palette);
}