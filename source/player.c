#include "entity.h"
#include "tile.h"
#include "level.h"
#include "hotbar.h"
#include "menu.h"
#include "player.h"

#include "keypad.h"
#include "bg.h"
#include "obj.h"
#include "text.h"

static void ent_move_all(level_t *lvl, const direction_t direction);


void ent_player_init(ent_t *e)
{
    e->player.inventory.parent = e;
    e->level->player = e;

	e->player.max_health = e->player.health = 20;
	e->player.max_stamina = e->player.stamina = 20;
}

/**
 * Called every frame
 */
void ent_player_update(ent_t *plr)
{
    u16 keys = key_pressed();
    const bool isMoving = keys & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);

    if(plr->player.invulnerability)
        plr->player.invulnerability--;

    if(keys & KEY_LEFT)
    {
        plr_move_by(plr, DIRECTION_LEFT);
    }
    
    if(keys & KEY_RIGHT)
    {
        plr_move_by(plr, DIRECTION_RIGHT);
    }

    if(keys & KEY_UP)
    {
        plr_move_by(plr, DIRECTION_UP);
        spr_set_tile(plr->sprite, 5);
    }

    if(keys & KEY_DOWN)
    {
        plr_move_by(plr, DIRECTION_DOWN);
        spr_set_tile(plr->sprite, 1);
    }

    if(plr->player.is_swimming) {
        if(plr->player.stamina && (lvl_ticks & 0x3F) == 0x3F) {
            plr->player.stamina--;
            bar_draw_stamina(plr);
        }
        // if(!plr->player.stamina)
        //     plr->events->onhurt(plr, NULL, 1);
    } else if(plr->player.stamina < plr->player.max_stamina)
    {
        if(plr->player.stamina_time++ >= 10)
        {
            plr->player.stamina++;
            plr->player.stamina_time = 0;
            bar_draw_stamina(plr);
        }

    }

    // if we are facing up or down
    if(isMoving && (plr->dir & (DIRECTION_DOWN | DIRECTION_UP)))
    {
        if(lvl_ticks & 0x08)
            spr_flip(plr->sprite, SPR_FLIP_HORIZONTAL);
        else
            spr_flip(plr->sprite, SPR_FLIP_NONE);
    } else if(isMoving) {
        // if we are facing left or right
        if(plr->dir == DIRECTION_LEFT)
            spr_flip(plr->sprite, SPR_FLIP_HORIZONTAL);
        else 
            spr_flip(plr->sprite, SPR_FLIP_NONE);
        
        if(lvl_ticks & 0x08)
            spr_set_tile(plr->sprite, 26);
        else
            spr_set_tile(plr->sprite, 22);
    }

}


/**
 * Redraws hotbar and subtracts from player's health
 * @returns true if player can pay the cost, otherwise false
 */
bool plr_pay_stamina(ent_t *plr, s8 amt)
{
    plr->player.stamina -= amt;

    if(plr->player.stamina < 0)
    {
        plr->player.stamina_time = plr->player.stamina = 0;
        return false;
    }

    bar_draw_stamina(plr);

    return true;
}


static obj_t *spr = NULL;

/**
 * @param item item that is part of its inventory, or can be null
 */
void ent_player_set_active_item(ent_t *plr, item_t *item)
{
    if(!spr)
        spr = spr_alloc(0, 160-8, 0);
    
    plr->player.activeItem = item;
    mnu_draw_item(item, 1, 2);
    spr_set_tile(spr, item ? item->tile : 0);
}


// inverts the direction
inline direction_t dir_get_opposite(direction_t direction)
{
    return direction ^ 1;
}


void plr_move_by(ent_t *player, const direction_t direction)
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
    ent_t *ent = lvl->entities;

    for(uint i = 0; i < lvl->ent_size; i++)
    {
        if(ent->type == ENT_TYPE_PLAYER) {
            ent++;
            continue;
        }
        
        ent->x += dx;
        ent->y += dy;

        ent_draw(ent++);
    }
}