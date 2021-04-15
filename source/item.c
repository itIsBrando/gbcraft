#include <string.h>
#include "text.h"

#include "item.h"
#include "entity.h"
#include "level.h"
#include "tile.h"

// order does matter. Just append to end
item_event_t ITEM_EVENTS[] = 
{
    { // wood
        .interact=item_wood_interact
    },
    { // stone
        .interact=item_stone_interact
    },
    { // tools
        .canattack=item_can_attack_all,
        .interact=item_tool_interact
    }
};


/** @note these can be in any order **/
const item_t ALL_ITEMS[] = {
    { // wood
        .tile=9,
        .count=1,
        .event=&ITEM_EVENTS[0],
        .type=ITEM_TYPE_WOOD,
        .name="WOOD",
    },
    { // stone
        .tile=10,
        .count=1,
        .event=&ITEM_EVENTS[1],
        .type=ITEM_TYPE_STONE,
        .name="STONE",
    },
    { // axe
        .tile=17,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_AXE,
        .name="STONE AXE",
    },
    { // pickaxe
        .tile=19,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_PICKAXE,
        .name="STONE PICK",
    },
    { // sword
        .tile=18,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_SWORD,
        .name="STONE SWORD",
    },
};


void item_add_to_inventory(const item_t *item, inventory_t *inv)
{
    item_t *i = item_get_from_inventory(item->type, inv);
    // check if we already have this item in our inventory
    if(i && i->type != ITEM_TYPE_TOOL) {
        i->count += item->count;
    } else {
        item_t *inv_item = &inv->items[inv->size++];
        *inv_item = *item;
        inv_item->parent = inv;
    }
}


bool item_remove_from_inventory(item_t *item)
{
    inventory_t *inv = item->parent;

    if(!inv)
        return false;

    for(u16 i = 0; i < inv->size; i++)
    {
        if(inv->items[i].type == item->type)
        {
            memcpy(&inv->items[i], &inv->items[i + 1], (30 - i) * sizeof(item_t));
            inv->size--;
            return true;
        }
    }

    return false;
}


item_t *item_get_from_inventory(const item_type_t type, const inventory_t *inv)
{
    for(u16 i = 0; i < inv->size; i++)
    {
        if(inv->items[i].type == type)
            return (item_t*)&inv->items[i];
    }

    return NULL;
}


bool item_wood_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{
    if(tile->type != TILE_GRASS)
        return false;

    item_change_count(item, -1);

    lvl_set_tile(plr->level, x, y, tile_get(TILE_WOOD));

    return true;
}


bool item_stone_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{
    if(tile->type != TILE_GRASS)
        return false;

    item_change_count(item, -1);

    lvl_set_tile(plr->level, x, y, tile_get(TILE_STONE));
    return true;
}



bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{
    if(tile->event->interact)
        tile->event->interact(plr, item, x, y);

    return true;
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
    if(item->count + change > 64)
        item->count = 64;
    else
        item->count += change;

    if(item->count <= 0)
    {
        ent_player_set_active_item(item->parent->parent, NULL);
        if(!item_remove_from_inventory(item))
            text_error("COULD NOT REMOVE ITEM FROM INVENTORY");
    }

}