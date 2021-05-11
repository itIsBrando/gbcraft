#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"

#include "../random.h"

void tile_stone_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 35)
    {
        uint px = lvl_to_pixel_x(x), py = lvl_to_pixel_y(y);

        lvl_set_tile(lvl, x, y, tile_get(lvl->layer > 0 ? TILE_MUD : TILE_GRASS));

        ent_item_new(lvl, px, py, (item_t*)&ITEM_STONE, 2);
        if((rnd_random() & 0xF) == 0)
            ent_item_new(lvl, px, py, &ITEM_COAL, (rnd_random() & 0x1) + 1);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


void tile_stone_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(item && item->tooltype == TOOL_TYPE_PICKAXE && plr_pay_stamina(ent, 4))
        tile_stone_hurt(ent->level, 6 + (item->level << 2), x, y);
}

