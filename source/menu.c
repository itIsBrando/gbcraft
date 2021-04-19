#include "bg.h"
#include "obj.h"
#include "text.h"
#include "window.h"
#include "keypad.h"
#include <gba_systemcalls.h>
#include <stdlib.h>

#include "menu.h"
#include "item.h"
#include "hotbar.h"
#include "entity.h"
#include "furniture.h"


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
        // clean up sprites
    for(u16 i = 0; i < size; i++)
    {
        spr_free(icons[i]);
    }

    free(icons);
}


/**
 * @param size number of items in the list
 * @returns a pointer to must be passed to `mnu_free_item_list`
 */
obj_t **mnu_draw_item_list(item_t *items, u8 size)
{
    obj_t **icons = malloc(sizeof(obj_t *) * size);

    // init all items in inventory
    for(u16 i = 0; i < size; i++)
    {
        text_print((char*)items[i].name, 2, 2 + i);

        if(items[i].type != ITEM_TYPE_TOOL)
            text_uint(items[i].count, 12, 2 + i);

        icons[i] = spr_alloc(16, 24 + (i << 3), items[i].tile);
    }

    return icons;
}


void mnu_show_inventory(ent_t *player)
{
    WIN_REGULAR *win = win_get_0();

    text_print("INVENTORY", 0, 0);
    bg_fill(win->background, 0, 1, 20, 10, 0);
    ent_hide_all(player->level);

    mnu_scroll_up();    

    inventory_t *inv = &player->player.inventory;
    u16 inv_size = inv->size;

    u16 key;

    obj_t **icons = mnu_draw_item_list(inv->items, inv_size);

    obj_t *cursor = spr_alloc(8, 24, 16);
    u8 curY = 0;

    while(true) {
        key_scan();
        key = key_pressed_no_repeat();

        if((key & KEY_UP) && curY > 0)
            curY--;
        else if((key & KEY_DOWN) && curY < inv_size-1)
            curY++;

        if(key == KEY_A)
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
    const u16 x = 16, y = 2;

    bg_fill(win_get_0()->background, x, y, 6, 4, 0); // clear background

    item_t *c = item_get_from_inventory_matching(recipe->result, &plr->player.inventory);
    text_print("YOU HAVE:", x, y + 5);
    text_uint(c ? c->count : 0, x + 1, y + 6);

    for(u16 i = 0; i < recipe->costs_num; i++)
    {
        const cost_t *cost = &recipe->costs[i];
        item_t *in_possession = item_get_from_inventory(cost->item_type, &plr->player.inventory);
        u8 count = in_possession ? in_possession->count : 0;

        spr_move(icons[i], 8 + (x << 3), 8 + ((y + i) << 3));
        spr_set_tile(icons[i], item_get_from_type(cost->item_type, -1)->tile);
        
        if(count < cost->required_amount)
            text_set_pal(1); // set text to red
        else
            text_set_pal(0); // otherwise set to blue
        text_uint(count, x + 1, y + i);
        text_uint(cost->required_amount, x+4, y + i);
        text_char('/', x + 3, y + i);
    }

    text_set_pal(0);
}

/**
 * Checks to see if an inventory can craft a recipe
 * @returns false if lacking the necessary resources
 */
bool item_can_craft(const recipe_t *r, const inventory_t *inv)
{
    for(u8 i = 0; i < r->costs_num; i++)
    {
        item_t *item = item_get_from_inventory(r->costs[i].item_type, inv);
        if(!item || item->count < r->costs[i].required_amount)
        {
            return false;
        }
    }

    return true;
}


void mnu_open_crafting(ent_t *plr)
{
    WIN_REGULAR *win = win_get_0();

    text_print("CRAFTING", 0, 0);
    bg_fill(win->background, 0, 1, 20, 10, 0);

    ent_hide_all(plr->level);

    mnu_scroll_up();

    // store all of the item icons in a list
    item_t items[CRAFTING_RECIPES_SIZE];
    for(u16 i = 0; i < CRAFTING_RECIPES_SIZE; i++)
    {
        items[i] = *CRAFTING_RECIPES[i].result;
    }

    obj_t **icons = mnu_draw_item_list(items, CRAFTING_RECIPES_SIZE);
    u16 key;

    obj_t **costIcons = malloc(4 * sizeof(obj_t *));
    for(u16 i = 0; i < 4; i++)
        costIcons[i] = spr_alloc(0, 0, 0);

    _crafting_draw_costs(&CRAFTING_RECIPES[0], costIcons, plr);

    // init cursor
    obj_t *cursor = spr_alloc(8, 24, 16);
    u8 curY = 0;

    do {
        key_scan();
        key = key_pressed_no_repeat();

        // handle cursor movement
        if((key & KEY_UP) && curY > 0) {
            curY--;
            _crafting_draw_costs(&CRAFTING_RECIPES[curY], costIcons, plr);
        } else if((key & KEY_DOWN) && curY < CRAFTING_RECIPES_SIZE-1) {
            curY++;
            _crafting_draw_costs(&CRAFTING_RECIPES[curY], costIcons, plr);
        }

        // check if we can craft
        if(key & KEY_A)
        {
            const recipe_t *r = &CRAFTING_RECIPES[curY];
            // if we can actually craft
            if(item_can_craft(r, &plr->player.inventory))
            {
                for(u8 i = 0; i < r->costs_num; i++)
                {
                    item_t *item = item_get_from_inventory(r->costs[i].item_type, &plr->player.inventory);
                    item_change_count(item, -r->costs[i].required_amount);
                }
                item_add_to_inventory(r->result, &plr->player.inventory);
                _crafting_draw_costs(&CRAFTING_RECIPES[curY], costIcons, plr);
            }
        }

        spr_move(cursor, 8, 24 + (curY << 3));
        VBlankIntrWait();
        spr_copy_all();
    } while(key != KEY_B);


    mnu_free_item_list(icons, plr->player.inventory.size);

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
 * Draws the player's hotbar
 */
void mnu_draw_hotbar(ent_t *player)
{
    bg_fill(win_get_0()->background, 0, 0, 20, 10, 0);
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
void mnu_draw_item(item_t *item, u16 x, u16 y)
{
    text_print(item ? (char*)item->name : "NONE", x, y);
    text_uint(item->count, x + 10, y);
}