#include <stdlib.h>
#include <string.h>
#include "text.h"

#include "menu.h"
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
    },
    { // furniture
        .interact=item_furniture_interact
    }
};


/** @note these can be in any order, so long as indexes are changed in `item.h` **/
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
    { // pickup tool
        .tile=20,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_PICKUP,
        .name="PICKUP TOOL",
    },
    {  // crafting table
        .tile=34,
        .count=1,
        .event=&ITEM_EVENTS[3],
        .type=ITEM_TYPE_FURNITURE,
        .name="BENCH",
    }
};


const item_t *item_get_from_type(item_type_t type, s16 data)
{
    for(u16 i = 0; i < sizeof(ALL_ITEMS) / sizeof(ALL_ITEMS[0]); i++)
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


item_t *item_get_from_inventory_matching(const item_t *item, const inventory_t *inv)
{
    for(u16 i = 0; i < inv->size; i++)
    {
        if(inv->items[i].type == item->type && inv->items[i].tooltype == item->tooltype)
            return (item_t*)&inv->items[i];
    }

    return NULL;

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


    u16 px = (plr->x + 4 + bg_get_scx(main_background)) >> 4,
        py = (plr->y + 4 + bg_get_scy(main_background)) >> 4;
    if(!(px == x && py == y))
    {   item_change_count(item, -1);
        lvl_set_tile(plr->level, x, y, tile_get(TILE_STONE));
    }

    return true;
}



bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{
    // pick up furniture entities
    if(item->tooltype == TOOL_TYPE_PICKUP)
    {
        x <<= 4, y <<= 4;
        x -= bg_get_scx(main_background);
        y -= bg_get_scy(main_background);
        u8 s;
        ent_t **e = ent_get_all(plr->level, x, y, &s);

        for(u16 i = 0; i < s; i++)
        {
            if(e[i]->type == ENT_TYPE_FURNITURE)
            {
                // @todo allow the type of furniture item to depend on the type of entity
                item_add_to_inventory(&ITEM_BENCH, &plr->player.inventory);
                ent_remove(e[i]->level, e[i]);
                free(e);
                return true;
            }
        }

        free(e);
        return false;
    }

    if(tile->event->interact)
        tile->event->interact(plr, item, x, y);

    return true;
}

// @todo prevent item from being placed on solid tile
bool item_furniture_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{

    // create furniture item
    x = (x << 4) - bg_get_scx(main_background);
    y = (y << 4) - bg_get_scy(main_background);
    x += dir_get_x(plr->dir);
    y += dir_get_y(plr->dir);

    ent_t *e = ent_add(plr->level, ENT_TYPE_FURNITURE, x, y);
    e->furniture = item->furniture;
    spr_set_tile(e->sprite, 53);  // @todo tile number will change based on furniture used

    if(tile->event->maypass && !tile->event->maypass(e))
    {
        ent_remove(plr->level, e);
        return false;
    }

    item_change_count(item, -1);

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

    if(item->parent->parent->player.activeItem == item)
    {

        if(item->count > 0)
        {
            mnu_draw_item(item, 1, 2);
        } else {
            ent_player_set_active_item(item->parent->parent, NULL);
            item_remove_from_inventory(item);
        }
    }


}