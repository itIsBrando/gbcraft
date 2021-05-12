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
#include "lib/keypad.h"
#include "random.h"
#include "lib/DMA.h"
#include "lib/mem.h"

#include "tiles16.h"
#include "character_idle.h"
#include "tileset.h"
#include "text.h"

#include "save.h"
#include "level.h"
#include "render.h"
#include "entity.h"
#include "terraingen.h"
#include "item.h"
#include "menu.h"
#include "lighting.h"


extern const char FLASH_ID_TEXT[];
extern void foo(const char *); // @todo remove the need for this
BG_REGULAR *main_background;

int main(void) {
	REG_DISPCNT = 1; //  mode 1: bg0=reg bg1=reg bg2=aff bg3=none

	// copy sprite data and palette
	memcpy16((u16*)sprite_palette_mem, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)sprite_palette_mem + 16, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)sprite_palette_mem + 32, (u16*)character_idlePal, character_idlePalLen);
	memcpy16((u16*)background_palette_mem, (u16*)tilesetPal, tilesetPalLen);

	// copy sprite to tile 1 in tileblock 4
	bg_load_tiles(4, 1, character_idleTiles, character_idleTilesLen, false);

	// copy over tileset
	bg_load_tiles(0, 1, tilesetTiles, tilesetTilesLen, true); // load 8bpp tiles
	bg_load_tiles(1, 1, tiles16Tiles, tiles16TilesLen, false); // load 4bpp tiles

	// set first tile to be non-transparent black
	memset16((u16*)&tile_mem[0][0], 0xCCCC, 64);
	// set tile 517 to be transparent black
	memset16((u16*)&tile_mem[0][517], 0x0000, 32); // this tile is 4bpp
	background_palette_mem[0xC] = RGB15(4, 4, 4);

	background_palette_mem[0] = RGB15(2, 2, 2);
	background_palette_mem[16 + 8] = RGB15(255, 0, 0); // set color red for text
	background_palette_mem[16 + 4] = RGB15(6, 6, 6); // set bg color to darkish-black for pal 1
	sprite_palette_mem[13] = RGB15(29, 31, 0);
	sprite_palette_mem[16 + 6] = RGB15(29, 31, 0); // yellow for golden apple
	sprite_palette_mem[16 + 12] = RGB15(0xff, 0xff, 0xff);
	sprite_palette_mem[16 + 3] = RGB15(29, 31, 0); // yellow for gold ore
	sprite_palette_mem[16 + 2] = RGB15(0xd, 8, 07); // set color brown (for wood tools)
	sprite_palette_mem[32 + 2] = RGB15(29, 29, 28); // set color cream (for iron tools)
	sprite_palette_mem[48 + 2] = RGB15(29, 31, 0); // set color yellow (for gold tools)
	sprite_palette_mem[16 + 8] = RGB15(0, 29, 3);

	BG_REGULAR bg;
reset:
	main_background = &bg;
	bg_affine_init(main_background, 12, 0, 2);
	bg_set_size(main_background, BG_SIZE_AFF_128x128);

	// create window
	BG_REGULAR bg_win;
	WIN_REGULAR window;
	bg_init(&bg_win, 30, 0, 1);
	win_init(&window, &bg_win, 0);
	text_init(&bg_win, 526); // initialize our text generator

	spr_init(); // initialize sprites
	lt_init();

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	uint menuOption = mnu_open_main();

	// empty all level_t pointers
	for(uint i = 0; i < 4; i++)
		world[i] = NULL;

	// generate our level
	lvl_set_target_background(main_background);
	level_t *level = lvl_new(0, NULL);

	win_move(&window, 0, 0, 240, 160);
	bg_set_priority(window.background, BG_PRIORITY_LOW);
	bg_set_priority(main_background, BG_PRIORITY_HIGHEST);

	ent_t *plr;

	if(menuOption == 0)
	{
		gen_generate(level);
		plr = ent_add(level, ENT_TYPE_PLAYER, 120-8, 80-8);

		item_add_to_inventory(&ITEM_WOOD, &plr->player.inventory);
		item_change_count(plr->player.inventory.items, 19);
		item_add_to_inventory(&ITEM_PICKUP, &plr->player.inventory);
		item_add_to_inventory(&ITEM_BENCH, &plr->player.inventory);
		item_add_to_inventory(&ITEM_WOOD_AXE, &plr->player.inventory);
		plr_move_to(plr, 32, 32);
	} else {
		if(!(level = sve_load_from_persistant(world)))
			goto reset;

		plr = lvl_get_player(level);
	}

	lvl_change_level(level);
	
	mnu_draw_hotbar(plr);
	foo(FLASH_ID_TEXT); // @todo remove this unnecessary function

	obj_t *cursor = spr_alloc(0, 0, 0);
	u8 curTime = 0;

	do {
		key_scan();
		uint keys = key_pressed_no_repeat();
		level_t *lvl = lvl_get_current();
		plr = lvl_get_player(lvl);

		if((keys & KEY_A) && !curTime)
		{
			ent_player_interact(plr);
			// cursor @todo move this elsewhere
			uint x = (plr->x+4) + (dir_get_x(plr->dir) * 9);
			uint y = (plr->y+4) + (dir_get_y(plr->dir) * 11);
			item_set_icon(cursor, plr_get_active_item(plr));
			curTime = 10;
			spr_move(cursor, x, y);
			spr_show(cursor);
		}

		// show inventory
		if(keys & KEY_START)
		{
			mnu_open_inventory(plr);
		}
		else if(keys == KEY_SELECT)
		{
			sve_save_game(world);
		}

		lvl_ticks++;
		lvl_try_spawn(lvl, 1);

		// random tick
		for(uint i = 0; i < 2; i++)
		{
			uint x = rnd_random() & 63;
			uint y = rnd_random() & 63;
			const tile_event_t *events = lvl_get_tile(lvl, x, y)->event;

			if(events->onrandomtick)
				events->onrandomtick(lvl, x, y);
		}

		ent_t *e = &lvl->entities[lvl->ent_size-1];
		for(int i = lvl->ent_size - 1; i >= 0; i--)
		{
			const void (*onupdate)(ent_t *) = e->events->onupdate;
			if(onupdate)
				onupdate(e);
			
			// @todo. is this necessary??
			// if we changed the current level, then these entities are no longer aactive
			if(lvl != lvl_get_current())
				break;
			e--;
		}

		if(curTime && --curTime == 0)
		{
			spr_hide(cursor);
		}
		
		spr_copy_all();
		// spr_copy_all_DMA();
		VBlankIntrWait();

	} while(!plr->player.dead);

	for(uint i = 0; i < 4; i++)
	{
		if(world[i])
			free(world[i]);
	}

	memset16((u16*)oam_mem, 0, 128 << 2);

	goto reset;
}
