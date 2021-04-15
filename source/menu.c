#include "bg.h"
#include "obj.h"
#include "text.h"
#include "window.h"
#include "keypad.h"
#include <gba_systemcalls.h>
#include <stdlib.h>

#include "menu.h"
#include "entity.h"



void mnu_show_inventory(ent_t *player)
{
    WIN_REGULAR *win = win_get_0();

    win_move(win, 8, 160-24, 240-16, 16);

    text_print("INVENTORY", 0, 0);
    bg_fill(win->background, 0, 1, 20, 10, 0);

    for(uint i = 0; i < (160-32) >> 2; i++)
    {
        win_move_by(win, DIRECTION_UP);
        win_move_by(win, DIRECTION_UP);
        win_move_by(win, DIRECTION_UP);
    
        win_move_by(win, DIRECTION_UP);
        win_set_size(win, win->w, win->h + 4);
		VBlankIntrWait();
    }

    inventory_t *inv = &player->player.inventory;
    u16 inv_size = inv->size;
    obj_t **icons = malloc(sizeof(obj_t *) * inv_size);

    // init all items in inventory
    for(u16 i = 0; i < inv_size; i++)
    {
        text_print((char*)inv->items[i].name, 2, 2 + i);
        if(inv->items[i].type != ITEM_TYPE_TOOL)
            text_uint(inv->items[i].count, 12, 2 + i);

        icons[i] = spr_alloc(16, 24 + (i << 3), inv->items[i].tile);
    }

    ent_hide_all(player->level);

    u16 key;

    obj_t *cursor = spr_alloc(8, 24, 16);
    u8 curY = 0;

    do {
        key_scan();
        key = key_pressed_no_repeat();

        if((key & KEY_UP) && curY > 0)
            curY--;
        else if((key & KEY_DOWN) && curY < inv_size)
            curY++;

        spr_move(cursor, 8, 24 + (curY << 3));
        VBlankIntrWait();
        spr_copy_all();
    } while(key != KEY_B);

    // clean up sprites
    for(u16 i = 0; i < inv_size; i++)
    {
        spr_free(icons[i]);
    }

    free(icons);
    spr_free(cursor);
    spr_copy_all(); // update the positions of the hidden sprites


    for(uint i = 0; i < (160-32) >> 2; i++)
    {
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_move_by(win, DIRECTION_DOWN);
        win_set_size(win, win->w, win->h - 4);
        VBlankIntrWait();
    }

    ent_show_all(player->level);

    ent_player_set_active_item(player, &inv->items[curY]);

    mnu_draw_hotbar(player);
}


/**
 * Draws the player's hotbar
 */
void mnu_draw_hotbar(ent_t *player)
{
    bg_fill(win_get_0()->background, 0, 0, 20, 10, 0);
    win_move(win_get_0(), 0, 160-24, 240, 24);
	text_print("HOTBAR   ", 0, 0);

    for(uint i = 0; i < player->player.max_health >> 1; i++)
    {
        bg_write_tile(win_get_0()->background, i, 1,
            (i <= (player->player.health >> 1)) ? 513 : 514
        );
    }

    mnu_draw_item(player->player.activeItem, 1, 2);
}


void mnu_draw_item(item_t *item, u16 x, u16 y)
{
    text_print(item ? (char*)item->name : "NONE", x, y);
    text_uint(item->count, x + 10, y);
}