#ifndef DEFINES_H
#define DEFINES_H

#include <gba_types.h>
#include <stdio.h>

/**
 * Refers to the size of the tilemap
 *
 */
typedef enum {
    BG_SIZE_REG_32x32=0,
    BG_SIZE_REG_64x32,
    BG_SIZE_REG_32x64,
    BG_SIZE_REG_64x64,

    BG_SIZE_AFF_16x16=0,
    BG_SIZE_AFF_32x32,
    BG_SIZE_AFF_64x64,
    BG_SIZE_AFF_128x128,
} bg_map_size_t;


typedef struct {
    u8 map_base;   // block referring to the base of the map
    u8 tile_base; // block referring to the tileset
    u8 map_number; // background 0-3
    u16 width_bits; // number of bits representing the width: tilewidth=1<<width_bits
    vu16 *BG_CNT;
    union {
        vu16 *reg; // private members (for regular bgs)
        vs32 *aff; // private members (for affine bgs)
    } x;
    union {
        vu16 *reg; // private members (for regular bgs)
        vs32 *aff; // private members (for affine bgs)
    } y;
    u16 bgx, bgy;   // because BG scroll registers are write only
    bool is_affine; // determines whether or not this struct is affine or regular
} BG_REGULAR;

typedef struct {
    BG_REGULAR *background;
    bool window_number;
    u16 x, y;
    u16 w, h;
} WIN_REGULAR;

typedef u32 tile_4bpp[8];
typedef tile_4bpp tile_block[512];
typedef u16 screen_block[1024]; // size of a VRAM block

#define oam_mem ((volatile OBJATTR *)OAM)
#define tile_mem ((volatile tile_block*)VRAM)
#define map_mem ((volatile screen_block*)(VRAM))
#define sprite_palette_mem ((vu16 *)(BG_PALETTE + 0x200))
#define background_palette_mem ((vu16 *)(BG_PALETTE))


#endif