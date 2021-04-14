#include <string.h>
#include "text.h"

#include "item.h"
#include "entity.h"
#include "level.h"
#include "tile.h"


item_event_t ITEM_EVENTS[] = 
{
    { // wood
        .interact=item_wood_interact
    }
};

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
        .type=ITEM_TYPE_STONE,
        .name="STONE",
    }
};


void item_add_to_inventory(item_t *item, inventory_t *inv)
{
    item_t *i = item_get_from_inventory(item->type, inv);
    // check if we already have this item in our inventory
    if(i) {
        i->count += item->count;
    } else {
        item->parent = inv;
        inv->items[inv->size++] = *item;
    }
}


bool item_remove_from_inventory(item_t *item)
{
    if(!item->parent)
        return false;

    inventory_t *inv = item->parent;
    for(u16 i = 0; i < inv->size; i++)
    {
        if(&inv->items[i] == item)
        {
            memcpy(&inv[i], &inv[i + 1], 30 - i);
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
        item_remove_from_inventory(item);
    }

}