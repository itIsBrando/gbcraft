#include "entity.h"
#include "tile.h"
#include "level.h"
#include "hotbar.h"
#include "menu.h"
#include "player.h"

#include "window.h"
#include "keypad.h"
#include "bg.h"
#include "obj.h"
#include "text.h"
#include <stdlib.h>

static void ent_move_all(level_t *lvl, const direction_t direction);


void ent_player_init(ent_t *e)
{
    e->player.inventory.parent = e;
    e->player.removed = false;

    e->level->player = e;
	e->player.max_health = e->player.health = 20;
	e->player.max_stamina = e->player.stamina = 20;
}


/**
 * Called every frame
 */
void ent_player_update(ent_t *plr)
{
    uint keys = key_pressed();
    uint keys_no_repeat = key_pressed_no_repeat();
    const bool isMoving = keys & (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN);

    if(plr->player.invulnerability)
        plr->player.invulnerability--;

    if(keys & KEY_LEFT)
    {
        plr_move_by(plr, DIRECTION_LEFT);
    } else if(keys & KEY_RIGHT)
    {
        plr_move_by(plr, DIRECTION_RIGHT);
    }

    if(keys & KEY_UP)
    {
        plr_move_by(plr, DIRECTION_UP);
        spr_set_tile(plr->sprite, 5);
    } else if(keys & KEY_DOWN)
    {
        plr_move_by(plr, DIRECTION_DOWN);
        spr_set_tile(plr->sprite, 1);
    }

    // player gets removed when the player changes level. Is this janky???
    if(plr->player.removed)
    {
        ent_remove(plr->level, plr);
        return;
    }

    // 'hotbar'
    if(keys_no_repeat & KEY_L)
    {
        player_change_hotbar_pos(plr, -1);
    } 
    else if(keys_no_repeat & KEY_R)
    {
        player_change_hotbar_pos(plr, 1);
    }

    // staircase timing
    if(plr->player.on_stairs)
        plr->player.on_stairs--;

    if(plr->player.is_swimming) {
        if(plr->player.stamina && (lvl_ticks & 0x3F) == 0x3F) {
            plr->player.stamina--;
            bar_draw_stamina(plr);
        }
        // if(!plr->player.stamina)
        //     plr->events->onhurt(plr, NULL, 1);
    }
    else if(plr->player.stamina < plr->player.max_stamina)
    {
        if(plr->player.stamina_time++ >= 10)
        {
            plr->player.stamina++;
            plr->player.stamina_time = 0;
            bar_draw_stamina(plr);
        }
    }

    if(isMoving)
    {
        // if we are facing up or down
        if(plr->dir == DIRECTION_DOWN || plr->dir == DIRECTION_UP)
        {
            if(lvl_ticks & 0x08)
                spr_flip(plr->sprite, SPR_FLIP_HORIZONTAL);
            else
                spr_flip(plr->sprite, SPR_FLIP_NONE);
        } else {
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

}


void ent_player_onrelocate(ent_t *eOld, ent_t *eNew)
{
    inventory_t *inv = &eOld->player.inventory;
    inventory_t *newInv = &eNew->player.inventory;

    newInv->parent = eNew;
    
    ent_player_set_active_item(eNew, NULL);
    ent_player_set_active_item(eOld, NULL);

    for(uint i = 0; i < inv->size; i++) 
    {
        newInv->items[i] = inv->items[i];

        newInv->items[i].parent = newInv;
    }

    newInv->size = inv->size;
}


static uint _hotbar_index = 0;


/**
 * @param index index in the player's inventory. Can be unbounded
 */
void player_set_hotbar_pos(ent_t *p, uint index)
{
    const inventory_t *inv = &p->player.inventory;
    if(!inv->size)
        return;
    
    _hotbar_index = index % inv->size;
    
    bg_fill(win_get_0()->background, 1, 2, 11, 1, 0);
    ent_player_set_active_item(p, (item_t*)&inv->items[_hotbar_index]);
}


/**
 * @param a -1 or 1 for left or right
 */
inline void player_change_hotbar_pos(ent_t *p, int a)
{
    player_set_hotbar_pos(p, _hotbar_index+a);
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


// inverts the direction @todo move elsewhere
inline direction_t dir_get_opposite(direction_t direction)
{
    return direction ^ 1;
}


/**
 * Sets the player's position. Does not check if it's a valid move.
 * @param x absolute tile x
 * @param y absolute tile y
 */
inline void plr_move_to(ent_t *plr, uint x, uint y)
{
    // get distance we will move
    uint oldX = bg_get_scx(main_background);
    uint oldY = bg_get_scy(main_background);

    uint bgx = (x - 7) << 4, bgy = ((y - 5) << 4) + 8;
    int dx = (int)bgx - oldX;
    int dy = (int)bgy - oldY;
    ent_t *e = plr->level->entities;
    
    bg_move(main_background, bgx, bgy);

    for(uint i = 0; i < plr->level->ent_size; i++)
    {
        if(e->type == ENT_TYPE_PLAYER) {
            e++;
            continue;
        }

        e->x -= (s16)dx;
        e->y -= (s16)dy;
        ent_draw(e++);
    }
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


/**
 * Called when the user presses the `A` button
 */
void ent_player_interact(const ent_t *plr)
{
    // check to see if we can interact with an entity
    uint px = 120 + (dir_get_x(plr->dir) * 8),
         py = 80 + (dir_get_y(plr->dir) * 8);
    ent_t *ents[5];
    uint s = 0;
    
    if(!plr->player.activeItem || (plr->player.activeItem && plr->player.activeItem->tooltype != TOOL_TYPE_PICKUP))
        s = ent_get_all_stack(plr->level, ents, px, py, 5);

    for(uint i = 0; i < s; i++)
    {
        ent_t *e = ents[i];
        if(e == plr) continue;

        if(e->events->onhurt && e->events->onhurt(e, (ent_t*)plr, 2))
            break;
    }

    uint x = lvl_to_tile_x(124), y = lvl_to_tile_y(84);

    x += dir_get_x(plr->dir);
    y += dir_get_y(plr->dir);

    // interact with held item
    if(plr->player.activeItem)
    { // interact with item
        const item_event_t *e = plr->player.activeItem->event;
        if(e->interact)
            e->interact(
                plr->player.activeItem,
                (ent_t*)plr,
                lvl_get_tile(plr->level, x, y),
                x, y
            );
    } else {
        // interact with tile
        const tile_t *t = lvl_get_tile(plr->level, x, y);
        const tile_event_t *e = t->event;
        if(e->interact)
            e->interact((ent_t*)plr, NULL, x, y);
    }
}

static inline uint min(uint a, uint b)
{
    return a > b ? b : a;
}


/**
 * Heals the player
 * @returns false if player is at max health
 */
bool ent_player_heal(ent_t *e, uint by)
{
    if(e->player.health < e->player.max_health)
    {
        e->player.health = min(e->player.max_health, e->player.health + by);
        mnu_draw_hotbar(e);
        return true;
    }

    return false;
}