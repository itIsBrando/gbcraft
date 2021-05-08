#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"


void tile_door_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    dmg += lvl_get_data(lvl, x, y);

    if(dmg > 2) {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));

        x = lvl_to_pixel_x(x);
        y = lvl_to_pixel_y(y);

        ent_item_new(lvl, x, y, &ITEM_DOOR, 1);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }
}


void tile_door_closed_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(item && item->tooltype == TOOL_TYPE_AXE) {
        if(plr_pay_stamina(ent, 5 - item->level))
            tile_door_hurt(ent->level, 1, x, y);
        return;
    }

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_OPEN));
}


void tile_door_open_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(item && item->tooltype == TOOL_TYPE_AXE) {
        if(plr_pay_stamina(ent, 5 - item->level))
            tile_door_hurt(ent->level, 1, x, y);
        return;
    }

    lvl_set_tile(ent->level, x, y, tile_get(TILE_DOOR_CLOSED));
}
