#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"

#include "../memory.h"


void tile_wood_hurt(level_t *lvl, uint dmg, uint x, uint y)
{
    dmg += lvl_get_data(lvl, x, y);

    //@todo add particles

    if(dmg > 20)
    {
        lvl_set_tile(lvl, x, y, tile_get(TILE_GRASS));
        item_add_to_inventory(&ITEM_WOOD, &lvl_get_player(lvl)->player.inventory);
    } else {
        lvl_set_data(lvl, x, y, dmg);
    }

}


void tile_wood_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(item && item->tooltype == TOOL_TYPE_AXE && plr_pay_stamina(ent, 4 - item->level))
    {
       tile_wood_hurt(ent->level, 5 + item->level, x, y);
    }
}
