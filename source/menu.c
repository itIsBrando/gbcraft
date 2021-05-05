#include "bg.h"
#include "obj.h"
#include "text.h"
#include "window.h"
#include "keypad.h"
#include <gba_systemcalls.h>
#include "memory.h"
#include <stdlib.h>

#include "menu.h"
#include "item.h"
#include "hotbar.h"
#include "entity.h"


#define EMPTY_TILE_INDEX 0

static void mnu_scroll_up()
{
    WIN_REGULAR *win = win_get_0();

    win_move(win, 8, 160-24, 240-16, 16);

    for(uint i = 0; i < (160-32) >> 2; i++)
    {
        win_move_by(win, DIRECTION_UP);
        win_move_by(win, DIRECTION_UP);
        win_move_by(win, DIRECTION_UP);
        win_move_by(win, DIRECTION_UP);
        win_set_size(win, win->w, win->h + 4);
        if(i & 1)
    		VBlankIntrWait();
    }
}

static void mnu_scroll_down()
{
    WIN_REGULAR *win = win_get_0();

    for(uint i = 0; i < (160-32) >> 2; i++)
    {
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_set_size(win, win->w, win->h - 4);
        if(i & 1)
            VBlankIntrWait();
    }
}


/**
 * Destroys all of the item icon sprites from `mnu_draw_item_list`
 * @param icons
 */
void mnu_free_item_list(obj_t **icons, u8 size)
{
    if(!size)
        return;
    
    // clean up sprites
    for(u16 i = 0; i < size; i++) {
        spr_free(icons[i]);
    }

    free(icons);
}


/**
 * @param size number of items in the list
 * @param tx tile x to start drawing list (2)
 * @param ty tile y to start drawing list (2)
 * @returns a pointer to must be passed to `mnu_free_item_list`
 */
obj_t **mnu_draw_item_list(item_t *items, u8 size, uint tx, uint ty)
{
    // do nothing if there are no items
    if(!size)
        return NULL;

    obj_t **icons = malloc(sizeof(obj_t *) * size);

    // init all items in inventory
    for(uint i = 0; i < size; i++)
    {
        text_print((char*)items[i].name, tx, ty + i);

        if(items[i].type != ITEM_TYPE_TOOL)
            text_uint(items[i].count, tx + 10, 2 + i);

        icons[i] = item_new_icon(&items[i], (tx << 3), 8 + ((ty + i) << 3));
    }

    return icons;
}


void mnu_open_inventory(ent_t *player)
{
    WIN_REGULAR *win = win_get_0();

    text_print("INVENTORY", 0, 0);
    bg_fill(win->background, 0, 1, 28, 18, EMPTY_TILE_INDEX);
    ent_hide_all(player->level);

    mnu_scroll_up();    

    inventory_t *inv = &player->player.inventory;
    uint inv_size = inv->size;

    uint key;

    obj_t **icons = mnu_draw_item_list(inv->items, inv_size, 2, 2);

    obj_t *cursor = spr_alloc(8, 24, 16);
    uint curY = 0;

    while(true) {
        key_scan();
        key = key_pressed_repeat_after(8);

        if((key & KEY_UP) && curY > 0)
            curY--;
        else if((key & KEY_DOWN) && curY < inv_size-1)
            curY++;

        if(key == KEY_A && inv_size)
        {
            ent_player_set_active_item(player, &inv->items[curY]);
            break;
        } else if(key == KEY_B) {
            ent_player_set_active_item(player, NULL);
            break;
        }

        spr_move(cursor, 8, 24 + (curY << 3));
        VBlankIntrWait();
        spr_copy_all();
    }

    mnu_free_item_list(icons, inv_size);
    spr_free(cursor);
    spr_copy_all(); // update the positions of the hidden sprites

    mnu_scroll_down();

    ent_show_all(player->level);

    mnu_draw_hotbar(player);
}



