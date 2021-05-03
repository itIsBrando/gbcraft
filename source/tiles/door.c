#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"



void tile_door_closed_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    // @todo add door_hurt
    if(item && item->tooltype == TOOL_TYPE_AXE)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_OPEN));
}


void tile_door_open_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    // @todo add door_hurt (can be the same for opened and closed)
    if(item && item->tooltype == TOOL_TYPE_AXE)
        return;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_CLOSED));
}
