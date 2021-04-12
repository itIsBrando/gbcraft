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
#include "memory.h"
#include "keypad.h"
#include "character_idle.h"
#include "tileset.h"
#include "text.h"

#include "level.h"
#include "tile.h"

//---------------------------------------------------------------------------------
// Program entry point
//---------------------------------------------------------------------------------
int main(void) {
	uint8_t x = 0;
	uint8_t y = 0;

	REG_DISPCNT = 0;

	// copy sprite data and palette
	memcpy16(SPRITE_PALETTE, (u16*)character_idlePal, character_idlePalLen >> 1);
	memcpy16(background_palette_mem, (u16*)tilesetPal, tilesetPalLen >> 1);

	// copy sprite to tile 1 in tileblock 4
	bg_load_tiles(4, 1, character_idleTiles, character_idleTilesLen);

	// copy over tileset
	bg_load_tiles(0, 1, tilesetTiles, tilesetTilesLen);

	background_palette_mem[0x0] = RGB15(2, 2, 2);
	
	BG_REGULAR bg;

	spr_init();
	bg_init(&bg, 30, 0, 0);
	// initialize our text generator
	text_init(&bg, 464);

	text_print("TEST", 5, 5);

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	obj_t *sprite = spr_alloc(120-8, 80-8, 1);

	spr_set_size(sprite, SPR_SIZE_16x16);

	lvl_set_target_background(&bg);
	level_t *level = lvl_new(0, NULL);

	lvl_blit(level);
	lvl_set_tile(level, 5, 5, tile_get(TILE_WOOD));
	lvl_set_tile(level, 5, 4, tile_get(TILE_WOOD));

	while (true) {
		key_scan();
		const u16 keys = key_pressed();

		if(keys & KEY_LEFT)
		{
			x--;
			spr_flip(sprite, SPR_FLIP_NONE);
		}
		
		if(keys & KEY_RIGHT)
		{
			x++;
			spr_flip(sprite, SPR_FLIP_HORIZONTAL);
		}

		if((keys & KEY_UP) && y > 0)
		{
			y--;
		}

		if(keys & KEY_DOWN)
		{
			y++;
		}

		if(keys & KEY_A)
		{
			u16 x = bg_get_scx(&bg) >> 4, y = bg_get_scy(&bg) >> 4;
			lvl_set_tile(level, x + 120/16, y + 80/16, tile_get(TILE_WOOD));
		}

		// spr_move(sprite, x, y);
		bg_move(&bg, x, y);
		spr_copy(sprite, 0);
		VBlankIntrWait();
	}


	// free(level);
}