static void _crafting_draw_costs(const recipe_t *recipe, obj_t **icons, ent_t *plr)
{
    const uint x = 16, y = 2;

    bg_fill(win_get_0()->background, x, y, 6, 4, EMPTY_TILE_INDEX); // clear background

    item_t *c = item_get_from_inventory_matching(recipe->result, &plr->player.inventory);
    text_print("YOU HAVE:", x, y + 5);
    text_uint(c ? (recipe->result->type == ITEM_TYPE_TOOL ? 1 : c->count) : 0, x + 1, y + 6);

    for(uint i = 0; i < recipe->costs_num; i++)
    {
        const cost_t *cost = &recipe->costs[i];
        item_t *in_possession = item_get_from_inventory(cost->item_type, &plr->player.inventory);
        u8 count = in_possession ? in_possession->count : 0;

        spr_move(icons[i], 8 + (x << 3), 8 + ((y + i) << 3));
        item_set_icon(icons[i], item_get_from_type(cost->item_type, -1));
        
        if(count < cost->required_amount)
            text_set_pal(1); // set text to red
        else
            text_set_pal(0); // otherwise set to blue
            
        text_uint(count, x + 1, y + i);
        text_uint(cost->required_amount, x+4, y + i);
        text_char('/', x + 3, y + i);
    }

    for(uint i = recipe->costs_num; i < 4; i++)
    {
        spr_hide(icons[i]);
    }

    text_set_pal(0);
}


/**
 * Checks to see if an inventory can craft a recipe
 * @returns false if lacking the necessary resources
 */
bool item_can_craft(const recipe_t *r, const inventory_t *inv)
{
    for(uint i = 0; i < r->costs_num; i++)
    {
        item_t *item = item_get_from_inventory(r->costs[i].item_type, inv);
        if(!item || item->count < r->costs[i].required_amount)
        {
            return false;
        }
    }

    return true;
}


void mnu_open_crafting(ent_t *plr, const recipe_t *recipes, const uint recipe_size)
{
    WIN_REGULAR *win = win_get_0();

    ent_player_set_active_item(plr, NULL);
    text_print("CRAFTING", 0, 0);
    bg_fill(win->background, 0, 1, 28, 18, EMPTY_TILE_INDEX);

    ent_hide_all(plr->level);

    mnu_scroll_up();

    // store all of the item icons in a list
    item_t items[recipe_size];
    for(uint i = 0; i < recipe_size; i++)
    {
        items[i] = *recipes[i].result;
        if(recipes[i].amount)
            items[i].count = recipes[i].amount;
    }

    obj_t **icons = mnu_draw_item_list(items, recipe_size, 2, 2);
    u16 key;

    obj_t **costIcons = malloc(4 * sizeof(obj_t *));
    for(uint i = 0; i < 4; i++)
        costIcons[i] = spr_alloc(0, 0, 0);

    _crafting_draw_costs(recipes, costIcons, plr);

    // init cursor
    obj_t *cursor = spr_alloc(8, 24, 16);
    uint curY = 0;

    do {
        key_scan();
        key = key_pressed_repeat_after(8);

        // handle cursor movement
        if((key & KEY_UP) && curY > 0) {
            curY--;
            _crafting_draw_costs(&recipes[curY], costIcons, plr);
        } else if((key & KEY_DOWN) && curY < recipe_size-1) {
            curY++;
            _crafting_draw_costs(&recipes[curY], costIcons, plr);
        }

        // check if we can craft
        if(key_pressed_no_repeat() & KEY_A)
        {
            const recipe_t *r = &recipes[curY];
            // if we can actually craft
            if(item_can_craft(r, &plr->player.inventory))
            {
                // remove costs from inventory
                for(uint i = 0; i < r->costs_num; i++)
                {
                    item_t *item = item_get_from_inventory(r->costs[i].item_type, &plr->player.inventory);
                    item_change_count(item, -r->costs[i].required_amount);
                }

                item_change_count(
                    item_add_to_inventory(r->result, &plr->player.inventory),
                    r->amount
                );
                _crafting_draw_costs(&recipes[curY], costIcons, plr);
            }
        }

        spr_move(cursor, 8, 24 + (curY << 3));
        VBlankIntrWait();
        spr_copy_all();
    } while(key != KEY_B);

    mnu_free_item_list(icons, recipe_size);

    // clear cost sprite icons
    for(u16 i = 0; i < 4; i++)
        spr_free(costIcons[i]);

    spr_free(cursor);

    free(costIcons);
    spr_copy_all();

    mnu_scroll_down();
    ent_show_all(plr->level);
    mnu_draw_hotbar(plr);
}


/**
 * Opens the menu for a chest
 * @param e chest entity
 * @param plr player entity who opened chest
 */
