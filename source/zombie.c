#include "obj.h"
#include "memory.h"

#include "entity.h"


void ent_zombie_update(ent_t *zombie)
{
    ent_move(zombie, rnd_random() & 0x3);
    
    ent_draw(zombie);
}