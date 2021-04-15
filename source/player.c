#include "entity.h"
#include "tile.h"
#include "level.h"

#include "keypad.h"
#include "bg.h"
#include "obj.h"
#include "text.h"

static void plr_move_by(ent_t *player, const direction_t direction);
static void ent_move_all(level_t *lvl, const direction_t direction);

/**
 * Called every frame
 */
void ent_player_update(ent_t *plr)
{
    u16 keys = key_pressed();

    if(keys & KEY_LEFT)
    {
        spr_flip(plr->sprite, SPR_FLIP_NONE);
        plr_move_by(plr, DIRECTION_LEFT);
    }
    
    if(keys & KEY_RIGHT)
    {
        spr_flip(plr->sprite, SPR_FLIP_HORIZONTAL);
        plr_move_by(plr, DIRECTION_RIGHT);
    }

    if(keys & KEY_UP)
    {
        plr_move_by(plr, DIRECTION_UP);
    }

    if(keys & KEY_DOWN)
    {
        plr_move_by(plr, DIRECTION_DOWN);
    }

}


static obj_t *spr = NULL;

void ent_player_set_active_item(ent_t *plr, item_t *item)
{
    if(!spr)
        spr = spr_alloc(0, 160-8, 0);
    
    plr->player.activeItem = item;
    text_print(item ? (char*)item->name : "NONE", 1, 2);
    spr_set_tile(spr, item ? item->tile : 0);
}


// inverts the direction
inline direction_t dir_get_opposite(direction_t direction)
{
    return direction ^ 1;
}


static void plr_move_by(ent_t *player, const direction_t direction)
{
    if(ent_can_move(player, direction))
    {
        ent_move_all(player->level, dir_get_opposite(direction));
        bg_move_by(main_background, direction);
    }
}


static void ent_move_all(level_t *lvl, const direction_t direction)
{
    const int dx = dir_get_x(direction);
    const int dy = dir_get_y(direction);

    for(u16 i = 0; i < lvl->ent_size; i++)
    {
        ent_t *ent = &lvl->entities[i];

        if(ent->type == ENT_TYPE_PLAYER)
            continue;
        
        ent->x += dx;
        ent->y += dy;

        // if(ent_is_on_screen(ent))
        //     spr_show(ent->sprite);
        // else
        //     spr_hide(ent->sprite);
    }
}