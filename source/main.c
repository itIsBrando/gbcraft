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

#include "level.h"
#include "tile.h"
#include "entity.h"
#include "terraingen.h"
#include "item.h"
#include "menu.h"


BG_REGULAR *main_background;


int main(void) {
	REG_DISPCNT = 1; //  mode 1: bg0=reg bg1=reg bg2=aff bg3=none

	// copy sprite data and palette
	memcpy16(SPRITE_PALETTE, (u16*)character_idlePal, character_idlePalLen >> 1);
	memcpy16(background_palette_mem, (u16*)tilesetPal, tilesetPalLen >> 1);

	// copy sprite to tile 1 in tileblock 4
	bg_load_tiles(4, 1, character_idleTiles, character_idleTilesLen, false);

	// copy over tileset
	bg_load_tiles(0, 1, tilesetTiles, tilesetTilesLen, true); // load 8bpp tiles
	bg_load_tiles(1, 1, tiles16Tiles, tiles16TilesLen, false); // load 4bpp tiles

	background_palette_mem[0] = RGB15(2, 2, 2);

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

	// generate our level
	lvl_set_target_background(main_background);
	level_t *level = lvl_new(0, NULL);

	win_move(&window, 0, 0, 240, 160);
	gen_generate(level);
	bg_move(main_background, 512, 512);
	lvl_blit(level);

	ent_t *plr = ent_add(level, ENT_TYPE_PLAYER, 120-8, 80-8);
	plr->player.max_health = plr->player.health = 20;
	plr->player.max_stamina = plr->player.stamina = 20;
	mnu_draw_hotbar(plr);

	ent_add(level, ENT_TYPE_ZOMBIE, 50, 50);

	obj_t *cursor = spr_alloc(0, 0, 5);
	u8 curTime = 0;

	item_add_to_inventory(&ITEM_WOOD, &plr->player.inventory);
	item_add_to_inventory(&ITEM_STONE, &plr->player.inventory);
	item_add_to_inventory(&ITEM_STONE, &plr->player.inventory);
	item_add_to_inventory(&ITEM_AXE, &plr->player.inventory);
	item_add_to_inventory(&ITEM_PICKAXE, &plr->player.inventory);
	item_add_to_inventory(&ITEM_BENCH, &plr->player.inventory);
	item_change_count(&plr->player.inventory.items[0], 5);
	item_change_count(&plr->player.inventory.items[1], 15);

	ent_player_set_active_item(plr, &plr->player.inventory.items[1]);

	while (true) {
		key_scan();
		u16 keys = key_pressed_no_repeat();

		if(keys & KEY_A)
		{
			u16 x = 124 + bg_get_scx(main_background),
				y = 84  + bg_get_scy(main_background);
			x>>=4, y>>=4;
			x += dir_get_x(plr->dir);
			y += dir_get_y(plr->dir);

			if(plr->player.activeItem)
			{
				item_event_t *e = plr->player.activeItem->event;
				if(e->interact)
					e->interact(
						plr->player.activeItem,
						(ent_t*)plr,
						lvl_get_tile(level, x, y),
						x, y
					);

				// check to see if we can interact with an entity
				u8 s;
				ent_t **ents = ent_get_all(plr->level, level->entities[0].x+4, level->entities[0].y+4, &s);
				for(u16 i = 0; i < s; i++)
				{
					ent_t *e = ents[i];
					if(e->events->onhurt)
						e->events->onhurt(e, plr);
				}

				free(ents);
				
			} else {
				// else, try to break the item
				const tile_event_t *events = lvl_get_tile(level, x, y)->event;
				if(events->interact)
					events->interact(plr, NULL, x, y);
			}

			// cursor @todo move this elsewhere
			x = (level->entities[0].x+4) + (dir_get_x(level->entities[0].dir) * 9);
			y = (level->entities[0].y+4) + (dir_get_y(level->entities[0].dir) * 11);
			spr_set_tile(cursor, plr->player.activeItem->tile);
			curTime = 10;
			spr_move(cursor, x, y);
			spr_show(cursor);
		}

		// show inventory
		if(keys & KEY_START)
		{
			mnu_show_inventory(plr);
		}

		for(u16 i = 0; i < level->ent_size; i++)
		{
			const void (*onupdate)(ent_t *) = level->entities[i].events->onupdate;
			if(onupdate)
				onupdate(&level->entities[i]);
		}


		if(curTime)
		{
			if(--curTime == 0)
			{
				spr_hide(cursor);
			}
		}
		
		VBlankIntrWait();
		spr_copy_all();
	}

	// free(level);
}
