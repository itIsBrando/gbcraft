#include "entity.h"
#include "level.h"
#include "item.h"

#include "random.h"
#include "text.h"

// absolute value add
inline u16 absa(s16 a, s16 b)
{
    return (a < 0 ? -a : a) + (b < 0 ? -b : b);
}


void ent_slime_init(ent_t *e)
{
    e->slime.health = 5 + (e->level->layer << 1);
}


void ent_slime_update(ent_t *s)
{
    slime_t *slime = &s->slime;
    direction_t d = dir_get(slime->xAccel, slime->yAccel);

    ent_apply_knockback(s);
    if(s->slime.invulernability)
    {
        s->slime.invulernability--;
        spr_set_pal(s->sprite, slime->invulernability & 0x1);
        
        ent_draw(s);
        return;
    }

    if(!ent_move(s, d, 1) || (rnd_random() & 0x1F) == 0)
    {
        if(slime->jump_time <= -20)
        {
            slime->xAccel = rnd_random_bounded(0, 3) - 1;
            slime->yAccel = rnd_random_bounded(0, 3) - 1;
            ent_t *plr = lvl_get_player(s->level);
            s16 xDist = plr->x - s->x;
            s16 yDist = plr->y - s->y;

            if(absa(xDist, yDist) < 45)
            {
                if(xDist) slime->xAccel = (xDist < 0) ? -1 : 1;
                if(yDist) slime->yAccel = (yDist < 0) ? -1 : 1;
            }
            
            if(slime->xAccel != 0 || slime->yAccel != 0)
                slime->jump_time = 10;
        }
    }

    // prevent underflow
    if(slime->jump_time > -100)
        slime->jump_time--;

    // stop moving
    if(slime->jump_time == 0)
        slime->xAccel = slime->yAccel = 0;

    if(slime->jump_time > 0)
        spr_set_tile(s->sprite, 57);
    else
        spr_set_tile(s->sprite, 61);

    ent_draw(s);
}


bool ent_slime_hurt(ent_t *e, ent_t *atker, s8 damage)
{
    if(e->slime.invulernability)
        return false;;
    
    // set knockback
    direction_t atk_dir = atker->dir;
    e->xKnockback = dir_get_x(atk_dir) * 8;
    e->yKnockback = dir_get_y(atk_dir) * 8;

    e->slime.health -= damage;
    e->slime.invulernability = 10;

    if(e->slime.health <= 0) {
        // if((rnd_random() & 0x3) == 0)
            ent_item_new(e->level, e->x, e->y, &ITEM_SLIME, 1);

        ent_kill(e);
        return true;
    }

    return false;
}