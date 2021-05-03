#include "../level.h"
#include "../entity.h"
#include "../tile.h"
#include "../lighting.h"
#include "../menu.h"
#include "../terraingen.h"

#include "../bg.h"

/** called when entity collides with this. This is a stairway down
 * @param e player entity
 * @param x absolute tile x
 * @param y absolute tile y
 * @returns true if the player changed levels
 */
bool tile_stair_down_ontouch(ent_t *e, uint x, uint y)
{
    if(e->type != ENT_TYPE_PLAYER)
        return false;
    
    if(e->player.on_stairs > 0)
        return false;

    mnu_load_level();

    // @todo enter new level
    level_t *curLevel = e->level;
    uint curLayer = curLevel->layer;
    level_t *newLevel;

    plr_move_to(e, x, y);

    if(world[curLayer+1])
        newLevel = world[curLayer+1];
    else {
        // generate new level if necessary
        newLevel = lvl_new(curLayer + 1, curLevel);
        gen_generate(newLevel);

        for(uint i = 0; i < 8; i++)
            lvl_set_tile(newLevel, x + dir_get_x(i), y + dir_get_y(i), tile_get(TILE_MUD));
    }

    lvl_set_tile(newLevel, x, y, tile_get(TILE_STAIR_UP));
   
    e = ent_change_level(e, newLevel);

    lvl_change_level(newLevel);
    lt_show(newLevel);

    e->player.on_stairs = 65;
    plr_set_swim(e, false);

	mnu_draw_hotbar(e);
    return true;
}


bool tile_stair_up_ontouch(ent_t *e, uint x, uint y)
{
    if(e->type != ENT_TYPE_PLAYER || e->player.on_stairs > 0)
        return false;

    mnu_load_level();

    level_t *curLevel = e->level;
    level_t *newLevel = world[curLevel->layer-1];
    plr_move_to(e, x, y);
    
    if(!curLevel->layer) // @todo safeguard against this. The window will block viewport
        return false;
    
    e = ent_change_level(e, newLevel);

    lvl_change_level(newLevel);

    if(!newLevel->layer) {
        lt_hide();
    }

	mnu_draw_hotbar(e);

    e->player.on_stairs = 65;
    plr_set_swim(e, false);

    return true;
}

