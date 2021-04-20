#include "obj.h"
#include "memory.h"
#include "bg.h"

#include "entity.h"
#include "level.h"


void ent_zombie_init(ent_t *e)
{
    e->zombie.health = 10 + (e->level->layer << 3);
    e->zombie.walk = 10;
}

void ent_zombie_update(ent_t *e)
{
    direction_t dir = dir_get(e->zombie.xAccel, e->zombie.yAccel);

    if(e->zombie.walk == 0)
    {
        ent_t *plr = lvl_get_player(e->level);
        s16 xDist = plr->x - e->x;
        s16 yDist = plr->y - e->y;
        // @todo add player tracking
    }

    if(!ent_move(e, dir) || (rnd_random() & 0x3F) == 0)
    {
        e->zombie.walk = 60;
        e->zombie.xAccel = rnd_random(0, 3) - 1;
        e->zombie.yAccel = rnd_random(0, 3) - 1;
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


void ent_zombie_hurt(ent_t *e, ent_t *atker, s8 damage)
{
    if(e->zombie.invulernability)
        return;
    
    // set knockback
    direction_t atk_dir = atker->dir;
    e->xKnockback = dir_get_x(atk_dir) * 8;
    e->yKnockback = dir_get_y(atk_dir) * 8;

    e->zombie.health -= damage;
    e->zombie.invulernability = 10;

    if(e->zombie.health <= 0)
        ent_kill(e);
}