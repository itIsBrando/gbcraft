#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sprites.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include "main.h"
#include "obj.h"
#include "bg.h"
#include "window.h"
#include "memory.h"
#include "keypad.h"

#include "tiles16.h"
#include "character_idle.h"
#include "tileset.h"
#include "text.h"

#include "save.h"
#include "level.h"
#include "tile.h"
#include "entity.h"
#include "terraingen.h"
#include "item.h"
#include "menu.h"

extern const char FLASH_ID_TEXT[];
extern void foo(const char *); // @todo remove the need for this
BG_REGULAR *main_background;

int main(void) {
	REG_DISPCNT = 1; //  mode 1: bg0=reg bg1=reg bg2=aff bg3=none

	// copy sprite data and palette
	memcpy16((u16*)sprite_palette_mem, (u16*)character_idlePal, character_idlePalLen >> 1);
	memcpy16((u16*)sprite_palette_mem + 16, (u16*)character_idlePal, character_idlePalLen >> 1);
	memcpy16((u16*)sprite_palette_mem + 32, (u16*)character_idlePal, character_idlePalLen >> 1);
	memcpy16((u16*)background_palette_mem, (u16*)tilesetPal, tilesetPalLen >> 1);

	// copy sprite to tile 1 in tileblock 4
	bg_load_tiles(4, 1, character_idleTiles, character_idleTilesLen, false);

	// copy over tileset
	bg_load_tiles(0, 1, tilesetTiles, tilesetTilesLen, true); // load 8bpp tiles
	bg_load_tiles(1, 1, tiles16Tiles, tiles16TilesLen, false); // load 4bpp tiles

	// set first tile to be non-transparent black
	memset16((u16*)&tile_mem[0][0], 0xCCCC, 32);
	background_palette_mem[0xC] = RGB15(4, 4, 4);

	background_palette_mem[0] = RGB15(2, 2, 2);
	background_palette_mem[16 + 7] = RGB15(255, 0, 0); // set color red
	background_palette_mem[16 + 4] = RGB15(6, 6, 6); // set bg color to darkish-black for pal 1
	sprite_palette_mem[16 + 12] = RGB15(0xff, 0xff, 0xff);
	sprite_palette_mem[16 + 2] = RGB15(0xd, 8, 07); // set color brown (for wood tools)
	sprite_palette_mem[32 + 2] = RGB15(29, 29, 28); // set color cream (for iron tools)

	BG_REGULAR bg;
	main_background = &bg;
	bg_affine_init(main_background, 10, 0, 2);
	bg_set_size(main_background, BG_SIZE_AFF_128x128);

	// create window
	BG_REGULAR bg_win;
	WIN_REGULAR window;
	bg_init(&bg_win, 30, 0, 1);
	win_init(&window, &bg_win, 0);
	text_init(&bg_win, 526); // initialize our text generator

	spr_init(); // initialize sprites

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	uint menuOption = mnu_open_main();

	// generate our level
	lvl_set_target_background(main_background);
	level_t *level = lvl_new(0, NULL);

	win_move(&window, 0, 0, 240, 160);
	bg_move(main_background, 512, 512);
	bg_set_priority(window.background, BG_PRIORITY_LOWEST);
	bg_set_priority(main_background, BG_PRIORITY_HIGH);

	ent_t *plr;

	if(menuOption == 0)
	{
		gen_generate_overworld(level);
		plr = ent_add(level, ENT_TYPE_PLAYER, 120-8, 80-8);

		ent_add(level, ENT_TYPE_SLIME, 50, 50);
		ent_add(level, ENT_TYPE_ZOMBIE, 150, 10);

		item_add_to_inventory(&ITEM_STONE, &plr->player.inventory);
		item_change_count(plr->player.inventory.items, 19);
		item_add_to_inventory(&ITEM_STONE_AXE, &plr->player.inventory);
		item_add_to_inventory(&ITEM_PICKUP, &plr->player.inventory);
		item_add_to_inventory(&ITEM_BENCH, &plr->player.inventory);
		item_add_to_inventory(&ITEM_CHEST, &plr->player.inventory);
	} else {
		sve_load_from_persistant(level);
		plr = lvl_get_player(level);
		ent_player_set_active_item(plr, NULL); // active item pointer probs decayed
	}

	lvl_blit(level);
	mnu_draw_hotbar(plr);
	foo(FLASH_ID_TEXT); // @todo remove this unnecessary function

	obj_t *cursor = spr_alloc(0, 0, 0);
	u8 curTime = 0;

	while (true) {
		key_scan();
		u16 keys = key_pressed_no_repeat();

		if((keys & KEY_A) && !curTime)
		{
			u16 x = 124 + bg_get_scx(main_background),
				y = 84  + bg_get_scy(main_background);
			x>>=4, y>>=4;
			x += dir_get_x(plr->dir);
			y += dir_get_y(plr->dir);


			if(plr->player.activeItem)
			{ // interact with item
				item_event_t *e = plr->player.activeItem->event;
				if(e->interact)
					e->interact(
						plr->player.activeItem,
						(ent_t*)plr,
						lvl_get_tile(level, x, y),
						x, y
					);
			} else { // interact with tile
				const tile_t *t = lvl_get_tile(level, x, y);
				const tile_event_t *e = t->event;
				if(e->interact)
					e->interact(plr, NULL, x, y);
			}

			// check to see if we can interact with an entity
	
			u8 px = plr->x + (dir_get_x(plr->dir) << 3) + 8,
			   py = plr->y + (dir_get_y(plr->dir) << 3) + 8;
			ent_t *ents[5];
			uint s = ent_get_all_stack(plr->level, ents, px, py, 5);

			for(uint i = 0; i < s; i++)
			{
				ent_t *e = ents[i];
				if(e == plr) continue;

				if(e->events->onhurt && e->events->onhurt(e, plr, 2))
					break;
			}

			// cursor @todo move this elsewhere
			x = (plr->x+4) + (dir_get_x(plr->dir) * 9);
			y = (plr->y+4) + (dir_get_y(plr->dir) * 11);
			item_set_icon(cursor, plr->player.activeItem);
			curTime = 10;
			spr_move(cursor, x, y);
			spr_show(cursor);
		}

		// show inventory
		if(keys & KEY_START)
		{
			mnu_show_inventory(plr);
		} else if(keys == KEY_SELECT)
		{
			sve_save_level(level);
		}

		for(int i = level->ent_size - 1; i >= 0; i--)
		{
			const void (*onupdate)(ent_t *) = level->entities[i].events->onupdate;
			if(onupdate)
				onupdate(&level->entities[i]);
		}

		lvl_ticks++;
		lvl_try_spawn(level, 1);

		if(curTime && --curTime == 0)
		{
			spr_hide(cursor);
		}
		
		VBlankIntrWait();
		spr_copy_all();
	}

	// free(level);
}
