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
#include "entity.h"


BG_REGULAR main_background;

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

	spr_init();
	bg_init(&main_background, 30, 0, 0);
	// initialize our text generator
	text_init(&main_background, 464);

	// enable interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	lvl_set_target_background(&main_background);
	level_t *level = lvl_new(0, NULL);

	lvl_blit(level);
	lvl_set_tile(level, 5, 5, tile_get(TILE_WOOD));
	lvl_set_tile(level, 5, 4, tile_get(TILE_WOOD));

	ent_add(level, ENT_TYPE_PLAYER, 120-8, 80-8);

	while (true) {
		key_scan();
		u16 keys = key_pressed();


		if(keys & KEY_A)
		{
			u16 x = bg_get_scx(&main_background) >> 4,
			 y = bg_get_scy(&main_background) >> 4;
			lvl_set_tile(level, x + 120/16, y + 80/16, tile_get(TILE_WOOD));
		}

		for(uint i = 0; i < level->ent_size; i++)
		{
			level->entities[i].events.onupdate(&level->entities[i]);
		}

		// spr_move(sprite, x, y);
		bg_move(&main_background, x, y);
		VBlankIntrWait();
	}


	// free(level);
}
