#include "obj.h"
#include "memory.h"
#include "bg.h"
#include <stdlib.h>

#include "entity.h"
#include "level.h"


void ent_zombie_init(ent_t *e)
{
    e->zombie.health = 10 + (e->level->layer << 3);
    e->zombie.walk = 10;
}

void ent_zombie_update(ent_t *e)
{
    if(e->zombie.walk == 0)
    {
        ent_t *plr = lvl_get_player(e->level);
        int xDist = plr->x - e->x;
        int yDist = plr->y - e->y;
        
        if(abs(xDist) + abs(yDist) < 45) {
            if(xDist) e->zombie.xAccel = (xDist < 0) ? -1 : 1;
            if(yDist) e->zombie.yAccel = (yDist < 0) ? -1 : 1;
        }
    }

    direction_t dir = dir_get(e->zombie.xAccel, e->zombie.yAccel);

    if(!ent_move(e, dir, 1) || (rnd_random() & 0x3F) == 0)
    {
        e->zombie.walk = 60;
        e->zombie.xAccel = rnd_random_bounded(-1, 1);
        e->zombie.yAccel = rnd_random_bounded(-1, 1);
    }

    if(e->zombie.walk)
        e->zombie.walk--;

    ent_apply_knockback(e);

    if(e->zombie.invulernability)
    {
        e->zombie.invulernability--;
        spr_set_pal(e->sprite, e->zombie.invulernability & 0x1);
    }
    
    ent_draw(e);
}


bool ent_zombie_hurt(ent_t *e, ent_t *atker, s8 damage)
{
    if(e->zombie.invulernability)
        return false;
    
    // set knockback
    direction_t atk_dir = atker->dir;
    e->xKnockback = dir_get_x(atk_dir) * 8;
    e->yKnockback = dir_get_y(atk_dir) * 8;

    e->zombie.health -= damage;
    e->zombie.invulernability = 10;

    if(e->zombie.health <= 0) {
        ent_kill(e);
        return true;
    }

    return false;
}