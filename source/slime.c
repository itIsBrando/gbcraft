#include "entity.h"
#include "level.h"

#include "memory.h"
#include "text.h"

// absolute value add
inline u16 absa(s16 a, s16 b)
{
    return (a < 0 ? -a : a) + (b < 0 ? -b : b);
}

void ent_slime_update(ent_t *s)
{
    slime_t *slime = &s->slime;
    direction_t d = dir_get(slime->xAccel, slime->yAccel);

    if(!ent_move(s, d) || (rnd_random() & 0xF) == 0)
    {
        if(slime->jump_time <= -20)
        {
            slime->xAccel = rnd_random_bounded(0, 3) - 1;
            slime->yAccel = rnd_random_bounded(0, 3) - 1;
            ent_t *plr = lvl_get_player(s->level);
            s16 xDist = plr->x - s->x;
            s16 yDist = plr->y - s->y;

            if(absa(xDist, yDist) < 40)
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

    ent_draw(s);
}