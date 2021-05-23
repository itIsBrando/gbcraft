#include "../level.h"
#include "../entity.h"
#include "../item.h"
#include "../tile.h"

#include "../random.h"


bool tile_grass_maypass(ent_t *e)
{
    if(e->type == ENT_TYPE_PLAYER) {
        plr_set_swim(e, false);
    }
    
    return true;
}


bool tile_grass_interact(ent_t *ent, item_t *item, uint x, uint y)
{
    if(!item)
        return false;

    if(item->tooltype != TOOL_TYPE_HOE || lvl_get_tile_type(ent->level, x, y) != TILE_GRASS)
        return false;

    lvl_set_tile(ent->level, x, y, tile_get(TILE_MUD));
    if((rnd_random() & 0x7) == 0)
        ent_item_new(ent->level, lvl_to_pixel_x(x), lvl_to_pixel_y(y), (item_t*)&ITEM_SEED, 1);
    
    return true;
}
