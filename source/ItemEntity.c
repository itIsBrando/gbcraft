#include "entity.h"
#include "level.h"
#include "random.h"

#include "item.h"
#include "bg.h"
#include "obj.h"


/**
 * @param x relative pixel x
 * @param y relative pixel y
 * @param item item_t that will be added to the player's inventory upon touching
 * @param cnt number of `item` to add to the inventory
 */
void ent_item_new(level_t *level, u16 x, u16 y, const item_t *item, u8 cnt)
{
    ent_t *e = ent_add(level, ENT_TYPE_ITEM_ENTITY, x + 8, y + 8);
    e->itemEntity.item = (item_t *)item;
    e->itemEntity.count = cnt;
    e->itemEntity.frames_alive = 0;
    e->xKnockback = rnd_random_bounded(-1, 1) * 6;
    e->yKnockback = rnd_random_bounded(-1, 1) * 6;

    spr_set_pal(e->sprite, item->palette);
    spr_set_size(e->sprite, SPR_SIZE_8x8);
    spr_set_tile(e->sprite, item->tile);
}


uint ent_item_get_tile(const ent_t *e)
{
    return e->itemEntity.item->tile;
}

bool ent_item_ontouch(ent_t *e, ent_t *other, u16 x, u16 y)
{
    if(other->type != ENT_TYPE_PLAYER || (e->xKnockback | e->yKnockback))
        return false;

    for(uint i = 0; i < e->itemEntity.count; i++)
        item_add_to_inventory(e->itemEntity.item, &lvl_get_player(e->level)->player.inventory);
    ent_remove(e->level, e);

    return true;
}


void ent_item_update(ent_t *e)
{
    // destroy after 20 seconds
    if(e->itemEntity.frames_alive++ > 60*15)
    {
        ent_remove(e->level, e);
        return;
    }
    
    ent_apply_knockback(e);
    ent_draw(e);

    // flash if we are about to disappear
    if(e->itemEntity.frames_alive > 60*10)
    {
        if((e->itemEntity.frames_alive & 4))
            spr_hide(e->sprite);
        else
            spr_show(e->sprite);
    }


}