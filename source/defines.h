#ifndef DEFINES_H
#define DEFINES_H

#include <gba_types.h>
#include <stdio.h>



typedef struct {
    uint8_t map_base;   // block referring to the base of the map
    uint8_t tile_base; // block referring to the tileset
    uint8_t map_number; // background 0-3
    uint width;
    vu16 *BG_CNT;
    vu16 *BG_Y;     // private members
    vu16 *BG_X;     // private members
    u16 bgx, bgy;   // cause BG scroll registers are write only
} BG_REGULAR;



typedef u32 tile_4bpp[8];
typedef tile_4bpp tile_block[512];
typedef u16 screen_block[1024]; // size of a VRAM block

#define oam_mem ((volatile OBJATTR *)OAM)
#define tile_mem ((volatile tile_block*)VRAM)
#define map_mem ((volatile screen_block*)(VRAM))
#define sprite_palette_mem ((vu16 *)(BG_PALETTE + 0x200))
#define background_palette_mem ((vu16 *)(BG_PALETTE))


#endif