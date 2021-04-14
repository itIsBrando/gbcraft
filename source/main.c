#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_sprites.h>

#include <string.h>
#include <stdio.h>

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

	BG_REGULAR bg;
	main_background = &bg;
	bg_affine_init(main_background, 10, 0, 2);
	bg_set_size(main_background, BG_SIZE_AFF_128x128);

	// create window
	BG_REGULAR bg_win;
	WIN_REGULAR window;
	bg_init(&bg_win, 30, 0, 1);
	win_init(&window, &bg_win, 0);
	win_move(&window, 0, 160-24, 240, 24);
	text_init(&bg_win, 526); // initialize our text generator
	text_print("HOTBAR", 0, 0);

	spr_init(); // initialize sprites

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	// generate our level
	lvl_set_target_background(main_background);
	level_t *level = lvl_new(0, NULL);

	gen_generate(level);
	lvl_blit(level);
	bg_move(main_background, 30, 30);

	ent_t *plr = ent_add(level, ENT_TYPE_PLAYER, 120-8, 80-8);
	ent_add(level, ENT_TYPE_ZOMBIE, 50, 50);

	obj_t *cursor = spr_alloc(0, 0, 5);
	spr_set_size(cursor, SPR_SIZE_16x16);

	item_add_to_inventory(&ITEM_WOOD, &plr->player.inventory);
	ent_player_set_active_item(plr, &plr->player.inventory.items[0]);

	while (true) {
		key_scan();
		u16 keys = key_pressed_no_repeat();

		if(keys & KEY_B)
		{
			spr_show(level->entities[1].sprite);
		}
		if(keys & KEY_A)
		{
			const ent_t *plr = &level->entities[0];
			u16 x = 120 + bg_get_scx(main_background),
				y = 80  + bg_get_scy(main_background);
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
			} else {
				// else, try to break the item
				void (*interact)(ent_t *, item_t *item, u16 x, u16 y) = lvl_get_tile(level, x, y)->interact;
				if(interact)
					interact(plr, NULL, x, y);
			}
		}

		for(u16 i = 0; i < level->ent_size; i++)
		{
			const void (*onupdate)(ent_t *) = level->entities[i].events->onupdate;
			if(onupdate)
				onupdate(&level->entities[i]);
		}

		text_int(bg_get_scx(main_background), 0, 1);
		// cursor @todo move this elsewhere
		u16 x = ((level->entities[0].x+8) >> 4) + dir_get_x(level->entities[0].dir);
		u16 y = ((level->entities[0].y+8) >> 4) + dir_get_y(level->entities[0].dir);
		x <<= 4, y <<= 4;
		spr_move(cursor, x - (bg_get_scx(main_background) & 0xf), y - (bg_get_scy(main_background) & 0xf));
		
		VBlankIntrWait();
		spr_copy_all();
	}

	// free(level);
}
