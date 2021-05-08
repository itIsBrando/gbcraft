#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"


void tile_floor_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));

    ent_item_new(lvl, lvl_to_pixel_x(x), lvl_to_pixel_y(y), &ITEM_FLOOR_WOOD, 1);
}



void tile_floor_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(!item || item->tooltype != TOOL_TYPE_AXE)
        return;

    if(plr_pay_stamina(ent, 2))
        tile_floor_hurt(ent->level, 1, x, y);
}
