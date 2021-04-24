#include "entity.h"
#include "level.h"
#include "memory.h"

#include "item.h"
#include "bg.h"
#include "obj.h"


/**
 * @param x relative pixel x
 * @param y relative pixel y
 * @param item item_t that will be added to the player's inventory upon touching
 * @param cnt number of `item` to add to the inventory
 */
void ent_item_new(level_t *level, u16 x, u16 y, item_t *item, u8 cnt)
{
    ent_t *e = ent_add(level, ENT_TYPE_ITEM_ENTITY, x + 8, y + 8);
    e->itemEntity.item = item;
    e->itemEntity.count = cnt;
    e->itemEntity.frames_alive = 0;
    e->xKnockback = rnd_random_bounded(-1, 1) * 6;
    e->yKnockback = rnd_random_bounded(-1, 1) * 6;

    spr_set_size(e->sprite, SPR_SIZE_8x8);
    spr_set_tile(e->sprite, item->tile);
}


void ent_item_ontouch(ent_t *e, ent_t *other, u16 x, u16 y)
{
    if(other->type != ENT_TYPE_PLAYER || (e->xKnockback != 0 || e->yKnockback != 0))
        return;

    for(u16 i = 0; i < e->itemEntity.count; i++)
        item_add_to_inventory(e->itemEntity.item, &lvl_get_player(e->level)->player.inventory);
    ent_remove(e->level, e);
}


void ent_item_update(ent_t *e)
{
    // destroy after 20 seconds
    if(e->itemEntity.frames_alive++ > 60*15)
    {
        ent_remove(e->level, e);
        return;
    }

    // flash if we are about to disappear
    if(e->itemEntity.frames_alive > 60*10)
    {
        spr_set_pal(e->sprite, (e->itemEntity.frames_alive & 3) == 0);
    }

    ent_apply_knockback(e);

    ent_draw(e);
}