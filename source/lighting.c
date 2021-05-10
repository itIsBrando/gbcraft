#include "bg.h"
#include "obj.h"

#include "lighting.h"
#include "level.h"

static BG_REGULAR lt_bg;
static obj_t *lt_player_mask = NULL;

/**
 * Should be called only once and before entering a cave.
 */
void lt_init()
{
    bg_init(&lt_bg, 10, 0, 0);
    lt_hide();
    bg_set_priority(&lt_bg, BG_PRIORITY_LOWEST);
    bg_set_size(&lt_bg, BG_SIZE_REG_32x32);
    bg_fill(&lt_bg, 0, 0, 30, 20-3, 0);

    u16 corners[] = {518, 517, 517, TILE_FLIP_HORIZONTAL(518),
        517, 517, 517, 517,
        517, 517, 517, 517,
        TILE_FLIP_VERTICAL(518), 517, 517, TILE_FLIP_HORIZONTAL(TILE_FLIP_VERTICAL(518))};
    bg_rect(&lt_bg, 13, 8, 4, 4, corners);
}


void lt_show(level_t *lvl)
{
    bg_show(&lt_bg);
    bg_move(&lt_bg, 0, 0);
/*     if(!lt_player_mask)
        lt_player_mask = spr_alloc(0, 0, 65);
    spr_set_priority(lt_player_mask, SPR_PRIORITY_LOWEST);
    spr_set_size(lt_player_mask, SPR_SIZE_64x64);
    spr_set_gfx_mode(lt_player_mask, SPR_GFX_ALPHA_BLENDING); */

    blnd_set(BLEND_BOT_BG2 | BLEND_TOP_BG0,
     BLEND_MODE_1);

    blnd_set_weights(31, 3);
}


void lt_hide()
{
    bg_hide(&lt_bg);

    if(lt_player_mask)
        spr_free(lt_player_mask);
    lt_player_mask = NULL;

    // disable blending
    blnd_set(0, 0);
}


/**
 * Creates a light source
 * @param tx absolute tile x
 * @param ty absolute tile y
 * @todo does nothing :'(
 */
void lt_create_source(uint tx, uint ty)
{

}