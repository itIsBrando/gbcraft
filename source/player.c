#include "entity.h"
#include "tile.h"
#include "level.h"
#include "hotbar.h"
#include "menu.h"
#include "player.h"
#include "item.h"
#include "lighting.h"

#include "window.h"
#include "lib/keypad.h"
#include "lib/sound.h"
#include "bg.h"
#include "obj.h"
#include "text.h"
#include "random.h"
#include <stdlib.h>
#include <gba_systemcalls.h>

static void ent_move_all(level_t *lvl, const direction_t direction, const uint dist);

static obj_t *swim_sprite = NULL;
static obj_t *hit_sprite = NULL;
uint hit_timer = 0;


/**
 * Note the difference between this and `ent_player_init`. This function is called ONCE in main
 * `ent_player_init` is called for every new player entity
 */
void player_init() {
    // create swim sprite
    swim_sprite = spr_alloc(112, 92, 30);
    spr_set_size(swim_sprite, SPR_SIZE_16x8);
    spr_set_priority(swim_sprite, SPR_PRIORITY_HIGHEST);
    
    // create hit sprite
    hit_sprite = spr_alloc(0, 0, 69);
    spr_set_size(hit_sprite, SPR_SIZE_16x16);
}


void ent_player_init(ent_t *e)
{
    e->player.inventory.parent = e;
    e->player.removed = false;
    e->player.is_swimming = false;
    e->player.active_item = 0;
    
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

    if(plr->player.invulnerability) {
        plr->player.invulnerability--;
        if((plr->player.invulnerability & 4))
            spr_hide(plr->sprite);
        else
            spr_show(plr->sprite);

    }

    if(keys & KEY_LEFT)
    {
        plr_move_by(plr, DIRECTION_LEFT, 1);
    } else if(keys & KEY_RIGHT)
    {
        plr_move_by(plr, DIRECTION_RIGHT, 1);
    }

    if(keys & KEY_UP)
    {
        plr_move_by(plr, DIRECTION_UP, 1);
        spr_set_tile(plr->sprite, 5);
    } else if(keys & KEY_DOWN)
    {
        plr_move_by(plr, DIRECTION_DOWN, 1);
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
    
    if(hit_timer)
    {
        if(--hit_timer == 0)
            spr_hide(hit_sprite);
    }

    plr_apply_knockback(plr);
}


/**
 * Changes the player's swimming state
 * @param state true to set player to be on water, else player is on grass
 */
void plr_set_swim(ent_t *e, bool state)
{
    if(e->player.is_swimming == state)
        return;
    
    e->player.is_swimming = state;
    

    if(state) {
        spr_show(swim_sprite);
        spr_set_size(e->sprite, SPR_SIZE_16x8);
    } else if(swim_sprite) {
        spr_hide(swim_sprite);
        spr_set_size(e->sprite, SPR_SIZE_16x16);
    }
}


void ent_player_onrelocate(ent_t *e, level_t *lvl)
{
    inventory_t *inv = &e->player.inventory;

    inv->parent = e;

    for(uint i = 0; i < inv->size; i++) 
    {
        inv->items[i].parent = inv;
    }
    
    lvl->player = e;
}


static int _hotbar_index = 0;


/**
 * @param index index in the player's inventory. Can be unbounded
 */
void player_set_hotbar_pos(ent_t *p, int index)
{
    const inventory_t *inv = &p->player.inventory;
    if(!inv->size)
        return;
    
    if(index < 0)
        index = inv->size-1;
    else if(index >= inv->size)
        index = 0;
    
    _hotbar_index = index;
    
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


// this is the sprite that represents the player's active item
static obj_t *spr = NULL;

/**
 * Sets the player's active item logically and visually
 * @param item item that is part of its inventory, or can be null
 */
void ent_player_set_active_item(ent_t *plr, item_t *item)
{
    if(!spr)
        spr = spr_alloc(0, 160-8, 0);

    mnu_draw_item(item, 1, 2);
    item_set_icon(spr, item);

    if(!item) {
        _hotbar_index = 0;
        plr->player.active_item = 0;
        return;
    }

    item_t *invItems = plr->player.inventory.items;
    for(uint i = 0; i < plr->player.inventory.size; i++)
    {
        if(item == invItems) {
            plr->player.active_item = i+1;
            _hotbar_index = i;
            return;
        }
        invItems++;
    }

    text_error("NOT FOUND");
}


/**
 * @returns the player's active item, or NULL
 */
item_t *plr_get_active_item(const ent_t *plr)
{
    if(plr->player.active_item == 0)
        return NULL;
    
    return (item_t*)&plr->player.inventory.items[plr->player.active_item-1];
}


const direction_t _dirs[] = {DIRECTION_RIGHT, DIRECTION_LEFT, DIRECTION_DOWN, DIRECTION_UP,
    DIRECTION_RIGHT_DOWN, DIRECTION_LEFT_DOWN, DIRECTION_RIGHT_UP, DIRECTION_LEFT_UP
};

/** inverts the direction @todo move elsewhere */
inline direction_t dir_get_opposite(direction_t direction)
{
    return _dirs[direction];
}


/**
 * Sets the player's position. Does not check if it's a valid move.
 * @param x absolute tile x
 * @param y absolute tile y
 */
void plr_move_to(ent_t *plr, uint x, uint y)
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


/**
 * @param e player entity
 * @param dmg amount of damage to deal
 * @return true if the player dies from this attack
 */
bool plr_hurt(ent_t *mob, ent_t *e, int dmg)
{
    if(e->player.invulnerability)
        return false;
    
    e->player.health -= dmg;
    e->xKnockback = dir_get_x(mob->dir) * 5;
    e->yKnockback = dir_get_y(mob->dir) * 5;

    if(e->player.health <= 0)
    {
        e->player.health = 0;
        plr_kill(e);
    }

    e->player.invulnerability = 40;

    bar_draw_health(e);
    return false;
}


void plr_move_by(ent_t *player, const direction_t direction, const uint dist)
{
    if(ent_can_move(player, direction, dist))
    {
        ent_move_all(player->level, dir_get_opposite(direction), dist);
        int dx = dir_get_x(direction) * dist;
        int dy = dir_get_y(direction) * dist;
        bg_move(main_background, bg_get_scx(main_background) + dx, bg_get_scy(main_background) + dy);
    }
}


static void ent_move_all(level_t *lvl, const direction_t direction, const uint dist)
{
    const int dx = dir_get_x(direction) * dist;
    const int dy = dir_get_y(direction) * dist;
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
 * Held item
 * @param item can be NULL
 * @returns the amount of additional damage should be dealt based on the held tool
 */
int plr_get_attack_bonus(const item_t *item)
{
    if(!item || item->type != ITEM_TYPE_TOOL)
        return 0;
    else if(item->tooltype == TOOL_TYPE_AXE)
        return item->level << 1;
    else if(item->tooltype == TOOL_TYPE_SWORD)
        return item->level * 3;
    
    return 1;
}


/**
 * Shows a sprite when the player interacts with a tile
 * @param dir player's facing direction
 */
void plr_show_interaction(direction_t dir)
{
    uint x = lvl_to_tile_x(120) + dir_get_x(dir);
    uint y = lvl_to_tile_y(80)  + dir_get_y(dir);
    
    spr_show(hit_sprite);
    spr_move(hit_sprite, lvl_to_pixel_x(x), lvl_to_pixel_y(y));
    hit_timer = 10;
}


/**
 * Called when the user presses the `A` button
 */
void ent_player_interact(const ent_t *plr)
{
    // check to see if we can interact with an entity
    uint px = 120 + (dir_get_x(plr->dir) * 15),
         py = 80 + (dir_get_y(plr->dir) * 15);
    ent_t *ents[5];
    uint s = 0;
    
    item_t *active_item = plr_get_active_item(plr);
    if(!active_item || (active_item && active_item->tooltype != TOOL_TYPE_PICKUP))
        s = ent_get_all_stack(plr->level, plr, ents, px, py, 5);

    for(uint i = 0; i < s; i++)
    {
        ent_t *e = ents[i];

        if(e->events->onhurt) {
            e->events->onhurt(
                e,
                (ent_t*)plr,
                1 + (rnd_random() & 0x3) + plr_get_attack_bonus(active_item)
            );
            return;
        }
    }

    uint x = lvl_to_tile_x(120), y = lvl_to_tile_y(80);

    x += dir_get_x(plr->dir);
    y += dir_get_y(plr->dir);

    snd_play_square(&SOUND_EFFECT_PUNCH);
    
    const tile_t *t = lvl_get_tile(plr->level, x, y);
    // interact with held item
    if(active_item)
    { // interact with item
        const item_event_t *e = active_item->event;
        if(e->interact && e->interact(
                active_item,
                (ent_t*)plr,
                t,
                x, y
        )) {
            plr_show_interaction(plr->dir);
            return;            
        }

    }
    
    // interact with tile if we did not interact with item
    const tile_event_t *e = t->event;
    if(e->interact)
        e->interact((ent_t*)plr, NULL, x, y);
    
}

static inline uint min(uint a, uint b)
{
    return a > b ? b : a;
}


/**
 * Heals the player
 * @returns false if player is at max health
 */
bool plr_heal(ent_t *e, uint by)
{
    if(e->player.health < e->player.max_health)
    {
        e->player.health = min(e->player.max_health, e->player.health + by);
        mnu_draw_hotbar(e);
        return true;
    }

    return false;
}


/**
 * Player death animation
 * @todo finish
 */
void plr_kill(ent_t *e)
{
    e->player.dead = true;
    lt_show(e->level);

    for(uint i = 0; i < 32; i++)
    {
        blnd_set_weights(31, 31-i);
        VBlankIntrWait();
        VBlankIntrWait();
        VBlankIntrWait();
    }

    bg_fill(win_get_0()->background, 0, 0, 240/8, 3, 0);
    text_set_pal(1);
    text_print("YOU HAVE DIED.", 15-7, 0);
    text_set_pal(0);
    
    for(uint i = 0; i < 60 * 10; i++)
    {
        key_scan();
        VBlankIntrWait();
        if(key_pressed_no_repeat() != 0)
            break;
    }
    
    lt_hide();
}


/**
 * Hides the extraneous sprites related to the player
 * @todo implement
 */
void plr_hide()
{
    spr_hide(spr);
    spr_hide(swim_sprite);
}


