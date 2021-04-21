#include "entity.h"
#include "level.h"

#include "item.h"
#include "bg.h"
#include "obj.h"


/**
 * @param x relative pixel x
 * @param y relative pixel y
 * @param item item_t that will be added to the player's inventory upon touching
 */
void ent_item_new(level_t *level, u16 x, u16 y, item_t *item)
{
    ent_t *e = ent_add(level, ENT_TYPE_PLAYER, x, y);
    e->itemEntity.item = item;
    e->itemEntity.frames_alive = 0;

    spr_set_size(e->sprite, SPR_SIZE_8x8);
    spr_set_tile(e->sprite, item->tile);
}


void ent_item_ontouch(ent_t *e, ent_t *other, u16 x, u16 y)
{
    if(other->type != ENT_TYPE_PLAYER)
        return;

    item_add_to_inventory(e->itemEntity.item, &lvl_get_player(e->level)->player.inventory);
    ent_remove(e->level, e);
}


void ent_item_update(ent_t *e)
{
    // destroy after 20 seconds
    if(e->itemEntity.frames_alive++ > 60*20)
    {
        ent_remove(e->level, e);
        return;
    }

    
}