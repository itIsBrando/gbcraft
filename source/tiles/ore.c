#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"

#include "../random.h"


void tile_iron_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    dmg += lvl_get_data(lvl, x, y);

    if(dmg > 80)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_MUD));
        x = lvl_to_pixel_x(x);
        y =  lvl_to_pixel_y(y);
        ent_item_new(lvl, x, y, (item_t*)&ITEM_IRON_ORE, 2 + (rnd_random() & 0x3));
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


void tile_iron_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if((item && item->tooltype != TOOL_TYPE_PICKAXE) || ent->type != ENT_TYPE_PLAYER)
        return;

    if(plr_pay_stamina(ent, 4 - item->level))
        tile_iron_hurt(ent->level, 2 + (item->level << 1), x, y);
}