void mnu_open_chest(ent_t *e, ent_t *plr)
{
    WIN_REGULAR *win = win_get_0();

    ent_player_set_active_item(plr, NULL);
    bg_fill(win->background, 0, 0, 28, 10, EMPTY_TILE_INDEX);
    text_print("INVENTORY", 0, 0);
    text_print("CHEST", 23, 0);

    ent_hide_all(plr->level);

    mnu_scroll_up();

    inventory_t *plr_inv = &plr->player.inventory;
    inventory_t *chst_inv= &e->furniture.inventory;
    inventory_t *invs[] = {plr_inv, chst_inv};

    // store all of the item icons in a list
    obj_t **plr_icons = mnu_draw_item_list(plr_inv->items, plr_inv->size, 2, 2);
    obj_t **chest_icons = mnu_draw_item_list(chst_inv->items, chst_inv->size, 16, 2);
    u16 key;

    uint xCur = 0, yCur = 0;
    obj_t *cursor = spr_alloc(8, 24, 16);

    do {
        key_scan();
        key = key_pressed_repeat_after(8);

        if((key & KEY_UP) && yCur > 0)
            yCur--;
        else if((key & KEY_DOWN) && invs[xCur]->size && yCur < invs[xCur]->size - 1)
            yCur++;
        
        if(key & KEY_LEFT)
            yCur = xCur = 0;
        else if(key & KEY_RIGHT)
            yCur = 0, xCur = 1;

        // switch inventory stuff
        if(key & KEY_A)
        {
            inventory_t *curInv = invs[xCur];
            inventory_t *otherInv = invs[!xCur];
            const item_t *item = &curInv->items[yCur];

            if(curInv->size) {
                mnu_free_item_list(plr_icons, plr_inv->size);
                mnu_free_item_list(chest_icons, chst_inv->size);
                
                item_add_to_inventory(item, otherInv);
                item_remove_from_inventory((item_t *)item);

                bg_fill(win->background, 0, 1, 30, 10, EMPTY_TILE_INDEX);
                plr_icons = mnu_draw_item_list(plr_inv->items, plr_inv->size, 2, 2);
                chest_icons = mnu_draw_item_list(chst_inv->items, chst_inv->size, 16, 2);

                // prevent overflow
                if(yCur == curInv->size && yCur)
                    yCur--;
            }
        }

        spr_move(cursor, 8 + (xCur * 14 * 8), 24 + (yCur << 3));

        VBlankIntrWait();
        spr_copy_all();
    } while(key != KEY_B);

    mnu_free_item_list(plr_icons, plr_inv->size);
    mnu_free_item_list(chest_icons, chst_inv->size);
    spr_free(cursor);
    spr_copy_all();

    mnu_scroll_down();
    ent_show_all(plr->level);
    mnu_draw_hotbar(plr);
}



/**
 * Draws the player's hotbar
 */
void mnu_draw_hotbar(ent_t *player)
{
    bg_fill(win_get_0()->background, 0, 0, 20, 10, EMPTY_TILE_INDEX);
    win_move(win_get_0(), 0, 160-24, 240, 24);
	text_print("HOTBAR   ", 0, 0);

    bar_draw_health(player);
    bar_draw_stamina(player);
    mnu_draw_item(player->player.activeItem, 1, 2);
}


/**
 * Draws the name and count of an item
 * @param x tile x
 * @param y tile y
 */
void mnu_draw_item(item_t *item, uint x, uint y)
{
    text_print(item ? (char*)item->name : "NONE    ", x, y);
    if(item && item->type != ITEM_TYPE_TOOL)
        text_uint(item->count, x + 10, y);
}


/**
 * Shows the main menu
 * @returns cursor position
 */
uint mnu_open_main()
{
    WIN_REGULAR *win = win_get_0();

    bg_fill(win->background, 0, 0, 240/8, 160/8, EMPTY_TILE_INDEX);
    text_print("GBACRAFT", 0, 0);
    text_print("PLAY", 2, 4);
    text_print("LOAD", 2, 5);
    win_move(win, 0, 0, 240, 160);

    uint key;

    *TME0CNT = 128; // enable timer 0

    obj_t *cur = spr_alloc(8, 32, 16);

    uint yCur = 0;

    do {
        key_scan();
        key = key_pressed_no_repeat();

        if((key & KEY_DOWN) && yCur < 2)
            yCur++;
        else if((key & KEY_UP) && yCur)
            yCur--;

        spr_move(cur, 8, 32 + (yCur << 3));

        VBlankIntrWait();
        spr_copy(cur, 0);
    } while(key != KEY_A);

    spr_free(cur);
    // set RNG seed
    rnd_seed(*TME0DATA);

    return yCur;
}



void mnu_load_level()
{
    WIN_REGULAR *w = win_get_0();
    bg_fill(w->background, 0, 0, 240/16, 10, 0);
    win_move(w, 0, 0, 240, 160);
    text_print("LOADING WORLD...", 0, 0);
}