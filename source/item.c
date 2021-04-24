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
    { // axe (STONE)
        .tile=17,
        .level=2,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_AXE,
        .name="STONE AXE",
    },
    { // pickaxe (STONE)
        .tile=19,
        .level=2,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_PICKAXE,
        .name="STONE PICK",
    },
    { // sword (STONE)
        .tile=18,
        .level=2,
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
        .furnituretype=FURNITURE_TYPE_CRAFTING,
        .type=ITEM_TYPE_FURNITURE,
        .name="BENCH",
    },
    { // axe (WOOD)
        .tile=17,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_AXE,
        .name="WOOD AXE",
    },
    { // pickaxe (WOOD)
        .tile=19,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_PICKAXE,
        .name="WOOD PICK",
    },
    { // sword (WOOD)
        .tile=18,
        .level=1,
        .event=&ITEM_EVENTS[2],
        .type=ITEM_TYPE_TOOL,
        .tooltype=TOOL_TYPE_SWORD,
        .name="WOOD SWORD",
    },
    {  // chest
        .tile=33,
        .count=1,
        .event=&ITEM_EVENTS[3],
        .furnituretype=FURNITURE_TYPE_CHEST,
        .type=ITEM_TYPE_FURNITURE,
        .name="CHEST",
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


bool item_can_attack(const item_t *item)
{
    if(item == NULL || item->type == ITEM_TYPE_TOOL)
        return true;

    return false;
}


void item_add_to_inventory(const item_t *item, inventory_t *inv)
{
    item_t *i = item_get_from_inventory_matching(item, inv);
    // check if we already have this item in our inventory
    if(i) {
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
        if(&inv->items[i] == item)
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
        {
            // make sure the tool levels are equal
            if(item->type == ITEM_TYPE_TOOL && item->level != inv->items[i].level)
                break;
            else if(item->type == ITEM_TYPE_FURNITURE && item->furnituretype != inv->items[i].furnituretype)
                break;
            
            return (item_t*)&inv->items[i];
        }
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


static const item_t *_furn_items[] = {
    &ITEM_BENCH,
    &ITEM_CHEST,
};


bool item_tool_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{

    // pick up furniture entities
    if(item->tooltype == TOOL_TYPE_PICKUP)
    {
        x = lvl_to_pixel_x(plr->level, x);
        y = lvl_to_pixel_y(plr->level, y);
        u8 s;
        ent_t **e = ent_get_all(plr->level, x, y, &s);

        for(uint i = 0; i < s; i++)
        {
            if(e[i]->type == ENT_TYPE_FURNITURE)
            {
                item_add_to_inventory(_furn_items[e[i]->furniture.type], &plr->player.inventory);
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


// sprite indexes for 16x16 furniture sprites
static const u8 _fur_spr[] = {53, 49};

bool item_furniture_interact(item_t *item, ent_t *plr, const tile_t *tile, u16 x, u16 y)
{

    // create furniture item
    x = (x << 4) - bg_get_scx(main_background);
    y = (y << 4) - bg_get_scy(main_background);
    x += dir_get_x(plr->dir);
    y += dir_get_y(plr->dir);

    ent_t *e = ent_add(plr->level, ENT_TYPE_FURNITURE, x, y);
    e->furniture.type = item->furnituretype;
    spr_set_tile(e->sprite, _fur_spr[item->furnituretype]);  // @todo tile number will change based on furniture used

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

/** palette numbers for tools
 * - wood
 * - stone
 * - iron
 * - gold
 * - gem
*/
static const u16 _pals[] = {1, 0, 0};


obj_t *item_new_icon(item_t *item , u16 x, u16 y)
{
    obj_t *s = spr_alloc(x, y, item->tile);
    item_set_icon(s, item);
    
    return s;
}


/** @see item_new_icon */
void item_set_icon(obj_t *obj, const item_t *item)
{
    spr_set_tile(obj, item->tile);
    spr_show(obj);

    if(item->type == ITEM_TYPE_TOOL)
        spr_set_pal(obj, _pals[item->level-1]);
}