#include "entity.h"

#include "keypad.h"
#include "bg.h"
#include "obj.h"

void ent_player_update(ent_t *plr)
{
    u16 keys = key_pressed();

    if(keys & KEY_LEFT)
    {
        spr_flip(plr->sprite, SPR_FLIP_NONE);
        bg_move_by(&main_background, DIRECTION_LEFT);
    }
    
    if(keys & KEY_RIGHT)
    {
        spr_flip(plr->sprite, SPR_FLIP_HORIZONTAL);
        bg_move_by(&main_background, DIRECTION_RIGHT);
    }

    if(keys & KEY_UP)
    {
        bg_move_by(&main_background, DIRECTION_UP);
    }

    if(keys & KEY_DOWN)
    {
        bg_move_by(&main_background, DIRECTION_DOWN);
    }


    spr_copy(plr->sprite, 0);
}


static move_all()
{
    
}