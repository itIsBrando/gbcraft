#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"

#include "../random.h"


void tile_tree_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        x = lvl_to_pixel_x(x);
        y = lvl_to_pixel_y(y);
        ent_item_new(lvl, x, y, (item_t*)&ITEM_WOOD, 2);
        if((rnd_random() & 0x3) == 0)
            ent_item_new(lvl, x, y, (item_t*)&ITEM_SAPLING, 1);
        if((rnd_random() & 0x7) == 0)
            ent_item_new(lvl, x, y, &ITEM_APPLE, 1);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


bool tile_tree_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if((item && item->tooltype != TOOL_TYPE_AXE) || ent->type != ENT_TYPE_PLAYER)
        return false;

    if(!plr_pay_stamina(ent, 3 + (!item)))
        return false;

    uint bonus = item ? item->level : -1;

    tile_tree_hurt(ent->level, 5 + bonus, x, y);
    
    return true;
}
